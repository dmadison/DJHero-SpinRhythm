# DJ Hero Adapter for Spin Rhythm XD
This project allows a user to play [*Spin Rhythm XD*](https://www.spinrhythmgame.com/) on PC using an Arduino and the Nintendo Wii variant of the *DJ Hero* turntable.

## Dependencies
The *DJ Hero* firmware uses my [NintendoExtensionCtrl library](https://github.com/dmadison/NintendoExtensionCtrl/) ([0.7.4](https://github.com/dmadison/NintendoExtensionCtrl/releases/tag/v0.7.4)) to handle communication to the *DJ Hero* controller itself.

This program was compiled using version [1.8.10](https://www.arduino.cc/en/Main/OldSoftwareReleases) of the [Arduino IDE](https://www.arduino.cc/en/Main/Software). If using an Arduino board such as the Leonardo or Pro Micro, the program is dependent on the Arduino Keyboard, Mouse, and Wire libraries which are installed with the IDE. If using a Teensy microcontroller, the program was built using [Teensyduino 1.48](https://forum.pjrc.com/threads/57906-Teensyduino-1-48-Released) which you can download [here](https://www.pjrc.com/teensy/td_148), and also requires the [i2c_t3 library](https://github.com/nox771/i2c_t3) version [10.1](https://github.com/nox771/i2c_t3/releases/tag/v10.1).

I've linked to the specific library releases that work with this code. Note that other versions may not be compatible.

## History
This repository is a fork of the [*DJ Hero* Lucio project](https://github.com/dmadison/DJHero-Lucio), where I modified a *DJ Hero* controller to play as the character of Lucio in *Overwatch*. It retains the same license (GPL v3) and code structure. Only the control scheme has changed.

For more information, check out the blog post on [PartsNotIncluded.com](http://www.partsnotincluded.com/altctrl/playing-lucio-with-a-dj-hero-turntable).

## License
This project is licensed under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html), either version 3 of the License, or (at your option) any later version.
