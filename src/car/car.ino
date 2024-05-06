// motor
const int PWMA1 = 3;
const int PWMA2 = 5;
const int PWMB1 = 6;
const int PWMB2 = 9;

// radio 
// https://3d-diy.ru/wiki/arduino-moduli/radio-modul-nrf24l01/
#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
#define PIN_CE  10  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 8 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN
int potValue[3]; // Создаём массив для приёма значений потенциометра

// servo
#include <using_servo.h>
ServoMotor myservo(2);

// const
const int GAS_MIN = 163;
const int GAS_MAX = 258;
const int RUL_MIN = 0;
const int RUL_MAX = 634;
const int SERVO_ANGLE = 45;
const int SERVO_CENTER = 90;

unsigned long int off_time = 0;

void setup() {
  //motor
  pinMode(PWMA1, OUTPUT);
  pinMode(PWMA2, OUTPUT);
  pinMode(PWMB1, OUTPUT);
  pinMode(PWMB2, OUTPUT);

  // radio 
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_250KBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_LOW); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу

  // servo
  // myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  myservo.write(SERVO_CENTER); 

  Serial.begin(9600);

  // motor(100,100);
  // delay(1000);
  // motor(150,0);
  // delay(1000);
  // motor(0,150);
  // delay(1000);
  // motor(-100,-100);
  // delay(1000);
  // motor(0,0);
  // delay(1000);
}

void loop() {
  if(radio.available()){ // Если в буфер приёмника поступили данные
    radio.read(&potValue, sizeof(potValue)); // Читаем показания потенциометра
    int rul = potValue[0];
    int gas = potValue[1];
    int button = potValue[2];
    // Serial.println(rul);
    // Serial.println(gas);
    // Serial.println(button);
    // Serial.println();
    rul = map(constrain(rul,RUL_MIN,RUL_MAX), RUL_MIN,RUL_MAX, SERVO_CENTER-SERVO_ANGLE,SERVO_CENTER+SERVO_ANGLE);
    gas = map(constrain(gas,GAS_MIN,GAS_MAX), GAS_MIN,GAS_MAX, -255,255);
    if (abs(gas)<80) gas = 0;
    // Serial.println(rul);
    // Serial.println(gas);
    // Serial.println(button);
    // Serial.println();
    myservo.write(rul); 
    if (button==1) motor(gas,gas);
    else block_motor();
    off_time = millis();
  }
  else if (millis()-off_time>500) {
    motor(0,0);
    myservo.write(SERVO_CENTER); 
  }
  ServoUpdate();
}

void block_motor() {
  digitalWrite(PWMA1,1);
  digitalWrite(PWMA2,1);
  digitalWrite(PWMB1,1);
  digitalWrite(PWMB2,1);
}

void motor(int a, int b) {
  analogWrite(PWMA1,constrain(a,0,255));
  analogWrite(PWMA2,constrain(-a,0,255));
  analogWrite(PWMB1,constrain(b,0,255));
  analogWrite(PWMB2,constrain(-b,0,255));
}
