#include <ESP32TaskKit.h>

TaskKit::Task autoTask;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfg;
  cfg.name = "";                           // en: leave empty to auto-generate name / ja: 空にして自動採番
  cfg.priority = 2;                        // en: priority / ja: 優先度
  cfg.stackSize = ARDUINO_LOOP_STACK_SIZE; // en: stack size (word) / ja: スタックサイズ（word）
  cfg.core = ARDUINO_RUNNING_CORE;         // en: pin to loop core / ja: loop コアに固定

  autoTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] name=%s\n", millis(), pcTaskGetName(nullptr)); // en: show auto name / ja: 自動採番された名前を表示
        return true;                                                            // en: continue / ja: 継続
      },
      cfg,
      800); // en: 0.8s interval / ja: 0.8秒周期
}

void loop()
{
  delay(1000); // en: idle loop / ja: メインループは待機
}
