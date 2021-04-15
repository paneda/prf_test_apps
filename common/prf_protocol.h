/******************************************************************************
* PRF protocol header.
* Import and use on devices using SPI/USB. See README.md.
******************************************************************************/
#pragma once

#include <stdint.h>

// The PRF firmware limits the MAX_CIF_CU_SIZE due to memory
// constraints. The MAX_CIF_CU_SIZE can be split between three
// subchannels.
// Effective bitrate is calculated by taking protection level into account.
// However for EEP-3A/UEP3 it can simply be calculated as:
// bitrate=MAX_CIF_CU_SIZE*4/3
#ifdef ENABLE_USB
#define MAX_CIF_CU_SIZE 216
#define MAX_NO_SUBCHANNELS 3
#elif SUBCH_PROCESSING
#define MAX_CIF_CU_SIZE 24
#define MAX_BITRATE 32
#define MAX_NO_SUBCHANNELS 1
#else
#define MAX_CIF_CU_SIZE 416
#define MAX_NO_SUBCHANNELS 3
#endif
#define NO_TF_FIBS 12 // ModeI has 12 fibs in each transmission frame
#define SERIAL_NO_LEN 6

typedef struct {
	char serialNo_[SERIAL_NO_LEN];
	uint32_t crc_;
}
prf_serial_no_t ;

typedef struct {
	int8_t channelIdx_;
	int8_t subchId_[MAX_NO_SUBCHANNELS];
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
	xy_diagram_opt_t tiiOpt_;
	uint8_t fftPosOpt_;
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

typedef enum {
	DFU_STATE_STOPPED,
	DFU_STATE_ENTER,
	DFU_STATE_STARTED,
	DFU_STATE_DOWNLOAD,
	DFU_STATE_UPLOAD,
	DFU_STATE_REVERT_FACTORY,
	DFU_STATE_EXIT
} prf_dfu_state_t;
typedef int8_t dfu_state;

typedef struct {
	dfu_state state_;
	int16_t totBlocks_;
	uint32_t crc_;
} prf_dfu_mode_t;

typedef struct {
	uint8_t data_[64];
	uint16_t actLen_;
	uint16_t nr_;
	uint32_t crc_;
} prf_dfu_block_t;

typedef enum {
	DFU_OK = 0,
	DFU_CRC_ERR = 1,
	DFU_FLASH_ERR = 2,
	DFU_MODE_NOT_SUPPORTED = 3,
	DFU_MODE_STATE_ERROR = 4,
	DFU_BLOCK_ERR = 5
} dfu_result_t;
typedef int16_t dfu_result;

typedef struct {
	uint16_t blockNr_;
	dfu_result dfuResult_;
	uint32_t crc_;
} prf_dfu_ack_t;

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

// Note: Need to match corresponding Fig_0_1 in dab_sdr_library
typedef struct {
	int8_t cn_;
	int8_t oe_;
	int8_t pd_;
	int8_t subchId_;
	uint16_t startAddress_;
	int8_t isLongForm_;
	uint8_t shortFormIdx_;
	uint8_t longFormOption_;
	uint8_t longFormProtLevel_;
	uint16_t longFormSubchSize_;
} prf_fig_0_1_t;

// prf_raw_subchannel_t provides unprocessed subchannels, i.e
// deinterleaving, viterbi decoding and scrambling has not been applied.
// Subchannels data carried in data_ are interleaved in 4 CIF's according to
// ETSI 300401. I.e. if two subchannels are used, they will be send as:
// chA0, chB0, chA1, chB1, chA2, chB2, chA3, chB3.
// fig01Arr_ describes needed processing and is sorted on startAddress_ in
// ascending order. I.e. startAddress_ of chA < startAddress_ of chB.
typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	// *8 for bytes * 4 cif:s
	uint8_t data_[MAX_CIF_CU_SIZE*8*4];
	// data can be divided into MAX_NO_SUBCHANNELS subchannels (x4)
	prf_fig_0_1_t fig01Arr_[MAX_NO_SUBCHANNELS];
	uint32_t crc_;
} prf_raw_subchannel_t;

// prf_subchannel_t provides processed subchannels and so since viterbi decoding
// has already been applied, it needs lesser data_ size. And as we expect
// EEP-3A, we set it to half.
typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	// *8 for bytes * 4 cif:s
	uint8_t data_[MAX_CIF_CU_SIZE*8*4/2];
	// data can be divided into MAX_NO_SUBCHANNELS subchannels (x4)
	prf_fig_0_1_t fig01Arr_[MAX_NO_SUBCHANNELS];
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
	int16_t peaks_[10];
	int16_t fftPos_;
	uint32_t crc_;
} prf_impulse_response_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	uint16_t data_[384];
	uint32_t crc_;
} prf_tii_t;

typedef struct {
	uint8_t mask_;
	uint32_t updateCounter_;
	uint32_t crc_;
} prf_tick_t;

#ifdef SUBCH_PROCESSING
#define MAX_MASK_BUFFER_SIZE sizeof(prf_signal_info_t) +\
	sizeof(prf_fib_info_t) +\
	sizeof(prf_subchannel_t) +\
	sizeof(prf_iq_t) +\
	sizeof(prf_spectrum_t) +\
	sizeof(prf_impulse_response_t) +\
	sizeof(prf_tii_t) +\
	sizeof(prf_tick_t)
#else
#define MAX_MASK_BUFFER_SIZE sizeof(prf_signal_info_t) +\
	sizeof(prf_fib_info_t) +\
	sizeof(prf_raw_subchannel_t) +\
	sizeof(prf_iq_t) +\
	sizeof(prf_spectrum_t) +\
	sizeof(prf_impulse_response_t) +\
	sizeof(prf_tii_t) +\
	sizeof(prf_tick_t)
#endif

// Developers note: When adding new commands, there's a pattern to follow:
// 'set' commands has most significant bit=0.
// 'get' commands has most significant bit=1.
// 'get' commands with 1 value byte has also next most significant bit = 1.
// 'set' commands must be added in successive order for spi.
//
// 'set' commands supported both via spi/usb
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
// spi only commands starts here
#define SET_DFU_MODE_CMD 0x0c
#define SET_DFU_BLOCK_CMD 0x0d
// usb only commands starts here
#define SET_MASK_CMD 0x11

// 'get' commands supported both via spi/usb
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
// spi only commands starts here
#define GET_DFU_MODE_CMD 0x8b
#define GET_DFU_ACK_CMD 0x8c
// usb only commands start here
#define GET_SIGNAL_INFO_CMD 0x90

// usb/spi 'get' commands with 1 value byte
#define GET_MASK_DATA_CMD 0xc0 // spi only
#define PING_CMD 0xc1

// mask bits
#define SIGNAL_INFO_MASK 0x01
#define FIB_INFO_MASK 0x02
#define SUBCHANNEL_MASK 0x04
#define IQ_MASK 0x08
#define SPECTRUM_MASK 0x10
#define IMPULSE_RESPONSE_MASK 0x20
#define TII_MASK 0x40
#define TICK_MASK 0x80

