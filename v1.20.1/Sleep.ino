//********** T12休眠判断决策 **********
void t12_sleep()
{
  if(sleep_time==999) return;
  if (sleep_state != digitalRead(t12_sleep_pin))//烙铁动了，计数清零
  {
    sleep_state = !sleep_state; //状态取反
    sleep_count = 0;   //计数清零
    if (zjm_sleep_ts == 1)
    {
      zjm_sleep_ts = 0;  //提示主界面退出显示休眠状态
      set_temp = sleep_temp_cache;
      sleep_temp_cache = 0;
      buzzer_temp_average_state = 0; //准备好到达温度提示声
      buzzer(150, 300);  //提示一下
    }
  }
  if (sleep_count == sleep_time && zjm_sleep_ts == 0) //达到休眠的阈值，进入休眠
  {
    EEPROM.put(set_temp_eeprom, set_temp); //自动保存当前设置温度
    zjm_sleep_ts = 1;  //提示主界面显示休眠中
    buzzer(150, 300);   //提示一下

    if (set_temp > sleep_temp)         //大于设置的休眠温度
    {
      sleep_temp_cache = set_temp;    //休眠前将温度记录下来，记录和调温
      set_temp = sleep_temp;          //将温度调至休眠温度
    }
    else if (set_temp <= sleep_temp)  //小于等于设置的休眠温度，只记录不调温
      sleep_temp_cache = set_temp;    //休眠前将温度记录下来
  }
  else if (sleep_count > 1800)      //半小时没操作，关闭加热
  {
    MsTimer2::stop();
    set_temp = sleep_temp_cache;
    sleep_temp_cache = 0;
    t12_switch = 0;
    sleep_count = 0;
    zjm_sleep_ts = 0;  //提示主界面退出休眠
    buzzer(150, 1000);   //提示一下
  }
}

//********** 系统休眠判断决策 **********
void system_sleep()
{
  if (xp_time_count >= 180) //无加热无操作超过180秒就显示息屏界面
  {
    if (xp_time_count - xp_time_count_old > 3) //每隔3秒刷新一次位置
    {
      display_count = ZJM;//退出至主界面
      xp_state = 1; //标记息屏状态
      xp_x = random(0, 90);
      xp_y = random(11, 31);
      xp_time_count_old = xp_time_count;
    }
    else if (xp_time_count >= 65530) //防止数值溢出
    {
      xp_time_count = 180;
      xp_time_count_old = 0;
    }
  }
}
