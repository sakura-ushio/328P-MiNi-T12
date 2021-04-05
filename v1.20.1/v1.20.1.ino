/*项目代码修改自 甘草酸不酸 开源https://github.com/jie326513988/mini-T12
 * PCB及BOM开源链接https://github.com/zxcvbnm1421899466/328P-MiNi-T12
 * _樱下小渚花田汐_ https://space.bilibili.com/34620092 修改
 * 编码器控制改为按键控制
 * 已改为适配V1.8单运放原理图
 * 屏幕驱动可改为SSD1316的0.87寸OLED
*/

// 默认支持深圳头，更换其他厂商的头需要自行校准温度曲线
// 若内存不足无法编译，检查是否为UNO的板子
// 并按照下面的链接修改U8G2库
// U8G2优化内存 https://github.com/olikraus/u8g2/wiki/u8g2optimization
// 本项目屏蔽了U8G2_WITH_CLIP_WINDOW_SUPPORT、U8G2_WITH_FONT_ROTATION、U8G2_WITH_INTERSECTION
// V1.10以下的版本不适合V2.2的PCB


/*程序更新说明
   v1.20
    1.主界面温度添加线性化动画
    2.修改蜂鸣器端口配置策略，停止后改为输入模式
    3.缩小字库删减不需要的字符并将字库放在本地调用
    4.休眠时间可以关闭，小于30秒为关闭
   v1.11 优化曲线和内存，自动获取设置温度上下限代码合并成自定义函数
   v1.10 适配PCBV2.2版本，温度范围40-450，延长ADC HIGH报警时间阈值

   //以下版本不适合V2.2的PCB
   v1.03
    1.增大旋转的音调音量，修改进入和退出设置界面的音调音量
    2.降低烙铁唤醒烙铁的灵敏度
    3.优化pid，并不是很明显
   v1.02
    1.新增停止时记录当前的设置温度（休眠时也会自动保存）
   v1.01
    1.调节PID运行间隔
    2.降低PWM频率为15HZ，手柄和mos的声音几乎没有了！之前为61HZ，声音还是有一点的。
   v1.00（此版本需要更新U8G2库）
    1.缩小一点点的内存
    2.修复T12 ADC HIGH报警时无法进入息屏的BUG
   v0.99
    1.开机就加热的模式下PWM动画会向下鬼畜，所以将开机过渡动画改为向上过渡，这样就看不到鬼畜啦
    2.开机加热模式的选择改为按下确认键就切换模式，取消原实心选框旋转选择
    3.添加只有在主界面下才能清除T12 ADC 超出读取读取范围的提示
   v0.98
    1.温度曲线适配最新版本的PCB
   v0.97
    1.优化校准界面的PID策略
   v0.96
    1.优化内存
   v0.95
    1.平滑的开机过度效果
   v0.94
    1.曲线拟合改为3项式，以更加准确的绘制温度曲线
   v0.93
    1.动态PID
    2.取消电流传感器，主界面取消电流显示改为显示PWM百分比
    3.设置菜单增加曲线拟合校准
    4.休眠计时和息屏倒计时改用定时器2计时
    5.息屏显示，无加热无操作3分钟后进入，息屏时显示环境温度
    6.开机提示音和开机画面
    7.休眠时自动保存当前设置温度到eeprom
    8.待机功耗8.8ma 息屏待机功耗6.4ma
*/
/*
  按键功能定义
    长按操作（5下短音最后1下长音）
      主界面：进入设置界面
      其他界面：退出至主界面（校准界面除外）
    双击操作（2下短音）
      主界面：加热或停止状态切换
      其他界面：无
    单击（1下短音）
      主界面：无
      设置界面：进入二级菜单
      二级菜单：切换数值更改的选中状态（空心选框或实心选框），或确认更改数值，无选框状态则退出至一级菜单
  菜单选项
    PID
      P
      I
      D
    休眠
      休眠时间
      休眠温度
    屏幕
      屏幕亮度
      屏幕方向
      编码器方向
    电源
      基准电压
      电源电压
      低压报警
    校准
      调节曲线1段温度
      调节曲线2段温度
      调节曲线2段温度
      调节曲线3段温度
      运行曲线拟合并校准曲线
    烙铁
      冷端补偿
      开机加热
      重置
*/

//********** 版本号 **********
#define version "v1.20.1"

#include <MsTimer2.h>
#include <EEPROM.h>      //掉电数值存储库
#include <avr/sleep.h>   //休眠库
#include <U8g2lib.h>     //屏幕驱动库
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_2_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);     //SSD1306  0.96寸OLED
//U8G2_SSD1316_128X32_2_HW_I2C u8g2(U8G2_R0,/* reset=*/ U8X8_PIN_NONE);             //SSD1316  0.87寸OLED

//调用字库
#include "font.c"
//声明外部变量
extern const uint8_t chinese_t12[1591] U8G2_FONT_SECTION("chinese19");

//********** 引脚定义 **********
#define SW    4          //按下 ，可更改其他引脚
#define CLK   3          //减，只可修改为2或3
#define DT    2          //加，只可修改为2或3
//除了10号脚不能改，其他都可以改，但模拟引脚只能改模拟引脚（休眠引脚除外）
#define t12_sleep_pin A0    //t12休眠   模拟读取引脚
#define t12_temp_pin  A2    //t12温度   模拟读取引脚
#define tc1047_pin    A3    //机内温度   模拟读取引脚
#define vin_pin       A6    //电源电压   模拟读取引脚
#define t12_pwm_pin   10    //t12加热    pwm输出引脚，定死不可更改
#define buzzer_pin    8     //蜂鸣器     输出引脚
//************ EEPROM地址定义 ************
uint8_t auto_get = 0;                     //自动刷写eeprom的状态存储，判断是否首次开机 0-首次 1-非首次
#define auto_get_eeprom     0             //自动刷写eeprom的状态存储，1位
#define p_eeprom            1             //p地址，4位
#define i_eeprom            p_eeprom+4    //i地址，4位
#define d_eeprom            i_eeprom+4    //d地址，4位
#define sleep_time_eeprom   d_eeprom+4            //休眠时间，4位
#define sleep_temp_eeprom   sleep_time_eeprom+4   //休眠温度，1位
#define oled_ld_eeprom      sleep_temp_eeprom+1   //OLED亮度，1位
#define vcc_refer_eeprom    oled_ld_eeprom+1      //基准电压，4位
#define vin_refer_eeprom    vcc_refer_eeprom+4    //电源电压校准值，4位
#define t12_kj_jr_eeprom    vin_refer_eeprom+4    //是否开机就加热？，1位
#define vin_low_eeprom      t12_kj_jr_eeprom+1    //输入电压低压报警阈值，4位
#define tc1047_refer_eeprom vin_low_eeprom+4      //冷端补偿校准值，1位
#define oled_fx_eeprom      tc1047_refer_eeprom+1 //OELD方向，1位
#define xzbmq_fx_eeprom     oled_fx_eeprom+1      //编码器旋转方向，1位
#define sz_temp0_eeprom     xzbmq_fx_eeprom+1     //深圳头曲线温度校准1段，4位
#define sz_temp1_eeprom     sz_temp0_eeprom+4     //深圳头曲线温度校准2段，4位
#define sz_temp2_eeprom     sz_temp1_eeprom+4     //深圳头曲线温度校准3段，4位
#define sz_temp3_eeprom     sz_temp2_eeprom+4     //深圳头曲线温度校准4段，4位
#define set_temp_eeprom     sz_temp3_eeprom+4     //t12设置温度，4位
//T12及温度相关
boolean t12_switch = 0;      //T12加热开关
boolean t12_kj_jr = 0;       //是否开机就加热？ 0-否 1-是

float t12_ad = 0.0;           //T12即时ADC值
int16_t t12_temp = 0;         //T12即时温度
int16_t t12_temp_display = 0; //T12显示温度

boolean t12_error = 0;       //手柄未接提示
boolean t12_adc_error = 0;   //温度超出测量范围
int16_t set_temp_max;       //自动计算设置的温度最大值
int16_t set_temp_min;       //自动计算设置的温度最小值

int16_t t12_temp_average_cache = 0;     //记录采集温度，缓存用
int16_t t12_temp_average = 0;           //8次内采集到的平均温度，显示用
uint8_t t12_temp_average_count = 0;     //平均温度采集计数
boolean buzzer_temp_average_state = 0;  //到达温度提示声

//float tc1047_ad = 0.0;      //tc1047ADC值
float tc1047_temp = 0.0;      //tc1047温度，机内温度
int8_t tc1047_refer = 30;     //T12环境参考温度校准值

float vcc_refer = 4.98;                //参考电压，稳压芯片输出的电压是多少就填多少
float vin = 0.0;                       //电源电压
float vin_refer = 0.0;                 //输入电压校准值
float vin_low = 3.3;                   //低压报警阈值
boolean vin_error = 0;                 //低压报警提示
#define vcc_refer_fl vcc_refer/1023.0  //参考电压每级分量

uint32_t xzbmq_time = 0;            //编码器消抖对比时间
uint32_t temp_read_time = 0;        //定时读取温度对比时间
uint32_t vin_read_time = 0;         //定时读取输入电压时间
uint32_t t12_temp_read_time = 450;  //定时T12温度采样时间

boolean buzzer_state = 0;      //蜂鸣器准备状态
uint16_t buzzer_time;          //蜂鸣器响的时间
uint16_t buzzer_count;         //蜂鸣器响的次数
boolean buzzer_szjm_state = 0; //进入设置界面的长音状态
boolean buzzer_zjm_state = 0;  //退出至主界面的长音状态

boolean zjm_sleep_ts = 0;        //主界面休眠提示  0-无 1-休眠中
uint16_t sleep_time = 150;       //进入休眠的时间阈值,单位秒
uint8_t sleep_temp = 200;        //休眠时的温度，单位摄氏度
boolean sleep_state = 0;         //休眠引脚状态 0低电平 1高电平
uint16_t sleep_count = 0;        //休眠计数
uint16_t sleep_temp_cache = 0;   //休眠前的温度缓存

uint8_t oled_ld = 50;      //OLED亮度
boolean oled_fx = 0;       //OLED方向 0-不旋转，横向显示 1-旋转180度
boolean xzbmq_fx = 0;      //编码器方向 0-顺加逆减 1-顺减逆加
boolean hfccsz_state = 0;  //恢复出厂设置 0-NO 1-YES

boolean xp_state = 0;           //息屏状态 0-无 1-息屏
uint16_t xp_time_count = 0;     //息屏计数
uint16_t xp_time_count_old = 0; //上一次时间计数
int8_t xp_x = 0;                //息屏随机变化位置X
int8_t xp_y = 0;                //息屏随机变化位置Y
int8_t xp_x_old = 50;           //上一次息屏随机变化位置X
int8_t xp_y_old = 21;           //上一次息屏随机变化位置Y

//深圳头曲线拟合的4段ADC
#define sz_adc0  10
#define sz_adc1  350
#define sz_adc2  650
#define sz_adc3  940         //原950
//uint16_t sz_temp[4] = {36, 224, 347, 446};   //深圳头曲线拟合的4段温度   Y 390倍
//uint16_t sz_temp[4] = {43, 259, 382, 490};   //深圳头曲线拟合的4段温度   Y 330倍
uint16_t sz_temp[4] = {49, 262, 391, 486};   //深圳头曲线拟合的4段温度   Y    1.03版本程序移植
float sz_p[4];                               //深圳头曲线拟合的4个系数   P
//********** 菜单标志位相关 **********
#define ZJM           1     //主界面
#define SZJM          2     //设置界面
#define SZJM_PID      3     //PID设置界面
#define SZJM_SLEEP    4     //休眠设置界面
#define SZJM_OLED     5     //OLED设置界面
#define SZJM_POWER    6     //电源设置界面
#define SZJM_JIAOZHUN 7     //校准设置界面
#define SZJM_LAOTIE   8     //烙铁头设置界面

uint8_t display_count = 1;   //默认要显示的界面
boolean dec_state = 0;       //小数点输入状态 0关闭 1开启
uint8_t qj_ca_count = 0;     //全局长按计数

boolean szjm_kxxk_L = 0;      //设置界面空心选框 顺时针转
boolean szjm_kxxk_R = 0;      //设置界面空心选框 逆时针转
int8_t szjm_kxxk_count = 0;   //设置界面空心选框

int8_t szjm_pid_kxxk_count = 0;   //pid界面空心选框
int8_t szjm_pid_sxxk_count = 0;   //pid界面实心选框

int8_t szjm_sleep_kxxk_count = 0;   //pid界面空心选框
int8_t szjm_sleep_sxxk_count = 0;   //pid界面实心选框

int8_t szjm_oled_kxxk_count = 0;   //oled界面空心选框
int8_t szjm_oled_sxxk_count = 0;   //oled界面实心选框

int8_t szjm_power_kxxk_count = 0;   //电源设置界面空心选框
int8_t szjm_power_sxxk_count = 0;   //电源设置界面实心选框

int8_t szjm_pwm_kxxk_count = 0;     //PWM界面空心选框
int8_t szjm_pwm_sxxk_count = 0;     //PWM界面实心选框

int8_t szjm_laotie_kxxk_count = 0;   //烙铁头设置界面空心选框
int8_t szjm_laotie_sxxk_count = 0;   //烙铁头设置界面实心选框

int8_t szjm_jiaozhun_kxxk_count = 0;   //烙铁头设置界面空心选框
int8_t szjm_jiaozhun_sxxk_count = 0;   //烙铁头设置界面实心选框
//********** 动态调参PID **********
float p = 25.0;                    //比例系数
float i = 8.7;                    //积分系数
float d = 2.6;                     //微分系数

#define p_jz  12.0                //校准界面用的 比例系数
#define i_jz  2.2                //校准界面用的 积分系数
#define d_jz  1.2                //校准界面用的 微分系数

uint16_t set_temp = 320;          //设置温度
int16_t thisError = 0;            //本次误差
int16_t thisError_average = 0;    //累积误差的平均值
int16_t lasterror = 0;            //前一拍偏差
int16_t preerror = 0;             //前两拍偏差
const int8_t deadband = 0;        //死区
int16_t pid_out = 0.0;            //PID控制器计算结果
uint8_t pid_out_bfb = 0.0;        //输出值0-100%
const float pwm_max = 2047.0;     //输出值上限
const float pwm_min = 0.0;        //输出值下限
const float errorabsmax = 2000.0; //偏差绝对值最大值
const float errorabsmin = 50.0;  //偏差绝对值最小值
const float alpha = 0.2;          //不完全微分系数
float deltadiff = 0.0;            //微分增量

void setup()
{
  //***** 首次开机自动刷写eeprom
  eerpom_auto();
  //***** 编码器配置
  pinMode(SW, INPUT_PULLUP);            //按下
  pinMode(CLK, INPUT_PULLUP);           //相位A
  pinMode(DT, INPUT_PULLUP);            //相位B
  pinMode(t12_temp_pin, INPUT);         //t12温度
  pinMode(t12_sleep_pin, INPUT_PULLUP); //t12休眠
  pinMode(vin_pin, INPUT);              //电源电压
  pinMode(tc1047_pin, INPUT);           //机内温度
  pinMode(buzzer_pin, OUTPUT);          //蜂鸣器
  pinMode(t12_pwm_pin, OUTPUT);         //t12加热，不可更改

  digitalWrite(t12_pwm_pin, 0);         //默认关闭
  digitalWrite(buzzer_pin, 0);          //默认关闭
  //***** OLED初始化
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setContrast(oled_ld);  //设置亮度1-255
  //u8g2.setBusClock(400000); //设置I2C通讯速度默认MAX400000
  //u8g2.setPowerSave(0);     //0开启屏幕 1关闭屏幕
  if (oled_fx)u8g2.setDisplayRotation(U8G2_R2);  //旋转180度
  else u8g2.setDisplayRotation(U8G2_R0);         //0度，默认方向
  //***** ADC初始化
  //ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);  // 设置ADC分频 128倍
  ADCSRA |= bit (ADPS1) | bit (ADPS2);    //设置ADC分频 64倍
  ADCSRA |= bit (ADIE);                   // 开启ADC中断
  interrupts();                           // 开启全局中断
  //***** 打开外部中断
  attachInterrupt(digitalPinToInterrupt(CLK), xuan_zhaun, FALLING);
  attachInterrupt(digitalPinToInterrupt(DT), xuan_zhaun, FALLING);

  //***** 播放开机画面和开机声
  kj_display();
  buzzer(600, 200); //时间，次数
  buzzer_run();
  buzzer(400, 300); //时间，次数
  buzzer_run();
  buzzer(130, 1000); //时间，次数
  buzzer_run();
  //读取电压/温度
  for (uint8_t i = 0; i < 10; i++) //读取初始值
  {
    vin_read();
    tc1047_temp_read();
    read_t12_temp();
  }
  //自动获取设置温度的范围
  auto_get_set_temp();
  //***** 开启看门狗
  //set_wdt_mod(1, 7);
  //***** 是否开机加热
  if (t12_kj_jr == 1) t12_switch = 1;
  else t12_switch = 0;
  //***** 开机计算曲线拟合系数
  qxnh_run();
  //Serial.begin(9600); //打开串口调试
}

void loop()
{
  //uint32_t time0 = micros();

  __asm__ __volatile__ ("wdr");  //看门狗复位
  an_xia();       //编码器按下决策
  draw_page();    //刷新屏幕
  buzzer_run();   //蜂鸣器运行决策
  read_timing();  //定时读取任务
  MsTimer2_rw1(); //定时器2任务

  //uint32_t time1 = micros();
  //Serial.println(time1 - time0);
}
