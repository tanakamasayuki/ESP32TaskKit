#include <ESP32TaskKit.h>

TaskKit::Task inlineCfgTask;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  inlineCfgTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] inline cfg task\n", millis()); // en: inline-configured task / ja: インライン設定のタスク
        return true;                                            // en: continue / ja: 継続
      },
      TaskKit::TaskConfig{
          "InlineCfg",             // en: task name / ja: タスク名
          ARDUINO_LOOP_STACK_SIZE, // en: stack size (word) / ja: スタックサイズ（word）
          2,                       // en: priority / ja: 優先度
          ARDUINO_RUNNING_CORE     // en: pin to loop core / ja: loop コアに固定
      },
      700); // en: 0.7s interval / ja: 0.7秒周期
}

void loop()
{
  delay(1000); // en: idle loop / ja: メインループは待機
}
