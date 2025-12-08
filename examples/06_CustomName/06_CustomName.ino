#include <ESP32TaskKit.h>

TaskKit::Task autoTask;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfg;
  cfg.name = "CustomName"; // en: set custom task name / ja: 任意のタスク名を指定
  // en: other fields use defaults (priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: それ以外はデフォルト（priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  autoTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] name=%s\n", millis(), pcTaskGetName(nullptr)); // en: show task name / ja: タスク名を表示
        return true;                                                            // en: continue / ja: 継続
      },
      cfg,
      800); // en: 0.8s interval / ja: 0.8秒周期
}

void loop()
{
  delay(1); // en: idle loop / ja: メインループは待機
}
