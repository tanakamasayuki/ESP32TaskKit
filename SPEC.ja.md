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

### ESP32AutoTask との使い分け
- ESP32AutoTask の利点: 弱宣言の `LoopCoreX_YYY` を書くだけでタスクが自動生成され、設定やクラス設計が不要。スタック/優先度は設定可能で、学習コストが低く「まず動かす」用途に最適。
- ESP32AutoTask の限界: コアごとにあらかじめ決まった数のタスクスロットで運用するため、細かいタスクを多数動かす用途には不向き。コア固定や細かなライフサイクル管理はできない。
- ESP32TaskKit の利点: Config でスタック/優先度/コアを明示し、`start`/`startLoop`/`requestStop` でライフサイクルを管理できる。ラムダや functor で C++ らしく書ける。
- ESP32TaskKit の限界: AutoTask ほど自動化されていないので最初に少し書く必要がある。特殊な FreeRTOS 直接操作が必要な場合は素の API を使う前提。
- 使い分け: 「簡単に動かしたい/まずは動作確認」なら AutoTask。タスクを柔軟に増やしたい・設定やライフサイクルをきれいに整えたい本番寄りのコードは TaskKit。さらに特殊な要件は素の FreeRTOS API へ。

### ESP32AutoSync との使い分け
- 完全に独立したライブラリで相互依存はない。どちらも素の FreeRTOS API を直接呼んでよい。
- 一般的な Queue/Notify/Semaphore/Mutex を使う範囲なら AutoSync を併用すると楽。特殊な同期が必要なら直接 FreeRTOS を使う。

---

## 1. コンセプト（Concept）

ESP32TaskKit の主な目的は次の 3 つです。

1. **FreeRTOS のタスクを “手書き” する負担を減らす**
2. **C++ らしい書き方（ラムダ・functor・RAII）でタスクを書く**
3. **タスクと同期（AutoSync）の責務を分離する**
4. **FreeRTOS らしいきれいな使い方を身につけられるようにする（通常の用途は本ライブラリで完結し、特殊な場合は素の FreeRTOS を使ってもらう）**

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
    const char*  name      = "";
    uint32_t     stackSize = ARDUINO_LOOP_STACK_SIZE; // 8192 words
    UBaseType_t  priority  = 2;
    int          core      = tskNO_AFFINITY;
};
```

メモ:
- `stackSize` はワード単位（`configSTACK_DEPTH_TYPE`）。デフォルトは `ARDUINO_LOOP_STACK_SIZE`（8192 words）。
- `core` は数値指定を標準とし、0 が `PRO_CPU_NUM`、1 が `APP_CPU_NUM`（ESP32 の呼び分け）で分かりやすくする。
- `core=tskNO_AFFINITY` ならコア固定せずスケジューラ任せ。固定したい場合は 0/1 を指定。
- `ARDUINO_RUNNING_CORE` は Arduino の `loop()` が動くコア番号（多くのボードで 1）。同じコアで動かしたい場合に指定できる。
- `priority` は FreeRTOS の 0（Idle）〜`configMAX_PRIORITIES-1` の範囲。Arduino の `loop()` は通常 1 なので、デフォルトで 2 を割り当て、リアルタイム性が必要なタスクは 2 以上を目安にする。
- `name` は FreeRTOS のタスク名（`configMAX_TASK_NAME_LEN` 文字まで）。未指定（空文字）の場合は `TaskKit#1` のように自動採番。Arduino 環境では標準のシリアルログでは見えず、`vTaskList` や JTAG/デバッガで確認する用途が中心。
- コア検証: デュアルコアなら `core` は 0/1/tskNO_AFFINITY のみ許容。シングルコア（`CONFIG_FREERTOS_UNICORE` 等が有効）では 0/tskNO_AFFINITY のみ許容。

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
- `periodMs > 0` なら `vTaskDelayUntil` で等間隔実行（デフォルト 1ms は WDT 回避と CPU 占有抑制のための最小値）。
- `periodMs == 0` ではディレイを呼ばずノンウエイトで回す（WDT 回避のため最低でも 1ms 以上を推奨し、0 を指定する場合はループ内で適宜 `delay()` などを呼ぶ必要あり）。

### ライフサイクルと終了メモ
- 状態: `Idle -> Running -> Idle` の単純遷移。`start` 成功時のみ `Running`、終了/失敗時は `Idle` のまま。
- 再利用: 終了後は同じ `Task` インスタンスで再 `start` 可能（終了時に `_handle=null`・`_running=false` を揃える）。
- 終了トリガ: `startLoop` は `false` を返すと終了。`requestStop()` はフラグを立てるだけなので、ループ内で `stopRequested()` を見て `return false;` する。C スタイルタスク（`start`）は自前でフラグを見て抜ける。`startLoop` はラッパ側が毎周回 `stopRequested()` をチェックし自動停止する。
- 終了待ち: `join` は提供せず、利用者が `isRunning()` をポーリングする前提（ポーリング時は `delay()` を挟んで WDT/CPU 占有を避ける）。
- デストラクタ: 強制終了しない方針。破棄前に利用者が停止を確認する（実行中に破棄された場合はログ/アサートで警告する程度の挙動にする）。
- エラー時: `start` が失敗した場合は状態を `Idle` に保ち、ハンドルを握らない。

### スレッドセーフティ方針
- 本ライブラリは基本的にスレッドセーフではない。`start` などの管理操作は単一タスク（所有者）からのみ呼ぶ前提。
- 例外として `requestStop()`/`stopRequested()`/`isRunning()` は他タスクから読んでよい程度の最小限のクロスコールを許容。
- ISR からの呼び出しは不可。他タスクから操作したい場合は FreeRTOS の通知や AutoSync (Queue/Notify/Semaphore) などスレッドセーフな手段でタスク間通信して対処する。
- `handle()` で取得した `TaskHandle_t` に直接 FreeRTOS API を呼ぶことは可能だが、ライブラリの状態管理とズレるため非推奨（例: 外部から `vTaskDelete` すると内部フラグが更新されない）。
- 内部フラグ（`_running`/`_stopRequested`）は Arduino 向けに通常の `bool` を用いる。クロスコールはベストエフォートで、厳密な排他が必要なら利用者側で同期を挟む。

### エラーハンドリング / ロギング
- `start`/`startLoop` の戻り値は成功/失敗の bool。失敗時は状態を `Idle` のまま保持。主な失敗要因は無効な `priority`/`core`、メモリ不足（スタック確保失敗）、`xTaskCreate` 系のエラー。
- ロギングは常に有効とし、Arduino 環境でも使える ESP-IDF のログマクロ `ESP_LOGE/W/I/D/V` を利用する。
- 主なログの目安:
  - `ESP_LOGE`: `start failed: invalid priority=%u core=%d`、`start failed: xTaskCreate err=%d`、`startLoop failed: alloc functor`
  - `ESP_LOGW`: `start called while running`、`destroying running task`
  - `ESP_LOGI/D`: タスク開始/終了などの状態遷移（必要に応じて）
- バリデーションは「範囲外なら失敗」でクランプしない。`priority` は 0〜`configMAX_PRIORITIES-1`、`core` は上記許容範囲外の場合 `start` を失敗させログを出す。
- 失敗理由を取得する軽量 API（例: `getLastError()` / `TaskResult`）は将来追加の候補。現状はログと戻り値で判定する。

### ラムダ/ファンクタのメモリと例外
- `startLoop` はラムダ/ファンクタを `std::function<bool()>` で保持する（簡潔さ優先）。ヒープ確保が発生する想定で、メモリ不足で `startLoop` が失敗する可能性がある。
- 例外は非対応（Arduino 環境のため）。タスク本体で例外を投げない前提で実装し、万一投げられても `std::terminate` させるか `catch(...)` でタスク終了に落とす。
- なるべくキャプチャの小さいラムダや関数ポインタを推奨。小型バッファ最適化やアロケータ差し替えなどの最適化は将来検討。

※ 本ライブラリは Arduino 環境専用。ESP-IDF 向けには対応しない。

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

### サンプルコードのコメント方針
- サンプルは英語/日本語を併記する。
- 行の前に書く場合は言語ごとに分ける（例: `// en: ...` の次行に `// ja: ...`）。
- 行末に書く場合は 1 行で区切る（例: `// en: ... / ja: ...`）。

---

## 7. 設計ポリシー補足

- requestStop による協調停止
- デストラクタで強制終了しない
- ISR から呼ばない（同期は AutoSync を使う）
- suspend/resume は提供しない（停止は requestStop と協調終了で行う）

---

## 8. 将来拡張

- TaskGroup
- join() / await()
- ランタイム統計
- キャンセル機構

---

## 9. ライセンス

（MIT など）
