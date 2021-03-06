SDCC SD Card Check
===============================================================================
Copyright (C) 2008  Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

===============================================================================

Required hardware:
   o one MBHP_CORE module
   o one Character LCD
   o SD card module connected to J6/J7 (Port C)

Optional hardware:
   o DIN module (1x SR)

===============================================================================

Checks the connected SD-card. The application first looks for the first sector
that is writeable/readable. Then it looks for the last sector readable/writeable.
When these boundaries are found, it performs a deep check of the R/W sector space,
writing nonsense data (two big arrays) to a sector, read it again and check if
the data is still the same. the define "check_step" sets the step used for the
deep check. If you want to check every single sector, set this to 1. Mind that
this will cause the check to take a very long time (ca. 2 hours for 256MB). 
When the check is done, the LCD will show the lower and upper R/W sector, and
the number of sectors that failed in R/W check.

I tested this application with a 1GB SanDisk, and a 256MB Pretec card. 

After initialization the application looks for the first writeable sector:
"FS 0x....."

It does this in single sector steps, if max_first_sector is reached, the application
will abort and show the last error. 

Second it looks for the highest writeable sector:
"LS 0x...."

It increments the sector by initial_sector_inc, if one sector fails, it halfs
the incrementer, tries again etc. The last sector is found when the incrementer
is 1 and a R/W fails.

Next the deep check is performed. It starts at the lowest writeable sector,
increments the sector by check_step up to the highest writeable sector. To
check every single sector, change "check_step" to 1. During the check, the
current checked sector, sectors failed the thest and the upper boudary will
be displayed in hexadecimal values:
"0x.... BAD 0x00"
"0x077BFF" (example, my 256MB card)

When the check is done, the lowest/highest writeable sector and failed sectors
will be shown:
"0x00 BAD 0x00"
"0x077BFF"

If a DIN module is connected, you can re-trigger the check by switching pin 0.

Internals: after each failed read/write operation, the applications inits the
card. Not doing this, causes the communication to fail sometimes, if the app
tried to read/write a sector out of range. 

===============================================================================
