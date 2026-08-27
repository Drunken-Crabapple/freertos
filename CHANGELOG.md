# 变更日志

## v0.1.0 - 2026-08-27

### 重要修改

- 新增 `05_create_task/README.md`，说明动态创建、静态创建及 CMSIS-RTOS2 创建任务的学习目的。
- 新增 `06_create_task_use_params/README.md`，说明通过结构体参数复用同一个任务入口函数的学习目的。
- 新增 `07_delete_task/README.md`，并将 `07_delete_task` 工程纳入版本管理。

### 关键参数

- `07_delete_task` 中红外码 `0xA8` 用于创建音乐任务，`0xA2` 用于删除音乐任务。
- 音乐任务栈深度为 `128` 个 `StackType_t`，优先级为 `osPriorityNormal`。
- `INCLUDE_vTaskDelete` 已设置为 `1`。

### 编译与测试

- 使用 Keil MDK 重新构建 `07_delete_task`，结果为 `0 Error(s), 15 Warning(s)`，HEX 文件生成成功。
- 当前警告为工程中已有的未使用变量、未使用静态函数及 UART 非空参数检查警告，不影响本次构建完成。

### 尚未完成的实机验证

- 尚未重新烧录验证红外播放键创建音乐任务、开关键删除音乐任务以及蜂鸣器停止动作。
