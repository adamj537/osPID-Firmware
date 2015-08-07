#README for osPID Controller
Author:  Adam Johnson (adamj537@gmail.com)
Last Updated:  8/7/2015

1.	LICENSING & ORIGINS

This firmware was developed from the osPID (http://www.ospid.com/blog/), version 1.6, released by Brett Beauregard and RocketScream (www.rocketscream.com) under the GPLv3 & BSD License, incorporating the following other open source modules.  All files have been reformatted and re-commented for uniformity and clarity.

The MAX31855 library is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.  The MAX6675 library is from www.ladyada.net and is in the public domain.  The EEPROMAnything module originated at playground.arduino.cc.  The Arduino AutoTune library is by Brett Beauregard, and originally ported from the AutotunerPID Toolkit by William Spinelli (http://www.mathworks.com/matlabcentral/fileexchange/4652-autotunerpid-toolkit) © 2004.  Brett's AutoTune library is licensed under the FreeBSD License:

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
-	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
-	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

2.	HARDWARE

The firmware requires Arduino-compatible hardware.  The firmware is configured for digital output card v1.5 & temperature input card v1.2.  If you are using a different I/O configuration, be sure to uncomment the appropriate #define statements in io.h.

3.	REVISIONS

Updates for version 2.0
-	reformatted all files with a uniform style
-	greatly edited comments to provide greater clarity
-	expanded README to include source of all firmware modules
-	TODO:  fix glitch causing EEPROM settings to be wiped out
-	TODO:  expand menu system to allow setup without "front-end" software

Updates for version 1.6
-	added support for v1.5 of the Temperature Input card (MAX31855 Thermocouple chip)
 
Updates for version 1.5
-	restructured code to allow for different IO cards
-	added reflow profile support
-	eliminated LCD flicker
-	error message when thermocouple is disconnected
-	extreme code size / RAM improvement (mainly menu and EEPRom)
-	consolodated the code into fewer files
-	osPID_Firmware.ino - Just about everything
-	io.h - IO card code.  pre-compiler flags control which card code is used
-	EEPROMAnything.h - halley's amazing EEPROMWriteAnything code.
-	AnalogButton .cpp _local.h - ospid button-reading/debounce code
-	PID_AutoTune_v0 .cpp _local.h - local copy of the autotune library (to avoid conflicts with possibly pre-installed copies)
-	PID_v1 .ccp _local.h - local copy of the PID library
-	max6675 .cpp _local.h - local copy of the max6675 library, used by the input card.
