#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(D3, D4);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char* ssid     = "POCO X3 NFC";   //replace with your own SSID
const char* password = "Plabon21";    //replace with your own password
const char* host = "api.pushingbox.com";

//int weekDays[7]={1, 2, 3, 4, 5, 6, 7};
int weekDay;
int currentHour;
int course;
 
String member = "";
String HostID = "";
int flag = 0;
void setup()
{
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("WELCOME");
  delay(2000);

  lcd.clear();

  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  lcd.setCursor(0, 0);
  Serial.print("Connecting to WiFi...       ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected          ");
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.setCursor(4, 1);
  lcd.print(WiFi.localIP());
  delay(1500);



  while (!Serial);
  delay(100);
  Serial.println("\n\n Waiting for Fingerprint Sensor");
  lcd.setCursor(0, 0);
  lcd.print("    Waiting for           ");
  lcd.setCursor(0, 1);
  lcd.print("      Sensor              ");
  delay(1500);
  finger.begin(57600);


   if (finger.verifyPassword())
  {
    Serial.println("Found Successfully");
    lcd.setCursor(0, 0);
    lcd.print("Sensor found!         ");
    lcd.setCursor(0, 1);
    lcd.print("                      ");
    delay(1500);
  } else
  {
    Serial.println("Fingerprint sensor not found!!!");
    lcd.setCursor(0, 0);
    lcd.print("Sensor Not found!         ");
    lcd.setCursor(0, 1);
    lcd.print("                      ");
    while (1)
    {
      delay(1);
    }
  }
  timeClient.begin();
  timeClient.setTimeOffset(21600);
}



void loop()
{
  timeClient.update();
  
  currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour); 

  weekDay = timeClient.getDay();
  Serial.print("Week Day: ");
  Serial.println(weekDay);

  if ((weekDay==2)&&(currentHour==19))
  { 
    course=1;
  }
  else if ((weekDay==2)&&(currentHour==20))
  {
    course=2;
  }
  
  int fingerprintID = getFingerprintID();
  delay(50);

  int i;
  for (i=1;i<=33;i++)
  {
    if (fingerprintID == i)
    {
      Serial.println("Welcome ");
      Serial.println(i);
      lcd.setCursor(0, 0);
      lcd.print("Welcome ");
      //lcd.setCursor(0, 1);
      //lcd.print(i);
      String myStr;
      String stringOne;
      String newStr;
      if (i<10)
      {
        stringOne = String("170600");
      }
      else
      {
        stringOne = String("17060");
      }
      myStr = String(i);
      newStr = stringOne + myStr;
      lcd.print(newStr);
      connecthost(newStr,"01");
      flag = 0;
      }
    else
    {
      Serial.println("Waiting for valid finger!!!");
      lcd.setCursor(0, 0);
      lcd.print("Place a Finger         ");
      //lcd.setCursor(0, 1);
     // lcd.print("     Finger           ");
    }
  }
//  if (fingerprintID == 1)
//  {
//    Serial.println("Welcome Abdullah");
//    lcd.setCursor(0, 0);
//    lcd.print("Welcome Abdullah        ");
//    lcd.setCursor(0, 1);
//    lcd.print("                       ");
//    connecthost("Abdullah","01");
//    flag = 0;
//  }
//  else if (fingerprintID == 2)
//  {
//    Serial.println("Welcome Mahmood");
//    lcd.setCursor(0, 0);
//    lcd.print("Welcome Mahmood          ");
//    lcd.setCursor(0, 1);
//    lcd.print("                       ");
//    connecthost("Mahmood","02");
//    flag = 0;
//  }
//  else if (fingerprintID == 3)
//  {
//    Serial.println("Welcome Plabon");
//    lcd.setCursor(0, 0);
//    lcd.print("Welcome Plabon           ");
//    lcd.setCursor(0, 1);
//    lcd.print("                       ");
//    connecthost("Plabon","03");
//    flag = 0;
//  }
//  else if (fingerprintID == 15)
//  {
//    Serial.println("Welcome Jubaier");
//    lcd.setCursor(0, 0);
//    lcd.print("Welcome  Jubaier         ");
//    lcd.setCursor(0, 1);
//    lcd.print("                       ");
//    connecthost("Jubaier","15");
//    flag = 0;
//  }
//  else
//  {
//    Serial.println("Waiting for valid finger!!!");
//    lcd.setCursor(0, 0);
//    lcd.print(" Place a Valid       ");
//    lcd.setCursor(0, 1);
//    lcd.print("     Finger           ");
//  }
}

int getFingerprintID()
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  return finger.fingerID;
}

void connecthost(String data, String ID)
{
  if (flag == 0)
  {
    member = data;
    HostID = ID;
    flag = 1;
    Serial.print("connecting to ");
    Serial.println(host);
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) 
    {
      Serial.println("connection failed");
      return;
    }

    // We now create a URL for the request
    String url = "/pushingbox?";
    url += "devid=";

    if (course == 1)
    {
      url += "vE93091CE0058720";
    } else if (course == 2){
      url += "v16D00EA7AA373DB";
    }
    

    url += "&Name=" + String(member);
    url += "&StudentID=" + String(HostID);

    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      Serial.print("Data Sent!");
    }

    Serial.println();
    Serial.println("closing connection");
  }
}
