# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Firmware + web UI for an ESP32-based oven ("Horno") controller: thermocouple temperature reading (MAX6675 + median filter), HT1621 LCD display, and a SvelteKit web interface served from the ESP32. Built on top of the [ESP32-SvelteKit](https://github.com/theelims/ESP32-sveltekit) framework (`lib/framework/`), forked and renamed from that project's "Light" demo to "Horno".

## Commands

### Firmware (PlatformIO, root of repo)

- `pio run` — build default env (`esp32dev`)
- `pio run -e <env>` — build a specific env: `esp32dev`, `esp32-c3-devkitm-1`, `esp32-s3-devkitc-1`, `adafruit_feather_esp32s3_nopsram`
- `pio run -t upload` — build and flash
- `pio device monitor` — serial monitor (115200 baud, `esp32_exception_decoder` filter enabled)
- `pio run -t uploadfs` — build/upload the LittleFS filesystem image (only relevant if not using `EMBED_WWW`)

No PlatformIO unit tests exist in this repo (no `test/` dir).

Building the firmware automatically triggers the frontend build via a pre-build script (`scripts/build_interface.py`) — no need to build the interface manually first. That script detects the JS package manager from whichever lockfile is present (`package-lock.json` here → npm) and gzips/embeds or copies the Vite output depending on the `EMBED_WWW` build flag.

### Frontend (`interface/`)

- `npm run dev` — Vite dev server
- `npm run build` — production build
- `npm run check` — `svelte-kit sync` + `svelte-check` (TypeScript/Svelte type checking)
- `npm run lint` — prettier check + eslint
- `npm run format` — prettier write

No JS test suite is configured (no test script in `package.json`).

### Docs

`mkdocs.yml` at repo root; deployed to GitHub Pages by `.github/workflows/ci.yaml` on push to `main`/`master` via `mkdocs gh-deploy`.

## Architecture

### Framework vs. app code

- `lib/framework/` — the reusable ESP32-SvelteKit backend framework (WiFi/AP management, security/JWT auth, MQTT client, NTP, OTA, sleep, battery, etc). Treat as vendored/upstream; app-specific changes belong in `src/`.
- `src/` — this project's app code (compiled by PlatformIO's default `src_dir`). `src/main.cpp` wires everything together.
- `interface/` — the SvelteKit frontend, structured to mirror backend feature areas (`routes/connections`, `routes/system`, `routes/user`, `routes/wifi`, `routes/demo`).

### StatefulService pattern (core abstraction, from the framework)

Each piece of app state is a plain data class with static `read(state, JsonObject&)` / `update(JsonObject&, state)` (de)serializers, managed by a `StatefulService<T>` subclass. That service can then be wired to any combination of:
- `HttpEndpoint<T>` — REST GET/POST
- `EventEndpoint<T>` — pub/sub over the single Event WebSocket (MessagePack-encoded)
- `WebSocketServer<T>` — dedicated WebSocket that pushes on every change
- `MqttEndpoint<T>` — pub/sub over a pair of MQTT topics
- `FSPersistence<T>` — auto-persists state to a JSON file on LittleFS

Updates return `StateUpdateResult::CHANGED` / `UNCHANGED` / `ERROR`; `CHANGED` triggers registered update handlers (each carrying an `originId` like `http`, `mqtt`, `websocketserver:{clientId}`) and propagation to other endpoints. See `docs/statefulservice.md` for full API and examples before adding a new stateful feature.

### App-specific services (`src/`)

- `HornoStateService` — runtime state: `ledOn` (placeholder for oven on/off), `temperature`. REST `/rest/hornoState`, WS `/ws/hornoState`, event `led`. Includes Home Assistant-style `homeAssistRead`/`homeAssistUpdate` serializers for MQTT, but the actual `MqttEndpoint`/MQTT client wiring in the constructor is currently commented out (MQTT feature is disabled, see below).
- `HornoSettingsService` — PID tuning config (`maxPowerUp/Down`, aggressive/conservative Kp/Ki/Kd), persisted to `/config/hornoSettings.json`, REST `/rest/hornoSettings`.
- `HornoMqttSettingsService` — MQTT topic/name/unique-id config, persisted to `/config/brokerSettings.json`, REST `/rest/brokerSettings`.
- `HT1621_custom` — driver for the HT1621 LCD segment display.
- `median_filter` — smooths raw MAX6675 thermocouple readings.

`main.cpp` runs two FreeRTOS tasks pinned to core 1: `readTemp` (samples the thermocouple into the median filter every 250ms) and `updateScreen` (pushes the filtered value to the LCD and into `HornoStateService` every 500ms). Networking/framework tasks are pinned to core 0 via the `ESP32SVELTEKIT_RUNNING_CORE=0` build flag, keeping WiFi/HTTP off the sensor/display loop.

### Feature flags and build config

- `features.ini` toggles framework features at compile time. In this repo **all optional features are currently disabled** (`FT_SECURITY=0`, `FT_MQTT=0`, `FT_NTP=0`, `FT_UPLOAD_FIRMWARE=0`, `FT_DOWNLOAD_FIRMWARE=0`, `FT_SLEEP=0`, `FT_BATTERY=0`, `FT_ANALYTICS=0`, `FT_COREDUMP=0`) — notably `FT_SECURITY=0` means no auth is enforced on any endpoint right now.
- `factory_settings.ini` provides default values (WiFi credentials, AP SSID/password, user credentials, timezone, etc.) used when no persisted settings exist on the filesystem.
- `platformio.ini` `[env]` section applies to all boards; per-env sections (`esp32dev`, etc.) override pins/partitions. `EMBED_WWW` is enabled, so the frontend ships embedded in the firmware binary rather than a separate LittleFS image.
