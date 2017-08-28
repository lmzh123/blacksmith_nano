#include <OneWire.h>
#include <DallasTemperature.h>

// Defino Estados de la MEF
#define ECOOL 0
#define ESMOKE 1
#define ETEMP 2
#define EDOOROPEN 3
#define EDOORALARM 4
#define EEMERGENCY 5

// MEF 2
#define EFANOFF 0
#define EFANON 1

// MEF 3
#define EALARMOFF 0
#define EALARMON 1

// Data wire is plugged into port 2 on the Arduino
#define SMOKE 0
#define DOOR 1
#define ONE_WIRE_BUS 2
#define BUZZER 3
#define FAN 4
#define FIRE_EXT 5
#define PILOT 6
#define EALARM 7

//Definición de constantes
const unsigned long topen = 5000; //Defino el tiempo de parpadeo como constante unsigned long y la inicio en 500 milisecs
unsigned long t_alarm_on = 500; //Defino el tiempo de ON como constante unsigned long y la inicio en 500 milisecs
unsigned long t_alarm_off = 500; //Defino el tiempo de ON como constante unsigned long y la inicio en 500 milisecs
const unsigned long t_fan_on = 4000; //Defino el tiempo de ON como constante unsigned long y la inicio en 500 milisecs
const unsigned long t_fan_off = 4000; //Defino el tiempo de ON como constante unsigned long y la inicio en 500 milisecs
const unsigned long t_door_open = 10000; //Defino el tiempo de ON como constante unsigned long y la inicio en 500 milisecs
const unsigned long t_emergency = 4000; //Defino el tiempo de ON como constante unsigned long y la inicio en 500 milisecs
float temp_low = 35.0; //Valor para threshold inferior de temperatura
float temp_high = 55.0; //Valor para threshold superior de temperatura

bool fan_flag = true;
bool alarm_flag = false;

float temp = 0.0;

unsigned int nxstate = ECOOL;
unsigned int nxstate_fan = EFANOFF;
unsigned int nxstate_alarm = EALARMOFF;

//Definición de variables de temporización
unsigned long tactual = 0; //Defino tactual como unsigned long
unsigned long tini_fan = 0; //Defino tini como unsigned long
unsigned long tstate_fan = 0; //Defino tstate como unsigned long

unsigned long tini_dooropen = 0; //Defino tini como unsigned long
unsigned long tstate_dooropen = 0; //Defino tstate como unsigned long

unsigned long tini_alarm = 0; //Defino tini como unsigned long
unsigned long tstate_alarm = 0; //Defino tstate como unsigned long

unsigned long tini_emergency = 0; //Defino tini como unsigned long
unsigned long tstate_emergency = 0; //Defino tstate como unsigned long

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // Start up the library
  sensors.begin();

  // Setup GPIO
  pinMode(SMOKE, INPUT);
  pinMode(DOOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(FIRE_EXT, OUTPUT);
  pinMode(PILOT, OUTPUT);
  
  digitalWrite(BUZZER, LOW);
  digitalWrite(FAN, LOW);
  digitalWrite(FIRE_EXT, LOW);
  digitalWrite(PILOT, LOW);

  tini_fan = millis(); //Inicializo por primera vez tini debido a que se usara de una vez en el void loop
  tini_alarm = millis();
  tini_emergency = millis();
}

void loop(void)
{ 
  tactual = millis(); //Tomo el tactual
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp = sensors.getTempCByIndex(0); // Store temperature

  switch (nxstate) {
    case ECOOL:
      if(temp > temp_high && digitalRead(SMOKE) == LOW){
        nxstate = EEMERGENCY;
        tini_emergency = millis();
        tini_alarm = millis();
        t_alarm_off = 500;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
        digitalWrite(FAN, LOW);
      }
      else if(temp > temp_high){
        nxstate = ETEMP;
        tini_alarm = millis();
        t_alarm_off = 1000;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
      }
      else if(digitalRead(SMOKE) == LOW){
        nxstate = ESMOKE;
        tini_alarm = millis();
        t_alarm_off = 1000;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
      }
      else if(digitalRead(DOOR) == HIGH){
        nxstate = EDOOROPEN;
        tini_dooropen = millis();
      }
      else{
        nxstate = ECOOL;
      }
    break;

    case ESMOKE:
      digitalWrite(FAN, HIGH);
      if(temp > temp_high && digitalRead(SMOKE) == LOW){
        nxstate = EEMERGENCY;
        tini_emergency = millis();
        tini_alarm = millis();
        t_alarm_off = 500;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
        digitalWrite(FAN, LOW);
      }
      else if(temp < temp_low){
        nxstate = ECOOL;
        digitalWrite(BUZZER, LOW);
        digitalWrite(PILOT, LOW);
        alarm_flag = false;
        fan_flag = true;
        digitalWrite(FAN, LOW);
      }
      else{
        nxstate = ESMOKE;
      }
    break;

    case ETEMP:
      digitalWrite(FAN, HIGH);
      if(temp > temp_high && digitalRead(SMOKE) == LOW){
        nxstate = EEMERGENCY;
        tini_emergency = millis();
        tini_alarm = millis();
        t_alarm_off = 500;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
        digitalWrite(FAN, LOW);
      }
      else if(digitalRead(SMOKE) == HIGH){
        nxstate = ECOOL;
        digitalWrite(BUZZER, LOW);
        digitalWrite(PILOT, LOW);
        alarm_flag = false;
        fan_flag = true;
        digitalWrite(FAN, LOW);
      }
      else{
        nxstate = ESMOKE;
      }
    break;

    case EDOOROPEN:
      tstate_dooropen = tactual - tini_dooropen;
      if(temp > temp_high && digitalRead(SMOKE) == LOW){
        nxstate = EEMERGENCY;
        tini_emergency = millis();
        tini_alarm = millis();
        t_alarm_off = 500;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
        digitalWrite(FAN, LOW);
      }
      else if(temp > temp_high){
        nxstate = ETEMP;
        tini_alarm = millis();
        t_alarm_off = 1000;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
      }
      else if(digitalRead(SMOKE) == LOW){
        nxstate = ESMOKE;
        tini_alarm = millis();
        t_alarm_off = 1000;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
      }
      else if(digitalRead(DOOR) == LOW){
        nxstate = ECOOL;
        digitalWrite(BUZZER, LOW);
        digitalWrite(PILOT, LOW);
        alarm_flag = false;
        fan_flag = true;
        digitalWrite(FAN, LOW);
      }
      else if(tstate_dooropen >= t_door_open){
        nxstate = EDOORALARM;
        t_alarm_off = 3000;
        t_alarm_on = 500;
        alarm_flag = true;
      }
      else{
        nxstate = EDOOROPEN;
      }
    break;

    case EDOORALARM:
      if(temp > temp_high && digitalRead(SMOKE) == LOW){
        nxstate = EEMERGENCY;
        tini_emergency = millis();
        tini_alarm = millis();
        t_alarm_off = 500;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
        digitalWrite(FAN, LOW);
      }
      else if(temp > temp_high){
        nxstate = ETEMP;
        tini_alarm = millis();
        t_alarm_off = 1000;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
      }
      else if(digitalRead(SMOKE) == LOW){
        nxstate = ESMOKE;
        tini_alarm = millis();
        t_alarm_off = 1000;
        t_alarm_on = 500;
        alarm_flag = true;
        fan_flag = false;
      }
      else if(digitalRead(DOOR) == LOW){
        nxstate = ECOOL;
        digitalWrite(BUZZER, LOW);
        digitalWrite(PILOT, LOW);
        alarm_flag = false;
        fan_flag = true;
        digitalWrite(FAN, LOW);
      }
      else{
        nxstate = EDOORALARM;
      }
    break;

    case EEMERGENCY:
      tstate_emergency = tactual - tini_emergency;
      if(temp < temp_low && digitalRead(SMOKE) == HIGH && tstate_emergency >= t_emergency){
        nxstate = ECOOL;
        digitalWrite(BUZZER, LOW);
        digitalWrite(PILOT, LOW);
        alarm_flag = false;
        fan_flag = true;
        digitalWrite(FAN, LOW);
      }
      else{
        nxstate = EEMERGENCY;
      }
    break;
  }

  // MEF ventilador
  if(fan_flag == true){
    switch(nxstate_fan){
      case EFANOFF:
        tstate_fan = tactual - tini_fan;
        digitalWrite(FAN, LOW);
        if(tstate_fan >= t_fan_off){
          nxstate_fan = EFANON;
          tini_fan = millis();
          //Serial.println("FAN ON");
        }
        else{
          nxstate_fan = EFANOFF;
        }
      break;
      case EFANON:
        tstate_fan = tactual - tini_fan;
        digitalWrite(FAN, HIGH);
        if(tstate_fan >= t_fan_on){
          nxstate_fan = EFANOFF;
          tini_fan = millis();
          //Serial.println("FAN OFF");
        }
        else{
          nxstate_fan = EFANON;
        }
      break;
    }
  }

// MEF alarma
  if(alarm_flag == true){
    switch(nxstate_alarm){
      case EALARMOFF:
        tstate_alarm = tactual - tini_alarm;
        digitalWrite(BUZZER, LOW);
        digitalWrite(PILOT, LOW);
        if(tstate_alarm >= t_alarm_off){
          nxstate_alarm = EALARMON;
          tini_alarm = millis();
          //Serial.println("ALARM ON");
        }
        else{
          nxstate_alarm = EFANOFF;
        }
      break;
      case EALARMON:
        tstate_alarm = tactual - tini_alarm;
        digitalWrite(BUZZER, HIGH);
        digitalWrite(PILOT, HIGH);
        if(tstate_alarm >= t_alarm_on){
          nxstate_alarm = EALARMOFF;
          tini_alarm = millis();
          //Serial.println("ALARM OFF");
        }
        else{
          nxstate_alarm = EALARMON;
        }
      break;
    }
  }
   
}
