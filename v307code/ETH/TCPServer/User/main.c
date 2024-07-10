/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/05/31
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
TCP Server example, demonstrating that TCP Server
receives data and sends back after connecting to a client.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
 */
#include "string.h"
#include "eth_driver.h"
#include "math.h"
#include "../General_Files/drivers/WS2812_PWM_drv.h"
#include "debug.h"

#define KEEPALIVE_ENABLE                1               //Enabe keep alive function

u8 MACAddr[6];                                          //MAC address
u8 IPAddr[4] = {192, 168, 4, 56};                       //IP address
u8 GWIPAddr[4] = {192, 168, 4, 1};                      //Gateway IP address
u8 IPMask[4] = {255, 255, 248, 0};                      //subnet mask
u16 srcport = 1000;                                     //source port

u8 SocketIdForListen;                                   //Socket for Listening
u8 socket[WCHNET_MAX_SOCKET_NUM];                       //Save the currently connected socket
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
u8 MyBuf[RECE_BUF_LEN];


uint32_t temp[300]={0};

u_int32_t ws2812_color[WS2812_NUM]={0x0f0f0f};


void ws2812_set_all(uint32_t color)
{
    for (uint8_t led_id = 0; led_id < WS2812_NUM; led_id++)
    {
        ws2812_color[led_id] = color;
    }
}



void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef  I2C_InitTSturcture = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_SMBusHost;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitTSturcture);

    I2C_Cmd(I2C2, ENABLE);
}



void LED_Init()
{
GPIO_InitTypeDef GPIO_InitStructure = {0};

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_0;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

GPIO_Init(GPIOA, &GPIO_InitStructure);

}


u16 IIC_ReadWord(u16 ReadAddr)
{
    u16 temp = 0;
    u16 highByte = 0;
    u16 lowByte = 0;

    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET)
         ;
     I2C_GenerateSTART(I2C2, ENABLE);

     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
         ;
     I2C_Send7bitAddress(I2C2, 0xB0, I2C_Direction_Transmitter);

     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
         ;

     I2C_SendData(I2C2, (u8)(ReadAddr & 0x00FF));
     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
         ;

     I2C_GenerateSTART(I2C2, ENABLE);

     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
         ;
     I2C_Send7bitAddress(I2C2, 0xB0, I2C_Direction_Receiver);

     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
         ;
     while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
         I2C_AcknowledgeConfig(I2C2, ENABLE);

     lowByte = I2C_ReceiveData(I2C2);
     while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
         I2C_AcknowledgeConfig(I2C2, DISABLE);

     highByte = I2C_ReceiveData(I2C2);
     I2C_GenerateSTOP(I2C2, ENABLE);

    // Combine high byte and low byte into 16-bit value
    temp = ((u16)highByte << 8) | lowByte;

    return temp;
}

















/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_CreateTcpSocketListen
 *
 * @brief   Create TCP Socket for Listening
 *
 * @return  none
 */
void WCHNET_CreateTcpSocketListen(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    i = WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf);
    printf("SocketIdForListen %d\r\n", SocketIdForListen);
    mStopIfError(i);
    i = WCHNET_SocketListen(SocketIdForListen);                   //listen for connections
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_DataLoopback
 *
 * @brief   Data loopback function.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void WCHNET_DataLoopback(u8 id)
{

    u32 len, totallen;
    u8 *p = MyBuf, TransCnt = 255;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    printf("Receive Len = %d\r\n", len);
    totallen = len;
    WCHNET_SocketRecv(id, MyBuf, &len);                                  //Read the data of the receive buffer into MyBuf



    if (strncmp(MyBuf, "CMDOF", 5)) {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    }
    if (strncmp(MyBuf, "CMDON", 5)) {
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
    }

    if (strncmp(MyBuf, "RGBALL", 6) == 0) {
            char hexStr[7] = {0};
            strncpy(hexStr, MyBuf + 6, 6);
            uint32_t colorValue = (uint32_t)strtol(hexStr, NULL, 16);
            ws2812_set_all(colorValue);
            WS2812_SendData(ws2812_color);
        }



    printf("%s",MyBuf);

    sprintf(MyBuf,"DATA: VIN: %lf ,IIN: %lf ,VOUT: %lf ,IOUT: %lf ,temp1: %lf ,temp2: %lf ,fanspeed: %lf ,VI_POUT: %lf \r\n ",
            (temp[0x88] & 0x7ff) / 4.0,
            (temp[0x89] & 0xff) / 256.0,
            (temp[0x8b]) / 512.0,
            (temp[0x8c]&0x07ff)/pow(2,(((~temp[0x8c])&0xf800)>>11))/2 ,
            (temp[0x8d] & 0x07ff) / 2.0,
            (temp[0x8e] & 0x07ff) / 2.0,
            (temp[0x90]) / 1.0,
            ((temp[0x8c] & 0x7ff) / 256.0) * ((temp[0x8b]) / 512.0));

    totallen=strlen(MyBuf);



    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if( !--TransCnt )  break;                                        //Timeout exit
        if(totallen) continue;                                           //If the data is not sent, continue to send
        break;                                                           //After sending, exit
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    u8 i;

    if (intstat & SINT_STAT_RECV)                                 //receive data
    {


        WCHNET_DataLoopback(socketid);                            //Data loopback


    }
    if (intstat & SINT_STAT_CONNECT)                              //connect successfully
    {
#if KEEPALIVE_ENABLE
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
#endif
        WCHNET_ModifyRecvBuf(socketid, (u32) SocketRecvBuf[socketid],
        RECE_BUF_LEN);
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            if (socket[i] == 0xff) {                              //save connected socket id
                socket[i] = socketid;
                break;
            }
        }
        printf("TCP Connect Success\r\n");
        printf("socket id: %d\r\n",socket[i]);

        GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    }
    if (intstat & SINT_STAT_DISCONNECT)                           //disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        printf("TCP Disconnect\r\n");

        GPIO_SetBits(GPIOA, GPIO_Pin_2);

    }
    if (intstat & SINT_STAT_TIM_OUT)                              //timeout disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        printf("TCP Timeout\r\n");
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();                              //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                              //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status){
            printf("PHY Link Success\r\n");
        GPIO_ResetBits(GPIOA, GPIO_Pin_3);}
        else {
            printf("PHY Link Error\r\n");
            GPIO_ResetBits(GPIOA, GPIO_Pin_3);
        }

    }
    if (intstat & GINT_STAT_SOCKET) {                             //socket related interrupt
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}







/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    u8 i;
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);                                    //USART initialize
    printf("TCPServer Test\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n", WCHNET_GetVer());

    LED_Init();
    WS2812_Init();


//

    ws2812_set_all(0x00ff00);
    WS2812_SendData(ws2812_color);
//    Delay_Ms(200);
//    WS2812_SendData(color2);
//    Delay_Ms(200);
//    WS2812_SendData(color3);
//    Delay_Ms(200);
//    WS2812_SendData(color4);
//    Delay_Ms(200);


    if (WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                   //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);           //Ethernet library initialize
    mStopIfError(i);
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
#if KEEPALIVE_ENABLE                                               //Configure keep alive parameters
    {
        struct _KEEP_CFG cfg;

        cfg.KLIdle = 20000;
        cfg.KLIntvl = 15000;
        cfg.KLCount = 9;
        WCHNET_ConfigKeepLive(&cfg);
    }
#endif
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    WCHNET_CreateTcpSocketListen();                               //Create TCP Socket for Listening



        IIC_Init(100000, 0x10);
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        GPIO_SetBits(GPIOA, GPIO_Pin_2);
        GPIO_SetBits(GPIOA, GPIO_Pin_3);

    while(1)
    {




        printf("DATA: VIN: %lf ,IIN: %lf ,VOUT: %lf ,IOUT: `%lf ,temp1: %lf ,temp2: %lf ,fanspeed: %lf ,VI_POUT: %lf \r\n ",
            (temp[0x88] & 0x7ff) / 4.0,
            (temp[0x89] & 0xff) / 256.0,
            (temp[0x8b]) / 512.0,
            (temp[0x8c]&0x07ff)/pow(2,(((~temp[0x8c])&0xf800)>>11))/2 ,
            (temp[0x8d] & 0x07ff) / 2.0,
            (temp[0x8e] & 0x07ff) / 2.0,
            (temp[0x90]) / 1.0,
            ((temp[0x8c] & 0x7ff) / 256.0) * ((temp[0x8b]) / 512.0));


        temp[0x88]=IIC_ReadWord(0x88);
        Delay_Ms(10);
        temp[0x89]=IIC_ReadWord(0x89);
        Delay_Ms(10);
        temp[0x8b]=IIC_ReadWord(0x8b);
        Delay_Ms(10);
        temp[0x8c]=IIC_ReadWord(0x8c);
        Delay_Ms(10);
        temp[0x8d]=IIC_ReadWord(0x8d);
        Delay_Ms(10);
        temp[0x8e]=IIC_ReadWord(0x8e);
        Delay_Ms(10);
        temp[0x90]=IIC_ReadWord(0x90);


        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if(WCHNET_QueryGlobalInt())
        {
            WCHNET_HandleGlobalInt();
        }

    }
}

