#include <ESP32TaskKit.h>

TaskKit::Task fastTask;
TaskKit::Task slowTask;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfgFast;
  cfgFast.name = "Fast";                       // en: fast task name / ja: 高頻度タスク名
  cfgFast.priority = 2;                        // en: priority / ja: 優先度
  cfgFast.stackSize = ARDUINO_LOOP_STACK_SIZE; // en/ja: スタックサイズ（word）
  cfgFast.core = ARDUINO_RUNNING_CORE;         // en: pin to loop core / ja: loop コアに固定

  TaskKit::TaskConfig cfgSlow = cfgFast;
  cfgSlow.name = "Slow"; // en: slow task name / ja: 低頻度タスク名

  fastTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] fast\n", millis()); // en/ja: 高頻度の例
        delay(50);                                   // en: simulate short work / ja: 短い処理を模擬
        return true;                                 // en/ja: continue
      },
      cfgFast,
      200); // en: 200ms interval / ja: 200ms 周期

  slowTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] slow\n", millis()); // en/ja: 低頻度の例
        delay(100);                                  // en: simulate longer work / ja: 少し長い処理を模擬
        return true;                                 // en/ja: continue
      },
      cfgSlow,
      1000); // en: 1s interval / ja: 1秒周期
}

void loop()
{
  delay(1000); // en: idle loop / ja: メインループは待機
}
