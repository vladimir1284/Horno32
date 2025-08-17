#include "OvenTemperaturePredictor.h"
#include <math.h>

OvenTemperaturePredictor::OvenTemperaturePredictor(uint16_t maxHistorySize) 
    : m_temperatures(nullptr)
    , m_timestamps(nullptr)
    , m_maxHistorySize(maxHistorySize)
    , m_currentSize(0)
    , m_writeIndex(0)
    , m_bufferFull(false)
    , m_samplingPeriod(DEFAULT_SAMPLING_PERIOD)
    , m_minSamplesRequired(DEFAULT_MIN_SAMPLES)
    , m_maxPredictionTime(DEFAULT_MAX_PREDICTION_TIME)
    , m_convergenceThreshold(DEFAULT_CONVERGENCE_THRESHOLD) {
    
    allocateMemory();
}

OvenTemperaturePredictor::~OvenTemperaturePredictor() {
    freeMemory();
}

bool OvenTemperaturePredictor::allocateMemory() {
    freeMemory(); // Ensure clean state
    
    m_temperatures = (float*)malloc(m_maxHistorySize * sizeof(float));
    m_timestamps = (uint32_t*)malloc(m_maxHistorySize * sizeof(uint32_t));
    
    if (!m_temperatures || !m_timestamps) {
        freeMemory();
        Serial.println("ERROR: Failed to allocate memory for temperature history");
        return false;
    }
    
    return true;
}

void OvenTemperaturePredictor::freeMemory() {
    if (m_temperatures) {
        free(m_temperatures);
        m_temperatures = nullptr;
    }
    if (m_timestamps) {
        free(m_timestamps);
        m_timestamps = nullptr;
    }
}

void OvenTemperaturePredictor::addTemperatureReading(float temperature) {
    if (!m_temperatures || !m_timestamps) {
        return; // Memory allocation failed
    }
    
    uint32_t currentTime = millis();
    
    m_temperatures[m_writeIndex] = temperature;
    m_timestamps[m_writeIndex] = currentTime;
    
    m_writeIndex = (m_writeIndex + 1) % m_maxHistorySize;
    
    if (m_currentSize < m_maxHistorySize) {
        m_currentSize++;
    } else {
        m_bufferFull = true;
    }
}

PredictionResult OvenTemperaturePredictor::predictSetpointReach(float setpointTemp) {
    PredictionResult result = {false, -1.0f, 0.0f, 0.0f, 0.0f, 0};
    
    if (!hasEnoughData()) {
        return result;
    }
    
    // Fit exponential model to historical data
    ModelParams params = fitExponentialModel();
    
    if (!params.valid || params.fitQuality < 0.3f) {
        return result; // Poor fit, prediction unreliable
    }
    
    result.steadyStateTemp = params.steadyStateTemp;
    result.timeConstant = params.timeConstant;
    result.fitQuality = params.fitQuality;
    result.samplesUsed = getEffectiveDataCount();
    
    float currentTemp = getLastTemperature();
    
    // Check if we're approaching setpoint from the right direction
    bool heatingUp = setpointTemp > currentTemp;
    bool coolingDown = setpointTemp < currentTemp;
    
    if (heatingUp && params.steadyStateTemp <= currentTemp + m_convergenceThreshold) {
        return result; // Can't reach higher setpoint
    }
    
    if (coolingDown && params.steadyStateTemp >= currentTemp - m_convergenceThreshold) {
        return result; // Can't reach lower setpoint
    }
    
    // Solve for time when T(t) = setpoint
    float tempDiffCurrent = currentTemp - params.steadyStateTemp;
    float tempDiffSetpoint = setpointTemp - params.steadyStateTemp;
    
    if (fabs(tempDiffCurrent) < m_convergenceThreshold) {
        result.willReachSetpoint = true;
        result.timeToSetpoint = 0.0f;
        return result;
    }
    
    if (tempDiffCurrent * tempDiffSetpoint <= 0) {
        return result; // Setpoint is on wrong side of steady state
    }
    
    float ratio = tempDiffSetpoint / tempDiffCurrent;
    if (ratio <= 0 || ratio >= 1.0f) {
        return result; // Invalid mathematical condition
    }
    
    float timeToReach = -params.timeConstant * logf(ratio);
    
    // Check if setpoint will be reached within configured prediction time
    if (timeToReach > 0 && timeToReach <= m_maxPredictionTime) {
        result.willReachSetpoint = true;
        result.timeToSetpoint = timeToReach;
    }
    
    return result;
}

float OvenTemperaturePredictor::predictTemperatureAt(float futureTimeSeconds) {
    if (!hasEnoughData()) {
        return getLastTemperature();
    }
    
    ModelParams params = fitExponentialModel();
    
    if (!params.valid || params.fitQuality < 0.3f) {
        return getLastTemperature(); // Poor fit, return current temperature
    }
    
    return predictTemperature(params, futureTimeSeconds);
}

OvenTemperaturePredictor::ModelParams OvenTemperaturePredictor::fitExponentialModel() const {
    ModelParams params = {0.0f, 0.0f, 0.0f, 0.0f, false};
    
    uint16_t dataCount = getEffectiveDataCount();
    if (dataCount < m_minSamplesRequired) {
        return params;
    }
    
    // Estimate steady-state temperature using recent samples
    float steadyStateTemp = estimateSteadyStateTemp();
    
    // Get initial temperature (oldest sample in circular buffer)
    uint16_t startIndex = m_bufferFull ? m_writeIndex : 0;
    float initialTemp = m_temperatures[startIndex];
    
    // If temperature is not changing significantly, assume near steady state
    if (fabs(steadyStateTemp - initialTemp) < 1.0f) {
        params.steadyStateTemp = steadyStateTemp;
        params.initialTemp = initialTemp;
        params.timeConstant = 1000.0f; // Large time constant = slow response
        params.fitQuality = 0.5f;
        params.valid = true;
        return params;
    }
    
    // Prepare data for linearized fitting
    float* xData = (float*)malloc(dataCount * sizeof(float));
    float* yData = (float*)malloc(dataCount * sizeof(float));
    
    if (!xData || !yData) {
        if (xData) free(xData);
        if (yData) free(yData);
        return params;
    }
    
    uint16_t validPoints = 0;
    getLinearizedData(steadyStateTemp, xData, yData, validPoints);
    
    if (validPoints < 3) {
        free(xData);
        free(yData);
        params.steadyStateTemp = steadyStateTemp;
        params.initialTemp = initialTemp;
        params.timeConstant = 100.0f; // Default time constant
        params.fitQuality = 0.1f;
        params.valid = true;
        return params;
    }
    
    // Perform linear regression
    float slope, intercept, rSquared;
    performLinearRegression(xData, yData, validPoints, slope, intercept, rSquared);
    
    free(xData);
    free(yData);
    
    // Convert slope to time constant: slope = -1/tau
    float timeConstant = (slope != 0) ? -1.0f / slope : 100.0f;
    
    // Ensure reasonable time constant bounds
    timeConstant = constrain(timeConstant, 1.0f, 3600.0f);
    
    params.steadyStateTemp = steadyStateTemp;
    params.initialTemp = initialTemp;
    params.timeConstant = timeConstant;
    params.fitQuality = max(0.0f, min(rSquared, 1.0f));
    params.valid = true;
    
    return params;
}

void OvenTemperaturePredictor::getLinearizedData(float steadyStateTemp, float* xData, float* yData, uint16_t& dataCount) const {
    dataCount = 0;
    uint16_t effectiveCount = getEffectiveDataCount();
    uint16_t startIndex = m_bufferFull ? m_writeIndex : 0;
    uint32_t baseTime = m_timestamps[startIndex];
    
    for (uint16_t i = 0; i < effectiveCount; i++) {
        uint16_t index = (startIndex + i) % m_maxHistorySize;
        float tempDiff = steadyStateTemp - m_temperatures[index];
        
        if (fabs(tempDiff) > 0.1f) { // Avoid log(0)
            xData[dataCount] = (m_timestamps[index] - baseTime) / 1000.0f; // Convert to seconds
            yData[dataCount] = logf(fabs(tempDiff));
            dataCount++;
        }
    }
}

void OvenTemperaturePredictor::performLinearRegression(const float* xData, const float* yData, uint16_t dataCount,
                                                      float& slope, float& intercept, float& rSquared) const {
    // Calculate sums
    float sumX = 0, sumY = 0, sumXY = 0, sumXX = 0, sumYY = 0;
    
    for (uint16_t i = 0; i < dataCount; i++) {
        sumX += xData[i];
        sumY += yData[i];
        sumXY += xData[i] * yData[i];
        sumXX += xData[i] * xData[i];
        sumYY += yData[i] * yData[i];
    }
    
    float n = dataCount;
    float denominator = n * sumXX - sumX * sumX;
    
    if (fabs(denominator) < 1e-6f) {
        slope = 0;
        intercept = sumY / n;
        rSquared = 0;
        return;
    }
    
    // Calculate slope and intercept
    slope = (n * sumXY - sumX * sumY) / denominator;
    intercept = (sumY - slope * sumX) / n;
    
    // Calculate R-squared
    float meanY = sumY / n;
    float ssRes = 0, ssTot = 0;
    
    for (uint16_t i = 0; i < dataCount; i++) {
        float predicted = intercept + slope * xData[i];
        ssRes += (yData[i] - predicted) * (yData[i] - predicted);
        ssTot += (yData[i] - meanY) * (yData[i] - meanY);
    }
    
    rSquared = (ssTot > 0) ? 1.0f - ssRes / ssTot : 0.0f;
}

float OvenTemperaturePredictor::estimateSteadyStateTemp() const {
    uint16_t effectiveCount = getEffectiveDataCount();
    uint16_t recentSamples = min(effectiveCount, (uint16_t)20);
    
    if (recentSamples == 0) return 0.0f;
    
    float sum = 0;
    uint16_t startIndex = m_bufferFull ? 
        (m_writeIndex + m_maxHistorySize - recentSamples) % m_maxHistorySize :
        max(0, (int)(m_currentSize - recentSamples));
    
    for (uint16_t i = 0; i < recentSamples; i++) {
        uint16_t index = (startIndex + i) % m_maxHistorySize;
        sum += m_temperatures[index];
    }
    
    return sum / recentSamples;
}

float OvenTemperaturePredictor::predictTemperature(const ModelParams& params, float futureTime) const {
    return params.steadyStateTemp - 
           (params.steadyStateTemp - params.initialTemp) * 
           expf(-futureTime / params.timeConstant);
}

void OvenTemperaturePredictor::getTimeVector(float* timeVector) const {
    uint16_t effectiveCount = getEffectiveDataCount();
    uint16_t startIndex = m_bufferFull ? m_writeIndex : 0;
    uint32_t baseTime = m_timestamps[startIndex];
    
    for (uint16_t i = 0; i < effectiveCount; i++) {
        uint16_t index = (startIndex + i) % m_maxHistorySize;
        timeVector[i] = (m_timestamps[index] - baseTime) / 1000.0f; // Convert to seconds
    }
}

uint16_t OvenTemperaturePredictor::getEffectiveDataCount() const {
    return m_bufferFull ? m_maxHistorySize : m_currentSize;
}

void OvenTemperaturePredictor::clearHistory() {
    m_currentSize = 0;
    m_writeIndex = 0;
    m_bufferFull = false;
}

uint16_t OvenTemperaturePredictor::getHistoryCount() const {
    return getEffectiveDataCount();
}

uint16_t OvenTemperaturePredictor::getMaxHistorySize() const {
    return m_maxHistorySize;
}

void OvenTemperaturePredictor::setSamplingPeriod(uint16_t seconds) {
    m_samplingPeriod = max((uint16_t)1, seconds);
}

void OvenTemperaturePredictor::setMinSamplesRequired(uint8_t samples) {
    m_minSamplesRequired = max((uint8_t)3, samples);
}

void OvenTemperaturePredictor::setMaxPredictionTime(uint16_t seconds) {
    m_maxPredictionTime = seconds;
}

void OvenTemperaturePredictor::setConvergenceThreshold(float threshold) {
    m_convergenceThreshold = max(0.01f, threshold);
}

float OvenTemperaturePredictor::getLastTemperature() const {
    if (m_currentSize == 0) return 0.0f;
    
    uint16_t lastIndex = (m_writeIndex + m_maxHistorySize - 1) % m_maxHistorySize;
    return m_temperatures[lastIndex];
}

bool OvenTemperaturePredictor::hasEnoughData() const {
    return getEffectiveDataCount() >= m_minSamplesRequired && m_temperatures && m_timestamps;
}

// Example usage for ESP32 main code
/*
#include "OvenTemperaturePredictor.h"

OvenTemperaturePredictor predictor(50); // 50 sample history
float setpoint = 200.0;

void setup() {
    Serial.begin(115200);
    
    // Configure predictor if needed
    predictor.setSamplingPeriod(1);  // 1 second sampling
    predictor.setMinSamplesRequired(10);
    predictor.setMaxPredictionTime(300); // 5 minutes
}

void loop() {
    // Read temperature from sensor (replace with actual sensor code)
    float currentTemp = readTemperatureSensor();
    
    // Add reading to predictor
    predictor.addTemperatureReading(currentTemp);
    
    // Check if we have enough data for prediction
    if (predictor.hasEnoughData()) {
        PredictionResult result = predictor.predictSetpointReach(setpoint);
        
        if (result.willReachSetpoint) {
            int minutes = (int)(result.timeToSetpoint / 60);
            int seconds = (int)result.timeToSetpoint % 60;
            
            Serial.printf("Setpoint will be reached in %d:%02d\n", minutes, seconds);
            Serial.printf("Steady-state temp: %.1f°C\n", result.steadyStateTemp);
            Serial.printf("Time constant: %.1fs\n", result.timeConstant);
            Serial.printf("Fit quality: %.2f\n", result.fitQuality);
        } else {
            Serial.println("Setpoint will not be reached within 5 minutes");
            
            // Show prediction for next 2 minutes anyway
            float futureTemp = predictor.predictTemperatureAt(120);
            Serial.printf("Predicted temp in 2 min: %.1f°C\n", futureTemp);
        }
    } else {
        Serial.printf("Collecting data... (%d/%d samples)\n", 
                     predictor.getHistoryCount(), 
                     predictor.getMinSamplesRequired());
    }
    
    delay(1000); // Sample every second
}

float readTemperatureSensor() {
    // Replace with actual sensor reading code
    // Example for thermocouple, RTD, or thermistor
    return 25.0; // Placeholder
}
*/