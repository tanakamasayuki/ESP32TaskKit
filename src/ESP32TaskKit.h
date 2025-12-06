#pragma once

#include <Arduino.h>
#include <esp_log.h>
#include <new>
#include <functional>
#include <utility>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifndef ARDUINO_LOOP_STACK_SIZE
#define ARDUINO_LOOP_STACK_SIZE 8192
#endif

namespace TaskKit
{

    struct TaskConfig
    {
        const char *name = "TaskKitTask";
        uint32_t stackSize = ARDUINO_LOOP_STACK_SIZE; // 8192 words
        UBaseType_t priority = 2;
        int core = tskNO_AFFINITY;
    };

    class Task
    {
    public:
        Task() noexcept;
        ~Task();

        Task(const Task &) = delete;
        Task &operator=(const Task &) = delete;

        bool start(TaskFunction_t fn,
                   void *arg = nullptr,
                   const TaskConfig &cfg = TaskConfig{});

        template <class F>
        bool startLoop(F &&loopFunc,
                       const TaskConfig &cfg = TaskConfig{},
                       uint32_t periodMs = 1);

        bool isRunning() const noexcept;
        TaskHandle_t handle() const noexcept;

        void requestStop() noexcept;
        bool stopRequested() const noexcept;

    private:
        struct StartContext
        {
            Task *self;
            TaskFunction_t fn;
            void *arg;
        };

        struct LoopContext
        {
            Task *self;
            std::function<bool()> func;
            uint32_t periodMs;
        };

        static void taskEntry(void *ctx);
        static void loopEntry(void *ctx);
        static bool validateCore(int core);
        static bool validatePriority(UBaseType_t priority);
        void onTaskExit() noexcept;

        TaskHandle_t _handle;
        bool _running;
        bool _stopRequested;
    };

    inline constexpr const char *kLogTag = "ESP32TaskKit";

    inline Task::Task() noexcept
        : _handle(nullptr), _running(false), _stopRequested(false) {}

    inline Task::~Task()
    {
        if (_running)
        {
            ESP_LOGW(kLogTag, "destroying running task: %s",
                     _handle ? pcTaskGetName(_handle) : "unknown");
        }
    }

    inline bool Task::start(TaskFunction_t fn, void *arg, const TaskConfig &cfg)
    {
        if (!fn)
        {
            ESP_LOGE(kLogTag, "start failed: null function");
            return false;
        }
        if (_running || _handle != nullptr)
        {
            ESP_LOGW(kLogTag, "start called while running");
            return false;
        }
        if (!validatePriority(cfg.priority))
        {
            ESP_LOGE(kLogTag, "start failed: invalid priority=%u", cfg.priority);
            return false;
        }
        if (!validateCore(cfg.core))
        {
            ESP_LOGE(kLogTag, "start failed: invalid core=%d", cfg.core);
            return false;
        }

        auto ctx = new (std::nothrow) StartContext{this, fn, arg};
        if (!ctx)
        {
            ESP_LOGE(kLogTag, "start failed: alloc StartContext");
            return false;
        }

        _stopRequested = false;
        _running = true;

        BaseType_t rc = xTaskCreatePinnedToCore(
            &Task::taskEntry,
            cfg.name,
            cfg.stackSize,
            ctx,
            cfg.priority,
            &_handle,
            cfg.core);

        if (rc != pdPASS)
        {
            ESP_LOGE(kLogTag, "start failed: xTaskCreate err=%d", rc);
            delete ctx;
            _handle = nullptr;
            _running = false;
            return false;
        }

        ESP_LOGI(kLogTag, "task started: %s", cfg.name);
        return true;
    }

    inline bool Task::isRunning() const noexcept
    {
        return _running;
    }

    inline TaskHandle_t Task::handle() const noexcept
    {
        return _handle;
    }

    inline void Task::requestStop() noexcept
    {
        _stopRequested = true;
    }

    inline bool Task::stopRequested() const noexcept
    {
        return _stopRequested;
    }

    inline void Task::taskEntry(void *ctx)
    {
        StartContext *args = static_cast<StartContext *>(ctx);
        Task *self = args->self;
        TaskFunction_t fn = args->fn;
        void *userArg = args->arg;
        delete args;

        fn(userArg);

        self->onTaskExit();
        vTaskDelete(nullptr);
    }

    inline void Task::loopEntry(void *ctx)
    {
        LoopContext *args = static_cast<LoopContext *>(ctx);
        Task *self = args->self;
        std::function<bool()> loopFunc = std::move(args->func);
        const uint32_t periodMs = args->periodMs;
        delete args;

        TickType_t lastWake = xTaskGetTickCount();
        for (;;)
        {
            if (self->stopRequested())
            {
                break;
            }

            bool cont = loopFunc();
            if (!cont)
            {
                break;
            }

            if (periodMs > 0)
            {
                vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(periodMs));
            }
        }

        self->onTaskExit();
        vTaskDelete(nullptr);
    }

    inline bool Task::validatePriority(UBaseType_t priority)
    {
        return priority < configMAX_PRIORITIES;
    }

    inline bool Task::validateCore(int core)
    {
#if (portNUM_PROCESSORS > 1)
        return core == tskNO_AFFINITY || core == 0 || core == 1;
#else
        return core == tskNO_AFFINITY || core == 0;
#endif
    }

    inline void Task::onTaskExit() noexcept
    {
        _running = false;
        _handle = nullptr;
        _stopRequested = false;
    }

    template <class F>
    bool Task::startLoop(F &&loopFunc,
                         const TaskConfig &cfg,
                         uint32_t periodMs)
    {
        if (_running || _handle != nullptr)
        {
            return false;
        }
        if (!validatePriority(cfg.priority))
        {
            ESP_LOGE(kLogTag, "startLoop failed: invalid priority=%u", cfg.priority);
            return false;
        }
        if (!validateCore(cfg.core))
        {
            ESP_LOGE(kLogTag, "startLoop failed: invalid core=%d", cfg.core);
            return false;
        }

        auto func = std::function<bool()>(std::forward<F>(loopFunc));
        if (!func)
        {
            ESP_LOGE(kLogTag, "startLoop failed: empty functor");
            return false;
        }

        auto ctx = new (std::nothrow) LoopContext{this, std::move(func), periodMs};
        if (!ctx)
        {
            ESP_LOGE(kLogTag, "startLoop failed: alloc LoopContext");
            return false;
        }

        _stopRequested = false;
        _running = true;

        BaseType_t rc = xTaskCreatePinnedToCore(&Task::loopEntry,
                                                cfg.name,
                                                cfg.stackSize,
                                                ctx,
                                                cfg.priority,
                                                &_handle,
                                                cfg.core);

        if (rc != pdPASS)
        {
            ESP_LOGE(kLogTag, "startLoop failed: xTaskCreate err=%d", rc);
            delete ctx;
            _handle = nullptr;
            _running = false;
            return false;
        }

        ESP_LOGI(kLogTag, "loop task started: %s", cfg.name);
        return true;
    }

} // namespace TaskKit
