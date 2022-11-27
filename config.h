#ifndef CONFIG_H
#define CONFIG_H

//#define unsigned int uint  

enum APP_Num{APP1 = 1,APP2};

enum ECU_E{MPC5744 = 0,RT1064,PIC18F};

enum Type {
    CANalyst = 0,
    PEAKCAN = 1,
    KVASER = 2,
    ZLGCAN= 3
};
/*消息码*/
#define B_BOOT  0x00
#define B_APP   0x01
#define B_DATA  0x02
#define B_END   0x03
#define B_EARSE 0x05
#define B_VERSION     0x06  //版本信息

#define B_APP1    0xA1
#define B_APP2    0xA2
/*提示码*/
#define M_CONNECT_CAN 0xC1  //链接CAN通道
#define M_TIMEOUT     0xC2  //等待超时
#define M_BUILD_COMM  0xC3  //建立连接

/*错误码*/
#define E_Frame       0xE1
#define E_CONNECT_CAN 0xE2  //连接CAN通道错误
#define E_FILESYSTEM  0xE3  //底层文件系统异常

/*版本信息*/
#define VERSION "V1.31 \n在1.30基础上修改为博世版本"

#define SLAVE 3
#endif // CONFIG_H
