# Gameboy Emulator for ESP32

This is a Gameboy emulator project developed for the NodeMCU ESP32 Devkit using an ILI9163 TFT 1.8" LCD display. The emulator is written in C and C++ and can run static Gameboy ROMs on the ESP32 hardware.

## Features
- Emulates the Gameboy system on ESP32
- Supports running Gameboy ROMs
- Display output on ILI9163 TFT LCD
- Button input for controlling the emulator

## Hardware Requirements

To run this emulator, you will need the following hardware:

- ESP32 microcontroller
- ILI9163 TFT LCD display
- Push buttons controlling the emulator

Software Requirements

To compile and upload the emulator to the ESP32, you will need the following software:

- Visual Studio Code with PlatformIO extension
- ESP32 Arduino Core (installed via PlatformIO)

## Getting Started

To get started with the emulator, follow these steps:

1. Clone this repository to your local machine or download it as a ZIP file.
2. Open the extracted project folder in Visual Studio Code.
3. Connect your ESP32 board to your computer.
4. Follow the [schematic diagram](schematics) to wire up.
5. Upload the compiled code to your ESP32 by clicking the PlatformIO upload button or using the `platformio run --target upload` command in the terminal.

## Configuration

You can also modify the emulator's configuration by editing the config.h file. Here are some of the available options:

* _Display Configuration_: Set the display resolution (not recommended), rotation, and other display-related settings (such as byte-swapping).
* _Button Configuration_: Define the GPIO pins used for button inputs.

Make sure to rebuild and upload the code after making any configuration changes.

## Contributing

Contributions to this project are welcome. If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request. Please ensure that your contributions align with the project's coding style and standards.

## License

This project is licensed under the GNU Public License v3. You are free to use, modify, and distribute this code for personal and commercial purposes.

## Acknowledgements

Special thanks to the developers and contributors of this projects for their valuable work.

- [lualiliu](https://github.com/lualiliu/esp32-gameboy): Creator of this fork.
- [zid](https://github.com/zid/gameboy): Original developer of Gameboy emulator.

## Disclaimer

The authors and contributors of this project are not responsible for any damages or issues caused by the use of this emulator. See [License](LICENSE)