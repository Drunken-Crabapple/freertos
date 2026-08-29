# FreeRTOS 08：任务优先级

## 教学目的

本工程用于学习 FreeRTOS 的任务优先级与抢占式调度，重点观察以下现象：

- 数值越大的任务优先级越高；高优先级任务进入就绪态后，会抢占正在运行的低优先级任务。
- 相同优先级的任务可以通过时间片轮转获得运行机会。
- 高优先级任务调用 `vTaskDelay()` 进入阻塞态后，低优先级任务会继续运行。
- 如果高优先级任务始终不阻塞，低优先级任务可能长期得不到运行机会。

## 工程中的任务

| 任务 | 创建方式 | 优先级 | 主要功能 |
| --- | --- | ---: | --- |
| `defaultTask` | `osThreadNew()` | `osPriorityNormal`（24） | 读取红外遥控器，创建或删除音乐任务 |
| `LightTask` | `xTaskCreateStatic()` | `osPriorityNormal`（24） | 控制绿色 LED 闪烁 |
| `ColorTask` | `xTaskCreateStatic()` | `osPriorityNormal`（24） | 周期改变全彩 LED 的颜色 |
| `SoundTask` | `xTaskCreate()` | `osPriorityNormal + 1`（25） | 驱动无源蜂鸣器播放音乐 |

`SoundTask` 的优先级比其余三个任务高一级。按下播放键创建该任务后，只要它处于就绪态，就会优先运行；播放音符时调用 `vTaskDelay()` 后，任务进入阻塞态，其他任务才会继续获得 CPU。

## 实验步骤

1. 使用 Keil MDK 打开 `01_freertos_template/MDK-ARM/01_freertos_template.uvprojx`。
2. 编译工程并将程序烧录到 STM32F103C8T6 开发板。
3. 上电后观察绿色 LED 和全彩 LED 的运行状态。
4. 按红外遥控器播放键（键值 `0xA8`），动态创建高优先级 `SoundTask` 并播放音乐。
5. 按电源键（键值 `0xA2`），删除 `SoundTask` 并停止蜂鸣器。

## 建议对比实验

- 将 `SoundTask` 的优先级改回 `osPriorityNormal`，观察它与其他同优先级任务的时间片轮转现象。
- 保持 `SoundTask` 为高优先级，把音乐播放中的 `vTaskDelay()` 临时换成持续占用 CPU 的延时，再观察低优先级任务是否还能及时运行。
- 分别调整 `LightTask`、`ColorTask` 的优先级，比较任务优先级变化对 LED 现象的影响。

## 关键配置

- MCU：STM32F103C8T6（Cortex-M3，72 MHz）
- 调度方式：抢占式调度，`configUSE_PREEMPTION = 1`
- 系统节拍：1 kHz
- 最大优先级数量：56
- FreeRTOS 堆：3072 字节，使用 `heap_4.c`

## 验证状态

- 已使用 Keil MDK 5.40、Arm Compiler 5.06 update 5 完整构建。
- 构建结果：`0 Error(s), 16 Warning(s)`，HEX 文件生成成功。
- 尚未进行固件烧录和开发板实机验证。

> 本工程为教学示例。编译通过只代表代码能够生成固件，红外遥控、LED 和蜂鸣器的最终效果仍需在配套开发板上实机验证。
