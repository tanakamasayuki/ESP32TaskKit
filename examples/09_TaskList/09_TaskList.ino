#include <ESP32TaskKit.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

TaskKit::Task worker;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  worker.startLoop(
      []()
      {
        // en: worker heartbeat / ja: ワーカーのハートビート
        Serial.printf("[+%lu ms] worker alive\n", millis());
        delay(200);  // en: simulate work / ja: 処理を模擬
        return true; // en: continue / ja: 継続
      },
      TaskKit::TaskConfig{}, // en: defaults / ja: デフォルト設定
      5000);                 // en: 5s interval / ja: 5秒周期
}

void loop()
{
  static uint32_t lastLog = 0;
  if (millis() - lastLog >= 2000)
  {
    lastLog = millis();

    Serial.println("---- vTaskList ----");           // en: header / ja: 見出し
    UBaseType_t numTasks = uxTaskGetNumberOfTasks(); // en: number of tasks / ja: タスク数
    Serial.printf("tasks=%lu\n", static_cast<unsigned long>(numTasks));

#if (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
    // en: vTaskList writes a human-readable table into the buffer
    // ja: vTaskList は可読なテーブルをバッファへ出力する
    char buf[2048] = {0};
    vTaskList(buf);
    Serial.println("Name            State   Prio    Stack   Num     Core");
    Serial.println(buf);
#else
    Serial.println("vTaskList not available (configUSE_TRACE_FACILITY && configUSE_STATS_FORMATTING_FUNCTIONS required)"); // en/ja: 必要な設定が無効
#endif
  }

  delay(10); // en: small wait / ja: 軽いウエイト
}
