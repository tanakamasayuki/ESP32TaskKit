# ESP32TaskKit — Specification

ESP32TaskKit is an **"intermediate-friendly task management library"** for ESP32 (Arduino).

- For getting started: **ESP32AutoTask**  
  → Just define weak declarations `LoopCoreX_YYY` and tasks are auto-generated—training wheels.  
  https://github.com/tanakamasayuki/ESP32AutoTask

- For synchronization: **ESP32AutoSync**  
  → C++ wrappers for FreeRTOS sync primitives such as Queue / Notify / Semaphore / Mutex.  
  https://github.com/tanakamasayuki/ESP32AutoSync

- In between: **ESP32TaskKit**  
  → A kit that makes FreeRTOS tasks (`xTaskCreatePinnedToCore`, etc.) easier to manage with C++ classes and config objects.  
  https://github.com/tanakamasayuki/ESP32TaskKit

### When to use ESP32AutoTask vs. ESP32TaskKit
- Strengths of ESP32AutoTask: writing weak declarations `LoopCoreX_YYY` automatically generates tasks; no need for config or class design. Stack/priority can be configured. Lowest learning cost—great for "just make it run".
- Limits of ESP32AutoTask: runs with a predetermined number of task slots per core, so not ideal for many fine-grained tasks. Cannot pin to cores or control lifecycle in detail.
- Strengths of ESP32TaskKit: explicitly set stack/priority/core via Config; manage lifecycle via `start`/`startLoop`/`requestStop`; write in C++ style with lambdas and functors.
- Limits of ESP32TaskKit: less automation than AutoTask, so you write a bit more up front. If you need special FreeRTOS operations, you are expected to call the raw APIs.
- Which to choose: use AutoTask when you want something running quickly. Use TaskKit for production-leaning code where you want flexible task counts and clean config/lifecycle. For special needs, drop to the raw FreeRTOS API.

### When to use ESP32AutoSync
- Completely independent library with no mutual dependency; both can call raw FreeRTOS APIs.
- If you only need common Queue/Notify/Semaphore/Mutex, using AutoSync alongside is convenient. For special synchronization needs, call FreeRTOS directly.

---

## 1. Concept

The main goals of ESP32TaskKit are:

1. **Reduce the burden of “hand-written” FreeRTOS tasks**
2. **Write tasks in a C++-friendly way (lambdas, functors, RAII)**
3. **Separate responsibilities of tasks and synchronization (AutoSync)**
4. **Encourage idiomatic FreeRTOS usage (cover normal needs here; use raw FreeRTOS for special cases)**

---

## 2. Core ideas and design policies

### 2.1 Manage tasks as classes
### 2.2 Configure everything via Config
### 2.3 Two start paths: start and startLoop
### 2.4 Leave synchronization to AutoSync

---

## 3. Namespaces and file layout

```text
ESP32TaskKit/
  src/
    ESP32TaskKit.h
```

---

## 4. TaskConfig

```cpp
struct TaskConfig {
    const char*  name      = "";
    uint32_t     stackSize = ARDUINO_LOOP_STACK_SIZE; // 8192 words
    UBaseType_t  priority  = 2;
    int          core      = tskNO_AFFINITY;
};
```

Notes:
- `stackSize` is in words (`configSTACK_DEPTH_TYPE`). Default is `ARDUINO_LOOP_STACK_SIZE` (8192 words).
- `core` is numeric for clarity: 0 = `PRO_CPU_NUM`, 1 = `APP_CPU_NUM` (ESP32 naming).
- `core=tskNO_AFFINITY` means no core pinning—leave it to the scheduler. Specify 0/1 if you want to pin.
- `ARDUINO_RUNNING_CORE` is the core number Arduino `loop()` runs on (often 1). Use it if you want to run on the same core.
- `priority` is FreeRTOS 0 (Idle) to `configMAX_PRIORITIES-1`. Arduino `loop()` is usually 1, so default 2. Use 2+ for tasks needing more real-time behavior.
- `name` is the FreeRTOS task name (up to `configMAX_TASK_NAME_LEN`). If empty, an auto-incremented name like `TaskKit#1` is used. In Arduino this mainly shows up via `vTaskList` or a debugger rather than Serial logs.
- Core validation: on dual core, only 0/1/tskNO_AFFINITY are allowed. On single core (`CONFIG_FREERTOS_UNICORE` etc.), only 0/tskNO_AFFINITY are allowed.

---

## 5. Task class

```cpp
class Task {
public:
    Task() noexcept;
    ~Task();

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    bool start(TaskFunction_t fn,
               void* arg = nullptr,
               const TaskConfig& cfg = TaskConfig{});

    template<class F>
    bool startLoop(F&& loopFunc,
                   const TaskConfig& cfg = TaskConfig{},
                   uint32_t periodMs = 1);

    bool isRunning() const noexcept;
    TaskHandle_t handle() const noexcept;

    void requestStop() noexcept;
    bool isStopRequested() const noexcept;

private:
    TaskHandle_t _handle;
    bool _running;
    bool _stopRequested;
};
```

### Notes on startLoop
- The loop ends when `loopFunc` returns `false`.
- If `periodMs > 0`, it runs at fixed intervals via `vTaskDelayUntil` (default 1 ms avoids WDT and CPU hogging).
- If `periodMs == 0`, it runs without delay. This is discouraged—call `delay()` etc. inside the loop to avoid WDT.

### Lifecycle and termination
- States: simple `Idle -> Running -> Idle`. Only on successful `start` does it go to `Running`; on finish/failure it stays/returns to `Idle`.
- Reuse: after finishing, you can `start` again on the same `Task` instance (it resets `_handle=null`, `_running=false`).
- Termination triggers: `startLoop` ends when it returns `false`. `requestStop()` just sets a flag; check `isStopRequested()` inside your loop and `return false;`. C-style tasks (`start`) should check their own stop flag. `startLoop` wrapper also checks `isStopRequested()` every iteration and stops automatically.
- Waiting for exit: no `join`; users poll `isRunning()` (insert `delay()` while polling to avoid WDT/CPU hogging).
- Destructor: does not force-stop. Users should confirm the task is stopped before destruction (destroying while running should warn via log/assert).
- On error: if `start` fails, state stays `Idle` and no handle is held.

### Thread-safety policy
- The library is generally not thread-safe. Management operations like `start` are expected from a single owner task.
- Exceptions: `requestStop()`/`isStopRequested()`/`isRunning()` may be read from other tasks as minimal cross-calls.
- ISR calls are not allowed. For cross-task control, use FreeRTOS notifications or AutoSync (Queue/Notify/Semaphore) for thread-safe messaging.
- You may call FreeRTOS APIs with the `TaskHandle_t` from `handle()`, but it can diverge from internal state (e.g., external `vTaskDelete` leaves flags stale), so it is discouraged.
- Internal flags (`_running`/`_stopRequested`) are plain `bool` for Arduino. Cross-calls are best-effort; add synchronization yourself if you need strict exclusion.

### Error handling / logging
- `start`/`startLoop` return bool success/failure. On failure, stay `Idle`. Typical failures: invalid `priority`/`core`, out of memory (stack alloc), `xTaskCreate` errors.
- Logging is always on, using ESP-IDF `ESP_LOGE/W/I/D/V` macros that work on Arduino.
- Typical log levels:
  - `ESP_LOGE`: `start failed: invalid priority=%u core=%d`, `start failed: xTaskCreate err=%d`, `startLoop failed: alloc functor`
  - `ESP_LOGW`: `start called while running`, `destroying running task`
  - `ESP_LOGI/D`: state transitions such as task start/end (as needed)
- Validation: "fail on out-of-range" rather than clamping. `priority` must be 0–`configMAX_PRIORITIES-1`; `core` outside allowed range makes `start` fail with a log.
- A lightweight API to get failure reasons (e.g., `getLastError()` / `TaskResult`) may be added later. For now, rely on logs and return values.

### Lambdas/functors: memory and exceptions
- `startLoop` stores lambdas/functors in `std::function<bool()>` (favoring brevity). This allocates on the heap and can fail if memory is low.
- Exceptions are unsupported (Arduino). Do not throw from task bodies; if thrown, they should terminate (`std::terminate`) or be caught internally to end the task.
- Prefer lambdas with small captures or function pointers. SBO/allocator tweaks may be considered later.

※ This library targets Arduino environments. ESP-IDF standalone is out of scope.

---

## 6. Examples

### 6.1 C-style task

```cpp
TaskKit::Task worker;

void WorkerTask(void* pv) {
    for (;;) {
        Serial.println("Working...");
        delay(100);
    }
}

void setup() {
    TaskKit::TaskConfig cfg;
    cfg.name = "Worker";
    cfg.priority = 2;

    worker.start(WorkerTask, nullptr, cfg);
}
```

---

### 6.2 Lambda task

```cpp
TaskKit::Task worker;

worker.startLoop(
    [] {
        Serial.println("Hello");
        delay(100);
        return true;
    }
);
```

---

### 6.3 Periodic task

```cpp
periodic.startLoop(
    [] {
        readSensor();
        return true;
    },
    TaskKit::TaskConfig{},
    5000
);
```

---

### 6.4 With AutoSync

```cpp
TaskKit::Task consumer;
AutoSync::Queue<int> q(8);

consumer.startLoop(
    [] {
        int v;
        if (q.receive(v)) {
            Serial.println(v);
        }
        return true;
    }
);
```

---

### Comment policy for samples
- Samples include both English and Japanese.
- When adding comments on their own lines, separate by language per line (e.g., `// en: ...` then next line `// ja: ...`).
- When adding end-of-line comments, keep them on one line (e.g., `// en: ... / ja: ...`).

---

## 7. Additional design policies

- Cooperative stop via requestStop
- Do not force-stop in the destructor
- Do not call from ISR (use AutoSync for synchronization)
- Suspend/resume are not provided (use requestStop and cooperative exit instead)

---

## 8. Future work

- TaskGroup
- join() / await()
- Runtime statistics
- Cancellation mechanism

---

## 9. License

(MIT, etc.)
