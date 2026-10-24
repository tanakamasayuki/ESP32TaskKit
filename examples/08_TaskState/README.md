# 08_TaskState

> 日本語版: [README.ja.md](README.ja.md)

Observe a worker state and stack headroom.

## Prepare and run

1. Check the [example guide](../../docs/GUIDE_EXAMPLES.md) for setup and hardware. Use the root README for supported boards and dependencies.
2. Open [08_TaskState.ino](08_TaskState.ino) and select the actual board/port. Prepare the dependencies used by these includes:

   `<ESP32TaskKit.h>`, `<freertos/FreeRTOS.h>`, `<freertos/task.h>`

3. Upload and open the Serial monitor at 115200 baud. Check startup messages and input conditions in the source.

## Follow the sketch

These APIs are listed in their first source order. Registering a callback does not execute its body at that point.

1. `worker.startLoop(...)` — Start a repeating callback; true continues and false finishes. Check creation success and keep referenced context alive.
2. `worker.handle(...)` — Obtain a live task handle only for observation; do not retain it after the worker exits.
3. `worker.isRunning(...)` — Observe the library-managed worker state rather than assuming start or stop requests completed.

## API usage and precautions

Keep `Task` objects and C-style argument storage alive until their workers exit. Check `start()` / `startLoop()` results when adapting the examples; a missing log may be a creation failure, not a timing problem. A loop callback returns true to continue and false to finish.

`requestStop()` requests cooperative termination: it does not immediately interrupt a long callback, and a C-style function must inspect the stop flag itself. Periods passed to `startLoop()` are milliseconds; raw FreeRTOS delays use ticks. On ESP32, task stack sizes and stack high-water values are bytes, even where an older sample output label says words.

Diagnostic features are conditional on the core configuration: unavailable statistics are not necessarily a failed worker. Prefer the root API documentation over duplicating default sizes or build-option tables.

## Inputs and observable output

These are selected output formats from the source. printf specifiers are replaced by runtime values; inspect the surrounding conditions rather than assuming these lines always occur in this order.

```text
[+%lu ms] worker alive
---- Task Info at +%lu ms ----
worker handle is null (stopped?)
```

## Adapt this particular example

Inspect a snapshot rather than assuming the worker stays in the displayed state. Check for a null handle before raw FreeRTOS inspection and do not use it after task exit.

## Verify and troubleshoot

Observe a worker state and stack headroom and verify the result on its output. First use the supplied input, then change one configuration or data source at a time.

| Symptom | First check |
|---|---|
| No output | Board/dependencies, initialization results, output destination and Serial baud |
| Starts but stops making progress | Wait conditions, buffer space, peer/input readiness and service calls |
| Unexpected values | Lengths/units, format, data lifetime, shared state and partial results |

See the [guide](../../docs/GUIDE_EXAMPLES.md) for shared principles and the [example index](../README.md) for the next sketch.
