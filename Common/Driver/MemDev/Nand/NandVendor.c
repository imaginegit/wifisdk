/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: NandVendor.c
Desc: 

Author: chenfen
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/


#include "MDConfig.h"


#if (NAND_DRIVER == 1)

#ifdef IN_LOADER

#endif

/*------------------------------------ Defines -------------------------------*/

#define NAND_SAMSUNG            1
#define NAND_TOSHIBA            1
#define NAND_HYNIX              1
#define NAND_MICRON             1


/*----------------------------------- Typedefs -------------------------------*/

typedef struct _HYNIX_INFO
{
    uint8 type;
    uint8 RetryCnt;
    uint8 RRRegCnt;
    uint8 RetryIdx[4];
    uint8 *RRRegAddr;    
    uint8 *SLCRegAddr;
    uint8 SLCRegCnt;
    
} HYNIX_INFO, *pHYNIX_INFO;

/*-------------------------- Forward Declarations ----------------------------*/


/* ------------------------------- Globals ---------------------------------- */


/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

#if (1 == NAND_SAMSUNG)

#define SAMS_MAX_READ_RETRY 15

_ATTR_FLASH_CODE_
const uint8 SamsungRRReg[4] = {0xA7, 0xA4, 0xA5, 0xA6}; //REG

_ATTR_FLASH_CODE_
const int8 SamsungRefValue[SAMS_MAX_READ_RETRY][4]=
{
    {0x00, 0x00, 0x00, 0x00}, //0
    {0x05, 0x0A, 0x00, 0x00},
    {0x28, 0x00, 0xEC, 0xD8},
    {0xED, 0xF5, 0xED, 0xE6},
    {0x0A, 0x0F, 0x05, 0x00},
    {0x0F, 0x0A, 0xFB, 0xEC},
    {0xE8, 0xEF, 0xE8, 0xDC},
    
    {0xF1, 0xFB, 0xFE, 0xF0}, //7
    
    {0x0A, 0x00, 0xFB, 0xEC},
    {0xD0, 0xE2, 0xD0, 0xC2},
    {0x14, 0x0F, 0xFB, 0xEC},
    {0xE8, 0xFB, 0xE8, 0xDC},
    {0x1E, 0x14, 0xFB, 0xEC},
    {0xFB, 0xFF, 0xFB, 0xF8},
    {0x07, 0x0C, 0x02, 0x00}, //14
};

/*
Name:        SamsungReadRetry
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 SamsungReadRetry(uint32 sec, void *pData, void *pSpare, uint8 nSec)
{
    uint8 CS;
    uint32 i, reg;
    int32 ret;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;

    NandIOMux();
    NandWaitBusy(NULL);       //wait busy

    CS = NandGetCS(sec, NULL);    
    NandIF = spec->NandIF[CS];

    for (i=1; i<SAMS_MAX_READ_RETRY; i++)
    {
        NandCs(CS);
        for (reg=0; reg<4; reg++)
        {
            NandIF->cmd=0xa1;
            NandIF->data=0;
            NandIF->data=SamsungRRReg[reg]; // ADDR
            NandIF->data=SamsungRefValue[i][reg];
            NandDelay(20); //delay 300ns 
        }
        NandDeCs(CS);
          
        ret = NandRead(sec, pData, pSpare, nSec, -1);     //不能预读
        //NandWaitBusy(NULL); 		//wait busy
        if (ERROR != ret)
            break;
    }

    NandCs(CS);              
    for (reg=0; reg<4; reg++)
    {
        NandIF->cmd=0xa1;
        NandIF->data=0;
        NandIF->data=SamsungRRReg[reg]; // ADDR
        NandIF->data=SamsungRefValue[0][reg];
        NandDelay(20); //delay 300ns 
    }
    NandDeCs(CS);
        
    if(i >= 8 && ret >= (spec->EccBits/2))
    {
        //ReadRety的次数，BCH出错比特数大于一定值时，refresh
        NandRefreshRec(sec);
    }

    NandIODeMux();
    return ret;
}

/*
Name:       SamsungInit
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 SamsungInit(uint8 *IDByte)
{
    uint8 tech;
    
    tech = IDByte[5] & 0x7;
    if (tech < 0x3)         //判断制程是否是 27nm 以上 0x3:27nm  0x4:21nm
        return OK;

    if (tech == 0x3)        //27nm
    {
        uint32 DevID = IDByte[1]<<24 | IDByte[2]<<16 | IDByte[3]<<8 | IDByte[4];
        if (DevID == 0xD7947A54 || DevID == 0xDED57A58)//K9GBG08U0A, K9LCG08U0A ,K9HDG08U1A
        {
            NandSetRdmzflag(1); //enable randonmize
        }
        return OK;
    }

    //21nm K9GBG08U0B   K9GCG08U0A  K9PHGx8x5A
    NandSetRdmzflag(1); //enable randonmize
    NandSetReadRetry(SamsungReadRetry);

    return OK;
}

#endif

/*-----------------Toshiba Nandflash code-------------------------------------*/

#if (1 == NAND_TOSHIBA)
#define TOSH_MAX_READ_RETRY 5

_ATTR_FLASH_CODE_ 
int8 const ToshibaRefValue[5]={0x04, 0x7C, 0x78,0x74,0x08};
/*
Name:        ToshibaReadRetry
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static uint32 ToshibaReadRetry(uint32 sec, void *pData, void *pSpare, uint8 nSec)
{
    int32 ret;
    uint32 i, CS, reg;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;

    NandIOMux();
    NandWaitBusy(NULL);       //wait busy

    CS = NandGetCS(sec, NULL);    
    NandCs(CS);
    NandIF = spec->NandIF[CS];

    NandIF->cmd=0x5C;
    NandIF->cmd=0xC5;
    for (i=0; i<TOSH_MAX_READ_RETRY; i++)
    {
        NandCs(CS);
        for (reg=0; reg<4; reg++) 
        {
            NandIF->cmd=0x55;
            NandIF->addr=4+reg;
            NandDelay(5);
            NandIF->data=ToshibaRefValue[i];
        }
        NandIF->cmd=0x26;
        NandIF->cmd=0x5D;
        NandDeCs(CS);

        ret = NandRead(sec, pData, pSpare, nSec, -1);     //不能预读
        if (ERROR != ret)
            break;
    }
    NandCs(CS);
    NandIF->cmd=0xFF;       //退出ReadRetrial模式

    if(i >= 2 && ret >= (spec->EccBits/2))
    {
        NandRefreshRec(sec);
    }
    
    NandDelay(100); //wait 10us,here wait 200ns,FlashWaitBusy will wait 10us
    NandWaitBusy(NULL); 
    NandDeCs(CS);

    NandIODeMux();
    return (ret);
}


/*
Name:        ToshibaInit
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 ToshibaInit(uint8 *IDByte)
{
    uint8 tech;
    //uint32 DevID;
    
    tech = IDByte[5] & 0x7;
    if (tech < 0x6)         //判断制程是否是 2xnm 以上 0x6:24nm  0x7:19nm
        return OK;

    //DevID = IDByte[1]<<24 | IDByte[2]<<16 | IDByte[3]<<8 | IDByte[4];

    NandSetRdmzflag(1); //enable randonmize
    NandSetReadRetry(ToshibaReadRetry);

    return OK;
}
#endif


/*----------------------Micron Nandflash code---------------------------------*/

#if (1 == NAND_MICRON)

#define MICRON_MAX_READ_RETRY  7

/*
Name:        MicronReadRetry
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 MicronReadRetry(uint32 sec, void *pData, void *pSpare, uint8 nSec)
{
    uint8 CS;
    uint32 i, reg;
    int32 ret;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;

    NandIOMux();
    NandWaitBusy(NULL);       //wait busy
    
    CS = NandGetCS(sec, NULL);    
    NandIF = spec->NandIF[CS];

    for (i=0; i<MICRON_MAX_READ_RETRY; i++)
    {
        NandCs(CS);
        NandIF->cmd=0xEF;
        NandIF->addr=0x89;
        NandDelay(5);       //tADL
        NandIF->data=i+1;
        NandIF->data=0;
        NandIF->data=0;
        NandIF->data=0;
        NandDeCs(CS);
        
        ret = NandRead(sec, pData, pSpare, nSec, -1);     //不能预读
        //NandWaitBusy(NULL); 		//wait busy
        if (ERROR != ret)
            break;
    }
        
    NandCs(CS); // SET Default Value
    NandIF->cmd=0xEF;
    NandIF->addr=0x89;
    NandDelay(4); //tADL
    NandIF->data=0;
    NandIF->data=0;
    NandIF->data=0;
    NandIF->data=0;
    NandDeCs(CS);

    if(i >= 2 && ret >= (spec->EccBits/2))
    {
        NandRefreshRec(sec);
    }
    NandIODeMux();

    return (ret);
}

/*
Name:       SamsungInit
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 MicronInit(uint8 *IDByte)
{
    uint32 DevID = IDByte[1]<<24 | IDByte[2]<<16 | IDByte[3]<<8 | IDByte[4];

    if(DevID == 0x64444BA9 || DevID == 0x84C54BA9)  //MT29F64G08CBABA 20nm  L84 MT29F256G08CJABB 20nm L84
    {
        NandSetRdmzflag(1); //enable randonmize
        NandSetReadRetry(MicronReadRetry);
    }

    return OK;
}

#endif


/*---------------Hynix Nandflash code-----------------------------------------*/

#if (1 == NAND_HYNIX)

//#define     HYNIX_ESLC_PROG       //目前只有 usbplug用到

#define MAX_HYNIX_NUM       2       //为了省空间 只考虑 最多接两片 hynix flash

_ATTR_FLASH_INIT_CODE_
const int8 Hynix26nRefValue[7][4]=
{
    {+0x00, +0x00, +0x00, +0x00}, //defaultValue
    {+0x00, +0x06, +0x0a, +0x06},
    {+0x00, -0x03, -0x07, -0x08},
    {+0x00, -0x06, -0x0d, -0x0f},
    {+0x00, -0x0b, -0x14, -0x17},  //-0x0b 与 refValue32g -0x09
    {+0x00, +0x00, -0x1a, -0x1e},
    {+0x00, +0x00, -0x20, -0x25}
};

/*---------------Hynix Read Retry Register Address----------------------------*/
_ATTR_FLASH_CODE_
const uint8 H26NRRRReg[] = {0xAC, 0xAD, 0xAE, 0xAF};

_ATTR_FLASH_CODE_
const uint8 H20NRRRRegA[] = {0xCC, 0xBF, 0xAA, 0xAB, 0xCD, 0xAD, 0xAE, 0xAF};

_ATTR_FLASH_CODE_
const uint8 H20NRRRRegB[] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7};

/*---------------Hynix E-SLC Program Register Address-------------------------*/

#ifdef HYNIX_ESLC_PROG

_ATTR_FLASH_CODE_
const uint8 H26NSLCReg64[] = {0xA4, 0xA5, 0xB0, 0xB1, 0xC9};

_ATTR_FLASH_CODE_
const uint8 H26NSLCReg32[] = {0xA0, 0xA1, 0xB0, 0xB1, 0xC9};

_ATTR_FLASH_CODE_
const uint8 H20NSLCRegA[] = {0xB0, 0xB1, 0xA0, 0xA1};

_ATTR_FLASH_CODE_
const uint8 H20NSLCRegB[] = {0xA0, 0xA1, 0xA7, 0xA8};
#endif

/*----------------------------------------------------------------------------*/

_ATTR_FLASH_BSS_
int8 HynixRefValue[MAX_HYNIX_NUM][8][8];       

#ifdef HYNIX_ESLC_PROG
_ATTR_FLASH_BSS_
uint8 HynixSLCDefValue[5];    //Default Program Verify Level

_ATTR_FLASH_BSS_
uint8 HynixSLCOptValue[5];    //Optimized Program Verify Level 
#endif

_ATTR_FLASH_BSS_  HYNIX_INFO HynixInfo;

#ifdef HYNIX_ESLC_PROG   //只有usbplug 用到
/*
Name:        HynixSetESLC
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
void HynixSetESLC(uint8 Enable)
{
    uint32 i;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;
    pHYNIX_INFO pHynix = &HynixInfo;
    uint8 *pValue;

    if (spec->Vonder != HYNIX || !pHynix->type)
        return;

    NandWaitBusy(NULL); 

    NandCs(0);
    NandDelay(4);               //tCS >= 100ns
    NandIF = spec->NandIF[0];
    pValue = (uint8 *)((1==Enable)?  HynixSLCOptValue : HynixSLCDefValue);
    NandIF->cmd=0x36;
    for (i=0; i<pHynix->SLCRegCnt; i++)       
    {
        NandIF->addr = pHynix->SLCRegAddr[i];
        NandDelay(10);       //tADL >= 200ns
        NandIF->data = *pValue++;
    }
    
    NandIF->cmd=0x16;
    if (0 == Enable && pHynix->type>2)
    {
        NandReadSpareCmd(0, 0, 4);
        NandWaitBusy(NULL);
    }
    
    NandDeCs(0);
}

/*
Name:        HynixGetSLCParam
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
void HynixGetSLCParam(void)
{
    uint32 i;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;
    pHYNIX_INFO pHynix = &HynixInfo;
    uint8 AddValue = 0;

    if (spec->Vonder != HYNIX || !pHynix->type)
        return;

    NandCs(0);
    NandIF = spec->NandIF[0];
    NandDelay(2);             //tCS >= 100ns      
    NandIF->cmd=0x37;
    for (i=0; i<pHynix->SLCRegCnt; i++)       
    {
        NandIF->addr = pHynix->SLCRegAddr[i];
        HynixSLCDefValue[i] = NandIF->data;
    }
    NandDeCs(0);

    if (pHynix->type ==1)  //26nm 64Gb
    {
        AddValue = 0x25;
    }
    else if (pHynix->type ==2) //26nm 32Gb
    {
        AddValue = 0x26;
    }
    else if (pHynix->type >2) //20nm
    {
        AddValue = 0x0A;
    }

    for (i=0; i<pHynix->SLCRegCnt; i++)     
    {
        if (i==4)
            AddValue = 0x01;
        HynixSLCOptValue[i] = HynixSLCDefValue[i]+AddValue;
    }
}
#endif

/*
Name:        HynixSetRRDefault
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void HynixSetRRDefault(void)
{
    uint8 i, CS;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;
    pHYNIX_INFO pHynix = &HynixInfo;

    if (spec->Vonder != HYNIX || !pHynix->type)
        return;

    NandIOMux();
    NandWaitBusy(NULL); 
    for (CS=0; CS<MAX_HYNIX_NUM; CS++)
    {
        if (!spec->TotPhySec[CS])
            break;

        NandCs(CS);
        NandDelay(2); //tCS >= 100ns
        NandIF = spec->NandIF[CS];
        NandIF->cmd=0x36;

        for (i=0; i<pHynix->RRRegCnt; i++)   
        {
            NandIF->addr = pHynix ->RRRegAddr[i];
            NandDelay(4);   //tADL >= 200ns
            NandIF->data = HynixRefValue[CS][0][i];
        }
        
        NandIF->cmd=0x16;
        NandDeCs(CS);
    }

    NandIODeMux();
}

/*
Name:        HynixReadRetry
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static void HynixSetRRValue(uint8 CS, uint32 index)
{
    uint32 reg;
    pHYNIX_INFO pHynix = &HynixInfo;
    pNAND_IF NandIF = NandSpec.NandIF[CS];

    NandCs(CS);
    NandDelay(4); //tCS >= 100ns
    NandIF->cmd=0x36;

    for (reg=0; reg<pHynix->RRRegCnt; reg++)   //2step 2: set new register data
    {
        NandIF->addr=pHynix->RRRegAddr[reg];
        NandDelay(10);   //tADL >= 200ns
        NandIF->data = HynixRefValue[CS][index][reg];
    }
    NandIF->cmd=0x16;
    NandDeCs(CS);
}

/*
Name:        HynixReadRetry
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 HynixReadRetry(uint32 sec, void *pData, void *pSpare, uint8 nSec)
{
    uint8 CS;
    uint32 i, index;
    int32 ret;
    pFLASH_SPEC spec = &NandSpec;
    pHYNIX_INFO pHynix = &HynixInfo;

    NandIOMux();
    NandWaitBusy(NULL); 

    CS = NandGetCS(sec, NULL);    
    index = pHynix->RetryIdx[CS];     //指向下一次 READ RETRY 的开始
    for (i=0; i<pHynix->RetryCnt+1; i++)    // +1的目的是都读不对的时候 RefValue 回到原值
    {
        if( ++index > pHynix->RetryCnt)
            index = 0;

        HynixSetRRValue(CS, index);
        ret = NandRead(sec, pData, pSpare, nSec, -1);     //不能预读
        //NandWaitBusy(NULL); 		//wait busy
        if (ERROR != ret)
			break;
    }
    
    pHynix->RetryIdx[CS] = index;    
    #ifndef IN_LOADER
    if(index >= 2 && ret >= (spec->EccBits/2))
    {
        int32 ret1;
        for (i=i+1; i<pHynix->RetryCnt; i++)
        {
            if( ++index > pHynix->RetryCnt)
                index = 0;

            HynixSetRRValue(CS, index);
            ret1 = NandRead(sec, NULL, NULL, nSec, -1);     //不能预读
            if (ERROR != ret1 && ret1 < spec->EccBits/2)
            {
                pHynix->RetryIdx[CS] = index;
                break;
            }
        }
        if (ERROR == ret1 ||  ret1 >= spec->EccBits/2)
        {
            HynixSetRRValue(CS, pHynix->RetryIdx[CS]);
            NandRefreshRec(sec);
        }
    }
    #endif
    NandIODeMux();
    return (ret);
}


/*
Name:        HynixGetRRTab
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 HynixGetRRTab(void)
{
    uint32 ret = OK;
    uint32 i, j, k, cs;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;
    pHYNIX_INFO pHynix = &HynixInfo;

    if (!pHynix->type)  //pHynix->type =0 不是2xnm 的flash
        return ERROR;

    NandIOMux();
    for (cs=0; cs<MAX_HYNIX_NUM; cs++)  
    {        
        if (!spec->TotPhySec[cs])
            break;
    
        NandCs(cs);  
        NandDelay(2); //tCS >= 100ns
        NandIF = spec->NandIF[cs];

        if (pHynix->type<3)        //26nm
        {
            int8 DefaultValue[4];
        
            NandIF->cmd=0x37;
            for (i=0; i<4; i++)     // 4 reg     
            {
                NandIF->addr=pHynix->RRRegAddr[i];
                NandDelay(2);                       //tWHR >= 80ns
                DefaultValue[i]=NandIF->data;
            }

            for (j=0; j<7; j++)             // 1 default + 6 read retry
            {
                if (j>1)
                    DefaultValue[0] = 0; //FIX 0
                    
                if (j>4)
                    DefaultValue[1] = 0; //FIX 0
            
                for (i=0; i<4; i++)
                    HynixRefValue[cs][j][i] = Hynix26nRefValue[j][i] + DefaultValue[i];
            }
        }
        else            //20nm
        {
            volatile uint8 tmp;
            uint8 *pRefValue;
        
            NandIF->cmd=0xFF;
            NandWaitBusy(NULL);
            NandIF->cmd=0x36;
            if(pHynix->type == 3)        //A-DIE
            {
                NandIF->addr=0xFF;
                NandDelay(4);           //tADL >= 200ns
                NandIF->data=0x40;
                NandIF->addr=0xCC;
            }
            else                        //B-DIE
            {
                NandIF->addr=0xAE;
                NandDelay(4);           //tADL >= 200ns
                NandIF->data=0x00;
                NandIF->addr=0xB0;
            }

            NandDelay(4);               //tADL >= 200ns
            NandIF->data=0x4D;
            NandIF->cmd=0x16;
            NandIF->cmd=0x17;
            NandIF->cmd=0x04;
            NandIF->cmd=0x19;
            NandIF->cmd=0x00;
            NandIF->addr=0x00;
            NandIF->addr=0x00;
            NandIF->addr=0x00;
            NandIF->addr=0x02;
            NandIF->addr=0x00;
            NandIF->cmd=0x30;
            NandWaitBusy(NULL);
            tmp = NandIF->data; //Total RR count
            tmp = NandIF->data; //RR Reg count
            
            for (k=0; k<8; k++) //8 copy set
            {
                for (j=0; j<8; j++) // 1 default + 7 RetryCount
                {
                    for (i=0; i<8; i++) // 8 RegCount
                        HynixRefValue[cs][j][i] = NandIF->data;
                }

                pRefValue = (uint8*)&HynixRefValue[cs][0][0];
                for (i=0; i<64; i++)
                {
                    tmp = NandIF->data;
                    if ((tmp+pRefValue[i]) != 0xFF)
                        break;
                }
                if (i>=64)
                    break;
            }
            
            if (k>=8)
                ret = ERROR;

            NandIF->cmd=0xFF;
            NandWaitBusy(NULL);
            NandIF->cmd=0x38;
            NandWaitBusy(NULL);
        }
        
        NandDeCs(cs);
    }

    return ret;
}

/*
Name:        HynixGetSLCParam
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 HynixInit(uint8 *IDByte)
{
    uint32 DevID;
    pHYNIX_INFO pHynix = &HynixInfo;
    uint8 tech;
    pFLASH_SPEC spec = &NandSpec;

    memset(pHynix, 0, sizeof(HYNIX_INFO));

    tech = IDByte[5] & 0x7;
    if (tech < 0x3)         //判断制程是否是 2xnm 以上
        return OK;
    
    DevID = IDByte[1]<<24 | IDByte[2]<<16 | IDByte[3]<<8 | IDByte[4];
    switch (DevID)
    {

        case 0xDE94D204:   //hynxi 26nm 8GB flash
            pHynix->type= 1;
            #ifdef HYNIX_ESLC_PROG
            pHynix->SLCRegAddr = (uint8*)H26NSLCReg64;
            #endif
            break;
           
        case 0xD794DA74:   //hynxi 26nm 4GB flash
            pHynix->type = 2;
            *((uint8 *)&H26NRRRReg[0]) = 0xA7; 
            *((int8 *)&Hynix26nRefValue[4][1]) = -0x09;
            #ifdef HYNIX_ESLC_PROG
            pHynix->SLCRegAddr = (uint8*)H26NSLCReg32;
            #endif
            break;
        case 0xDE94DA74:   //H27UCG08U0A 20nm 8GB A-DIE
            pHynix->type = 3;
            pHynix->RRRegAddr = (uint8*)H20NRRRRegA;
            #ifdef HYNIX_ESLC_PROG
            pHynix->SLCRegAddr = (uint8*)H20NSLCRegA;
            #endif
            break;
        case 0xD7949160:   //H27UBG08U0C 20nm 4GB flash
        case 0xDE94EB74:   //H27UCG08U0B 20nm 8GB B-DIE
            pHynix->type = 4;
            pHynix->RRRegAddr = (uint8*)H20NRRRRegB;
            #ifdef HYNIX_ESLC_PROG
            pHynix->SLCRegAddr = (uint8*)H20NSLCRegB;
            #endif
            break;
        default:
            break;
    }

    if (pHynix->type)
    {
        //NandSpec.EnESLC = 1;
        NandSetRdmzflag(1); //enable randonmize
        spec->LSBMode = 1;
        spec->NeedRRT = 1;
        //NandSetReadRetry(HynixReadRetry);
        if (pHynix->type < 3)    //26nm
        {
            pHynix->RetryCnt = 6;
            pHynix->RRRegCnt = 4;
            pHynix->SLCRegCnt = 5;
            pHynix->RRRegAddr = (uint8*)H26NRRRReg;
        }
        else                    //20nm
        {
            pHynix->RetryCnt = 7;
            pHynix->RRRegCnt = 8;
            pHynix->SLCRegCnt = 4;
        }
    }

    return OK;
}
#else
_ATTR_FLASH_CODE_
void HynixSetRRDefault(void)
{
}
#endif

/*
Name:        NandGetRRT
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 NandGetRRT(uint8 *pRRTab)
{
    uint32 ret = ERROR;
    #if (1 == NAND_HYNIX)
    if (NandSpec.NeedRRT)
    {
        if (pRRTab)
        {
            memcpy(HynixRefValue, pRRTab, MAX_HYNIX_NUM*8*8);
            pRRTab += MAX_HYNIX_NUM*8*8;
            #ifdef HYNIX_ESLC_PROG
            memcpy(HynixSLCDefValue, pRRTab, 5);
            memcpy(HynixSLCOptValue, pRRTab+5, 5);
            #endif
            ret = OK;
        }
        else
        {
            #ifdef HYNIX_ESLC_PROG
            HynixGetSLCParam();
            #endif
            ret = HynixGetRRTab();              
        }
        if (OK == ret)
        {
            NandSetReadRetry(HynixReadRetry);
        }
    }
    #endif
    return ret;
}


/*
Name:   NandVendorInit     
Desc:        
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 NandVendorInit(uint8 *IDByte)
{
    uint32 ret = OK;

    switch (IDByte[0])
    {
        case 0xEC: 
            #if (1 == NAND_SAMSUNG)
            ret = SamsungInit(IDByte);
            break;
            #endif

        #if (1 == NAND_TOSHIBA)   
        case 0x98:         
        case 0x45:  //sandisk
            ret = ToshibaInit(IDByte);
            break;
        #endif

        #if (1 == NAND_MICRON)  
        case 0x2C:  
            ret = MicronInit(IDByte);
            break;
        #endif

        #if (1 == NAND_HYNIX)
        case 0xAD:   
            ret = HynixInit(IDByte);
            break;
        #endif
            
        default:
            break;
    }

    return ret;
}

#endif
