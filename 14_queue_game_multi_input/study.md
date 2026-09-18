# 第35集：队列实验——旋转编码器

## 数据通路

```text
红外中断 ─────────────────────────→ 挡球板队列 → 挡球板任务
编码器中断 → 编码器队列 → 编码器任务 ──↗
```

- 中断收集数据，任务完成后续处理，再转发到另一个队列，减少中断中的处理工作。
- 编码器任务是普通任务，即使处理的数据来自中断，也使用任务版队列接口。

## 静态创建队列

```c
struct rotary_data {
    int32_t cnt;
    int32_t speed;
};

QueueHandle_t g_xQueueRotary;
static uint8_t g_ucQueueRotaryBuf[10 * sizeof(struct rotary_data)];
static StaticQueue_t g_xQueueRotaryStaticStruct;

// game1_task 内，先创建队列，再创建使用它的任务
g_xQueueRotary = xQueueCreateStatic(
    10,
    sizeof(struct rotary_data),
    g_ucQueueRotaryBuf,
    &g_xQueueRotaryStaticStruct
);
```

| 实参 | 对应形参 | 含义 |
|---|---|---|
| `10` | `UBaseType_t uxQueueLength` | 最多10条数据 |
| `sizeof(struct rotary_data)` | `UBaseType_t uxItemSize` | 每条8字节 |
| `g_ucQueueRotaryBuf` | `uint8_t *pucQueueStorageBuffer` | 数据区首地址；数组提供80字节 |
| `&g_xQueueRotaryStaticStruct` | `StaticQueue_t *pxQueueBuffer` | 队列管理结构的地址 |

- 动态创建由 FreeRTOS 分配内存；静态创建由调用者提供数据区和管理结构，两块内存在队列使用期间都必须有效。
- 数组名在调用中转换成首元素地址；普通结构体变量用 `&` 取地址。
- 容量改为20条，数据数组也必须增至160字节；只改调用参数不会自动扩容，可能越界。管理结构仍只需一个。
- 定义句柄不等于创建队列；成功创建后把返回的句柄保存到 `g_xQueueRotary`。

## 中断发送、任务接收

```c
// 编码器驱动：引用 game1.c 中的同一个句柄，不是重新创建队列
extern QueueHandle_t g_xQueueRotary;

// 中断内
struct rotary_data rdata;
rdata.cnt = g_count;
rdata.speed = g_speed;
xQueueSendFromISR(g_xQueueRotary, &rdata, NULL);

// 编码器任务内：这里的 rdata 是另一个局部变量
struct rotary_data rdata;
xQueueReceive(g_xQueueRotary, &rdata, portMAX_DELAY);
```

- 发送时 `&rdata` 是复制源地址，接收时是复制目标地址；复制长度由创建队列时的元素大小决定。
- 本例队列存整个结构体的副本，不是存它的地址。发送成功后，即使局部变量失效或速度从80改为100，已入队的速度仍是80。
- 任务正在处理旧数据时，中断成功发送的新数据留在队尾，不会直接覆盖任务的 `rdata`；任务下次接收再从队头取出，按 FIFO 处理。
- 接收时已有数据就直接取；队列为空才阻塞。阻塞期间不占 CPU，数据到来后恢复就绪，获得 CPU 后继续执行。
- 本工程 `INCLUDE_vTaskSuspend = 1`，接收中的 `portMAX_DELAY` 表示无限期等待。
- 中断必须用 `FromISR` 接口，不能阻塞；第三个参数传 `NULL` 表示不接收“是否唤醒更高优先级任务”的标志，不代表禁止唤醒任务。

## 任务转发与等待时间

```c
// idata 填好后，编码器任务向挡球板队列发送
xQueueSend(g_xQueuePlatform, &idata, 0);
```

| 实参 | 对应形参 | 含义 |
|---|---|---|
| `g_xQueuePlatform` | `QueueHandle_t xQueue` | 目标队列句柄 |
| `&idata` | `const void *pvItemToQueue` | 待复制数据的地址 |
| `0` | `TickType_t xTicksToWait` | 满队列时不等待，立即返回失败 |

- 成功返回 `pdPASS`；当前代码未检查发送结果，也没有重试。
- 等待值为10：有空位就直接发送；满了最多阻塞10个 tick。期间有空位可以提前唤醒，超时仍无空位则失败，不是固定延时10个 tick。
- 发送等空位，接收等数据；等待值的单位是 tick，不直接等同于毫秒。
- 若只剩2个空位、期间无人接收，以等待值0连续发送4条：前2条成功，后2条失败；本代码不重试，因此后2条丢失。不会覆盖队列中原有数据。
