# 智能加湿器
## 湿度低于60%或者温度低于28℃时，开启加湿
## 引脚定义
```C
//SPI LCD屏幕引脚连接定义
const int TFT_CS = 7;
const int TFT_DC = 6;
const int TFT_MOSI = 3;
const int TFT_SCLK = 2;
const int TFT_RST = 10;
const int TFT_BACKLIGHT = 11;

//摇杆GPIO连接定义
OneButton lkey(8,true,true);
OneButton ukey(9,true,true);
OneButton ckey(19,true,true);
OneButton rkey(13,true,true);
OneButton dkey(18,true,true);

//雾化片控制引脚
pinMode(0,OUTPUT);
```