//**************************************************************************************************
// Several structs and enums.                                                                      *
//**************************************************************************************************
//

#include <WString.h>
#ifndef TYPES_H
#define TYPES_H

enum qdata_type
{
    QDATA,
    QSTARTSONG,
    QSTOPSONG, // datatyp in qdata_struct,
    QSTOPTASK
};
struct qdata_struct // Data in queue for playtask (dataqueue)
{
    qdata_type datatyp;                          // Identifier
    __attribute__((aligned(4))) uint8_t buf[32]; // Buffer for chunk of mp3 data
};

struct ini_struct
{
    String mqttbroker;    // The name of the MQTT broker server
    String mqttprefix;    // Prefix to use for topics
    uint16_t mqttport;    // Port, default 1883
    String mqttuser;      // User for MQTT authentication
    String mqttpasswd;    // Password for MQTT authentication
    uint8_t reqvol;       // Requested volume
    uint8_t rtone[4];     // Requested bass/treble settings
    String clk_server;    // Server to be used for time of day clock
    int8_t clk_offset;    // Offset in hours with respect to UTC
    int8_t clk_dst;       // Number of hours shift during DST
    int8_t ir_pin;        // GPIO connected to output of IR decoder
    int8_t enc_clk_pin;   // GPIO connected to CLK of rotary encoder
    int8_t enc_dt_pin;    // GPIO connected to DT of rotary encoder
    int8_t enc_sw_pin;    // GPIO connected to SW of rotary encoder / ZIPPY B5
    int8_t enc_up_pin;    // GPIO connected to UP of ZIPPY B5 side switch
    int8_t enc_dwn_pin;   // GPIO connected to DOWN of ZIPPY B5 side switch
    int8_t tft_cs_pin;    // GPIO connected to CS of TFT screen
    int8_t tft_dc_pin;    // GPIO connected to D/C or A0 of TFT screen
    int8_t tft_scl_pin;   // GPIO connected to SCL of i2c TFT screen
    int8_t tft_sda_pin;   // GPIO connected to SDA of I2C TFT screen
    int8_t tft_bl_pin;    // GPIO to activate BL of display
    int8_t tft_blx_pin;   // GPIO to activate BL of display (inversed logic)
    int8_t nxt_rx_pin;    // GPIO for input from NEXTION
    int8_t nxt_tx_pin;    // GPIO for output to NEXTION
    int8_t sd_cs_pin;     // GPIO connected to CS of SD card
    int8_t sd_detect_pin; // GPIO connected to SC card detect (LOW is inserted)
    int8_t vs_cs_pin;     // GPIO connected to CS of VS1053
    int8_t vs_dcs_pin;    // GPIO connected to DCS of VS1053
    int8_t vs_dreq_pin;   // GPIO connected to DREQ of VS1053
    int8_t shutdown_pin;  // GPIO to shut down the amplifier
    int8_t shutdownx_pin; // GPIO to shut down the amplifier (inversed logic)
    int8_t spi_sck_pin;   // GPIO connected to SPI SCK pin
    int8_t spi_miso_pin;  // GPIO connected to SPI MISO pin
    int8_t spi_mosi_pin;  // GPIO connected to SPI MOSI pin
    int8_t i2s_bck_pin;   // GPIO Pin number for I2S "BCK"
    int8_t i2s_lck_pin;   // GPIO Pin number for I2S "L(R)CK"
    int8_t i2s_din_pin;   // GPIO Pin number for I2S "DIN"
    int8_t i2s_spdif_pin; // GPIO Pin number for SPDIF output
    int8_t eth_mdc_pin;   // GPIO Pin number for Ethernet controller MDC
    int8_t eth_mdio_pin;  // GPIO Pin number for Ethernet controller MDIO
    int8_t eth_power_pin; // GPIO Pin number for Ethernet controller POWER
    uint16_t bat0;        // ADC value for 0 percent battery charge
    uint16_t bat100;      // ADC value for 100 percent battery charge
};

struct WifiInfo_t // For list with WiFi info
{
    char *ssid;       // SSID for an entry
    char *passphrase; // Passphrase for an entry
};

// Preset info
enum station_state_t
{
    ST_PRESET,
    ST_REDIRECT, // Possible preset status
    ST_PLAYLIST,
    ST_STATION
};

struct tftMessage{
    char message[100];
    int section = -1;
};

struct preset_info_t
{
    int16_t preset;                // Preset to play
    int16_t highest_preset;        // Highest possible preset
    station_state_t station_state; // Station state
    int16_t playlistnr;            // Index in playlist
    int16_t highest_playlistnr;    // Highest possible preset
    String playlisthost;           // Host with playlist
    String host;                   // Resulting host
    String hsym;                   // Symbolic name (comment after name)
};

enum datamode_t
{
    INIT = 0x1,
    HEADER = 0x2,
    DATA = 0x4, // State for datastream
    METADATA = 0x8,
    PLAYLISTINIT = 0x10,
    PLAYLISTHEADER = 0x20,
    PLAYLISTDATA = 0x40,
    STOPREQD = 0x80,
    STOPPED = 0x100
};

struct progpin_struct // For programmable input pins
{
    int8_t gpio;    // Pin number
    bool reserved;  // Reserved for connected devices
    bool avail;     // Pin is available for a command
    String command; // Command to execute when activated
                    // Example: "uppreset=1"
    bool cur;       // Current state, true = HIGH, false = LOW
};

struct touchpin_struct // For programmable input pins
{
    int8_t gpio;    // Pin number GPIO
    bool reserved;  // Reserved for connected devices
    bool avail;     // Pin is available for a command
    String command; // Command to execute when activated
                    // Example: "uppreset=1"
    bool cur;       // Current state, true = HIGH, false = LOW
    int16_t count;  // Counter number of times low level
};

struct mqttpub_struct
{
    const char *topic; // Topic as partial string (without prefix)
    uint8_t type;      // Type of payload
    void *payload;     // Payload for this topic
    bool topictrigger; // Set to true to trigger MQTT publish
};

struct iosetting
{
    const char *gname; // Name in preferences
    int8_t *gnr;       // Address of target GPIO pin number
    int8_t pdefault;   // Default pin
};

enum enc_menu_t
{
  VOLUME,
  PRESET,
  TRACK
}; // State for rotary encoder menu

enum
{
  MQTT_IP,
  MQTT_ICYNAME,
  MQTT_STREAMTITLE,
  MQTT_NOWPLAYING,
  MQTT_PRESET,
  MQTT_VOLUME,
  MQTT_PLAYING,
  MQTT_PLAYLISTPOS
};

enum
{
  MQSTRING,
  MQINT8,
  MQINT16
}; // Type of variable to publish

#endif