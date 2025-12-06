# ESP32TaskKit — 仕様書

ESP32TaskKit は、ESP32（Arduino）向けの  
**「中級者向けタスク管理ライブラリ」** です。

- 入門用：**ESP32AutoTask**  
  → 弱宣言 `LoopCoreX_YYY` を定義するだけでタスクが自動生成される「補助輪」
  https://github.com/tanakamasayuki/ESP32AutoTask

- 同期用：**ESP32AutoSync**  
  → Queue / Notify / Semaphore / Mutex などの FreeRTOS 同期プリミティブの C++ ラッパ
  https://github.com/tanakamasayuki/ESP32AutoSync

- その中間：**ESP32TaskKit**  
  → FreeRTOS のタスク (`xTaskCreatePinnedToCore` など) を  
     **C++ のクラスと設定オブジェクトで、わかりやすく管理するためのキット**
     https://github.com/tanakamasayuki/ESP32TaskKit

という位置づけです。

---

## 1. コンセプト（Concept）

ESP32TaskKit の主な目的は次の 3 つです。

1. **FreeRTOS のタスクを “手書き” する負担を減らす**
2. **C++ らしい書き方（ラムダ・functor・RAII）でタスクを書く**
3. **タスクと同期（AutoSync）の責務を分離する**

---

## 2. 基本思想・設計ポリシー

### 2.1 タスク＝クラスで管理
### 2.2 Config でまとめて設定
### 2.3 start と startLoop の 2 系統
### 2.4 同期は AutoSync に任せる

---

## 3. 名前空間とファイル構成

```text
ESP32TaskKit/
  src/
    ESP32TaskKit.h
```

---

## 4. TaskConfig

```cpp
struct TaskConfig {
    const char*  name      = "TaskKitTask";
    uint32_t     stackSize = 8192;
    UBaseType_t  priority  = 1;
    int          core      = -1;
};
```

---

## 5. Task クラス

```cpp
class Task {
public:
    Task() noexcept;
    ~Task();

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    bool start(TaskFunction_t fn,
               void* arg = nullptr,
               const TaskConfig& cfg = TaskConfig{});

    template<class F>
    bool startLoop(F&& loopFunc,
                   const TaskConfig& cfg = TaskConfig{},
                   uint32_t periodMs = 1);

    bool isRunning() const noexcept;
    TaskHandle_t handle() const noexcept;

    void suspend();
    void resume();

    void requestStop() noexcept;
    bool stopRequested() const noexcept;

private:
    TaskHandle_t _handle;
    bool _running;
    bool _stopRequested;
};
```

### startLoop の挙動メモ
- `loopFunc` が `false` を返した時点でループ終了（タスク終了）。
- `periodMs > 0` なら `vTaskDelayUntil` で等間隔実行。
- `periodMs == 0` ではディレイを呼ばずノンウエイトで回す（WDT 回避のため最低でも 1ms 以上を推奨し、0 を指定する場合はループ内で適宜 `delay()` などを呼ぶ必要あり）。

---

## 6. 使用例

### 6.1 C スタイルタスク

```cpp
TaskKit::Task worker;

void WorkerTask(void* pv) {
    for (;;) {
        Serial.println("Working...");
        delay(100);
    }
}

void setup() {
    TaskKit::TaskConfig cfg;
    cfg.name = "Worker";
    cfg.priority = 2;

    worker.start(WorkerTask, nullptr, cfg);
}
```

---

### 6.2 ラムダタスク

```cpp
TaskKit::Task worker;

worker.startLoop(
    [] {
        Serial.println("Hello");
        delay(100);
        return true;
    }
);
```

---

### 6.3 周期タスク

```cpp
periodic.startLoop(
    [] {
        readSensor();
        return true;
    },
    TaskKit::TaskConfig{},
    5000
);
```

---

### 6.4 AutoSync と併用

```cpp
TaskKit::Task consumer;
AutoSync::Queue<int> q(8);

consumer.startLoop(
    [] {
        int v;
        if (q.receive(v)) {
            Serial.println(v);
        }
        return true;
    }
);
```

---

## 7. 設計ポリシー補足

- requestStop による協調停止
- デストラクタで強制終了しない
- ISR から呼ばない（同期は AutoSync を使う）

---

## 8. 将来拡張

- TaskGroup
- join() / await()
- ランタイム統計
- キャンセル機構

---

## 9. ライセンス

（MIT など）
