TPIL - LBR PROJECT

The project contains both Linux Kernel Module and a user-space CLI to configure and run the LBR of the CPU on a certain program.

project structure

1. lbr_interface - Kernel moudle implemntaion with IOCTL.
2. lbr_logger - logging the input of the LBR.
3. lbr_control - enable, disable and configure the LBR.
4. lbr_info - get the information about the LBR in its current state: depth options, max depth, current MSR CTL state and current depth.
5. lbr_common - 
