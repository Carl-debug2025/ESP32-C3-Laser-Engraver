# ESP32-C3 激光雕刻机

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![MCU](https://img.shields.io/badge/MCU-ESP32--C3FH4-green)](https://www.espressif.com/en/products/socs/esp32-c3)
[![Framework](https://img.shields.io/badge/Framework-Arduino-orange)](https://www.arduino.cc/)

**中文 | [English](README_EN.md)**

> **一体化微型激光雕刻机解决方案**
>
> 基于 ESP32-C3 的紧凑型激光雕刻机，集成 Web 控制端，支持蓝牙无线操作。

---

## 📌 项目亮点

- ✅ **一体化 PCB 设计**：告别模块化组装，直接可用
- ✅ **ESP32-C3FH4 芯片**：内置 Flash，单核 RISC-V 160MHz
- ✅ **Web 控制端**：无需安装软件，浏览器直接控制
- ✅ **BLE 蓝牙通信**：无线 G 代码传输
- ✅ **实时轨迹预览**：雕刻过程可视化
- ✅ **开源免费**：遵循 GPLv3 许可证

---

## 📸 产品展示

![产品照片](hardware/images/product-front.jpg)

> 一体化 PCB 设计，告别模块化组装，直接可用

---

## 📖 项目起源与致谢

本项目是以下开源项目的衍生作品：

### 下位机固件

**基于 [Grbl_ESP32](https://github.com/bdring/Grbl_Esp32)**
原作者：Bart Dring
许可证：GNU General Public License v3.0

Grbl_ESP32 是 [Grbl](https://github.com/gnea/grbl)（原作者：Sungeun K. Jeon）在 ESP32 平台上的移植版本。本项目在 Grbl_ESP32 基础上进行了以下改进：

- **ESP32-C3 适配**：修改底层驱动以支持 ESP32-C3FH4 芯片
- **硬件配置优化**：针对一体化 PCB 重新设计引脚映射
- **运动参数调优**：适配特定步进电机和丝杆参数

### 上位机软件

**G 代码生成算法参考 [LaserGRBL](https://github.com/arkypita/LaserGRBL)**
原作者：arkypita
许可证：GNU General Public License v3.0

本项目开发了全新的 Web 控制端，核心图像处理算法受 LaserGRBL 启发：

- **智能跳白优化**：快速移动空白区域，提升雕刻效率
- **自适应速度控制**：根据路径复杂度动态调整进给速度
- **实时状态监控**：BLE 通信实现双向数据流

### 本项目的原创贡献

根据 GPLv3 许可证要求，本项目完整开源以下内容：

1. **硬件设计**
   - 一体化 PCB 设计（去模块化）
   - ESP32-C3FH4 最小系统电路
   - A4988 步进驱动集成方案

2. **固件改进**
   - `Grbl_Esp32/src/Machines/esp32_c3_write.h`：C3 芯片专用配置
   - ESP32-C3 RMT/I2S 兼容性修复
   - 禁用硬限位后的 Alarm 状态处理优化

3. **软件创新**
   - `G代码生成器.html`：基于 Web Bluetooth API 的控制端
   - 实时轨迹绘制和状态监控
   - 中文界面和预设参数优化

---

## 📦 快速开始

### 硬件清单

| 组件 | 型号 | 数量 | 备注 |
|------|------|------|------|
| 主控板 | ESP32-C3FH4 | 1 | 自行设计 PCB |
| 步进驱动 | A4988 | 2 | 16 细分模式 |
| 步进电机 | PL15S-020 (18°) | 2 | 光驱拆机电机 |
| 激光模块 | 250-500mW PWM | 1 | 5V TTL 控制 |
| 电源 | 12V 2A | 1 | USB PD 诱骗 |

### 软件编译

```bash
# 1. 克隆仓库
git clone https://github.com/Carl-debug2025/ESP32-C3-Laser-Engraver.git
cd ESP32-C3-Laser-Engraver

# 2. 安装 PlatformIO (VSCode 插件)

# 3. 编译固件
pio run -e esp32c3

# 4. 烧录固件
pio run -e esp32c3 -t upload --upload-port COMx
```

### 首次使用

```bash
# 串口连接 (115200 波特率)
$RST=$        # 重置所有设置
$$            # 查看当前参数
```

---

## 🎮 使用方法

### 方式一：Web 控制端（推荐）

1. 打开 `G代码生成器.html`（Chrome 浏览器）
2. 点击 **🔗 连接蓝牙**，选择 `GRBL_ESP32C3`
3. 点击 **🔓 解锁**（首次开机需要）
4. 上传图片 → 调整参数 → 生成 G 代码
5. 点击 **📤 发送雕刻**

### 方式二：串口命令

```bash
$X            # 解锁报警状态
G92 X0 Y0     # 设置当前位置为原点
G0 X10 Y10    # 快速移动
G1 X20 S800   # 雕刻移动（激光功率 80%）
M5            # 关闭激光
?             # 查询状态
```

---

## ⚙️ 核心参数

| 参数 | 值 | 说明 |
|------|-----|------|
| 工作范围 | 42×42 mm | X/Y 轴最大行程 |
| 定位精度 | ~0.01 mm | 理论分辨率 |
| 最大速度 | 3000 mm/min | 实际推荐 500-1000 |
| 步进分辨率 | 106.67 steps/mm | 20步/圈 × 16细分 ÷ 3mm |
| 激光 PWM | 5kHz, 12-bit | GPIO3 输出 |

### GRBL 关键设置

```bash
$100=106.67   # X轴 steps/mm
$101=106.67   # Y轴 steps/mm
$110=3000     # X轴最大速度 (mm/min)
$111=3000     # Y轴最大速度 (mm/min)
$120=100      # X轴加速度 (mm/s²)
$121=100      # Y轴加速度 (mm/s²)
$130=42       # X轴最大行程 (mm)
$131=42       # Y轴最大行程 (mm)
$3=2          # Y轴方向反转
$1=255        # 电机永不断电
```

---

## 🔧 硬件连接

### 引脚定义

| 功能 | GPIO | 说明 |
|------|------|------|
| X_STEP | GPIO0 | X 轴步进脉冲 |
| X_DIR | GPIO4 | X 轴方向 |
| Y_STEP | GPIO1 | Y 轴步进脉冲 |
| Y_DIR | GPIO5 | Y 轴方向 |
| ENABLE | GPIO10 | 电机使能（低有效） |
| LASER_PWM | GPIO3 | 激光功率控制 |

### 接线示意

```
ESP32-C3FH4                 A4988 (X轴)
┌─────────┐                ┌─────────┐
│ GPIO0   ├───────────────►│ STEP    │
│ GPIO4   ├───────────────►│ DIR     │
│ GPIO10  ├────────────────┤ EN      │
│         │                │         │
│         │   A4988 (Y轴)  │ VMOT ◄──┼─ 12V
│ GPIO1   ├───────────────►│ STEP    │
│ GPIO5   ├───────────────►│ DIR     │
│ GPIO10  ├────────────────┤ EN      │
│         │                │         │
│ GPIO3   ├───────────────►│ PWM     │ 激光模块
└─────────┘                └─────────┘
```

---

## 📁 项目结构

```
├── Grbl_Esp32/              # 下位机固件
│   ├── src/
│   │   ├── Machines/
│   │   │   └── esp32_c3_write.h  # ⭐ C3 专用配置
│   │   ├── Grbl.cpp              # 系统初始化
│   │   ├── GCode.cpp             # G 代码解析
│   │   ├── Protocol.cpp          # 通信协议
│   │   ├── Planner.cpp           # 运动规划
│   │   ├── Stepper.cpp           # 步进控制
│   │   └── WebUI/
│   │       └── BLEConfig.cpp     # BLE 蓝牙服务
│   └── Grbl_Esp32.ino            # Arduino 入口
├── hardware/                # 硬件设计文件
│   ├── schematic.pdf        # 电路原理图
│   ├── bom.csv              # 物料清单
│   ├── gerber/              # Gerber 文件（PCB 打样）
│   └── images/              # 硬件照片
│       ├── pcb_front.jpg
│       ├── pcb_back.jpg
│       └── product-front.jpg
├── G代码生成器.html          # ⭐ Web 控制端
├── LICENSE                  # GPLv3 许可证
├── README.md                # 中文文档
├── README_EN.md             # English Documentation
├── CHANGELOG.md             # 更新日志
└── CONTRIBUTING.md          # 贡献指南
```

---

## ⚠️ 安全注意事项

### 三大禁忌

| ❌ 禁止操作 | 后果 |
|------------|------|
| 通电时拔插电机 | 烧毁 A4988 驱动器 |
| 无散热长时间运行 | 芯片过热 |
| 直视激光光束 | 眼睛永久损伤 |

### 激光安全

- ✅ 佩戴激光防护眼镜（对应波长）
- ✅ 保持通风（雕刻产生烟雾）
- ✅ 准备灭火器（木材可能起火）
- ✅ 预留急停开关

---

## 🐛 故障排除

| 问题 | 可能原因 | 解决方法 |
|------|----------|----------|
| 开机 Alarm 状态 | 禁用限位后的默认状态 | 点击"解锁"即可（正常现象） |
| 电机不动 | EN 引脚电平错误 | 检查 $4 设置 |
| Y 轴方向反 | 配置已反转 | 这是正确配置，无需修改 |
| 位置不准 | steps/mm 错误 | 重新计算 $100/$101 |
| 蓝牙连不上 | ESP32 未重启 | 重新上电 |

---

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request！

### 开发环境

- VSCode + PlatformIO
- ESP-IDF v4.4+
- Chrome 浏览器（Web Bluetooth 支持）

### 代码规范

- 遵循原有 Grbl 代码风格
- 新增功能需添加注释
- GPLv3 许可证要求：修改文件需标注变更

---

## 📜 许可证

本项目遵循 **GNU General Public License v3.0** 许可证。

### 你的权利

- ✅ 自由使用、研究、修改本软件
- ✅ 自由分发本软件及其修改版本

### 你的义务

- ⚠️ **必须保留原始许可证声明**
- ⚠️ **必须标注原作者信息**
- ⚠️ **修改后的版本必须以相同许可证开源**
- ⚠️ **必须提供源代码（或提供获取方式）**

### 原始作品

- **Grbl** - Copyright © 2011-2016 Sungeun K. Jeon
- **Grbl_ESP32** - Copyright © 2018-2021 Bart Dring
- **LaserGRBL** - Copyright © arkypita

详见 [LICENSE](./LICENSE) 文件。

---

## 📚 参考资料

- [Grbl Wiki](https://github.com/gnea/grbl/wiki)
- [Grbl_ESP32 项目](https://github.com/bdring/Grbl_Esp32)
- [LaserGRBL 项目](https://github.com/arkypita/LaserGRBL)
- [ESP32-C3 技术手册](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [A4988 数据手册](https://www.pololu.com/file/0J450/A4988.pdf)

---

## 📧 联系方式

- Issues: [GitHub Issues](https://github.com/Carl-debug2025/ESP32-C3-Laser-Engraver/issues)
- 欢迎提交 PR 和 Issue！

---

**Happy Engraving! 🎉**
