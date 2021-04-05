//定时器2
uint32_t t12_sleep_old_time = 0;
void MsTimer2_rw1()
{
  if (t12_switch == 1) //烙铁启动，打开休眠判断决策
  {
    if (bitRead(TIMSK2, TOIE2) == 0) //检测定时是否有打开 1-打开 0-无
    {
      sleep_count = 0;
      xp_time_count = 0;
      MsTimer2::set(1000, sleep_timing); // 1000ms 运行一次
      MsTimer2::start();                 //打开定时器2计数
    }
    if (millis() - t12_sleep_old_time > 700)
    {
      t12_sleep(); //T12休眠决策
      t12_sleep_old_time = millis();
    }
  }
  else if (t12_switch == 0)
  {
    if (bitRead(TIMSK2, TOIE2) == 0)
    {
      sleep_count = 0;
      xp_time_count = 0;
      MsTimer2::set(1000, xp_timing);    // 1000ms 运行一次
      MsTimer2::start();                 //打开定时器2计数
    }
    system_sleep();  //系统休眠决策
  }
}

void sleep_timing() //休眠定时任务
{
  sleep_count++;   //烙铁没动，计数增加
}

void xp_timing()  //息屏定时任务,校准界面不息屏
{
  if (display_count != SZJM_JIAOZHUN) xp_time_count++; //烙铁没动，计数增加
}
