#include <ESP32TaskKit.h>

TaskKit::Task worker;

void WorkerTask(void *pv)
{
  for (;;)
  {
    Serial.printf("[+%lu ms] C-style task running\n", millis()); // en/ja: Cスタイルタスクの動作を表示
    delay(200);                                                  // en: simulate work / ja: 処理を模擬
  }
}

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfg;
  cfg.name = "CStyle";                     // en: task name / ja: タスク名
  cfg.priority = 2;                        // en: priority / ja: 優先度
  cfg.stackSize = ARDUINO_LOOP_STACK_SIZE; // en/ja: スタックサイズ（word）
  cfg.core = ARDUINO_RUNNING_CORE;         // en: pin to loop core / ja: loop コアに固定

  worker.start(&WorkerTask, nullptr, cfg);
}

void loop()
{
  delay(1000); // en: idle loop / ja: メインループは待機
}
