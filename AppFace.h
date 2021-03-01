/*
  File:	appface.h
  Description:  The Application Interface of AppFace UI Development Kits .
  Version  2.0
 
  Compatible with:
       Microsoft Windows 9x, NT, 2000, XP ,2003
	   Microsoft Visual C++
	   Borland   C++ Builder

  uses no MFC
  uses no VCL

  Copyright (c) 2003-2008, Matinsoft Inc. All rights reserved.
  
Website: http://www.appface.com
E-mail 
  Global: support@appface.com
  China : support_cn@appface.com
  
History:

2008.05.18   2.0  Released
Added callback defines of PaintCustomProc .


2007.01.08   1.8  Released
Added macro SKINSTART and SKINSETEX
Added two defines for SkinWindowSetEx: SKIN_CLASS_PAUSESKIN,SKIN_CLASS_REDOSKIN
Modified CAppFace member functions according the dll interfaces


2006.08.11   1.7  Released
1.Modified CAppFace member function: Start/StartW , SkinWindowEx/SkinWindowEx for better UNICODE support
2.Added the below defines for SkinWindowSet :
#define SKIN_CLASS_PAUSESKIN         2  
#define SKIN_CLASS_REDOSKIN          3  
#define SKIN_CLASS_REMOVESKIN        4  
#define SKIN_CLASS_NOSKINEX          5  
#define SKIN_SET_UNICODE_URF_FONT  661


2006.05.29   1.61 Released
Added some defines for SkinWindowSet: SKIN_CLASS_REMOVESKIN 


2006.04.25   1.6  Released
Added three defines for SkinWindowSetEx : SKIN_SET_UNSKIN,SKIN_CLASS_SCROLLWINBORDER and SKIN_CLASS_SCROLLPANEL

2006.03.09   1.5  Released
Added defines for message WM_FRAME_BUTTON


2005.12.12   1.4  Released
Added defines for SKIN_SET_EFFECT and EFFECT_INOUT_...

2005.10.25   1.31 Released
Added defines for groupbox,radiobutton,checkbutton and shape window

2005.10.18   1.3 Released
Added interface for SkinWindowSetEx and SkinWindowSetExW

2005.06.29   1.21 Released
Added a skin class define : SKIN_CLASS_STATUSBAR_VB6

2005.04.15  1.2 Released
  Added interface for BkCreate,BkDelete,BkDraw

2005.03.06  1.1  Released ,added the below define:
#define GTP_LOAD_FILE_ONLY         4    
#define GTP_LOAD_MEMORY_ONLY       5
#define GTP_LOAD_RESOURCE_ONLY     6

  2005.02.16  1.0  Released
 */

#ifndef __APPFACE_H__
#define __APPFACE_H__

//The message appface used
#define WM_MDICLIENT_NOTIFY    WM_USER + 0x361
#define WM_DATETIME_LBUTTONUP  WM_USER + 0x362
#define WM_SETMENU_NOTIFY      WM_USER + 0x363

//The client frame window will receive this message when user click a frame button
#define WM_FRAME_BUTTON        WM_USER + 0x364

//Send this message to the hyperlink control to modify the URL 
#define WM_HYPERLINK_URL       WM_USER + 0X365

//Send this message to display a transparent GIF on a SKIN_CLASS_STATIC control
#define WM_TRANSPARENT_GIF     WM_USER + 0X365

//Give users a chance to do custom draw for the skinned windows ,
//user can draw anything they want on the AppFace background in the callback function body .
typedef int (CALLBACK *PAINTCUSTOMPROC)(HDC , HWND) ; 


  //For WM_FRAME_BUTTON  WPARAM
#define FRAME_BUTTON_MAX             1
#define FRAME_BUTTON_RESTORE         2
#define FRAME_BUTTON_MIN             3
#define FRAME_BUTTON_CLOSE           4
  
  //For custom frame button
#define FRAME_BUTTON_CUSTOMBASE  0x200           //The custom control button

#define FB_COMMAND_REMOVE            0
#define FB_COMMAND_ENABLE            1
#define FB_COMMAND_DISABLE           2
#define FB_COMMAND_CHECK             3
#define FB_COMMAND_UNCHECK           4


//Load URF,and skin windows automatically
#define GTP_LOAD_FILE                1
#define GTP_LOAD_MEMORY              2
#define GTP_LOAD_RESOURCE            3
//Load the URF into memory only ,but do not skin any windows,
//until users call SkinWindowSet
#define GTP_LOAD_FILE_ONLY           4    
#define GTP_LOAD_MEMORY_ONLY         5
#define GTP_LOAD_RESOURCE_ONLY       6

//The window was created by which development tool
#define WINDOW_TYPE_AUTOFILTER       0  //Auto detected by appface
#define WINDOW_TYPE_SDK              1
#define WINDOW_TYPE_VC               1
#define WINDOW_TYPE_VB6              2
#define WINDOW_TYPE_BCB              3
#define WINDOW_TYPE_DELPHI           3
#define WINDOW_TYPE_NET              4

//Windows skin type,used for SkinWindowSet function
#define SKIN_CLASS_NOSKIN            0   //Do not skin a window ,but allocate the needed resource for it

  //Windows
#define SKIN_CLASS_AUTOFILTER        1   //AppFace skin this window automatically

#define SKIN_CLASS_PAUSESKIN         2   //Pause skin for a special window,but do not free resource
#define SKIN_CLASS_REDOSKIN          3   //Redo skin from pause state
#define SKIN_CLASS_REMOVESKIN        4   //Unskin a window and remove all the allocated skin resource for it at the same time
#define SKIN_CLASS_NOSKINEX          5   //Unskin a window and all the child windows of it


#define SKIN_CLASS_UNKNOWN           6
#define SKIN_CLASS_AUTOFILTEREX      7
#define SKIN_CLASS_SCROLLWIN        10
#define SKIN_CLASS_SCROLLWINBORDER  11
#define SKIN_CLASS_FRAMEWIN        101
#define SKIN_CLASS_FRAMEDIALOG     102
#define SKIN_CLASS_INSIDEDIALOG    103
#define SKIN_CLASS_MDICLIENT       104
#define SKIN_CLASS_SCROLLPANEL     105
#define SKIN_CLASS_FRAMEBACKGROUND 106

  //Controls
#define SKIN_CLASS_COMBOBOX        201
#define SKIN_CLASS_DATETIME        202
#define SKIN_CLASS_HEADER          203
#define SKIN_CLASS_GROUPBOX        204
#define SKIN_CLASS_IMAGEBUTTON     205
#define SKIN_CLASS_MENU            206
#define SKIN_CLASS_PROGRESS        207
#define SKIN_CLASS_PUSHBUTTON      208
#define SKIN_CLASS_SCROLLBAR       209
#define SKIN_CLASS_SLIDER          210
#define SKIN_CLASS_SPIN            211
#define SKIN_CLASS_SPILTER         212
#define SKIN_CLASS_STATUSBAR       213
#define SKIN_CLASS_TAB             214
#define SKIN_CLASS_TEXT            215
#define SKIN_CLASS_TOOLBAR         216
#define SKIN_CLASS_TOOLBARPANEL    217
#define SKIN_CLASS_PANEL           218
#define SKIN_CLASS_PANELEX         219
#define SKIN_CLASS_PANELELIXIR     220
#define SKIN_CLASS_HYPERLINK       221
#define SKIN_CLASS_STATUSBAR_VB6   222
#define SKIN_CLASS_SHAPEWIN        223
#define SKIN_CLASS_CHECKBUTTON     224
#define SKIN_CLASS_RADIOBUTTON     225
#define SKIN_CLASS_READONLYEDIT    226
#define SKIN_CLASS_FRAMEBTN        227
#define SKIN_CLASS_SLIDEREX        228


#define SKIN_CLASS_EDITBOX         229
#define SKIN_CLASS_LISTBOX         230
#define SKIN_CLASS_COMBOBOXEX      231
#define SKIN_CLASS_LISTVIEW        232
#define SKIN_CLASS_TREE            233

#define SKIN_CLASS_TOOLBARREAL     301



 //Only for SkinWindowSetEx
#define SKIN_SET_TRANSPARENT       601
#define SKIN_SET_THEME_COLOR       602
#define SKIN_SET_EFFECT            603
#define SKIN_SET_REDRAW            604

#define SKIN_SET_PAINTCUSTOMPROC   605
#define SKIN_SET_NCPAINTCUSTOMPROC 606

#define SKIN_SET_SPECIAL_TOOL      610

//Only for VB6 scrollbar control
#define SKIN_SET_VB6_SCROLL_INFO   611


#define SKIN_SET_UNSKIN            660
#define SKIN_SET_UNICODE_URF_FONT  661

//Pause all the skin functions of AppFace for all windows in the target process,
//SKIN_CLASS_PAUSESKIN only pause skin functions for one special window .
//These two parameters do not release the allocated resource both.
#define SKIN_SET_PAUSESKIN         662

//Restore skin functions from SKIN_SET_PAUSESKIN state
#define SKIN_SET_REDOSKIN          663

//Indicate skin or not skin VC static control automatically
#define SKIN_SET_VCLABEL           664

//To control the custom frame button
#define SKIN_SET_FRAMEBTN          665

//Create bk object from another URF
#define SKIN_GET_BK                701


//

  //Only for SKIN_SET_EFFECT
#define EFFECT_IN_SPIN               2
#define EFFECT_IN_VORTEX             3
#define EFFECT_IN_SCATTER            4
#define EFFECT_IN_STAR               5
#define EFFECT_IN_RAZZLE             6

#define EFFECT_OUT_SPIN             52
#define EFFECT_OUT_VORTEX           53
#define EFFECT_OUT_SCATTER          54
#define EFFECT_OUT_STAR             55
#define EFFECT_OUT_RAZZLE           56


  //Background type
#define BK_DIALOGPANEL               1
#define BK_MDICLIENT                 2
#define BK_MDICHILD                  3
#define BK_SPLITTER                  4
#define BK_STATUSBAR                 5
#define BK_MENUBARBK                 6
#define BK_MENUBARHOVER              7
#define BK_MENUBARDOWN               8
#define BK_MENU_BK                   9
#define BK_MENU_HOVER               10
#define BK_SCROLL_BK                11
#define BK_SLIDER_CHANNEL_H         12
#define BK_SLIDER_CHANNEL_V         13
#define BK_PROGRESS_H_NORMAL        14
#define BK_PROGRESS_V_NORMAL        15
#define BK_PROGRESS_H_OVER          16
#define BK_PROGRESS_V_OVER          17

#define BK_IMAGE_ARROW            1001
#define BK_CUSTOM                10000

#define ARROW_UP_NORMAL              1
#define ARROW_UP_DOWN                2
#define ARROW_DOWN_NORMAL            3
#define ARROW_DOWN_DOWN              4


#define AFDLL_GET_VERSION       0xc001


#define SKIN_CLASS_NET2005_LOCK   SKIN_CLASS_PAUSESKIN
#define SKIN_CLASS_NET2005_UNLOCK SKIN_CLASS_REDOSKIN
#define SKIN_CLASS_TOOLBAREX      SKIN_CLASS_TOOLBARPANEL


//Application    Interface
#ifdef __cplusplus  
extern "C"
{
#endif

BOOL   __stdcall SkinStart(char* SkinFile  ,
				   int   nDefWinType       ,
				   char* szCheckSum        ,
				   int   nURFType          ,
				   DWORD hInstance         ,
				   char* szResType         
				   )      ;

BOOL   __stdcall SkinStartW(PCWSTR SkinFile,
				   int    nDefWinType      ,
				   PCWSTR szCheckSum       ,
				   int    nURFType         ,
				   DWORD  hInstance        ,
				   PCWSTR szResType        
				   )     ;

void   __stdcall SkinThread() ;

BOOL   __stdcall SkinWindowSet(HWND hWnd,int nSkinType) ;


BOOL   __stdcall SkinWindowSetEx( HWND   hWnd       ,
								  int    nSkinType  ,
								  int    nID        ,
		                          int    nURFType   ,
								  char*  SkinFile   ,								  
								  DWORD  hInst      , 
								  char*  szResType  ) ;

BOOL   __stdcall SkinWindowSetExW(HWND   hWnd       ,
								  int    nSkinType  ,
								  int    nID        ,
		                          int    nURFType   ,
								  PCWSTR SkinFile   ,								  
								  DWORD  hInst      , 
								  PCWSTR szResType  ) ;


void   __stdcall SkinRemove() ;         

HANDLE __stdcall BkCreate(int nBkType) ;

BOOL   __stdcall BkDraw(HANDLE bkHandle    ,
						HDC dcTarget       , 
						LPRECT rcTarget    ,
						int nReserved         //nControl only for bitmap 
						) ;

void   __stdcall BkDelete(HANDLE bkHandle) ;


//Define SKINSTART and SKINSETEX macro for both ANSI and UNICODE
#ifdef _UNICODE
#define SKINSTART SkinStartW
#define SKINSETEX SkinWindowSetExW
#else
#define SKINSTART SkinStart
#define SKINSETEX SkinWindowSetEx
#endif

#ifdef __cplusplus
}
#endif  //__cplusplus

//Help class for loading AppFace  dynamic
#ifdef __cplusplus

class CAppFace
{
public:

typedef BOOL   (__stdcall *APPFACE_START)(char*,int,char*,int,DWORD,char*) ;
typedef BOOL   (__stdcall *APPFACE_STARTW)(PCWSTR,int,PCWSTR,int,DWORD,PCWSTR) ;
typedef void   (__stdcall *APPFACE_REMOVE)();
typedef BOOL   (__stdcall *APPFACE_SKINWINDOW)(HWND,int);
typedef void   (__stdcall *APPFACE_THREAD)();

typedef HANDLE (__stdcall *APPFACE_BKCREATE)(int);
typedef BOOL   (__stdcall *APPFACE_BKDRAW)(HANDLE,HDC,LPRECT,int);
typedef void   (__stdcall *APPFACE_BKDELETE)(HANDLE);


typedef BOOL   (__stdcall *APPFACE_SKINWINDOWEX)(HWND,long,long,int,char*,DWORD,char*) ;
typedef BOOL   (__stdcall *APPFACE_SKINWINDOWEXW)(HWND,long,long,int,PCWSTR,DWORD,PCWSTR) ;


//After Ver 2.4 and the later, appface.dll is for ANSI ,appfaceu.dll is for UNICODE

#ifdef _UNICODE
	CAppFace(LPCTSTR DllPath = L"AppFaceu.dll")
#else
   	CAppFace(LPCTSTR DllPath = "AppFace.dll")
#endif
	{
		pfnStart        = NULL ;
		pfnStartW       = NULL ;
		pfnRemove       = NULL ;
		pfnThread       = NULL ;
		pfnSkinWindow   = NULL ;

		pfnBkCreate     = NULL ;
		pfnBkDraw       = NULL ;
		pfnBkDelete     = NULL ;

		pfnSkinWindowEx = NULL ;
		pfnSkinWindowExW= NULL ;

		m_bValid        = FALSE; 
		m_hDll          = NULL ;
		Init(DllPath) ;
	};
	~CAppFace(){Release(); };

	BOOL Init(LPCTSTR DllPath)
	{
		if(m_hDll && m_bValid)	return TRUE;
		m_hDll = ::LoadLibrary(DllPath);
		if(m_hDll == NULL)		return FALSE ;
		
		pfnStart = (APPFACE_START) GetProcAddress(m_hDll, "SkinStart"); 
		if(!pfnStart)	    return FALSE; 
		pfnStartW = (APPFACE_STARTW) GetProcAddress(m_hDll, "SkinStartW"); 
		if(!pfnStartW)	    return FALSE; 
		pfnRemove= (APPFACE_REMOVE)GetProcAddress(m_hDll, "SkinRemove"); 
		if(!pfnRemove)	    return FALSE; 
		pfnSkinWindow= (APPFACE_SKINWINDOW)GetProcAddress(m_hDll, "SkinWindowSet"); 
		if(!pfnSkinWindow)	return FALSE; 
		pfnThread= (APPFACE_THREAD)GetProcAddress(m_hDll, "SkinThread"); 
		if(!pfnThread)	    return FALSE; 

		pfnBkCreate= (APPFACE_BKCREATE)GetProcAddress(m_hDll, "BkCreate"); 
		if(!pfnBkCreate)	    return FALSE; 
		pfnBkDraw= (APPFACE_BKDRAW)GetProcAddress(m_hDll, "BkDraw"); 
		if(!pfnBkDraw)	    return FALSE; 		
		pfnBkDelete= (APPFACE_BKDELETE)GetProcAddress(m_hDll, "BkDelete"); 
		if(!pfnBkDelete)	    return FALSE; 

		pfnSkinWindowExW= (APPFACE_SKINWINDOWEXW)GetProcAddress(m_hDll, "SkinWindowSetExW"); 
		if(!pfnSkinWindowExW)	    return FALSE; 
		pfnSkinWindowEx= (APPFACE_SKINWINDOWEX)GetProcAddress(m_hDll, "SkinWindowSetEx"); 
		if(!pfnSkinWindowEx)	    return FALSE; 

		m_bValid = TRUE ;
		return TRUE ;
	};

	BOOL IsValid(){return m_bValid;};

	//----------------------------
	//We'll call these functions.
	void SkinThread(){if(pfnThread) pfnThread() ;}; 
	
	void SkinRemove() { if(pfnRemove) pfnRemove() ;};

	BOOL SkinWindowSet(HWND hWnd,int nSkinType){if(pfnSkinWindow) return pfnSkinWindow(hWnd,nSkinType);return FALSE;} ;

	HANDLE BkCreate(int nBkType) {
		if(pfnBkCreate) 
			return pfnBkCreate(nBkType) ; 
		else 
			return NULL ;
	};
	void   BkDelete(HANDLE h) {if(pfnBkDelete) pfnBkDelete(h) ;};
	BOOL   BkDraw(HANDLE bkHandle          ,
						HDC dcTarget       , 
						LPRECT rcTarget    ,
						int nReserved = 0) {
		if(pfnBkDraw) 
			return pfnBkDraw(bkHandle,dcTarget,rcTarget,nReserved) ;
		else
			return FALSE ;
	};

	BOOL SkinStart(LPCTSTR   SkinFile=NULL,  //Source buffer
			   int     nDefWinType = WINDOW_TYPE_AUTOFILTER ,   //The window was created by which development tool       
			   LPCTSTR   szCheckSum= NULL , //Checksum string,not used for the free version
			   int     nLoadType = GTP_LOAD_FILE, //Loading type
			   DWORD   hInst= NULL,   //The HINSTANCE for the loaded module or the size of the memory block
			   LPCTSTR   szResType =NULL  //Resource type ,only used when Type is GTP_LOAD_RESOURCE
			)
	{
#ifdef _UNICODE
		if(pfnStartW)
			return pfnStartW((PCWSTR)SkinFile,nDefWinType,(PCWSTR)szCheckSum,nLoadType,hInst,(PCWSTR)szResType);		
#else
		if(pfnStart)
			return pfnStart((char *)SkinFile,nDefWinType,(char *)szCheckSum,nLoadType,hInst,(char *)szResType);
#endif		
		return FALSE;
	};


	BOOL SkinWindowSetEx( HWND   hWnd  ,  //The target window handle
				  long   nSkinType  ,  //The skin type 
				  long   nID        ,  //The resource ID in the skin
		          int    nURFType  = GTP_LOAD_FILE,  //The type that indicate how to load this URF skin 
				  LPCTSTR  SkinFile  = NULL  ,  //Urf skin path
				  DWORD  hInst     = NULL  ,  //The instance of the module or the size of memory block
				  LPCTSTR  szResType = NULL )  //The resource type if nURFType is GTP_LOAD_RESOURCE
	{
#ifdef _UNICODE
		if(pfnSkinWindowExW)
			return pfnSkinWindowExW(hWnd,nSkinType,nID,nURFType,(PCWSTR)SkinFile,hInst,(PCWSTR)szResType);
#else
		if(pfnSkinWindowEx)
			return pfnSkinWindowEx(hWnd,nSkinType,nID,nURFType,(char *)SkinFile,hInst,(char *)szResType);
#endif		
		return FALSE ;
	};

	//----------------------------
	// To compatible with the previous verisons
	void Remove() { SkinRemove() ;};

	BOOL SkinWindow(HWND hWnd,int nSkinType){return SkinWindowSet(hWnd,nSkinType);} ;


	BOOL Start(LPCTSTR   SkinFile=NULL,  //Source buffer
			   int     nDefWinType = WINDOW_TYPE_AUTOFILTER ,   //The window was created by which development tool       
			   LPCTSTR   szCheckSum= NULL , //Checksum string,not used for the free version
			   int     nLoadType = GTP_LOAD_FILE, //Loading type
			   DWORD   hInst= NULL,   //The HINSTANCE for the loaded module or the size of the memory block
			   LPCTSTR   szResType =NULL  //Resource type ,only used when Type is GTP_LOAD_RESOURCE
			)
	{
		return SkinStart(SkinFile,nDefWinType,szCheckSum,nLoadType,hInst,szResType) ;
	};


	BOOL SkinWindowEx( HWND   hWnd  ,  //The target window handle
				  long   nSkinType  ,  //The skin type 
				  long   nID        ,  //The resource ID in the skin
		          int    nURFType  = GTP_LOAD_FILE,  //The type that indicate how to load this URF skin 
				  LPCTSTR  SkinFile  = NULL  ,  //Urf skin path
				  DWORD  hInst     = NULL  ,  //The instance of the module or the size of memory block
				  LPCTSTR  szResType = NULL )  //The resource type if nURFType is GTP_LOAD_RESOURCE
	{
		return SkinWindowSetEx(hWnd,nSkinType,nID,nURFType,SkinFile,hInst,szResType);
	};
	//----------------------------

private:
	BOOL m_bValid  ;  
	HMODULE m_hDll ;

	APPFACE_START          pfnStart         ;
	APPFACE_STARTW         pfnStartW        ;
	APPFACE_REMOVE         pfnRemove        ;
	APPFACE_SKINWINDOW     pfnSkinWindow    ;
	APPFACE_THREAD         pfnThread        ;

	APPFACE_BKCREATE       pfnBkCreate      ;
	APPFACE_BKDRAW         pfnBkDraw        ;
	APPFACE_BKDELETE       pfnBkDelete      ;

	APPFACE_SKINWINDOWEX   pfnSkinWindowEx  ;
	APPFACE_SKINWINDOWEXW  pfnSkinWindowExW ;

protected:
	void Release()
	{
		try{  
			if(m_hDll)
			{
				FreeLibrary(m_hDll)    ;
				m_hDll = NULL          ;
				m_bValid = false       ;
				pfnStart = NULL        ;
				pfnStartW= NULL        ;
				pfnRemove= NULL        ;
				pfnSkinWindow = NULL   ;
				pfnBkCreate   = NULL   ;
				pfnBkDraw     = NULL   ;
				pfnBkDelete   = NULL   ;
				pfnSkinWindowEx  = NULL;
				pfnSkinWindowExW = NULL;
			}
		}catch(...){}
	};
};
#endif //__cplusplus

#endif  //__APPFACE_H__
