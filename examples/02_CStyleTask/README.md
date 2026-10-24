# 02_CStyleTask

> 日本語版: [README.ja.md](README.ja.md)

Start a C-style FreeRTOS worker function.

## Prepare and run

1. Check the [example guide](../../docs/GUIDE_EXAMPLES.md) for setup and hardware. Use the root README for supported boards and dependencies.
2. Open [02_CStyleTask.ino](02_CStyleTask.ino) and select the actual board/port. Prepare the dependencies used by these includes:

   `<ESP32TaskKit.h>`

3. Upload and open the Serial monitor at 115200 baud. Check startup messages and input conditions in the source.

## Follow the sketch

These APIs are listed in their first source order. Registering a callback does not execute its body at that point.

1. `worker.start(...)` — Start work using the configured entry point; the context and objects must remain valid while it runs.

## API usage and precautions

Keep `Task` objects and C-style argument storage alive until their workers exit. Check `start()` / `startLoop()` results when adapting the examples; a missing log may be a creation failure, not a timing problem. A loop callback returns true to continue and false to finish.

`requestStop()` requests cooperative termination: it does not immediately interrupt a long callback, and a C-style function must inspect the stop flag itself. Periods passed to `startLoop()` are milliseconds; raw FreeRTOS delays use ticks. On ESP32, task stack sizes and stack high-water values are bytes, even where an older sample output label says words.

Diagnostic features are conditional on the core configuration: unavailable statistics are not necessarily a failed worker. Prefer the root API documentation over duplicating default sizes or build-option tables.

## Adapt this particular example

WorkerTask contains its own infinite loop. A C-style entry does not gain the startLoop stop checks automatically; add a cooperative exit path if your application needs one.

## Verify and troubleshoot

Start a C-style FreeRTOS worker function and verify the result on its output. First use the supplied input, then change one configuration or data source at a time.

| Symptom | First check |
|---|---|
| No output | Board/dependencies, initialization results, output destination and Serial baud |
| Starts but stops making progress | Wait conditions, buffer space, peer/input readiness and service calls |
| Unexpected values | Lengths/units, format, data lifetime, shared state and partial results |

See the [guide](../../docs/GUIDE_EXAMPLES.md) for shared principles and the [example index](../README.md) for the next sketch.
