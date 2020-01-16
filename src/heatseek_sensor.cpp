/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/Jake/Documents/GitHub/feather_sensor/src/heatseek_sensor.ino"
#include <Particle.h>
#include <Wire.h>
#include <SPI.h>
#include "user_config.h"

void setup();
void loop();
void read_temperatures(float *temperature_f, float *humidity, float *heat_index);
void log_to_sd(float temperature_f, float humidity, float heat_index, uint32_t current_time);
void initialize_sd();
#line 6 "/Users/Jake/Documents/GitHub/feather_sensor/src/heatseek_sensor.ino"
#ifdef HEATSEEK_BORON_LTE
#include <SdFat.h>
#include <PietteTech_DHT.h>
#else
#include <SD.h>
#include <DHT.h>
#endif

#include "transmit.h"
#include "config.h"
#include "watchdog.h"
#include "rtc.h"

#ifdef HEATSEEK_BORON_LTE
static PietteTech_DHT dht(DHT_DATA, DHT22);
#else
static DHT dht(DHT_DATA, DHT22);
#endif
uint32_t startup_millis = 0;

void setup() {
  watchdog_init();

  Serial.begin(9600);
  delay(2000);
  
  Serial.print("initializing heatseek data logger: ");
  #ifdef TRANSMITTER_WIFI
    Serial.println("WIFI");
  #else
    Serial.println("cellular");
  #endif

  initialize_sd();
  rtc_initialize();

  dht.begin();

  if (!read_config()) set_default_config();

  watchdog_feed();

  startup_millis = millis();
}

void loop() {
  float temperature_f;
  float humidity;
  float heat_index;

  int32_t current_time = rtc.now().unixtime();
  int32_t last_reading_time = get_last_reading_time();
  int32_t time_since_last_reading = current_time - last_reading_time;

  char command = Serial.read();
  if (command == 'C') {
    enter_configuration();
  }
      
  Serial.print("Time since last reading: ");
  Serial.print(time_since_last_reading);
  Serial.print(", reading_interval: ");
  Serial.print(CONFIG.data.reading_interval_s);
  Serial.print(".  Code version: ");
  Serial.print(CODE_VERSION);
  Serial.println(". Press 'C' to enter config.");

  if (millis() - startup_millis < 15000) {
    Serial.println("Allowing 15 seconds to enter config mode [C] before taking first reading.");
    watchdog_feed();
    delay(2000);
    return;
  }
  
  if (CONFIG.data.reading_interval_s - time_since_last_reading > SEND_SAVED_READINGS_THRESHOLD) {
    Serial.println("Checking for queued temperature readings");
    watchdog_feed();
    transmit_queued_temps();
    delay(2000);
    watchdog_feed();
    return;
  } else if (time_since_last_reading < CONFIG.data.reading_interval_s) {
    delay(2000);
    watchdog_feed();
    return;
  }
  
  watchdog_feed();

  read_temperatures(&temperature_f, &humidity, &heat_index);
  log_to_sd(temperature_f, humidity, heat_index, current_time);

  watchdog_feed();
  
  update_last_reading_time(current_time);
  watchdog_feed();
  
  transmit(temperature_f, humidity, heat_index, current_time);

  watchdog_feed();

  delay(2000);
}

void read_temperatures(float *temperature_f, float *humidity, float *heat_index) {
  while (true) {
    bool success = true;

    #ifdef HEATSEEK_BORON_LTE
    int result = dht.acquireAndWait(1000);

    switch (result) {
    case DHTLIB_OK:
      Serial.println("OK");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.println("Error\n\r\tChecksum error");
      break;
    case DHTLIB_ERROR_ISR_TIMEOUT:
      Serial.println("Error\n\r\tISR time out error");
      break;
    case DHTLIB_ERROR_RESPONSE_TIMEOUT:
      Serial.println("Error\n\r\tResponse time out error");
      break;
    case DHTLIB_ERROR_DATA_TIMEOUT:
      Serial.println("Error\n\r\tData time out error");
      break;
    case DHTLIB_ERROR_ACQUIRING:
      Serial.println("Error\n\r\tAcquiring");
      break;
    case DHTLIB_ERROR_DELTA:
      Serial.println("Error\n\r\tDelta time to small");
      break;
    case DHTLIB_ERROR_NOTSTARTED:
      Serial.println("Error\n\r\tNot started");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }

    *temperature_f = dht.getFahrenheit();
    *humidity = dht.getHumidity();
    #else
    *temperature_f = dht.readTemperature(true);
    *humidity = dht.readHumidity();
    #endif
    
    if (!isnan(*temperature_f) && !isnan(*humidity)) {  
      Serial.print("Temperature (actual reading): ");
      Serial.print(*temperature_f);
      Serial.println(" *F");

      *temperature_f = *temperature_f + CONFIG.data.temperature_offset_f;
      Serial.print("Temperature (after calibration): ");
      Serial.print(*temperature_f);
      Serial.println(" *F");
      
      Serial.print("Humidity: ");
      Serial.print(*humidity);
      Serial.println("%");
      
      #ifdef HEATSEEK_BORON_LTE
      *heat_index = 0;
      #else
      *heat_index = dht.computeHeatIndex(*temperature_f, *humidity);
      
      Serial.print("Heat index: ");
      Serial.println(*heat_index);
      #endif

      return;
      
    } else {
      Serial.println("Error reading temperatures!");
    }
  
    delay(2000);

    // if we continue to fail to read a temperature, the watchdog will
    // eventually cause a reboot
  }
}

void log_to_sd(float temperature_f, float humidity, float heat_index, uint32_t current_time) {
  Serial.println("writing to SD card...");
  
  File data_file;
  
  if (data_file = SD.open("data.csv", FILE_WRITE)) {
    data_file.print(current_time); data_file.print(",");
    data_file.print(temperature_f); data_file.print(",");
    data_file.print(humidity); data_file.print(",");
    data_file.print(heat_index); data_file.println();
    Serial.println("wrote to SD");
    data_file.close();
  } else {
    Serial.println("unable to open data.csv");
    while(true); // watchdog will reboot
  }
}

void initialize_sd() {
  // Stop LORA module from interfering with SPI
  #ifdef TRANSMITTER_GSM
    pinMode(LORA_CS, OUTPUT);
    digitalWrite(LORA_CS, HIGH);
  #endif
  
  #ifdef HEATSEEK_BORON_LTE
  while (!SD.begin(SD_CS, SPI_FULL_SPEED)) {
    Serial.println("failed to initialize SD card");
    delay(1000);
  }
  #else
  while (!SD.begin(SD_CS)) {
    Serial.println("failed to initialize SD card");
    delay(1000); // watchdog will reboot
  }
  #endif

  
}