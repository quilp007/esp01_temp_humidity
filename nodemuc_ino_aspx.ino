#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include "time.h"
 
//#define PIN_DHT D4
#define PIN_DHT 2
 
const char* ssid     = "myWifi_2.4";
const char* password = "01065087677";
//String host = "http://203.251.78.135";
String host = "http://112.175.88.55";

//const long interval = 1*1000*60;
const long interval = 1*1000;
unsigned long previousMillis = 0;

time_t now;
time_t prevEpoch;
 
WiFiServer server(80);
WiFiClient client;
HTTPClient http;
String mac;
DHT DHTsensor(PIN_DHT, DHT11);

uint8_t cnt;

const char* ntpServer = "pool.ntp.org";
uint8_t timeZone = 9;
uint8_t summerTime = 0; // 3600

struct tm * timeinfo;

int dd, M, yy, ss, mm, hh, week;
char currentTime[8];
char currentDateTime[19];
char currentDate[8];
 
void get_NTP() {
  configTime(3600 * timeZone, 3600 * summerTime, ntpServer);
  timeinfo = localtime(&now);
  while (timeinfo->tm_year +1900 == 1970) {
    Serial.println("Failed to obtain time");
    localtime(&now);
    return;
  }
}

//'20220101', '2022-01-01 12:12:12'
void printLocalTime() {
  if (time(&now) != prevEpoch) {
    Serial.println(time(&now));  // 현재 UTC 시간 값 출력
    timeinfo = localtime(&now);
    //char *current_time = asctime(timeinfo);
    //Serial.print(current_time);
    dd    = timeinfo->tm_mday;
    M     = timeinfo->tm_mon + 1;
    yy    = timeinfo->tm_year +1900;
    ss    = timeinfo->tm_sec;
    mm    = timeinfo->tm_min;
    hh    = timeinfo->tm_hour;
    week  = timeinfo->tm_wday;

    //sprintf(currentTime, "%02d:%02d:%02d", hh, mm, ss);
    //Serial.println(currentTime);        

    sprintf(currentDate, "%4d%02d%02d", yy, M, dd);
    Serial.println(currentDate);

    sprintf(currentDateTime, "%4d-%02d-%02d %02d:%02d:%02d", yy, M, dd, hh, mm, ss);
    Serial.println(currentDateTime);        

    prevEpoch = time(&now);
  }
}
 
void setup() {
	DHTsensor.begin();
	
	Serial.begin(9600);

  mac = WiFi.macAddress();
  Serial.println(mac);
  
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connecting to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();
	Serial.println("Server started");
  
  //get_NTP();
}

StaticJsonBuffer<500> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
JsonArray& Param = root.createNestedArray("Param");
JsonObject& data = Param.createNestedObject();


String toJson(float temp, float humi, String mac)
{
  //JsonObject& BME280DATA = jsonBuffer.createObject();
  String output;

  data["NM_SP"]       = "AP_IF_PRD_THERMOHYGRO_LOG_I";
  data["YN_TRAN"]     = "Y";
  data["CD_FIRM"]     = "1000";
  data["CD_LOC"]      = "";
  data["DC_MAC"]      = mac;
  data["QT_THERMO"]   = String(temp);
  data["QT_HYGRO"]    = String(humi);
  data["QT_PRESSURE"] = "1013";
  data["YN_ABNORMAL"] = "Y";
  data["DC_RMK"]      = "N";
  data["DT_MEASURE"]  = "N";
  data["TM_MEASURE"]  = "N";

  //root.printTo(Serial);
  root.printTo(output);
  return output;
}

void loop() {

   //printLocalTime();
 
	// 센서값 DB 전송 부분
	//unsigned long currentMillis = millis();
	//if(currentMillis - previousMillis >= interval) {
		//previousMillis = currentMillis;
		float humidity = DHTsensor.readHumidity();
		float temp = DHTsensor.readTemperature();
		
    Serial.println(temp);
    Serial.println(humidity);
/*
		String phpHost = host+"/insert_t1.php?CD_LOC=" + String(1) + "&QT_TEMP=" + String(int(temp)) +\
                                          "&QT_HUMI=" + String(int(humidity)) + "&QT_PRESSURE=" + String(10) +\
                                          "&YN_ABNORMAL=N" + "&DC_RMK=" + mac;                                       
*/
    String output = toJson(temp, humidity, mac);
		String phpHost = host + String("/mob_json/APP_IF.aspx?IF=") + output;
/*
    String phpHost = host+"/mob_json/APP_IF.aspx?IF=" \
                  "\{\"Param\"\:\[\{" \
                  "\"NM_SP\"\: \"AP_IF_PRD_THERMOHYGRO_LOG_I\"\, " \ 
                  "\"YN_TRAN\"\: \"Y\"\, " \
                  "\"CD_FIRM\"\: 1000\, " \
                  "\"CD_LOC\"\: \"\"\, " \
                  "\"DC_MAC\"\: " + "\"" + mac + "\"\, " \
                  "\"QT_THERMO\"\: " + String(temp) + "\, " \
                  "\"QT_HYGRO\"\: " + String(humidity) + "\, " \
                  "\"QT_PRESSURE\"\: 0\, " \
                  "\"YN_ABNORMAL\"\: \"Y\"\, " \
                  "\"DC_RMK\"\: \"\"\, " \
                  "\"DT_MEASURE\"\: \"\"\, " \
                  "\"TM_MEASURE\"\: \"\" " \
                  "\}\]" \
                  "\} ";                                      
*/

		Serial.print("Connect to ");
		//Serial.println(phpHost);
		
		http.begin(client, phpHost);
		http.setTimeout(1000);
		int httpCode = http.GET();
   
		if(httpCode > 0) {
			Serial.printf("GET code : %d\n\n", httpCode);
 
			if(httpCode == HTTP_CODE_OK) {
				String payload = http.getString();
				Serial.println(payload);
			}
		} 
		else {
			Serial.printf("GET failed, error: %s\n", http.errorToString(httpCode).c_str());
		}
		http.end();
	//}

  cnt = 60;
  while(cnt--){
    Serial.println(cnt);
    delay(interval);
  }  
}
