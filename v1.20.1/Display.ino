//********** 显示决策 **********
uint32_t system_sleep_db_time = 0; //系统进入睡眠对比时间
void display()
{
  if (xp_state == 0)  //正常显示
  {
    if (display_count == ZJM) zjm();
    else if (display_count == SZJM) szjm();
    else if (display_count == SZJM_PID) szjm_pid();
    else if (display_count == SZJM_SLEEP) szjm_sleep();
    else if (display_count == SZJM_OLED) szjm_oled();
    else if (display_count == SZJM_POWER) szjm_power();
    else if (display_count == SZJM_LAOTIE) szjm_laotie();
    else if (display_count == SZJM_JIAOZHUN)  szjm_jiaozhun();
  }
  else xpjm(); //显示息屏界面
}

void draw_page(void)
{
  static uint8_t is_next_page = 0;
  // 调用第一页，如果需要
  if ( is_next_page == 0 )
  {
    u8g2.firstPage();
    is_next_page = 1;
  }
  // 画我们的屏幕
  display();
  // 调用下一页
  if ( u8g2.nextPage() == 0 )
  {
    is_next_page = 0;      // 确保调用了第一个页面
  }
}

void display_hfccsz() //恢复出厂设置进度显示界面
{
  uint16_t x_old = 0;
  for (int i = 0 ; i < EEPROM.length() ; i++)
  {
    __asm__ __volatile__ ("wdr");  //防止没清除完因看门狗没喂狗导致重启
    uint16_t x_new = map(i, 0, EEPROM.length(), 0, 100);
    if (x_new - x_old >= 5)
    {
      u8g2.firstPage();
      do {
        u8g2.setCursor(50, 16);
        u8g2.print(x_new);
        u8g2.setCursor(75, 16);
        u8g2.print("%");
        x_old = x_new;
      } while ( u8g2.nextPage() );
    }
    EEPROM.write(i, 0); //清除所有EEPROM
  }
  set_wdt_mod(1, 0); //利用看门狗实现软重启
}

void kj_display() //开机显示版本号
{
  u8g2.firstPage();
  do {
    u8g2.setFont(chinese_t12);
    u8g2.setCursor(50, 21);
    u8g2.print(version);
  } while ( u8g2.nextPage() );
}

void xpjm() //息屏界面
{
  if (xp_x - xp_x_old > 0)
  {
    if (xp_x_old != xp_x) xp_x_old++;
  }
  else if (xp_x - xp_x_old < 0 )
  {
    if (xp_x_old != xp_x) xp_x_old--;
  }

  if (xp_y - xp_y_old > 0)
  {
    if (xp_y_old != xp_y) xp_y_old++;
  }
  else if (xp_y - xp_y_old < 0 )
  {
    if (xp_y_old != xp_y) xp_y_old--;
  }

  u8g2.setFont(chinese_t12);
  u8g2.setCursor(xp_x_old, xp_y_old);
  u8g2.print(tc1047_temp, 1);
  u8g2.setCursor(xp_x_old + 27, xp_y_old - 1);
  u8g2.print("℃");
}

/*u8g2.firstPage();
  do {
  u8g2.setFont(chinese_t12);
  u8g2.setCursor(xp_x, xp_y);
  u8g2.print(tc1047_temp, 1);
  u8g2.setCursor(xp_x + 27, xp_y - 1);
  u8g2.print("℃");
  } while ( u8g2.nextPage() );*/
