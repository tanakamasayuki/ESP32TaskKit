# ESP32TaskKit

[English README](README.md)

Arduino 上で ESP32 の FreeRTOS タスクを手軽に扱うための小さなヘルパーです。

## 特長
- C スタイル関数で開始する `start` と、ラムダ/ファンクタでループを書く `startLoop`
- `requestStop()` と `isStopRequested()` による協調的な停止
- タスクごとにスタックサイズ・優先度・実行コアを指定可能
- 名前未指定時は自動採番
- ESP-IDF の `ESP_LOG*` マクロによるシンプルなロギング

## 動作条件
- Arduino-ESP32 3.x 系
- C++17 対応ツールチェーン（Arduino-ESP32 同梱のものを使用）

## かんたん例
```cpp
#include <ESP32TaskKit.h>

TaskKit::Task worker;

void setup() {
  Serial.begin(115200);

  // ループタスクの例
  worker.startLoop([] {
    Serial.println("working...");
    delay(100);
    return true;                  // 継続
  }, TaskKit::TaskConfig{}, 500); // 500ms 周期
}

void loop() {
  // 5 秒後に停止要求
  static uint32_t start = millis();
  if (worker.isRunning() && millis() - start > 5000) {
    worker.requestStop();
  }
  delay(200);
}
```

### C スタイルタスクでの協調停止
```cpp
TaskKit::Task worker;

void WorkerTask(void *pv) {
  for (;;) {
    if (worker.isStopRequested()) {
      break; // 停止要求でループを抜ける
    }
    // ここで処理
    delay(200);
  }
}

void setup() {
  worker.start(&WorkerTask);
}
```

## サンプル
- `examples/01_BasicLoop` — 最小のループタスク
- `examples/02_CStyleTask` — C スタイルの基本
- `examples/03_RequestStop` — startLoop での requestStop
- `examples/04_RequestStopCStyle` — C スタイルでの requestStop
- `examples/05_TwoTasks` — 2 つのループタスク
- `examples/06_CustomName` — タスク名の指定
- `examples/07_InlineConfig` — TaskConfig をインライン指定
- `examples/08_TaskState` — FreeRTOS のタスク状態/情報をループから参照

## メモ
- `isRunning()` はライブラリが管理する状態を返し、`eTaskGetState` を直接は参照しません。
- 協調停止はフラグ方式です。`requestStop()` はフラグを立てるだけなので、`isStopRequested()` を見てループを抜けてください。
- 本ライブラリは Arduino 環境向けであり、ESP-IDF 単体向けではありません。

## ライセンス
`LICENSE` を参照してください。
