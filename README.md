# prf_test_apps
Test applications for Paneda DAB RF front-end board.

For information about each application, please read the source code.

## Hardware
Mount the PRF board onto a Raspberry PI. Any Raspberry PI board with the 40-pin
GPIO header can used.
Connect a DAB antenna to the coax connector on the PRF board.

## Installation
Install Raspberry PI OS (or Arch Linux Arm) and boot the Raspberry PI.

On the Raspberry PI, SPI needs to be enabled. This can be done using the
following step.

Add the following line to /boot/config.txt
```
dtparams=spi=on
```

Optional: Allow access to spidev without sudo:
```bash
sudo groupadd --system spi
sudo gpasswd -a <username> spi
echo "SUBSYSTEM==\"spidev\", GROUP=\"spi\"" | sudo tee /etc/udev/rules.d/90-spi.rules
```
Reboot the Raspberry PI.

## Building 
On Raspberry Pi OS install necessary packages:
```bash
sudo apt install git cmake build-essential
```

..or on Arch Linux ARM:
```
sudo pacman -S git cmake base-devel
```

In a terminal run the following commands:
```bash
git clone https://github.com/paneda/prf_test_apps.git
cd prf_test_apps
mkdir build && cd build
cmake ..
make
```

Demo apps should now be ready to run.

