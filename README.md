# USB 

This file is about How to read files data on USB over STM32F407 Discovery Kit

  - Compiler IDE: CubeIDE  (Framework v1.26 has issuse because of that it will be good choice if you create firttime file with CubeMX)
  
  - OTG Mass Storage Class
  - FatFile System
  - Systick (168 Mhz)
  - USB_OTG_FS
  - retarget folder fir (printf and scanf functions)
  - Debug Test: used UART3  
 
 
 
#Example Output:

USB  Total Size KB:   30720
USB Free Space KB:    30709
File: //rec.wav       1816 KB
File: //sample.bin    4 KB
File: //ses1.wav      2343 KB
File: //ses2.wav      1969 KB
File: //ses3.wav      2141 KB
File: //ses4.wav      2487 KB
Dir: DIR1
File: //text1.txt  23 KB

Enter a any file name to read from exist files:
-> sample.bin \r\n

08 09 7E 05 D3 01 10 FB 1F F4 5D FC 6B 08 FF 04 
2A 05 EE 13 50 16 69 06 01 F8 8E EE 5D ED 63 FD 
CF 0C BF 08 A4 00 FD 02 A9 05 7B FF 0F F5 ED F3 
7C FF D3 08 EC 0A C4 0B 14 05 F6 FA 56 FD 53 08 
A4 10 79 10 14 07 F3 03 18 0A EA 02 B7 F3 C9 F2 
86 F5 B2 F0 E6 F2 DD FF E3 07 FF 04 26 01 1A 03 
D9 05 19 07 38 07 C2 02 85 FD 31 FF CB 04 0D 06 
13 00 3A F7 EF F1 CA F3 A6 FD 32 06 50 03 E5 FE


File *sample.bin* CLOSED successfully
