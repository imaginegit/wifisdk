/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiMusicBrowWin.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef  _WIFIMUSICBROWWIN_H
#define  _WIFIMUSICBROWWIN_H

#undef  EXT
#ifdef  _IN_WIFIMUSICBROWWIN_
#define EXT
#else
#define EXT extern
#endif

//section define
//music menu permanent code.
#define _ATTR_WiFiMUSICBrow_CODE_         __attribute__((section("WiFiMusicBrowCode")))
#define _ATTR_WiFiMUSICBrow_DATA_         __attribute__((section("WiFiMusicBrowData")))
#define _ATTR_WiFiMUSICBrow_BSS_          __attribute__((section("WiFiMusicBrowBss"),zero_init))


EXT void   WiFiMusicBrowWinInit(void *pArg);
EXT void   WiFiMusicBrowWinDeInit(void);

EXT UINT32 WiFiMusicBrowWinService(void);
EXT UINT32 WiFiMusicBrowWinKeyProc(void);
EXT void   WiFiMusicBrowWinPaint(void);




/*
********************************************************************************
*
*                         Description:  window sturcture definition
*
********************************************************************************
*/
#ifdef _IN_WIFIMUSICBROWWIN_
_ATTR_WiFiMUSICBrow_DATA_ EXT WIN WiFiMusicBrowWin = 
{
    
    NULL,
    NULL,
    
    WiFiMusicBrowWinService,                    //window service handle function.
    WiFiMusicBrowWinKeyProc,                    //window key service handle function.
    WiFiMusicBrowWinPaint,                      //window display service handle function.
    
    WiFiMusicBrowWinInit,                       //window initial handle function.
    WiFiMusicBrowWinDeInit                      //window auti-initial handle function.
    
};
#else 
_ATTR_WiFiMUSICBrow_DATA_ EXT WIN WiFiMusicBrowWin;
#endif

/*
********************************************************************************
*
*                         End of WiFiMusicBrowWin.h
*
********************************************************************************
*/
#endif
