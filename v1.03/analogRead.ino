//********** 读取电压、电流、温度 **********
float vin_cache = 0.0;
uint8_t vin_cache_count = 0;

float tc1047_cache = 0.0;
uint8_t tc1047_cache_count = 0;

#define cycs 10    //时间平均滤波的采样次数 采样周期 = 采样次数*定时执行任务的时间
uint8_t t12_adc_error_count = 0; //t12ADC超出检测范围错误计数
void read_timing() //定时读取电源电压和机内温度
{
  if (millis() - vin_read_time > t12_temp_read_time)
  {
    read_t12_temp();
    vin_read_time = millis();
  }
}
void read_timing2()
{
  vin_read();          //读取电压
  tc1047_temp_read();  //读取室温
  if (szjm_oled_sxxk_count == 1) u8g2.setContrast(oled_ld); //改变亮度
}
void read_t12_temp() //定时读取T12的温度
{
  float t12_ad_nwe = 0.0; //临时AD
  digitalWrite(t12_pwm_pin, 0);
  read_timing2();
  delay(1);
  t12_ad_nwe = denoiseAnalog(t12_temp_pin); //读取即时AD
  //t12_ad_nwe = analogRead(t12_temp_pin); //读取即时AD
  t12_ad += (t12_ad_nwe - t12_ad) * 0.7;  //平滑滤波
  //if (digitalRead(t12_sleep_pin) == 1) t12_ad += 2; //补偿上拉噪声干扰
  //计算温度，温度曲线由校准程序自动计算
  t12_temp = sz_p[3] * t12_ad * t12_ad * t12_ad + sz_p[2] * t12_ad * t12_ad + sz_p[1] * t12_ad + sz_p[0];
  t12_temp += (tc1047_temp - tc1047_refer);  //环境温度补偿

  //记录平均温度
  t12_temp_average_cache += t12_temp;
  t12_temp_average_count++;
  if (t12_temp_average_count == 8)
  {
    t12_temp_average = t12_temp_average_cache >> 3;
    t12_temp_average_cache = 0; //累计缓存温度清零
    t12_temp_average_count = 0; //计数清零
  }
  //运行PID
  pid();

  //手柄检测及超出测量范围检测
  if (t12_switch == 0 && t12_ad >= 1020) //无手柄
  {
    t12_error = 1;
    t12_switch = 0;
  }
  else if (t12_switch == 1 && t12_ad >= 1020) //超出测量范围检测，延时检测
  {
    t12_adc_error_count++;
    if (t12_adc_error_count >= 8)
    {
      MsTimer2::stop(); //停止一次 然后在MsTimer2_rw1()里自动切换定时的中断任务
      t12_adc_error_count = 0; //清除计数
      t12_adc_error = 1;
      t12_switch = 0;
    }
  }
  else if (t12_switch == 1 && t12_ad < 1020)
  {
    t12_adc_error = 0;
    t12_adc_error_count = 0; //清除计数
  }
  else  t12_error = 0;
}

void vin_read()
{
  /*
    串联电阻的中点电压计算公式
    Eout 中点电压
    Vcc 输入电压（两端施加电压）
    R1、R2 两个串联的分压电阻
    Eout = Vcc * R2/(R1+R2) R1靠近电源 R2靠近地
  */
  /*vin = (denoiseAnalog(vin_pin) / 0.1282) * vcc_refer_fl; //计算电源电压 10/68+10=0.1282
  板子实际R2是62k，所以是10/(62+10)=0.1389
    if (vin < vin_low && vin_low >= 3.3) //电压过低提示
    {
    vin_error = 1;
    t12_switch = 0;
    }*/

  vin_cache += (denoiseAnalog(vin_pin) / 0.1389) * vcc_refer_fl;
  vin_cache_count++;
  if (vin_cache_count >= cycs)
  {
    vin = vin_cache / cycs + vin_refer;
    vin_cache_count = 0;
    vin_cache = 0;
    //电压过低提示
    if (vin < vin_low && vin_low >= 3.3)
    {
      vin_error = 1;
      t12_switch = 0;
    }
    else vin_error = 0;
  }
}

void tc1047_temp_read() //读取tc1047温度
{
  //tc1047_temp = ((denoiseAnalog(tc1047_pin) * vcc_refer_fl) - 0.5) * 100;

  tc1047_cache += ((denoiseAnalog(tc1047_pin) * vcc_refer_fl) - 0.5) * 100;
  tc1047_cache_count++;
  if (tc1047_cache_count >= cycs)
  {
    tc1047_temp = tc1047_cache / cycs;
    tc1047_cache_count = 0;
    tc1047_cache = 0;
  }
}

/*uint16_t denoiseAnalog(uint8_t pin)
  {
  uint8_t low, high;
  if (pin >= A0) pin -= A0;             //设置端口
  ADMUX = (0x0F & pin) | bit(REFS0);    //设置基准电压为5V，设置端口
  ADCSRA |= bit (ADEN) | bit (ADIF);    //启用ADC，关闭任何挂起的中断
  set_sleep_mode (SLEEP_MODE_ADC);      //在取样时进行睡眠以降低噪音，需要开启ADC中断进行唤醒
  sleep_mode();                         //进入休眠自动启动转换
  //bitSet(ADCSRA, ADSC);               //ADSC 写1启动AD转换
  while (bitRead(ADCSRA, ADSC));        //等待转换结束
  low  = ADCL;
  high = ADCH;
  bitClear (ADCSRA, ADEN);              // 禁用ADC
  return (high << 8) | low;             //返回数值
  }*/
uint16_t denoiseAnalog (byte port)
{
  uint16_t result = 0;
  if (port >= A0) port -= A0;           //设置端口
  ADMUX = (0x0F & port) | bit(REFS0);   //设置基准电压为5V，设置端口
  ADCSRA |= bit (ADEN) | bit (ADIF);    //启用ADC，关闭任何挂起的中断
  set_sleep_mode (SLEEP_MODE_ADC);      //在取样时进行睡眠以降低噪音，需要开启ADC中断进行唤醒
  for (uint8_t i = 0; i < 16; i++) {    // get 32 readings
    sleep_mode();                       // go to sleep while taking ADC sample
    while (bitRead(ADCSRA, ADSC));      // 等待转换结束
    result += ADC;                      // add them up
  }
  bitClear (ADCSRA, ADEN);              // 禁用ADC
  return (result >> 4);                 // devide by 32 and return value
}
//ADC中断服务
EMPTY_INTERRUPT (ADC_vect);             // 这里没什么可做的
