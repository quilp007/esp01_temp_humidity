#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include "time.h"
 
//#define PIN_DHT D4
#define PIN_DHT 2
 
const char* ssid     = "myWifi_2.4";
const char* password = "01065087677";
String host = "http://203.251.78.135";

const long interval = 1*60*1000;
unsigned long previousMillis = 0;

time_t now;
time_t prevEpoch;
 
WiFiServer server(80);
WiFiClient client;
HTTPClient http;
String mac;
DHT DHTsensor(PIN_DHT, DHT11);

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
	
	Serial.begin(115200);

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

 #if 0
 void loop() {
  printLocalTime();
 }

 #else
void loop() {

   //printLocalTime();
 
	// 센서값 DB 전송 부분
	unsigned long currentMillis = millis();
	if(currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		float humidity = DHTsensor.readHumidity();
		float temp = DHTsensor.readTemperature();
		
    Serial.println(temp);
    Serial.println(humidity);

		//String phpHost = host+"/insert.php?temp="+String(temp)+"&humidity="+String(humidity);
    /*
		String phpHost = host+"/insert_t1.php?CD_LOC=" + String(1) + "&QT_TEMP=" + String(int(temp)) +\
                                          "&QT_HUMI=" + String(int(humidity)) + "&QT_PRESSURE=" + String(10) +\
                                          "&YN_ABNORMAL=N" + "&DC_RMK=NA" + "&DT_REG=" + "20221203" +\
                                          "&TM_REG=" + currentDateTime;                                       

		String phpHost = host+"/insert_t1.php?CD_LOC=" + String(1) + "&QT_TEMP=" + String(int(temp)) +\
                                          "&QT_HUMI=" + String(int(humidity)) + "&QT_PRESSURE=" + String(10) +\
                                          "&YN_ABNORMAL=N" + "&DC_RMK=" + mac + "&DT_REG=" + currentDate +\
                                          "&TM_REG=" + currentDateTime;   ;                                       
    */
		String phpHost = host+"/insert_t1.php?CD_LOC=" + String(1) + "&QT_TEMP=" + String(int(temp)) +\
                                          "&QT_HUMI=" + String(int(humidity)) + "&QT_PRESSURE=" + String(10) +\
                                          "&YN_ABNORMAL=N" + "&DC_RMK=" + mac;                                       

		Serial.print("Connect to ");
		Serial.println(phpHost);
		
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
 
	}
 
 /*	
	// 웹서버 부분
	client = server.available();
	if(!client) return;
 
	Serial.println("새로운 클라이언트");
	client.setTimeout(5000);
 
	String request = client.readStringUntil('\r');
	Serial.println("request: ");
	Serial.println(request);
 
	while(client.available()) {
		client.read();
	}
 
	float humidity = DHTsensor.readHumidity();
	float temp = DHTsensor.readTemperature();
 
	client.print("HTTP/1.1 200 OK");
	client.print("Content-Type: text/html\r\n\r\n");
	client.print("<!DOCTYPE HTML>");
	client.print("<html>");
	client.print("<head>"); 
	client.print("<meta charset=\"UTF-8\" http-equiv=\"refresh\" content=\"1\">");
	client.print("<title>DHT senrsor test Webpage</title>");
	client.print("</head>");
	client.print("<body>");
	client.print("<h2>DHT senrsor test Webpage</h2>");
	client.print("<br>");
	client.print("Temperature : ");
	client.print(temp);
	client.print(" °C");
	client.print("<br>");
	client.print("Humidity : ");
	client.print(humidity);
	client.print(" %");
	client.print("</body>");
	client.print("</html>");
 
	Serial.println("클라이언트 연결 해제");
  */  
  }
#endif
