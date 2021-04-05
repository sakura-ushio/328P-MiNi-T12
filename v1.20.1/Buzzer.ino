//********** 控制蜂鸣器发声的频率和时间，系统延时法，占用系统速度大 **********
uint32_t error_buzzer_db_time = 0;
void buzzer_run()
{
  pinMode(buzzer_pin, OUTPUT);
  if (buzzer_state == 1)
  {
    for (uint16_t i = 0; i < buzzer_count; i++)
    {
      digitalWrite(buzzer_pin, 1);
      delayMicroseconds(buzzer_time);
      digitalWrite(buzzer_pin, 0);
      if (i < buzzer_count - 1) delayMicroseconds(buzzer_time); //最后一次不需要延时
    }
    buzzer_state = 0;
  }
  //错误提示报警音
  if (vin_error == 1 || t12_adc_error == 1)
  {
    if (millis() - error_buzzer_db_time > 1000)
    {
      buzzer(120, 300);
      error_buzzer_db_time = millis();
    }
  }

  if (buzzer_szjm_state == 1)    //进入设置界面时发出声音提示一下
  {
    buzzer(150, 720);
    buzzer_szjm_state = 0;
  }
  else if (buzzer_zjm_state == 1) //退出至主界面时发出声音提示一下
  {
    buzzer(150, 720);
    buzzer_zjm_state = 0;
  }
  pinMode(buzzer_pin, INPUT);
}

void buzzer(uint16_t time, uint16_t count)
{
  buzzer_state = 1;      //准备好蜂鸣器
  buzzer_time = time;    //设置时间
  buzzer_count = count;  //设置次数
}
//*********************************************************

//****** 控制蜂鸣器发声的频率和时间，内部中断法，占用系统速度小 ******

/*void buzzer(uint16_t time, uint16_t count) //时间单位ms
  {
  if (buzzer_state == 0 && digitalRead(buzzer_pin) == 0) //蜂鸣器停止状态才能设置定时器
  {
    buzzer_count = count;  //设置次数
    Timer1.initialize(time); //设置时间
    Timer1.attachInterrupt(buzzer_isr); //设置定时器中断函数
    Timer1.start(); //启动定时器
  }
  }
  void buzzer_isr()
  {
  buzzer_isr_count++;
  bitWrite(PINB, 0, 1);  //设置pin8为输出 PINX写1翻转状态
  buzzer_state = 1;      //标识蜂鸣器正在工作
  if (buzzer_isr_count >= buzzer_count)
  {
    buzzer_isr_count = 0;  // 清除计数
    buzzer_state = 0; //标识蜂鸣器已停止
    digitalWrite(buzzer_pin, 0); //关闭蜂鸣器
    Timer1.stop();  //停止中断
  }
  }*/
//*********************************************************
uint8_t get_num_digit(uint32_t num)//获取数字的位数
{
  uint8_t count = 0;
  if (num == 0) count = 1;
  while (num != 0)
  {
    // n = n/10
    num /= 10;
    ++count;
  }
  return count;
}
