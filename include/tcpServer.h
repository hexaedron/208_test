#include <ch32v003fun.h>
#include <cstring>
#include "eth_driver.h"

class tcpServer
{
private:
    uint8_t MACAddr[6];                                          //MAC address
    uint8_t* IPAddr;                     //IP address
    uint8_t* GWIPAddr;                    //Gateway IP address
    uint8_t* IPMask;                    //subnet mask
    uint16_t srcport;                                    //source port

    uint8_t SocketIdForListen;                                   //Socket for Listening
    uint8_t socket[WCHNET_MAX_SOCKET_NUM];                       //Save the currently connected socket
    uint8_t SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
    uint8_t MyBuf[RECE_BUF_LEN];
    bool keepAlive = false;
    struct _KEEP_CFG cfg;

    void DataLoopback(u8 id);
    bool CreateTcpSocketListen(void);
    void TIM2_Init(void);
    void mStopIfError(u8 iError);
    void HandleSockInt(u8 socketid, u8 intstat);

public:
    void setIPAddr(uint8_t* addr);
    void setGWIPAddr(uint8_t* addr);
    void setIPMask(uint8_t* mask);
    void setIPPort(uint16_t port);
    void configKeepAlive(uint32_t KLIdle = 20000, uint32_t KLIntvl = 15000, uint32_t KLCount = 9);
    bool init(void);
    void HandleGlobalInt(void);
    void MainTask(void);
    uint8_t QueryGlobalInt(void);
    tcpServer(uint8_t* IPAddr, uint8_t* GWIPAddr, uint8_t* IPMask, uint16_t IPPort);
    ~tcpServer();
};