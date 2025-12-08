#include <ESP32TaskKit.h>

TaskKit::Task worker;
unsigned long startedAt = 0;

void WorkerTask(void *pv)
{
  for (;;)
  {
    if (worker.isStopRequested())
    {
      Serial.printf("[+%lu ms] stop requested -> exit\n", millis()); // en/ja: 停止要求を検知したので終了
      break;
    }

    Serial.printf("[+%lu ms] C-style task running\n", millis()); // en/ja: Cスタイルタスクの動作を表示
    delay(200);                                                  // en/ja: 処理を模擬
  }

  Serial.printf("[+%lu ms] C-style task done\n", millis()); // en/ja: 終了メッセージ
}

void setup()
{
  Serial.begin(115200); // en/ja: シリアル開始

  TaskKit::TaskConfig cfg;
  // en/ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  worker.start(&WorkerTask, nullptr, cfg); // en/ja: Cスタイルタスクを開始
  startedAt = millis();
}

void loop()
{
  // en/ja: 5秒後に停止要求を出す
  if (worker.isRunning() && millis() - startedAt > 5000)
  {
    Serial.println("requestStop()");
    worker.requestStop();
  }

  Serial.printf("isRunning=%d\n", worker.isRunning()); // en/ja: 状態表示
  delay(500);
}
