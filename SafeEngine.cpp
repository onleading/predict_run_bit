#include "stdafx.h"
#include "SafeEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// ȫ�ְ�ȫ����
CSafeEngine g_SafeEngine;

//////////////////////////////////////////////////////////////////////////
// ��ȫ����ʵ��
CSafeEngine::CSafeEngine(BOOL bEnableDebug)
	: m_bEnableDebug(bEnableDebug)
{
	// �õ���ǰ����ϵͳ�İ汾
	if(GetVersion()>=0x80000000)
			m_bIsWin98=TRUE;
	else	m_bIsWin98=FALSE;

	// �õ���ǰʵ�����
	m_hInstance=AfxGetStaticModuleState()->m_hCurrentInstanceHandle;

	// ׼���б�(���ڲ��Ҵ���ʵ��)
	m_mapInsts.RemoveAll();

	// ���ӱ���
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
	// �������Ƿ�����
	if(pControl==NULL) return FALSE;
	if(hControl==NULL) return FALSE;
	LPVOID pVoid=NULL;
	if(m_mapInsts.Lookup(hControl,pVoid)) return FALSE;

	// �������ӵĵ�һ���ؼ�,��װ����,���趨��ʱ��,�ڶ�ʱ���ڼ��ϵͳ��ȫ
	if(m_mapInsts.GetCount()==0)
	{
		// ��װ�ҹ�
		m_hHookCallWndProc=SetWindowsHookEx(WH_CALLWNDPROC,__CallWndProc,m_hInstance,GetCurrentThreadId());
		m_hHookCallWndRetProc=SetWindowsHookEx(WH_CALLWNDPROCRET,__CallWndRetProc,m_hInstance,GetCurrentThreadId());
		m_hHookKeyboardProc=SetWindowsHookEx(WH_KEYBOARD,__KeyboardProc,m_hInstance,GetCurrentThreadId());
		m_hHookKeyboardProcLL=SetWindowsHookEx(WH_KEYBOARD_LL,__KeyboardProcLL,GetModuleHandle(NULL),NULL);
		if(!m_bEnableDebug) m_hHookDebugProc=SetWindowsHookEx(WH_DEBUG,__DebugProc,m_hInstance,GetCurrentThreadId());

		// ���ö�ʱ��,��ʱ���¹ҹ�
		m_dwTimerID=time(NULL);
		::SetTimer(NULL,m_dwTimerID,1000,__SecurityPatrol);
	}

	// ��װ��ǰʵ��
	m_mapInsts.SetAt(hControl,pControl);

	return TRUE;
}

BOOL CSafeEngine::RemoveControl(HWND hControl)
{
	// ���б���ɾ���ؼ�
	LPVOID pVoid=NULL;
	if(!m_mapInsts.Lookup(hControl,pVoid)) return FALSE;
	m_mapInsts.RemoveKey(hControl);

	// ��������һ��ɾ���Ķ���,�����ͷŹ���
	if(m_mapInsts.GetCount()==0)
	{
		// �رն�ʱ���ϵͳ��ȫ�Ķ�ʱ��
		::KillTimer(NULL,m_dwTimerID);
		m_dwTimerID = 0;

		// �رռ��̹���
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
	// ж�ص�ǰ��װ�Ĺ���
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
	// ���°�װ����
	m_hHookCallWndProc=SetWindowsHookEx(WH_CALLWNDPROC,__CallWndProc,m_hInstance,GetCurrentThreadId());
	m_hHookCallWndRetProc=SetWindowsHookEx(WH_CALLWNDPROCRET,__CallWndRetProc,m_hInstance,GetCurrentThreadId());
	m_hHookKeyboardProc=SetWindowsHookEx(WH_KEYBOARD,__KeyboardProc,m_hInstance,GetCurrentThreadId());
	m_hHookKeyboardProcLL=SetWindowsHookEx(WH_KEYBOARD_LL,__KeyboardProcLL,GetModuleHandle(NULL),NULL);
	if(!m_bEnableDebug) m_hHookDebugProc=SetWindowsHookEx(WH_DEBUG,__DebugProc,NULL,GetCurrentThreadId());
}

LRESULT CALLBACK CSafeEngine::__CallWndProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookCallWndProc,nCode,wParam,lParam);
	
	// ����ǵ�ǰʵ������Ϣ,������Ϣ�����ݸ������ҹ�
	BOOL bInThread=(wParam!=NULL);
	PCWPSTRUCT pCWPStruct=(PCWPSTRUCT)lParam;
	LPVOID pVoid=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(pCWPStruct->hwnd,pVoid))
		return CallNextHookEx(g_SafeEngine.m_hHookCallWndProc,nCode,wParam,lParam);

	// ����ֻ�зǼ��̵���Ϣ�ܵõ�
	if(bInThread&&pCWPStruct->message>=WM_KEYFIRST&&pCWPStruct->message<=WM_KEYLAST) return 0;
	return CallNextHookEx(g_SafeEngine.m_hHookCallWndProc,nCode,wParam,lParam);
}

LRESULT CALLBACK CSafeEngine::__CallWndRetProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookCallWndRetProc,nCode,wParam,lParam);
	
	// ����ǵ�ǰʵ������Ϣ,������Ϣ�����ݸ������ҹ�
	BOOL bInThread=(wParam!=NULL);
	PCWPRETSTRUCT pCWPRetStruct=(PCWPRETSTRUCT)lParam;
	LPVOID pVoid=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(pCWPRetStruct->hwnd,pVoid))
		return CallNextHookEx(g_SafeEngine.m_hHookCallWndRetProc,nCode,wParam,lParam);
	
	// ����ֻ�зǼ��̵���Ϣ�ܵõ�
	if(bInThread&&pCWPRetStruct->message>=WM_KEYFIRST&&pCWPRetStruct->message>=WM_KEYLAST) return 0;
	return CallNextHookEx(g_SafeEngine.m_hHookCallWndRetProc,nCode,wParam,lParam);
}

LRESULT CALLBACK CSafeEngine::__KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	// �涨����nCodeС��0��ʱ����������һ������
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);
	// ������ڸ߼��ҹ�,����Ա����Ĵ���
	if(g_SafeEngine.m_hHookKeyboardProcLL!=NULL)
		return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);

	// �жϵ�ǰ�����Ƿ�����ǰ��,�����ǰ���̲�λ����ǰ���򲻽��д���
	HWND hForeground=::GetForegroundWindow();
	DWORD dwProcessId=0;
	GetWindowThreadProcessId(hForeground,&dwProcessId);
	if(dwProcessId!=GetCurrentProcessId()) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);

	// �õ����㴰��,������㲻����,���ڲ��ǵ�ǰ����,���ڲ��ǵ�ǰ��,��ֱ�ӷ���
	HWND hWndFocus=::GetFocus();
	if(hWndFocus==NULL) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);
	CSafeControl* pSafeControl=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(hWndFocus,(LPVOID&)pSafeControl)) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProc,nCode,wParam,lParam);

	// ������һ������,ͨ�����������뺯��,���ﷵ��ʧ��,���ڷַ���Ϣ
	if(nCode==HC_ACTION)
	{
		// ת���õ�������Ϣ
		KBDEVENT KbdEvent;
		memset(&KbdEvent,0,sizeof(KbdEvent));
		KbdProcParamToKbdEvent(wParam,lParam,&KbdEvent);
		// ��ϵͳ�����������⴦��
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
	// �涨����nCodeС��0��ʱ����������һ������
	if(nCode<0) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);

	// �жϵ�ǰ�����Ƿ�����ǰ��
	HWND hForeground=::GetForegroundWindow();
	DWORD dwProcessId=0;
	GetWindowThreadProcessId(hForeground,&dwProcessId);
	if(dwProcessId!=GetCurrentProcessId()) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);

	// �õ����㴰��,������㲻����,���ڲ��ǵ�ǰ����,���ڲ��ǵ�ǰ��,��ֱ�ӷ���
	HWND hWndFocus=::GetFocus();
	if(hWndFocus==NULL) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);
	CSafeControl* pSafeControl=NULL;
	if(!g_SafeEngine.m_mapInsts.Lookup(hWndFocus,(LPVOID&)pSafeControl)) return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);

	// �����״̬��,��������
	UINT naCtlKeys[]={VK_MENU,VK_LMENU,VK_RMENU,
					VK_CONTROL,VK_LCONTROL,VK_RCONTROL,
					VK_NUMLOCK,VK_CAPITAL,VK_SCROLL};
	LPKBDLLHOOKSTRUCT lpKBDLLHookStruct=(LPKBDLLHOOKSTRUCT)lParam;
	for(UINT nCtlKey=0; nCtlKey<sizeof(naCtlKeys)/sizeof(naCtlKeys[0]); nCtlKey++)
	{	if(lpKBDLLHookStruct->vkCode==naCtlKeys[nCtlKey])
			return CallNextHookEx(g_SafeEngine.m_hHookKeyboardProcLL,nCode,wParam,lParam);
	}
	// ������һ������,ͨ�����������뺯��,���ﷵ��ʧ��,���ڷַ���Ϣ
	if(nCode==HC_ACTION)
	{
		// ת���õ�������Ϣ
		KBDEVENT KbdEvent;
		memset(&KbdEvent,0,sizeof(KbdEvent)); 
		KbdProcLLParamToKbdEvent(wParam,lParam,&KbdEvent);
		// ��ϵͳ�����������⴦��
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

// KBD���ӵ�KBDEVENTת��
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

// KBDLL���ӵ�KBDEVENTת��
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

// WM_KEYDOWN��Ϣ��KBDEVENTת��
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

// KBDEVENT��WM_KEYDOWN��Ϣ����ת��
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
