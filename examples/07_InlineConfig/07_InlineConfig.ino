// en: Build task configuration at the start call
// ja: 開始する箇所でタスク設定を組み立てる
#include <ESP32TaskKit.h>

ESP32TaskKit::Task inlineCfgTask;

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  // en: Start a repeating callback; true continues and false finishes. Check creation success and keep referenced context alive.
  // ja: trueで継続、falseで終了する繰り返し処理を開始します。生成の成否と参照するコンテキストの寿命を確認します。
  inlineCfgTask.startLoop(
      []()
      {
        Serial.printf("[+%lu ms] inline cfg task\n", millis()); // en: inline-configured task / ja: インライン設定のタスク
        return true;                                            // en: continue / ja: 継続
      },
      []()
      {
        ESP32TaskKit::TaskConfig cfg;   // en: already has defaults / ja: 構造体にデフォルトが入っている
        cfg.name = "InlineCustom"; // en: custom name / ja: 任意の名前
        // en: smaller stack (bytes on ESP32) for low memory
        // ja: 省メモリ用に小さめ（ESP32ではbyte単位）
        cfg.stackSize = 2048;
        cfg.priority = 0;          // en: idle priority / ja: アイドル優先度
        cfg.core = 0;              // en: pin to core 0 / ja: コア0に固定
        return cfg;                // en/ja: return config
      }(),
      700); // en: 0.7s interval / ja: 0.7秒周期
}

void loop()
{
  delay(1); // en: idle loop / ja: メインループは待機
}
