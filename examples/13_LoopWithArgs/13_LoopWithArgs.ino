#include <ESP32TaskKit.h>

#define LED_PIN_1 2
#define LED_PIN_2 4

ESP32TaskKit::Task blinkTask1;
ESP32TaskKit::Task blinkTask2;

// en: helper that toggles the given GPIO / ja: 指定 GPIO をトグルするヘルパー
bool blinkOnce(uint8_t pin)
{
  int current = digitalRead(pin);
  int next = current == HIGH ? LOW : HIGH;
  digitalWrite(pin, next);
  Serial.printf("[+%lu ms] GPIO %u -> %s\n", millis(), pin, next == HIGH ? "HIGH" : "LOW");
  return true; // en: continue / ja: 継続
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

  // en: task #1 uses LED_PIN_1 / ja: タスク1 は LED_PIN_1 を使用
  blinkTask1.startLoop(
      []()
      {
        return blinkOnce(LED_PIN_1); // en: pass pin as argument / ja: ピン番号を引数で渡す
      },
      cfg,
      300); // en: 300ms interval / ja: 300ms 周期

  // en: task #2 uses LED_PIN_2 / ja: タスク2 は LED_PIN_2 を使用
  blinkTask2.startLoop(
      []()
      {
        return blinkOnce(LED_PIN_2);
      },
      cfg,
      700); // en: 700ms interval / ja: 700ms 周期
}

void loop()
{
  delay(1); // en: idle loop / ja: メインループは待機
}
