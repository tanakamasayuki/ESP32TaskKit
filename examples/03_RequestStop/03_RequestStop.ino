// en: Request cooperative termination of a loop worker
// ja: ループタスクへ協調的な停止を要求する
#include <ESP32TaskKit.h>

ESP32TaskKit::Task worker;
unsigned long startedAt = 0;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  ESP32TaskKit::TaskConfig cfg;
  // en: use defaults (auto name, priority=2, stack=ARDUINO_LOOP_STACK_SIZE, core=tskNO_AFFINITY)
  // ja: デフォルトを使用（自動名、priority=2、stack=ARDUINO_LOOP_STACK_SIZE、core=tskNO_AFFINITY）

  // en: Start a repeating callback; true continues and false finishes. Check creation success and keep referenced context alive.
  // ja: trueで継続、falseで終了する繰り返し処理を開始します。生成の成否と参照するコンテキストの寿命を確認します。
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
    // en: Request cooperative termination; the worker must reach a stop check before it can finish.
    // ja: 協調的な終了を要求します。ワーカーが停止確認へ進むまで終了しません。
    worker.requestStop();
  }

  // en: show state
  // ja: 状態表示
  Serial.printf("[+%lu ms] isRunning=%d isStopRequested=%d\n", millis(), worker.isRunning(), worker.isStopRequested());
  delay(500);
}
