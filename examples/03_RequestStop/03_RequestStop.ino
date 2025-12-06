#include <ESP32TaskKit.h>

TaskKit::Task worker;
unsigned long startedAt = 0;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfg;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  worker.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] working...\n", millis()); // en: working message / ja: 動作中メッセージ
        delay(100);                                        // en: simulate work / ja: 処理を模擬
        return true;                                       // en: continue / ja: 継続
      },
      cfg,
      500); // en: 0.5s interval / ja: 0.5秒周期

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
  Serial.printf("isRunning=%d\n", worker.isRunning());
  delay(500);
}
