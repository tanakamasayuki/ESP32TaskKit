# ESP32TaskKit サンプル

> English: [README.md](README.md)

## 実行ガイドと個別README

最初に[サンプル入門ガイド](../docs/GUIDE_EXAMPLES.ja.md)を読み、目的に合う例へ進んでください。

最初の候補は[01_BasicLoop](01_BasicLoop/README.ja.md)です。必要な機器を確認し、対象が異なる場合は下の一覧から選び直してください。

| 例 | 学ぶこと |
|---|---|
| [01_BasicLoop](01_BasicLoop/README.ja.md) | Arduino loopと別に周期コールバックを動かす |
| [02_CStyleTask](02_CStyleTask/README.ja.md) | C形式のFreeRTOSタスク関数を開始する |
| [03_RequestStop](03_RequestStop/README.ja.md) | ループタスクへ協調的な停止を要求する |
| [04_RequestStopCStyle](04_RequestStopCStyle/README.ja.md) | C形式のタスク内で停止要求を確認する |
| [05_TwoTasks](05_TwoTasks/README.ja.md) | 異なる周期の2つのタスクを動かす |
| [06_CustomName](06_CustomName/README.ja.md) | タスク名を指定して実行中に確認する |
| [07_InlineConfig](07_InlineConfig/README.ja.md) | 開始する箇所でタスク設定を組み立てる |
| [08_TaskState](08_TaskState/README.ja.md) | タスクの状態とスタックの余裕を観察する |
| [09_TaskList](09_TaskList/README.ja.md) | 設定で利用可能ならFreeRTOSタスク一覧を表示する |
| [10_TaskStatusArray](10_TaskStatusArray/README.ja.md) | システムのスナップショットからタスク情報を調べる |
| [11_RunTimeStats](11_RunTimeStats/README.ja.md) | 設定で有効な実行時間統計を表示する |
| [12_TaskSystemState](12_TaskSystemState/README.ja.md) | System State APIでタスク群を調べる |
| [13_LoopWithArgs](13_LoopWithArgs/README.ja.md) | 寿命を保ったアプリの値をループ処理へ渡す |
| [14_CStyleArgs](14_CStyleArgs/README.ja.md) | C形式のタスクへコンテキストのポインタを渡す |
