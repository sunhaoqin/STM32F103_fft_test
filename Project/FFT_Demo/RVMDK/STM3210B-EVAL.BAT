SET PATH=C:\Keil\ARM\ARMCC\bin\;C:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\bin;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\lib;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\bin;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\lib;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\IDM Computer Solutions\UltraCompare;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Users\jacob.sun\AppData\Local\Microsoft\WindowsApps;C:\Users\jacob.sun\AppData\Local\Programs\Python\Python36;C:\Users\jacob.sun\AppData\Local\GitHubDesktop\bin
SET CPU_TYPE=STM32F103VC
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=STM3210B-EVAL
SET CPU_CLOCK=0x007A1200
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\main.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\stm32f10x_it.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\stm3210b_lcd.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\stm32f10x_spi.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\misc.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\stm32f10x_flash.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\stm32f10x_gpio.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\stm32f10x_rcc.__i"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\startup_stm32f10x_md._ia"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\pid_c_stm32.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\iir_stm32.__i"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\cr4_fft_64_stm32._ia"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\cr4_fft_256_stm32._ia"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\cr4_fft_1024_stm32._ia"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\fir_stm32._ia"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\iirarma_stm32._ia"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\pid_stm32._ia"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\core_cm3.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\system_stm32f10x.__i"
"C:\Keil\ARM\ARMCC\bin\ArmLink" --Via ".\Obj\STM3210B-EVAL.lnp"
