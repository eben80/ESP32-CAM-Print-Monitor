#include <esp_camera.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>


//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//this edit code doesn't use a gzipped html source code
//the html -code is open and is easy to update or modify  on the other tab page = app_httpd.cpp

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_M5STACK_PSRAM
#define CAMERA_MODEL_AI_THINKER

//Config Start
const char *KNOWN_SSID[] = {"Belkin", "Mi Phone", "AndroidAP"};
const char *KNOWN_PASSWORD[] = {"Fixui27d69!", "12345678", "7b95ad53f0c9"};
const int KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]);

#define RELAY_PIN 13 //Relay Pin for Printer Mains Relay
//Config end

//Flash Pin
#define ledPin 4

#ifdef __cplusplus
extern "C"
{
#endif

uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

#if defined(CAMERA_MODEL_WROVER_KIT)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 21
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 19
#define Y4_GPIO_NUM 18
#define Y3_GPIO_NUM 5
#define Y2_GPIO_NUM 4
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM 15
#define XCLK_GPIO_NUM 27
#define SIOD_GPIO_NUM 25
#define SIOC_GPIO_NUM 23

#define Y9_GPIO_NUM 19
#define Y8_GPIO_NUM 36
#define Y7_GPIO_NUM 18
#define Y6_GPIO_NUM 39
#define Y5_GPIO_NUM 5
#define Y4_GPIO_NUM 34
#define Y3_GPIO_NUM 35
#define Y2_GPIO_NUM 32
#define VSYNC_GPIO_NUM 22
#define HREF_GPIO_NUM 26
#define PCLK_GPIO_NUM 21

#elif defined(CAMERA_MODEL_AI_THINKER) //zie esp32-cam schema v1.6.pdf
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#else
#error "Camera model not selected"
#endif

void startCameraServer();

void blink() //This blinks the onboard LED flash once you are connected to the Wifi.
{
  digitalWrite(ledPin, HIGH); // turn on the LED
  delay(50);                  // wait for half a second or 500 milliseconds
  digitalWrite(ledPin, LOW);  // turn off the LED
  delay(50);                  // wait for half a second or 500 milliseconds
  digitalWrite(ledPin, HIGH); // turn on the LED
  delay(50);                  // wait for half a second or 500 milliseconds
  digitalWrite(ledPin, LOW);  // turn off the LED
  delay(50);                  // wait for half a second or 500 milliseconds
}

// Remote debug over telnet - not recommended for production/release, only for development

// Disable all debug ?

// Important to compile for prodution/release
// Disable all debug ? Good to release builds (production)
// as nothing of RemoteDebug is compiled, zero overhead :-)
// For it just uncomment the DEBUG_DISABLED
// On change it, if in another IDE than Arduino IDE, like Eclipse or VSCODE,
// please clean the project, before compile

//#define DEBUG_DISABLED true

// Disable the auto function feature of RemoteDebug ?

//#define DEBUG_DISABLE_AUTO_FUNC true

#include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug

#ifndef DEBUG_DISABLED // Only if debug is not disabled (for production/release)

// Instance of RemoteDebug

RemoteDebug Debug;

#endif

// This code was been converted by RemoteDebugConverter


void setup()
{
  boolean wifiFound = false;
  int i, n;

  pinMode(ledPin, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(250000);
  Serial.setTimeout(5000);
  Serial.setDebugOutput(true);
  Serial.println();


  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //drop down frame size for higher initial frame rate
  sensor_t *s = esp_camera_sensor_get();
  //s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_framesize(s, FRAMESIZE_SVGA); //v12345vtm

  // ----------------------------------------------------------------
  // WiFi.scanNetworks will return the number of networks found
  // ----------------------------------------------------------------
  Serial.println(F("scan start"));
  int nbVisibleNetworks = WiFi.scanNetworks();
  Serial.println(F("scan done"));
  if (nbVisibleNetworks == 0)
  {
    Serial.println(F("no networks found. Reset to try again"));
    while (true)
      ; // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here at least some networks are visible
  // ----------------------------------------------------------------
  Serial.print(nbVisibleNetworks);
  Serial.println(" network(s) found");

  // ----------------------------------------------------------------
  // check if we recognize one by comparing the visible networks
  // one by one with our list of known networks
  // ----------------------------------------------------------------
  for (i = 0; i < nbVisibleNetworks; ++i)
  {
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < KNOWN_SSID_COUNT; n++)
    { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str()))
      {
        Serial.print(F("\tNot matching "));
        Serial.println(KNOWN_SSID[n]);
      }
      else
      { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound)
      break; // break from the "for each visible network" loop
  }          // end for each visible network

  if (!wifiFound)
  {
    Serial.println(F("no Known network identified. Reset to try again"));
    while (true)
      ; // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here you found 1 known SSID
  // ----------------------------------------------------------------
  Serial.print(F("\nConnecting to "));
  Serial.println(KNOWN_SSID[n]);

  // ----------------------------------------------------------------
  // We try to connect to the WiFi network we found
  // ----------------------------------------------------------------
  WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // ----------------------------------------------------------------
  // SUCCESS, you are connected to the known WiFi network
  // ----------------------------------------------------------------
  Serial.println(F("WiFi connected, your IP address is "));
  Serial.println(WiFi.localIP());

  blink();

  // Start OTA Config
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("PrintCamDev1");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  // End OTA Config

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  Serial.print("stream Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println(":9601/stream ");

#ifndef DEBUG_DISABLED // Only for development

    // Initialize the WiFi server of RemoteDebug

    Debug.begin("PrintCamDev1"); // Initiaze the WiFi server (please put here the hostname)

    //Debug.setPassword("r3m0t0."); // Password of WiFi (as telnet) connection ?

    Debug.setResetCmdEnabled(true); // Enable the reset command

    Debug.showProfiler(true); // Profiler (Good to measure times, to optimize codes)

    Debug.showColors(true); // Colors

    //Debug.setSerialEnabled(true); // if you wants serial echo - only recommended if ESP is plugged in USB

#endif

}

void loop()
{

#ifndef DEBUG_DISABLED

    // RemoteDebug handle (for WiFi connections)

    Debug.handle();


#endif
  if (WiFi.status() != WL_CONNECTED) //Loop used to reconnect wifi after disconnection
  {
    rprintDln(WiFi.status());
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      rprintD(".");
    }
    rprintDln("WiFi Reconnected");
  }
  ArduinoOTA.handle();
  delay(1000);
}
