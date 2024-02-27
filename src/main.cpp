#include <Arduino.h>

#include <Wire.h> // библиотека для управления устройствами по I2C

#include <RtcDS1302.h>

#define DISPLAY_ADDR 0x27

#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>              // подключаем библиотеку для QAPASS 16x2
LiquidCrystal_I2C lcd(DISPLAY_ADDR, 16, 2); // присваиваем имя LCD для дисплея

// CONNECTIONS:
// DS1302 CLK/SCLK --> 4
// DS1302 DAT/IO --> 5
// DS1302 RST/CE --> 6
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
ThreeWire myWire(5, 4, 6); // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))

// переменные для вывода
float dispTemp;
byte dispHum;
int dispPres;

void readSensors()
{
  dispTemp = bmp.readTemperature();
  // dispHum = bmp.readHumidity();
  dispPres = (float)bmp.readPressure() * 0.00750062;
}

void printDateTime(const RtcDateTime &dt)
{
  char datestring[26];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(F("Date/Time: "));
  Serial.print(datestring);
  lcd.setCursor(0, 2);
  lcd.print(datestring);
}

void setup()
{
  Serial.begin(9600); // запуск последовательного порта
  Wire.begin();
  boolean status = true;
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 1);
  lcd.print(F("RTC... "));
  Serial.print(F("RTC... "));
  Rtc.Begin();
  bmp.begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  RtcDateTime now = Rtc.GetDateTime();
  if (!Rtc.IsDateTimeValid())
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }
  delay(50);
  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }
  delay(50);
  if (Rtc.GetIsRunning())
  {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  }
  else
  {
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }

  lcd.setCursor(0, 2);
  lcd.print(F("BMP280... "));
  Serial.print(F("BMP280... "));
  delay(50);
  if (!bmp.begin())
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    delay(2000);
  }
  else
  {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  }

  if (status)
  {
    lcd.print(F("All good"));
    Serial.println(F("All good"));
  }
  else
  {
    lcd.print(F("Check wires!"));
    Serial.println(F("Check wires!"));
  }

  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                  Adafruit_BMP280::SAMPLING_X1, // temperature
                  Adafruit_BMP280::SAMPLING_X1, // pressure
                  Adafruit_BMP280::FILTER_OFF);

  readSensors();
}

void loop()
{
  // lcd.clear();
  lcd.setCursor(0, 1);
  readSensors();
  Serial.print("Temperature: ");
  Serial.println(dispTemp); // отправляем значение температуры на монитор

  // Serial.print("Humidity: ");
  // Serial.println(dispHum); // отправляем значение влажности на монитор

  Serial.print("Pres: ");
  Serial.println(dispPres); // отправляем значение давления на монитор

  Serial.println(" "); // пустая строка
  Serial.println("Date/Time: ");
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  if (!now.IsValid())
  {
    Serial.println("RTC lost confidence in the DateTime!");
  }

  delay(10000);
}
W
