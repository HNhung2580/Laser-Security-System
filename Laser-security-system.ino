
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLLSdREmMn"
#define BLYNK_DEVICE_NAME "BLINK LED"
#define BLYNK_AUTH_TOKEN "roRcyuyiqX7X7Rsj_wAoMr5I4c7PI1qp"

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "roRcyuyiqX7X7Rsj_wAoMr5I4c7PI1qp";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "abcde";
char pass[] = "1234567891";

#define led D0
#define RST_PIN D3
#define SS_PIN D4
#define trangthaicanhbao V3

int servoPin = D1;
Servo myservo;

int button1;
int button;
int UID[4], i;

int ID1[4] = {01, 135, 177, 29}; //Thẻ bật laser và quang trở // Mã Card mở cửa
int temp[4];
MFRC522 mfrc522(SS_PIN, RST_PIN);

int laserPin = D2;
int sensorPin = A0;
int buzzerPin = D8;

int led1 = D0;

int quangtro = 0;
bool state = true;
bool stateLaser = true;
bool stateBuzzer = false;
int count = 0;
bool buzzerKeu = false;
int buzzer = 0;
bool sta = false;
WidgetLED ledd(V3);

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  //pinMode(servoPin, OUTPUT);
  myservo.attach(servoPin);
  Blynk.begin(auth, ssid, pass);

  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(laserPin, OUTPUT);
  pinMode(led1, OUTPUT);

}

BLYNK_WRITE(V0) {
  button1 = param.asInt();
  if (button1 == 1) {
    Blynk.virtualWrite(V5, quangtro);
  }
  else {
    Blynk.virtualWrite(V5, 0);
  }
}

BLYNK_WRITE(V2) {
  button = param.asInt();
  if (button == 1) {
    state = true;
  }
  else {
    state = false;
  }
}

BLYNK_WRITE(V3) {
  quangtro = param.asInt();
  if (quangtro == 1) {
    state = true;
  }
  else {
    state = false;
  }
}

BLYNK_WRITE(V6) {
  int button6 = param.asInt();
  if (button6 == 1) {
    sta = true;
  }
  else {
    sta = false;
  }
}


void loop()
{
  Blynk.run();
  //Blynk.virtualWrite(V5, quangtro);
  if (state == true) Serial.print("true");
  else Serial.print("false");
  if (stateLaser == true)  Serial.print(" true");
  else Serial.print(" false");
  if (stateBuzzer == true)  Serial.print(" true");
  else Serial.print(" false ");

  if (state == true) {
    digitalWrite(led1, HIGH);
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      quangtro = analogRead(sensorPin);
      Serial.println(quangtro);
      Serial.println(count);


      if ((stateLaser == true && quangtro > 700 && count < 3) || (stateLaser == false && count < 3)) {
        stateBuzzer = false;
      }
      else {
        //tone(buzzerPin, 1500);
        stateBuzzer = true;
        buzzerKeu = true;
        
        //        myservo.write(180);
        Blynk.logEvent("warning", String("Cảnh báo có trộm"));
        Blynk.notify("Warning: trộm vào nhà");
        digitalWrite(buzzerPin, HIGH);
        
        Serial.println("Đã bật cảnh báo!");
//        Blynk.notify("Warning: trộm vào nhà");
        quangtro=1;
      }
      digitalWrite(led1, HIGH);
      if (state == true) {
        if (stateLaser == true) {
          digitalWrite(laserPin, LOW);
        } else {
          digitalWrite(laserPin, HIGH);
        }
        if (stateBuzzer == true) {
          tone(buzzerPin, 5000);
          sta = true;
          ledd.on();
          Blynk.virtualWrite(trangthaicanhbao, HIGH);
          digitalWrite(led1, HIGH);
          delay (200);
          digitalWrite(led1, LOW);
          delay (200);
        } else {
          noTone(buzzerPin);
          Blynk.virtualWrite(trangthaicanhbao, LOW);
        }
      } else {
        stateLaser = false;
        noTone(buzzerPin);
        stateBuzzer = false;
        digitalWrite(laserPin, HIGH);
      }
      return;
    }

    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }

    Serial.print("UID của thẻ: ");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      UID[i] = mfrc522.uid.uidByte[i];
      temp[i] = UID[i];
      Serial.print(UID[i]);
    }
    Serial.println("   ");

    if (temp[i] == ID1[i])
    {
      if (stateBuzzer == true)
      {
        noTone(buzzerPin);
        stateBuzzer = false;
      }
      if ( stateLaser == false) {
        stateLaser = true;
        digitalWrite(laserPin, LOW);
        count = 0;
      }
      else {
        stateLaser = false;
        digitalWrite(laserPin, HIGH);
        count = 0;
        myservo.write(60);
        delay(3000);
        stateLaser = true;
        digitalWrite(laserPin, LOW);
        myservo.write(180);
        // state = false;
        quangtro = 10;
      }
    }
    else
    {
      count++;
      if (count == 3) {
        quangtro = 1000;
      }
      Serial.println("Sai thẻ");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
  else {
    digitalWrite(led1, LOW);
    Serial.println("Hệ thống đã tắt");
    stateLaser = false;
    noTone(buzzerPin);
    stateBuzzer = false;
    digitalWrite(laserPin, HIGH);
  }

}
