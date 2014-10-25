/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: MemDev.c
Desc: 

Author: chenfen
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/
#include "MDconfig.h"
#include "SDconfig.h"


/*------------------------------------ Defines -------------------------------*/

/*----------------------------------- Typedefs -------------------------------*/


//memory operation function
typedef uint32 (*MEM_INIT)(void);
typedef uint32 (*MEM_READ)(uint8 lun, uint32 SecAdrr, uint32 nSec, void *pBuf);
typedef uint32 (*MEM_WRITE)(uint8 lun, uint32 SecAdrr, uint32 nSec, void *pBuf);
//typedef uint32 (*MEM_IOCTRL)(uint32 cmd, uint32 arg, void *pBuf);

typedef void (*MEM_DEINIT)(void);
typedef uint32 (*MEN_GETDEVINFO)(uint8 lun, pMEMDEV_INFO pDevInfo);
typedef bool (*MEM_CHECKEVENT)(uint32 event);

typedef uint32 (*MEM_PROBE)(void);


typedef struct tagMEM_DEVICE
{
    MEM_INIT init;
    MEM_READ read;
    MEM_WRITE write;
    MEN_GETDEVINFO GetInfo;
    MEM_CHECKEVENT CheckEvent;
    MEM_PROBE probe;     
    MEM_DEINIT DeInit;
    //char*  name;
    uint16 major;
    uint8 removable;
    uint8 reserved;
    //uint32 priv;

}MEM_DEVICE, *pMEM_DEVICE;


/*-------------------------- Forward Declarations ----------------------------*/
/* ------------------------------- Globals ---------------------------------- */
_ATTR_SYS_BSS_ uint32 	SysProgDiskCapacity;			//系统程序盘容量
_ATTR_SYS_BSS_ uint32 	SysProgRawDiskCapacity;			//系统程序盘容量
_ATTR_SYS_BSS_ uint32 	SysDataDiskCapacity;			//系统程序盘容量
_ATTR_SYS_BSS_ uint32 	SysUserDisk2Capacity;			//系统程序盘容量

_ATTR_SYS_BSS_  uint16    UpgradeTimes;
_ATTR_SYS_BSS_  uint16    LoaderVer;
_ATTR_SYS_BSS_  uint8     NandIOMuxRef;         //Nand与LCD IO 切换计数          

_ATTR_SYS_CODE_
static MEM_DEVICE const gMemDevTab[] =
{
    #if (NAND_DRIVER==1) 
    {
        FTLInit,
        FtlRead,
        MDFtlWrite,
        MDGetFtlInfo,
        NULL,
        NULL,      
        NandDeInit,
        MEM_DEV_NAND,
        0,
        0
    },
    #endif
    #if (SPINOR_DRIVER==1)   
    {
        SPINorInit,
        MDSPINorRead,
        MDSPINorWrite,
        MDSPIGetInfo,
        NULL,
        NULL,
        NULL,
        MEM_DEV_SPINOR,
        0,
        0
    },
    #endif
    #if (CONFIG_EMMC_SPEC ==1) 
    {
        EmmcInit,
        MDEmmcRead,
        MDEmmcWrite,
        MDEmmcGetInfo,
        NULL,
        NULL,
        EmmcDeInit,
        MEM_DEV_EMMC,
        0,
        0
    },
    #endif
    #if (CONFIG_SD_SPEC ==1) 
    {
        SDM_Init,
        MDSDMRead,
        MDSDMWrite,
        MDSDMGetInfo,
        MDSDMChkEvent,
        SDMCardProbe,
        NULL,
        MEM_DEV_SD,
        1,
        0
    }
    #endif  
};

#define MEM_DEV_NUM  (sizeof(gMemDevTab)/sizeof(MEM_DEVICE))

_ATTR_SYS_DATA_
MEMDEV_ID UserDiskIDTab[MD_MAX_UDISK]=
{
    UserDisk0ID,
    UserDisk1ID,
    UserDisk2ID
};

_ATTR_SYS_DATA_
MEN_NOTIFIER    MDNotifier[MD_MAX_NOTIFY];

/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/
static uint32 MDNotifyClient(uint16 major, uint32 event, uint32 param);

/*------------------------ Function Implement --------------------------------*/

/*
Name:       MemDevInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_INIT_CODE_
uint32 MemDevInit(void)
{
    uint32 i;
    uint32 ret =OK;
    pMEM_DEVICE pDev = (pMEM_DEVICE)&gMemDevTab[0];

    NandIOMuxRef =0;
    memset(MDNotifier, 0, sizeof(MDNotifier));
    
    for (i=0; i<MEM_DEV_NUM; i++, pDev++)
    {
        if (OK != pDev->init())
        {
			ret = ERROR;        
        }
    }

    return ret;
}


/*
Name:       MDDeInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDDeInitAll(void)
{
    uint32 ret = OK;
    uint32 i;
    pMEM_DEVICE pDev = (pMEM_DEVICE)&gMemDevTab[0];

    for (i=0; i<MEM_DEV_NUM; i++, pDev++)
    {
        if (pDev->DeInit)
        {
            pDev->DeInit();
        }
    }
    
    return ret;

}

/*
Name:       MDGetDev
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
pMEM_DEVICE MDGetDev(MEMDEV_ID DevID)
{
    uint32 i;
    uint16 major  = MD_MAJOR(DevID);
    pMEM_DEVICE pDev = (pMEM_DEVICE)&gMemDevTab[0];

    for(i=0; i<MEM_DEV_NUM; i++, pDev++)
    {
        if (major == pDev->major)
        {
            return pDev;
        }
    }
    return NULL;
}

/*
Name:       MDRead
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDRead(MEMDEV_ID DevID, uint32 SecAdrr, uint32 nSec, void* pBuf)
{
   pMEM_DEVICE pDev;

    pDev = MDGetDev(DevID);
    if (!pDev)
        return ERROR;
    
    return pDev->read(MD_MKLUN(DevID), SecAdrr, nSec, pBuf);//FtlRead
}

/*
Name:       MDWrite
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDWrite(MEMDEV_ID DevID, uint32 SecAdrr, uint32 nSec, void* pBuf)
{
    pMEM_DEVICE pDev;
    
    pDev = MDGetDev(DevID);
    if (!pDev)
    {
        printf("MDWrite: get device error!\n");
        return ERROR;
    }

    return pDev->write(MD_MKLUN(DevID), SecAdrr, nSec, pBuf);
}

/*
Name:       MDGetInfo
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDGetInfo(MEMDEV_ID DevID, pMEMDEV_INFO pDevInfo)
{
    pMEM_DEVICE pDev;

     pDev = MDGetDev(DevID);
     if (!pDev)
         return ERROR;

     return pDev->GetInfo(MD_MKLUN(DevID), pDevInfo);
}


/*
Name:       MDGetInfo
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDGetCapacity(MEMDEV_ID DevID)
{
    MEMDEV_INFO DevInfo;

    if (OK == MDGetInfo(DevID, &DevInfo))
    {
        return DevInfo.capacity;
    }

    return 0;
}

/*
Name:       MDChkValid
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
bool MDChkValid(MEMDEV_ID DevID)
{
    pMEM_DEVICE pDev;
    
    pDev = MDGetDev(DevID);
    if (!pDev)
    {
        return FALSE;
    }

    if (pDev->removable)
    {
        return pDev->CheckEvent(MD_EVENT_VAILID);
    }
    
    return TRUE;
}

/*
Name:       MDCheckChanged
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
bool MDChkPresent(MEMDEV_ID DevID)
{
    pMEM_DEVICE pDev;
    
    pDev = MDGetDev(DevID);
    if (!pDev)
         return FALSE;

    if (pDev->removable)
    {
        return pDev->CheckEvent(MD_EVENT_PRESENT);
    }
    
    return TRUE;
}

/*
Name:       MDProbe
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDProbe(MEMDEV_ID DevID)
{
    pMEM_DEVICE pDev;
    
    pDev = MDGetDev(DevID);
    if (!pDev)
         return ERROR;

    if (pDev->probe)
    {
        return pDev->probe();
    }

    return ERROR;
}

/*
Name:       MDScanDev
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDScanDev(uint8 AutoInit)
{
    uint32 i, ret;
    pMEM_DEVICE pDev = (pMEM_DEVICE)&gMemDevTab[0];

    for (i=0; i<MEM_DEV_NUM; i++, pDev++)
    {
        if (pDev->removable)
        {
            if (pDev->CheckEvent(MD_EVENT_CHANGE))
            {
                bool present;
                
                present = pDev->CheckEvent(MD_EVENT_PRESENT);
              
                MDNotifyClient(pDev->major, MD_EVENT_CHANGE, present);

                if (present && AutoInit && !pDev->CheckEvent(MD_EVENT_VAILID))
                {
                    ret = pDev->probe();
                
                    MDNotifyClient(pDev->major, MD_EVENT_VAILID, (OK ==ret)? 1:0);
                }

            }
        }
    }

    return OK;
}

/*
Name:       MDGetUDiskTab
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDGetUDiskTab(MEMDEV_ID *pUDiskID)
{
    uint32 i, num=0;

    for(i=0; i<MD_MAX_UDISK; i++)
    {
        if (UserDiskIDTab[i])
        {
            num++;
            if (pUDiskID)
            {
                *pUDiskID++ = UserDiskIDTab[i];
            }
        }
    }

    return num;
}

/*
Name:       MDGetUDiskTab
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDSyncIO(void)
{
    #if (NAND_DRIVER==1)
    NandSyncIO();
    #endif
    
    return OK;
}


#if 0
/*
Name:       MDRegDiskID
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/i
_ATTR_SYS_CODE_
uint32 MDRegDiskID(uint32 DiskID)
{
    uint32 i;

    for(i=0; i<MD_MAX_UDISK; i++)
    {
        if (!UserDiskIDTab[i])
        {
            UserDiskIDTab[i] = DiskID;
            return OK;
        }
    }

    return ERROR;
}


/*
Name:       MDUnRegDiskID
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/i
_ATTR_SYS_CODE_
uint32 MDUnRegDiskID(uint32 DiskID)
{
    uint32 i;

    for(i=0; i<MD_MAX_UDISK; i++)
    {
        if (DiskID == UserDiskIDTab[i])
        {
            UserDiskIDTab[i] = 0;
            return OK;
        }
    }

    return ERROR;
}
#endif

/*
Name:       MDRegNotify
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDRegNotify(MEN_NOTIFIER notify)
{
    uint32 i;

    for(i=0; i<MD_MAX_NOTIFY; i++)
    {
        if (!MDNotifier[i])
        {
            MDNotifier[i] = notify;
            return OK;
        }
    }

    return ERROR;
}

/*
Name:       MDUnRegNotify
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
uint32 MDUnRegNotify(MEN_NOTIFIER notify)
{
    uint32 i;

    for(i=0; i<MD_MAX_NOTIFY; i++)
    {
        if (notify == MDNotifier[i])
        {
            MDNotifier[i] = NULL;
            return OK;
        }
    }

    return ERROR;

}

/*
Name:       MDNotifyClient
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_CODE_
static uint32 MDNotifyClient(uint16 major, uint32 event, uint32 param)
{
    uint32 i;


    for(i=0; i<MD_MAX_NOTIFY; i++)
    {
        if (MDNotifier[i])
        {
            MDNotifier[i](major, event, param);
        }
    }
    return OK;
}

/*
Name:       
Desc:       设置数据盘大小，在MemDevInit之后马上调用
Param:      数据盘大小
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_INIT_CODE_
void SetDataDiskSize(int nMB)
{
    SysDataDiskCapacity =  (uint32)(nMB)<<11;
}

/*
Name:       
Desc:       设置第二个用户盘大小，在MemDevInit之后马上调用
Param:      数据盘大小
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SYS_INIT_CODE_
void SetUserDisk2Size(int nMB)
{
    SysUserDisk2Capacity =  (uint32)(nMB)<<11;
}

#if 0
uint32 BroDiskIDTab[MD_MAX_UDISK];
uint32 BroDiskNum;

void SampleNotify(uint16 major, uint32 event, uint32 param)
{
    uint32 i;

    if (MD_EVENT_CHANGE == event)
    {
        bool present = (bool)param;

        for (i=0; i<BroDiskNum; i++)
        {
            if (major==MD_MAJOR(BroDiskIDTab[i]))
            {
                if (present)
                {
                     //MDScanDev(0) manul init
                    if(!MDChkValid(BroDiskIDTab[i]))
                    {
                        if (OK == MDProbe(BroDiskIDTab[i]))
                        {
                            ;//display disk                         
                        }
                        else
                        {
                            ; //no display disk
                        }
                    }
                    else
                    {
                        ;//display disk  
                    }
                }
                else
                {
                    ;//no display disk
                }
            }
        }

    }
    else if (MD_EVENT_VAILID == event) //MDScanDev(1) auto init
    {
        bool valid = (bool)param;

        for (i=0; i<BroDiskNum; i++)
        {
            if (major==MD_MAJOR(BroDiskIDTab[i]))
            {
                if (valid)
                {
                    ;//display disk
                }
                else
                {
                    ;// no display disk                    

                }
                
            }
        }

    }
    else
    {
        ;
    }
    
}

/*
Name:       MDSample1
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
void MDSample1(void)
{

    uint32 i, num;
    uint32 ret;
    
    BroDiskNum = MDGetUDiskTab(BroDiskIDTab);
    ret = MDRegNotify(SampleNotify); 
    if (OK != ret)
    {
        while(1);
    }


    for (i=0; i<BroDiskNum; i++)
    {
        if (MDChkValid(BroDiskIDTab[i]))
        {
            ; //display disk
        }
    } 

    MDUnRegNotify(SampleNotify);

}

#endif
