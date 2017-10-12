#ifndef TRANSMIT_H
#define TRANSMIT_H

#define CODE_VERSION "F-1.0.0"

#ifdef MCU_STM32F205RG
  #define TRANSMITTER_WIFI
#else
  #define TRANSMITTER_GSM
#endif

#ifdef TRANSMITTER_WIFI
  #include <libmaple/iwdg.h>
  #include <adafruit_feather.h>
  #include <adafruit_http.h>

  #define DHT_DATA  PC2
  #define SD_CS     PB4
  
  #define TRANSMITS_PER_LOOP 20
#endif

#ifdef TRANSMITTER_GSM
  #include "Adafruit_FONA.h"
  #include <Adafruit_SleepyDog.h>

  #define DHT_DATA  A2
  #define SD_CS     10
  #define FONA_RST  A4
  #define LORA_CS   8
  
  #define TRANSMITS_PER_LOOP 5
#endif

#define SEND_SAVED_READINGS_THRESHOLD (10 * 60)
#define USER_AGENT_HEADER  "curl/7.45.0"
#define PORT               80

void transmit(float temperature_f, float humidity, float heat_index, uint32_t current_time);
void transmit_queued_temps();
void clear_queued_transmissions();

#endif
