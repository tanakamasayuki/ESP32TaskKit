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
  if (millis() - lastLog >= 3000)
  {
    lastLog = millis();

    Serial.println("---- vTaskGetRunTimeStats ----"); // en: header / ja: 見出し

#if (configGENERATE_RUN_TIME_STATS == 1) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
    // en: vTaskGetRunTimeStats writes a table (requires run time stats setup)
    // ja: vTaskGetRunTimeStats はテーブルを出力（ランタイム統計の設定が必要）
    char buf[2048] = {0};
    vTaskGetRunTimeStats(buf);
    Serial.println("Name            Time            Percentage");
    Serial.println(buf);
#else
    Serial.println("vTaskGetRunTimeStats not available (enable configGENERATE_RUN_TIME_STATS & stats formatting)"); // en/ja: 必要な設定が無効
#endif
  }

  delay(10); // en: small wait / ja: 軽いウエイト
}
