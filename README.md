# Embedded Xinu for the Pi: The ReWork!!! #

Embedded Xinu, Copyright (C) 2008, 2009, 2010, 2013, 2018.  All rights reserved.
>
This code while usually running is a work in progress .. AKA Alpha. It is walking a line between staying functional as well as being able to make the large changes required on the code base. I am updating the Wiki as I go if you do want to track what I am working on.

#### Hard/Soft FPU compiling: ####
The code has been altered to allow either orginal soft FPU compiling or now hard FPU compiling ... Please see notes.
>
Default is set for hard float of the repository.
>https://github.com/LdB-ECM/Docs_and_Images/blob/master/Documentation/HardSoft_fpu.md
#### Makefile change: #### 
I changed the makefile to build object files to a single directory so I could make the clean process faster. I hopefully added make under linux support in at the same time (untested).

#### I am starting a collection of funny code I ran across and fixed: ####
>https://github.com/LdB-ECM/Docs_and_Images/blob/master/Documentation/Xinu.md

#### Current Pi Working Image (works on all models) is always located: #### 
>https://github.com/LdB-ECM/Xinu/blob/master/compile/kernel.img
>
Compiling of ARM7(Pi2) and ARM8(Pi3) 32 bit code added as close to release (1 bug in ethernet driver). As the FPU is offline in code due to restriction with context switcher code there is little advantage right now. For stability stay with kernel.img although the other binaries are precompiled out of interest.

Version: 3.00 Alpha the background

 1. Xinu 2.0 Repo: The Good, The Bad, The Ugly?
 2. Current changes
 3. Future planned changes
 4. Installation Instructions
    1. Build Instructions
    2. Build Controls
 5. Useful Links


## 1. Xinu 2.0 Repo: The Good, The Bad, The Ugly? ##

##### The Good:
    -The Code being derived off UNIX has enough similarities to Linux to be useful
    -The design is simple, some may even say elegant
    - There is plenty of documentation and teaching examples
    
##### The Bad:
    - There is no storage media support
    - The coding standards makes the repository awkward especially if you try to port to 64bit.
    - The integeration of python and shell commands into the build process is frustrating
   
##### The Ugly:
    - There is no C standard the code adheres with.
    - Standard C libraries either don't exist like stdbool.h or have been butchered like stddef.h
    - A number of hideous macros and code which are poor substitues to the missing C standard functions

## 2. Current Changes ##

    1.) <stdbool.h> the C standard created 
    2.) Fixes to <stdint.h> to align it closer to the C standard for AARCH64 especially
    3.) Reclaimed stddef.h for C standard, placing wrong enties into xinu.h
    4.) Slowly removing every uint, uchar, ulong etc and replacing them with standards 
    5.) Entries with no size requirement given plain C types (unsigned char, unsigned int, int etc) 
    6.) Entries with specific width changed to stdint.h entries (uint16_t, uint32_t, uint64_t).
    7.) Device system being reworked so they just auto load into device table.
    8.) Having done 7 conf.c, conf.h are blank empty drive tables simply with a maximum device capacity.
    9.) Having done 7 & 8 the integration of python to make source files was removed as just ugly.
    10.) All shell commands were removed from the makefile system (It now builds solely using make)
    11.) Build now working correctly with just make and the compiler
    12.) For the Pi start.s and platforminit.c are sorting out all models so they work (it's ugly atm)
    13.) Fixed Framebuffer device so it works on all model Pi
    14.) Fixed framebuffer device so it supports 3 colour modes RGB565, RGB, RGBA
    15.) Added VT100 parser from console output to FrameBuffer screen.
    16.) Added lan78xx driver as needed by Pi3B+
    17.) Added GPIO driver for all Pi models
    18.) Arm7, Arm8 32 bit compiling support done
    19.) Hard/Soft FPU compiling added.
    
    *** 12.) Will be fixed as I publish what I am expecting to enforce of drivers in a soon to 
     be printed standard for drivers.

## 3. Future Planned Changes ##
    [ ] SD & HDD drivers done, FAT32 driver done. Need to sort out mount and console.
    [ ] AARCH64 bit compilation is working but I have some alignment bugs with bad code
    [ ] Full MMU support is done in both 32/64 but again bad code causing bugs is issue
    [x] I can bring the FPU online but debugging and checking needs to be done
    [ ] To many platforms rather than correctly use platformint.c use #define
    [ ] Driver directory with device sharing needs to be sorted .. I prefer subdirectories
    [ ] Multiprocessor task/switcher system needs to be added. I have played around and 
        it isn't hard to replace the switcher and scheduler with different variants.

## 4. Installation Instruction ##

  
  #### 4.1 Build Instructions: ####
     It should be as simple as download a C compiler for your O/S I am using the ARM standard 
     toolset in the useful link list down the page
           
     Download a makefile executable (make.exe) for your O/S .. On windows I am using GNU make 4.21
     
     Place the binary directories for the compiler and makefile on your path enviroment.
     
     Open a command console and navigate to the "compile" directory on your copy of the repository
     Issue the command "make".
     
     It should start building away :-)
    
           
  #### 4.2 Build controls: ####
     The build has two important extra commands
     "make clean" ... will remove all the temp compiled binary object files which forces a full build
     "make libclean" ... will remove all the temp compiled library files so the library can be rebuilt.
     
     From time to time you will need both depending what files you change.
     
## 5. Useful Links ##
Arm open source develpment tools
>https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
>
Linux driver file repository
>https://github.com/torvalds/linux/tree/master/drivers
>
GNU 4.2.1 Make prebuilt binaries for Windows
>https://github.com/mbuilov/gnumake-windows
