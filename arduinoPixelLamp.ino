// Libraries:
//#include <Adafruit_NeoPixel.h>
#include "FastLED.h"
#include <Arduino.h>
#include <TM1637Display.h>
#include <Wire.h> 
#include "RTClib.h" 
#include <EEPROM.h>
#include <SoftwareSerial.h>

///////////////////////// Creates the bluetooth object:
SoftwareSerial bluetooth(5, 6);
//Turns PIN 5 into a RX (must be connected to Bluetooth's TX)
//Turns PIN 6 into a TX (must be connected to Bluetooth's RX)

///////////////////////// Variables for light mode fireplace()
#define FRAMES_PER_SECOND 60
bool gReverseDirection = false;
CRGBPalette16 gPal;
int COOLING = 8;
int SPARKING = 80; //40

///////////////////////// TM1637 DISPLAY MODULE
//Pins for TM1637 display module:
#define CLK 8
#define DIO 7

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};
TM1637Display display(CLK, DIO);
////////////////////////////////////////////////////////////

///////////////////////// LED WS2811
// Definitions for the LEDS
#define DATA_PIN    4  // Data output for LEDs
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    10
#define BRIGHTNESS  255
CRGB leds[NUM_LEDS];
////////////////////////////////////////////////////////////

///////////////////////// RTC MODULE DS1307

RTC_DS1307 rtc; //OBJETO DO TIPO RTC_DS1307 

//DECLARAÇÃO DOS DIAS DA SEMANA
//char daysOfTheWeek[7][12] = { "Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado" };
//int valorfinal;
//int DataTime[7];
//byte segto = 0x80, hora = 0, minuto = 0;
////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////// BUZZER

const int pinBuzzer = 2; //Digital pin for buzer

////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////// BUTTONS


///////////////////////////////////// BUTTON CLOCK
const int safe_button_press = 200;

bool read_button_clock = true;

const byte button_clock = 12; // Button clock on pin 12
byte button_clock_state = 0;
byte button_clock_last_state = 0;
unsigned long button_clock_startPressed = 0;    // the moment the button was pressed
unsigned long button_clock_endPressed = 0;      // the moment the button was released
unsigned long button_clock_holdTime = 0;        // how long the button was hold
unsigned long button_clock_idleTime = 0;        // how long the button was idle
//unsigned long time_before_play_alarm = 0;
unsigned long time_last_played;

int beep_delay = 2000;

int times_beeped = 0;
int times_beeped_absolute = 0;


unsigned long currentMillis;

unsigned long allow_change_clock = 0;

////////////////////////////////////////////////////////////

///////////////////////////////////// BUTTON UP
const byte button_up = 11; // Button clock on pin 11
byte button_up_state = 0;
byte button_up_last_state = 0;
unsigned long button_up_startPressed = 0;    // the moment the button was pressed
unsigned long button_up_endPressed = 0;      // the moment the button was released
unsigned long button_up_holdTime = 0;        // how long the button was hold
unsigned long button_up_idleTime = 0;        // how long the button was idle



///////////////////////////////////// 

///////////////////////////////////// BUTTON DOWN
const byte button_down = 10; // Button clock on pin 10
byte button_down_state = 0;
byte button_down_last_state = 0;
unsigned long button_down_startPressed = 0;    // the moment the button was pressed
unsigned long button_down_endPressed = 0;      // the moment the button was released
unsigned long button_down_holdTime = 0;        // how long the button was hold
unsigned long button_down_idleTime = 0;        // how long the button was idle

///////////////////////////////////// 

///////////////////////////////////// BUTTON LIGHTS
const byte button_lights = 9; // Button clock on pin 10
byte button_lights_state = 0;
byte button_lights_last_state = 0;
unsigned long button_lights_startPressed = 0;    // the moment the button was pressed
unsigned long button_lights_endPressed = 0;      // the moment the button was released
unsigned long button_lights_holdTime = 0;        // how long the button was hold
unsigned long button_lights_idleTime = 0;        // how long the button was idle

/////////////////////////////////////



//////////////////////////////////////////////////////////// FLAGS
// Set alarm:
DateTime now;
int  alarm_time_hour;
int alarm_time_hour_address = 0;
int  alarm_time_minute;
int alarm_time_minute_address = 1;
// Set new hour and minute:
int new_minute;
int new_hour;

unsigned long time_begin_sunshine = 0;
unsigned long time_end_sunshine = 0;
unsigned long time_end_alarm = 0;


unsigned long time_last_showed = 0;

bool set_hour_minute_flag = false;
bool clock_on = true;
bool alarm_on = true;
bool sleeping = true;
bool set_alarm = false;
bool allow_change_set_alarm = true;
bool allow_change_alarm_on = false;
bool allow_change_hour_minute = false;
bool lights_on = true;
bool button_check;

int light_mode = 1;
int num_light_modes = 6;

int brilho = 3;
int old_brilho = 0;

String comando_bt;
String data_bt = "";
char comandoMaster;

int pot_val_old = 0;
int min_pot_val = 1;
int max_pot_val = 1023;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// VOID SETUP()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

    delay(2000); // Sanity delay

    Serial.begin(9600);
    bluetooth.begin(9600);

    ////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////// LED WS2811
    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setDither(BRIGHTNESS < 255);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////RTC
    rtc.begin();
    now = rtc.now();

    //////////////////////////////////////////////////////////// BUTTONS

    pinMode(button_clock, INPUT_PULLUP);
    button_clock_last_state = digitalRead(button_clock);
    button_clock_state = digitalRead(button_clock);
    pinMode(button_up, INPUT_PULLUP);
    pinMode(button_down, INPUT_PULLUP);
    pinMode(button_lights, INPUT_PULLUP);

    pinMode(pinBuzzer, OUTPUT);

    // Potentiometer
    pinMode(A1, INPUT);

    //////////////////////////////////////////////////////////// ALARM

    alarm_time_hour = EEPROM.read(alarm_time_hour_address);
    alarm_time_minute = EEPROM.read(alarm_time_minute_address);

    // IF YOU WANT TO SET THE PC'S DATE AND TIME ON THE RTC
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //tone(pinBuzzer, 600, 150);  

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// VOID LOOP()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

    if (bluetooth.available()) {

        comandoMaster = 'x';

        while (bluetooth.available()) {
            
            char char_bt = (bluetooth.read());

            comando_bt += char_bt;

            if (comandoMaster == 'x') {
                comandoMaster = char_bt;
            }

        }

        data_bt = comando_bt.substring(1);        
    }

    while (comandoMaster != 'x') {

        if (comandoMaster == 'j') {

            light_mode += 1;

            if (light_mode > num_light_modes) {
                light_mode = 1;
            }

            comandoMaster = 'x';
            break;
        }

        if (comandoMaster == 'v') {

            light_mode -= 1;

            if (light_mode < 1) {
                light_mode = num_light_modes;
            }

            comandoMaster = 'x';
            break;
        }

        if (comandoMaster == 't') {

            clock_on = !clock_on;            
            comandoMaster = 'x';
            break;
        }

        if (comandoMaster == 'm') {
            lights_on = !lights_on;
            comandoMaster = 'x';
            break;
        }

        if (comandoMaster == 'e') {

           /* static String hora = data_bt.substring(1, 2);
            static String minuto = data_bt.substring(3, 4);

            int int_minuto = minuto.toInt();
            int int_hora = hora.toInt();

            EEPROM.write(alarm_time_minute_address, int_minuto);
            EEPROM.write(alarm_time_hour_address, int_hora);

            comandoMaster = 'x';*/
        }

        if (comandoMaster == 'k') {            

            light_mode = 1;
            static String pot_val = data_bt;
            pot_val_old = map(pot_val.toInt(),0,100,min_pot_val,max_pot_val);

            display.showNumberDec(data_bt.toInt(), false);
            display.setBrightness(7);




            comandoMaster = 'x';
        }

        comandoMaster = 'x';

    }


    ///////////////////////////////////////////////////////////////////////// ALWAYS DO

    // Get current time:
    now = rtc.now();
    currentMillis = millis();

    //// Get potentiometer value
    int pot_val_new = analogRead(A1);

    if (abs(pot_val_new - pot_val_old) > 10) {
        pot_val_old = pot_val_new;
    }

    /*Serial.print("new= ");
    Serial.print(pot_val_new);
    Serial.print("  old= ");
    Serial.println(pot_val_old);*/

    //// Check if any button was pressed
    button_check = check_if_buttons_were_pressed();

    ///////////////////////////////////////////////////////////////////////// FLAGGED ACTIONS


    //////////////////////////////////////////////// SHOW LIGHTS

    if (lights_on) {

        if (light_mode == 1) {

            brilho = map(pot_val_old, min_pot_val, max_pot_val, 20, BRIGHTNESS);

            if (brilho > 220) {
                brilho = BRIGHTNESS;
            }

            //Serial.println(brilho);

           /* if ((brilho - old_brilho) > 2 || (brilho - old_brilho) < -2) {
                old_brilho = brilho;
            }*/

            moon(brilho);
        }

        else if (light_mode == 2) {
            SPARKING = map(pot_val_old, min_pot_val, max_pot_val, 10, 200);
            fireplace(SPARKING, COOLING);
            FastLED.show();
        }

        else if (light_mode == 3) {
            jujuba(pot_val_old);
        }

        else if (light_mode == 4) {

            rainbowCycle(map(pot_val_old, min_pot_val, max_pot_val, 1, 20));
            FastLED.show();
        }
        else if (light_mode == 5) {
            Respiro();
        }
        else if (light_mode == 6) {
            myColor(pot_val_old);
        }
        else if (light_mode == 100) {
            sunshine();
        }
        else {
            light_mode = 1; // Safety control
        }

    }

    if (!lights_on) {
        turn_off_leds();
    }

    //////////////////////////////////////////////// SHOW CLOCK

    if (clock_on) {
        display_current_time(now, alarm_time_hour, alarm_time_minute);
    }

    if (!clock_on) {
        display.clear();
    }

    //////////////////////////////////////////////// SET ALARM
    // Activated when button CLOCK is pressed for 3 seconds
    // Press button UP and DOWN to change HOUR and MINUTE values for alarm time

    if (set_alarm) {

        read_button_clock = true;
        clock_on = true;

        alarm_time_hour = EEPROM.read(alarm_time_hour_address);
        alarm_time_minute = EEPROM.read(alarm_time_minute_address);

    }

    if (!set_alarm && !set_hour_minute_flag) {
        time_last_showed = millis();
    }

    //////////////////////////////////////////////// SET NEW HOUR AND MINUTE
    // Activated when button CLOCK is pressed for 10 seconds
    // Press button UP and DOWN to change HOUR and MINUTE values

    if (set_hour_minute_flag) {

        set_alarm = false;
        read_button_clock = true;
        clock_on = true;

    }

    //////////////////////////////////////////////// ALARM CHECK

    if (alarm_on) {

        // Sunshine check (lights up 5 minutes before playing alarm)

        if (sunshine_check(now, alarm_time_hour, alarm_time_minute)) {
            lights_on = true;
            light_mode = 100;
            clock_on = true;
        }
        else {

            time_begin_sunshine = millis();
            time_end_sunshine = time_begin_sunshine + (240000);
            time_end_alarm = time_begin_sunshine + (900000);           


        }

        // Check if it's time to wake up:
        if (wake_up_check(now, alarm_time_hour, alarm_time_minute)) {

            set_alarm = false;
            sleeping = false;
            play_alarm();

        }
    }

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int map_my_pot(int pot_value) {

    int pot_mapped = map(pot_value, 0, 840, 0, 100);
    return pot_mapped;
}


 ////////////////////////////////////////////////////////////////////////////////////////////////////////// ALARM FUNCTIONS
void show_alarm_status(bool status) {

    unsigned long time_to_stop = millis() + 2000;

    while (millis() < time_to_stop) {

        if (status) {

            for (int i = 0; i < NUM_LEDS; i++) {
                leds[i] = CRGB::ForestGreen;
            }
            FastLED.show();

        }

        if (!status) {

            for (int i = 0; i < NUM_LEDS; i++) {
                leds[i] = CRGB::Red;
            }
            FastLED.show();


        }

    }



}

void get_current_time_once() {

    // Get current time:
    DateTime now_to_change = rtc.now();

    new_minute = now_to_change.minute();
    new_hour = now_to_change.hour();

}

void play_alarm() {

    if (millis() > time_last_played) {
        tone(pinBuzzer, 600, 350);
        time_last_played = millis() + beep_delay;
        times_beeped += 1;
        times_beeped_absolute += 1;
    }

    if (times_beeped >= 15) {

        if (beep_delay >= 600) {
            beep_delay -= 500;
        }
        else {
            beep_delay = 400;
        }
        
        times_beeped = 0;
    }

    if (millis() >= time_end_alarm) {  //Turn alarm off
        alarm_on = false;
        times_beeped = 0;
        times_beeped_absolute = 0;
        beep_delay = 2000;
    }

    // Turn on the lights
    light_mode = 100;
    lights_on = true;
    CRGB color1 = CHSV(255, 220, 0);

    // fade all existing pixels toward bgColor by "5" (out of 255)
    fadeTowardColor(leds, NUM_LEDS, color1, 1);
    FastLED.show();

}

bool sunshine_check(DateTime now, int alarm_time_hour, int alarm_time_minute) {

    // Compares hours and minutes from time set for alarm with current time

    String compare_alarm = "";    
    
    if (alarm_time_hour == 0) {
        compare_alarm += "00";
    }
    else {
        compare_alarm += alarm_time_hour;
    }

    if (alarm_time_minute <= 9) {
        compare_alarm += "0";    
        if (alarm_time_minute == 0) {
            compare_alarm += "0";
        }
        else {
            compare_alarm += alarm_time_minute;
        }
    }    else {
        compare_alarm += alarm_time_minute;
    }

    String compare_now = "" ;

    if (now.hour() == 0) {
        compare_now += "00";
    }
    else {
        compare_now += now.hour();
    }

    if (now.minute() <= 9) {
        compare_now += "0";
        if (now.minute() == 0) {
            compare_now += "0";
        }
        else {
            compare_now += now.minute();
        }
    }    else {
        compare_now += now.minute();
    }
    
    int compare_alarm_int = compare_alarm.toInt();
    int compare_now_int = compare_now.toInt();

    if ((compare_alarm_int - compare_now_int)<=5 && (compare_alarm_int - compare_now_int) >=0 ){ 

            // Time to shine!
            return true;        
    }
    else {
        return false;
    }

}

bool wake_up_check(DateTime now, int alarm_time_hour, int alarm_time_minute) {

    // Compares hours and minutes from time set for alarm with current time

    if (alarm_time_hour == now.hour() && alarm_time_minute == now.minute()) {
        // Time to wake up!
        return true;
    }
    else {
        return false;
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////// END OF ALARM FUNCTIONS

 ////////////////////////////////////////////////////////////////////////////////////////////////// CLOCK AND TIME FUNCTIONS

void display_current_time(DateTime now, int alarm_hour, int alarm_minute) {

    /////////////////////////// TO SHOW CURRENT TIME:
    if (!set_alarm && !set_hour_minute_flag) {


       /* display.showNumberDec(pot_val_old, true);
        display.setBrightness(7);*/



        //// if (!set_alarm) {

        //// Set brightness
        //display.setBrightness(7);

        //// Displays it:
        ////display.showNumberDec(current_time, true);  

        //// Zera e cria o array position[] para armazenar os valores do cronômetro
        //uint8_t position[] = { 0, 0, 0, 0 };
        //// Calculo individual para posição no display
        //position[0] = display.encodeDigit(now.hour() / 10); // Dezena de hora
        //position[1] = display.encodeDigit(now.hour() % 10); // Unidade de hora
        //position[2] = display.encodeDigit((now.minute() % 60) / 10); // Dezena de minuto
        //position[3] = display.encodeDigit((now.minute() % 60) % 10); // Unidade de minuto
        //// Coloca o separador ":"
        //position[1] = position[1] + 128;
        //display.setSegments(position);
    }

    /////////////////////////// TO SHOW ALARM TIME:

    if (set_alarm) {

        if (millis() > time_last_showed && millis() < time_last_showed + 500) {


            // Set brightness
            display.setBrightness(7);

            // Zera e cria o array position[] para armazenar os valores do cronômetro
            uint8_t position[] = { 0, 0, 0, 0 };
            // Calculo individual para posição no display
            position[0] = display.encodeDigit(alarm_hour / 10); // Dezena de hora
            position[1] = display.encodeDigit(alarm_hour % 10); // Unidade de hora
            position[2] = display.encodeDigit((alarm_minute % 60) / 10); // Dezena de minuto
            position[3] = display.encodeDigit((alarm_minute % 60) % 10); // Unidade de minuto
            // Coloca o separador ":"
            position[1] = position[1] + 128;
            display.setSegments(position);
        }

        if (millis() >= time_last_showed + 500) {
            display.clear();
            time_last_showed = millis() + 250;
        }
    }

    /////////////////////////// TO SET NEW HOUR AND MINUTE:

    if (set_hour_minute_flag) {

        if (millis() > time_last_showed && millis() < time_last_showed + 500) {


            // Set brightness
            display.setBrightness(7);

            // Zera e cria o array position[] para armazenar os valores do cronômetro
            uint8_t position[] = { 0, 0, 0, 0 };
            // Calculo individual para posição no display
            position[0] = display.encodeDigit(new_hour / 10); // Dezena de hora
            position[1] = display.encodeDigit(new_hour % 10); // Unidade de hora
            position[2] = display.encodeDigit((new_minute % 60) / 10); // Dezena de minuto
            position[3] = display.encodeDigit((new_minute % 60) % 10); // Unidade de minuto
            // Coloca o separador ":"
            position[1] = position[1] + 128;
            display.setSegments(position);
        }

        if (millis() >= time_last_showed + 500) {
            display.clear();
            time_last_showed = millis() + 250;
        }


    }



}

void  set_hour_minute() {

    rtc.adjust(DateTime(2020, 01, 01, new_hour, new_minute, 00)); //(YEAR), (MONTH), (DAY), (HOUR), (MINUTES), (SECONDS)
  //
}
void  update_my_time_now() {
    DateTime now = rtc.now();
}

//////////////////////////////////////////////////////////////////////////////////////////// END OF CLOCK AND TIME FUNCTIONS

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// LIGHT MODES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

byte cor_sunshine_R = 0xff;
byte cor_sunshine_G = 0xbf;
byte cor_sunshine_B = 0x00;

void sunshine() {

    float r, g, b;

    unsigned long fator = map(millis(), time_begin_sunshine, time_end_sunshine, 0, 1000);

    if (fator > 1000) {
        fator = 1000;
    }


    r = cor_sunshine_R * fator / 1000;
    g = cor_sunshine_G * fator / 1000;
    b = cor_sunshine_B * fator / 1000;
    setAll(r, g, b);
    showStrip(); 


    /*for (int i = 0; i < NUM_LEDS; i++) {


        leds[i] = CRGB::Gold;
        FastLED.setBrightness(brilho3);
        FastLED.show();

        if (!check_if_buttons_were_pressed) {
            break;
        }

    }*/

}

void myColor(int pot_val) {


    CRGB color1 = CHSV(map(pot_val, min_pot_val, max_pot_val, 0, 255), 255, 255);

    // fade all existing pixels toward bgColor by "5" (out of 255)
    fadeTowardColor(leds, NUM_LEDS, color1, 20);
    FastLED.show();
    // Serial.println(map(pot_val,0,1023,0,255));

}


bool comecar = true;
bool respirar = false;
bool segurar = false;
bool expirar = false;
bool segurar2 = false;

unsigned long time_begin;
unsigned long time_to_hold;
unsigned long time_to_let_go;
unsigned long time_to_let_go2;
unsigned long time_to_begin_again;

byte cor_respiro_R = 0xff;
byte cor_respiro_G = 0x55;
//byte cor_respiro_B = 0x16;
byte cor_respiro_B = 0x00;


//byte cor_respiro_R = 0xff;
//byte cor_respiro_G = 0xd1;
//byte cor_respiro_B = 0x19;


void Respiro() {

    // Relaxation through breathing exercise. Inhale while orange light grows; hold while blue; exhale while light fades, hold while dark

    float r, g, b;

    if (comecar) {
        time_begin = millis();
        time_to_hold = millis() + 4000;
        time_to_let_go = time_to_hold + 3000;
        time_to_let_go2 = time_to_let_go + 300;
        comecar = false;
        respirar = true;
    }

    if (respirar) {

        //Inspira

        unsigned long fator = map(millis(), time_begin, time_to_hold, 0, 1000);

        r = cor_respiro_R * fator / 1000;
        g = cor_respiro_G * fator / 1000;
        b = cor_respiro_B * fator / 1000;
        setAll(r, g, b);
        showStrip();

        if (millis() >= time_to_hold) {

            respirar = false;

        }
    }

    if (millis() >= time_to_hold) {
        //time_to_let_go = millis() + 80000;

        CRGB bgColor1(0, 100, 255);
        fadeTowardColor(leds, NUM_LEDS, bgColor1, 20);
        FastLED.show();
    }

    if (millis() >= time_to_let_go) {
        time_to_hold = millis() + 80000;

        CRGB bgColor2(255, 85, 0);
        fadeTowardColor(leds, NUM_LEDS, bgColor2, 20);
        FastLED.show();

    }

    if (millis() >= time_to_let_go2) {
        time_to_let_go = millis() + 80000;
        time_to_let_go2 = millis() + 80000;
        time_begin = millis();
        time_to_hold = millis() + 2500;
        expirar = true;

    }

    if (expirar) {
        // Expira

        unsigned long fator = map(millis(), time_begin, time_to_hold, 1000, 0);

        r = (cor_respiro_R * fator / 1200);
        g = (cor_respiro_G * fator / 1200);
        b = (cor_respiro_B * fator / 1200);
        setAll(r, g, b);
        showStrip();

        if (millis() >= time_to_hold) {
            /*for (int i = 0; i < NUM_LEDS; i++) {
                leds[i] = CRGB::Black;
            }
            FastLED.show();*/
            time_to_begin_again = millis() + 3300;
            time_to_hold = millis() + 80000;
            expirar = false;
            segurar2 = true;
        }
    }

    if (segurar2) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Black;
        }
        FastLED.show();

        if (millis() >= time_to_begin_again) {
            comecar = true;
            segurar2 = false;
        }
    }
}

void FadeInOut(byte red, byte green, byte blue, int speed) {
    float r, g, b;

    for (int k = 0; k < (256 * speed); k = k + 1) {
        r = (k / (256.0 * speed)) * red;
        g = (k / (256.0 * speed)) * green;
        b = (k / (256.0 * speed)) * blue;
        setAll(r, g, b);
        showStrip();
        if (check_if_buttons_were_pressed()) {
            break;
        }
    }

    for (int k = (255 * speed); k >= 0; k = k - 2) {
        r = (k / (256.0 * speed)) * red;
        g = (k / (256.0 * speed)) * green;
        b = (k / (256.0 * speed)) * blue;
        setAll(r, g, b);
        showStrip();
        if (check_if_buttons_were_pressed()) {
            break;
        }
    }
}

void rainbowCycle(int SpeedDelay) {
    byte* c;
    uint16_t i, j;



    for (j = 0; j < 256; j++) { // 5 cycles of all colors on wheel
        for (i = 0; i < NUM_LEDS; i++) {
            c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
            setPixel(i, *c, *(c + 1), *(c + 2));
            if (check_if_buttons_were_pressed()) {
                break;
            }

        }
        showStrip();

        if (check_if_buttons_were_pressed()) {
            break;
        }

        delay(SpeedDelay);
    }
}

byte* Wheel(byte WheelPos) {
    static byte c[3];

    if (WheelPos < 85) {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    }
    else if (WheelPos < 170) {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    }
    else {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }    

    return c;
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        setPixel(i, red, green, blue);
        showStrip();
        delay(SpeedDelay);
    }
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }

    delay(ReturnDelay);

    for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }

    delay(ReturnDelay);
}

void moon(int brilho1) {

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Beige;
        FastLED.setBrightness(brilho1);
        FastLED.show();

        if (!check_if_buttons_were_pressed) {
            break;
        }

    }
}

void fireplace(int SPARKING, int COOLING) {

    gPal = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::OrangeRed, CRGB::Yellow);

    // This first palette is the basic 'black body radiation' colors,
    // which run from black to red to bright yellow to white.
    //gPal = HeatColors_p;

    // These are other ways to set up the color palette for the 'fire'.
    // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
    //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);

    // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
    //   gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);

    // Third, here's a simpler, three-step gradient, from black to red to white
    //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);

    // Add entropy to random number generator; we use a lot of it.
    random16_add_entropy(random());

    // Fourth, the most sophisticated: this one sets up a new palette every
    // time through the loop, based on a hue that changes every time.
    // The palette is a gradient from black, to a dark color based on the hue,
    // to a light color based on the hue, to white.
    //
    //   static uint8_t hue = 0;
    //   hue++;
    //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
    //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
    //   gPal = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);

    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING) {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++) {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        byte colorindex = scale8(heat[j], 240);
        CRGB color = ColorFromPalette(gPal, colorindex);
        int pixelnumber;
        if (gReverseDirection) {
            pixelnumber = (NUM_LEDS - 1) - j;
        }
        else {
            pixelnumber = j;
        }
        leds[pixelnumber] = color;
    }

    FastLED.show(); // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);

}

void turn_off_leds() {

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }

    FastLED.show();

}

void showStrip() {


    // FastLED
    FastLED.show();

}

void setPixel(int Pixel, byte red, byte green, byte blue) {

    // FastLED
    leds[Pixel].r = red;
    leds[Pixel].g = green;
    leds[Pixel].b = blue;

}

void setAll(byte red, byte green, byte blue) {
    for (int i = 0; i < NUM_LEDS; i++) {
        setPixel(i, red, green, blue);
    }
    showStrip();
}

void nblendU8TowardU8(uint8_t& cur, const uint8_t target, uint8_t amount)
{
    if (cur == target) return;

    if (cur < target) {
        uint8_t delta = target - cur;
        delta = scale8_video(delta, amount);
        cur += delta;
    }
    else {
        uint8_t delta = cur - target;
        delta = scale8_video(delta, amount);
        cur -= delta;
    }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor(CRGB& cur, const CRGB& target, uint8_t amount)
{
    nblendU8TowardU8(cur.red, target.red, amount);
    nblendU8TowardU8(cur.green, target.green, amount);
    nblendU8TowardU8(cur.blue, target.blue, amount);
    return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor(CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount)
{
    for (uint16_t i = 0; i < N; i++) {
        fadeTowardColor(L[i], bgColor, fadeAmount);
    }
}

CRGB color = CHSV(random8(), 255, 255);

unsigned long next_color = 0;

void jujuba(int pot_val) {

    if (millis() >= next_color) {
        color = CHSV(random8(), 255, 255);
        next_color = millis() + map(pot_val, min_pot_val, max_pot_val, 1000, 15000);
    }

    // fade all existing pixels toward bgColor by "5" (out of 255)
    fadeTowardColor(leds, NUM_LEDS, color, 1);
    FastLED.show();
}

//////////////////////////////////////////////////////////////////// BUTTON FUNCTIONS
// Debounce button, time-while-holding counter, etc
void button_lights_update_state() {

    if (digitalRead(button_lights) == LOW) {  // If lights button is pressed: 

        button_lights_startPressed = millis();
        button_lights_idleTime = button_lights_startPressed - button_lights_endPressed;

        // TURN ON / OFF LIGHTS
        lights_on = !lights_on;

    }

    else {
        button_lights_endPressed = millis();
        button_lights_holdTime = button_lights_endPressed - button_lights_startPressed;
    }



}

void button_lights_updateCounter() {

    // the button is still pressed
    if (button_lights_state == LOW) {
        button_lights_holdTime = millis() - button_lights_startPressed;
    }
    else {
        button_lights_idleTime = millis() - button_lights_endPressed;
    }
}

void button_down_update_state() {

    if (digitalRead(button_down) == LOW) {  // If down button is pressed: 

        button_down_startPressed = millis();
        button_down_idleTime = button_down_startPressed - button_down_endPressed;

        // CHANGES VALUE ON ALARM_MINUTE
        if (set_alarm) {
            alarm_time_minute = alarm_time_minute + 5;

            EEPROM.write(alarm_time_minute_address, alarm_time_minute);

            if (alarm_time_minute >= 60) {
                alarm_time_minute = 0;
                EEPROM.write(alarm_time_minute_address, alarm_time_minute);
            }

        }
        else {

            light_mode -= 1;

            if (light_mode <= 0) {
                light_mode = num_light_modes;
            }


            button_down_endPressed = millis();
            button_down_holdTime = button_down_endPressed - button_down_startPressed;



        }

        // CHANGES VALUE FOR NEW MINUTE
        if (set_hour_minute_flag) {

            new_minute = new_minute + 1;

            if (new_minute >= 60) {
                new_minute = 0;
            }
        }

    }

    /* else {

         light_mode -= 1;

         if (light_mode <= 0) {
             light_mode = num_light_modes;
         }


         button_down_endPressed = millis();
         button_down_holdTime = button_down_endPressed - button_down_startPressed;
     }*/



}

void button_down_updateCounter() {

    // the button is still pressed
    if (button_down_state == LOW) {
        button_down_holdTime = millis() - button_down_startPressed;
    }
    else {
        button_down_idleTime = millis() - button_down_endPressed;
    }
}

void button_up_update_state() {

    if (digitalRead(button_up) == LOW) {  // If up button is pressed: 

        button_up_startPressed = millis();
        button_up_idleTime = button_up_startPressed - button_up_endPressed;

        // CHANGES VALUE ON ALARM_HOUR
        if (set_alarm) {

            alarm_time_hour = alarm_time_hour + 1;

            EEPROM.write(alarm_time_hour_address, alarm_time_hour);

            if (alarm_time_hour >= 24) {
                alarm_time_hour = 0;
                EEPROM.write(alarm_time_hour_address, alarm_time_hour);
            }

        }
        else {
            // Changes light mode

            light_mode += 1;

            if (light_mode > num_light_modes) {
                light_mode = 1;
            }

            button_up_endPressed = millis();
            button_up_holdTime = button_up_endPressed - button_up_startPressed;

        }

        // CHANGES VALUE FOR NEW HOUR
        if (set_hour_minute_flag) {

            new_hour = new_hour + 1;

            if (new_hour >= 24) {
                new_hour = 0;
            }
        }

    }
}

void button_up_updateCounter() {

    // the button is still pressed
    if (button_up_state == LOW) {
        button_up_holdTime = millis() - button_up_startPressed;
    }
    else {
        button_up_idleTime = millis() - button_up_endPressed;
    }
}

void button_clock_update_state() {

    allow_change_set_alarm = true;  // Enable permission to change the set_alarm flag

    if (digitalRead(button_clock) == LOW) {  // If clock button is pressed: 

        button_clock_startPressed = millis();
        button_clock_idleTime = button_clock_startPressed - button_clock_endPressed;

        clock_on = !clock_on; // If its true, it changes it's value to false, and vice versa   


    }
    else {
        button_clock_endPressed = millis();
        button_clock_holdTime = button_clock_endPressed - button_clock_startPressed;


    }


    // Turn alarm off

    if (!sleeping) {
        alarm_on = false;
        sleeping = true;
        times_beeped = 0;
        times_beeped_absolute = 0;
    }

    if (button_clock_last_state == LOW) { // If it was LOW (pressed), it means it's just been released
       /* Serial.print("Button 'clock' was hold for ");
        Serial.println(button_clock_holdTime);*/
    }
    if (button_clock_last_state == HIGH) {
        /*  Serial.print("Button 'clock' was idle for ");
          Serial.println(button_clock_idleTime);*/
    }

}

void button_clock_updateCounter() {

    // the button is still pressed
    if (button_clock_state == LOW) {
        button_clock_holdTime = millis() - button_clock_startPressed;

        if (button_clock_holdTime > 1500) { // If it was pressed for more than 1.5 second:

            if (allow_change_set_alarm) {

                set_alarm = !set_alarm;
                Serial.println("SET ALARM FLAG CHANGED");

                allow_change_set_alarm = false;

                allow_change_alarm_on = true;
            }

        }

        if (button_clock_holdTime > 3000) { // If it was pressed for more than 3 second:

            if (allow_change_alarm_on) {

                alarm_on = !alarm_on;
                set_alarm = !set_alarm;
                allow_change_hour_minute = true;

                show_alarm_status(alarm_on);

                if (alarm_on) {
                    Serial.println("Alarm is ON!");
                }
                if (!alarm_on) {
                    Serial.println("Alarm is OFF!");
                }

                allow_change_alarm_on = false;


            }

        }

        if (button_clock_holdTime > 8000) { // If it was pressed for more than 8 seconds:

            if (allow_change_hour_minute) {

                Serial.println("set_hour_minute_flag is True");

                // BEFORE FLAG CHANGES VALEU:
                if (set_hour_minute_flag) {
                    set_hour_minute();
                }

                set_hour_minute_flag = !set_hour_minute_flag;
                set_alarm = false;
                clock_on = true;

                get_current_time_once();

                alarm_on = !alarm_on;
                allow_change_hour_minute = false;

            }



        }





    }
    else {
        button_clock_idleTime = millis() - button_clock_endPressed;
    }
}

bool check_if_buttons_were_pressed() {

    //////////////////////////////////////////////// BUTTON CLOCK

   // Check if button clock is pressed
    if (read_button_clock) {
        button_clock_state = digitalRead(button_clock);
    }

    if (button_clock_state != button_clock_last_state) { // button state changed     

        button_clock_update_state();
        button_clock_last_state = button_clock_state;        // save state for next loop
        return true;

    }
    else {
        button_clock_updateCounter(); // button state not changed. It runs in a loop.
    }


    button_clock_last_state = button_clock_state;        // save state for next loop


     //////////////////////////////////////////////// BUTTON UP

    button_up_state = digitalRead(button_up);

    if (button_up_state != button_up_last_state) { // button state changed     

        button_up_update_state();
        button_up_last_state = button_up_state;        // save state for next loop
        return true;

    }
    else {
        button_up_updateCounter(); // button state not changed. It runs in a loop.
    }


    button_up_last_state = button_up_state;        // save state for next loop

    //////////////////////////////////////////////// BUTTON DOWN

    button_down_state = digitalRead(button_down);

    if (button_down_state != button_down_last_state) { // button state changed     

        button_down_update_state();
        button_down_last_state = button_down_state;        // save state for next loop
        return true;

    }
    else {
        button_down_updateCounter(); // button state not changed. It runs in a loop.
    }


    button_down_last_state = button_down_state;        // save state for next loop

    //////////////////////////////////////////////// BUTTON LIGHTS

    button_lights_state = digitalRead(button_lights);

    if (button_lights_state != button_lights_last_state) { // button state changed     

        button_lights_update_state();
        button_lights_last_state = button_lights_state;        // save state for next loop
        return true;

    }
    else {
        button_lights_updateCounter(); // button state not changed. It runs in a loop.
    }


    button_lights_last_state = button_lights_state;        // save state for next loop


    return false;


}

//////////////////////////////////////////////////////////////////// END OF  BUTTON FUNCTIONS




