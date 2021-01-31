//********** 编码器按键决策 **********
#define xklv_time 120     //切换选框的滤波时间
#define szlv_time 20      //切换选框的滤波时间
#define szjm_qrcs 2;

void xuan_zhaun()  //旋转中断 选框决策
{

  /*Serial.print("DT "); Serial.print(digitalRead(DT));
    Serial.print(" | ");
    Serial.print("CLK "); Serial.print(digitalRead(CLK));
    Serial.print(" | ");
    Serial.print("SW "); Serial.print(digitalRead(SW));
    Serial.println(" ");*/
  if ((millis() - xzbmq_time) < szlv_time) return;
  buzzer(100, 70); //时间，次数
  if (display_count == ZJM)
  {
    zjm_xz_key();
    buzzer_temp_average_state = 0; //准备好到达温度提示声
    sleep_count = 0;    //休眠计数归零
    zjm_sleep_ts = 0;   //提示主界面退出显示休眠状态
    t12_adc_error = 0; //清除温度超出测量范围提示
    xp_state = 0;          //结束息屏
    xp_time_count = 0;     //清除息屏计数
    xp_time_count_old = 0; //清除息屏计数旧
  }
  else if (display_count == SZJM) szjm_xz_key();
  else if (display_count == SZJM_PID) pid_xz_key();
  else if (display_count == SZJM_SLEEP) sleep_xz_key();
  else if (display_count == SZJM_OLED) oled_xz_key();
  else if (display_count == SZJM_POWER) power_xz_key();
  else if (display_count == SZJM_LAOTIE) laotie_xz_key();
  else if (display_count == SZJM_JIAOZHUN) jiaozhun_xz_key();
  xzbmq_time = millis();
}

void an_xia()
{
  if ((millis() - xzbmq_time) < 160) return;
  qj_sw_key(); //全局长按进入设置界面 或 退出至主界面
}
//************************ 全局长按进入设置界面 或 退出至主界面 ************************
void qj_sw_key()
{
  //长按计数增加
  if (digitalRead(SW) == 0)
  {
    qj_ca_count++;  //计数加一
    buzzer(80, 150); //时间，次数

    if (display_count == ZJM) t12_adc_error = 0; //清除温度超出测量范围提示
    xp_state = 0;          //结束息屏
    xp_time_count = 0;     //清除息屏计数
    xp_time_count_old = 0; //清除息屏计数旧

    xzbmq_time = millis();
  }
  //判断按了几次 该做些什么
  if (qj_ca_count >= 5) //进入设置界面
  {
    if (display_count == ZJM)
    {
      display_count = SZJM; //在主界面时进入设置界面
      buzzer_szjm_state = 1;
    }
    else if (display_count != SZJM_JIAOZHUN)
    {
      display_count = ZJM; //在其他界面是退出至主界面
      buzzer_zjm_state = 1;
    }
    qj_ca_count = 0;  //计数清零
  }
  else if ((millis() - xzbmq_time) > 260) //等待一段时间，确定松手了
  {
    //在主界面的按下决策
    if (display_count == ZJM) zjm_sw_key();
    //在设置界面的按下决策
    else if (display_count == SZJM) szjm_sw_key();
    //在PID界面的按下决策
    else if (display_count == SZJM_PID) pid_sw_key();
    //在休眠界面的按下决策
    else if (display_count == SZJM_SLEEP) sleep_sw_key();
    //在OLED亮度界面的按下决策
    else if (display_count == SZJM_OLED) oled_sw_key();
    //在电源校准界面的按下决策
    else if (display_count == SZJM_POWER) power_sw_key();
    //在烙铁界面的按下决策
    else if (display_count == SZJM_LAOTIE) laotie_sw_key();
    //在校准界面的按下决策
    else if (display_count == SZJM_JIAOZHUN) jiaozhun_sw_key();
    qj_ca_count = 0;//计数清零
  }
}
//************************ 主界面 ************************
void zjm_sw_key()
{
  if (qj_ca_count == 2 && vin_error == 0 && t12_error == 0) // 启动或停止加热
  {
    zjm_dh_run = 1;    //允许播放动画 ，动画播放完在DisplayMain里切换T12工作状态
    zjm_sleep_ts = 0;  //提示主界面退出显示休眠状态
    buzzer_temp_average_state = 0; //准备好到达温度提示声
  }
}
void zjm_xz_key() //主界面 旋转
{
  if (digitalRead(DT) == xzbmq_fx)  set_temp += 10;
  else if (digitalRead(CLK) == xzbmq_fx) set_temp -= 10;
  //自动获取设置温度的范围
  set_temp_max = ((sz_temp[3]) / 10 ) * 10;
  set_temp_min = ((sz_temp[0]) / 10) * 10;
  if (set_temp > set_temp_max) set_temp = set_temp_min;
  else if (set_temp < set_temp_min) set_temp = set_temp_max;
}
//************************ 设置界面 ************************
void szjm_sw_key() //设置界面 按下
{
  if (qj_ca_count == 1) //进入二级菜单
  {
    if (szjm_kxxk_count == 0) display_count = SZJM_SLEEP;  //进入休眠设置菜单
    else if (szjm_kxxk_count == -5 || szjm_kxxk_count == 1) display_count = SZJM_PID;   //进入PID设置菜单
    else if (szjm_kxxk_count == -1 || szjm_kxxk_count == 5) display_count = SZJM_OLED;  //进入OELD亮度设置菜单
    else if (szjm_kxxk_count == -2 || szjm_kxxk_count == 4) display_count = SZJM_POWER; //进入电源设置菜单
    else if (szjm_kxxk_count == -4 || szjm_kxxk_count == 2) display_count = SZJM_LAOTIE;//进入烙铁设置菜单
    else if (szjm_kxxk_count == -3 || szjm_kxxk_count == 3)
    {
      //使用比较柔和的pid参数，退出校准界面时记得恢复正常的PID策略
      p = p_jz;   //比例系数
      i = i_jz;   //积分系数
      d = d_jz;   //微分系数
      display_count = SZJM_JIAOZHUN;//进入烙铁设置菜单
    }
  }
}
void szjm_xz_key()  //设置界面 旋转
{
  if (digitalRead(DT) == xzbmq_fx)
  {
    if (szjm_kxxk_L == 0 && szjm_kxxk_R == 0) //顺时针
    {
      szjm_kxxk_L = 1;  //允许播放动画
      szjm_kxxk_count++; //下一项
    }
  }
  else if (digitalRead(CLK) == xzbmq_fx)
  {
    if (szjm_kxxk_L == 0 && szjm_kxxk_R == 0) //逆时针
    {
      szjm_kxxk_R = 1; //允许播放动画
      szjm_kxxk_count--; //上一项
    }
  }
}
//************************ PID设置界面 ************************
void pid_sw_key()  //设置界面 按下
{
  if (qj_ca_count == 1) //返回或进入调节框
  {
    switch (szjm_pid_kxxk_count)
    {
      case 0: //返回至设置界面
        display_count = SZJM;
        break;
      case 1: //p 存储
        if (szjm_pid_sxxk_count == 0) szjm_pid_sxxk_count = szjm_pid_kxxk_count;
        else if (szjm_pid_sxxk_count == szjm_pid_kxxk_count && dec_state == 0) dec_state = 1;
        else if (szjm_pid_sxxk_count == szjm_pid_kxxk_count && dec_state == 1)
        {
          EEPROM.put(p_eeprom, p);
          dec_state = 0;
          szjm_pid_sxxk_count = 0;
        }
        break;
      case 2: //i 存储
        if (szjm_pid_sxxk_count == 0) szjm_pid_sxxk_count = szjm_pid_kxxk_count;
        else if (szjm_pid_sxxk_count == szjm_pid_kxxk_count && dec_state == 0) dec_state = 1;
        else if (szjm_pid_sxxk_count == szjm_pid_kxxk_count && dec_state == 1)
        {
          EEPROM.put(i_eeprom, i);
          dec_state = 0;
          szjm_pid_sxxk_count = 0;
        }
        break;
      case 3: //d 存储
        if (szjm_pid_sxxk_count == 0) szjm_pid_sxxk_count = szjm_pid_kxxk_count;
        else if (szjm_pid_sxxk_count == szjm_pid_kxxk_count && dec_state == 0) dec_state = 1;
        else if (szjm_pid_sxxk_count == szjm_pid_kxxk_count && dec_state == 1)
        {
          EEPROM.put(d_eeprom, d);
          dec_state = 0;
          szjm_pid_sxxk_count = 0;
        }
        break;
    }
  }
}
void pid_xz_key()  //设置界面 旋转
{
  if (szjm_pid_sxxk_count == 0) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)  szjm_pid_kxxk_count++;
    else if (digitalRead(CLK) == xzbmq_fx)  szjm_pid_kxxk_count--;
  }
  else if (szjm_pid_sxxk_count == 1) //p加减
  {
    if (digitalRead(DT) == xzbmq_fx && dec_state == 0)       p += 1;
    else if (digitalRead(CLK) == xzbmq_fx && dec_state == 0) p -= 1;
    else if (digitalRead(DT) == xzbmq_fx && dec_state == 1)  p += 0.1;
    else if (digitalRead(CLK) == xzbmq_fx && dec_state == 1) p -= 0.1;
  }
  else if (szjm_pid_sxxk_count == 2) //i加减
  {
    if (digitalRead(DT) == xzbmq_fx && dec_state == 0)       i += 1;
    else if (digitalRead(CLK) == xzbmq_fx && dec_state == 0) i -= 1;
    else if (digitalRead(DT) == xzbmq_fx && dec_state == 1)  i += 0.1;
    else if (digitalRead(CLK) == xzbmq_fx && dec_state == 1) i -= 0.1;
  }
  else if (szjm_pid_sxxk_count == 3) //d加减
  {
    if (digitalRead(DT) == xzbmq_fx && dec_state == 0)       d += 1;
    else if (digitalRead(CLK) == xzbmq_fx && dec_state == 0) d -= 1;
    else if (digitalRead(DT) == xzbmq_fx && dec_state == 1)  d += 0.1;
    else if (digitalRead(CLK) == xzbmq_fx && dec_state == 1) d -= 0.1;
  }
  if (szjm_pid_kxxk_count > 3) szjm_pid_kxxk_count = 0;
  else if (szjm_pid_kxxk_count < 0) szjm_pid_kxxk_count = 3;
}
//************************ 休眠设置界面 ************************
void sleep_sw_key()  //设置界面 按下
{
  if (qj_ca_count == 1) //返回或进入调节框
  {
    switch (szjm_sleep_kxxk_count)
    {
      case 0: //返回至设置界面
        display_count = SZJM;
        break;
      case 1: //休眠时间 存储
        if (szjm_sleep_sxxk_count == 0) szjm_sleep_sxxk_count = szjm_sleep_kxxk_count;
        else if (szjm_sleep_sxxk_count == szjm_sleep_kxxk_count)
        {
          EEPROM.put(sleep_time_eeprom, sleep_time);  //休眠时间
          szjm_sleep_sxxk_count = 0;
        }
        break;
      case 2: //休眠温度 存储
        if (szjm_sleep_sxxk_count == 0) szjm_sleep_sxxk_count = szjm_sleep_kxxk_count;
        else if (szjm_sleep_sxxk_count == szjm_sleep_kxxk_count)
        {
          EEPROM.put(sleep_temp_eeprom, sleep_temp);  //休眠温度
          szjm_sleep_sxxk_count = 0;
        }
        break;
    }
  }
}
void sleep_xz_key()  //设置界面 旋转
{
  if (szjm_sleep_sxxk_count == 0) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)  szjm_sleep_kxxk_count++;
    else if (digitalRead(CLK) == xzbmq_fx)  szjm_sleep_kxxk_count--;
  }
  else if (szjm_sleep_sxxk_count == 1) //休眠时间加减
  {
    if (digitalRead(DT) == xzbmq_fx)      sleep_time += 1;
    else if (digitalRead(CLK) == xzbmq_fx) sleep_time -= 1;
  }
  else if (szjm_sleep_sxxk_count == 2) //休眠温度加减
  {
    if (digitalRead(DT) == xzbmq_fx)      sleep_temp += 1;
    else if (digitalRead(CLK) == xzbmq_fx) sleep_temp -= 1;
  }
  if (szjm_sleep_kxxk_count > 2) szjm_sleep_kxxk_count = 0;
  else if (szjm_sleep_kxxk_count < 0) szjm_sleep_kxxk_count = 2;
  else if (sleep_time > 999) sleep_time = 10;
  else if (sleep_time < 10) sleep_time = 999;
}
//************************ OLED亮度设置界面 ************************
void oled_sw_key()  //设置界面 按下
{
  if (qj_ca_count == 1) //返回或进入调节框
  {
    switch (szjm_oled_kxxk_count)
    {
      case 0: //返回至设置界面
        display_count = SZJM;
        break;
      case 1: //OLED亮度 存储
        if (szjm_oled_sxxk_count == 0) szjm_oled_sxxk_count = szjm_oled_kxxk_count;
        else if (szjm_oled_sxxk_count == szjm_oled_kxxk_count)
        {
          EEPROM.put(oled_ld_eeprom, oled_ld);  //OLED亮度
          szjm_oled_sxxk_count = 0;
        }
        break;
      case 2: //OLED亮度 存储
        if (szjm_oled_sxxk_count == 0) szjm_oled_sxxk_count = szjm_oled_kxxk_count;
        else if (szjm_oled_sxxk_count == szjm_oled_kxxk_count)
        {
          EEPROM.put(oled_fx_eeprom, oled_fx);  //OLED方向
          if (oled_fx == 1)u8g2.setDisplayRotation(U8G2_R2);  //旋转180度
          else u8g2.setDisplayRotation(U8G2_R0);              //0度，默认方向
          szjm_oled_sxxk_count = 0;
        }
        break;
      case 3: //旋转编码器方向 存储
        if (szjm_oled_sxxk_count == 0) szjm_oled_sxxk_count = szjm_oled_kxxk_count;
        else if (szjm_oled_sxxk_count == szjm_oled_kxxk_count)
        {
          EEPROM.put(xzbmq_fx_eeprom, xzbmq_fx);  //OLED方向
          EEPROM.get(xzbmq_fx_eeprom, xzbmq_fx);  //OLED方向
          szjm_oled_sxxk_count = 0;
        }
        break;
    }
  }
}

void oled_xz_key()  //设置界面 旋转
{
  if (szjm_oled_sxxk_count == 0) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)  szjm_oled_kxxk_count++;
    else if (digitalRead(CLK) == xzbmq_fx)  szjm_oled_kxxk_count--;
    if (szjm_oled_kxxk_count > 3) szjm_oled_kxxk_count = 0;
    else if (szjm_oled_kxxk_count < 0) szjm_oled_kxxk_count = 3;
  }
  else if (szjm_oled_sxxk_count == 1) //OLED亮度 加减
  {
    if (digitalRead(DT) == xzbmq_fx)       oled_ld += 5;
    else if (digitalRead(CLK) == xzbmq_fx) oled_ld -= 5;
    if (oled_ld > 1 && oled_ld < 5) oled_ld = 5;
    else if (oled_ld == 0)oled_ld = 255;
  }
  else if (szjm_oled_sxxk_count == 2) //OLED方向 状态切换
  {
    if (digitalRead(DT) == xzbmq_fx)       oled_fx = !oled_fx;
    else if (digitalRead(CLK) == xzbmq_fx) oled_fx = !oled_fx;
  }
  else if (szjm_oled_sxxk_count == 3) //旋转编码器方向 状态切换
  {
    if (digitalRead(DT) == xzbmq_fx)       xzbmq_fx = !xzbmq_fx;
    else if (digitalRead(CLK) == xzbmq_fx) xzbmq_fx = !xzbmq_fx;
  }
}
//************************ 电源校准界面 ************************
void power_sw_key()  //设置界面 按下
{
  if (qj_ca_count == 1) //返回或进入调节框
  {
    switch (szjm_power_kxxk_count)
    {
      case 0: //返回至设置界面
        display_count = SZJM;
        break;
      case 1: //基准电压 存储
        if (szjm_power_sxxk_count == 0) szjm_power_sxxk_count = szjm_power_kxxk_count;
        else if (szjm_power_sxxk_count == szjm_power_kxxk_count)
        {
          EEPROM.put(vcc_refer_eeprom, vcc_refer);
          szjm_power_sxxk_count = 0;
        }
        break;
      case 2: //电源电压校准值 存储
        if (szjm_power_sxxk_count == 0) szjm_power_sxxk_count = szjm_power_kxxk_count;
        else if (szjm_power_sxxk_count == szjm_power_kxxk_count)
        {
          EEPROM.put(vin_refer_eeprom, vin_refer);
          szjm_power_sxxk_count = 0;
        }
        break;
      case 3: //低压报警阈值 存储
        if (szjm_power_sxxk_count == 0) szjm_power_sxxk_count = szjm_power_kxxk_count;
        else if (szjm_power_sxxk_count == szjm_power_kxxk_count)
        {
          EEPROM.put(vin_low_eeprom, vin_low);
          szjm_power_sxxk_count = 0;
        }
        break;
    }
  }
}

void power_xz_key()  //设置界面 旋转
{
  if (szjm_power_sxxk_count == 0) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)  szjm_power_kxxk_count++;
    else if (digitalRead(CLK) == xzbmq_fx)  szjm_power_kxxk_count--;
    if (szjm_power_kxxk_count > 3) szjm_power_kxxk_count = 0;
    else if (szjm_power_kxxk_count < 0) szjm_power_kxxk_count = 3;
  }
  else if (szjm_power_sxxk_count == 1) //基准电压 加减
  {
    if (digitalRead(DT) == xzbmq_fx)       vcc_refer += 0.01;
    else if (digitalRead(CLK) == xzbmq_fx) vcc_refer -= 0.01;
  }
  else if (szjm_power_sxxk_count == 2) //电源电压校准值 加减
  {
    if (digitalRead(DT) == xzbmq_fx)       vin_refer += 0.1;
    else if (digitalRead(CLK) == xzbmq_fx) vin_refer -= 0.1;
  }
  else if (szjm_power_sxxk_count == 3) //低压报警阈值 加减
  {
    if (digitalRead(DT) == xzbmq_fx)       vin_low += 0.1;
    else if (digitalRead(CLK) == xzbmq_fx) vin_low -= 0.1;
    if (vin_low > 30)vin_low = 3.2;
    else if (vin_low < 3.2)vin_low = 30;
  }
}

//************************ 烙铁界面 ************************
void laotie_sw_key()  //设置界面 按下
{
  if (qj_ca_count == 1) //返回或进入调节框
  {
    switch (szjm_laotie_kxxk_count)
    {
      case 0: //返回至设置界面
        display_count = SZJM;
        break;
      case 1: //冷端补偿 存储
        if (szjm_laotie_sxxk_count == 0) szjm_laotie_sxxk_count = szjm_laotie_kxxk_count;
        else if (szjm_laotie_sxxk_count == szjm_laotie_kxxk_count)
        {
          EEPROM.put(tc1047_refer_eeprom, tc1047_refer);
          szjm_laotie_sxxk_count = 0;
        }
        break;
      case 2: //开机加热 存储
        if (szjm_laotie_sxxk_count == 0)
        {
          t12_kj_jr = !t12_kj_jr;
          EEPROM.put(t12_kj_jr_eeprom, t12_kj_jr);
        }
        break;
      case 3: //恢复出厂设置 存储
        if (szjm_laotie_sxxk_count == 0) szjm_laotie_sxxk_count = szjm_laotie_kxxk_count;
        else if (szjm_laotie_sxxk_count == szjm_laotie_kxxk_count)
        {
          if (hfccsz_state == 1) display_hfccsz();
          szjm_laotie_sxxk_count = 0;
        }
        break;
    }
  }
}
void laotie_xz_key()  //设置界面 旋转
{
  if (szjm_laotie_sxxk_count == 0) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)  szjm_laotie_kxxk_count++;
    else if (digitalRead(CLK) == xzbmq_fx)  szjm_laotie_kxxk_count--;
    if (szjm_laotie_kxxk_count > 3) szjm_laotie_kxxk_count = 0;
    else if (szjm_laotie_kxxk_count < 0) szjm_laotie_kxxk_count = 3;
  }
  else if (szjm_laotie_sxxk_count == 1) //冷端补偿校准值 加减
  {
    if (digitalRead(DT) == xzbmq_fx)       tc1047_refer += 1;
    else if (digitalRead(CLK) == xzbmq_fx) tc1047_refer -= 1;
    if (tc1047_refer > 99)tc1047_refer = 0;
    else if (tc1047_refer < -99) tc1047_refer = 0;
  }
  else if (szjm_laotie_sxxk_count == 3) //恢复出厂设置 状态切换
  {
    if (digitalRead(DT) == xzbmq_fx)       hfccsz_state = !hfccsz_state;
    else if (digitalRead(CLK) == xzbmq_fx) hfccsz_state = !hfccsz_state;
  }
}
//************************ 校准界面 ************************
void jiaozhun_sw_key()  //设置界面 按下
{
  if (qj_ca_count == 1) //返回或进入调节框
  {
    switch (szjm_jiaozhun_kxxk_count)
    {
      case 0: //返回至设置界面,并恢复正常PID参数
        display_count = SZJM;
        szjm_jiaozhun_kxxk_count = 0;
        EEPROM.get(p_eeprom, p);         //pid-p
        EEPROM.get(i_eeprom, i);         //pid-i
        EEPROM.get(d_eeprom, d);         //pid-d
        break;
      case 1: //深圳校准温度1段 存储
        if (szjm_jiaozhun_sxxk_count == 0)
        {
          szjm_jiaozhun_sxxk_count = szjm_jiaozhun_kxxk_count;
          t12_switch = 1; //打开加热
        }
        else if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
        {
          EEPROM.put(sz_temp0_eeprom, sz_temp[0]);
          szjm_jiaozhun_sxxk_count = 0;
          t12_switch = 0; //关闭加热
        }
        break;
      case 2: //深圳校准温度2段 存储
        if (szjm_jiaozhun_sxxk_count == 0)
        {
          szjm_jiaozhun_sxxk_count = szjm_jiaozhun_kxxk_count;
          t12_switch = 1; //打开加热
        }
        else if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
        {
          EEPROM.put(sz_temp1_eeprom, sz_temp[1]);
          szjm_jiaozhun_sxxk_count = 0;
          t12_switch = 0; //关闭加热
        }
        break;
      case 3: //深圳校准温度3段 存储
        if (szjm_jiaozhun_sxxk_count == 0)
        {
          szjm_jiaozhun_sxxk_count = szjm_jiaozhun_kxxk_count;
          t12_switch = 1; //打开加热
        }
        else if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
        {
          EEPROM.put(sz_temp2_eeprom, sz_temp[2]);
          szjm_jiaozhun_sxxk_count = 0;
          t12_switch = 0; //关闭加热
        }
        break;
      case 4: //深圳校准温度4段 存储
        if (szjm_jiaozhun_sxxk_count == 0)
        {
          szjm_jiaozhun_sxxk_count = szjm_jiaozhun_kxxk_count;
          t12_switch = 1; //打开加热
        }
        else if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
        {
          EEPROM.put(sz_temp3_eeprom, sz_temp[3]);
          szjm_jiaozhun_sxxk_count = 0;
          t12_switch = 0; //关闭加热
        }
        break;
      case 5: //开始曲线拟合系数计算
        qxnh_run(); //计算曲线拟合系数
        szjm_jiaozhun_kxxk_count = 6;
        break;
      case 6: //返回至设置界面,并恢复正常PID参数
        display_count = SZJM;
        szjm_jiaozhun_kxxk_count = 0;
        EEPROM.get(p_eeprom, p);         //pid-p
        EEPROM.get(i_eeprom, i);         //pid-i
        EEPROM.get(d_eeprom, d);         //pid-d
        break;
    }
  }
}
void jiaozhun_xz_key()  //设置界面 旋转
{
  if (szjm_jiaozhun_sxxk_count == 0) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)  szjm_jiaozhun_kxxk_count++;
    else if (digitalRead(CLK) == xzbmq_fx)  szjm_jiaozhun_kxxk_count--;
    if (szjm_jiaozhun_kxxk_count > 6) szjm_jiaozhun_kxxk_count = 0;
    else if (szjm_jiaozhun_kxxk_count < 0) szjm_jiaozhun_kxxk_count = 6;
  }
  else if (szjm_jiaozhun_sxxk_count == 1) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)       sz_temp[0] += 1;
    else if (digitalRead(CLK) == xzbmq_fx) sz_temp[0] -= 1;
  }
  else if (szjm_jiaozhun_sxxk_count == 2) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)       sz_temp[1] += 1;
    else if (digitalRead(CLK) == xzbmq_fx) sz_temp[1] -= 1;
  }
  else if (szjm_jiaozhun_sxxk_count == 3) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)       sz_temp[2] += 1;
    else if (digitalRead(CLK) == xzbmq_fx) sz_temp[2] -= 1;
  }
  else if (szjm_jiaozhun_sxxk_count == 4) //选框移动
  {
    if (digitalRead(DT) == xzbmq_fx)       sz_temp[3] += 1;
    else if (digitalRead(CLK) == xzbmq_fx) sz_temp[3] -= 1;
  }
}
