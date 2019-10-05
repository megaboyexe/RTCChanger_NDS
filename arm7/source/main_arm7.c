/*---------------------------------------------------------------------------------

	RTCChanger NDS - created by MegaBoyEXE
     
    Changelog:
	v1.2.0 - September 23, 2019 (Updated by DeadSkullzJr)
    - Tweaked some of the text
	- Added the homebrew application version number to the title in the menu.
	
    v1.1.1 - September 22, 2019
    - Fixed the day roll over when it is a leap year in February.
    
    v1.1.0 - September 22, 2019 (Updated by DeadSkullzJr)
    - Added a new homebrew icon
    - Added a new homebrew description

    v1.0.0 - September 22, 2019
    - Initial Release
    
    
    Modified default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
    

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <dswifi7.h>
#include <maxmod7.h>

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	Wifi_Update();
}


//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}


RTCtime cur_dateTime;
RTCtime new_dateTime;
int clock_refresh = 0;
int editRequested = 0;

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	// clear sound registers
	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	fifoInit();
	touchInit();

	mmInstall(FIFO_MAXMOD);

	SetYtrigger(80);

	installWifiFIFO();
	installSoundFIFO();

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	setPowerButtonCB(powerButtonCB);
    
	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
        
        ++clock_refresh;
        if (clock_refresh == 60 && !editRequested)
        {
            rtcGetTimeAndDate((uint8*)&cur_dateTime);
            clock_refresh = 0;
            
            u32 date_toSend = (cur_dateTime.year << 16) | (cur_dateTime.month << 8) | (cur_dateTime.day);
            u32 time_toSend = (cur_dateTime.weekday << 24) | (cur_dateTime.hours << 16) | (cur_dateTime.minutes << 8) | (cur_dateTime.seconds);
            
            fifoSendValue32(FIFO_USER_01, date_toSend);
            fifoSendValue32(FIFO_USER_02, time_toSend);
        }
        
        if (fifoCheckValue32(FIFO_USER_03))
        {
            fifoGetValue32(FIFO_USER_03);
            editRequested = 3;
        }
        
        if (editRequested > 1)
        {
            if (fifoCheckValue32(FIFO_USER_04))
            {
                u32 newDate = fifoGetValue32(FIFO_USER_04);
                
                new_dateTime.day = newDate & 0xFF;
                new_dateTime.month = (newDate >> 8) & 0xFF;
                new_dateTime.year = (newDate >> 16) & 0xFF;
                
                editRequested--;
            }
            if (fifoCheckValue32(FIFO_USER_05))
            {
                u32 newTime = fifoGetValue32(FIFO_USER_05);
                
                new_dateTime.seconds = newTime & 0xFF;
                new_dateTime.minutes = (newTime >> 8) & 0xFF;
                new_dateTime.hours = (newTime >> 16) & 0xFF;
                new_dateTime.weekday = (newTime >> 24) & 0xFF;
                
                editRequested--;
            }
        }
        else if (editRequested == 1)
        {
            rtcSetTimeAndDate((uint8*)&new_dateTime);
            
            editRequested = 0;
            clock_refresh = 0;
        }
        
		swiWaitForVBlank();
	}
	return 0;
}
