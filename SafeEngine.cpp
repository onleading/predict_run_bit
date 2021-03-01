#include "stdafx.h"
#include "SafeEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// 全局安全引擎
CSafeEngine g_SafeEngine;

//////////////////////////////////////////////////////////////////////////
// 安全引擎实现
CSafeEngine::CSafeEngine(BOOL bEnableDebug)
	: m_bEnableDebug(bEnableDebug)
{
	// 得到当前操作系统的版本
	if(GetVersion()>=0x80000000)
			m_bIsWin98=TRUE;
	else	m_bIsWin98=FALSE;

	// 得到当前实例句柄
	m_hInstance=AfxGetStaticModuleState()->m_hCurrentInstanceHandle;

	// 准备列表(用于查找窗口实现)
	m_mapInsts.RemoveAll();

	// 钩子变量
	m_hHookCallWndProc=NULL;
	m_hHookCallWndRetProc=NULL;
	m_hHookDebugProc=NULL;
	m_hHookKeyboardProc=NULL;
	m_hHookKeyboardProcLL=NULL;
	m_dwTimerID=0;
}

CSafeEngine::~CSafeEngine()
{
}

VOID CSafeEngine::SetModuleInstance(HINSTANCE hInstance)
{	m_hInstance=hInstance;
}

BOOL CSafeEngine::AddControl(HWND hControl,CSafeControl* pControl)
{
	// 检查参数是否正常
	if(pControl==NULL) return FALSE;
	if(hControl==NULL) return FALSE;
	LPVOID pVoid=NULL;
	if(m_mapInsts.Lookup(hControl,pVoid)) return FALSE;

	// 如果是添加的第一个控件,则安装钩子,并设定定时器,在定时器内检查系统安全
	if(m_mapInsts.GetCount()==0)
	{
		// 安装挂钩
		m_hHookCallWndProc=SetWindowsHookEx(WH_CALLWNDPROC,__CallWndProc,m_hInstance,GetCurrentThreadId());
		m_hHookCallWndRetProc=SetWindowsHookEx(WH_CALLWNDPROCRET,__CallWndRetProc,m_hInstance,GetCurrentThreadId());
		m_hHookKeyboardProc=SetWindowsHookEx(WH_KEYBOARD,__KeyboardProc,m_hInstance,GetCurrentThreadId());
		m_hHookKeyboardProcLL=SetWindowsHookEx(WH_KEYBOARD_LL,__KeyboardProcLL,GetModuleHandle(NULL),NULL);
		if(!m_bEnableDebug) m_hHookDebugProc=SetWindowsHookEx(WH_DEBUG,__DebugProc,m_hInstance,GetCurrentThreadId());

		// 设置定时器,定时更新挂钩
		m_dwTimerID=time(NULL);
		::SetTimer(NULL,m_dwTimerID,1000,__SecurityPatrol);
	}

	// 安装当前实例
	m_mapInsts.SetAt(hControl,pControl);

	return TRUE;
}

BOOL CSafeEngine::RemoveControl(HWND hControl)
{
	// 从列表中删除控件
	LPVOID pVoid=NULL;
	if(!m_mapInsts.Lookup(hControl,pVoid)) return FALSE;
	m_mapInsts.RemoveKey(hControl);

	// 如果是最后一个删除的对象,这里释放钩子
	if(m_mapInsts.GetCount()==0)
	{
		// 关闭定时检查系统安全的定时器
		::KillTimer(NULL,m_dwTimerID);
		m_dwTimerID = 0;

		// 关闭键盘钩子
		if(m_hHookCallWndProc!=NULL) UnhookWindowsHookEx(m_hHookCallWndProc);
		if(m_hHookCallWndRetProc!=NULL) UnhookWindowsHookEx(m_hHookCallWndRetProc);
		if(m_hHookDebugProc!=NULL) UnhookWindowsHookEx(m_hHookDebugProc);
		if(m_hHookKeyboardProc!=NULL) UnhookWindowsHookEx(m_hHookKeyboardProc);
		if(m_hHookKeyboardProcLL!=NULL) UnhookWindowsHookEx(m_hHookKeyboardProcLL);
		m_hHookCallWndProc=NULL;
		m_hHookCallWndRetProc=NULL;
		m_hHookDebugProc=NULL;
		m_hHookKeyboardProc=NULL;
		m_hHookKeyboardProcLL=NULL;
	}

	return TRUE;
}

VOID CSafeEngine::DoSecurityPatrol()
{
	if(m_dwTimerID==0) return;
	// 卸载当前安装的钩子
	if(m_hHookCallWndProc!=NULL) UnhookWindowsHookEx(m_hHookCallWndProc);
	if(m_hHookCallWndRetProc!=NULL) UnhookWindowsHookEx(m_hHookCallWndRetProc);
	if(m_hHookDebugProc!=NULL) UnhookWindowsHookEx(m_hHookDebugProc);
	if(m_hHookKeyboardProc!=NULL) UnhookWindowsHookEx(m_hHookKeyboardProc);
	if(m_hHookKeyboardProcLL!=NULL) UnhookWindowsHookEx(m_hHookKeyboardProcLL);
	m_hHookCallWndProc=NULL;
	m_hHookCallWndRetProc=NULL;
	m_hHookDebugProc=NULL;
	m_hHookKeyboardProc=NULL;
	m_hHookKeyboardProcLL=NULL;
	// 重新安装钩子
	m_hHookCallWndProc=SetWindowsHookEx(WH_CALLWNDPROC,__CallWndProc,m_hInstance,GetCurrentThreadId());
	m_hHookCallWndRetProc=SetWindowsHookEx(WH_CALLWNDPROCRET,__CallWndRetProc,m_hInstance,GetCurrentThreadId());
	m_hHookKeyboardProc=SetWindowsHookEx(WH_KEYBOARD,__KeyboardProc,m_hInstance,GetCurrentThreadId());
	m_hHookKeyboardProcLL=SetWindowsHookEx(WH_KEYBOARD_LL,__KeyboardProcLL,GetModuleHandle(NULL),NULL);
	if(!m_bEnableDebug) m_hHookDebugProc=SetWindowsHookEx(WH_DEBUG,__DebugProc,NULL,GetCurrentThreadId());
}

LRESULT CALLBACK CSafeEngine::__CallWndProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookCallWndProc,nCode,wParam,lParam);
	
	// 如果非当前实例的消息,允许消息被传递给其他挂钩
	BOOL bInThread=(wParam!=NULL);
	PCWPSTRUCT pCWPStruct=(PCWPSTRUCT)lParam;
	LPVOID pVoid=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(pCWPStruct->hwnd,pVoid))
		return CallNextHookEx(g_SafeEngine.m_hHookCallWndProc,nCode,wParam,lParam);

	// 否则只有非键盘的消息能得到
	if(bInThread&&pCWPStruct->message>=WM_KEYFIRST&&pCWPStruct->message<=WM_KEYLAST) return 0;
	return CallNextHookEx(g_SafeEngine.m_hHookCallWndProc,nCode,wParam,lParam);
}

LRESULT CALLBACK CSafeEngine::__CallWndRetProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookCallWndRetProc,nCode,wParam,lParam);
	
	// 如果非当前实例的消息,允许消息被传递给其他挂钩
	BOOL bInThread=(wParam!=NULL);
	PCWPRETSTRUCT pCWPRetStruct=(PCWPRETSTRUCT)lParam;
	LPVOID pVoid=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(pCWPRetStruct->hwnd,pVoid))
		return CallNextHookEx(g_SafeEngine.m_hHookCallWndRetProc,nCode,wParam,lParam);
	
	// 否则只有非键盘的消息能得到
	if(bInThread&&pCWPRetStruct->message>=WM_KEYFIRST&&pCWPRetStruct->message>=WM_KEYLAST) return 0;
	return CallNextHookEx(g_SafeEngine.m_hHookCallWndRetProc,nCode,wParam,lParam);
}

LRESULT CALLBACK CSafeEngine::__KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	// 规定调用nCode小于0的时候必须调用下一个钩子
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);
	// 如果存在高级挂钩,则忽略本处的处理
	if(g_SafeEngine.m_hHookKeyboardProcLL!=NULL)
		return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);

	// 判断当前进程是否是在前端,如果当前进程不位于最前端则不进行处理
	HWND hForeground=::GetForegroundWindow();
	DWORD dwProcessId=0;
	GetWindowThreadProcessId(hForeground,&dwProcessId);
	if(dwProcessId!=GetCurrentProcessId()) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);

	// 得到焦点窗口,如果焦点不存在,窗口不是当前进程,窗口不是当前类,则直接返回
	HWND hWndFocus=::GetFocus();
	if(hWndFocus==NULL) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);
	CSafeControl* pSafeControl=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(hWndFocus,(LPVOID&)pSafeControl)) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);

	// 否则是一个窗口,通过调用其输入函数,这里返回失败,不在分发消息
	if(nCode==HC_ACTION)
	{
		// 转换得到按键消息
		KBDEVENT KbdEvent;
		memset(&KbdEvent,0,sizeof(KbdEvent));
		KbdProcParamToKbdEvent(wParam,lParam,&KbdEvent);
		// 对系统按键进行特殊处理
		if(	KbdEvent.m_uVK==VK_NUMLOCK||KbdEvent.m_uVK==VK_CAPITAL||KbdEvent.m_uVK==VK_SCROLL||
			KbdEvent.m_uVK==VK_ESCAPE||KbdEvent.m_uVK==VK_RETURN||KbdEvent.m_uVK==VK_TAB||
			(KbdEvent.m_uVK>=VK_F1&&KbdEvent.m_uVK<=VK_F12))
		{	WPARAM wParam=0;
			LPARAM lParam=0;
			KbdEventToKeyDownMsgParam(&KbdEvent,wParam,lParam);
			if(KbdEvent.m_fTransitionState==0)
					PostMessage(hWndFocus,WM_KEYDOWN,wParam,lParam);
			else	PostMessage(hWndFocus,WM_KEYUP,wParam,lParam);
		}
		pSafeControl->HandleKbdEventSafe(&KbdEvent);
	}
	return 1;
}

LRESULT CALLBACK CSafeEngine::__KeyboardProcLL(int nCode,WPARAM wParam,LPARAM lParam)
{
	// 规定调用nCode小于0的时候必须调用下一个钩子
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);

	// 判断当前进程是否是在前端
	HWND hForeground=::GetForegroundWindow();
	DWORD dwProcessId=0;
	GetWindowThreadProcessId(hForeground,&dwProcessId);
	if(dwProcessId!=GetCurrentProcessId()) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);

	// 得到焦点窗口,如果焦点不存在,窗口不是当前进程,窗口不是当前类,则直接返回
	HWND hWndFocus=::GetFocus();
	if(hWndFocus==NULL) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);
	CSafeControl* pSafeControl=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(hWndFocus,(LPVOID&)pSafeControl)) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);

	// 如果是状态键,正常传递
	UINT naCtlKeys[]={VK_MENU,VK_LMENU,VK_RMENU,
					VK_CONTROL,VK_LCONTROL,VK_RCONTROL,
					VK_NUMLOCK,VK_CAPITAL,VK_SCROLL};
	LPKBDLLHOOKSTRUCT lpKBDLLHookStruct=(LPKBDLLHOOKSTRUCT)lParam;
	for(UINT nCtlKey=0; nCtlKey<sizeof(naCtlKeys)/sizeof(naCtlKeys[0]); nCtlKey++)
	{	if(lpKBDLLHookStruct->vkCode==naCtlKeys[nCtlKey])
			return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);
	}
	// 否则是一个窗口,通过调用其输入函数,这里返回失败,不在分发消息
	if(nCode==HC_ACTION)
	{
		// 转换得到按键消息
		KBDEVENT KbdEvent;
		memset(&KbdEvent,0,sizeof(KbdEvent)); 
		KbdProcLLParamToKbdEvent(wParam,lParam,&KbdEvent);
		// 对系统按键进行特殊处理
		if(	KbdEvent.m_uVK==VK_NUMLOCK||KbdEvent.m_uVK==VK_CAPITAL||KbdEvent.m_uVK==VK_SCROLL||
			KbdEvent.m_uVK==VK_ESCAPE||KbdEvent.m_uVK==VK_RETURN||KbdEvent.m_uVK==VK_TAB||
			(KbdEvent.m_uVK>=VK_F1&&KbdEvent.m_uVK<=VK_F12))
		{	WPARAM wParam=0;
			LPARAM lParam=0;
			KbdEventToKeyDownMsgParam(&KbdEvent,wParam,lParam);
			if(KbdEvent.m_fTransitionState==0)
					PostMessage(hWndFocus,WM_KEYDOWN,wParam,lParam);
			else	PostMessage(hWndFocus,WM_KEYUP,wParam,lParam);
		}
		pSafeControl->HandleKbdEventSafe(&KbdEvent);
	}
	return 1;
}

LRESULT CALLBACK CSafeEngine::__DebugProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(wParam!=WH_KEYBOARD&&wParam!=WH_KEYBOARD_LL) return CallNextHookEx(g_SafeEngine.m_hHookDebugProc,nCode,wParam,lParam);
	PDEBUGHOOKINFO pDebugHookInfo=(PDEBUGHOOKINFO)lParam;
	if(nCode<0||pDebugHookInfo->idThread!=GetCurrentThreadId())
	{	pDebugHookInfo->wParam=0;
		return CallNextHookEx(g_SafeEngine.m_hHookDebugProc,nCode,wParam,lParam);
	}
	if(g_SafeEngine.m_hHookDebugProc==NULL&&g_SafeEngine.m_hHookKeyboardProcLL==NULL)
		return CallNextHookEx(g_SafeEngine.m_hHookDebugProc,nCode,wParam,lParam);
	return 1;
}

VOID CSafeEngine::__SecurityPatrol(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(idEvent);
	UNREFERENCED_PARAMETER(dwTime);
	g_SafeEngine.DoSecurityPatrol();
}

// KBD钩子到KBDEVENT转换
VOID CSafeEngine::KbdProcParamToKbdEvent(WPARAM wParam,LPARAM lParam,LPKBDEVENT lpKbdEvent)
{
	KBDSTATE KbdState;
	memcpy(&KbdState,(LPBYTE)&lParam,sizeof(KbdState));
	lpKbdEvent->m_uVK=wParam;
	lpKbdEvent->m_uRepeatCount=KbdState.m_uRepeatCount;
	lpKbdEvent->m_cScanCode=KbdState.m_cScanCode;
	lpKbdEvent->m_fExtend=KbdState.m_fExtend;
	lpKbdEvent->m_fAlt=KbdState.m_fAlt;
	lpKbdEvent->m_fCtrl=GetAsyncKeyState(VK_CONTROL)!=0;
	lpKbdEvent->m_fShift=(GetAsyncKeyState (VK_SHIFT) >> ((sizeof(SHORT) * 8) - 1))!=0;	
	lpKbdEvent->m_fWin=GetAsyncKeyState(VK_LWIN|VK_RWIN)!=0;
	lpKbdEvent->m_fNumLock=GetKeyState(VK_NUMLOCK)&0X0001;
	lpKbdEvent->m_fCaps=GetKeyState(VK_CAPITAL)&0X0001;
	lpKbdEvent->m_fScrollLock=GetKeyState(VK_SCROLL)&0X0001;
	lpKbdEvent->m_fPreviousState=KbdState.m_fPreviousState;
	lpKbdEvent->m_fTransitionState=KbdState.m_fTransitionState;
	lpKbdEvent->m_fHardware=1;
}

// KBDLL钩子到KBDEVENT转换
VOID CSafeEngine::KbdProcLLParamToKbdEvent(WPARAM wParam,LPARAM lParam,LPKBDEVENT lpKbdEvent)
{
	LPKBDLLHOOKSTRUCT lpKBDLLHookStruct=(LPKBDLLHOOKSTRUCT)lParam;
	BYTE cFKBDLLFlag=(BYTE)lpKBDLLHookStruct->flags;
	KBDLLSTATE KbdLLState;
	memcpy(&KbdLLState,&cFKBDLLFlag,sizeof(cFKBDLLFlag));
	lpKbdEvent->m_uVK=lpKBDLLHookStruct->vkCode;
	lpKbdEvent->m_uRepeatCount=1;
	lpKbdEvent->m_cScanCode=(BYTE)lpKBDLLHookStruct->scanCode;
	lpKbdEvent->m_fExtend=KbdLLState.m_fExtend;
	lpKbdEvent->m_fAlt=KbdLLState.m_fAlt;
	lpKbdEvent->m_fCtrl=GetAsyncKeyState(VK_CONTROL)!=0;
	lpKbdEvent->m_fShift=(GetAsyncKeyState (VK_SHIFT) >> ((sizeof(SHORT) * 8) - 1))!=0;
	lpKbdEvent->m_fWin=GetAsyncKeyState(VK_LWIN|VK_RWIN);
	lpKbdEvent->m_fNumLock=GetKeyState(VK_NUMLOCK)&0X0001;
	lpKbdEvent->m_fCaps=GetKeyState(VK_CAPITAL)&0X0001;
	lpKbdEvent->m_fScrollLock=GetKeyState(VK_SCROLL)&0X0001;
	lpKbdEvent->m_fPreviousState=(wParam==WM_KEYDOWN||wParam==WM_SYSKEYDOWN)?1:0;
	lpKbdEvent->m_fTransitionState=KbdLLState.m_fTransitionState;
	lpKbdEvent->m_fHardware=1;
}

// WM_KEYDOWN消息到KBDEVENT转换
VOID CSafeEngine::KeyDownMsgParamToKbdEvent(WPARAM wParam,LPARAM lParam,LPKBDEVENT lpKbdEvent)
{
	lpKbdEvent->m_uVK=wParam;
	LPKBDSTATE lpKbdState=(LPKBDSTATE)(&lParam);
	lpKbdEvent->m_uRepeatCount=lpKbdState->m_uRepeatCount;
	lpKbdEvent->m_cScanCode=lpKbdState->m_cScanCode;
	lpKbdEvent->m_fExtend=lpKbdState->m_fExtend;
	lpKbdEvent->m_fAlt=lpKbdState->m_fAlt;
	lpKbdEvent->m_fCtrl=GetAsyncKeyState(VK_CONTROL)!=0;
	lpKbdEvent->m_fShift=(GetAsyncKeyState (VK_SHIFT) >> ((sizeof(SHORT) * 8) - 1))!=0;
	lpKbdEvent->m_fWin=GetAsyncKeyState(VK_LWIN|VK_RWIN)!=0;
	lpKbdEvent->m_fNumLock=GetKeyState(VK_NUMLOCK)&0X0001;
	lpKbdEvent->m_fCaps=GetKeyState(VK_CAPITAL)&0X0001;
	lpKbdEvent->m_fScrollLock=GetKeyState(VK_SCROLL)&0X0001;
	lpKbdEvent->m_fPreviousState=lpKbdState->m_fPreviousState;
	lpKbdEvent->m_fTransitionState=lpKbdState->m_fTransitionState;
	lpKbdEvent->m_fHardware=0;
}

// KBDEVENT到WM_KEYDOWN消息参数转换
VOID CSafeEngine::KbdEventToKeyDownMsgParam(LPKBDEVENT lpKbdEvent,WPARAM& wParam,LPARAM& lParam)
{
	wParam=lpKbdEvent->m_uVK;
	LPKBDSTATE lpKbdState=(LPKBDSTATE)(&lParam);
	lpKbdState->m_uRepeatCount=lpKbdEvent->m_uRepeatCount;
	lpKbdState->m_cScanCode=lpKbdEvent->m_cScanCode;
	lpKbdState->m_fExtend=lpKbdEvent->m_fExtend;
	lpKbdState->m_cReserved=0;
	lpKbdState->m_fAlt=lpKbdEvent->m_fAlt;
	lpKbdState->m_fPreviousState=lpKbdEvent->m_fPreviousState;
	lpKbdState->m_fTransitionState=lpKbdEvent->m_fTransitionState;
}
