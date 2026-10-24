// en: Pass a context pointer to a C-style task
// ja: C形式のタスクへコンテキストのポインタを渡す
#include <ESP32TaskKit.h>

#define LED_PIN_1 2
#define LED_PIN_2 4

ESP32TaskKit::Task blinkTask1;
ESP32TaskKit::Task blinkTask2;

struct BlinkArgs
{
  uint8_t pin;
  TickType_t periodMs;
};

BlinkArgs blinkArgs1{LED_PIN_1, 300};
BlinkArgs blinkArgs2{LED_PIN_2, 700};

// en: FreeRTOS-style task receiving its argument via void* / ja: FreeRTOS 的に void* で引数を受け取るタスク
void BlinkTask(void *pv)
{
  BlinkArgs *args = static_cast<BlinkArgs *>(pv);
  TickType_t lastWake = xTaskGetTickCount();

  for (;;)
  {
    int current = digitalRead(args->pin);
    int next = current == HIGH ? LOW : HIGH;
    digitalWrite(args->pin, next);
    Serial.printf("[+%lu ms] GPIO %u -> %s\n", millis(), args->pin, next == HIGH ? "HIGH" : "LOW");

    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(args->periodMs));
  }
}

void setup()
{
  Serial.begin(115200); // en: start serial / ja: シリアル開始

  // en: prepare GPIOs for output / ja: GPIO を出力設定
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);

  ESP32TaskKit::TaskConfig cfg; // en: defaults / ja: デフォルト設定

  // en: start tasks, passing pin/period via args / ja: ピンと周期を引数経由で渡す
  // en: Start work using the configured entry point; the context and objects must remain valid while it runs.
  // ja: 設定した入口から処理を開始します。実行中はコンテキストとオブジェクトの寿命を保ちます。
  blinkTask1.start(&BlinkTask, &blinkArgs1, cfg);
  blinkTask2.start(&BlinkTask, &blinkArgs2, cfg);
}

void loop()
{
  delay(1); // en: idle loop / ja: メインループは待機
}
