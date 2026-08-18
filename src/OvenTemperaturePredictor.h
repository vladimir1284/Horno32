#ifndef OVEN_TEMPERATURE_PREDICTOR_H
#define OVEN_TEMPERATURE_PREDICTOR_H

#include <Arduino.h>

struct PredictionResult {
    bool willReachSetpoint;
    float timeToSetpoint;    // seconds until setpoint is reached
    float steadyStateTemp;   // predicted final temperature
    float timeConstant;      // estimated system time constant
    float fitQuality;        // R-squared value (0-1, higher is better)
    uint16_t samplesUsed;    // number of samples used for prediction
};

class OvenTemperaturePredictor {
public:
    // Constructor with configurable history size (default 50 records)
    explicit OvenTemperaturePredictor(uint16_t maxHistorySize = 50);
    
    // Destructor to free allocated memory
    ~OvenTemperaturePredictor();
    
    // Add a new temperature reading
    void addTemperatureReading(float temperature);
    
    // Predict if and when setpoint will be reached within next 5 minutes
    PredictionResult predictSetpointReach(float setpointTemp);
    
    // Get predicted temperature at specific time in the future
    float predictTemperatureAt(float futureTimeSeconds);
    
    // Clear all temperature history
    void clearHistory();
    
    // Get current number of stored readings
    uint16_t getHistoryCount() const;
    
    // Get maximum history capacity
    uint16_t getMaxHistorySize() const;
    
    // Configuration methods
    void setSamplingPeriod(uint16_t seconds);
    void setMinSamplesRequired(uint8_t samples);
    void setMaxPredictionTime(uint16_t seconds);
    void setConvergenceThreshold(float threshold);
    
    // Get last recorded temperature
    float getLastTemperature() const;
    
    // Check if enough data is available for prediction
    bool hasEnoughData() const;

private:
    // Configuration constants
    static constexpr uint8_t DEFAULT_MIN_SAMPLES = 10;
    static constexpr uint16_t DEFAULT_MAX_PREDICTION_TIME = 300; // 5 minutes
    static constexpr float DEFAULT_CONVERGENCE_THRESHOLD = 0.1f; // °C
    static constexpr uint16_t DEFAULT_SAMPLING_PERIOD = 1; // seconds
    
    // Dynamic memory for temperature history
    float* m_temperatures;
    uint32_t* m_timestamps;     // millis() timestamps
    uint16_t m_maxHistorySize;
    uint16_t m_currentSize;
    uint16_t m_writeIndex;      // Circular buffer write position
    bool m_bufferFull;
    
    // Configuration variables
    uint16_t m_samplingPeriod;
    uint8_t m_minSamplesRequired;
    uint16_t m_maxPredictionTime;
    float m_convergenceThreshold;
    
    // Internal computation structures
    struct ModelParams {
        float steadyStateTemp;
        float initialTemp;
        float timeConstant;
        float fitQuality;
        bool valid;
    };
    
    // Internal methods
    ModelParams fitExponentialModel() const;
    float predictTemperature(const ModelParams& params, float futureTime) const;
    void getLinearizedData(float steadyStateTemp, float* xData, float* yData, uint16_t& dataCount) const;
    void performLinearRegression(const float* xData, const float* yData, uint16_t dataCount, 
                                float& slope, float& intercept, float& rSquared) const;
    float estimateSteadyStateTemp() const;
    uint16_t getEffectiveDataCount() const;
    void getTimeVector(float* timeVector) const;
    
    // Memory management helpers
    bool allocateMemory();
    void freeMemory();
};

#endif // OVEN_TEMPERATURE_PREDICTOR_H