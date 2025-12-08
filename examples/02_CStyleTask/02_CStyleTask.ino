#include <ESP32TaskKit.h>

TaskKit::Task worker;

void WorkerTask(void *pv)
{
  // en: C-style tasks must implement their own infinite loop
  // ja: Cスタイルタスクは自前で無限ループを書く
  for (;;)
  {
    // en: show C-style task activity
    // ja: Cスタイルタスクの動作を表示
    Serial.printf("[+%lu ms] C-style task running TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST\n", millis());

    // en: simulate work (Serial time shifts period)
    // ja: 処理を模擬（シリアル出力時間分だけ周期がずれる）
    delay(1000);
    // en: Use vTaskDelayUntil to keep a fixed period if you need precise timing
    // ja: 周期を正確に保ちたい場合は vTaskDelayUntil を使って補正する
    // en: Call delay/yield periodically; otherwise equal-or-lower priority tasks may starve and WDT can fire
    // ja: delay や yield を適度に呼ばないと、同等以下の優先度タスクが動かず WDT が発火することがある
    // en: For cooperative stop, check isStopRequested() as in 04_RequestStopCStyle.ino
    // ja: 協調停止が必要な場合は 04_RequestStopCStyle.ino のように isStopRequested() を確認する
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
  delay(1); // en: idle loop / ja: メインループは待機
}
