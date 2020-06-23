#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#include <time.h>
#include <MQ135.h>
#include <math.h>


//#define FIREBASE_HOST "seminar-e150d.firebaseio.com"
//#define FIREBASE_AUTH "B3PigEgmADJ77MqyoZ4rNmoAmHQysQKvOduLjZnM"

#define FIREBASE_HOST "iottest1-e66c5.firebaseio.com"
#define FIREBASE_AUTH "AH2q9yfh7L0JIAvomNef7Yl5146TBvqTM2Ww8yWJ"

//#define WIFI_SSID "KAI Coffee"
//#define WIFI_PASSWORD "kaicoffeeshop"

//#define WIFI_SSID "AnKiet"
//#define WIFI_PASSWORD "30082017"

#define WIFI_SSID "Khanhlinh"
#define WIFI_PASSWORD "02062015"

//#define WIFI_SSID "THREE O'CLOCK"
//#define WIFI_PASSWORD "3open24h"


#define DHTPIN 14  

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

int timezone = 7 * 3600;
int dst = 0;

void setup() {

  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  dht.begin();
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST);  

  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK");   

}

void loop() {

  float h = dht.readHumidity();

  float t = dht.readTemperature();

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  ////////////////////////////////////////////////////////////
   MQ135 gasSensor = MQ135(A0);
  int sensorValue = analogRead(A0);

 
  float rzero = gasSensor.getRZero();

  float co2_ppm = gasSensor.getPPM();

  int ppm = co2_ppm / 4;

  //Vrl = val * ( 5.00 / 1024.0  );      // V

  //Rs = 20000 * ( 5.00 - Vrl) / Vrl ;   // Ohm 

  //ratio =  Rs/Ro;

  ////////////////////////////////////////////////////////////

  if (isnan(h) || isnan(t)) {

    Serial.println("Failed to read from DHT sensor!");

    return;
  }
  Serial.print("Humidity: ");  Serial.print(h);
  String fireHumid = String(h) + String("%");                                         //convert integer humidity to string humidity 
  Serial.print("%  Temperature: ");  Serial.print(t);  Serial.print("°C ");
  String fireTemp = String(t) + String("°C");                                                     //convert integer temperature to string temperature
 
 String datetime =  String(p_tm->tm_mday) + ("-") + String(p_tm->tm_mon + 1)+ ("-") + String(p_tm->tm_year + 1900)+(" ")+String(p_tm->tm_hour)+ (":")+String(p_tm->tm_min)+(":")+String(p_tm->tm_sec)  ;
 Serial.println(datetime);


///////////////////////////////////////////////////////////////
     
  Serial.println("rzero=" + (String)rzero + "    ");

  Serial.println("co2 ppm=" + (String)co2_ppm + "    ");

  Serial.println("A0=" + (String)sensorValue + " Resis="+(String)gasSensor.getResistance() +" ");

 //////////////////////////////////////////////////////////////
  
  delay(2000);


  
  Firebase.setString("Humidity", fireHumid);                                  //setup path and send readings
  Firebase.setString("Temp", fireTemp);
  Firebase.setString("DateTime", datetime);
  Firebase.setString("AirQuality", (String)sensorValue); 
  Firebase.setString("CO2ppm", (String)co2_ppm);

 
  
}
