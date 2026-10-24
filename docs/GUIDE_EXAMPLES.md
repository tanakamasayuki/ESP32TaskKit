# ESP32TaskKit — Example workflow

> 日本語版: [GUIDE_EXAMPLES.ja.md](GUIDE_EXAMPLES.ja.md)

Start, stop and inspect managed FreeRTOS tasks.

Check installation and platform requirements in the [library README](../README.md), then choose a sketch from the [example index](../examples/README.md). Version/support tables and internal capacity defaults stay in their original reference rather than being duplicated here.

## From setup to a verified result

1. Choose the smallest relevant sketch and check its board, hardware, dependencies and bundled files.
2. Match pins, peers and input files to your actual setup. Each README identifies important placeholders.
3. Open the .ino, select board/port and upload. Match Serial baud where used.
4. Locate the last completed stage: initialization, input, processing or output. Compilation does not test hardware I/O.
5. Reproduce the supplied example before changing one setting at a time.

## API and data handling

Keep `Task` objects and C-style argument storage alive until their workers exit. Check `start()` / `startLoop()` results when adapting the examples; a missing log may be a creation failure, not a timing problem. A loop callback returns true to continue and false to finish.

`requestStop()` requests cooperative termination: it does not immediately interrupt a long callback, and a C-style function must inspect the stop flag itself. Periods passed to `startLoop()` are milliseconds; raw FreeRTOS delays use ticks. On ESP32, task stack sizes and stack high-water values are bytes, even where an older sample output label says words.

Diagnostic features are conditional on the core configuration: unavailable statistics are not necessarily a failed worker. Prefer the root API documentation over duplicating default sizes or build-option tables.

## When adapting it

- Distinguish bool status, processed counts and handles; do not use stale output after failure.
- Registered callbacks run later from their documented context. Decide synchronization and retained-data lifetime.
- Keep bytes, samples, frames and pixels distinct; capacity is different from valid length.
- Retain views/pointers/buffers for the required period. Copying a pointer does not copy its payload.
- Add application-specific failure handling, retry and cleanup that a minimal example may omit.

## Read the individual README

Use Prepare and run for hardware and steps, Follow the sketch for the actual APIs and precautions for feature-specific constraints. If output is missing, check initialization first, continued service calls next and input/peer readiness after that.
