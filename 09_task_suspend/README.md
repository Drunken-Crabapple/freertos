# FreeRTOS 09：挂起与恢复任务

## 教学目的

本工程在任务动态创建、删除和优先级实验的基础上，继续学习 FreeRTOS 的任务挂起与恢复：

- 使用 `vTaskSuspend()` 将指定任务无限期挂起。
- 使用 `vTaskResume()` 让挂起任务重新进入就绪态。
- 理解挂起只暂停任务运行，不会删除任务控制块和任务栈。
- 比较任务挂起、阻塞与删除在状态和资源处理上的区别。
- 使用任务句柄和运行标志记录、控制任务当前状态。

## 工程行为

工程通过红外遥控器控制高优先级音乐任务 `SoundTask`：

| 操作 | 按键值 | 执行动作 | 状态变化 |
| --- | --- | --- | --- |
| 第一次按播放键 | `0xA8` | `xTaskCreate()` | 创建 `SoundTask`，`bRunning = 1` |
| 第二次按播放键 | `0xA8` | `vTaskSuspend()` | 挂起 `SoundTask`，关闭蜂鸣器，`bRunning = 0` |
| 第三次按播放键 | `0xA8` | `vTaskResume()` | 恢复 `SoundTask`，`bRunning = 1` |
| 按电源键 | `0xA2` | `vTaskDelete()` | 删除 `SoundTask`，关闭蜂鸣器并清空任务句柄 |

以后继续按播放键，会在挂起和恢复之间切换；删除任务后再次按播放键，则会重新创建一个新的 `SoundTask`。

## 核心代码

```c
if (xSoundTaskHandle == NULL)
{
    xTaskCreate(PlayMusic, "SoundTask", 128, NULL,
                osPriorityNormal + 1, &xSoundTaskHandle);
    bRunning = 1;
}
else if (bRunning)
{
    vTaskSuspend(xSoundTaskHandle);
    PassiveBuzzer_Control(0);
    bRunning = 0;
}
else
{
    vTaskResume(xSoundTaskHandle);
    bRunning = 1;
}
```

`SoundTask` 的优先级为 `osPriorityNormal + 1`（25）。恢复后它重新进入就绪态，会抢占优先级为 `osPriorityNormal`（24）的普通任务。

## 三种状态操作的区别

| 操作 | 是否保留任务 | 是否保留任务栈和上下文 | 如何重新运行 |
| --- | --- | --- | --- |
| `vTaskDelay()` | 是 | 是 | 延时时间到后自动进入就绪态 |
| `vTaskSuspend()` | 是 | 是 | 必须由其他任务调用 `vTaskResume()` |
| `vTaskDelete()` | 否 | 动态任务资源等待空闲任务回收 | 必须重新调用 `xTaskCreate()` |

挂起任务不会自动复位它控制的外设，因此代码在挂起和删除音乐任务时都显式调用 `PassiveBuzzer_Control(0)` 关闭蜂鸣器。

## 实验步骤

1. 使用 Keil MDK 打开 `01_freertos_template/MDK-ARM/01_freertos_template.uvprojx`。
2. 编译并烧录到 STM32F103C8T6 开发板。
3. 第一次按播放键，观察音乐任务被创建并开始播放。
4. 再按一次播放键，观察任务被挂起且蜂鸣器停止。
5. 第三次按播放键，观察任务恢复并继续运行。
6. 按电源键删除音乐任务，再按播放键验证任务能够重新创建。

## 关键配置

- MCU：STM32F103C8T6（Cortex-M3，72 MHz）
- `SoundTask` 栈深度：128 个 `StackType_t`
- `SoundTask` 优先级：`osPriorityNormal + 1`（25）
- 系统节拍：1 kHz
- FreeRTOS 堆：3072 字节，使用 `heap_4.c`
- `INCLUDE_vTaskSuspend = 1`

## 验证状态

- 已使用 Keil MDK 5.40、Arm Compiler 5.06 update 5 完整构建。
- 构建结果：`0 Error(s), 16 Warning(s)`，HEX 文件生成成功。
- 尚未进行固件烧录和开发板实机验证。

> 本工程为教学示例。编译通过只代表固件能够生成，红外按键、任务挂起/恢复以及蜂鸣器行为仍需在配套开发板上实机验证。
