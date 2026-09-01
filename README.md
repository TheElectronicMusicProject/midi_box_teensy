# MIDI Box Teensy

A one channel MIDI box implementation using the Teensy 2.0 microcontroller with USB MIDI transmission support.

## Description

This project provides a MIDI controller/interface built on the Teensy 2.0 platform. It enables USB-based MIDI communication, allowing seamless integration with DAWs (Digital Audio Workstations) and other MIDI-compatible software on your computer.

## Features

- USB MIDI transmission over USB to your computer
- Single channel MIDI operation
- Built for Teensy 2.0 microcontroller

## Requirements

- Teensy 2.0 microcontroller
- USB cable
- Arduino IDE with Teensyduino extension
- Teensy Loader software

## Installation

1. Install [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) for Arduino IDE
2. Clone or download this repository
3. Open the project in Arduino IDE
4. Select "Teensy 2.0" from Tools → Board menu
5. Connect your Teensy 2.0 via USB
6. Click Upload

## Usage

After uploading the firmware to your Teensy 2.0:
- Connect your Teensy to your computer via USB
- Your DAW or MIDI application should detect it as a USB MIDI device
- Begin sending MIDI data over the single channel

## Hardware Connections

Refer to the Teensy 2.0 documentation for GPIO pin assignments. Typical MIDI implementations may use:
- USB for MIDI transmission (built-in on Teensy 2.0)
- Additional GPIO pins as needed for input controls

## Technical Details

- **Target Platform**: Teensy 2.0
- **Communication Protocol**: USB MIDI

## Project Structure

- Source files contain the main firmware implementation
- Configuration options available in header files

## Contributing

Contributions, issues, and feature requests are welcome. Feel free to open an issue or submit a pull request.

## License

[Add your license information here]

## Resources

- [Teensy Official Site](https://www.pjrc.com/teensy/)
- [Teensy 2.0 Documentation](https://www.pjrc.com/teensy/teensy20.html)
- [USB MIDI Specification](https://www.usb.org/)
- [Arduino IDE](https://www.arduino.cc/)

## Author

Circuitry Passion

---

**Repository**: [TheElectronicMusicProject/midi_box_teensy](https://github.com/TheElectronicMusicProject/midi_box_teensy)
