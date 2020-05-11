//only sends gps data for simplicity
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <TinyGPS++.h>

//must open debug window if ture
//once debug is done set to false
#define debug false

static uint8_t payload[6];
static osjob_t sendjob;

const unsigned TX_INTERVAL = 30;

static const u1_t PROGMEM APPEUI[8] = {  };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

static const u1_t PROGMEM DEVEUI[8] = {  };
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}

static const u1_t PROGMEM APPKEY[16] = {  };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

const lmic_pinmap lmic_pins = {
  .nss = 8,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = {3, 6, LMIC_UNUSED_PIN},
  .rxtx_rx_active = 0,
  .rssi_cal = 8,
  .spi_freq = 8000000,
};



void onEvent (ev_t ev) {

  if (debug)Serial.print(os_getTime());
  if (debug)Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      if (debug)if (debug)Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      if (debug)if (debug)Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      if (debug)if (debug)Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      if (debug)if (debug)Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      if (debug)if (debug)Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      if (debug)Serial.println(F("EV_JOINED"));
      {
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
        LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
        if (debug)Serial.print("netid: ");
        if (debug)Serial.println(netid, DEC);
        if (debug)Serial.print("devaddr: ");
        if (debug)Serial.println(devaddr, HEX);
        if (debug)Serial.print("artKey: ");
        for (int i = 0; i < sizeof(artKey); ++i) {
          if (i != 0)
            if (debug)Serial.print("-");
          if (debug)Serial.print(artKey[i], HEX);
        }
        if (debug)Serial.println("");
        if (debug)Serial.print("nwkKey: ");
        for (int i = 0; i < sizeof(nwkKey); ++i) {
          if (i != 0)
            if (debug)Serial.print("-");
          if (debug)Serial.print(nwkKey[i], HEX);
        }
        if (debug)Serial.println("");
      }

      LMIC_setLinkCheckMode(0);
      break;
    case EV_JOIN_FAILED:
      if (debug)Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      if (debug)Serial.println(F("EV_REJOIN_FAILED"));
      break;
      break;
    case EV_TXCOMPLETE:
      if (debug)Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        if (debug)Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        if (debug)Serial.println(F("Received "));
        if (debug)Serial.println(LMIC.dataLen);
        if (debug)Serial.println(F(" bytes of payload"));
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      if (debug)Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      if (debug)Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      if (debug)Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      if (debug)Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      if (debug)Serial.println(F("EV_LINK_ALIVE"));
      break;

    case EV_TXSTART:
      if (debug)Serial.println(F("EV_TXSTART"));
      break;
    default:
      if (debug)Serial.print(F("Unknown event: "));
      if (debug)Serial.println((unsigned) ev);
      break;
  }
}

TinyGPSPlus gps;

void do_send(osjob_t* j) {
  
    if (LMIC.opmode & OP_TXRXPEND) {
      if (debug)Serial.println(F("OP_TXRXPEND, not sending"));
    } else {

    uint32_t payloadLat;
    uint32_t payloadLng;

    while (Serial1.available() > 0) {
      gps.encode(Serial1.read());
      if (gps.location.isUpdated()) {
        
        if (debug)Serial.print("lat: ");
        if (debug)Serial.println(gps.location.lat(),8);

        if (debug)Serial.print("lng: ");
        if (debug)Serial.println(gps.location.lng(),8);
        
        payloadLat = gps.location.lat() * 100000;
        payload[0] = payloadLat;
        payload[1] = payloadLat >> 8;
        payload[2] = payloadLat >> 16;
        
        payloadLng = gps.location.lng() * 100000;
        payload[3] = payloadLng;
        payload[4] = payloadLng >> 8;
        payload[5] = payloadLng >> 16;
      }
    }

    LMIC_setTxData2(1, payload, sizeof(payload), 0);

  }
}

void setup() {
  delay(5000);
  if (debug) while (!Serial);
  if (debug)Serial.begin(57600);
  if (debug)Serial.println(F("Starting"));

  Serial1.begin(9600);

  os_init();

  LMIC_reset();
  LMIC_setLinkCheckMode(0);
  LMIC_setDrTxpow(DR_SF7, 14);
  LMIC_selectSubBand(1);

  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}
