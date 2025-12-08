# ESP32TaskKit

[日本語版 READMEはこちら](README.ja.md)

Lightweight helpers for running FreeRTOS tasks from Arduino on ESP32.

## Highlights
- Start tasks with either a C-style function (`start`) or a C++ functor/lambda loop (`startLoop`)
- Cooperative stop via `requestStop()` and `isStopRequested()`
- Configure stack size, priority, and CPU core per task
- Auto-generated task names when none are provided
- Minimal logging with ESP-IDF `ESP_LOG*` macros

## Requirements
- Arduino core for ESP32 3.x
- C++17 capable toolchain (as provided by Arduino-ESP32)

## Quick Start
```cpp
#include <ESP32TaskKit.h>

TaskKit::Task worker;

void setup() {
  Serial.begin(115200);

  // Loop task example
  worker.startLoop([] {
    Serial.println("working...");
    delay(100);
    return true;                  // continue
  }, TaskKit::TaskConfig{}, 500); // run every 500 ms
}

void loop() {
  // Request stop after 5 seconds
  static uint32_t start = millis();
  if (worker.isRunning() && millis() - start > 5000) {
    worker.requestStop();
  }
  delay(200);
}
```

### C-style task with cooperative stop
```cpp
TaskKit::Task worker;

void WorkerTask(void *pv) {
  for (;;) {
    if (worker.isStopRequested()) {
      break;  // exit when stop requested
    }
    // do work
    delay(200);
  }
}

void setup() {
  worker.start(&WorkerTask);
}
```

## Examples
- `examples/01_BasicLoop` — simplest loop task
- `examples/02_CStyleTask` — basic C-style task
- `examples/03_RequestStop` — requestStop with startLoop
- `examples/04_RequestStopCStyle` — requestStop with C-style task
- `examples/05_TwoTasks` — two loop tasks
- `examples/06_CustomName` — custom task names
- `examples/07_InlineConfig` — inline TaskConfig usage
- `examples/08_TaskState` — query FreeRTOS task state/info from loop
- `examples/09_TaskList` — dump task list via `vTaskList`
- `examples/10_TaskStatusArray` — inspect tasks via `pxGetTaskStatusArray`/`uxTaskGetSystemState`
- `examples/11_RunTimeStats` — show runtime statistics via `vTaskGetRunTimeStats`
- `examples/12_TaskSystemState` — enumerate tasks via `uxTaskGetSystemState`

## Notes
- `isRunning()` reports the library-managed state (not directly `eTaskGetState`).
- Cooperative stop: `requestStop()` only sets a flag; user code should exit loops when `isStopRequested()` becomes true.
- This library targets Arduino for ESP32 and is not intended for ESP-IDF standalone use.

## License
See `LICENSE`.
