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

    Serial.printf("[+%lu ms] C-style task running\n", millis()); // en: show C-style task activity / ja: Cスタイルタスクの動作を表示
    delay(10);                                                   // en: simulate 10ms work; vTaskDelayUntil keeps 500ms interval / ja: 10ms の処理を模擬。vTaskDelayUntil で周期はズレない

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

  // en: show state
  // ja: 状態表示
  Serial.printf("[+%lu ms] isRunning=%d isStopRequested=%d\n", millis(), worker.isRunning(), worker.isStopRequested());
  delay(500);
}
