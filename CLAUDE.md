# ESP32-C3 激光雕刻机 - 项目说明（给 AI 助手）

## 项目简介

基于 ESP32-C3FH4 的紧凑型一体化激光雕刻机，移植自 Grbl_ESP32（原作者 Bart Dring）。
集成 BLE 蓝牙 + Web 控制端，支持无线 G 代码传输和实时轨迹预览。

GitHub: https://github.com/Carl-debug2025/ESP32-C3-Laser-Engraver

## 技术栈

- **芯片**：ESP32-C3FH4（单核 RISC-V 160MHz，内置 Flash）
- **框架**：Arduino + Grbl_ESP32
- **构建工具**：PlatformIO（配置见 `platformio.ini`）
- **上位机**：纯 Web 前端 + BLE 通信（参考 LaserGRBL 的 G 代码生成算法）
- **协议**：GPLv3

## 目录结构（关键路径）

```
.
├── platformio.ini            # PlatformIO 主配置（src_dir = Grbl_Esp32）
├── Grbl_Esp32/               # 固件源码
│   ├── src/
│   │   ├── Machine.h         # 入口：根据芯片自动选择机器配置
│   │   └── Machines/
│   │       └── esp32_c3_write.h   # ⭐ 当前激光雕刻机的引脚和参数定义
│   └── data/                 # SPIFFS 数据（Web 控制端文件）
├── libraries/                # 第三方库
├── G代码生成器.html          # 自研 Web 端 G 代码生成工具
├── README.md                 # 中文项目介绍（面向用户）
└── CLAUDE.md                 # 本文件（面向 AI 助手）
```

## 硬件配置（ESP32-C3 激光雕刻机）

引脚定义在 `Grbl_Esp32/src/Machines/esp32_c3_write.h`，关键映射：

| 功能 | GPIO | 备注 |
|------|------|------|
| X 步进脉冲 | GPIO0 | STEP |
| X 方向 | GPIO4 | DIR |
| Y 步进脉冲 | GPIO1 | STEP |
| Y 方向 | GPIO5 | DIR |
| 步进电机使能 | GPIO10 | 低电平使能 |
| 激光 PWM 输出 | GPIO3 | SPINDLE_OUTPUT_PIN（PWM 0~1000） |

**运动参数**：
- X/Y 最大速度：3000 mm/min
- X/Y 加速度：100 mm/s²
- 默认激光模式：关闭（grbl $32 = 0）
- 限位开关：未启用（DEFAULT_HOMING_ENABLE = 0）

## 构建 / 烧录

```bash
# PlatformIO 编译
pio run

# 编译并烧录
pio run -t upload

# 烧录 SPIFFS 数据（Web 端文件）
pio run -t uploadfs

# 监视串口
pio device monitor
```

## 代码约定

- 引脚定义统一写在 `Machines/*.h`，不要散落在 .cpp 里
- 修改硬件相关参数时优先改 `esp32_c3_write.h`，不要动 Grbl 核心代码
- Git 提交信息用 Conventional Commits：`feat:` `fix:` `docs:` `refactor:` `chore:`

## 当前进度

- [x] ESP32-C3 适配，基础 G 代码运动跑通
- [x] PCB 设计完成
- [x] BLE 通信框架
- [x] Web 控制端（实时轨迹预览）
- [ ] （在这里写你正在做的事）
- [ ] （在这里写下一步要做的）

## AI 工作约束 ⭐ 重要

1. **修改引脚定义**前必须先确认芯片型号（ESP32 vs ESP32-C3 引脚不通用）
2. **不要修改** `Grbl_Esp32/src/` 下的 Grbl 核心源码（Stepper.cpp / Planner.cpp 等）—— 那是上游 Grbl_ESP32 的代码，改了会破坏运动控制
3. **修改 Web 端**（HTML/JS）时记得 `pio run -t uploadfs` 重新烧录文件系统
4. 重要修改前先 `git status` + 提醒我提交存档
5. 中文文档/注释保留中文，不要擅自翻译为英文
6. 不知道怎么做时，**先问我**，不要自己瞎改一通

## 已知坑

- ESP32-C3 是 **RISC-V 单核**，跟 ESP32（双核 Xtensa）的中断/任务行为不同
- `Machine.h` 里有芯片自动判断，加新机型记得也加 `#if defined(...)` 分支
- SPIFFS 烧录前要先编译数据：`platformio run -t buildfs`

## 相关参考

- 上游项目：https://github.com/bdring/Grbl_Esp32
- 项目主页：https://carl-debug2025.github.io/ESP32-C3-Laser-Engraver/
- ESP32-C3 引脚说明：https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf
