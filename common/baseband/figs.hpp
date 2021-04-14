#pragma once
#include <stdint.h>

#ifndef __cplusplus
typedef int8_t bool;
#define true 1
#define false 0
#endif

#define INVALID_OCCURCHANGE 0xff
#define INVALID_SUBCH_ID -1
#define INVALID_SUBCH_START_ADDRESS 0xffff

typedef enum { fib_ok, fib_crc_error, fib_empty, fig_invalid } FibStatus;

typedef struct {
	bool cn_;
	bool oe_;
	bool pd_;
} FigBase0;

typedef struct {
	FigBase0 base_;
	uint16_t eId_;
	uint8_t cf_;
	bool af_;
	uint16_t cifCount_;
	uint8_t occurChange_;

} Fig_0_0;

typedef struct {
	FigBase0 base_;
	int8_t subchId_;
	uint16_t startAddress_;
	bool isLongForm_;
	uint8_t shortFormIdx_;
	uint8_t longFormOption_;
	uint8_t longFormProtLevel_;
	uint16_t longFormSubchSize_;
} Fig_0_1;

typedef struct {
	bool triggerOn_;
	uint8_t noScenarioIds_;
	uint8_t scenarioId_[23]; // 23 = max possible scenarioIds in a single fig
	uint8_t noZoneIds_;
	uint32_t zoneId_[6]; // 6 = max possible zones in a single fig
} Fig_7_0;

