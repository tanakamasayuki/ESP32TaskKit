#include <ESP32TaskKit.h>

TaskKit::Task worker;
unsigned long startedAt = 0;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  TaskKit::TaskConfig cfg;
  cfg.name = "StopDemo";                   // en: task name / ja: タスク名
  cfg.priority = 2;                        // en: priority / ja: 優先度
  cfg.stackSize = ARDUINO_LOOP_STACK_SIZE; // en/ja: スタックサイズ（word）
  cfg.core = ARDUINO_RUNNING_CORE;         // en: pin to loop core / ja: loop コアに固定

  worker.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] working...\n", millis()); // en/ja: 動作中メッセージ
        delay(100);                                        // en: simulate work / ja: 処理を模擬
        return true;                                       // en/ja: continue
      },
      cfg,
      500); // en: 0.5s interval / ja: 0.5秒周期

  startedAt = millis();
}

void loop()
{
  // en: request stop after 5 seconds / ja: 5秒後に停止要求
  if (worker.isRunning() && millis() - startedAt > 5000)
  {
    Serial.println("requestStop()");
    worker.requestStop();
  }

  // en: show state / ja: 状態表示
  Serial.printf("isRunning=%d\n", worker.isRunning());
  delay(500);
}
