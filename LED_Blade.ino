#include <Adafruit_NeoPixel.h>
#include <Encoder.h>
#include "color.h"

#define LED_PIN           1
#define LED_NUM           2

#define SWITCH_LEFT_PIN   4
#define SWITCH_CENTER_PIN 3
#define SWITCH_RIGHT_PIN  2
#define NO_PUSH           0
#define SHORT_PUSH        1
#define LONG_PUSH         2

#define PIN_ENC_A         7
#define PIN_ENC_B         8

#define MODE_NORMAL       0
#define MODE_SHAKE        1
#define MODE_RYOURAN      2
#define MODE_CHANGE_THRESH  100

#define GYRO_THRESH       1000

//スイッチ用変数
unsigned short int mode = MODE_NORMAL;
unsigned short int left_switch_state = NO_PUSH;
unsigned short int center_switch_state = NO_PUSH;
unsigned short int right_switch_state = NO_PUSH;
short int color_main_index = 0;
short int color_sub_index = 0;

//ジャイロセンサ用変数
float GYRO_SENS = 0.67;//[mV/deg/s]
float GYRO_OFFSET = 1.44;//[V]
float gyro1 = 0.0;
float gyro2 = 0.0;

//エンコーダ用変数
volatile long oldPosition  = 0; 
volatile long newPosition  = 0;
short int brightness = 50;

//繚乱モードのインターバル
short int interval = 100;

//エンコーダクラス
Encoder myEnc(PIN_ENC_A, PIN_ENC_B);

//マイコンLEDクラス
Adafruit_NeoPixel LED_Pixel = Adafruit_NeoPixel(LED_NUM,
                                                LED_PIN,
                                                NEO_RGB+NEO_KHZ800);
//LEDカラーライブラリクラス
COLORS colors;

//色のクラス
HSV HSV_Val;
RGB RGB_Val;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(SWITCH_LEFT_PIN, INPUT);
  pinMode(SWITCH_CENTER_PIN, INPUT);
  pinMode(SWITCH_RIGHT_PIN, INPUT);

  HSV_Val.h=0;
  HSV_Val.s=1;
  HSV_Val.v=1;
  
  RGB_Val.r = 0;
  RGB_Val.g = 0;
  RGB_Val.b = 0;

  LED_Pixel.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  //左スイッチの押下時間のチェック
  if (digitalRead(SWITCH_LEFT_PIN)){
    left_switch_state = check_push_time(SWITCH_LEFT_PIN);
  }else{
    left_switch_state = NO_PUSH;
  }

  //センタースイッチの押下時間のチェック
  if (digitalRead(SWITCH_CENTER_PIN)){
    center_switch_state = check_push_time(SWITCH_CENTER_PIN);
  }else{
    center_switch_state = NO_PUSH;
  }

  //右スイッチの押下時間のチェック
  if (digitalRead(SWITCH_RIGHT_PIN)){
    right_switch_state = check_push_time(SWITCH_RIGHT_PIN);
  }else{
    right_switch_state = NO_PUSH;
  }

  //モードチェンジのチェック
  if (left_switch_state == LONG_PUSH){
    mode = MODE_SHAKE;
  }else if(right_switch_state == LONG_PUSH){
    mode = MODE_RYOURAN;
  }else if(center_switch_state == LONG_PUSH){
    mode = MODE_NORMAL;
  }else{
    //何もしない
  }

  //モードごとの対応
  if(mode == MODE_NORMAL){
    if (left_switch_state == SHORT_PUSH){
      color_sub_index = color_sub_index - 1;
    }else if (right_switch_state == SHORT_PUSH){
      color_sub_index = color_sub_index + 1;
    }else if (center_switch_state == SHORT_PUSH){
      color_main_index = color_sub_index;
    }else{
      //何もしない
    }

    //オーバーフロー防止
    if (color_sub_index < 0){
      color_sub_index = COLOR_NUM - 1;
    }

    if (color_sub_index >= COLOR_NUM){
      color_sub_index = 0;
    }

    //LEDの明るさを調整
    brightness = calc_bright_from_enc(brightness);
    //Serial.println(brightness);
    LED_Pixel.setBrightness(brightness);

    //LEDの色のセット
    LED_Pixel.setPixelColor(0, 
                            colors.red[color_sub_index],
                            colors.green[color_sub_index], 
                            colors.blue[color_sub_index]);//アドレス、GRBの順

    LED_Pixel.setPixelColor(1, 
                            colors.red[color_main_index],
                            colors.green[color_main_index], 
                            colors.blue[color_main_index]);//アドレス、GRBの順

    delay(100);

  }else if(mode == MODE_SHAKE){
    //ジャイロセンサの取得値が閾値を超えていたら、色を変える
    //Serial.println("Mode Gyro");

    //AD変換は3.3Vを1024分割している
    gyro1 = (analogRead(A5) / 1024.0) * 3.3 - GYRO_OFFSET;
    gyro1 = gyro1 * 1000 / GYRO_SENS;//[deg/s]
    
    gyro2 = (analogRead(A6) / 1024.0) * 3.3 - GYRO_OFFSET;
    gyro2 = gyro2 * 1000 / GYRO_SENS;//[deg/s]

    if (gyro1 > GYRO_THRESH || gyro2 > GYRO_THRESH){
      color_main_index = color_main_index + 1;
    }

    //オーバーフロー防止
    if (color_main_index < 0){
      color_main_index = COLOR_NUM - 1;
    }

    if (color_main_index >= COLOR_NUM){
      color_main_index = 0;
    }

    //LEDの明るさを調整
    brightness = calc_bright_from_enc(brightness);
    //Serial.println(brightness);
    LED_Pixel.setBrightness(brightness);

    LED_Pixel.setPixelColor(0, 0, 0, 0);//アドレス、GRBの順
    LED_Pixel.setPixelColor(1, 
                            colors.red[color_main_index],
                            colors.green[color_main_index], 
                            colors.blue[color_main_index]);//アドレス、GRBの順

    delay(100);
    /*
    Serial.print(gyro1);
    Serial.print(",");
    Serial.println(gyro2);
    */
  }else if(mode == MODE_RYOURAN){
    //色をグラデーションする
    HSV2RGB(&HSV_Val, &RGB_Val);

    HSV_Val.h = HSV_Val.h + 5;
    if(HSV_Val.h >= 360){
      HSV_Val.h = 0;
    }

    //Serial.println(interval);
    LED_Pixel.setBrightness(255);
    LED_Pixel.setPixelColor(0, 0, 0, 0);//アドレス、GRBの順
    LED_Pixel.setPixelColor(1, RGB_Val.r, RGB_Val.g, RGB_Val.b);//アドレス、GRBの順

    interval = calc_interval_from_enc(interval);
    delay(interval);
    //delay(10);
  }
  
  //LEDの点灯
  LED_Pixel.show(); 

  /*
  Serial.print("mode:");
  Serial.print(mode);
  Serial.print("switch:");
  Serial.print(left_switch_state);
  Serial.print(",");
  Serial.print(center_switch_state);
  Serial.print(",");
  Serial.println(right_switch_state);
  */
}

unsigned short int check_push_time(unsigned short int switch_num){
  unsigned short int tmp_check = 0;
  unsigned short int tmp_cnt = 0;
  unsigned short int push_state = NO_PUSH;
  int i = 0;

  for(i=0; i<MODE_CHANGE_THRESH; i++){
    tmp_check = digitalRead(switch_num);
    tmp_cnt = tmp_cnt + tmp_check;
    delay(10);
    if(tmp_check == 0){
      break;
    }
  }

  //Serial.println(tmp_cnt);

  if (tmp_cnt > ((float)MODE_CHANGE_THRESH / 2.0)){
    push_state = LONG_PUSH;
  }else if(tmp_cnt > 0 && tmp_cnt <= ((float)MODE_CHANGE_THRESH / 2.0)){
    push_state = SHORT_PUSH;
  }else{
    push_state = NO_PUSH;
  }

  return push_state;
}

short int calc_bright_from_enc(short int old_brightness){
  short int new_brightness;

  newPosition = myEnc.read();

  new_brightness = old_brightness;

  //ロータリーエンコーダの数値が変化した場合
   if (newPosition != oldPosition) {

   //回転方向が正方向だった場合brightness(明るさ)を1増やす
   //負方向だった場合brightnessを1減らす
    if (newPosition > oldPosition) {
      new_brightness = old_brightness + 20;
    } else {
      new_brightness = old_brightness - 20;
    }

    //brightnessを0-255に調整
    if (new_brightness > 255){
      new_brightness = 255;
    }else if(new_brightness < 0){
      new_brightness = 0;
    }

    // 前回位置oldPosition を現在位置newPosition に更新する
    oldPosition = newPosition;
   }

   return new_brightness;
}

short int calc_interval_from_enc(short int old_interval){
  short int new_interval;

  newPosition = myEnc.read();

  new_interval = old_interval;

  //ロータリーエンコーダの数値が変化した場合
   if (newPosition != oldPosition) {

   //回転方向が正方向だった場合brightness(明るさ)を1増やす
   //負方向だった場合brightnessを1減らす
    if (newPosition > oldPosition) {
      new_interval = old_interval + 10;
    } else {
      new_interval = old_interval - 10;
    }

    //brightnessを0-255に調整
    if (new_interval > 100){
      new_interval = 100;
    }else if(new_interval < 10){
      new_interval = 10;
    }

    // 前回位置oldPosition を現在位置newPosition に更新する
    oldPosition = newPosition;
   }

   return new_interval;
}

void HSV2RGB(HSV* hsv, RGB* rgb)
{
    double r, g, b ;
    double h = hsv->h;
    double s = hsv->s;
    double v = hsv->v;
    double c = s;
    double h2 = h / 60.;
    double x = c * (1 - fabs( fmod(h2,2)  -1 ));

    if (0 <= h2 && h2 < 1) r = c, g = x, b = 0;
    else if (1 <= h2 && h2 < 2) r = x, g = c, b = 0;
    else if (2 <= h2 && h2 < 3) r = 0, g = c, b = x;
    else if (3 <= h2 && h2 < 4) r = 0, g = x, b = c;
    else if (4 <= h2 && h2 < 5) r = x, g = 0, b = c;
    else if (5 <= h2 && h2 < 6) r = c, g = 0, b = x;
    else r = g = b = 0;
    r += (v - c);
    g += (v - c);
    b += (v - c);

    rgb->r = r*255;
    rgb->g = g*255;
    rgb->b = b*255;

    return;
}