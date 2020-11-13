//Librarys that are nessarily for the Time Module and the Servo
#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>

//Servo is defined as ventil
Servo ventil;

//The Time module is defined as rtc
RTC_DS3231 rtc;

//Variables that ar nessary for the Timer
String Null = String(0);
String Hour_Now;
String Minute_Now;
int Duration;

//Librarys that are nessarily for the Webserver
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//Webserver runs at port 80
AsyncWebServer server(80);

//Two Variables to check the current state of the time
String Timer_Time = "none";
String Current_Time;

//Enter for the connection to the WiFi
const char* ssid = "FRITZ!Box 7590 Me";
const char* password = "64030783815406180269";

//Variables for the input of the html Webserver
const char* PARAM_INPUT_time = "time_input";
const char* PARAM_INPUT_duration = "duration_input";


//In this part the main Page for the Wateringsystem is defined as variable
//written in Html and Css
const char Watering_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>Wateringsystem</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body style="background-color: #444444; font-family: sans-serif; color: #ffffff; margin: 0; padding: 0;">
    <table style="width: 100%; text-align: center;">
        <tr style="height: 5%;">
            <th style="width: 5%;"></th>
            <th style="width: 90%; vertical-align: center;"><h1 style="margin: 0;">Wateringsystem</h1></th>
            <th style="width: 5%; vertical-align: top;"><a href="/timer" style="text-decoration: none; color: #ffffff;">Current Timer</a></th>
        </tr>
        <tr>
            <th style="width: 5%;"></th>
            <th style="width: 90%;">
                <form action="/get">
                    <table style="width: 100%;">
                        <tr>
                            <th style="text-align: left;">Time to start watering</th> 
                            <th style="text-align: right;"><input type="time" name="time_input" value="00:00"></th>
                        </tr>
                        <tr>
                            <th style="text-align: left;">how long</th> 
                            <th style="text-align: right;"><input type="number" name="duration_input" value="00" min="1" max="60" style="width: 68px;"></th>
                        </tr>
                        <tr>
                            <th colspan="2"><input type="submit" value="Submit" ></th>
                        </tr>
                        <tr>
                            <th colspan="2"></th>
                        </tr>
                    </table>
                </form>
            </th>
            <th style="width: 5%;"></th>
        </tr>
    </table>
  </body>
</html>)rawliteral";

//This function sends a alert if the underpage isn`t found
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
//Serial Monitor connection is build up
  Serial.begin(115200);
  
//The Servo attachment is defined at pin D6
  ventil.attach(D6);
  

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", Watering_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputParam1;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_time)) {
      inputMessage1 = request->getParam(PARAM_INPUT_time)->value();
      inputParam1 = PARAM_INPUT_time;
      inputMessage2 = request->getParam(PARAM_INPUT_duration)->value();
      inputParam2 = PARAM_INPUT_duration;
    }
    else {
      inputMessage1 = "isn´t set jet";
      inputParam1 = "none";
      inputMessage2 = "isn´t set jet";
      inputParam2 = "none";
    }
    Timer_Time = inputMessage1;
    Duration = inputMessage2.toInt();
    Serial.println(inputMessage1);
    Serial.println(inputMessage2);
    request->send_P(200, "text/html", Watering_html);
  });

  server.on("/Wateringsystem-timer", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String Single = " minute";
      String Multiple = " minutes";
      String Timeing;
      
      if (Duration == 1) {
        Timeing = Single;
      } else {
        Timeing = Multiple;
      };
      
      request->send(200, "text/html", "<head>"
                                      "<title>Timer</title>"
                                      "</head>"
                                      "<body style=\"background-color: #444444; font-family: sans-serif; color: #ffffff; text-align: cnter; margin: 0; padding: 0;\">"
                                      "<table style=\"width: 100%;\">"
                                      "<tr style=\"height: 5%;\"><th style=\"width: 5%;\"></th><th style=\"width: 90%; vertical-align: center;\"><h1 style=\"margin: 0;\">Timer</h1></th><th style=\"width: 5%; vertical-align: top;\"><a href=\"/\" style=\"text-decoration: none; color: #ffffff;\">Back</a></th></tr>"
                                      "<tr><th style=\"width: 5%;\"></th><th style=\"width: 90%;\"><p>The timer is currently set at:</p>" + Timer_Time + "</th><th style=\"width: 5%;\"></th></tr>"
                                      "<tr><th style=\"width: 5%;\"></th><th style=\"width: 90%;\"><p>And it`s running for:</p>" + Duration + Timeing + "</th><th style=\"width: 5%;\"></th></tr>"
                                      "</body>");
  });

  server.onNotFound(notFound);
  server.begin();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();

  Hour_Now = now.hour();
  Minute_Now = now.minute();

  if (now.hour() < 10) {
    Hour_Now = Null + Hour_Now;
  };
  
  if (now.minute() < 10) {
    Minute_Now = Null + Minute_Now;
  };
  
  Current_Time = (Hour_Now + ":" + Minute_Now);
  
    // Read in what our current datestamp is from RTC
    if (Current_Time == Timer_Time) {
      ventil.write(56);
      Serial.print("Watering is active!");
      delay(Duration*60000);
      Serial.println();
    } else {
      ventil.write(0);
      Serial.println(Current_Time);
      delay(1000);
    };
}
