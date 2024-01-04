#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
int buzzerPin = 4; // Corrected variable name
int LEDpin = 2;    // Corrected variable name
int IRsensorPin = 3;

// Define RFID tag values and corresponding messages and prices
struct RFIDMessage
{
  String tag;
  String message;
  int price;
};

RFIDMessage rfidMessages[] = {
    {"C3 99 7C 96", "Item Price:1000/=",1000},
    {"01 86 B1 1C", "Item Price:1500/=",1500},
   
    // Add more RFID tag values, messages, and prices as needed
};

int totalPrice = 0;

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600); // Initiate a serial communication
  SPI.begin();         // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(buzzerPin, OUTPUT); // Corrected variable name
  pinMode(LEDpin, OUTPUT);    // Corrected variable name
  pinMode(IRsensorPin, INPUT);
}

void loop()
{
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("WELCOME!");
  lcd.setCursor(0, 1);
  lcd.print("Scan New Product");

  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
  {
    // Show UID on the serial monitor
    Serial.print("UID tag :");
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();

    // Check the scanned RFID tag against the predefined values
    for (int i = 0; i < sizeof(rfidMessages) / sizeof(rfidMessages[0]); i++)
    {
      if (content.substring(1) == rfidMessages[i].tag)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(rfidMessages[i].message);
        digitalWrite(buzzerPin, HIGH);
        delay(400);
        digitalWrite(buzzerPin, LOW);
        Serial.println();

        // Update total price
        totalPrice += rfidMessages[i].price;

        lcd.setCursor(0, 1);
        lcd.print("Total: " + String(totalPrice) + "/=");
        delay(2000); // Display total for 2 seconds
      }
    }

    // Add a delay here to allow the RFID module to reset
    delay(10000);
  }

  // Check IR sensor
  if (digitalRead(IRsensorPin) == LOW)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PAYMENT SUCCESS");
    lcd.setCursor(0, 1);
    lcd.print("PAID " + String(totalPrice) + "/=");

    digitalWrite(LEDpin, HIGH);
    Serial.println("Got the signal");
    delay(1000); // Adjust the delay time as needed

    // Reset total price after successful payment
    totalPrice ;
  }
  else
  {
    digitalWrite(LEDpin, LOW);
    Serial.println("No signal");
    delay(1000); // Adjust the delay time as needed
  }
}
