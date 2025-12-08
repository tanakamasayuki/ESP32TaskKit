#include <ESP32TaskKit.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

TaskKit::Task worker;

const char *taskStateName(eTaskState s)
{
  switch (s)
  {
  case eRunning:
    return "Running";
  case eReady:
    return "Ready";
  case eBlocked:
    return "Blocked";
  case eSuspended:
    return "Suspended";
  case eDeleted:
    return "Deleted";
  default:
    return "Unknown";
  }
}

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

    Serial.println("---- uxTaskGetSystemState ----"); // en: header / ja: 見出し

#if (configUSE_TRACE_FACILITY == 1)
    UBaseType_t count = uxTaskGetNumberOfTasks(); // en: estimate current tasks / ja: 現在のタスク数を見積もり
    TaskStatus_t *arr = static_cast<TaskStatus_t *>(pvPortMalloc(count * sizeof(TaskStatus_t)));
    if (arr)
    {
      UBaseType_t got = uxTaskGetSystemState(arr, count, nullptr); // en/ja: 実際に取得できた件数
      Serial.printf("tasks=%lu\n", static_cast<unsigned long>(got));
      for (UBaseType_t i = 0; i < got; ++i)
      {
        const TaskStatus_t &t = arr[i];
        Serial.printf("%s state=%s basePrio=%u stackHWM=%lu",
                      t.pcTaskName ? t.pcTaskName : "(null)",
                      taskStateName(t.eCurrentState),
                      t.uxBasePriority,
                      static_cast<unsigned long>(t.usStackHighWaterMark));
#if defined(configTASKLIST_INCLUDE_COREID) && (configTASKLIST_INCLUDE_COREID == 1)
        Serial.printf(" core=%ld", static_cast<long>(t.xCoreID)); // en: core id / ja: 実行コア
#else
        Serial.print(" core=N/A"); // en: core info disabled / ja: コア取得無効
#endif
        Serial.printf(" runtime=%lu\n", static_cast<unsigned long>(t.ulRunTimeCounter)); // en: runtime counter (0 if disabled) / ja: ランタイム（無効設定なら0）
      }
      vPortFree(arr); // en: free FreeRTOS allocation / ja: FreeRTOS の確保を解放
    }
    else
    {
      Serial.println("uxTaskGetSystemState alloc failed"); // en: allocation failed / ja: 取得用メモリ確保に失敗
    }
#else
    Serial.println("uxTaskGetSystemState not available (configUSE_TRACE_FACILITY==0)"); // en: required config disabled / ja: 必要な設定が無効
#endif
  }

  delay(10); // en: small wait / ja: 軽いウエイト
}
