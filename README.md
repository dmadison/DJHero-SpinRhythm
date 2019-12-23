# DJ Hero Adapter for Spin Rhythm XD
This project allows a user to play [Spin Rhythm XD](https://www.spinrhythmgame.com/) using an Arduino and a DJ Hero turntable for the Nintendo Wii.

## Dependencies
The DJ Hero firmware uses my [NintendoExtensionCtrl library](https://github.com/dmadison/NintendoExtensionCtrl/) ([0.7.1](https://github.com/dmadison/NintendoExtensionCtrl/releases/tag/v0.7.1)) to handle communication to the DJ Hero controller itself.

This program was compiled using version [1.8.6](https://www.arduino.cc/en/Main/OldSoftwareReleases) of the [Arduino IDE](https://www.arduino.cc/en/Main/Software). If using an Arduino board such as the Leonardo or Pro Micro, the program is dependent on the Arduino Keyboard, Mouse, and Wire libraries which are installed with the IDE. If using a Teensy microcontroller, the program was built using [Teensyduino 1.44](https://www.pjrc.com/teensyduino-1-44-released/) which you can download [here](https://www.pjrc.com/teensy/td_144), and also requires the [i2c_t3 library](https://github.com/nox771/i2c_t3) version [10.1](https://github.com/nox771/i2c_t3/releases/tag/v10.1).

I've linked to the specific library releases that work with this code. Note that other versions may not be compatible.

## History
This repository is a fork of the [DJHero-Lucio project](https://github.com/dmadison/DJHero-Lucio), where I modified a DJ Hero controller to play the character of Lucio in *Overwatch*. It retains the same license (GPL v3) and code structure. Only the outgoing control scheme has changed.

For more information, check out the blog post on [PartsNotIncluded.com](http://www.partsnotincluded.com/altctrl/playing-lucio-with-a-dj-hero-turntable).

## License
This project is licensed under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html), either version 3 of the License, or (at your option) any later version.
