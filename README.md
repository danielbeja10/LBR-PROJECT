# TPIL - LBR PROJECT

The project contains both Linux Kernel Module and a user-space CLI to configure and run the LBR of the CPU on a certain program.


## 📂 Project Structure

 ### Kernel Module
1. **lbr_interface** - Kernel module implementation with IOCTL.
2. **lbr_logger**- logging the input of the LBR.
3. **lbr_control** - enable, disable and configure the LBR.
4. **lbr_info** - get the information about the LBR in its current state: depth options, max depth, current MSR CTL state and current depth.

### User Space
1. **lbrctl_common** - contains the USAGE, the code the open the device in the user space and responsible of writing the json file with the branches.
2. **lbrctl_config** - configure the LBR according to the user request.
3. **lbrctl_run** - run the LBR along with a program.
4. **lbrctl_status** - show the user the information of the LBR.
5. **main** - run the program.(config/run/status)

## ⚙️ Build Instructions

> **Note:** All those command should have: **taskset -c 0** , before them because we want to use only 1 core of the CPU.

1. **make** (you can also make the moudle and the user in a different command: **make moudle**, **make user**).
2. **sudo insmod lbr_driver.ko** -  in order to activate the module.
3. if needed - check kernel logs: **dmesg | tail -n 20**
4. configure the LBR for example: **sudo ./lbrctl config depth 8 usr ker.**
5. check the status: **sudo ./lbrctl status**
6. run a program under LBR: **sudo ./lbrctl run -- ./my_program.**
7. when finish running, close the Kernel module: **sudo rmmod lbr_driver.ko**
8. **make clean** to delete the compiled files.

**Simple Test with given program**
the project contain simple program to test the LBR with, before building the project you can:
1. **gcc -O0 -g -o my_program my_program.c**
2. run the program. for example: **./my_program 8**
3. the output should be:
- when x > 10 output big
- when 5 < x < 10  output medium
- when  x < 5 output small   




-----------------------------
Inside the docs folder there is the document for the client and the project instructions.
