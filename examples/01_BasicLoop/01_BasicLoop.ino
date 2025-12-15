#include <ESP32TaskKit.h>

ESP32TaskKit::Task worker;

void setup()
{
  Serial.begin(115200); // en: start serial for demo / ja: デモ用にシリアル開始

  ESP32TaskKit::TaskConfig cfg;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  worker.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] Hello from ESP32TaskKit\n", millis()); // en: print elapsed / ja: 経過時間を表示
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
