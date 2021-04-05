//********** 画设置主界面及其他界面 **********
#define szjm_y1 21
int16_t szjm_dh_x[6] = {0, 50, 100, 150, 200, 250};
int16_t szjm_dh_x_count = 0;   //设置界面动画x增量记录

#define szjm_dh_bc 5 //变化步长，只能为5的倍数
void szjm()
{
  //画十字坐标
  //u8g2.drawHLine(0, 15, 128);
  //u8g2.drawVLine(63, 0, 32);
  //********** 线性滚动动画
  if (szjm_kxxk_L == 1) //顺时针滚动
  {
    szjm_dh_x_count += szjm_dh_bc; //记录变化次数
    for (uint8_t i = 0; i < 6; i++) //开始变化
    {
      szjm_dh_x[i] += szjm_dh_bc;  //步长5
    }
    for (uint8_t i = 0; i < 6; i++) //数值限制
    {
      if (szjm_dh_x[i] > 250)szjm_dh_x[i] = -45;
    }
    if (szjm_dh_x_count >= 50) //变化完毕
    {
      szjm_kxxk_L = 0;       //释放状态
      szjm_dh_x_count = 0;   //变化次数清零
      if (szjm_kxxk_count >= 6) szjm_kxxk_count = 0; //数值限制
    }
  }
  else if (szjm_kxxk_R == 1) //逆时针滚动
  {
    szjm_dh_x_count -= szjm_dh_bc; //记录变化次数
    for (uint8_t i = 0; i < 6; i++) //开始变化
    {
      szjm_dh_x[i] -= szjm_dh_bc; //步长5
    }
    for (uint8_t i = 0; i < 6; i++) //数值限制
    {
      if (szjm_dh_x[i] < -150)szjm_dh_x[i] = 145;
    }
    if (szjm_dh_x_count <= -50) //变化完毕
    {
      szjm_kxxk_R = 0;      //释放状态
      szjm_dh_x_count = 0;  //变化次数清零
      if (szjm_kxxk_count <= -6) szjm_kxxk_count = 0; //数值限制
    }
  }
  //**************************************************
  //********** 画文字和框框
  //调试信息显示
  //u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
  //u8g2.setCursor(0, 31);
  //u8g2.print(szjm_dh_x_count);
  //u8g2.setCursor(100, 31);
  //u8g2.print(szjm_kxxk_count);
  //u8g2.setCursor(50, 31);
  //u8g2.print(szjm_dh_x[0]);

  u8g2.setFont(chinese_t12);
  if (szjm_dh_x[0] >= -50 && szjm_dh_x[0] <= 150)
  {
    u8g2.setCursor(szjm_dh_x[0], szjm_y1);
    u8g2.print("PID");
  }
  if (szjm_dh_x[1] >= -50 && szjm_dh_x[1] <= 150)
  {
    u8g2.setCursor(szjm_dh_x[1], szjm_y1);
    u8g2.print("休眠");
  }
  if (szjm_dh_x[2] >= -50 && szjm_dh_x[2] <= 150)
  {
    u8g2.setCursor(szjm_dh_x[2], szjm_y1);
    u8g2.print("屏幕");
  }
  if (szjm_dh_x[3] >= -50 && szjm_dh_x[3] <= 150)
  {
    u8g2.setCursor(szjm_dh_x[3], szjm_y1);
    u8g2.print("电源");
  }
  if (szjm_dh_x[4] >= -50 && szjm_dh_x[4] <= 150)
  {
    u8g2.setCursor(szjm_dh_x[4], szjm_y1);
    u8g2.print("校准");
  }
  if (szjm_dh_x[5] >= -50 && szjm_dh_x[5] <= 150)
  {
    u8g2.setCursor(szjm_dh_x[5], szjm_y1);
    u8g2.print("烙铁");
  }
  u8g2.setDrawColor(2);
  u8g2.drawRBox(43, 6, 40, 20, 1); //空心圆角方形
  u8g2.setDrawColor(1);
}

//********** PID设置界面 **********
#define szjm_pid_x1  2
#define szjm_pid_x2  14

#define szjm_pid_x3  82
#define szjm_pid_x4  94

#define szjm_pid_y1  14
#define szjm_pid_y2  29
void szjm_pid()
{
  u8g2.setCursor(szjm_pid_x1, szjm_pid_y1);
  u8g2.print("P");
  u8g2.setCursor(szjm_pid_x2, szjm_pid_y1);
  u8g2.print(p, 1);

  u8g2.setCursor(szjm_pid_x3, szjm_pid_y1);
  u8g2.print("I");
  u8g2.setCursor(szjm_pid_x4, szjm_pid_y1);
  u8g2.print(i, 1);

  u8g2.setCursor(szjm_pid_x1, szjm_pid_y2);
  u8g2.print("D");
  u8g2.setCursor(szjm_pid_x2, szjm_pid_y2);
  u8g2.print(d, 1);

  //画选框
  switch (szjm_pid_kxxk_count) //空心选框的位置
  {
    case 1:  //p
      if (szjm_pid_sxxk_count == szjm_pid_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_pid_x1 - 2, szjm_pid_y1 - 13, 48, 16); //实心方形
        //画水平线，标识进入小数点模式
        if (dec_state == 1)  u8g2.drawHLine(szjm_pid_x2 + 4 + (get_num_digit(p) * 6), szjm_pid_y1 + 1, 10);
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_pid_x1 - 2, szjm_pid_y1 - 13, 48, 16); //空心方形
      break;
    case 2:  //i
      if (szjm_pid_sxxk_count == szjm_pid_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_pid_x3 - 2, szjm_pid_y1 - 13, 48, 16); //实心方形
        //画水平线，标识进入小数点模式
        if (dec_state == 1)  u8g2.drawHLine(szjm_pid_x4 + 4 + (get_num_digit(i) * 6), szjm_pid_y1 + 1, 10);
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_pid_x3 - 2, szjm_pid_y1 - 13, 48, 16); //空心方形
      break;
    case 3:  //d
      if (szjm_pid_sxxk_count == szjm_pid_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_pid_x1 - 2, szjm_pid_y2 - 13, 48, 16); //实心方形
        //画水平线，标识进入小数点模式
        if (dec_state == 1)  u8g2.drawHLine(szjm_pid_x2 + 4 + (get_num_digit(d) * 6), szjm_pid_y2 + 1, 10);
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_pid_x1 - 2, szjm_pid_y2 - 13, 48, 16); //空心方形
      break;
  }
}

//********** 休眠设置界面 **********
#define szjm_sleep_x1  2
#define szjm_sleep_x2  105
#define szjm_sleep_y1  14
#define szjm_sleep_y2  29
void szjm_sleep()
{
  u8g2.setCursor(szjm_sleep_x1, szjm_sleep_y1);
  u8g2.print("休眠时间 S");
  u8g2.setCursor(szjm_sleep_x2, szjm_sleep_y1);
  if (sleep_time != 999)u8g2.print(sleep_time);
  else u8g2.print("关");

  u8g2.setCursor(szjm_sleep_x1, szjm_sleep_y2);
  u8g2.print("休眠温度 ℃");
  u8g2.setCursor(szjm_sleep_x2, szjm_sleep_y2);
  u8g2.print(sleep_temp);

  //画选框
  switch (szjm_sleep_kxxk_count) //空心选框的位置
  {
    case 1:  //设置温度
      if (szjm_sleep_sxxk_count == szjm_sleep_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_sleep_x1 - 2, szjm_sleep_y1 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_sleep_x1 - 2, szjm_sleep_y1 - 13, 128, 16); //空心方形
      break;
    case 2:  //设置温度
      if (szjm_sleep_sxxk_count == szjm_sleep_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_sleep_x1 - 2, szjm_sleep_y2 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_sleep_x1 - 2, szjm_sleep_y2 - 13, 128, 16); //空心方形
      break;
  }
}

//********** OLED亮度设置界面 **********
#define szjm_oled_x1  2
#define szjm_oled_x2  106
#define szjm_oled_x3  86
#define szjm_oled_y1  14
#define szjm_oled_y2  29
void szjm_oled()
{
  if (szjm_oled_kxxk_count == 3) //第2页
  {
    u8g2.setCursor(szjm_oled_x1, szjm_oled_y1);
    u8g2.print("旋转方向");
    u8g2.setCursor(szjm_oled_x3, szjm_oled_y1);
    if (xzbmq_fx == 0) u8g2.print("顺+逆-");
    else u8g2.print("顺-逆+");
  }
  else //第1页
  {
    u8g2.setCursor(szjm_oled_x1, szjm_oled_y1);
    u8g2.print("屏幕亮度");
    u8g2.setCursor(szjm_oled_x2, szjm_oled_y1);
    u8g2.print(oled_ld);

    u8g2.setCursor(szjm_oled_x1, szjm_oled_y2);
    u8g2.print("屏幕方向");
    u8g2.setCursor(szjm_oled_x2, szjm_oled_y2);
    if (oled_fx == 0) u8g2.print("0");
    else u8g2.print("180");
  }
  //画选框
  switch (szjm_oled_kxxk_count) //空心选框的位置
  {
    case 1:  //OLED亮度
      if (szjm_oled_sxxk_count == szjm_oled_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_oled_x1 - 2, szjm_oled_y1 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_oled_x1 - 2, szjm_oled_y1 - 13, 128, 16); //空心方形
      break;
    case 2:  //OLED方向
      if (szjm_oled_sxxk_count == szjm_oled_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_oled_x1 - 2, szjm_oled_y2 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_oled_x1 - 2, szjm_oled_y2 - 13, 128, 16); //空心方形
      break;
    case 3:  //旋转方向
      u8g2.drawFrame(szjm_oled_x1 - 2, szjm_oled_y1 - 13, 128, 16); //空心方形
      break;
  }
}

//********** 电源设置界面 **********
#define szjm_power_x1  2
#define szjm_power_x2  100
#define szjm_power_y1  14
#define szjm_power_y2  29
void szjm_power()
{
  if (szjm_power_kxxk_count == 3) //第2页
  {
    u8g2.setCursor(szjm_power_x1, szjm_power_y1);
    u8g2.print("低压报警 V");
    u8g2.setCursor(szjm_power_x2, szjm_power_y1);
    if (vin_low >= 3.3) u8g2.print(vin_low, 1);
    else if (vin_low < 3.3) u8g2.print("关");
  }
  else  //第1页
  {
    u8g2.setCursor(szjm_power_x1, szjm_power_y1);
    u8g2.print("基准电压 V");
    u8g2.setCursor(szjm_power_x2, szjm_power_y1);
    u8g2.print(vcc_refer);
    u8g2.setCursor(szjm_power_x1, szjm_power_y2);
    u8g2.print("电源电压 V");
    if (vin_refer >= 0)
    {
      u8g2.setCursor(szjm_power_x2 - 8, szjm_power_y2);
      u8g2.print("+");
      u8g2.setCursor(szjm_power_x2, szjm_power_y2);
      u8g2.print(vin_refer);
    }
    else
    {
      u8g2.setCursor(szjm_power_x2 - 6, szjm_power_y2);
      u8g2.print(vin_refer);
    }
  }
  //画选框
  switch (szjm_power_kxxk_count) //空心选框的位置
  {
    case 1:  //基准电压(V)
      if (szjm_power_sxxk_count == szjm_power_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_power_x1 - 2, szjm_power_y1 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_power_x1 - 2, szjm_power_y1 - 13, 128, 16); //空心方形
      break;
    case 2:  //基准电压(V)
      if (szjm_power_sxxk_count == szjm_power_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_power_x1 - 2, szjm_power_y2 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_power_x1 - 2, szjm_power_y2 - 13, 128, 16); //空心方形
      break;
    case 3:  //电源电压(V)
      if (szjm_power_sxxk_count == szjm_power_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_power_x1 - 2, szjm_power_y1 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_power_x1 - 2, szjm_power_y1 - 13, 128, 16); //空心方形
      break;
  }
}
//********** 烙铁设置界面 **********
#define szjm_laotie_x1  2
#define szjm_laotie_x4  105

#define szjm_laotie_y1  14
#define szjm_laotie_y2  29

void szjm_laotie()
{

  if (szjm_laotie_kxxk_count == 3) //第2页
  {
    u8g2.setCursor(szjm_laotie_x1, szjm_laotie_y1);
    u8g2.print("重置");
    u8g2.setCursor(szjm_laotie_x4, szjm_laotie_y1);
    if (hfccsz_state)  u8g2.print("是");
    else u8g2.print("否");
  }
  else //第1页
  {
    u8g2.setCursor(szjm_laotie_x1, szjm_laotie_y1);
    u8g2.print("冷端补偿 ℃");
    if (tc1047_refer <= -10) u8g2.setCursor(szjm_laotie_x4, szjm_laotie_y1);
    else u8g2.setCursor(szjm_laotie_x4, szjm_laotie_y1);
    u8g2.print(tc1047_refer);

    u8g2.setCursor(szjm_laotie_x1, szjm_laotie_y2);
    u8g2.print("开机加热");
    u8g2.setCursor(szjm_laotie_x4, szjm_laotie_y2);
    if (t12_kj_jr)  u8g2.print("是");
    else u8g2.print("否");
  }

  //画选框
  switch (szjm_laotie_kxxk_count) //空心选框的位置
  {
    case 1:  //冷端补偿
      if (szjm_laotie_sxxk_count == szjm_laotie_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_laotie_x1 - 2, szjm_laotie_y1 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_laotie_x1 - 2, szjm_laotie_y1 - 13, 128, 16); //空心方形
      break;
    case 2:  //开机加热
      u8g2.drawFrame(szjm_laotie_x1 - 2, szjm_laotie_y2 - 13, 128, 16); //空心方形
      break;
    case 3:  //恢复出厂设置
      if (szjm_laotie_sxxk_count == szjm_laotie_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_laotie_x1 - 2, szjm_laotie_y1 - 13, 128, 16); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_laotie_x1 - 2, szjm_laotie_y1 - 13, 128, 16); //空心方形
      break;
  }
}
//********** 校准设置界面 **********
#define szjm_jiaozhun_x1  0

#define szjm_jiaozhun_x2  30
#define szjm_jiaozhun_x3  54
#define szjm_jiaozhun_x4  80
#define szjm_jiaozhun_x5  106

#define szjm_jiaozhun_x6  102

#define szjm_jiaozhun_y1  7
#define szjm_jiaozhun_y2  18
#define szjm_jiaozhun_y3  31
void szjm_jiaozhun()
{
  u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
  if (szjm_jiaozhun_kxxk_count == 6)
  {
    u8g2.setCursor(0, 7);
    u8g2.print("P0:");
    u8g2.setCursor(25, 7);
    u8g2.print(sz_p[0], 10);

    u8g2.setCursor(0, 15);
    u8g2.print("P1:");
    u8g2.setCursor(25, 15);
    u8g2.print(sz_p[1], 10);

    u8g2.setCursor(0, 24);
    u8g2.print("P2:");
    u8g2.setCursor(25, 24);
    u8g2.print(sz_p[2], 10);

    u8g2.setCursor(0, 32);
    u8g2.print("P3:");
    u8g2.setCursor(25, 32);
    u8g2.print(sz_p[3], 10);
  }
  else
  {
    u8g2.setCursor(szjm_jiaozhun_x1, szjm_jiaozhun_y1);
    u8g2.print("ADC   10   350  650  950");
    u8g2.setCursor(szjm_jiaozhun_x1, szjm_jiaozhun_y2);
    u8g2.print("Temp");
    u8g2.setCursor(szjm_jiaozhun_x2, szjm_jiaozhun_y2);
    u8g2.print(sz_temp[0]);
    u8g2.setCursor(szjm_jiaozhun_x3, szjm_jiaozhun_y2);
    u8g2.print(sz_temp[1]);
    u8g2.setCursor(szjm_jiaozhun_x4, szjm_jiaozhun_y2);
    u8g2.print(sz_temp[2]);
    u8g2.setCursor(szjm_jiaozhun_x5, szjm_jiaozhun_y2);
    u8g2.print(sz_temp[3]);

    u8g2.drawHLine(0, 22, 128);

    u8g2.setCursor(szjm_jiaozhun_x1, szjm_jiaozhun_y3);
    u8g2.print("Now ADC:");
    u8g2.setCursor(48, szjm_jiaozhun_y3);
    u8g2.print(t12_ad, 0);

    u8g2.setCursor(szjm_jiaozhun_x6, szjm_jiaozhun_y3);
    u8g2.print("Save");
  }
  //画选框
  switch (szjm_jiaozhun_kxxk_count) //空心选框的位置
  {
    case 1:  //深圳头1段温度
      if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_jiaozhun_x2 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_jiaozhun_x2 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //空心方形
      break;
    case 2:  //深圳头2段温度
      if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_jiaozhun_x3 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_jiaozhun_x3 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //空心方形
      break;
    case 3:  //深圳头3段温度
      if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_jiaozhun_x4 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_jiaozhun_x4 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //空心方形
      break;
    case 4:  //深圳头4段温度
      if (szjm_jiaozhun_sxxk_count == szjm_jiaozhun_kxxk_count)
      {
        u8g2.setDrawColor(2);
        u8g2.drawBox(szjm_jiaozhun_x5 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //实心方形
        u8g2.setDrawColor(1);
      }
      else u8g2.drawFrame(szjm_jiaozhun_x5 - 2, szjm_jiaozhun_y2 - 9, 21, 11); //空心方形
      break;
    case 5:  //保存
      u8g2.setDrawColor(2);
      u8g2.drawBox(szjm_jiaozhun_x6 - 2, szjm_jiaozhun_y3 - 8, 28, 9); //实心方形
      u8g2.setDrawColor(1);
      break;
  }
}
