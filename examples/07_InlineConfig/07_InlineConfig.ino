#include <ESP32TaskKit.h>

TaskKit::Task inlineCfgTask;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  inlineCfgTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] inline cfg task\n", millis()); // en: inline-configured task / ja: インライン設定のタスク
        return true;                                            // en: continue / ja: 継続
      },
      []() {
        TaskKit::TaskConfig cfg{};             // en: start from defaults / ja: デフォルトから開始
        cfg.name = "InlineCustom";             // en: custom name / ja: 任意の名前
        cfg.stackSize = 2048;                  // en: smaller stack (words) for low memory / ja: 省メモリ用に小さめ（word 単位）
        cfg.priority = 0;                      // en: idle priority / ja: アイドル優先度
        cfg.core = 0;                          // en: pin to core 0 / ja: コア0に固定
        return cfg;                            // en/ja: return config
      }(),
      700); // en: 0.7s interval / ja: 0.7秒周期
}

void loop()
{
  delay(1000); // en: idle loop / ja: メインループは待機
}
