//******************* 自动刷写eeprom *******************
void eerpom_auto()
{
  EEPROM.get(auto_get_eeprom, auto_get);  //获取自动刷写的状态
  if (auto_get > 1) auto_get = 0;        //防止eeprom第0位不为0
  if (auto_get == 0) //状态为0开启刷写
  {
    EEPROM.put(p_eeprom, p);         //pid-p
    EEPROM.put(i_eeprom, i);         //pid-i
    EEPROM.put(d_eeprom, d);         //pid-d
    EEPROM.put(sleep_time_eeprom, sleep_time);  //休眠时间
    EEPROM.put(sleep_temp_eeprom, sleep_temp);  //休眠温度
    EEPROM.put(oled_ld_eeprom, oled_ld);        //OLED亮度
    EEPROM.put(vcc_refer_eeprom, vcc_refer);    //基准电压
    EEPROM.put(vin_refer_eeprom, vin_refer);    //电源电压校准值
    EEPROM.put(t12_kj_jr_eeprom, t12_kj_jr);    //是否开机就加热
    EEPROM.put(vin_low_eeprom, vin_low);        //输入电压过底报警阈值
    EEPROM.put(tc1047_refer_eeprom, tc1047_refer);  //T12环境参考温度校准值
    EEPROM.put(oled_fx_eeprom, oled_fx);        //OLED方向
    EEPROM.put(xzbmq_fx_eeprom, xzbmq_fx);      //旋转编码器方向
    //写入曲线的4段温度
    EEPROM.put(sz_temp0_eeprom, sz_temp[0]);
    EEPROM.put(sz_temp1_eeprom, sz_temp[1]);
    EEPROM.put(sz_temp2_eeprom, sz_temp[2]);
    EEPROM.put(sz_temp3_eeprom, sz_temp[3]);
    //写入设置温度
    EEPROM.put(set_temp_eeprom, set_temp);
    //刷写完，记录auto_get的状态，防止下次开机再次刷写
    EEPROM.put(auto_get_eeprom, 1);
    auto_get = 1;
  }
  if (auto_get == 1)//状态为1只读取
  {
    EEPROM.get(p_eeprom, p);         //pid-p
    EEPROM.get(i_eeprom, i);         //pid-i
    EEPROM.get(d_eeprom, d);         //pid-d
    EEPROM.get(sleep_time_eeprom, sleep_time);  //休眠时间
    EEPROM.get(sleep_temp_eeprom, sleep_temp);  //休眠温度
    EEPROM.get(oled_ld_eeprom, oled_ld);        //OLED亮度
    EEPROM.get(vcc_refer_eeprom, vcc_refer);    //基准电压
    EEPROM.get(vin_refer_eeprom, vin_refer);    //电源电压校准值
    EEPROM.get(t12_kj_jr_eeprom, t12_kj_jr);    //是否开机就加热
    EEPROM.get(vin_low_eeprom, vin_low);        //输入电压过底报警阈值
    EEPROM.get(tc1047_refer_eeprom, tc1047_refer);  //T12环境参考温度校准值
    EEPROM.get(oled_fx_eeprom, oled_fx);        //OLED方向
    EEPROM.get(xzbmq_fx_eeprom, xzbmq_fx);      //旋转编码器方向
    //获取曲线的4段温度
    EEPROM.get(sz_temp0_eeprom, sz_temp[0]);
    EEPROM.get(sz_temp1_eeprom, sz_temp[1]);
    EEPROM.get(sz_temp2_eeprom, sz_temp[2]);
    EEPROM.get(sz_temp3_eeprom, sz_temp[3]);
    //获取设置温度
    EEPROM.get(set_temp_eeprom, set_temp);
  }
}
//******************************************************
