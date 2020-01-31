/*
*  Project     DJ Hero - Spin Rhythm XD Controller
*  @author     David Madison
*  @link       github.com/dmadison/DJHero-SpinRhythm
*  @license    GPLv3 - Copyright (c) 2020 David Madison
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Forked from the DJ Hero Lucio project: github.com/dmadison/DJHero-Lucio
*/

#include <NintendoExtensionCtrl.h>

// User Settings
const int8_t WheelSensitivity = 5;  // Mouse sensitivity multipler - 12 max
const int8_t MaxWheelInput = 10;    // Ignore values above this threshold as extranous

// Tuning Options
const unsigned long UpdateRate = 4;          // Controller polling rate, in milliseconds (ms)
const unsigned long DetectTime = 1000;       // Time before a connected controller is considered stable (ms)
const unsigned long ConnectRate = 500;       // Rate to attempt reconnections, in ms
const unsigned long EffectsTimeout = 400;    // Timeout for the effects tracker, in ms
const uint8_t       EffectThreshold = 3;     // Threshold to trigger input from the fx dial. One revolution = 31.
// #define IGNORE_DETECT_PIN                 // Ignore the state of the 'controller detect' pin, for breakouts without one.

// Debug Flags (uncomment to add)
// #define DEBUG                // Enable to use any prints
// #define DEBUG_RAW            // See the raw data from the turntable
// #define DEBUG_HID            // See HID inputs as they're pressed/released
// #define DEBUG_COMMS          // Follow the controller connect and update calls
// #define DEBUG_CONTROLDETECT  // Trace the controller detect pin functions

// ---------------------------------------------------------------------------

#include "DJSpinRhythm_LED.h"   // LED handling classes
#include "DJSpinRhythm_HID.h"   // HID classes (Keyboard, Mouse)
#include "DJSpinRhythm_Controller.h"  // Turntable connection and data helper classes

DJTurntableController dj;
ConnectionHelper controller(dj, DetectPin, UpdateRate, DetectTime, ConnectRate);

MouseButton grabWheel(MOUSE_LEFT);
MouseButton tapWheel(MOUSE_RIGHT);

KeyboardButton beat(' ');
KeyboardButton cancel(KEY_ESC);
KeyboardButton submit(KEY_RETURN);

KeyboardButton navigateUp('w');
KeyboardButton navigateLeft('a');
KeyboardButton navigateDown('s');
KeyboardButton navigateRight('d');

EffectHandler fx(dj, EffectsTimeout);

void setup() {
	#ifdef USB_SERIAL_HID
	Serial.begin(115200);
	Serial.println("DJ Hero - Spin Rhythm XD Controller v0.0.0");
	Serial.println("By David Madison, (c) 2020");
	Serial.println("http://www.partsnotincluded.com");
	Serial.println("----------------------------");
	#endif

	#ifdef DEBUG
	while (!Serial);  // Wait for connection
	#endif

	pinMode(SafetyPin, INPUT_PULLUP);
	if (digitalRead(SafetyPin) == LOW) {
		DEBUG_PRINTLN("Safety loop activated! Exiting program");
		for (;;);  // Safety loop!
	}

	LED.begin();  // Set LED pin mode
	controller.begin();  // Initialize controller bus and detect pins

	DEBUG_PRINTLN("Initialization finished. Starting program...");
}

void loop() {
	if (controller.isReady()) {
		djController();
	}
	LED.update();
}

void djController() {
	// "Wheel" Control
	int8_t turntableOutput = 0;

	if (dj.getNumTurntables() == 2) {
		const uint8_t crossfade = dj.crossfadeSlider();  // use slider as selector

		if (crossfade < 6) turntableOutput = dj.left.turntable();  // slider is left, use left turntable only
		else if (crossfade > 8) turntableOutput = dj.right.turntable();  // slider is right, use right turntable only
		else turntableOutput = dj.turntable();  // centered, get a mix of both (+)
	}
	else {
		turntableOutput = dj.turntable();  // use whichever turntable is connected
	}

	moveWheel(turntableOutput);

	// Turntable Buttons
	grabWheel.set(dj.buttonRed());
	tapWheel.set(dj.buttonGreen() || dj.buttonBlue());

	// Base Unit Controls
	beat.set(dj.buttonEuphoria());
	cancel.set(dj.buttonMinus());
	submit.set(dj.buttonPlus());

	// Menu Navigation
	joyWASD(dj.joyX(), dj.joyY());

	// FX Dial
	fx.update();  // update tracker with new data

	if (fx.changed(EffectThreshold)) {
		HID_Button& rollUp   = navigateDown;  // linked outputs
		HID_Button& rollDown = navigateUp;

		// check against state to avoid interfering w/ other inputs using the same control
		if (fx.getTotal() > 0 && rollUp.isPressed() == false) rollUp.press();
		else if (fx.getTotal() < 0 && rollDown.isPressed() == false) rollDown.press();
		fx.reset();  // Input was triggered, reset to 0
	}
}

void moveWheel(int8_t xIn) {
	// Check for signed overflow (int8_t). The USB mouse output only takes one int8_t value per update.
	static_assert(WheelSensitivity * MaxWheelInput <= 127, "Your sensitivity is too high!");  // Check for signed overflow (int8_t)

	static int8_t lastAim = 0;

	// Check if above max threshold
	if (abs(xIn) >= MaxWheelInput) xIn = lastAim;
	else lastAim = xIn;

	Mouse.move(xIn * WheelSensitivity, 0);

	#ifdef DEBUG_HID
	if (xIn != 0) {
		DEBUG_PRINT("Moved the mouse {");
		DEBUG_PRINT(xIn * WheelSensitivity);
		DEBUG_PRINTLN(", 0}");
	}
	#endif
}

void joyWASD(uint8_t x, uint8_t y) {
	const uint8_t JoyCenter = 32;
	const uint8_t JoyDeadzone = 20;  // +/-, centered at 32 in (0-63)

	navigateLeft.set(x < JoyCenter - JoyDeadzone);
	navigateRight.set(x > JoyCenter + JoyDeadzone);

	navigateUp.set(y > JoyCenter + JoyDeadzone);
	navigateDown.set(y < JoyCenter - JoyDeadzone);
}
