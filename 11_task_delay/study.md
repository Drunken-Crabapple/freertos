# 两个 Delay 函数学习记录

日期：2026-09-14

课程：[韦东山 FreeRTOS 第 26 集：两个 Delay 函数](https://www.bilibili.com/video/BV1Jw411i7Fz/?p=26)

## 本节核心知识

- `vTaskDelay(N)`：从调用时的 tick 计数计算唤醒目标，适合做完事情后再等待一段时间；循环中的工作耗时会影响下一轮开始时间。
- `vTaskDelayUntil(&preTime, N)`：以先前的时间基准加上间隔 N，计算本次目标，并推进 `preTime`，适合按固定节奏执行周期任务。
- 两者参数的时间单位都是 tick。本工程 `Core/Inc/FreeRTOSConfig.h` 配置 `configTICK_RATE_HZ = 1000`，因此 1 tick = 1 ms；通用写法用 `pdMS_TO_TICKS(500)` 表达 500 ms。
- 延时到期只表示任务恢复就绪，不保证立刻运行。更高优先级任务持续就绪时，当前任务仍需等待 CPU。
- tick 是离散计数：调用发生在两个 tick 中断之间时，实际等待时间会受调用位置影响；调度也可能使返回时间推迟。不能把 `vTaskDelay(N)` 理解成严格至少等待 N 个完整 tick 时长。

## 反复确认、容易混淆的点

### 1. 周期是第二个参数，preTime 是时间基准

```c
/* 简化示意：推荐使用 TickType_t 保存 tick 计数 */
TickType_t preTime = xTaskGetTickCount();

while (1)
{
    ReadSensor();
    vTaskDelayUntil(&preTime, pdMS_TO_TICKS(500));
}
```

第二个参数指定相邻目标时间的间隔。`preTime` 不是周期长度；传入它的地址，是为了让函数更新这个变量。

假设起点为 0、间隔为 15 tick，时间基准依次推进为 `0 → 15 → 30 → 45`。它保存预定目标，不是每次实际获得 CPU 的时间。源码在决定是否阻塞后就更新这个基准，不必等任务实际醒来才更新。

### 2. 为什么只在循环前初始化一次

正常写法让函数沿原来的基准递增。如果每次在做完事情后、调用延时前重新执行：

```c
preTime = xTaskGetTickCount();
vTaskDelayUntil(&preTime, 15);
```

就变成“从现在再等 15 tick”，丢掉原来的周期基准。假设 1 tick = 1 ms，忽略边界误差和调度干扰：第一轮从 0 ms 开始，做事 1 ms，再等 15 ms，到 16 ms；第二轮做事 10 ms，再等 15 ms，到 41 ms。

### 3. 固定周期不等于每次等待相同时间

这是本轮最容易混淆的一点：基准为 1000 ms，周期为 500 ms，目标是 1500 ms，但做完事情、记录 `t1` 时已经到了 1003 ms。

| 调用方式 | 结束等待的时刻 | 本次等待时间 |
| --- | --- | --- |
| `vTaskDelayUntil(&preTime, 500)` | 1500 ms | 1500 − 1003 = **497 ms** |
| `vTaskDelay(500)` | 约 1503 ms | 约 **500 ms** |

以上假设 1 tick = 1 ms，并忽略调度、tick 边界和测量开销。`Until` 的 500 表示目标间隔，不表示每次都重新等待 500 ms。

同理，周期 15 ms 时：第一轮做事 1 ms，等待 14 ms；第二轮做事 10 ms，等待 5 ms。理想情况下各轮从 0、15、30 ms 开始。

### 4. 已经错过目标时刻怎么办

目标为 30 ms，但到 33 ms 才调用 `vTaskDelayUntil()`，这次不再阻塞。`preTime` 仍推进到预定的 30 ms，而不是改成 33 ms；下一次目标为 45 ms。

下一轮从 33 ms 开始，做事 4 ms，到 37 ms 调用，便等待到 45 ms，即等待 8 ms。固定的是目标时间序列；工作超时或调度延迟仍会导致实际执行错过目标。

### 5. 恢复就绪与抢占的区别

A 延时期间处于阻塞态，此时 B 获得 CPU，不叫抢占正在运行的 A，因为 A 本来没有运行。只有 A 正在运行、B 就绪并使 A 让出 CPU 时，才叫抢占 A。

A 延时结束后若 B 优先级更高且一直就绪，A 只是恢复就绪，不能立即从延时函数返回并执行下一行。

## 当前工程实验对应

实际文件：`01_freertos_template/Core/Src/freertos.c`，函数 `LcdPrintTask()`。

```c
/* 当前源码节选 */
preTime = xTaskGetTickCount();
/* 循环中的工作包含：mdelay(cnt & 0x3); */
t1 = system_get_ns();
//vTaskDelay(500);  // 500000000
vTaskDelayUntil(&preTime, 500);
t2 = system_get_ns();
```

- `mdelay(cnt & 0x3)` 通过低两位产生变化的忙等耗时，是重复序列，不是真正的随机数。
- `t2 - t1` 测量延时调用前后经过的时间，单位为纳秒，包含期间的调度影响；它不是整个循环的周期，也不表示 CPU 一直在执行此函数。
- 当前启用 `vTaskDelayUntil()`，`vTaskDelay()` 被注释；仅创建 LCD 的 `task1`。
- 当前源码将 `preTime` 声明为 `BaseType_t`；接口对应的 tick 变量应使用 `TickType_t`，上面的推荐示意采用后者。本次只记录差异，没有改动源码。
- 字幕在约 10:46—10:56 明确说明由第 6 个工程复制修改为第 11 个工程，后续实验内容与本工程一致。

## 验证边界

本次核对了当前源码、tick 配置及课堂讨论内容；仅新增学习文档，没有重新编译、烧录或进行板上测量。课程演示中的测量结果不作为当前本地工程的实机验证结果。
