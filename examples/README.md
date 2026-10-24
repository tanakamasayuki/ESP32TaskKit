# ESP32TaskKit examples

> 日本語版: [README.ja.md](README.ja.md)

## Workflow and individual READMEs

Start with the [example workflow](../docs/GUIDE_EXAMPLES.md), then choose the relevant sketch.

A useful starting point is [01_BasicLoop](01_BasicLoop/README.md). Check its required hardware and choose another entry below if it does not match your setup.

| Example | Learn |
|---|---|
| [01_BasicLoop](01_BasicLoop/README.md) | Run a repeating worker callback alongside Arduino loop |
| [02_CStyleTask](02_CStyleTask/README.md) | Start a C-style FreeRTOS worker function |
| [03_RequestStop](03_RequestStop/README.md) | Request cooperative termination of a loop worker |
| [04_RequestStopCStyle](04_RequestStopCStyle/README.md) | Check a stop request inside a C-style worker |
| [05_TwoTasks](05_TwoTasks/README.md) | Run two workers with different periods |
| [06_CustomName](06_CustomName/README.md) | Assign and inspect a worker task name |
| [07_InlineConfig](07_InlineConfig/README.md) | Build task configuration at the start call |
| [08_TaskState](08_TaskState/README.md) | Observe a worker state and stack headroom |
| [09_TaskList](09_TaskList/README.md) | Print a FreeRTOS task list when supported |
| [10_TaskStatusArray](10_TaskStatusArray/README.md) | Inspect task status records from a system snapshot |
| [11_RunTimeStats](11_RunTimeStats/README.md) | Print configured runtime statistics |
| [12_TaskSystemState](12_TaskSystemState/README.md) | Inspect tasks using the system-state API |
| [13_LoopWithArgs](13_LoopWithArgs/README.md) | Pass persistent application data to a loop callback |
| [14_CStyleArgs](14_CStyleArgs/README.md) | Pass a context pointer to a C-style task |
