# 第33集：队列实验——多设备玩游戏（思路）

本节讲设计。13工程已实现红外队列改造，尚未实现编码器队列B和编码器任务。

## 13工程的数据通路

```text
红外中断解析键值 → 挡球板队列 → platform_task接收并处理
```

实际代码：

```c
// game1_task内：先创建队列，再创建使用它的任务
g_xQueuePlatform = xQueueCreate(10, sizeof(struct input_data));
xTaskCreate(platform_task, "platform_task", 128, NULL, osPriorityNormal, NULL);

// 红外中断内发送
xQueueSendToBackFromISR(g_xQueuePlatform, &idata, NULL);

// platform_task内接收
if (pdPASS == xQueueReceive(g_xQueuePlatform, &idata, portMAX_DELAY))
{
    data = idata.val;
    // 处理输入
}
```

- 队列容量为10条，每条大小为 `sizeof(struct input_data)`；创建失败返回 `NULL`，使用前应检查。当前源码未检查创建结果。
- 本工程 `INCLUDE_vTaskSuspend = 1`，`portMAX_DELAY` 用于无限期阻塞等待数据。
- 挡球板等待输入不影响独立的游戏任务；若把阻塞接收放进游戏更新循环，没输入就无法继续更新。

## 易混淆结论

- 循环调用 `xTaskCreate()` 是不断创建新任务，不是重新初始化同一个任务；同名、同入口的任务也有各自的栈和任务控制块。
- 多个任务接收同一队列时，一条消息只会被其中一个成功取走，不会自动广播。
- 队列未创建与队列为空不同：`NULL`句柄无效，可能触发断言或异常；有效但空的队列才可以正常阻塞等待。
- `&idata` 表示复制源或接收目标地址；这里传递的是整个结构体的副本，不是保存该指针。发送后修改原变量，不影响已入队的数据。
- 中断使用 `FromISR` 接口，不能阻塞等待；任务接口可以指定等待时间。当前中断发送未检查返回值，满队列发送失败时，该条输入不会自动重发。

## 本节规划的多输入结构

```text
红外中断 ──────────────────────────→ 队列A → 挡球板任务
编码器中断 → 队列B → 编码器任务处理 → 队列A
```

- 队列B传编码器原始信息；编码器任务根据方向、速度生成移动指令，再发往队列A。转得快就发送更多条指令。
- 规划中各输入来源向队列A发送格式、含义一致的指令；当前13工程仍在接收端解释红外键值，不能直接把任意设备原始数据混入。
- 普通队列不会按消息类型选择接收任务；若两个任务竞争读取混合数据，可能拿走对方需要的消息。`xQueueReceive()`不能指定“只取编码器数据”。
- 编码器也可在中断内完成简单转换后直接写队列A；增加队列B和任务，是为了演示把后续处理移出中断，缩短中断占用时间。
- 编码器任务即使处理中断送来的数据，仍是普通任务，可以被更高优先级任务抢占。
- 摇杆等新设备只要生成相同的移动指令，也能写队列A，复用挡球板任务。

# 第34集：队列实验——红外改造

## 忙等与阻塞

- 原来的 `IRReceiver_Read()` 无数据就返回，任务反复循环查询，仍消耗 CPU。
- 队列为空时，挡球板任务阻塞，不占用 CPU；写入数据后恢复就绪，等调度再运行。
- 队列已有数据时直接读取，不需要等下一次中断。让出无效轮询占用的 CPU 时间，可改善音乐播放。

## 创建队列

```c
// 接口形式；xQueueCreate 实际是宏
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength,
                          UBaseType_t uxItemSize);

struct input_data {
    uint32_t dev;
    uint32_t val;
};

QueueHandle_t g_xQueuePlatform;

// game1_task 内执行
g_xQueuePlatform = xQueueCreate(10, sizeof(struct input_data));
```

- 两个参数分别为最大条数、每条的字节数；成功返回句柄，失败返回 `NULL`。
- 定义句柄不等于创建队列；`xQueueCreate()` 才动态分配并初始化队列。
- 一个结构体是一条数据，`dev` 和 `val` 合占一个队列位置；本例每条8字节，10条的数据区为80字节，另有管理结构开销。
- 上面的结构体补齐了本地 `typedefs.h` 中 `val` 后缺少的分号，仅为笔记示例，未修改源码。

## 任务读队列

```c
BaseType_t xQueueReceive(QueueHandle_t xQueue,
                        void *pvBuffer,
                        TickType_t xTicksToWait);

struct input_data idata;
if (pdPASS == xQueueReceive(g_xQueuePlatform, &idata, portMAX_DELAY))
{
    data = idata.val;
}
```

- 参数：队列句柄、接收目标地址、队列为空时最多等待的 tick 数。
- 成功返回 `pdPASS`，把完整结构体复制到 `idata`，并从队列移除该条数据。
- 本工程 `INCLUDE_vTaskSuspend = 1`，这里的 `portMAX_DELAY` 表示无限期阻塞等待。

## 中断写队列

```c
// 接口形式
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue,
                           const void *pvItemToQueue,
                           BaseType_t *pxHigherPriorityTaskWoken);

// 本地红外驱动的实际写法，与视频的 xQueueSendFromISR 均为队尾发送
idata.dev = datas[0];
idata.val = datas[2];
xQueueSendToBackFromISR(g_xQueuePlatform, &idata, NULL);
```

- 参数：队列句柄、发送数据地址、唤醒更高优先级任务的标志输出指针；视频第三个参数先传 `NULL`，未展开。
- 中断必须使用 `FromISR` 接口，不能阻塞；普通 `xQueueSend()` 即使等待时间设为0，也不能替代中断接口。
- 发送成功返回 `pdPASS`；队列满则立即返回失败，新数据未入队，原有数据保留。
- 不检查返回值不等于必然发送失败；队列满导致失败，而忽略返回值会让失败未被处理。本例不会自动重发，因而丢失该次输入。
- 视频先误用普通发送接口导致卡死，改用 `FromISR` 后再开启音乐任务验证效果。
