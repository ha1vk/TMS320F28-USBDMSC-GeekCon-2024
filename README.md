# TMS320F28-USBDMSC-GeekCon-2024
This code can be opened and compiled using TI's Code Composer Studio. On the TMS320F28 C2000 series chip, if the chip supports USB, the internal flash can be simulated as a mass storage device (MSC), which is not available in the official SDK. At GeekCon 2024, we also designed a secure USB flash drive for demonstration. This is the code of the secure USB flash drive. The data storage area can be protected by turning on DCSM to prevent the JTAG debugger from extracting data.

本代码可以使用TI的Code Composer Studio打开进行编译，在TMS320F28 C2000系列芯片上，如果芯片支持USB，则可以将内部flash模拟为大容量存储设备(MSC)，这是官方SDK中所不具有的。在GeekCon 2024上我们也设计了一个安全U盘用于演示，这个就是安全U盘的代码，可以通过开启DCSM对数据存储区进行保护避免被JTAG调试器提取数据
