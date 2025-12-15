#include <ESP32TaskKit.h>

ESP32TaskKit::Task fastTask;
ESP32TaskKit::Task slowTask;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  ESP32TaskKit::TaskConfig cfgFast;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  ESP32TaskKit::TaskConfig cfgSlow = cfgFast;

  fastTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] fast\n", millis()); // en: high-frequency example / ja: 高頻度の例
        delay(50);                                   // en: simulate short work / ja: 短い処理を模擬
        return true;                                 // en: continue / ja: 継続
      },
      cfgFast,
      200); // en: 200ms interval / ja: 200ms 周期

  slowTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] slow\n", millis()); // en: low-frequency example / ja: 低頻度の例
        delay(100);                                  // en: simulate longer work / ja: 少し長い処理を模擬
        return true;                                 // en: continue / ja: 継続
      },
      cfgSlow,
      1000); // en: 1s interval / ja: 1秒周期
}

void loop()
{
  delay(1); // en: idle loop / ja: メインループは待機
}
