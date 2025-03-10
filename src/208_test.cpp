#include <ch32fun.h>
#include <ch32v20xhw.h>

#include "funny_defs.h"
#include "funny_time.h"
#include "simpleTimer.h"
#include "SystemInit120_HSE32.h"

//#include "ethIF.h"
//#include "tcpServer.h"
//#include "tcpClient.h"
#include "MQTTClient.h"

#include <cstdlib>

#include "GTimer.h"

//static uint8_t IPAddr[4]    = {0,0,0,0};//{ 192, 168, 1, 43 };                   //IP address
static uint8_t destIPAddr[4]    = { 192, 168, 1, 253 };                   //IP address
//static uint8_t GWIPAddr[4]  = {0,0,0,0};//{ 192, 168, 1, 1 };                    //Gateway IP address
//static uint8_t IPMask[4]    = {0,0,0,0};//{ 255, 255, 255, 0 };                  //subnet mask
//uint16_t srcport = 1000; 
#define SUB_TOPIC_COUNT 2UL

char receivedBuf[128] = "Received message: ";
bool msgFlag = false;

// This function is being called each time the MQTT client receives any topic.
// So all the incoming topic handling should be done there.
void topicCallback(char* topicName, uint8_t* topicPayload, int payloadLen, int topicQos, unsigned char retained, unsigned char dup )
{
    memcpy(receivedBuf + 18, topicPayload, payloadLen);
    receivedBuf[payloadLen + 17] = '\0';
    msgFlag = true;
}

int main()
{  
    SystemInit120_HSE32();
	
    system_initSystick();

   ////ethIF myIF(IPAddr, GWIPAddr, IPMask);
   ethIF myIF;
   myIF.configKeepAlive();
   if(!myIF.init())
   {
        while (1){}  
   }

    MQTTClient<SUB_TOPIC_COUNT> myClient(&myIF, destIPAddr, 1883, 10);

    myClient.addSubTopic((char*)"ch32topic/test/cmd1");
    myClient.addSubTopic((char*)"ch32topic/test1/cmd2");
    myClient.registerTopicCallback(topicCallback);

    myClient.addWillTopic((char*)"ch32topic/test/will", myIF.getDnsName());

    GTimer<millis32> myTimer(3000);
    myTimer.setMode(GTMode::Interval);
    myTimer.keepPhase(true);
    myTimer.start();

    char uptimeStr[128] = "ch32v208 uptime is  ";

    while(1)
    {
        myIF.mainTask();        // Ethernet library main task function, should be called cyclically
        myClient.mainTask();    // The same for MQTT client main task

        if(myTimer)
        {
            char buf[14];

            itoa(millis32() / 1000, buf, 10);

            memcpy(uptimeStr + 19, buf, strlen(buf) + 1);
            strcat(uptimeStr, " seconds.");

            
            myClient.MQTTPublish((char*)"ch32topic/test/str", 0, uptimeStr);

        }

        if(msgFlag)
        {
            myClient.MQTTPublish((char*)"ch32topic/test/receivedstr", 0, receivedBuf);
            msgFlag = false;
        }

    }
}

