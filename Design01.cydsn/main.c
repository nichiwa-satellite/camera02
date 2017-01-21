/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdbool.h>

#define SZ_COMMAND  8

typedef enum PSC_STATEtag
{
    PSC_ST_IDLE,
    PSC_ST_RUN,
    PSC_ST_MAX
    
}PSC_STATE;

typedef enum DEV_IDtag
{
    DEV_ID_COMM,
    DEV_ID_CAM,
    DEV_ID_MAX,
}DEV_ID;

typedef struct PSC_CMDtag
{
    DEV_ID  dev_id;
    char    cmd[SZ_COMMAND];    
    
}PSC_CMD;


uint8 rec;
uint8 get[]            ={0x56, 0x00, 0x11, 0x00};
uint8 reset_camera[]    =  { 0x56, 0x00, 0x26, 0x00 };
uint8 baurate[]         =  { 0x56, 0x00, 0x24, 0x16, 0x01, 0x2A, 0xF2 };
uint8 config_datasize[] =  { 0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x00 };    //640×480
uint8 take_picture[]    =  { 0x56, 0x00, 0x36, 0x01, 0x00 };
uint8 read_datasize[]   =  { 0x56, 0x00, 0x34, 0x01, 0x00 };
uint8 load_data[]       =  { 0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCA, 0x6C, 0x00, 0xFF };

PSC_STATE PSC_PROG_STATE;           //status

bool PSC_Initialize();                  //INITIALIZE
bool PSC_CmdHandle(PSC_CMD*);                   //CommandHandling

bool PSC_GetCommand(PSC_CMD*);    //Get Command form Device
bool PSC_SndCommand(PSC_CMD*);    //Send Command to Device

bool psc_GetRecvLine(DEV_ID, char*);
bool psc_GetRecvChar(DEV_ID, char*);

bool psc_SndDataLine(PSC_CMD*);



int main(void)
{
    PSC_CMD stCmd;

    if( PSC_Initialize() == false )
    {
       // ERROR
    }

    memset(&stCmd,0x00,sizeof(stCmd));

    while(1)
    {
        //Get Command
        if(PSC_GetCommand(&stCmd) == false )
        {
            //Get ERROR
            continue;
        }
        
        //Process Command
        if(PSC_CmdHandle(&stCmd) == false )
        {
            //Get ERROR
            continue;
        }
        
    }


}

// INITIALIZE
bool PSC_Initialize()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_TO_CAMERA_Init();
    UART_TO_COMM_Init();
    PSC_PROG_STATE = PSC_ST_IDLE;
    return true;
}

bool PSC_GetCommand(PSC_CMD* pstData)
{
    DEV_ID dev_id;
    char cmd[SZ_COMMAND];
    
    switch(PSC_PROG_STATE)
    {
        case PSC_ST_IDLE:
            dev_id = DEV_ID_COMM;
            break;
        case PSC_ST_RUN:
            dev_id = DEV_ID_CAM;
            break;
        default:
            dev_id = DEV_ID_MAX;
            break;
    }
    if( dev_id == DEV_ID_MAX)
    {
        return false;
    }
    
    if( psc_GetRecvLine(dev_id, cmd) == false )
    {
            return false;
    }
    
    memcpy(pstData->cmd, cmd, sizeof(pstData->cmd));
    pstData->dev_id = dev_id;
    
    return true;
}

/* [] END OF FILE */
