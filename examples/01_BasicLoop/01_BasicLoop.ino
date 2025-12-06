#include <ESP32TaskKit.h>

TaskKit::Task worker;

void setup()
{
  Serial.begin(115200); // en: start serial for demo / ja: デモ用にシリアル開始

  TaskKit::TaskConfig cfg;
  cfg.priority = 2;                        // en: priority (higher than loop) / ja: 優先度（loop より高め）
  cfg.stackSize = ARDUINO_LOOP_STACK_SIZE; // en: stack size in words / ja: スタックサイズ（word 単位）
  cfg.core = ARDUINO_RUNNING_CORE;         // en: pin to Arduino loop core / ja: Arduino の loop コアに固定

  worker.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] Hello from TaskKit\n", millis()); // en: print elapsed / ja: 経過時間を表示
        delay(100);                                                // en: simulate 100ms work; vTaskDelayUntil keeps 1s interval / ja: 100ms の処理を模擬。vTaskDelayUntil で周期はズレない
        return true;                                               // en: continue looping / ja: ループ継続
      },
      cfg,
      1000); // en: 1s interval (no drift even with delay inside) / ja: 1秒間隔（中で delay しても周期がずれにくい）
}

void loop()
{
  delay(1); // en: idle loop / ja: メインループは待機
}
