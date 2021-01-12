/******************************************************************************
* PRF protocol header.
* Import and use on devices using SPI/USB. See README.md.
******************************************************************************/
#pragma once

#include <stdint.h>

#define MAX_TF_SIZE 7680 // fic + 544kbit/s (204CU) (gross capacity)
#define NO_TF_FIBS 12 // ModeI has 12 fibs in each transmission frame
#define SERIAL_NO_LEN 6

typedef struct {
	char serialNo_[SERIAL_NO_LEN];
	uint32_t crc_;
}
prf_serial_no_t ;

// DAB config struct. channelIdx_ refers to dabVhfFrequencies table.
// Changing this config will re-tune and result in data loss.
typedef struct {
	int8_t channelIdx_;
	uint32_t crc_;
} prf_dab_config_t;

typedef struct {
	int8_t useDigitalDiode_;
	int8_t useBBFilter770_;
	uint8_t fir_[23];
	uint32_t crc_;
} prf_tuner_option_t;

// Makes it possible to select mode for USB endpoint 1.
// USB_OUT_RAW: All samples from tuner are forwarded over USB unprocessed.
// USB_OUT_MASK_DATA: Processed data are forwarded as structs according to
// below.
typedef enum {
	USB_OUT_RAW,
	USB_OUT_MASK_DATA
} usb_output_t;
typedef uint8_t usb_output;

// offset_opt makes it possible to disable offset tuning for test.
// Must typically we written to flash as offset tuning is done immediately
// at start.
typedef enum {
	OFFSET_OPT_FINE, // coarse+fine
	OFFSET_OPT_COARSE, // only coarse
	OFFSET_OPT_NONE
} offset_opt_t;
typedef uint8_t offset_opt;

typedef enum {
	IQ_OPT_RAW, // iq diagram not processed
	IQ_OPT_NORM, //iq diagram normalized
} iq_opt_t;
typedef uint8_t iq_opt;

typedef struct {
	int8_t windowFunction_;
	uint8_t yScalar_;
	uint8_t yOffset_;
	uint8_t xScalar_;
} xy_diagram_opt_t;

// Various processing options. Changes does not cause re-tune.
typedef struct {
	usb_output usbOutput_;
	offset_opt offsetOpt_;
	uint16_t fibCrcTrapLimit_; // multiples of 96ms
	iq_opt iqOpt_;
	xy_diagram_opt_t spectrumOpt_;
	xy_diagram_opt_t impulseResponseOpt_;
	uint32_t crc_;
} prf_process_option_t;

typedef struct {
	uint8_t eventId_1_;
	uint8_t eventId_2_;
	uint32_t zoneId_;
	uint32_t crc_;
} prf_pws_config_t;

typedef struct {
	uint8_t triggered_;
	uint32_t crc_;
} prf_pws_status_t;

// these structs are sent as mask data and needs the additional fields:
// mask_, the corresponding mask value
// updateCounter_, increments++ for every TF
typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	int32_t coarseOffset_;
	int32_t fineOffsetAccum_;
	int16_t fineOffset_;
	uint8_t ratio_;
	int8_t isTii_;
	int8_t powerIn_;
	int8_t isBlocker_;
	int16_t temperature_;
	uint32_t crc_;
} prf_signal_info_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	uint8_t fibData_[NO_TF_FIBS * 32];
	int8_t fibCrcErrors_[NO_TF_FIBS];
	uint16_t bitErrors_;
	uint16_t usedBerBits_;
	uint32_t crc_;
} prf_fib_info_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	uint8_t data_[MAX_TF_SIZE];
	uint16_t actLen_;
	uint32_t crc_;
} prf_subchannel_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	int8_t data_[3072];
	uint32_t crc_;
} prf_iq_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	uint8_t data_[2048];
	uint32_t crc_;
} prf_spectrum_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	uint8_t data_[1008];
	uint32_t crc_;
} prf_impulse_response_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	uint32_t crc_;
} prf_tick_t;

#define MAX_MASK_BUFFER_SIZE sizeof(prf_signal_info_t) + sizeof(prf_fib_info_t) +\
			sizeof(prf_subchannel_t) + sizeof(prf_iq_t) + sizeof(prf_spectrum_t) +\
			sizeof(prf_impulse_response_t) + sizeof(prf_tick_t)

// Developers note: When adding new commands, there's a pattern to follow:
// 'set' commands has most significant bit=0.
// 'get' commands has most significant bit=1.
// 'get' commands with 1 value byte has also next most significant bit = 1.
// 'set' commands must be added in successive order.
#define SET_PERSISTENT_DAB_CONFIG_CMD 0x01
#define SET_VOLATILE_DAB_CONFIG_CMD 0x02
#define SET_PERSISTENT_TUNER_OPT_CMD 0x03
#define SET_VOLATILE_TUNER_OPT_CMD 0x04
#define SET_PERSISTENT_PROCESS_OPT_CMD 0x05
#define SET_VOLATILE_PROCESS_OPT_CMD 0x06
#define SET_TILE1_PORT4B_CMD 0x07
#define SET_TILE1_PORT4C_CMD 0x08
#define SET_PWS_CONFIG_CMD 0x09
#define SET_TUNER_REG_CMD 0x0a
#define SET_SERIAL_NO_CMD 0x0b
// here continue with commands supported via spi/usb
//
// usb only commands starts here
#define SET_MASK_CMD 0x11

// 'get' commands
#define GET_PERSISTENT_DAB_CONFIG_CMD 0x81
#define GET_VOLATILE_DAB_CONFIG_CMD 0x82
#define GET_PERSISTENT_TUNER_OPT_CMD 0x83
#define GET_VOLATILE_TUNER_OPT_CMD 0x84
#define GET_PERSISTENT_PROCESS_OPT_CMD 0x85
#define GET_VOLATILE_PROCESS_OPT_CMD 0x86
#define GET_PWS_CONFIG_CMD 0x87
#define GET_PWS_STATUS_CMD 0x88
#define GET_VERSION_CMD 0x89
#define GET_SERIAL_NO_CMD 0x8a
// here continue with commands supported via spi/usb
//
// usb only commands start here
#define GET_SIGNAL_INFO_CMD 0x90

// usb/spi 'get' commands with 1 value byte
#define GET_MASK_DATA_CMD 0xc0
#define PING_CMD 0xc1

// mask bits
#define SIGNAL_INFO_MASK 0x01
#define FIB_INFO_MASK 0x02
#define SUBCHANNEL_MASK 0x04
#define IQ_MASK 0x08
#define SPECTRUM_MASK 0x10
#define IMPULSE_RESPONSE_MASK 0x20
#define TICK_MASK 0x40
// #define TII_MASK 0x80

