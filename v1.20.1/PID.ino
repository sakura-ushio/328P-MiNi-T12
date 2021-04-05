//********** PID算法 **********
//算法来至https://blog.csdn.net/foxclever/article/details/105006205 By foxclever
void pid()
{
  if (t12_switch == 1 && vin_error == 0 && t12_error == 0) //加热开关开启
  {
    //计算设定值与平均温度的差值
    int16_t ek_average = set_temp - t12_temp_average;
    //平均差值小于3，发出声音提示一下
    if (ek_average >= -3 && ek_average <= 3 && buzzer_temp_average_state == 0)
    {
      buzzer(120, 300);
      buzzer_temp_average_state = 1;
    }
    if (display_count == SZJM_JIAOZHUN)
      PIDRegulator(t12_ad);  //在校准界面时使用T12的ADC作为PID的目标值
    else
      PIDRegulator(t12_temp); //在其他界面时使用T12的温度值作为PID的目标值
  }
  else
  {
    pid_out = 0;
    lasterror = 0;
    preerror = 0;
    t12_temp_read_time = 450;
  }
  pwm_1b(pid_out);
  pid_out_bfb = (pid_out / (pwm_max - pwm_min)) * 100;
}

void PIDRegulator(float PV)
{
  float factor = 0.0;       //变积分系数
  float increment = 0.0;
  float pError = 0.0, dError = 0.0, iError = 0.0;
  if (display_count == SZJM_JIAOZHUN) //在校准界面时使用T12的ADC作为PID的目标值 ，计算偏差
  {
    if (szjm_jiaozhun_sxxk_count == 1) thisError = sz_adc0 - PV;
    else if (szjm_jiaozhun_sxxk_count == 2) thisError = sz_adc1 - PV;
    else if (szjm_jiaozhun_sxxk_count == 3) thisError = sz_adc2 - PV;
    else if (szjm_jiaozhun_sxxk_count == 4) thisError = sz_adc3 - PV;
  }
  else   //在其他界面时使用T12的温度值作为PID的目标值 ，计算偏差
  {
    thisError = set_temp - PV; //得到偏差值
    thisError_average = set_temp - t12_temp_average; //得到平均值的偏差值
  }
  //规定采样时间
  if (thisError < -5) t12_temp_read_time = 150;
  else if (thisError >= -5 && thisError <= 10) t12_temp_read_time = 400;
  else if (thisError > 10 && thisError <= 30) t12_temp_read_time = 500;
  else t12_temp_read_time = 800;
  if (thisError <= 30 && thisError > -15)
  {
    if (fabs(thisError) > deadband)
    {
      pError = thisError - lasterror;
      iError = float((thisError + lasterror)) / 2.0;
      dError = thisError - 2 * (lasterror) + preerror;

      //变积分系数获取
      factor = get_factor(thisError, errorabsmax, errorabsmin);

      //计算微分项增量带不完全微分
      deltadiff = d * (1 - alpha) * dError + alpha * deltadiff;

      increment = p * pError + i * factor * iError + deltadiff; //增量计算
    }
    else
    {
      if ((fabs(set_temp - pwm_min) < deadband) && (fabs(PV - pwm_min) < deadband))
      {
        pid_out = pwm_min;
      }
      increment = 0.0;
    }

    pid_out = pid_out + increment;

    /*对输出限值，避免超调和积分饱和问题*/
    if (pid_out >= pwm_max)
    {
      if (thisError <= 0) pid_out = pwm_min;
      else pid_out = pwm_max;
    }
    else if (pid_out <= pwm_min) pid_out = pwm_min;

  }
  else if (thisError > 30) pid_out = pwm_max;
  else if (thisError < -15) pid_out = pwm_min;
  preerror = lasterror; //存放偏差用于下次运算
  lasterror = thisError;
}

float get_factor(float ek0, float absmax, float absmin) //变积分系数获取
{
  float factor1 = 0.0;
  factor1 = map(ek0, absmax, absmin, 0.0, 1.0);
  if (abs(ek0) >= absmax) factor1 = 0.0;
  else if (abs(ek0) <= absmin) factor1 = 1.0;
  return factor1;
}

void pwm_1b(uint16_t val)//自定义PWM，使用相位和频率校正模式，2048级控温
{
  //输出通道B 占空比: (OCR1B+1) / (OCR1A+1)
  //占空比不能大于OCR1A

  if (val >= pwm_max) digitalWrite(t12_pwm_pin, 1);
  else if (val <= pwm_min) digitalWrite(t12_pwm_pin, 0);
  else
  {
    //pinMode(10, OUTPUT); //pin10连接至B通道
    //bitWrite(DDRB, 2, 1);  //设置pin10为输出
    //相位和频率校正PWM,分频比64，频率: 16000000/2*256*(OCR1A+1) = 15HZ
    TCCR1A = B00100001;
    TCCR1B = B00010100;
    OCR1A = pwm_max;  //从0开始数到0CR1A，0-65535，设置周期和频率
    OCR1B = val;   //用来设置比较器,占空比
  }
  //ec_read();        //读取电流
}
/*时钟输出 | Arduino输出Pin | 芯片Pin | Pin 名字
  OC0A       6                12       PD6
  OC0B       5                11       PD5
  OC1A       9                15       PB1
  OC1B       10               16       PB2
  OC2A       11               17       PB3
  OC2B       3                5        PD3

  脉冲生成模式控制位（WGM）：用来设置PWM的模式或CTC模式
  时钟选择位（CS）：设置时钟的预分频
  输出模式控制位（COMnA和COMnB）：使能/禁用/反相 输出A和输出B
  输出比较器（OCRnA和OCRnB）：当计数器等于这两个值时，输出值根据不同的模式进行变化

  快速PWM（0-255 0-255 数完归零）
  相位修正PWM（0-255 255-0 数完倒数）

  8位(定时器0/2)
  CSn0 001 = 1
  CSn1 010 = 8
  CSn2 011= 32
  CSn3 100= 64
  CSn4 101= 128
  CSn5 110= 256
  CSn6 111= 1024
  16位(定时器1)
  CSn0 001 = 1
  CSn1 010 = 8
  CSn2 011= 64
  CSn3 100= 256
  CSn4 101= 1024

  COMnA1 COMnB1 =  占空比AB都可控
  从0数到2047 0时输出高电平
  OCRnA 当数到OCRnA，输出低电平，即A的占空比=OCRnA的值
  OCRnB 当数到OCRnB，输出低电平，即B的占空比=OCRnB的值

  COMnA0 COMnB1 =  A当数到头是反相 占空比A50% B可控
  OCRnA 从0数到OCRnA 0时输出高电平
  OCRnB 当OCRnA=OCRnB，输出低电平
  输出通道B 占空比: (OCR1B+1) / (OCR1A+1)
  占空比不能大于OCR1A
  pinMode(10, OUTPUT); //B
  bitWrite(DDRB, 2, 1); //设置pin10为输出
  快速PWM,分频比64，频率: 16000000/64*(OCR1A+1) = 976HZ
  TCCR1A = B00100011;
  TCCR1B = B00011011;
  相位和频率校正PWM,分频比8，频率: 16000,000/2*8*(OCR1A+1) = 488HZ
  TCCR1A = B00100001;
  TCCR1B = B00010010;
  相位校正PWM,分频比256，频率: 16000000/2*256*(OCR1A+1) = 61HZ
  TCCR1A = B00100011;
  TCCR1B = B00010100;
  OCR1A = 2047; //从0开始数到0CR1A，0-65535，设置周期和频率
  OCR1B = val; //用来设置比较器,占空比*/
