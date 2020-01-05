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
const int8_t HorizontalSens = 5;  // Mouse sensitivity multipler - 6 max
const int8_t VerticalSens   = 2;  // Mouse sensitivity multipler - 6 max
const int8_t MaxAimInput = 20;    // Ignore aim values above this threshold as extranous

// Tuning Options
const unsigned long UpdateRate = 4;          // Controller polling rate, in milliseconds (ms)
const unsigned long DetectTime = 1000;       // Time before a connected controller is considered stable (ms)
const unsigned long ConnectRate = 500;       // Rate to attempt reconnections, in ms
const unsigned long ConfigThreshold = 3000;  // Time the euphoria and green buttons must be held to set a new config (ms)
const unsigned long EffectsTimeout = 1200;   // Timeout for the effects tracker, in ms
const uint8_t       EffectThreshold = 10;    // Threshold to trigger abilities from the fx dial, 10 = 1/3rd of a revolution
// #define IGNORE_DETECT_PIN                 // Ignore the state of the 'controller detect' pin, for breakouts without one.

// Debug Flags (uncomment to add)
// #define DEBUG                // Enable to use any prints
// #define DEBUG_RAW            // See the raw data from the turntable
// #define DEBUG_HID            // See HID inputs as they're pressed/released
// #define DEBUG_COMMS          // Follow the controller connect and update calls
// #define DEBUG_CONTROLDETECT  // Trace the controller detect pin functions
// #define DEBUG_CONFIG         // Debug the config read/set functionality

// ---------------------------------------------------------------------------

#include "DJSpinRhythm_LED.h"   // LED handling classes
#include "DJSpinRhythm_HID.h"   // HID classes (Keyboard, Mouse)
#include "DJSpinRhythm_Controller.h"  // Turntable connection and data helper classes
#include "DJSpinRhythm_ConfigMode.h"  // Configuration mode (left/right) switching class

DJTurntableController dj;

DJTurntableController::TurntableExpansion * mainTable = &dj.right;
DJTurntableController::TurntableExpansion * altTable = &dj.left;

KeyboardButton navigateUp('w');
KeyboardButton navigateLeft('a');
KeyboardButton navigateDown('s');
KeyboardButton navigateRight('d');

EffectHandler fx(dj, EffectsTimeout);

ConnectionHelper controller(dj, DetectPin, UpdateRate, DetectTime, ConnectRate);
TurntableConfig config(dj, &DJTurntableController::buttonEuphoria, &DJTurntableController::TurntableExpansion::buttonGreen, ConfigThreshold);

void setup() {
	#ifdef DEBUG
	Serial.begin(115200);
	while (!Serial);  // Wait for connection
	DEBUG_PRINTLN("DJ Hero - Spin Rhythm XD Controller v0.0.0");
	DEBUG_PRINTLN("By David Madison, (c) 2020");
	DEBUG_PRINTLN("http://www.partsnotincluded.com");
	DEBUG_PRINTLN("----------------------------");
	#endif

	pinMode(SafetyPin, INPUT_PULLUP);
	if (digitalRead(SafetyPin) == LOW) {
		DEBUG_PRINTLN("Safety loop activated! Exiting program");
		for (;;);  // Safety loop!
	}

	LED.begin();  // Set LED pin mode
	config.read();  // Set expansion pointers from EEPROM config
	controller.begin();  // Initialize controller bus and detect pins

	DEBUG_PRINTLN("Initialization finished. Starting program...");
}

void loop() {
	if (controller.isReady()) {
		djController();
		config.check();
	}
	LED.update();
}

void djController() {
	// Single turntable (either side)
	if (dj.getNumTurntables() == 1) {
		aiming(dj.turntable(), 0);
	}

	// --Base Station Abilities--
	fx.update();

	// Menu Navigation
	joyWASD(dj.joyX(), dj.joyY());

	// --Cleanup--
	if (fx.changed(EffectThreshold)) {
		fx.reset();  // Already used abilities, reset to 0
	}
}

void aiming(int8_t xIn, int8_t yIn) {
	static_assert(HorizontalSens * MaxAimInput <= 127, "Your sensitivity is too high!");  // Check for signed overflow (int8_t)
	static_assert(VerticalSens   * MaxAimInput <= 127, "Your sensitivity is too high!");

	static int8_t lastAim[2] = { 0, 0 };
	int8_t * aim[2] = { &xIn, &yIn };  // Store in array for iterative access

	// Iterate through X/Y
	for (int i = 0; i < 2; i++) {
		// Check if above max threshold
		if (abs(*aim[i]) >= MaxAimInput) {
			*aim[i] = lastAim[i];
		}

		// Set 'last' value to current
		lastAim[i] = *aim[i];
	}

	Mouse.move(xIn * HorizontalSens, yIn * VerticalSens);

	#ifdef DEBUG_HID
	if (xIn != 0 || yIn != 0) {
		DEBUG_PRINT("Moved the mouse {");
		DEBUG_PRINT(xIn * HorizontalSens);
		DEBUG_PRINT(", ");
		DEBUG_PRINT(yIn * VerticalSens);
		DEBUG_PRINTLN("}");
	}
	#endif
}

void joyWASD(uint8_t x, uint8_t y) {
	const uint8_t JoyCenter = 32;
	const uint8_t JoyDeadzone = 6;  // +/-, centered at 32 in (0-63)

	navigateLeft.press(x < JoyCenter - JoyDeadzone);
	navigateRight.press(x > JoyCenter + JoyDeadzone);

	navigateUp.press(y > JoyCenter + JoyDeadzone);
	navigateDown.press(y < JoyCenter - JoyDeadzone);
}
