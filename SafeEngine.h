#ifndef __SAFE_ENGINE__H__
#define __SAFE_ENGINE__H__
#if _MSC_VER > 1000
#pragma once
#endif

// 安全控件声明
class CSafeControl
{
public:
	virtual VOID OnSystemNotSafe() PURE;
	virtual BOOL HandleKbdEventSafe(LPVOID pUnknownKbdEvent) PURE;
};

// 安全引擎声明
class CSafeEngine
{
public:
	CSafeEngine(BOOL bEnableDebug=FALSE);
	virtual~CSafeEngine();
	VOID SetModuleInstance(HINSTANCE hInstance);
	BOOL IsWin98() { return m_bIsWin98; }
	BOOL AddControl(HWND hControl,CSafeControl* pControl);
	BOOL RemoveControl(HWND hControl);
	VOID DoSecurityPatrol();
protected:
	static LRESULT CALLBACK __CallWndProc(int nCode,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK __CallWndRetProc(int nCode,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK __KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK __KeyboardProcLL(int nCode,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK __DebugProc(int nCode,WPARAM wParam,LPARAM lParam);
	static VOID WINAPI __SecurityPatrol(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime);
protected:
	BOOL m_bEnableDebug;
	BOOL m_bIsWin98;
	HINSTANCE m_hInstance;
	CMapPtrToPtr m_mapInsts;
	HHOOK m_hHookCallWndProc;
	HHOOK m_hHookCallWndRetProc;
	HHOOK m_hHookDebugProc;
	HHOOK m_hHookKeyboardProcLL;
	HHOOK m_hHookKeyboardProc;
	DWORD m_dwTimerID;
public:
	// 键盘钩子状态信息
	typedef struct tagKBDSTATE
	{	WORD m_uRepeatCount;		// 1
		BYTE m_cScanCode;			// 0~9 a~z A~Z
		BYTE m_fExtend:1;			// 0
		BYTE m_cReserved:4;			// 0
		BYTE m_fAlt:1;				// 0
		BYTE m_fPreviousState:1;	// 按下:0; 弹起:1
		BYTE m_fTransitionState:1;  // 按下:1; 弹起:0
	} KBDSTATE,*LPKBDSTATE; 
	// 底层键盘钩子键盘状态结构
	#ifndef WH_KEYBOARD_LL
	#define WH_KEYBOARD_LL (13)
		typedef struct tagKBDLLHOOKSTRUCT {
			DWORD   vkCode;
			DWORD   scanCode;
			DWORD   flags;
			DWORD   time;
			DWORD   dwExtraInfo;
		} KBDLLHOOKSTRUCT, FAR *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;
	#endif
	// 底层键盘钩子键盘状态结构
	typedef struct tagKBDLLSTATE
	{	BYTE m_fExtend:1;
		BYTE m_cReserved1:3;
		BYTE m_cReject:1;
		BYTE m_fAlt:1;
		BYTE m_cReserved2:1;
		BYTE m_fTransitionState:1;
	} KBDLLSTATE,*LPKBDLLSTATE;
	// 键盘抽象的按键结构(将可被转换为WM_KEYDOWN消息)
	typedef struct tagKBDEVENT
	{	UINT m_uVK;
		WORD m_uRepeatCount;
		BYTE m_cScanCode;
		BYTE m_fExtend:1;
		BYTE m_fAlt:1;
		BYTE m_fCtrl:1;
		BYTE m_fShift:1;
		BYTE m_fWin:1;
		BYTE m_fNumLock:1;
		BYTE m_fCaps:1;
		BYTE m_fScrollLock:1;
		BYTE m_fPreviousState:1;
		BYTE m_fTransitionState:1;
		BYTE m_fHardware:1;
	} KBDEVENT,*LPKBDEVENT;
	static VOID KbdProcParamToKbdEvent(WPARAM wParam,LPARAM lParam,LPKBDEVENT lpKbdEvent);
	static VOID KbdProcLLParamToKbdEvent(WPARAM wParam,LPARAM lParam,LPKBDEVENT lpKbdEvent);
	static VOID KeyDownMsgParamToKbdEvent(WPARAM wParam,LPARAM lParam,LPKBDEVENT lpKbdEvent);
	static VOID KbdEventToKeyDownMsgParam(LPKBDEVENT lpKbdEvent,WPARAM& wParam,LPARAM& lParam);
};
extern CSafeEngine g_SafeEngine;

#endif
