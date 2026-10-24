// en: Run a repeating worker callback alongside Arduino loop
// ja: Arduino loopと別に周期コールバックを動かす
#include <ESP32TaskKit.h>

ESP32TaskKit::Task worker;

void setup()
{
  Serial.begin(115200); // en: start serial for demo / ja: デモ用にシリアル開始

  ESP32TaskKit::TaskConfig cfg;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  // en: Start a repeating callback; true continues and false finishes. Check creation success and keep referenced context alive.
  // ja: trueで継続、falseで終了する繰り返し処理を開始します。生成の成否と参照するコンテキストの寿命を確認します。
  worker.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] Hello from ESP32TaskKit\n", millis()); // en: print elapsed / ja: 経過時間を表示
        // en: simulate 100ms work; vTaskDelayUntil keeps 1s interval
        // ja: 100ms の処理を模擬。vTaskDelayUntil で周期はズレない
        delay(100);
        return true;                                               // en: continue looping / ja: ループ継続
      },
      cfg,
      // en: 1s interval (no drift even with delay inside)
      // ja: 1秒間隔（中で delay しても周期がずれにくい）
      1000);
}

void loop()
{
  delay(1); // en: idle loop / ja: メインループは待機
}
