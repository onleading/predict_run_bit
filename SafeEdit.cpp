#include "stdafx.h"
#include "SafeEdit.h"
#include <imm.h>
#include <malloc.h>
#include <windows.h>
#include "SafeEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef ARRAYSIZE
	#define ARRAYSIZE(a)		(sizeof(a)/sizeof(a[0]))
	#define SIZEOF(a)			(sizeof(a))
#endif

IMPLEMENT_DYNAMIC(CSafeEdit,CWnd)

CSafeEdit::CSafeEdit(BOOL bSafeMode)
	: m_bSafeMode(bSafeMode)
	, m_dwExtendedStyle(ES_EX_PASSWORD|ES_EX_HASMENU|ES_EX_HASCOPYMENU|ES_EX_HASCUTMENU|ES_EX_HASPASTEMENU|ES_EX_WANTALLCHARS|ES_EX_GUIFONT|ES_EX_HASBORDER)
{
	// 注册当前类
	RegisterWindowClass(NULL,_T("SafeEdit"));
	// 基本选项
	m_strHideMask="*";
	// 随机生成密钥
	GenerateWeakKey(m_acWeakKey,sizeof(m_acWeakKey));
	// 得到系统字体
	m_fontEdit.CreateStockObject(DEFAULT_GUI_FONT);
	// 数据变量
	m_nSelStartChar=0;
	m_nSelEndChar=0;
	m_nFirstVisible=0;
	m_bCaretValid=FALSE;
	m_bCaretVisible=FALSE;
	m_bTrackMouse=FALSE;
	m_nTrackFrom=0;
	m_nShiftFrom=0;
	m_ptTrackFrom=CPoint(0,0);
	m_nLength=0;
	m_nLengthLimit=ARRAYSIZE(m_szText)-1;
	memset(m_szText,0,sizeof(m_szText));

	m_bUseFeedback=FALSE;
	m_hFeedback=NULL;
	m_nFeedbackMsg=0;
}

CSafeEdit::~CSafeEdit()
{
}

BOOL CSafeEdit::Create(DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID)
{
	if(!CWnd::Create(_T("SafeEdit"),"",dwStyle,rect,pParentWnd,nID)) return FALSE;
	return TRUE;
}

VOID CSafeEdit::SetExtendedStyle(DWORD dwExtendedStyle)
{	m_dwExtendedStyle=dwExtendedStyle;
	if(IsWindow(m_hWnd)) RedrawWindow();
}

BOOL CSafeEdit::ModifyExtendedStyle(DWORD dwRemove,DWORD dwAdd,BOOL bRedraw)
{	
	DWORD dwNewStyle=(m_dwExtendedStyle & (~dwRemove))|dwAdd;
	if(m_dwExtendedStyle==dwNewStyle) 
		return FALSE;
	m_dwExtendedStyle=dwNewStyle;
	if(bRedraw&&IsWindow(m_hWnd)) RedrawWindow();
	return TRUE;
}

VOID CSafeEdit::SetHideMask(LPCSTR pszHideMask)
{	if(pszHideMask!=NULL&&strlen(pszHideMask)>0)
	{	m_strHideMask=pszHideMask;
		if(IsWindow(m_hWnd)) RedrawWindow();
	}
}

LONG CSafeEdit::GetFirstVisible() const
{	return m_nFirstVisible;
}

VOID CSafeEdit::GetSel(LONG& nStartChar,LONG& nEndChar) const
{	nStartChar=min(m_nSelStartChar,m_nSelEndChar);
	nEndChar=max(m_nSelStartChar,m_nSelEndChar);
}

VOID CSafeEdit::SetSel(LONG nStartChar,LONG nEndChar,BOOL bNoScroll)
{
	// 清除全部选定(忽略结束字符)
	if(nStartChar==-1)
	{	m_nSelEndChar=m_nSelStartChar;
	}
	// 否则选中指定的内容(包括指定内容后的内容)
	else
	{	m_nSelStartChar=max(0,min(GetLengthSafe(),nStartChar));
		if(nEndChar==-1)
				m_nSelEndChar=GetLengthSafe();
		else	m_nSelEndChar=max(0,min(GetLengthSafe(),nEndChar));
	}
	// 矫正选中区域
	m_nSelStartChar=CorrectCharIndex(m_nSelStartChar);
	m_nSelEndChar=CorrectCharIndex(m_nSelEndChar);

	// 判断是否需要滚动
	if(!bNoScroll)
	{
		// 得到视图位置
		CRect rcView;
		GetClientRect(rcView);
		if(IsHasBorder()) rcView.DeflateRect(CX_MARGINS,CY_MARGINS);
		LONG nCaret=GetCaret();
		CPoint ptCaret=PosFromCharIndex(nCaret);
		// 向左滚动
		if(ptCaret.x<rcView.left)
		{	m_nFirstVisible=nCaret;
			m_nFirstVisible=max(0,min(m_nLength,m_nFirstVisible));
		}
		// 向右滚动
		else if(ptCaret.x>rcView.right)
		{	LONG nRight=CharIndexFromPos(rcView.right);
			m_nFirstVisible+=(nCaret-nRight);
			m_nFirstVisible=max(0,min(m_nLength,m_nFirstVisible));
		}
	}
	// 判断窗口有效,完成界面相关处理
	if(IsWindow(m_hWnd))
	{
		// 重新绘制界面
		RedrawWindow();

		// 移动光标
		SetCaretPos(PosFromCharIndex(GetCaret()));
		HideCaretWhenVisible();
		ShowCaretWhenVisible();
	}
}

LONG CSafeEdit::GetCaret() const
{	return m_nSelEndChar;
}


VOID CSafeEdit::SetLimitText(LONG nMax)
{	CString strText;
	GetTextSafe(strText);
	m_nLengthLimit=max(0,min(nMax,ARRAYSIZE(m_szText)-1));
	SetTextSafe(strText);
	CleanString(&strText);
}

LONG CSafeEdit::GetLimitText() const
{
	return m_nLengthLimit;
}

LONG CSafeEdit::GetLengthSafe() const
{
	return m_nLength;
}

VOID CSafeEdit::GetTextSafe(CString& rString) const
{	rString.Empty();
	if(m_nLength>0)
	{	CHAR szText[MAX_EDITLIMITS]={0};
		memcpy(szText,m_szText,m_nLength);
		XorByWeakKey(szText,m_nLength,m_acWeakKey,sizeof(m_acWeakKey));
		szText[sizeof(szText)-1]=0;
		rString=szText;
		memset(szText,0,sizeof(szText));
	}
}

VOID CSafeEdit::GetTextSafe(char *pStr,int nNum) const
{	
	CString tmpString;
	GetTextSafe(tmpString);
	strncpy(pStr,tmpString,nNum);
	pStr[nNum]=0;
}

VOID CSafeEdit::SetTextSafe(LPCSTR pszString)
{
	// 进行数据预处理,剔除不满足条件的字符
	CString strText=pszString;
	for(LONG i=strText.GetLength()-1; i>=0; i--)
		if(!IsCharWanted(strText.GetAt(i))) strText.Delete(i);
	if(GetExtendedStyle()&ES_EX_UPPERCASE) strText.MakeUpper();
	if(GetExtendedStyle()&ES_EX_LOWERCASE) strText.MakeLower();

	// 保存数据
	m_nLength=max(0,min(strText.GetLength(),m_nLengthLimit));
	m_nLength=min(m_nLength,ARRAYSIZE(m_szText)-1);
	if(m_nLength>0)
	{	memcpy(m_szText,strText,m_nLength);
		XorByWeakKey(m_szText,m_nLength,m_acWeakKey,sizeof(m_acWeakKey));
	}

	// 矫正选中的位置
	m_nSelStartChar=max(0,min(m_nLength,m_nSelStartChar));
	m_nSelEndChar=max(0,min(m_nLength,m_nSelEndChar));
	m_nFirstVisible=max(0,min(m_nLength,m_nFirstVisible));

	// 如果窗口可见,重新绘制
	if(IsWindow(m_hWnd)) RedrawWindow();
	CleanString(&strText);

	// 通知父窗口
	if(m_bUseFeedback)
	{	if(m_hFeedback!=NULL&&m_nFeedbackMsg!=0)
			::SendMessage(m_hFeedback,m_nFeedbackMsg,0,0);
	}
	else
	{	CWnd* pFeedback=GetOwner();
		if(pFeedback==NULL) pFeedback=GetParent();
		HWND hFeedback=pFeedback->GetSafeHwnd();
		::SendMessage(hFeedback,WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),EN_CHANGE),(LPARAM)GetSafeHwnd());
	}
}


VOID CSafeEdit::CleanString(CString* pszString)
{
	CString strTmp('A',MAX_EDITLIMITS);
	*pszString=strTmp;
	(*pszString).Empty();
}

VOID CSafeEdit::PostCharKey(TCHAR cChar)
{
	CSafeEngine::KBDEVENT KbdEvent={0};
	DWORD dwStyle=GetStyle();
	if (dwStyle&(ES_READONLY|WS_DISABLED)==1) return;

	TCHAR szShiftChar[]=")!@#$%^&*(";
	UINT aMapperVK1[]={0XC0,0XBD,0XBB,0XDB,0XDD,0XDC,0XBA,0XDE,0XBC,0XBE,0XBF};
	TCHAR aMapperChar1[]={"`-=[]\\;',./~_+{}|:\"<>?"};

	KbdEvent.m_uRepeatCount=1;

	if (cChar>='A'&&cChar<='Z')
	{	KbdEvent.m_uVK=(UINT)cChar;
		KbdEvent.m_cScanCode=(BYTE)KbdEvent.m_uVK;
		KbdEvent.m_fShift=1;
		KbdEvent.m_fExtend=0;
		KbdEvent.m_fAlt=0;
		KbdEvent.m_fCtrl=0;
		KbdEvent.m_fWin=0;
		KbdEvent.m_fNumLock=0;
		KbdEvent.m_fCaps=0;
		KbdEvent.m_fScrollLock=0;
		HandleKbdEventSafe(&KbdEvent);
		return;
	}

	for (UINT nIndex=0;nIndex<sizeof(szShiftChar);nIndex++)
	{	if (cChar==szShiftChar[nIndex])
		{	KbdEvent.m_uVK=(UINT)(nIndex+'0');
			KbdEvent.m_cScanCode=(BYTE)KbdEvent.m_uVK;
			KbdEvent.m_fShift=1;
			KbdEvent.m_fExtend=0;
			KbdEvent.m_fAlt=0;
			KbdEvent.m_fCtrl=0;
			KbdEvent.m_fWin=0;
			KbdEvent.m_fNumLock=0;
			KbdEvent.m_fCaps=0;
			KbdEvent.m_fScrollLock=0;
			HandleKbdEventSafe(&KbdEvent);
			return;
		}
	}
	
	for (nIndex=0;nIndex<sizeof(aMapperChar1);nIndex++)
	{	if (cChar==aMapperChar1[nIndex])
		{	if (nIndex>=sizeof(aMapperVK1)/sizeof(aMapperVK1[0]))
			{	nIndex-=sizeof(aMapperVK1)/sizeof(aMapperVK1[0]);
				KbdEvent.m_fShift=1;
			}
			KbdEvent.m_uVK=aMapperVK1[nIndex];
			KbdEvent.m_cScanCode=(BYTE)KbdEvent.m_uVK;
			KbdEvent.m_fExtend=0;
			KbdEvent.m_fAlt=0;
			KbdEvent.m_fCtrl=0;
			KbdEvent.m_fWin=0;
			KbdEvent.m_fNumLock=0;
			KbdEvent.m_fCaps=0;
			KbdEvent.m_fScrollLock=0;
			HandleKbdEventSafe(&KbdEvent);
			return;
		}
	}

	if (cChar>='a'&&cChar<='z')
	{
		KbdEvent.m_uVK=(UINT)(cChar-'a'+'A');
	}
	else
		KbdEvent.m_uVK=(UINT)cChar;
	KbdEvent.m_cScanCode=(BYTE)KbdEvent.m_uVK;
	KbdEvent.m_fShift=0;
	KbdEvent.m_fExtend=0;
	KbdEvent.m_fAlt=0;
	KbdEvent.m_fCtrl=0;
	KbdEvent.m_fWin=0;
	KbdEvent.m_fNumLock=0;
	KbdEvent.m_fCaps=0;
	KbdEvent.m_fScrollLock=0;
	KbdEvent.m_fHardware=0;
	HandleKbdEventSafe(&KbdEvent);
	return;

}

VOID CSafeEdit::SetFeedBack(BOOL bUseFeedback,HWND hFeedback,UINT nFeedbackMsg)
{	m_bUseFeedback=bUseFeedback;
	m_hFeedback=hFeedback;
	m_nFeedbackMsg=nFeedbackMsg;
}

BOOL CSafeEdit::IsCharWanted(TCHAR cChar)
{
	DWORD dwWantTypes=(GetExtendedStyle()&ES_EX_WANTTYPES);
	if((dwWantTypes&ES_EX_WANTTYPES)==ES_EX_WANTALLCHARS) return TRUE;
	switch(dwWantTypes)
	{
	case ES_EX_WANTNUM:
		if(isdigit(cChar)!=0) return TRUE;
		break;
	case ES_EX_WANTCHAR:
		if(cChar>='A'&&cChar<='Z') return TRUE;
		if(cChar>='a'&&cChar<='z') return TRUE;
		break;
	case ES_EX_WANTALNUM:
		if(isdigit(cChar)!=0) return TRUE;
		if(cChar>='A'&&cChar<='Z') return TRUE;
		if(cChar>='a'&&cChar<='z') return TRUE;
		break;
	}
	return FALSE;
}

VOID CSafeEdit::OnSystemNotSafe()
{
}

BOOL CSafeEdit::HandleKbdEventSafe(LPVOID pUnknownKbdEvent)
{
	CSafeEngine::LPKBDEVENT pKbdEvent=(CSafeEngine::LPKBDEVENT)pUnknownKbdEvent;

	// 进行键盘映射(映射小键盘的按键)
	UINT uVK=pKbdEvent->m_uVK;
	UINT aPadKeyUK[]={VK_NUMPAD0,VK_NUMPAD1,VK_NUMPAD2,VK_NUMPAD3,VK_NUMPAD4,
		VK_NUMPAD5,VK_NUMPAD6,VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD9,VK_DECIMAL,VK_ADD,VK_SUBTRACT,
		VK_MULTIPLY,VK_DIVIDE};
	UINT aPadeKeyAUKMap1[]={'0','1','2','3','4','5','6','7','8','9',VK_DECIMAL,VK_ADD,VK_SUBTRACT,VK_MULTIPLY,VK_DIVIDE};
	UINT aPadeKeyBUKMap1[]={VK_INSERT,VK_END,VK_DOWN,VK_NEXT,VK_LEFT,0,VK_RIGHT,VK_HOME,VK_UP,VK_PRIOR,
		VK_DELETE,VK_ADD,VK_SUBTRACT,VK_MULTIPLY,VK_DIVIDE};
	for(UINT uPadKey=0; uPadKey<sizeof(aPadKeyUK)/sizeof(aPadKeyUK[0]); uPadKey++)
	{	if(aPadKeyUK[uPadKey]==uVK)
		{	uVK=(pKbdEvent->m_fNumLock)?(aPadeKeyAUKMap1[uPadKey]):(aPadeKeyBUKMap1[uPadKey]);
			break;
		}
	}
	if(uVK==0) return TRUE;

	// 对状态按键消息进行基本判断,忽略部分组合键
	if(pKbdEvent->m_fWin) return TRUE;
	if(pKbdEvent->m_fAlt) return TRUE;
	if(pKbdEvent->m_fCtrl&&pKbdEvent->m_fAlt) return TRUE;

	// 组合键处理
	if(pKbdEvent->m_fCtrl)
	{
		if(pKbdEvent->m_fTransitionState==0)
		{	switch(uVK)
			{
			case 'C':	OnEditCopy(); break;
			case 'X':	OnEditCut(); break;
			case 'V':	OnEditPaste(); break;
			case 'A':	OnEditSelectAll(); break;
			case VK_SHIFT:
			case VK_LSHIFT:
			case VK_RSHIFT:
				if(pKbdEvent->m_fTransitionState==0)
				{	m_nShiftFrom=GetCaret();
				}
			break;
			case VK_LEFT:
				if(pKbdEvent->m_fTransitionState==0)
				{	if(pKbdEvent->m_fShift)
						SetSel(m_nShiftFrom,0);
					else	SetSel(0,0);
				}
				break;
			case VK_RIGHT:
				if(pKbdEvent->m_fTransitionState==0)
				{	if(pKbdEvent->m_fShift)
						SetSel(m_nShiftFrom,GetLengthSafe());
					else	SetSel(GetLengthSafe(),GetLengthSafe());
				}
			default:	break;
			}
		}
		return FALSE;
	}
	// 其他输入
	else
	{	switch(uVK)
		{
		case VK_INSERT:
			break;
		case VK_SHIFT:
		case VK_LSHIFT:
		case VK_RSHIFT:
			if(pKbdEvent->m_fTransitionState==0)
			{	m_nShiftFrom=GetCaret();
			}
			break;
		case VK_LEFT:
			if(pKbdEvent->m_fTransitionState==0)
			{	LONG nCaret=GetCaret();
				LONG nNextCaret=nCaret;
				for(;;)
				{	if(nNextCaret==0) break;
					nNextCaret=CorrectCharIndex(nNextCaret-1);
					if(nNextCaret!=nCaret) break;
				}
				if(pKbdEvent->m_fShift)
						SetSel(m_nShiftFrom,nNextCaret);
				else	SetSel(nNextCaret,nNextCaret);
			}
			break;
		case VK_RIGHT:
			if(pKbdEvent->m_fTransitionState==0)
			{	LONG nCaret=GetCaret();
				LONG nNextCaret=nCaret;
				for(;;)
				{	if(nNextCaret>=GetLengthSafe()) break;
					nNextCaret=CorrectCharIndex(nNextCaret+1);
					if(nNextCaret!=nCaret) break;
				}
				if(pKbdEvent->m_fShift)
						SetSel(m_nShiftFrom,nNextCaret);
				else	SetSel(nNextCaret,nNextCaret);
			}
			break;
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
			break;
		case VK_END:
			if(pKbdEvent->m_fTransitionState==0)
			{	if(pKbdEvent->m_fShift)
						SetSel(m_nShiftFrom,GetLengthSafe());
				else	SetSel(GetLengthSafe(),GetLengthSafe());
			}
			break;
		case VK_HOME:
			if(pKbdEvent->m_fTransitionState==0)
			{	if(pKbdEvent->m_fShift)
						SetSel(m_nShiftFrom,0);
				else	SetSel(0,0);
			}
			break;
		case VK_DELETE:
			if(pKbdEvent->m_fTransitionState==0)
			{	if(!IsReadOnly())
				{	CString strText,strTextNew;
					GetTextSafe(strText);
					LONG nStartChar=0,nEndChar=0;
					GetSel(nStartChar,nEndChar);
					if(nStartChar!=nEndChar)
					{	strTextNew=strText.Left(nStartChar);
						strTextNew+=strText.Right(GetLengthSafe()-nEndChar);
					}
					else if(nStartChar<GetLengthSafe())
					{	strTextNew=strText.Left(nStartChar);
						strTextNew+=strText.Right(GetLengthSafe()-nStartChar-1);
					}
					else
					{	strTextNew=strText;
					}
					SetTextSafe(strTextNew);
					SetSel(nStartChar,nStartChar);
					strText=strTextNew="";
				}
			}
			break;
		case VK_BACK:
			if(pKbdEvent->m_fTransitionState==0)
			{	if(!IsReadOnly())
				{	CString strText,strTextNew;
					GetTextSafe(strText);
					LONG nStartChar=0,nEndChar=0;
					GetSel(nStartChar,nEndChar);
					if(nStartChar!=nEndChar)
					{	strTextNew=strText.Left(nStartChar);
						strTextNew+=strText.Right(GetLengthSafe()-nEndChar);
					}
					else if(nStartChar>0)
					{	strTextNew=strText.Left(nStartChar-1);
						strTextNew+=strText.Right(GetLengthSafe()-nStartChar);
						nStartChar--;
					}
					else
					{	strTextNew=strText;
					}
					SetTextSafe(strTextNew);
					SetSel(nStartChar,nStartChar);
					strText=strTextNew="";
				}
				else
				{	MessageBeep(MB_ICONASTERISK);
				}
			}
			break;
		default:
			if(pKbdEvent->m_fTransitionState==0)
			{	// 用户输入
				TCHAR cChar=0;
				switch(uVK)
				{
				case VK_SPACE:		cChar=' ';	break;
				case VK_DECIMAL:	cChar='.';	break;
				case VK_ADD:		cChar='+';	break;
				case VK_SUBTRACT:	cChar='-';	break;
				case VK_MULTIPLY:	cChar='*';	break;
				case VK_DIVIDE:		cChar='/';	break;
				default:
					// 字符输入
					if(uVK>='A'&&uVK<='Z')
					{	BOOL bUpper=pKbdEvent->m_fCaps;
						if(pKbdEvent->m_fShift) bUpper=!bUpper;
						cChar=(TCHAR)(bUpper?(uVK):(_tolower(uVK)));
					}
					// 数字输入
					else if(uVK>='0'&&uVK<='9')
					{	TCHAR szShiftChar[]=")!@#$%^&*(";
						cChar=(TCHAR)(!pKbdEvent->m_fShift?(uVK):(szShiftChar[uVK-'0']));
					}
					// 查找大键盘上的具备SHIFT功能按键
					else
					{	UINT aMapperVK1[]={0XC0,0XBD,0XBB,0XDB,0XDD,0XDC,0XBA,0XDE,0XBC,0XBE,0XBF};
						TCHAR aMapperChar1[]={"`-=[]\\;',./~_+{}|:\"<>?"};
						for(UINT nMapper1=0; nMapper1<sizeof(aMapperVK1)/sizeof(aMapperVK1[0]); nMapper1++)
						{	if(aMapperVK1[nMapper1]==uVK)
							{	UINT nIndex=nMapper1;
								if(pKbdEvent->m_fShift) nIndex+=sizeof(aMapperVK1)/sizeof(aMapperVK1[0]);
								cChar=aMapperChar1[nIndex];
								break;
							}
						}
					}
				}
				if(cChar!=0)
				{
					// 进行输入处理
					if(!IsReadOnly()&&IsCharWanted(cChar))
					{	// 得到文本
						CString strText;
						GetTextSafe(strText);
						// 得到删除选中串后的字符串
						LONG nStartChar=0,nEndChar=0;
						GetSel(nStartChar,nEndChar);
						strText=strText.Left(nStartChar)+strText.Right(GetLengthSafe()-nEndChar);
						if(strText.GetLength()<GetLimitText())
						{	// 在游标位置插入(注:删除后选中位置会发生变更)
							strText.Insert(nStartChar,cChar);
							// 调整选中区域和游标
							SetTextSafe(strText);
							SetSel(nStartChar+1,nStartChar+1);
						}
						CleanString(&strText);
					}
					else
					{	MessageBeep(MB_ICONASTERISK);
					}
				}
			}
			break;
		}
	}
	return FALSE;
}

BEGIN_MESSAGE_MAP(CSafeEdit, CWnd)
	//{{AFX_MSG_MAP(CSafeEdit)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_CLEAR_ALL,OnEditClearAll)
	ON_COMMAND(ID_EDIT_SELECT_ALL,OnEditSelectAll)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

VOID CSafeEdit::PreSubclassWindow() 
{
	// 设置默认风格,进行默认处理
	ModifyStyle(WS_OVERLAPPED|WS_MAXIMIZEBOX,WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_LEFT|ES_AUTOHSCROLL);
	ModifyStyleEx(NULL,WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_NOPARENTNOTIFY);
	CWnd::PreSubclassWindow();

	// 令当前控件接收键盘消息
	if(IsSafeMode()) g_SafeEngine.AddControl(GetSafeHwnd(),this);
}

LRESULT CSafeEdit::DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam) 
{	
	LRESULT lResult=0;
	switch(message)
	{
	// 支持长度获取
	case EM_GETLIMITTEXT: lResult=GetLimitText(); break;
	case EM_SETLIMITTEXT: SetLimitText(wParam); break;
	case WM_GETTEXTLENGTH: lResult=GetLengthSafe(); break;
	// 支持选区
		
	case EM_GETSEL:
		{	LONG nStartChar=0,nEndChar=0;
			GetSel(nStartChar,nEndChar);
			if(wParam!=NULL) (*((LPDWORD)wParam))=nStartChar;
			if(lParam!=NULL) (*((LPDWORD)lParam))=nEndChar;
			lResult=MAKEWPARAM(nStartChar,nEndChar);
		}
		break;
	case EM_SETSEL:
		{	LONG nStartChar=(LONG)wParam;
			LONG nEndChar=(LONG)lParam;
			SetSel(nStartChar,nEndChar);
			lResult=TRUE;
		}
		break;
	// 支持设置文本,不支持获取文本
	case WM_GETTEXT: lResult=0; break;
	case WM_SETTEXT: SetTextSafe((LPCSTR)lParam); lResult=0; break;
	// 不支持UNDO操作
	case EM_CANUNDO: lResult=0; break;
	case EM_UNDO: lResult=TRUE; break;
	// 支持复制,剪切,粘贴以及删除
	case WM_COPY:	OnEditCopy(); lResult=0; break;
	case WM_CUT:	OnEditCut(); lResult=0; break;
	case WM_PASTE:	OnEditPaste(); lResult=0; break;
	case WM_CLEAR:	OnEditClear(); lResult=0; break;
	// 键盘消息处理(仅非安全模式支持)
	case WM_KEYDOWN:
		// TAB/RETURN/ESCAPE的处理特殊处理
		if(	wParam==VK_NUMLOCK||wParam==VK_CAPITAL||wParam==VK_SCROLL||
			wParam==VK_ESCAPE||wParam==VK_RETURN||wParam==VK_TAB||
			(wParam>=VK_F1&&wParam<=VK_F12))
		{	lResult=CWnd::DefWindowProc(message,wParam,lParam);
		}
		// 否则: 1.如果采用安全模式,按键在挂钩中已经调用并处理(SafeEngine调用)
		//       2.如果非安全模式,本处处理(PreTranslateMessage中处理)
		break;
	case WM_PAINT:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		break;
	default:
		lResult=CWnd::DefWindowProc(message,wParam,lParam);
		break;
	}
	return lResult;
}

VOID CSafeEdit::OnDestroy() 
{
	if(IsSafeMode()) g_SafeEngine.RemoveControl(m_hWnd);
	CWnd::OnDestroy();
}

VOID CSafeEdit::OnContextMenu(CWnd* pWnd,CPoint point)
{
	// 不支持菜单则直接返回
	if(!IsHasMenu()) return;

	// 计算弹出位置
	if(point.x==-1||point.y==-1)
	{	point=GetCaretPos();
		ClientToScreen(&point);
	}

	// 各种标志
	BOOL bClipboardAvail=::IsClipboardFormatAvailable(CF_TEXT);
	UNREFERENCED_PARAMETER(bClipboardAvail);
	LONG nStartChar=0,nEndChar=0;
	GetSel(nStartChar,nEndChar);

	// 创建菜单
	CMenu PopMenu;
	PopMenu.CreatePopupMenu();
	// 复制菜单
	if(IsHasCopyMenu()&&!IsPassword()&&nStartChar!=nEndChar)
		PopMenu.AppendMenu(MF_STRING,ID_EDIT_COPY,_T("复制(&C)\tCTRL+C"));
	// 剪切菜单
	if(IsHasCutMenu()&&!IsPassword()&&!IsReadOnly()&&nStartChar!=nEndChar)
		PopMenu.AppendMenu(MF_STRING,ID_EDIT_CUT,_T("剪切(&T)\tCTRL+T"));
	// 粘贴菜单
	if(IsHasPasteMenu()&&!IsReadOnly())
		PopMenu.AppendMenu(MF_STRING,ID_EDIT_PASTE,_T("粘贴(&V)\tCTRL+V"));
	// 删除菜单
	if(!IsReadOnly()&&nStartChar!=nEndChar)
		PopMenu.AppendMenu(MF_STRING,ID_EDIT_CLEAR,_T("删除(&D)"));
	// 分割符
	if(PopMenu.GetMenuItemCount()>0)
		PopMenu.AppendMenu(MF_SEPARATOR,0,"");
	// 全删
	if(!IsReadOnly()&&GetLengthSafe()>0)
		PopMenu.AppendMenu(MF_STRING,ID_EDIT_CLEAR_ALL,_T("全删(&E)"));
	// 全选菜单
	PopMenu.AppendMenu(MF_STRING,ID_EDIT_SELECT_ALL,_T("全选(&A)\tCTRL+A"));
	// 弹出菜单进行处理
	PopMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,point.x,point.y,pWnd,NULL);
}

VOID CSafeEdit::OnEditCopy()
{
	if(IsHasCopyMenu()&&!IsPassword())
	{	CString strText;
		GetTextSafe(strText);
		LONG nStartChar=0,nEndChar=0;
		GetSel(nStartChar,nEndChar);
		if(nStartChar!=nEndChar)
		{	strText=strText.Mid(nStartChar,nEndChar-nStartChar);
			SetStringToClipboard(strText);
		}
		CleanString(&strText);
	}
}

VOID CSafeEdit::OnEditCut()
{
	if(IsHasCutMenu()&&!IsPassword()&&!IsReadOnly())
	{
		CString strText,strTextNew;
		GetTextSafe(strText);
		LONG nStartChar=0,nEndChar=0;
		GetSel(nStartChar,nEndChar);
		if(nStartChar!=nEndChar)
		{	strTextNew=strText.Left(nStartChar);
			strTextNew+=strText.Right(GetLengthSafe()-nEndChar);
			strText=strText.Mid(nStartChar,nEndChar-nStartChar);
			SetStringToClipboard(strText);
			SetTextSafe(strTextNew);
			SetSel(nStartChar,nStartChar);
		}
		strText=strTextNew="";
	}
}

VOID CSafeEdit::OnEditPaste()
{
	if(IsHasPasteMenu()&&!IsReadOnly())
	{
		// 得到文本
		CString strText;
		GetTextSafe(strText);
		// 得到删除选中串后的字符串
		LONG nStartChar=0,nEndChar=0;
		GetSel(nStartChar,nEndChar);
		strText=strText.Left(nStartChar)+strText.Right(GetLengthSafe()-nEndChar);
		// 在游标位置插入(注:删除后选中位置会发生变更)
		LONG nCanInsert=GetLimitText()-strText.GetLength();
		if(nCanInsert>0)
		{	CHAR szText[MAX_EDITLIMITS]={0};
			GetStringFromClipboard(szText,sizeof(szText));
			nCanInsert=min(nCanInsert,(LONG)strlen(szText));
			szText[nCanInsert]=0;
			strText.Insert(nStartChar,szText);
			memset(szText,0,sizeof(szText));
		}
		// 调整选中区域和游标
		SetTextSafe(strText);
		SetSel(nStartChar+nCanInsert,nStartChar+nCanInsert);
		CleanString(&strText);
	}
}

VOID CSafeEdit::OnEditClear()
{
	if(!IsReadOnly())
	{	CString strText,strTextNew;
		GetTextSafe(strText);
		LONG nStartChar=0,nEndChar=0;
		GetSel(nStartChar,nEndChar);
		if(nStartChar!=nEndChar)
		{	strTextNew=strText.Left(nStartChar);
			strTextNew+=strText.Right(GetLengthSafe()-nEndChar);
			SetTextSafe(strTextNew);
			SetSel(nStartChar,nStartChar);
		}
		strText=strTextNew="";
	}
}

VOID CSafeEdit::OnEditClearAll()
{
	if(!IsReadOnly())
	{	SetSel(0,-1);
		OnEditClear();
	}
}

VOID CSafeEdit::OnEditSelectAll()
{
	SetSel(0,-1);
}

VOID CSafeEdit::OnPaint()
{
	// 内存绘图
	CRect rcClient;
	GetClientRect(rcClient);
	CPaintDC PaintDC(this);


	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(&PaintDC);
	MemBitmap.CreateCompatibleBitmap(&PaintDC,rcClient.Width(),rcClient.Height());
	CBitmap* pOldBitmap=MemDC.SelectObject(&MemBitmap);

	CFont* pOldFont=NULL;
	if(m_dwExtendedStyle & ES_EX_GUIFONT)
		pOldFont = MemDC.SelectObject(&m_fontEdit);
	// 得到颜色绘制背景
	COLORREF crSelText=GetSysColor(COLOR_HIGHLIGHTTEXT);
	COLORREF crSelBkgd=GetSysColor(COLOR_HIGHLIGHT);
	COLORREF crText=GetSysColor(COLOR_WINDOWTEXT);
	COLORREF crBkgd=GetSysColor(COLOR_WINDOW);
	DWORD dwStyle=GetStyle();
	if((dwStyle&WS_DISABLED)!=0)
	{	crText=GetSysColor(COLOR_GRAYTEXT);
		crBkgd=GetSysColor(COLOR_3DFACE);
	}
	else if(IsReadOnly())
	{	crText=GetSysColor(COLOR_BTNTEXT);
		crBkgd=GetSysColor(COLOR_3DFACE);
	}
	MemDC.FillSolidRect(rcClient,crBkgd);

	// 绘制边框
	CRect rcBorder=rcClient;
	if(IsHasBorder())
	{	MemDC.Draw3dRect(rcBorder,GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DHIGHLIGHT));
		rcBorder.DeflateRect(1,1);
		MemDC.Draw3dRect(rcBorder,GetSysColor(COLOR_3DDKSHADOW),GetSysColor(COLOR_3DLIGHT));
		rcBorder.DeflateRect(1,1);
	}

	// 设定选区
	CRect rcClip=rcClient;
	if(IsHasBorder()) rcClip.DeflateRect(CX_MARGINS,CY_MARGINS);
	CRgn rgnClip;
	rgnClip.CreateRectRgn(rcClip.left,rcClip.top,rcClip.right,rcClip.bottom);
	MemDC.SelectClipRgn(&rgnClip);

	// 绘制文本内容
	CString strText;
	GetPaintTextSafe(strText);
	CPoint ptText=PosFromCharIndex(0);
	MemDC.SetTextColor(crText);
	MemDC.SetBkMode(TRANSPARENT);
	MemDC.TextOut(ptText.x,ptText.y,strText);

	// 绘制高亮字体
	if((dwStyle&WS_DISABLED)==0)
	{	LONG nSelStartChar=0,nSelEndChar=0;
		GetSel(nSelStartChar,nSelEndChar);
		if(nSelStartChar<nSelEndChar)
		{	CPoint ptSelText=PosFromCharIndex(nSelStartChar);
			CString strSelText;
			SubPaintText(strText,strSelText,nSelStartChar,nSelEndChar-nSelStartChar);
			MemDC.SetTextColor(crSelText);
			MemDC.SetBkMode(OPAQUE);
			MemDC.SetBkColor(crSelBkgd);
			MemDC.TextOut(ptSelText.x,ptSelText.y,strSelText);
			strSelText="";
		}
	}

	//把内存绘图拷贝到屏幕
	strText="";
	PaintDC.BitBlt(rcClient.left,rcClient.top,rcClient.right,rcClient.bottom,&MemDC,0,0,SRCCOPY);
	MemDC.SelectObject(pOldBitmap);
	if(pOldFont)
		MemDC.SelectObject(pOldFont);
	MemDC.DeleteDC();
	MemBitmap.DeleteObject();
}

VOID CSafeEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags,point);
	// 设置焦点
	BOOL bIsFocus=(::GetFocus()==m_hWnd);
	if(!bIsFocus) SetFocus();
	// 设置选中
	LONG nStartCharNew=CharIndexFromPos(point);
	SetSel(nStartCharNew,nStartCharNew);
	// 设置跟踪开始
	if(!m_bTrackMouse) { m_bTrackMouse=TRUE; ::SetCapture(m_hWnd); m_nTrackFrom=GetCaret(); m_ptTrackFrom=point; }
}

VOID CSafeEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags,point);
	// 设置跟踪停止
	if(m_bTrackMouse) { m_bTrackMouse=FALSE; ::ReleaseCapture(); }
}

void CSafeEdit::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags,point);

	// 选中全部
	SetSel(0,-1);
}

VOID CSafeEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags,point);
	// 跟踪选中区域
	if(m_bTrackMouse)
	{	CRect rcClient;
		GetClientRect(&rcClient);
		point.x=max(min(point.x,rcClient.right+1),rcClient.left-1);
		SetSel(m_nTrackFrom,CharIndexFromPos(point));
	}
}

BOOL CSafeEdit::OnEraseBkgnd(CDC* pDC)
{	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

VOID CSafeEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);

	// 如果存在输入法,禁止输入法
	HKL hklCurrent=GetKeyboardLayout(0);
	CString strTest;
	strTest.Format("%08x",hklCurrent);
	if(strcmp(strTest.Left(4),strTest.Right(4)))
	{	BOOL bRet=ImmIsIME(hklCurrent);
		if(bRet) bRet=ImmSimulateHotKey(GetSafeHwnd(),IME_CHOTKEY_IME_NONIME_TOGGLE);
	}

	// 创建关标并显示
	CRect rcClient;
	GetClientRect(rcClient);
	m_bCaretValid=TRUE;
	CreateSolidCaret(1,rcClient.Height()-2*(IsHasBorder()?CY_MARGINS:0+1));
	SetCaretPos(PosFromCharIndex(GetCaret()));
	ShowCaretWhenVisible();

	// 通知父窗口焦点变化
	CWnd* pFeedback=GetOwner();
	if(pFeedback==NULL) pFeedback=GetParent();
	HWND hFeedback=pFeedback->GetSafeHwnd();
	::SendMessage(hFeedback,WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),EN_SETFOCUS),(LPARAM)GetSafeHwnd());
}

VOID CSafeEdit::OnKillFocus(CWnd* pNewWnd)
{	HideCaretWhenVisible();
	DestroyCaret();
	m_bCaretValid=FALSE;
	CWnd::OnKillFocus(pNewWnd);

	// 设置跟踪停止
	if(m_bTrackMouse) { m_bTrackMouse=FALSE; ::ReleaseCapture(); }

	// 通知父窗口焦点变化
	CWnd* pFeedback=GetOwner();
	if(pFeedback==NULL) pFeedback=GetParent();
	HWND hFeedback=pFeedback->GetSafeHwnd();
	::SendMessage(hFeedback,WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),EN_KILLFOCUS),(LPARAM)GetSafeHwnd());
}

UINT CSafeEdit::OnGetDlgCode() 
{
	return DLGC_WANTARROWS|DLGC_HASSETSEL|DLGC_WANTCHARS;
}


BOOL CSafeEdit::RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName)
{
	
	WNDCLASS WndClass={0};
	ZeroMemory(&WndClass,sizeof(WndClass));
	WndClass.style=CS_PARENTDC|CS_DBLCLKS;
	WndClass.lpfnWndProc=::DefWindowProc;
	WndClass.hInstance=hInstance?hInstance:(AfxGetModuleState()->m_hCurrentInstanceHandle);
	WndClass.hCursor=AfxGetApp()->LoadStandardCursor(IDC_IBEAM);
	WndClass.lpszClassName=lpszClassName;
	WndClass.hIcon=NULL;
	return AfxRegisterClass(&WndClass);
}

VOID CSafeEdit::GenerateWeakKey(LPBYTE pWeakKey,DWORD cbWeakKey)
{	BYTE acKeySet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+";
	for(DWORD dwi=0; dwi<cbWeakKey; dwi++)
	{	DWORD dwKeyNo=rand();
		dwKeyNo=dwKeyNo%(sizeof(acKeySet)/sizeof(acKeySet[0]));
		pWeakKey[dwi]=acKeySet[dwKeyNo];
	}
}

VOID CSafeEdit::XorByWeakKey(LPSTR pszString,LONG nLength,const unsigned char* pWeakKey,DWORD cbWeakKey) const
{	for(LONG i=0; i<min(nLength,(LONG)cbWeakKey); i++)
		pszString[i]=(CHAR)(pszString[i]^pWeakKey[i]);
}

VOID CSafeEdit::GetPaintTextSafe(CString& rString) const
{
	GetTextSafe(rString);
	if(IsPassword())
	{	LONG nLength=rString.GetLength();
		rString.Empty();
		for(LONG i=0; i<nLength; i++) rString+=m_strHideMask;
	}
}

VOID CSafeEdit::SubPaintText(LPCTSTR pszText,CString& rString,LONG nStart,LONG nCount) const
{
	if(IsPassword())
			rString=CString(pszText).Mid(nStart*m_strHideMask.GetLength(),nCount*m_strHideMask.GetLength());
	else	rString=CString(pszText).Mid(nStart,nCount);
}

CPoint CSafeEdit::PosFromCharIndex(LONG nCharIndex)
{
	// 计算字符位置
	CRect rcView;
	GetClientRect(rcView);
	if(IsHasBorder()) rcView.DeflateRect(CX_MARGINS,CY_MARGINS);

	// 得到可见的绘制内容,如果可见的字符为空,则返回空
	CString strText,strLeftText1,strLeftText2;
	GetPaintTextSafe(strText);
	SubPaintText(strText,strLeftText1,0,m_nFirstVisible);
	SubPaintText(strText,strLeftText2,0,nCharIndex);
	CDC DC;
	DC.Attach(::GetDC(NULL));

	CFont* pOldFont=NULL;
	if(m_dwExtendedStyle & ES_EX_GUIFONT)
		pOldFont = DC.SelectObject(&m_fontEdit);
	CSize szLeftText1=DC.GetTextExtent(strLeftText1);
	CSize szLeftText2=DC.GetTextExtent(strLeftText2);
	if(pOldFont)
		DC.SelectObject(pOldFont);
	::ReleaseDC(NULL,DC.Detach());
	CleanString(&strText);
	CleanString(&strLeftText1);
	CleanString(&strLeftText2);
	return CPoint(rcView.left+szLeftText2.cx-szLeftText1.cx,rcView.top);
}

LONG CSafeEdit::CharIndexFromPos(CPoint point)
{
	// 得到第一个字符的位置
	CPoint ptLeft=PosFromCharIndex(0);

	// 判断鼠标是否在当前位置中
	CString strText;
	GetTextSafe(strText);
	CDC DC;
	DC.Attach(::GetDC(NULL));
	CFont* pOldFont=NULL;
	if(m_dwExtendedStyle & ES_EX_GUIFONT)
		pOldFont = DC.SelectObject(&m_fontEdit);
	BOOL bHalf=FALSE;
	for(LONG i=0; i<GetLengthSafe(); i++)
	{	if(!bHalf)
		{	CString strLeftText;
			GetPaintTextSafe(strLeftText);
			SubPaintText(strLeftText,strLeftText,0,i+1);
			CSize szText=DC.GetTextExtent(strLeftText);
			strLeftText="";
			if(point.x<=ptLeft.x+szText.cx) break;
		}
		BYTE cChar=strText.GetAt(i);
		if(cChar>=(BYTE)0XA1) bHalf=!bHalf;
	}
	if(pOldFont)
		DC.SelectObject(pOldFont);
	::ReleaseDC(NULL,DC.Detach());
	CleanString(&strText);
	return i;
}

LONG CSafeEdit::CorrectCharIndex(LONG nCharIndex)
{
	CString strText;
	GetTextSafe(strText);
	BOOL bHalf=FALSE;
	for(LONG i=0; i<strText.GetLength(); i++)
	{	if(i==nCharIndex)
		{	if(bHalf&&nCharIndex>0) nCharIndex--;
			break;
		}
		BYTE cChar=strText.GetAt(i);
		if(cChar>=(BYTE)0XA1) bHalf=!bHalf;
	}
	CleanString(&strText);
	return nCharIndex;
}

VOID CSafeEdit::ShowCaretWhenVisible()
{
	if(!m_bCaretValid) return;
	if(m_bCaretVisible) return;

	CRect rcView;
	GetClientRect(rcView);
	if(IsHasBorder()) rcView.DeflateRect(CX_MARGINS,CY_MARGINS);
	CPoint ptCaret=GetCaretPos();
	if(rcView.PtInRect(ptCaret))
	{	m_bCaretVisible=TRUE;
		ShowCaret();
	}
}

VOID CSafeEdit::HideCaretWhenVisible()
{
	if(!m_bCaretValid) return;
	if(m_bCaretVisible)
	{	m_bCaretVisible=FALSE;
		HideCaret();
	}
}

VOID CSafeEdit::GetStringFromClipboard(LPSTR pszString,DWORD cbString)
{	if(pszString==NULL||cbString==0) return;
	pszString[0]=0;
	OpenClipboard();
	HANDLE hGlobalMemory=GetClipboardData(CF_TEXT);
	if(hGlobalMemory!=NULL)
	{	LPCSTR lpGlobalMemory=(LPCSTR)GlobalLock(hGlobalMemory);
		if(lpGlobalMemory!=NULL)
		{	strncpy(pszString,(LPSTR)lpGlobalMemory,cbString);
			pszString[cbString-1]=0;
		}
		GlobalUnlock(hGlobalMemory);
	}
	CloseClipboard();
}

VOID CSafeEdit::SetStringToClipboard(LPCSTR pszString)
{
	DWORD dwLength=strlen(pszString);
	HANDLE hGlobalMemory=GlobalAlloc(GHND,dwLength+1);
	LPBYTE lpGlobalMemory=(LPBYTE)GlobalLock(hGlobalMemory);
	strcpy((LPSTR)lpGlobalMemory,pszString);
	GlobalUnlock(hGlobalMemory);
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_TEXT,hGlobalMemory);
	CloseClipboard();
}


//兼容模式下处理combox键盘消息
BOOL CSafeEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message>=WM_KEYFIRST&&pMsg->message<=WM_KEYLAST)
	{
		// TAB/RETURN/ESCAPE的处理特殊处理
		if(	pMsg->wParam==VK_NUMLOCK||pMsg->wParam==VK_CAPITAL||pMsg->wParam==VK_SCROLL||
			pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN||pMsg->wParam==VK_TAB||
			(pMsg->wParam>=VK_F1&&pMsg->wParam<=VK_F12))
		{	return FALSE;
		}
		// 其他按键处理
		else if(!IsSafeMode())
		{	CSafeEngine::KBDEVENT KbdEvent={0};
			CSafeEngine::KeyDownMsgParamToKbdEvent(pMsg->wParam,pMsg->lParam,&KbdEvent);
			return !HandleKbdEventSafe(&KbdEvent);
		}
		return TRUE;
	}
	return CWnd::PreTranslateMessage(pMsg);
}
