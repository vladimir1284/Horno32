#include <HornoStateService.h>

HornoStateService::HornoStateService(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit) : _httpEndpoint(HornoState::read,
                                                                                HornoState::update,
                                                                                this,
                                                                                server,
                                                                                HORNO_SETTINGS_ENDPOINT_PATH,
                                                                                sveltekit->getSecurityManager(),
                                                                                AuthenticationPredicates::IS_AUTHENTICATED)
{
  // configure led to be output
  pinMode(LED_PIN, OUTPUT);

  // configure settings service update handler to update LED state
  addUpdateHandler([&](const String &originId)
                   { onConfigUpdated(); }, false);
}

void HornoStateService::begin()
{
  _httpEndpoint.begin();
  _state.on = DEFAULT_LED_STATE;
  _state.temperature = 0;
  onConfigUpdated();
}

void HornoStateService::updateTemp(float newTemperature){
  _state.temperature = newTemperature;
}

void HornoStateService::onConfigUpdated()
{
  digitalWrite(LED_PIN, _state.on ? LED_ON : LED_OFF);
}
