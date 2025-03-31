//  6 Channel Receiver | 6 Kanal Alıcı
// KendinYap Channel

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define CE_PIN 9
#define CSN_PIN 10

#define LOOPLED A6

#define BLINKRATE 0x01FF

uint16_t loopcounter = 0;

uint8_t impulscounter = 0;
uint16_t resetcounter = 0;
uint16_t radiocounter = 1;

int ch_width_1 = 0;
int ch_width_2 = 0;
int ch_width_3 = 0;
int ch_width_4 = 0;
int ch_width_5 = 0;
int ch_width_6 = 0;

Servo ch1;
Servo ch2;
Servo ch3;
Servo ch4;
Servo ch5;
Servo ch6;

struct Signal {

byte throttle;
byte pitch;  
byte roll;
byte yaw;
byte aux1;
byte aux2;
    
};

Signal data;

const uint64_t pipeIn = 0xABCDABCD71LL;
RF24 radio(CE_PIN, CSN_PIN); 

void ResetData()
{

data.throttle = 0;                                         // Define the inicial value of each data input. | Veri girişlerinin başlangıç değerleri
data.roll = 127;
data.pitch = 127;
data.yaw = 127;
data.aux1 = 0;                                              
data.aux2 = 0;
resetcounter++;
                                                    
}

void setup()
{
  Serial.begin(19200);
  pinMode(LOOPLED,OUTPUT);
  
  // Set the pins for each PWM signal | Her bir PWM sinyal için pinler belirleniyor.
  ch1.attach(2);
  ch2.attach(3);
  ch3.attach(4);
  ch4.attach(5);
  ch5.attach(6);
  ch6.attach(7);
                                                           
  ResetData();                                             // Configure the NRF24 module  | NRF24 Modül konfigürasyonu
  radio.begin();
  radio.openReadingPipe(1,pipeIn);
  radio.setChannel(124);
  radio.setAutoAck(false);
  //radio.setDataRate(RF24_250KBPS);    // The lowest data rate value for more stable communication  | Daha kararlı iletişim için en düşük veri hızı.
  radio.setDataRate(RF24_2MBPS); // Set the speed of the transmission to the quickest available
  radio.setPALevel(RF24_PA_MAX);                           // Output power is set for maximum |  Çıkış gücü maksimum için ayarlanıyor.
  radio.setPALevel(RF24_PA_MIN); 
  radio.setPALevel(RF24_PA_MAX); 
  
  radio.startListening(); 
   if (radio.failureDetected) 
  {
    radio.failureDetected = false;
    delay(250);
    Serial.println("Radio failure detected, restarting radio");
  }
  else
  {
    Serial.println("Radio OK"); 
  }
// Start the radio comunication for receiver | Alıcı için sinyal iletişimini başlatır.
  ResetData();
}

unsigned long lastRecvTime = 0;

void recvData()
{
  while ( radio.available() ) 
  {
    radiocounter++;
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis();                                    // Receive the data | Data alınıyor
  }
}

void loop()
{
  loopcounter++;
  
  if(loopcounter >= BLINKRATE)
  {
    loopcounter = 0;
    impulscounter+=16;
    digitalWrite(LOOPLED, ! digitalRead(LOOPLED));
    
    Serial.print("roll: ");
    Serial.print(data.roll);
    
    Serial.print("\tpitch: ");
    Serial.print(data.pitch);
   
    Serial.print("\tyaw: ");
    Serial.print(data.yaw);

    Serial.print("\tthrottle: ");
    Serial.print(data.throttle);

    /*
    Serial.print("\tradio: ");
    Serial.print(radiocounter);

    Serial.print("\timpuls: ");
    Serial.print(impulscounter);

    */
    Serial.print("\treset: ");
    Serial.print(resetcounter);
    Serial.print("\n");
  }
  
recvData();
unsigned long now = millis();
if ( now - lastRecvTime > 1000 ) 
{
ResetData();                                                // Signal lost.. Reset data | Sinyal kayıpsa data resetleniyor
}

//data.roll = (impulscounter & 0xFF );//& 0xFF00) >> 8;
ch_width_1 = map(data.roll, 0, 255, 1000, 2000);

ch_width_2 = map(data.pitch, 0, 255, 1000, 2000); 
ch_width_3 = map(data.throttle, 0, 255, 800, 2200); 
ch_width_4 = map(data.yaw, 0, 255, 1000, 2000); 
ch_width_5 = map(data.aux1, 0, 1, 1000, 2000); 
//ch_width_6 = map(data.aux2, 0, 1, 1000, 2000); 
ch_width_6 = map((impulscounter & 0xFF ), 0, 255, 1000, 2000);

ch1.writeMicroseconds(ch_width_1);                          // Write the PWM signal | PWM sinyaller çıkışlara gönderiliyor
ch2.writeMicroseconds(ch_width_2);
ch3.writeMicroseconds(ch_width_3);
ch4.writeMicroseconds(ch_width_4);
ch5.writeMicroseconds(ch_width_5);
ch6.writeMicroseconds(ch_width_6); 

}
