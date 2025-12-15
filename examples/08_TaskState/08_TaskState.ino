#include <ESP32TaskKit.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

ESP32TaskKit::Task worker;

#if (INCLUDE_eTaskGetState == 1)
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
#endif

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
      ESP32TaskKit::TaskConfig{}, // en: defaults / ja: デフォルト設定
      3000);                 // en: 3s interval / ja: 3秒 周期
}

void loop()
{
  static uint32_t lastLog = 0;
  if (millis() - lastLog >= 2000)
  {
    lastLog = millis();

    Serial.printf("---- Task Info at +%lu ms ----\n", millis()); // en: log header / ja: ログヘッダ

    TaskHandle_t h = worker.handle();
    if (!h)
    {
      Serial.println("worker handle is null (stopped?)"); // en: no handle (maybe stopped)
      // ja: ハンドルなし（停止済みの可能性）
      delay(10);
      return;
    }

    const char *name = pcTaskGetName(h); // en: task name / ja: タスク名
    Serial.printf("name=%s\n", name ? name : "(null)");

    UBaseType_t taskNumber = uxTaskGetTaskNumber(h); // en: task number / ja: タスク番号
    Serial.printf("taskNumber=%u\n", taskNumber);

#if (INCLUDE_eTaskGetState == 1)
    eTaskState st = eTaskGetState(h); // en: get FreeRTOS task state / ja: FreeRTOS のタスク状態を取得
    Serial.printf("state=%s\n", taskStateName(st));
#else
    Serial.println("eTaskGetState not available (INCLUDE_eTaskGetState==0)"); // en: not enabled in config / ja: 設定で無効な場合
#endif

    UBaseType_t prio = uxTaskPriorityGet(h); // en: priority / ja: 優先度
    Serial.printf("priority=%u\n", prio);

    UBaseType_t watermark = uxTaskGetStackHighWaterMark(h); // en: stack high-water mark (words) / ja: スタック余裕（word 単位）
    Serial.printf("stackHighWaterMark(words)=%lu\n", static_cast<unsigned long>(watermark));

    // en: worker's core
    // ja: ワーカーのコア
    int coreId = xTaskGetCoreID(h);
    if (coreId == tskNO_AFFINITY)
    {
      Serial.println("coreid=NO_AFFINITY"); // en/ja: コア非固定の場合
    }
    else
    {
      Serial.printf("coreid=%d\n", coreId);
    }

#if (configUSE_TRACE_FACILITY == 1)
    TaskStatus_t info = {};
    // en: get detailed info (includes stack high-water mark and runtime counter)
    // ja: 詳細情報を取得（スタック余裕やランタイムカウンタなど）
    vTaskGetInfo(h, &info, pdTRUE, eInvalid);
    Serial.printf("info.name=%s\n", info.pcTaskName ? info.pcTaskName : "(null)");
    Serial.printf("info.taskNumber=%lu\n", static_cast<unsigned long>(info.xTaskNumber));
    Serial.printf("info.state=%s\n", taskStateName(info.eCurrentState));
    Serial.printf("info.currentPriority=%u\n", info.uxCurrentPriority);
    Serial.printf("info.basePriority=%u\n", info.uxBasePriority);
    Serial.printf("info.runTimeCounter=%lu\n", static_cast<unsigned long>(info.ulRunTimeCounter));
    Serial.printf("info.pxStackBase=%p\n", info.pxStackBase);

#if ((portSTACK_GROWTH > 0) && (configRECORD_STACK_HIGH_ADDRESS == 1))
    Serial.printf("info.pxTopOfStack=%p\n", info.pxTopOfStack); // en: top of stack / ja: スタックのトップ
    Serial.printf("info.pxEndOfStack=%p\n", info.pxEndOfStack); // en: end of stack / ja: スタックの末尾
#endif

    Serial.printf("info.stackHighWaterMark(words)=%lu\n", static_cast<unsigned long>(info.usStackHighWaterMark));

#if (configTASKLIST_INCLUDE_COREID == 1)
    if (info.xCoreID == tskNO_AFFINITY)
    {
      Serial.println("info.coreID=NO_AFFINITY"); // en/ja: コア非固定の場合
    }
    else
    {
      Serial.printf("info.coreID=%ld\n", static_cast<long>(info.xCoreID)); // en: running core / ja: 実行中のコア
    }
#else
    Serial.println("info.coreID unavailable (configUSE_CORE_AFFINITY==0)"); // en/ja: コアID取得が無効な場合
#endif
#endif

    Serial.printf("isRunning=%d\n", worker.isRunning()); // en: library-managed running state / ja: ライブラリ管理の状態
  }

  delay(10); // en: small wait in main loop / ja: メインループの軽いウエイト
}
