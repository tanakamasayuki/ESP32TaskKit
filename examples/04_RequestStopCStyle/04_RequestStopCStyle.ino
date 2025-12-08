#include <ESP32TaskKit.h>

TaskKit::Task worker;
unsigned long startedAt = 0;

void WorkerTask(void *pv)
{
  for (;;)
  {
    if (worker.isStopRequested())
    {
      Serial.printf("[+%lu ms] stop requested -> exit\n", millis()); // en: exit when stop requested / ja: 停止要求を検知したら終了
      break;
    }

    Serial.printf("[+%lu ms] C-style task running\n", millis()); // en: show C-style task activity / ja: Cスタイルタスクの動作を表示
    delay(200);                                                  // en: simulate work / ja: 処理を模擬
  }

  Serial.printf("[+%lu ms] C-style task done\n", millis()); // en: finished message / ja: 終了メッセージ
}

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfg;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  worker.start(&WorkerTask, nullptr, cfg); // en: start C-style task / ja: Cスタイルタスクを開始
  startedAt = millis();
}

void loop()
{
  // en: request stop after 5 seconds
  // ja: 5秒後に停止要求
  if (worker.isRunning() && millis() - startedAt > 5000)
  {
    Serial.println("requestStop()");
    worker.requestStop();
  }

  Serial.printf("isRunning=%d\n", worker.isRunning()); // en: show state / ja: 状態表示
  delay(500);
}
