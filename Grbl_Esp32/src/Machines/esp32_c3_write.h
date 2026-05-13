#pragma once
// clang-format off

// ⚠️ 如需将所有 grbl 参数重置为代码中的默认值，
//    取消下行注释后烧录一次，启动后再注释回来重新烧录
// #define FORCE_SETTINGS_RESET

/*
    esp32_c3_write.h
    ESP32-C3 激光雕刻机 配置文件 - 基础版本

    硬件信息:
      - 主控:  ESP32-C3FH4（内置 Flash，单核 RISC-V 160MHz）
      - 驱动:  A4988，设置 16 微步细分
      - 电机:  PL15S-020（18° 步进角，20 步/圈，丝杆导程 3mm）
      - 激光:  PWM 调光模组，5V 输入

    依赖: Grbl_ESP32（原作者 Bart Dring）
*/

// ============ 机器标识 ============
// 串口 $I 命令返回的机器名称，便于多机区分
#define MACHINE_NAME            "ESP32_C3_LASER"

// ============ ESP32-C3 兼容性配置 ============
// ESP32-C3 不支持 RMT 步进 / I2S 步进（这两个是双核 ESP32 的硬件加速特性），
// 这里强制 undef 防止编译报错
#ifdef USE_RMT_STEPS
    #undef USE_RMT_STEPS
#endif

#ifdef USE_I2S_STEPS
    #undef USE_I2S_STEPS
#endif

// ============ 2轴配置 ============
// 启用 X + Y 共 2 轴（激光雕刻机不需要 Z 轴）
// ⚠️ 增加轴数时需同步增加对应轴的引脚定义和参数
#define N_AXIS 2

// 舵机单向旋转间隔（ms），本机未使用舵机，保留默认即可
#define SERVO_ONE_WAY_INTERVAL 5

// ============ X轴配置 ============
// X 轴步进脉冲输出引脚
// ⚠️ GPIO0 是 ESP32-C3 的 strapping pin（启动模式控制），
//    上电瞬间该引脚若为高电平会进入下载模式。
//    本配置依赖驱动板 A4988 默认拉低，实际使用前建议用万用表确认上电瞬间电平。
#define X_STEP_PIN              GPIO_NUM_0
// X 轴方向控制引脚（高电平/低电平决定旋转方向）
#define X_DIRECTION_PIN         GPIO_NUM_4

// ============ Y轴配置 ============
// Y 轴步进脉冲输出引脚
#define Y_STEP_PIN              GPIO_NUM_1
// Y 轴方向控制引脚
#define Y_DIRECTION_PIN         GPIO_NUM_5

// ============ 方向反转 ============
// bit0 = X 轴反转，bit1 = Y 轴反转
// 2 = 0b10 → X 不反，Y 反（修正机械装配带来的方向不一致）
// 调整时机：电机实际转向与坐标系不匹配时改这个掩码
#define DEFAULT_DIRECTION_INVERT_MASK   2 //0b10 X 不反，Y 反

// ============ 电机使能 ============
// 所有步进电机共用一个使能引脚（A4988 的 EN 输入）
// 低电平使能（A4988 默认逻辑），可用 DEFAULT_INVERT_ST_ENABLE 反转
#define STEPPERS_DISABLE_PIN    GPIO_NUM_10

// ============ 激光配置 ============
// 主轴类型设为激光，启用激光专用 PWM 控制逻辑
#define SPINDLE_TYPE            SpindleType::LASER
// 主轴 PWM 输出引脚（兼容传统主轴接口）
#define SPINDLE_OUTPUT_PIN      GPIO_NUM_3
// 激光 PWM 输出引脚（与上面同一个 GPIO，分别给不同模块用作引用）
#define LASER_OUTPUT_PIN        GPIO_NUM_3

// 默认激光模式：0=关闭，1=开启（对应 grbl $32 参数）
// 上电默认关闭，防止误激发；开始雕刻前通过串口发 $32=1 启用
#define DEFAULT_LASER_MODE      0//默认激光关闭 对应grbl $32
// 激光 PWM 最大值（对应 100% 功率）
#define DEFAULT_SPINDLE_RPM_MAX 1000
// 激光 PWM 最小值（对应 0% 功率）
#define DEFAULT_SPINDLE_RPM_MIN 0

// ============ 禁用所有限位和归零功能 ============
// 本机未连接机械限位开关，相关引脚预留位置如下：
// 如以后添加限位：取消下两行注释，并把 DEFAULT_HOMING_ENABLE 改为 1
// #define X_LIMIT_PIN          GPIO_NUM_7
// #define Y_LIMIT_PIN          GPIO_NUM_6

// 是否启用自动归零功能：0=禁用（无限位开关时必须禁用）
#define DEFAULT_HOMING_ENABLE   0
// 是否启用硬限位（碰到限位开关立即停止）：0=禁用
#define DEFAULT_HARD_LIMIT_ENABLE 0
// 是否启用软限位（坐标超出 MAX_TRAVEL 时拒绝运动）：0=禁用
#define DEFAULT_SOFT_LIMIT_ENABLE 0

// ============ 运动参数 ============
// X 轴最大进给速度（单位：mm/min）
// 取值依据：PL15S-020 步进电机在 16 细分下不失步的保守上限
#define DEFAULT_X_MAX_RATE 3000.0 //mm/min
#define DEFAULT_Y_MAX_RATE 3000.0 //mm/min

// X 轴加速度（mm/s²）
// 100 较保守，避免小型激光机机械抖动；雕刻平稳后可逐步上调测试 200~400
#define DEFAULT_X_ACCELERATION 100.0 //mm/s^2
#define DEFAULT_Y_ACCELERATION 100.0 //mm/s^2

// 每毫米所需脉冲数（核心精度参数）
// 计算公式：steps/mm = (步数/圈 × 细分数) ÷ 丝杆导程
// PL15S-020 电机: 18度步进角, 20步/圈, 导程3mm
// A4988 驱动板 16细分: 20 × 16 = 320 微步/圈
// steps/mm = 320 / 3 = 106.67
// ⚠️ 修改硬件（换电机/换驱动细分/换丝杆）必须重新计算
#define DEFAULT_X_STEPS_PER_MM 106.67
#define DEFAULT_Y_STEPS_PER_MM 106.67

// 工作行程上限（单位 mm），超出此值的 G 代码将被软限位拒绝（如启用）
// 修改时需与实际机械结构匹配
#define DEFAULT_X_MAX_TRAVEL 42.0
#define DEFAULT_Y_MAX_TRAVEL 42.0

// ============ 电机使能设置 ============
// 0 = A4988 默认低电平使能；1 = 反转极性为高电平使能
#define DEFAULT_INVERT_ST_ENABLE 0

// ============ 电机空闲时间 ============
// 运动结束后电机持续锁定的时间：
//   255 = 永不断电（持续锁定，精度高但电机发热）
//   N   = 停止 N ms 后断电（节能但可能丢步）
// 激光雕刻推荐 255 保证定位精度
// 255 = 永不断电
#define DEFAULT_STEPPER_IDLE_LOCK_TIME 255

// ============ 禁用安全门 ============
// 本机无安全门开关，undef 上游可能预定义的引脚，防止误启用
#ifdef CONTROL_SAFETY_DOOR_PIN
    #undef CONTROL_SAFETY_DOOR_PIN
#endif
#ifdef ENABLE_SAFETY_DOOR_INPUT_PIN
    #undef ENABLE_SAFETY_DOOR_INPUT_PIN
#endif
