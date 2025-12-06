#include <ESP32TaskKit.h>

TaskKit::Task worker;

void WorkerTask(void *pv)
{
  for (;;)
  {
    Serial.printf("[+%lu ms] C-style task running\n", millis()); // en: show C-style task activity / ja: Cスタイルタスクの動作を表示
    delay(200);                                                  // en: simulate work / ja: 処理を模擬
  }
}

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfg;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  worker.start(&WorkerTask, nullptr, cfg);
}

void loop()
{
  delay(1000); // en: idle loop / ja: メインループは待機
}
