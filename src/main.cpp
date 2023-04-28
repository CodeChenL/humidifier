#include <Adafruit_GFX.h>    // https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_ST7735.h> // https://github.com/adafruit/Adafruit-ST7735-Library
#include <SPI.h>
#include <OneButton.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;
TaskHandle_t taskhandle_srceen;
TaskHandle_t taskhandle_button;
TaskHandle_t taskhandle_blink;
SemaphoreHandle_t xMutextft=NULL;
sensors_event_t humidity, temp;
boolean xMutexclick=pdPASS;

const int TFT_CS = 7;
const int TFT_DC = 6;
const int TFT_MOSI = 3;
const int TFT_SCLK = 2;
const int TFT_RST = 10;
const int TFT_BACKLIGHT = 11;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

OneButton lkey(8,true,true);
OneButton ukey(9,true,true);
OneButton ckey(19,true,true);
OneButton rkey(13,true,true);
OneButton dkey(18,true,true);

void click(const char w[]){
  if(xSemaphoreTake(xMutextft,50)==pdPASS){
    if("no click"!=w){
      xMutexclick=pdFAIL;
    }
    tft.setCursor(0, 0);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST77XX_BLUE);
    tft.setTextSize(2);
    tft.setTextWrap(true);
    tft.println(w);
    tft.println(xTaskGetTickCount()/1000);
    // tft.println(ESP.getHeapSize());
    // tft.println(ESP.getFreeHeap());
    // tft.println(uxTaskGetStackHighWaterMark(taskhandle_button));
    aht.getEvent(&humidity, &temp);
    tft.print(temp.temperature);
    tft.println("C");
    tft.print(humidity.relative_humidity);
    tft.println("%");
    if ((humidity.relative_humidity<=70 || temp.temperature>=26) && humidity.relative_humidity<=90){
      Serial.println("ON");
      digitalWrite(0,LOW);
    }else{
      Serial.println("OFF");
      digitalWrite(0,HIGH);
    }
    
    Serial.println(w);
    xSemaphoreGive(xMutextft);
  }
}

void srceen_task(void *pt){
  TickType_t xlwt_srceen=xTaskGetTickCount();
  while (true)
  {
    if(xMutexclick==pdPASS){
      click("no click");  
    }else{
      xMutexclick=pdPASS;
    }
    vTaskDelayUntil(&xlwt_srceen,10000);
  }
  
}
void button_task(void *pt){
  TickType_t xlwt_button=xTaskGetTickCount();
  while (true)
  {
    lkey.tick();
    rkey.tick();
    ckey.tick();
    ukey.tick();
    dkey.tick();
    vTaskDelayUntil(&xlwt_button,100);
  }
  
}
void blink_task(void *pt){
  TickType_t xlwt_blink=xTaskGetTickCount();
  while (true)
  {
    static int duty = 0;
    static int step = 1;
    ledcWrite(0, duty);
    duty += step;
    if (abs(duty - 0) >= 10|| duty<=0) {
      step = -step;
    }
    vTaskDelayUntil(&xlwt_blink,100);
  }
}

void setup(void) {
  Serial.begin(115200);
  Serial.print(F("Hello! ST77xx TFT Test"));
  // 初始化 0.96" 160x80 TFT 的方法
  tft.initR(INITR_MINI160x80);  // Init ST7735S mini display

  //设置SPI的频率
  tft.setSPISpeed(40000000);
  tft.setRotation(4);
  Serial.println(F("Initialized"));
  tft.fillScreen(ST77XX_BLACK);

  pinMode(0,OUTPUT);
  digitalWrite(0,HIGH);
  pinMode(12,OUTPUT);
  ledcSetup(0, 5000, 12);
  ledcAttachPin(12, 0);
  pinMode(8,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);
  pinMode(19,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(18,INPUT_PULLUP);

  lkey.attachClick([]() {click("lclick");});
  rkey.attachClick([]() {click("rclick");});
  ckey.attachClick([]() {click("cclick");});
  ukey.attachClick([]() {click("uclick");});
  dkey.attachClick([]() {click("dclick");});

  xMutextft=xSemaphoreCreateMutex();

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
  delay(1000);
  xTaskCreate(srceen_task,"srceen_task",1024*8,NULL,1,&taskhandle_srceen);
  xTaskCreate(button_task,"button_task",1024*8,NULL,1,&taskhandle_button);
  xTaskCreate(blink_task,"blink_task",1024,NULL,1,&taskhandle_blink);
}

void loop() {
  delay(4000000000);
}
