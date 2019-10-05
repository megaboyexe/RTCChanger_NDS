# RTCChanger NDS

This tool changes the Nintendo DS hardware RTC without modifying the "RTC Offset" data in the firmware, so games cannot detect time changes: no trigger for "anti-time travel" feature that blocks in-game time events for 24h (eg. Pokemon games; Animal Crossing).

The [GBA-RTCRead](https://github.com/megaboyexe/GBA_RTCRead) tool was used as base code for GUI, so if you used that tool before, you should be familiar with it.

Just be advised that you are responsible to also sync the "week day" value when changing the Date.

Tested in-game events without penalties, like growing berries by changing time in increments of 4 hours, and saving again.