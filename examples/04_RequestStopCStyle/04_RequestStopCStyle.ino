// en: Check a stop request inside a C-style worker
// ja: C形式のタスク内で停止要求を確認する
#include <ESP32TaskKit.h>

ESP32TaskKit::Task worker;
unsigned long startedAt = 0;

void WorkerTask(void *pv)
{
  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t period = 500; // en: fixed 500ms period / ja: 500ms の固定周期

  for (;;)
  {
    if (worker.isStopRequested())
    {
      Serial.printf("[+%lu ms] stop requested -> exit\n", millis()); // en: exit when stop requested / ja: 停止要求を検知したら終了
      break;
    }

    // en: show C-style task activity
    // ja: Cスタイルタスクの動作を表示
    Serial.printf("[+%lu ms] C-style task running\n", millis());
    // en: simulate 10ms work; vTaskDelayUntil keeps 500ms interval
    // ja: 10ms の処理を模擬。vTaskDelayUntil で周期はズレない
    delay(10);

    // en: keep a fixed period with vTaskDelayUntil (includes Serial time)
    // ja: vTaskDelayUntil でシリアル出力時間込みの固定周期にする
    vTaskDelayUntil(&lastWake, period);
  }

  Serial.printf("[+%lu ms] C-style task done\n", millis()); // en: finished message / ja: 終了メッセージ
}

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  ESP32TaskKit::TaskConfig cfg;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  // en: Start work using the configured entry point; the context and objects must remain valid while it runs.
  // ja: 設定した入口から処理を開始します。実行中はコンテキストとオブジェクトの寿命を保ちます。
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
    // en: Request cooperative termination; the worker must reach a stop check before it can finish.
    // ja: 協調的な終了を要求します。ワーカーが停止確認へ進むまで終了しません。
    worker.requestStop();
  }

  // en: show state
  // ja: 状態表示
  Serial.printf("[+%lu ms] isRunning=%d isStopRequested=%d\n", millis(), worker.isRunning(), worker.isStopRequested());
  delay(500);
}
