// SwitcherWnd.cpp
// Download by http://www.codefans.net
// Programmed by: JIMMY BRUSH (Kathy007@email.msn.com)
// 
// Legal:
//
// THIS CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
// You may use and distribute this code provided that you do not
// remove this title header and that you do not charge money for
// it. If you want to update the code, feel free to do so, as long
// as you *mark your changes* in code AND in the revision log below
// (and send it to me ;)
//
//
//
// Version: 1.0 revision 1
//
// Revision Log:
//
// SUN MAR 14 1999 - JIMMY BRUSH -  Finished Writing version 1.0
// MON MAR 15 1999 - JIMMY BRUSH -  Fixed RemoveButton to correctly decrement selected button
//									Added CMemDC_Ex by Keith Rule
//									Fixed up Animation a bit
//
//
//
// In the next version \ Wish List:
//
// 1. Tool Tips in CSwitcherButton
// 2. Support for more buttons than can display (with the up/down button at the right)

#include "stdafx.h"
#include "SwitcherButton.h"
#include "SwitcherWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULTWIDTH 130
#define DEFAULTHEIGHT 25
#define MINWIDTH 24

/////////////////////////////////////////////////////////////////////////////
// CSwitcherWnd

CSwitcherWnd::CSwitcherWnd()
{
	m_iButtonWidth = DEFAULTWIDTH;
	m_iNextButtonStart = 0;
	m_iSelectedButton = -1;
}

CSwitcherWnd::~CSwitcherWnd()
{
}


BEGIN_MESSAGE_MAP(CSwitcherWnd, CWnd)
	//{{AFX_MSG_MAP(CSwitcherWnd)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(SWM_SELCHANGE, OnSelChange)
	ON_MESSAGE(SWM_UNSELECT, OnUnselect)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSwitcherWnd message handlers

BOOL CSwitcherWnd::DoCreate(CWnd *parent, int x, int y, int cx, int cy, CString text)
{
	m_wndParent = parent;

	WNDCLASS myclass;
	myclass.style = CS_HREDRAW|CS_VREDRAW|CS_PARENTDC;
	myclass.lpfnWndProc = AfxWndProc;
	myclass.cbClsExtra = 0;
	myclass.cbWndExtra = 0;
	myclass.hInstance = AfxGetInstanceHandle();
	myclass.hIcon = NULL;
	myclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myclass.hbrBackground = NULL;
	myclass.lpszMenuName = NULL;
	myclass.lpszClassName = "SwitcherWndClass";
	AfxRegisterClass(&myclass);

	return Create("SwitcherWndClass", text, WS_CHILD|WS_VISIBLE, CRect(x,y,cx+x,cy+y), parent, 0);
}

BOOL CSwitcherWnd::AddButton(CString title, HICON icon)
{
	CRect rect;
	GetClientRect(&rect);

	CSwitcherButton* newbutton = new CSwitcherButton();
	newbutton->m_iIcon = icon;
	newbutton->m_iID = m_Buttons.GetSize();
	if (!newbutton->DoCreate(this, m_iNextButtonStart, rect.top, m_iButtonWidth, DEFAULTHEIGHT, title))
		return FALSE;

	m_Buttons.Add((void*)newbutton);
	m_iNextButtonStart += m_iButtonWidth + 3;

	if (m_iNextButtonStart-3 > rect.Width())
	{
		// this loop makes a neat little animation
		int newsize = (rect.Width() / (m_Buttons.GetSize())) - 3;
		register int y;
		for (y = m_iButtonWidth; y >= newsize; y-=3)
		{
			ResizeButtons(y);
			Sleep(15);
		}

		if (y != newsize) ResizeButtons(newsize);

		if (m_iButtonWidth < MINWIDTH)
		{
			// the smallest allowable button size has been reached...
			// in this version, we can't handle this
			ASSERT(0);
			return FALSE;
		}
	}

	return TRUE;
}

void CSwitcherWnd::OnDestroy() 
{
	RemoveAll();
	CWnd::OnDestroy();
}

void CSwitcherWnd::RemoveAll()
{
	// remove the first button over and over
	int size = m_Buttons.GetSize();
	for (int x = 1; x <= size; x++)
	{
		CSwitcherButton* btn = (CSwitcherButton*)m_Buttons.GetAt(0);
		delete btn;
		m_Buttons.RemoveAt(0);
	}

	m_iNextButtonStart = 0;
	m_iButtonWidth = DEFAULTWIDTH;
	m_iSelectedButton = -1;
}

void CSwitcherWnd::ResizeButtons(int NewSize)
{
	if (NewSize < MINWIDTH || NewSize > DEFAULTWIDTH)
	{
		NewSize = max(NewSize, MINWIDTH);
		NewSize = min(NewSize, DEFAULTWIDTH);
	}

	m_iButtonWidth = NewSize;
	m_iNextButtonStart = 0;

	for (register int x = 0; x < m_Buttons.GetSize(); x++)
	{
		CSwitcherButton* btn = (CSwitcherButton*)m_Buttons.GetAt(x);
		btn->SetWindowPos(NULL, m_iNextButtonStart, 0, m_iButtonWidth, DEFAULTHEIGHT, SWP_NOZORDER|SWP_NOCOPYBITS);
		m_iNextButtonStart += m_iButtonWidth + 3;
	}

	RedrawWindow();
}

void CSwitcherWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// simply paint it the window bg color
	CRect rect;
	GetClientRect(&rect);

	CBrush brush;
	brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	dc.FillRect(&rect, &brush);
	brush.DeleteObject();
}

void CSwitcherWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (m_Buttons.GetSize() == 0)
		return;

	CRect rect;
	GetClientRect(&rect);

	int newsize = (rect.Width() / (m_Buttons.GetSize())) - 3;
	if (/*newsize <= DEFAULTWIDTH && */m_iButtonWidth != newsize)
		ResizeButtons(newsize);
}

CSwitcherButton* CSwitcherWnd::GetButtonFromID(int id)
{
	return (CSwitcherButton*)m_Buttons.GetAt(id);
}

LRESULT CSwitcherWnd::OnSelChange(WPARAM wp, LPARAM)
{
	// sent when a button gets clicked
	CSwitcherButton* newsel = (CSwitcherButton*)wp;

	if (m_iSelectedButton > -1)
		GetButtonFromID(m_iSelectedButton)->Unselect();

	m_iSelectedButton = newsel->m_iID;
	GetParent()->SendMessage(UM_MDITABSELCHANGED, m_iSelectedButton, 0);

	return 1;
}

void CSwitcherWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);

	if (m_iSelectedButton > -1)
		GetButtonFromID(m_iSelectedButton)->Unselect();

	m_iSelectedButton = -1;
}

LRESULT CSwitcherWnd::OnUnselect(WPARAM, LPARAM)
{
	// sent when a button gets clicked when its selected
	// (which unselects it)
	m_iSelectedButton = -1;
	return 1;
}

BOOL CSwitcherWnd::RemoveButton(int index)
{
	if (index >= m_Buttons.GetSize() || index < 0)
	{
		ASSERT(0); // out of range
		return FALSE;
	}

	if (m_iSelectedButton == index)
		m_iSelectedButton = -1;
	if (m_iSelectedButton > index)
		m_iSelectedButton -= 1;

	delete GetButtonFromID(index);
	m_Buttons.RemoveAt(index);

	for (register int x = index; x < m_Buttons.GetSize(); x++)
		GetButtonFromID(x)->m_iID -= 1;

	m_iNextButtonStart -= m_iButtonWidth + 3;

	if (m_iButtonWidth != DEFAULTWIDTH)
	{
		// do that funky animation thing
		CRect rect;
		GetClientRect(&rect);
		int newsize = (rect.Width() / (m_Buttons.GetSize())) - 3;
		if (newsize > DEFAULTWIDTH) newsize = DEFAULTWIDTH;

		if (newsize > m_iButtonWidth)
		{
			register int y;
			for (y = m_iButtonWidth; y <= newsize; y+=3)
			{
				ResizeButtons(y);
				Sleep(15);
			}
			if (y != newsize) ResizeButtons(newsize);
		}
	}

	ResizeButtons(m_iButtonWidth);

	return TRUE;
}

// icon can be NULL to not search the icons, just text
int CSwitcherWnd::FindButton(CString text, HICON icon, int StartAt)
{
	if (StartAt < 0 || StartAt >= m_Buttons.GetSize())
	{
		ASSERT(0); // out of range
		return -1;
	}

	for (register int x = StartAt; x < m_Buttons.GetSize(); x++)
	{
		CSwitcherButton* btn = GetButtonFromID(x);
		CString title;
		btn->GetWindowText(title);

		if (icon == NULL)
		{
			if (title == text)
				return x;
		}
		else
		{
			if (title == text && icon == btn->m_iIcon)
				return x;
		}
	}

	return -1;
}

int CSwitcherWnd::GetSel()
{
	return m_iSelectedButton;
}

BOOL CSwitcherWnd::SetSel(int num)
{
	if (num < 0 || num >= m_Buttons.GetSize())
	{
		ASSERT(0); // out of range
		return FALSE;
	}

	if (m_iSelectedButton > -1)
		GetButtonFromID(m_iSelectedButton)->Unselect();

	GetButtonFromID(num)->Select();
	m_iSelectedButton = num;

	return TRUE;
}

CString CSwitcherWnd::GetButtonText(int id)
{
	if (id < 0 || id >= m_Buttons.GetSize())
	{
		ASSERT(0); // out of range
		return "";
	}

	CString retval = "";
	GetButtonFromID(id)->GetWindowText(retval);
	return retval;
}

HICON CSwitcherWnd::GetButtonIcon(int id)
{
	if (id < 0 || id >= m_Buttons.GetSize())
	{
		ASSERT(0); // out of range
		return NULL;
	}

	return GetButtonFromID(id)->m_iIcon;
}

// note that this is 1 greater than the last button ID
int CSwitcherWnd::GetNumButtons()
{
	return m_Buttons.GetSize();
}

BOOL CSwitcherWnd::ModifyButton(int index, CString text, HICON icon)
{
	if (index < 0 || index >= m_Buttons.GetSize())
	{
		ASSERT(0); // out of range
		return NULL;
	}

	CSwitcherButton* btn = GetButtonFromID(index);
	btn->SetWindowText(text);
	btn->m_iIcon = icon;
	btn->Refresh();

	return TRUE;
}
