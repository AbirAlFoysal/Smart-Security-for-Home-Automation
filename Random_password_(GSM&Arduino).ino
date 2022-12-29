#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
long EEPROMReadlong(int address);
void EEPROMWritelong(int address, long value);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String Data;
byte data_count = 0;
char customKey;
int a = 0;
int wrongAttemptOwner = 0;
int wrongAttemptGuest = 0;
const int ownerAddress = 0;
const int guestAddress = 10;
String ownerPassword = String(EEPROMReadlong(ownerAddress));
String guestPassword = "A" + String(EEPROMReadlong(guestAddress));
const String warningOwnerMessage = "Someone has tried to access Your Home as an OWNER. Please Take Action.......";
const String warningGuestMessage = "Someone has tried to access Your Home as a GUEST. Please Take Action.......";
const String newOwnerPasswordMessage = "New Password for Owner : ";
const String newGuestPasswordMessage = "New Password for Guest : ";

const int maxAttemptOwner = 3;
const int maxAttemptGuest = 3;

// owners phone number
const String phoneNumber = "your phone number here";

void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(10, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  randomSeed(analogRead(0));
}

void loop()
{
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  int window1 = digitalRead(11);
  int window2 = digitalRead(12);

  if (window1 == 0 || window2 == 0)
  {
    for (int q = 1; q <= 10; q += 1)
      buzzer();

    sendMessage("Someone has broke Your Window. Please Take Action.");
  }

  customKey = customKeypad.getKey();
  if (customKey == '*')
  {
    Serial.println("Inter Data :");
    a = 1;
  }

  if (customKey == 'C')
  {
    lcd.clear();
    clearData();
  }

  if (a == 1)
  {
    if (customKey)
    {
      if (customKey == '*' || customKey == 'D')
      {
      }
      else
      {
        Data += char(customKey);
        Serial.println(customKey);
        lcd.setCursor(data_count, 1);
        lcd.print(customKey);
        data_count++;
      }
    }
    if (customKey == 'D')
    {
      Serial.println(Data);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please Wait........");

      if (isDigit(Data[0]))
      {
        if (Data == ownerPassword)
        {
          wrongAttemptOwner = 0;

          successAlarm();
          Serial.println(Data);
        }
        else
        {
          for (int q = 0; q <= 5; q += 1)
          {
            buzzer();
          }

          sendMessage(warningOwnerMessage);

          wrongAttemptOwner++;
          if (wrongAttemptOwner >= maxAttemptOwner)
          {
            int ownerPasswordValue = random(10000, 99999);
            EEPROMWritelong(ownerAddress, ownerPasswordValue);
            ownerPassword = String(ownerPasswordValue);

            wrongAttemptOwner = 0;
            sendMessage("Owner Password is reset because of maximum wrong attempt. \n" + newOwnerPasswordMessage + ownerPassword);
          }
        }
      }
      else
      {
        if (Data == guestPassword)
        {
          int guestPasswordValue = random(1000, 9999);
          EEPROMWritelong(guestAddress, guestPasswordValue);
          guestPassword = "A" + String(guestPasswordValue);
          wrongAttemptGuest = 0;
          sendMessage("Previous Guest Password is Used. \n" + newGuestPasswordMessage + guestPassword);

          successAlarm();
          Serial.println(Data);
        }
        else
        {
          for (int q = 0; q <= 5; q += 1)
          {
            buzzer();
          }

          sendMessage(warningGuestMessage);

          wrongAttemptGuest++;
          if (wrongAttemptGuest >= maxAttemptGuest)
          {
            int guestPasswordValue = random(1000, 9999);
            EEPROMWritelong(guestAddress, guestPasswordValue);
            guestPassword = "A" + String(guestPasswordValue);
            wrongAttemptGuest = 0;

            sendMessage("Guest Password is reset because of maximum wrong attempt. \n" + newGuestPasswordMessage + guestPassword);
          }
        }
      }
      clearData();
    }
  }
  delay(10);
}

void clearData() {
  Serial.println("clear Data......");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please Wait........");
  delay(500);
  a = 0;
  data_count = 0;
  String asd;
  Data = asd;
  lcd.clear();
  return;
}

void sendMessage(String message) {
  Serial.println("AT+CMGF=1");                    //Sets the GSM Module in Text Mode
  delay(1000);                                    // Delay of 1000 milli seconds or 1 second
  Serial.println("AT+CMGS=\"" + phoneNumber + "\"\r"); // Replace x with mobile number
  delay(1000);
  Serial.println(message); // The SMS text you want to send
  delay(100);
  Serial.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
}

void buzzer() {
  digitalWrite(A1, HIGH);
  digitalWrite(10, HIGH);
  delay(300);
  digitalWrite(A1, LOW);
  digitalWrite(10, LOW);
  delay(300);
}

void successAlarm() {
  digitalWrite(A0, HIGH);
  digitalWrite(10, HIGH);
  delay(300);
  digitalWrite(10, LOW);
  delay(1000);
  digitalWrite(A0, LOW);
}

long EEPROMReadlong(int address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWritelong(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}
