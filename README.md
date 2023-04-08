# Electric Kettle DEVS model

This model executes the temperature controller for an electric kettle. We use the DS18B20 temperature probe to measure the temperature and three outputs for a green led (temperature OK), red led (temperature out of bounds) and relay. This model is intended to run on an ESP32 board, where:

* DS18B20: pin 4 (G4)
* Red LED: pi 16 (G16)
* Green LED: pin 15 (G15)
* Relay: pin 17 (G17)

This model is based on Cadmium\_v2 version modified by Sasisekhar [here](https://github.com/Sasisekhar/RT-CADMIUM_ESP32) and the Blinky led basic example [here](https://github.com/Sasisekhar/Blinky-ESPIDF).

Under the folder `components/` you will find:
```
├── CMakeLists.txt
├── main
│   ├── include/
│   ├── CMakeLists.txt
│   └── main.c
├── components
│   ├── boost_1_79_0/
│   ├── cadmium_v2/
│   ├── esp32-ds18b20/
│   └── esp32-owb/
└── README.md                  This is the file you are currently reading
```

Subfolders `cadmium_v2/`, `esp32-ds18b20/` and `esp32-owb` are treated as git submodules. As for the `boost_1_79_0/` you must download it and decompress from: https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz.

In file: `components/cadmium_v2/include/cadmium/core/real_time/rt_clock.hpp` the macro `MISSED_DEADLINE_TOLERANCE` must be changed to `-1` to avoid checking the slid of real time.

## ESP-32 Toolchain

For information about the ESP-32 toolchain check [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/).

To install the tools required to work with ESP32 run the steps [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html):

1. For Linux users install the required tools:
```bash
sudo apt-get install git wget flex bison gperf python3 python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
```
2. Then clone the repo with the esp tools:
```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
```
3. Build these tools:
```bash
cd ~/esp/esp-idf
./install.sh esp32
```
4. Finally, setup the environmental variables:
```bash
. ./export.sh
```
5. To automate this last step you can add the following line to the `.bashrc` script:
```
alias get_idf='. $HOME/<path-to-esp-folder>/esp/esp-idf/export.sh'
```
where `<path-to-esp-folder>` must be replaced by the correct path.
6. Then, to make sure that the environmental variables are correctly defined after launching a new terminal just run either:
```bash
get-idf
```

7. To build the model run:
```bash
idf.py build
```

8. Flash the board:
```bash
idf.py -p /dev/ttyUSB0 flash
```

9. Monitor the serial port:
```bash
idf.py monitor
```
To close the monitor just type: CTRL+].


