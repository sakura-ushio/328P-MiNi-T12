//********** 画主界面 **********
uint8_t zjm_x1 = 0;
uint8_t zjm_x2 = 100;
#define zjm_x3  75
#define zjm_x4  zjm_x3+4

#define zjm_y1  7
#define zjm_y2  19
#define zjm_y3  32
#define zjm_y4  14
boolean zjm_dh_run = 0;  //主界面动画状态 0-不允许 1-允许
uint32_t zjm_dh_time1 = 0;  //动画1运转对比时间
uint32_t zjm_dh_time2 = 0;  //动画2运转对比时间
//热、停切换动画变量
uint8_t zjm_dh_x1 = 0;
uint8_t zjm_dh_y1 = 0;
uint8_t zjm_dh_r1 = 1;
//pwm动效动画
uint8_t zjm_dh_l[14];
//开机过度动画y增量
int8_t zjm_dh_y = 32;
void zjm()
{
  //****** 画当前温度及烙铁错误提示
  if (t12_error == 0 && t12_adc_error == 0) //正常画面
  {
    u8g2.setFont(u8g2_font_bubble_tn);
    if (t12_temp >= 99)u8g2.setCursor(0, zjm_dh_y + 18);
    else u8g2.setCursor(10, zjm_dh_y + 18);
    if (t12_switch == 1 && (thisError_average <= 4 && thisError_average >= -4)) u8g2.print(t12_temp_average);
    else u8g2.print(t12_temp);
  }
  else if (t12_adc_error == 1) zjm_error2(); //T12ADC超出测量范围画面
  else if (t12_error == 1) zjm_error3();      //手柄未接显示画面
  //****** 画设置温度进度条
  u8g2.setFont(u8g2_font_saikyosansbold8_8n);
  //自动获取设置温度的范围
  set_temp_max = ((sz_temp[3]) / 10 ) * 10;
  set_temp_min = ((sz_temp[0]) / 10) * 10;
  zjm_x1 = map(set_temp, set_temp_min, set_temp_max, 3, 57);
  u8g2.drawDisc(zjm_x1, zjm_dh_y + 28, 2);//画实心圆
  u8g2.drawBox(0, zjm_dh_y + 30, 61, 2);  //画实心框
  if (set_temp <= 260)u8g2.setCursor(zjm_x1 + 4, zjm_dh_y + 29); //画设置温度
  else u8g2.setCursor(zjm_x1 - 26, zjm_dh_y + 29);   //画设置温度
  u8g2.print(set_temp);

  //****** 画加热提示及切换动画
  u8g2.setFont(chinese15);
  if (zjm_dh_y != 0) //开机过渡动画变量变化
  {
    zjm_dh_y --;
    u8g2.setCursor(50, zjm_dh_y - 11);
    u8g2.print(version);
  }
  if (/*millis() - zjm_dh_time1 > 30 &&*/ zjm_dh_run == 1)
  {
    if (zjm_dh_x1 < 7)
    {
      zjm_dh_x1++;
      zjm_dh_y1++;
      if (zjm_dh_x1 == 7)
      {
        zjm_dh_r1 = 0;
        if (t12_switch == 1 && zjm_sleep_ts == 0) EEPROM.put(set_temp_eeprom, set_temp); //停止时保存当前的设置温度
        t12_switch = !t12_switch; //动画播放完，切换T12工作状态
        MsTimer2::stop(); //停止一次 然后在MsTimer2_rw1()里自动切换定时的中断任务
      }
      zjm_dh_time1 = millis();
    }
    else if (zjm_dh_x1 >= 7 && millis() - zjm_dh_time1 > 300)
    {
      zjm_dh_x1 = 0;
      zjm_dh_y1 = 0;
      zjm_dh_r1 = 1;
      zjm_dh_run = 0; //关闭动画
    }
  }
  u8g2.drawRFrame(zjm_x3 - 3, zjm_dh_y + zjm_y4 - 14, 18, 18, 3); //空心圆角方形
  u8g2.setCursor(zjm_x3, zjm_dh_y + zjm_y4);
  if (t12_switch == 1 && zjm_sleep_ts == 0) u8g2.print("热");
  else if (t12_switch == 1 && zjm_sleep_ts == 1) u8g2.print("休");
  else u8g2.print("停");
  if (zjm_dh_x1 == 7)u8g2.setDrawColor(2);
  u8g2.drawRBox(80 - zjm_dh_x1, zjm_dh_y + 8 - zjm_dh_y1, 2 + zjm_dh_x1 * 2, 2 + zjm_dh_y1 * 2, zjm_dh_r1); //实心圆角方形
  if (zjm_dh_x1 == 7)u8g2.setDrawColor(1);

  //****** 画PWM输出比例动画
  if (t12_switch == 1)
  {
    uint16_t zjm_pwmdh_time = map(pid_out, pwm_min, pwm_max, 450, 150); //输出比例决定刷新时间
    uint8_t zjm_pwmdh_l = map(pid_out, pwm_min, pwm_max, 3, 12); //输出比例决定L的最大值
    if (millis() - zjm_dh_time2 > zjm_pwmdh_time)
    {
      for (uint8_t i = 0; i < 14; i++)
      {
        zjm_dh_l[i] = random(1, zjm_pwmdh_l);
      }
      zjm_dh_time2 = millis();
    }

    for (uint8_t i = 0; i < 14; i++)
    {
      u8g2.drawLine(74 + i, zjm_dh_y + 32, 74 + i, zjm_dh_y + 32 - zjm_dh_l[i]); //绘制垂直线，X,Y,长度
      //delay(100);
    }
  }
  else u8g2.drawHLine(74, zjm_dh_y + 31, 14);

  //****** 画室温-电压-电流-工作状态
  if (vin_error == 1) zjm_error1(); //电压电流有一个报警就显示 报警提示画面
  else zjm_vin_ec_temp_state(); //显示正常画面

  //****** 调试信息显示
  //u8g2.setCursor(75, 32);
  //u8g2.print(zjm_ca_count);
}


void zjm_vin_ec_temp_state() //画室温-电压-电流-工作状态
{
  //画室温-电压
  u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
  u8g2.setCursor(zjm_x2, zjm_dh_y + zjm_y1);
  u8g2.print(tc1047_temp, 1);
  u8g2.drawCircle(zjm_x2 + 21, zjm_dh_y + zjm_y1 - 6, 1, U8G2_DRAW_ALL); //画空心圆
  u8g2.setCursor(zjm_x2 + 23, zjm_dh_y + zjm_y1);
  u8g2.print("C");

  u8g2.setCursor(zjm_x2, zjm_dh_y + zjm_y2);
  u8g2.print(vin, 1);
  u8g2.setCursor(zjm_x2 + 22, zjm_dh_y + zjm_y2);
  u8g2.print("V");

  u8g2.setCursor(zjm_x2, zjm_dh_y + zjm_y3);
  u8g2.print(pid_out_bfb);
  u8g2.setCursor(zjm_x2 + 20, zjm_dh_y + zjm_y3);
  u8g2.print("%");

  //u8g2.setCursor(70, 27);
  //u8g2.print(t12_ad,0);

  //u8g2.setCursor(45, 27);
  //u8g2.print(sleep_count);
  //u8g2.setCursor(65, 27);
  //u8g2.print(xp_time_count);

  /*u8g2.setCursor(55, 13);
    u8g2.print(sleep_MsTimer2_state);*/

  //u8g2.setCursor(50, 27);
  //u8g2.print(pid_out);

  //u8g2.setCursor(0, 29);
  //u8g2.print(t12_temp_read_time);

  //u8g2.setCursor(0, 29);
  //u8g2.print(analogRead(tc1047_pin));
}
void zjm_error1() //主界面 电压电流报警 提示
{
  u8g2.setFont(chinese15);
  if (vin_error == 1)
  {
    u8g2.setCursor(95, zjm_dh_y + 13);
    u8g2.print("电 压");
    u8g2.setCursor(95, zjm_dh_y + 29);
    u8g2.print("过 低");
  }
  u8g2.setDrawColor(2);
  u8g2.drawBox(95 - 2, zjm_dh_y + 0, 40, 32); //实心方形
  u8g2.setDrawColor(1);
}
void zjm_error2() //主界面 温度超出测量范围 错误提示
{
  u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
  u8g2.setCursor(12, zjm_dh_y + 8);
  u8g2.print("T12 ADC");
  u8g2.setCursor(23, zjm_dh_y + 17);
  u8g2.print("HIGH");
  u8g2.setDrawColor(2);
  u8g2.drawBox(0, zjm_dh_y + 0, 61, 18); //实心方形
  u8g2.setDrawColor(1);
}
void zjm_error3() //主界面 手柄未接显示界面 错误提示
{
  u8g2.setFont(chinese15);
  u8g2.setCursor(5, zjm_dh_y + 14);
  u8g2.print("手柄未接");

  u8g2.setDrawColor(2);
  u8g2.drawBox(0, zjm_dh_y + 0, 61, 18); //实心方形
  u8g2.setDrawColor(1);
}
