/*
  Скетч создан на основе FASTSPI2 EFFECTS EXAMPLES автора teldredge (www.funkboxing.com)
  А также вот этой статьи https://www.tweaking4all.com/hardware/arduino/adruino-led-LEDS-effects/#cylon
  Доработан, переведён и разбит на файлы 2017 AlexGyver
  Смена выбранных режимов кнопкой. Кнопка подключена на D2 и GND
*/





#include <iarduino_Encoder_tmr.h>             //  Подключаем библиотеку iarduino_Encoder_tmr для работы с энкодерами через аппаратный таймер
iarduino_Encoder_tmr enc(A1,A4);              //  Объявляем объект enc для работы с энкодером указывая (№ вывода A, № вывода B)
#include <EEPROM.h>                                              //  Если при объявлении объектов перепутать выводы, то поворот влево будет расценен как поворот вправо и наоборот
#include "GyverButton.h"
GButton butt1(A5);                                  //  При использовании библиотеки iarduino_Encoder_tmr можно подключить до 8 энкодеров.


         // библиотека для работы с лентой

#define LED_COUNT 65          // число светодиодов в кольце/ленте
#define LED_DT 13             // пин, куда подключен DIN ленты

static byte bright;
static int counting;  
#include "FastLED.h" 
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED LEDS CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR

int thisindex = 0;
int thisRED = 0;
int thisGRN = 0;
int thisBLU = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR

volatile uint32_t btnTimer;
volatile byte modeCounter;
volatile boolean changeFlag;
int   counter=0;

CRGBPalette16 firePalette;

void setup()
{
 randomSeed(analogRead(0));
  counter = random(0, 30000);

  firePalette = CRGBPalette16(
                  getFireColor(0 * 16),
                  getFireColor(1 * 16),
                  getFireColor(2 * 16),
                  getFireColor(3 * 16),
                  getFireColor(4 * 16),
                  getFireColor(5 * 16),
                  getFireColor(6 * 16),
                  getFireColor(7 * 16),
                  getFireColor(8 * 16),
                  getFireColor(9 * 16),
                  getFireColor(10 * 16),
                  getFireColor(11 * 16),
                  getFireColor(12 * 16),
                  getFireColor(13 * 16),
                  getFireColor(14 * 16),
                  getFireColor(15 * 16)
                );
                
   butt1.setDirection(NORM_OPEN);
    butt1.setType(HIGH_PULL);
  butt1.setTickMode(AUTO);
  butt1.setDebounce(90);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);
  
  bright=EEPROM.read(1);
  counting=EEPROM.read(2);


   LEDS.setBrightness(bright);
  randomSeed(analogRead(0));

                
  enc.begin(); 
  Serial.begin(9600);              // открыть порт для связи
  LEDS.setBrightness(bright);  // ограничить максимальную яркость

  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)
  one_color_all(0, 0, 0);          // погасить все светодиоды
  LEDS.show();                     // отослать команду

  randomSeed(analogRead(0));
  pinMode(2, INPUT_PULLUP);
//  attachInterrupt(0, btnISR, FALLING);
 change_mode(counting);   
}

void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < LED_COUNT; i++ ) {
    leds[i].setRGB( cred, cgrn, cblu);
  }
}

void loop() {


    int a=enc.read();                         //  Читаем состояние энкодера в переменную a
    if(butt1.isClick()){
          if(counting>13)counting=0;
          else counting++;
      Serial.println( counting);
    }
    if(a){               
        if(butt1.isHold()){
        if(a==encLEFT ){
          if(counting>1)counting--;
          else counting=14;
          } 
        if(a==encRIGHT){
          if(counting>11)counting=1;
          else counting++;
          }
           EEPROM.write(2,counting); 
           Serial.println(String(counting));
       change_mode(counting);   
       changeFlag = true;
        }
        else{
          if(a==encLEFT ){
          if(bright>1)bright-=25;
          else bright=0;
          } 
        if(a==encRIGHT){
          if(bright>=250)bright=250;
          else bright+=25;
          }
             Serial.println("bright   "+String(bright));
          EEPROM.write(1,bright); 
          LEDS.setBrightness(bright);
        }
         
    }
    
    

  /*
    if (Serial.available() > 0) {     // если что то прислали
      ledMode = Serial.parseInt();    // парсим в тип данных int
      change_mode(ledMode);           // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)
    }
  */
  switch (counting) { //37 бильш нрав
    case 999: break;                           // пазуа
    case  1: rainbow_loop(); break;            // крутящаяся радуга // da
    case  2: flicker(); break;                 // случайный стробоскоп
    case 3:fireLine(); break;
    case 4: new_rainbow_loop(); break;        // крутая плавная вращающаяся радуга // daq!!!
    case 5:fireLineNoise();break;  
    }
}



void change_mode(int newmode) {
  thissat = 255;
  switch (newmode) {
   case 1: thisdelay = 20; thisstep = 10; break;       //---RAINBOW LOOP
    case 2: thishue = 160; thissat = 50; break;         //--- FLICKER
    case 4: thisdelay = 15; break;                      //---NEW RAINBOW LOOP

    case 6: one_color_all(255, 0, 0); LEDS.show(); break; //---ALL RED
    case 7: one_color_all(0, 255, 0); LEDS.show(); break; //---ALL GREEN
    case 8: one_color_all(0, 0, 255); LEDS.show(); break; //---ALL BLUE
    case 9: one_color_all(255, 255, 0); LEDS.show(); break; //---ALL COLOR X
    case 10: one_color_all(0, 255, 255); LEDS.show(); break; //---ALL COLOR Y
    case 11: one_color_all(255, 0, 255); LEDS.show(); break; //---ALL COLOR Z
    case 12:  one_color_all(255, 91, 0); LEDS.show(); break; //---ALL RED
    }
  bouncedirection = 0;
  one_color_all(0, 0, 0);
//  ledMode = newmode;
}
