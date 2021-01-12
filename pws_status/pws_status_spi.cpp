#include <chrono>
#include <thread>
#include <memory>
#include <atomic>
#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "crc32.h"
#include "prf_protocol.h"

// spi parameters
static int fd_ = 0;
static const char *device_ = "/dev/spidev0.0";
static uint32_t mode_ = 0x01;
static uint8_t bits_ = 8;
static uint32_t speed_ = 7800000;
static uint16_t delay_ = 0;

std::atomic<bool> testDone_;
static void sigHandler(int signum) {
	std::cout << "Caught ctrl-c\n";
	testDone_ = true;
}

// Start of Linux specific spi methods ////////////////////////////////////////
static void prfSpiInit() {
	fd_ = open(device_, O_RDWR);
	if (fd_ < 0) {
		throw std::runtime_error("failed to open device");
	}

	int ret = ioctl(fd_, SPI_IOC_WR_MODE, &mode_);
	if (ret == -1) {
		throw std::runtime_error("failed to set spi mode");
	}

	ret = ioctl(fd_, SPI_IOC_RD_MODE, &mode_);
	if (ret == -1) {
		throw std::runtime_error("failed to set spi mode");
	}

	ret = ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_);
	if (ret == -1) {
		throw std::runtime_error("failed to set bits per word");
	}

	ret = ioctl(fd_, SPI_IOC_RD_BITS_PER_WORD, &bits_);
	if (ret == -1) {
		throw std::runtime_error("failed to set bits per word");
	}

	ret = ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed_);
	if (ret == -1) {
		throw std::runtime_error("failed set max speed hz");
	}

	ret = ioctl(fd_, SPI_IOC_RD_MAX_SPEED_HZ, &speed_);
	if (ret == -1) {
		throw std::runtime_error("failed set max speed hz");
	}
}

static void prfSpiClose(void) {
	close(fd_);
	fd_ = 0;
}

static void transfer(uint8_t const *tx, uint8_t const *rx, unsigned len) {
	int ret;
	int out_fd;

	if (fd_ == 0) {
		throw std::runtime_error("spi device not opened");
	}
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.speed_hz = speed_,
		.delay_usecs = delay_,
		.bits_per_word = bits_,
	};

	ret = ioctl(fd_, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) {
		throw std::runtime_error("failed to send spi message" + std::to_string(ret));
	}

	if (ret != len) {
		throw std::runtime_error("not all bytes written");
	}
}
// End of Linux specific spi methods //////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The PRF board communicates in half duplex mode. I.e. sending or receiving
// data to/from the PRF board must be done in separate spi transfers.
//
// A 'get' command needs two spi transfers. First spi transfer sends the
// command of what we want to 'get'. Second spi transfer receives the
// corresponding data.
//
// As the 'transfer()' method used in this example is full duplex. Its
// important that the buffer, not used for either direction, is set to 0.
// See further comments below.
///////////////////////////////////////////////////////////////////////////////
static prf_pws_status_t prfSpiGetPwsStatus() {
	// Send the single byte 'get' command as defined in common/prf_protocol.h.
	// As we're not receiving anything, set the rx buf to 0.
	uint8_t cmd = GET_PWS_STATUS_CMD;
	uint8_t zRx = 0;
	transfer(&cmd, &zRx, 1);

	// Now receive data.
	// As we're not sending anything, set the tx buf to 0.
	prf_pws_status_t status;
	uint8_t zTxBuf[sizeof(prf_pws_status_t)] = {0};
	transfer(zTxBuf, (uint8_t*) &status, sizeof(prf_pws_status_t));

	// Check CRC to make sure received data is valid.
	uint32_t crc = calcCrc32((uint8_t*) &status, sizeof(prf_pws_status_t) - 4);
	if (crc != status.crc_) {
		throw std::runtime_error("prfSpiGetPwsStatus crc error");
	}
	return status;
}

// Simple main application that polls the prf board for pws status once every
// second. Typical poll rate for real applications should preferably be set
// lower, e.g. 100ms, for faster reaction time.
int main(int argc, char** argv) {
	std::cout << "Attempting to connect via spi..\n";
	prfSpiInit();

	std::cout << "Press ctrl-c to exit..\n";
	testDone_ = false;
	while(!testDone_) {
		prf_pws_status_t pwsStatus = prfSpiGetPwsStatus();
		std::cout << "PWS: " << (pwsStatus.triggered_?"on":"off") << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	prfSpiClose();
	return 0;
}

