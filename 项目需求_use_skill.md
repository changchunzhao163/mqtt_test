# 项目需求（基于 Skill）

## Skill 依赖

本项目的编译环境、部署流程、MQTT 服务器等平台信息全部由以下 skill 提供，
不再在此文档中重复：

> **Skill**: `cross-compile-openwrt-mips`
> **注册文件**: `opencode.json`
> **路径**: `.opencode/skills/cross-compile-openwrt-mips.md`

使用此项目前，请确保 OpenCode 已加载该 skill，或通过 `opencode.json`
的 `skills` 数组引用。

---

## 项目功能需求

### 程序名
`system_monitor`

### 描述
目标板上运行的系统监控程序，每 10 秒采集一次系统状态并通过 MQTT 上报，
同时接受远程命令查询即时状态。

### MQTT 主题

| 方向 | 主题 | 说明 |
|------|------|------|
| 发布 | `/system_monitor/{hostname}/status` | 每 10 秒上报 JSON 格式的系统状态 |
| 订阅 | `/system_monitor/{hostname}/cmd` | 接受远程控制命令 |

`{hostname}` 替换为目标板的 `hostname`（通过 `uname -n` 获取）。

### 状态发布格式 (JSON)

```json
{
  "ts": 1712345678,
  "uptime": 123456,
  "load_1m": 0.15,
  "load_5m": 0.10,
  "load_15m": 0.08,
  "mem_used_kb": 28456,
  "mem_total_kb": 65536
}
```

字段含义：
- `ts`: Unix 时间戳
- `uptime`: 系统运行秒数
- `load_1m/5m/15m`: 系统负载（从 `/proc/loadavg` 读取）
- `mem_used_kb / mem_total_kb`: 内存使用（从 `/proc/meminfo` 读取，`MemTotal - MemAvailable`）

### 远程命令

| 命令 | 行为 |
|------|------|
| `ping` | 立即发布一次状态消息作为回复 |
| `reboot` | 向 stderr 打印 `"reboot requested"` 日志，不做实际重启 |

接收到的命令消息需要打印到终端日志：
```
Received cmd [{hostname}/cmd]: {payload}
```

### 实现要求

- 纯 C 语言
- 用 Makefile 编译，Makefile 中引用 skill 的环境变量（`STAGING_DIR`、交叉编译器前缀）
- 生成的可执行文件命名为 `system_monitor`
- 程序启动时打印 `"system_monitor started, hostname=%s"` 到 stderr
- 收到 SIGINT/SIGTERM 时打印 `"system_monitor stopped"` 并退出

### 验证方法

1. 本地运行 `mosquitto_sub -t /system_monitor/+/status -v` 验证状态上报
2. 本地运行 `mosquitto_pub -t /system_monitor/{hostname}/cmd -m ping` 验证命令响应
3. 检查目标板日志确认接收到的命令消息已打印
