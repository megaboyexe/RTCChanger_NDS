#include <stdio.h>
#include <nds.h>

/**********************************************************************************
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
    
**********************************************************************************/


#define _YEAR	0
#define _MONTH	1
#define _DAY	2
#define _WKD	3
#define _HOUR	4
#define _MIN	5
#define _SEC	6

void integerToBCD(uint8 * data, uint32 length) {
//---------------------------------------------------------------------------------
	u32 i;
	for (i = 0; i < length; i++) {
		int high, low;
		swiDivMod(data[i], 10, &high, &low);
		data[i] = (high<<4) | low;
	}
}


int main()
{
	consoleDemoInit();
	
	int keys_pressed = 0;
	int appState = 0;
	
	u8 edit_pos = 0;
    
    u32 last_time = 0;
    u32 last_date = 0;

    u8 datetime[7];
	u8 edit_datetime[7];
	
    datetime[_HOUR]=0; datetime[_MIN]=0; datetime[_SEC]=0;
	datetime[_DAY]=0; datetime[_MONTH]=0; datetime[_YEAR]=0; datetime[_WKD]=0;
    
    edit_datetime[_HOUR]=0; edit_datetime[_MIN]=0; edit_datetime[_SEC]=0;
	edit_datetime[_DAY]=0; edit_datetime[_MONTH]=0; edit_datetime[_YEAR]=0; edit_datetime[_WKD]=0;

	
	while (1)
	{
        scanKeys();
        keys_pressed = keysDown();
        
		switch (appState)
        {
			case 0: //initial state
                if (keys_pressed & KEY_SELECT)
                {
                    edit_datetime[_HOUR] 	= (datetime[_HOUR]);
					edit_datetime[_MIN]		= (datetime[_MIN]);
					edit_datetime[_SEC] 	= (datetime[_SEC]);
					edit_datetime[_DAY] 	= (datetime[_DAY]);
					edit_datetime[_MONTH] 	= (datetime[_MONTH]);
					edit_datetime[_YEAR] 	= (datetime[_YEAR]);
					edit_datetime[_WKD] 	= (datetime[_WKD]);
                    
					appState = 1;
				}
			break;
			
			case 1: //edit clock state
				iprintf("\x1b[2J"); //clear the screen
				
				iprintf("\n Edit Real-Time Clock\n ----------------------------\n\n Date (DD/MM/YYYY)\n\n  ");
					
				//line to show cursor position
				iprintf("%s   %s     %s            %s",(edit_pos==0?"--":"  "),(edit_pos==1?"--":"  "),(edit_pos==2?"--":"  "),(edit_pos==3?"-":" "));
				
				iprintf("\n  %02d / %02d / 20%02d  Week Day: %d\n  ",edit_datetime[_DAY],edit_datetime[_MONTH],edit_datetime[_YEAR],edit_datetime[_WKD]);
				
				//line to show cursor position
				iprintf("%s   %s     %s            %s",(edit_pos==0?"--":"  "),(edit_pos==1?"--":"  "),(edit_pos==2?"--":"  "),(edit_pos==3?"-":" "));
				
				iprintf("\n\n Time (HH:MM:SS) \n\n  ");
				
				//line to show cursor position
				iprintf("%s   %s   %s",(edit_pos==4?"--":"  "),(edit_pos==5?"--":"  "),(edit_pos==6?"--":"  "));
				
				iprintf("\n  %02d : %02d : %02d\n  ",edit_datetime[_HOUR],edit_datetime[_MIN],edit_datetime[_SEC]);
				
				//line to show cursor position
				iprintf("%s   %s   %s",(edit_pos==4?"--":"  "),(edit_pos==5?"--":"  "),(edit_pos==6?"--":"  "));
				
				iprintf("\n\n Press START to save to RTC.");
                iprintf("\n\n Press B to cancel.");
				
				if (keys_pressed & KEY_UP)
                {
					switch (edit_pos)
                    {
						case 0:
							//day
							switch (edit_datetime[_MONTH])
                            {
								case 1: case 3: case 5: case 7: case 8: case 10: case 12:

									if(edit_datetime[_DAY]==31) {edit_datetime[_DAY]=1;} else {edit_datetime[_DAY]++;}
								break;
								case 4: case 6: case 9: case 11:
									if(edit_datetime[_DAY]==30) {edit_datetime[_DAY]=1;} else {edit_datetime[_DAY]++;}
								break;
								case 2:
									if((edit_datetime[_YEAR]%4==0 && edit_datetime[_DAY]==29) || (edit_datetime[_YEAR]%4>0 && edit_datetime[_DAY]==28)) {edit_datetime[_DAY]=1;} else {edit_datetime[_DAY]++;}
								break;
							}
						break;
                        
						case 1:
							//month
							if(edit_datetime[_MONTH]==12) {edit_datetime[_MONTH]=1;} else {edit_datetime[_MONTH]++;}
						break;
                        
						case 2:
							//year
							if(edit_datetime[_YEAR]==99) {edit_datetime[_YEAR]=0;} else {edit_datetime[_YEAR]++;}
						break;
                        
						case 3:
							//week day
							if(edit_datetime[_WKD]==6) {edit_datetime[_WKD]=0;} else {edit_datetime[_WKD]++;}
						break;
                        
						case 4:
							//hour
							if(edit_datetime[_HOUR]==23) {edit_datetime[_HOUR]=0;} else {edit_datetime[_HOUR]++;}
						break;
                        
						case 5:
							//minute
							if(edit_datetime[_MIN]==59) {edit_datetime[_MIN]=0;} else {edit_datetime[_MIN]++;}
						break;
                        
						case 6:
							//second
							if(edit_datetime[_SEC]==59) {edit_datetime[_SEC]=0;} else {edit_datetime[_SEC]++;}
						break;
					
					}
				} 
                else if(keys_pressed & KEY_DOWN)
                {
					switch (edit_pos)
                    {
						case 0:
							switch(edit_datetime[_MONTH])
                            {
								case 1: case 3: case 5: case 7: case 8: case 10: case 12:
									if(edit_datetime[_DAY]==1) {edit_datetime[_DAY]=31;} else {edit_datetime[_DAY]--;}
								break;
								case 4: case 6: case 9: case 11:
									if(edit_datetime[_DAY]==0) {edit_datetime[_DAY]=1;} else {edit_datetime[_DAY]--;}
								break;
								case 2:
									if(edit_datetime[_DAY]==1) {
										if(edit_datetime[_YEAR]%4==0) {
											edit_datetime[_DAY]=29;
										} else {
											edit_datetime[_DAY]=28;
										}
									} else {
										edit_datetime[_DAY]--;
									}
								break;
							}
						break;
						case 1:
							if(edit_datetime[_MONTH]==1) {edit_datetime[_MONTH]=12;} else {edit_datetime[_MONTH]--;}
						break;
						case 2:
							if(edit_datetime[_YEAR]==0) {edit_datetime[_YEAR]=99;} else {edit_datetime[_YEAR]--;}
						break;
						case 3:
							//week day
							if(edit_datetime[_WKD]==0) {edit_datetime[_WKD]=6;} else {edit_datetime[_WKD]--;}
						break;
						case 4:
							//hour
							if(edit_datetime[_HOUR]==0) {edit_datetime[_HOUR]=23;} else {edit_datetime[_HOUR]--;}
						break;
						case 5:
							//minute
							if(edit_datetime[_MIN]==0) {edit_datetime[_MIN]=59;} else {edit_datetime[_MIN]--;}
						break;
						case 6:
							//second
							if(edit_datetime[_SEC]==0) {edit_datetime[_SEC]=59;} else {edit_datetime[_SEC]--;}
						break;
					
					}
				}
                else if(keys_pressed & KEY_RIGHT)
                {
					if(edit_pos == 6)
                    {
						edit_pos = 0;
					}
                    else
                    {
						edit_pos++;
					}
				}
                else if(keys_pressed & KEY_LEFT)
                {
					if(edit_pos == 0)
                    {
						edit_pos = 6;
					} else
                    {
						edit_pos--;
					}
				}
                else if(keys_pressed & KEY_START)
                {
                    // Currently, libnds does not convert back to BCD
                    // before writing RTC data. Comment the conversion below
                    // if they change that someday.
                    integerToBCD(edit_datetime,7);
                    
                    u32 date_toSend = (edit_datetime[_YEAR] << 16) | (edit_datetime[_MONTH] << 8) | (edit_datetime[_DAY]);
                    u32 time_toSend = (edit_datetime[_WKD] << 24) | (edit_datetime[_HOUR] << 16) | (edit_datetime[_MIN] << 8) | (edit_datetime[_SEC]);
                    
					//commit edited RTC value
                    fifoSendValue32(FIFO_USER_03, 1);
                    fifoSendValue32(FIFO_USER_04, date_toSend);
                    fifoSendValue32(FIFO_USER_05, time_toSend);

					appState = 2;
					iprintf("\x1b[2J"); //clear the screen
					iprintf("\n Clock updated.\n Press START to continue.");
				}
                else if(keys_pressed & KEY_B)
                {
					appState = 0;
					iprintf("\x1b[2J"); //clear the screen
				}
				
			break;
            
            case 2: //clock commited
                if(keys_pressed & KEY_START)
                {
					appState = 0;
					iprintf("\x1b[2J"); //clear the screen
				}
            break;
		}

        
        if (fifoCheckValue32(FIFO_USER_01))
        {
            last_date = fifoGetValue32(FIFO_USER_01);
            
            datetime[_DAY] = last_date & 0xFF;
            datetime[_MONTH] = (last_date >> 8) & 0xFF;
            datetime[_YEAR] = (last_date >> 16) & 0xFF;
        }
        
        if (fifoCheckValue32(FIFO_USER_02))
        {
            last_time = fifoGetValue32(FIFO_USER_02);
            
            datetime[_SEC] = last_time & 0xFF;
            datetime[_MIN] = (last_time >> 8) & 0xFF;
            datetime[_HOUR] = (last_time >> 16) & 0xFF;
            datetime[_WKD] = (last_time >> 24) & 0xFF;
            
            if (appState == 0)
            {
                iprintf("\x1b[2J"); //clear the screen
                iprintf("\n RTCChanger NDS (v1.2.0)\n ------------------------------\n\n");
                iprintf(" Current date is:\n   %02u/%02u/%u  Week Day: % 2d\n Current time is:\n   %02d:%02d:%02d\n",(datetime[2]),(datetime[1]),(datetime[0])+1900+100,(datetime[3]),(datetime[4]),(datetime[5]),(datetime[6]));
                iprintf("\n\n Press SELECT to edit.");
            }
        }

		swiWaitForVBlank();
	}
}