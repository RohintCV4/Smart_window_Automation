#include <Servo.h>

#include "DHT.h"
#define DHTPIN 4 
#define DHTTYPE DHT11
  
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;  
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  aa
#include <ArduinoJson.h>
#define BOTtoken "5743225904:AAGxvy5wd8Uboa0Eojv5lsUoojZ1524XI8A" 
#define CHAT_ID "1156498647"
const char* ssid = "redmi123";
const char* password = "12345678";
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
const int ledPin = 2;
bool ledState = LOW;
int pos = 0;
int val;
int val2;
String aut;


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);



void open(){
  for (pos = val; pos <= 90; pos += 1) { 
    myservo.write(pos);              
    delay(15);                       
  }
}
void clos(){
  for (pos =  val2; pos >= 0; pos -= 1) {
    myservo.write(pos);             
    delay(15);                
  }
}

//for humidity---------------
int humidity(){

  delay(2000);

  float h = dht.readHumidity();

  if (isnan(h)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return h;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% "));
  
  Serial.println(F("------------"));

  return h;
  
}

void autu(){
  int hum=humidity();

  if (hum>=60&& hum<=68){
    val=90;
    open();
   }
   else{
    val2=0;
    clos();
   }
}


void handleNewMessages(int numNewMessages) {
  Serial.print("NewMessages :");
  Serial.print(String(numNewMessages));
  Serial.println(" ");

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your windows.\n\n";
      welcome += "/openwindows  \n";
      welcome += "/closewindows \n";
      welcome += "/state to request current state \n";
      welcome += "/auto to request automatic window control \n";
      bot.sendMessage(chat_id, welcome, "");
    }


    if (text == "/openwindows") {
      bot.sendMessage(chat_id, "The window is open", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      open();

    }
    
    if (text == "/closewindows") {
      bot.sendMessage(chat_id, "The window is closed", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      clos();
    }
    
    if (text == "/state") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "The window is open", "");
      }
      else{
        bot.sendMessage(chat_id, "The window is closed", "");
      }
    }
    

    if(text== "/auto"){
      bot.sendMessage(chat_id, "automatic window control is enabled", "");
      aut="yes";
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    else{
      aut="no";
       ledState = LOW;
      digitalWrite(ledPin, ledState);
      }

  }
}

void setup() {
  Serial.begin(115200);
    myservo.attach(18); 
  Serial.println(F("DHT11 & servo test!"));

  dht.begin();


  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (aut=="yes"){
    autu();
    }

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
