#include <SoftwareSerial.h>
#define RxD 0
#define TxD 1
String password = "12345";
SoftwareSerial blueToothSerial(RxD, TxD);
int relay = 2;
int led = 3;
String messaggio;
bool ricevuto = LOW;

void setup()
{
  blueToothSerial.begin(38400);
  delay(1000);
  blueToothSerial.flush();
  delay(2000);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
}

void loop()
{
  char recvChar;

  while (blueToothSerial.available())
  {
    recvChar = blueToothSerial.read();
    messaggio += recvChar;
    ricevuto = HIGH;
  }
  if (ricevuto == HIGH)
  {
    delay(200);
    if (messaggio == password)
    {
      digitalWrite(relay, HIGH);
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(relay, LOW);
      delay(1000);
      digitalWrite(led, LOW);
      messaggio = "";
      ricevuto = LOW;
      for (int i = 0; i < 3; i++)
      {
        blueToothSerial.print(1);
        delay(50);
      }
    }
    else
    {
      for (int i = 0; i < 3; i++)
      {
        blueToothSerial.print(0);
        delay(50);
      }
      ricevuto = LOW;
      messaggio = "";
    }
  }
}
