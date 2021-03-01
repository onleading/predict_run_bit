// HisView.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "HisView.h"
#include "ViewManager.h"

#include "SelectDateDlg.h"
#include "SetQRParamDlg.h"
#include "SelectGPDlg.h"
#include "MainFrm.h"

#include "ComView.h"
#include "TxtView.h"
#include "GridView.h"
#include "IndexSortDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHisView

IMPLEMENT_DYNCREATE(CHisView, CView)

extern void GetInitDomain(int &WhichGPType,int &BlockFlag,int FirstType,int FirstBlock);

CHisView::CHisView()
{
	KeyGuyFlag = FALSE;
	bAutoPage  = FALSE;

	m_bStartDrag = FALSE;
	m_bDragging = FALSE;

	//��������û�й��������ֱ��SetDomain�Ź������
	bHasRM = FALSE;
	bHasZB = FALSE;
	bHasQR = FALSE;
	bHasQJ = FALSE;
	//ȱʡ����Domain

	OldCurType = -1;
	CurType = HisCurType; //��ȫ�ֱ�������ʼ�����ȴ򿪵���ʷ��������

	//����ʼ��Ʊ����
	GetInitDomain(m_WhichGPType,m_nBlockFlag,FirstHisGPType,FirstHisBlockFlag);

	m_bQuitting = FALSE;

	m_pMemDC = new CDC ;
	m_pBm = new CBitmap;
}

CHisView::~CHisView()
{
	theViewManager.RemoveView(this);
	delete m_pMemDC; //CLEAN UP OUR VARIABLES
	delete	m_pBm;
}

BEGIN_MESSAGE_MAP(CHisView, CView)
	ON_COMMAND(ID_INBLOCK,OnInBlock)
	ON_COMMAND(ID_BATCH,OnBatch)
	ON_UPDATE_COMMAND_UI(ID_BATCH, OnUpdateBatch)
	ON_COMMAND(ID_GPFL,OnGPFL)
	ON_COMMAND_RANGE(ID_RIGHT_TYPE0,ID_RIGHT_TYPE20,OnClickCorner)
	ON_COMMAND_RANGE(ID_DY1,ID_DY32,OnDYBlock)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DY1,ID_DY32,OnUpdateDYBlock)
	ON_COMMAND_RANGE(ID_HY1,ID_HY52,OnHYBlock)
	ON_UPDATE_COMMAND_UI_RANGE(ID_HY1,ID_HY52,OnUpdateHYBlock)
	ON_COMMAND_RANGE(ID_CONCEPT_MENU_1,ID_CONCEPT_MENU_100,OnConceptClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CONCEPT_MENU_1,ID_CONCEPT_MENU_100,OnUpdateConceptClick)

	ON_COMMAND_RANGE(ID_ZHB_MENU_1,ID_ZHB_MENU_100,OnZHBClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZHB_MENU_1,ID_ZHB_MENU_100,OnUpdateZHBClick)
	ON_COMMAND_RANGE(ID_ZJHHY_MENU_1,ID_ZJHHY_MENU_200,OnZJHHYClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZJHHY_MENU_1,ID_ZJHHY_MENU_200,OnUpdateZJHHYClick)

	ON_UPDATE_COMMAND_UI_RANGE(ID_RIGHT_TYPE0,ID_RIGHT_TYPE20,OnUpdateClickCorner)
	ON_COMMAND_RANGE(ID_BLOCK_MENU_1,ID_BLOCK_MENU_400,OnMenuBlockClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BLOCK_MENU_1,ID_BLOCK_MENU_400,OnUpdateMenuBlockClick)

	ON_COMMAND_RANGE(ID_RIGHT_OPEN,ID_RIGHT_NEWWINDOW,OnToGPVip)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_NEWWINDOW,OnUpdateNewWindow)

	ON_COMMAND_RANGE(ID_SELECTDATE,ID_ADDINDEXRANK,OnClickRight)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SELECTDATE,ID_ADDINDEXRANK,OnUpdateClickRight)

	ON_COMMAND(ID_DELINDEXRANK,OnDelIndexRank)
	ON_UPDATE_COMMAND_UI(ID_DELINDEXRANK, OnUpdateDelIndexRank)

	ON_COMMAND_RANGE(ID_RMREPORT,ID_QJTYPE4, OnChangeReport)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RMREPORT,ID_QJTYPE4, OnUpdateChangeReport)

	ON_COMMAND_RANGE(ID_ALLBLOCKFX,ID_ZJHHYBLOCKFX,OnSelBlockFX)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ALLBLOCKFX,ID_ZJHHYBLOCKFX,OnUpdateSelBlockFX)

	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnUpdateMarkStockFlag)

	ON_MESSAGE(WM_KEYGUY_MESSAGE,OnKeyGuyMessage)
	ON_MESSAGE(WM_TREE_MESSAGE,OnTreeMessage)
	ON_MESSAGE(WM_CHANGEBLOCK,OnChangeBlock)
	ON_MESSAGE(UM_HSCROLLBAR,OnHScrollMessage)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	//{{AFX_MSG_MAP(CHisView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CHANGEGPTYPE, OnChangeGPType)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_RIGHT_ADDBLOCK, OnRightAddblock)
	ON_COMMAND(ID_RIGHT_ADDZXG, OnRightAddZxg)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_ADDBLOCK, OnUpdateRightAddblock)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_ADDZXG, OnUpdateRightAddblock)
	ON_COMMAND(ID_DELFROMBLOCK, OnDelFromBlock)
	ON_COMMAND(ID_QJREPORT, OnQjreport)
	ON_UPDATE_COMMAND_UI(ID_QJREPORT, OnUpdateQjreport)
	ON_COMMAND(ID_WRITETOTXTFILE, OnWritetotxtfile)
	ON_COMMAND(ID_AUTOPAGE, OnAutopage)
	ON_UPDATE_COMMAND_UI(ID_AUTOPAGE, OnUpdateAutopage)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHisView drawing

//�����Invalidate����,������OnDraw��ʽ
//DRAW_ALL 1 DRAW_NOREADDATA 2
void CHisView::SpecialReDraw(int Flag)
{
	if(CurType == ZB_REPORT)
	{
		m_ZBReport.RefreshScrollBar();
		m_ZBReport.DrawFlag = Flag;
	}
	else if(CurType == QR_REPORT)
	{
		m_QRReport.RefreshScrollBar();
		m_QRReport.DrawFlag = Flag;
	}
	else if(CurType == QJ_REPORT)
	{
		m_QJReport.RefreshScrollBar();
		m_QJReport.DrawFlag = Flag;
	}
	else
	{
		m_RMReport.RefreshScrollBar();
		m_RMReport.DrawFlag = Flag;
	}
	Invalidate(FALSE);
}

void CHisView::OnDraw(CDC* pDC)
{
	if(g_pToolTipMark) 
		g_pToolTipMark->ClearToolTipInfo(this);
	if(CurType == ZB_REPORT)
	{
		if(bHasZB)	m_ZBReport.DrawReport(pDC);
		else 
		{
			if(OldCurType == QR_REPORT)
				m_QRReport.DrawReport(pDC);
			else if(OldCurType == QJ_REPORT)
				m_QJReport.DrawReport(pDC);
			else if(OldCurType == RM_REPORT)
				m_RMReport.DrawReport(pDC);
			else
				m_ZBReport.DrawReport(pDC);
		}
	}
	if(CurType == QR_REPORT)
	{
		if(bHasQR) m_QRReport.DrawReport(pDC);
		else
		{
			if(OldCurType == ZB_REPORT)
				m_ZBReport.DrawReport(pDC);
			else if(OldCurType == QJ_REPORT)
				m_QJReport.DrawReport(pDC);
			else if(OldCurType == RM_REPORT)
				m_RMReport.DrawReport(pDC);
			else
				m_QRReport.DrawReport(pDC);
		}
	}
	if(CurType == QJ_REPORT)
	{
		if(bHasQJ) m_QJReport.DrawReport(pDC);
		else
		{
			if(OldCurType == ZB_REPORT)
				m_ZBReport.DrawReport(pDC);
			else if(OldCurType == QR_REPORT)
				m_QRReport.DrawReport(pDC);
			else if(OldCurType == RM_REPORT)
				m_RMReport.DrawReport(pDC);
			else
				m_QJReport.DrawReport(pDC);
		}
	}
	if(CurType == RM_REPORT)
	{
		if(bHasRM) m_RMReport.DrawReport(pDC);
		else
		{
			if(OldCurType == ZB_REPORT)
				m_ZBReport.DrawReport(pDC);
			else if(OldCurType == QR_REPORT)
				m_QRReport.DrawReport(pDC);
			else if(OldCurType == QJ_REPORT)
				m_QJReport.DrawReport(pDC);
			else
				m_RMReport.DrawReport(pDC);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHisView diagnostics

#ifdef _DEBUG
void CHisView::AssertValid() const
{
	CView::AssertValid();
}

void CHisView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHisView message handlers

BOOL CHisView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CHisView::OnInitialUpdate()
{
	CView::OnInitialUpdate();	
	Modify_CorpWebMenu();

	if(g_pToolTipMark) 
		g_pToolTipMark->AddTool(this, ""); 
		
	CString title = "�������";
	theViewManager.AddView(title, this);
	
	m_ZBReport.RefreshScrollBar();
	m_QRReport.RefreshScrollBar();
	m_QJReport.RefreshScrollBar();
	m_RMReport.RefreshScrollBar();

	WakeUp(CurType,g_nCurQJType);
}

void CHisView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	theViewManager.pOldActiveView2 = theViewManager.pActiveView;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_His1+CurType);
	}
}

void CHisView::ChangeGPType(BOOL bTurn)  //�ڷ���ɡ���Ʊ�ء���ѡ�ɼ��л�
{
	int index = m_WhichGPType;
	if(index != -1)
	{
		if(bTurn) index = (index + 1)%18; //����
		else
		{
			if(index == 0)	index = 17;
			else	index = (index - 1)%18; //����
		}
	}
	else	index = 0;

	OnClickCorner(index + ID_RIGHT_TYPE0);
}

BOOL CHisView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
		//�����'-'�ţ����Զ���ҳ
		if(pMsg->wParam == '-')
		{
			OnAutopage();
			return TRUE;
		}
		//��Ʊ�����л�
		if(pMsg->wParam == ']' || pMsg->wParam == '[' )
		{
			if(pMsg->wParam == ']')
				ChangeGPType(TRUE);  //����
			else
				ChangeGPType(FALSE);//����
			return TRUE;
		}
		if(pMsg->wParam == VK_TAB)
		{
			if(FiltTabKeyCom()) return TRUE;
			int index = (CurType+1)%4;
			OnChangeReport(index + ID_RMREPORT);
			return TRUE;
		}
	}
	if(pMsg->message==WM_KEYDOWN && (pMsg->wParam=='z'||pMsg->wParam=='Z'))
	{
		SHORT CtrlDown=::GetKeyState(VK_CONTROL);
		if(CtrlDown<0)
		{
			if(::GetKeyState(VK_SHIFT)<0)
				PostMessage(WM_COMMAND,ID_DELFROMBLOCK);
			else
				PostMessage(WM_COMMAND,ID_RIGHT_ADDBLOCK);
			return TRUE;
		}
	}
	if(g_pToolTipMark) 
		g_pToolTipMark->RelayEvent(pMsg);
	return CView::PreTranslateMessage(pMsg);
}

int CHisView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_RMReport.Create(this,CRect(0,0,0,0));
	m_ZBReport.Create(this,CRect(0,0,0,0));
	m_QRReport.Create(this,CRect(0,0,0,0));
	m_QJReport.Create(this,CRect(0,0,0,0));

	//�����Զ���ҳ�Ķ�ʱ��
	SetTimer(1,AutoPageTime*1000,NULL);
	SetTimer(2,RMREPORT_SECOND*1000,NULL); //���Ű������Ķ�̬���㶨ʱ��

	m_DragImage.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_DragImage.Add(hIcon);

	return 0;
}

void CHisView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if(CurType == RM_REPORT)
		m_RMReport.OnSize(nType,cx,cy);
	if(CurType == ZB_REPORT)
		m_ZBReport.OnSize(nType,cx,cy);
	if(CurType == QR_REPORT)
		m_QRReport.OnSize(nType,cx,cy);
	if(CurType == QJ_REPORT)
		m_QJReport.OnSize(nType,cx,cy);
}

void CHisView::OnHScrollMessage(WPARAM wParam,LPARAM lParam)
{
	if(CurType == RM_REPORT)
		m_RMReport.OnHScrollMessage(wParam,lParam);
	if(CurType == ZB_REPORT)
		m_ZBReport.OnHScrollMessage(wParam,lParam);
	if(CurType == QR_REPORT)
		m_QRReport.OnHScrollMessage(wParam,lParam);
	if(CurType == QJ_REPORT)
		m_QJReport.OnHScrollMessage(wParam,lParam);
}

void CHisView::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	if(CurType == RM_REPORT)
		m_RMReport.OnVScrollMessage(wParam,lParam);
	if(CurType == ZB_REPORT)
		m_ZBReport.OnVScrollMessage(wParam,lParam);
	if(CurType == QR_REPORT)
		m_QRReport.OnVScrollMessage(wParam,lParam);
	if(CurType == QJ_REPORT)
		m_QJReport.OnVScrollMessage(wParam,lParam);
}

void CHisView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(CurType == RM_REPORT)
		m_RMReport.OnKeyDown(nChar,nRepCnt,nFlags);
	if(CurType == ZB_REPORT)
		m_ZBReport.OnKeyDown(nChar,nRepCnt,nFlags);
	if(CurType == QR_REPORT)
		m_QRReport.OnKeyDown(nChar,nRepCnt,nFlags);
	if(CurType == QJ_REPORT)
		m_QJReport.OnKeyDown(nChar,nRepCnt,nFlags);

	if(nChar == VK_RETURN)
	{
		if(CurType == RM_REPORT)
			m_RMReport.OpenBlockHQ();
		else if(GetCurrentGPIndex() != -1)
			ToGPVip(FALSE);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CHisView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(CurType == RM_REPORT)
		m_RMReport.OnLButtonDown(nFlags, point);
	if(CurType == ZB_REPORT)
		m_ZBReport.OnLButtonDown(nFlags, point);
	if(CurType == QR_REPORT)
		m_QRReport.OnLButtonDown(nFlags, point);
	if(CurType == QJ_REPORT)
		m_QJReport.OnLButtonDown(nFlags, point);

	DraggingGPNo = -1;
	if(CurType == ZB_REPORT)
		DraggingGPNo = m_ZBReport.GetDragItem(point);
	if(CurType == QR_REPORT)
		DraggingGPNo = m_QRReport.GetDragItem(point);
	if(CurType == QJ_REPORT)
		DraggingGPNo = m_QJReport.GetDragItem(point);

	CView::OnLButtonDown(nFlags, point);
}

void CHisView::OnUpdateMenuBlockClick(CCmdUI *pCmdUI)
{
	if(CurType == ZB_REPORT)
		pCmdUI->SetCheck( pCmdUI->m_nID-ID_BLOCK_MENU_1 == m_ZBReport.m_nBlockFlag );
	if(CurType == QR_REPORT)
		pCmdUI->SetCheck( pCmdUI->m_nID-ID_BLOCK_MENU_1 == m_QRReport.m_nBlockFlag );
	if(CurType == QJ_REPORT)
		pCmdUI->SetCheck( pCmdUI->m_nID-ID_BLOCK_MENU_1 == m_QJReport.m_nBlockFlag );
}

void CHisView::OnUpdateClickCorner(CCmdUI* pCmdUI)
{
	if(CurType == ZB_REPORT)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_RIGHT_TYPE0 == m_ZBReport.m_WhichGPType);
	if(CurType == QR_REPORT)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_RIGHT_TYPE0 == m_QRReport.m_WhichGPType);
	if(CurType == QJ_REPORT)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_RIGHT_TYPE0 == m_QJReport.m_WhichGPType);
}

//���ѡ�з���ɡ���ѡ�ɡ���Ʊ��
void CHisView::OnClickCorner(UINT nID)
{
	int index = nID - ID_RIGHT_TYPE0;
	m_nBlockFlag = -1;
	m_WhichGPType = index;
	//������ѡ�ɺ͹�Ʊ�ص����ǰ���
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

void CHisView::OnDYBlock(UINT nID)
{
	m_nBlockFlag = nID-ID_DY1+DY_START;
	m_WhichGPType = -1;
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

void CHisView::OnUpdateDYBlock(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_DY1 == m_nBlockFlag-DY_START);
}

void CHisView::OnHYBlock(UINT nID)
{
	m_nBlockFlag = nID-ID_HY1+HY_START;
	m_WhichGPType = -1;
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

void CHisView::OnUpdateHYBlock(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_HY1 == m_nBlockFlag-HY_START);
}

void CHisView::OnConceptClick(UINT nID)
{
	m_nBlockFlag = nID-ID_CONCEPT_MENU_1+CON_START;
	m_WhichGPType = -1;
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

void CHisView::OnUpdateConceptClick(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_CONCEPT_MENU_1 == m_nBlockFlag-CON_START);
}

void CHisView::OnZHBClick(UINT nID)
{
	m_nBlockFlag = nID-ID_ZHB_MENU_1+ZHB_START;
	m_WhichGPType = -1;
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

void CHisView::OnUpdateZHBClick(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_ZHB_MENU_1 == m_nBlockFlag-ZHB_START);
}

void CHisView::OnZJHHYClick(UINT nID)
{
	m_nBlockFlag = nID-ID_ZJHHY_MENU_1+ZJHHY_START;
	m_WhichGPType = -1;
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

void CHisView::OnUpdateZJHHYClick(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_ZJHHY_MENU_1 == m_nBlockFlag-ZJHHY_START);
}

void CHisView::OnMenuBlockClick(UINT nID)
{
	m_nBlockFlag = nID-ID_BLOCK_MENU_1;
	m_WhichGPType = -1;
	//������ѡ�ɺ͹�Ʊ�ص����ǰ���
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

void CHisView::OnToGPVip(UINT nID)
{
	if(nID == ID_RIGHT_OPEN)
		ToGPVip(FALSE);
	else 
		ToGPVip(TRUE);
}

void CHisView::OnUpdateNewWindow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theViewManager.ComWindowNum < 3);
}

void CHisView::OnClickRight(UINT nID)
{
	switch(nID)
	{
	case ID_SELECTDATE:	//ѡ������
		{
			CSelectDateDlg dlg;
			dlg.m_Date1 = m_ZBReport.GetDate();
			if( dlg.DoModal() == IDOK )
				m_ZBReport.SetDate(0,dlg.m_Date1);
			break;
		}
	case ID_SELECTQJ:	//ѡ������
		{
			CSelectDateDlg dlg;
			dlg.bSingle = FALSE;
			if(CurType == QJ_REPORT)
			{
				dlg.m_Date1 = m_QJReport.GetDate(TRUE);
				dlg.m_Date2 = m_QJReport.GetDate(FALSE);
				if( dlg.DoModal() == IDOK )
					m_QJReport.SetDateSection(dlg.m_Date1,dlg.m_Date2);
			}
			break;
		}
	case ID_SELECTRMQJ:
		{
			CSelectDateDlg dlg;
			dlg.bSingle = FALSE;
			if(CurType == RM_REPORT)
			{
				dlg.m_Date1 = m_RMReport.GetDate(TRUE);
				dlg.m_Date2 = m_RMReport.GetDate(FALSE);
				if( dlg.DoModal() == IDOK )
					m_RMReport.SetDateSection(dlg.m_Date1,dlg.m_Date2);
			}
			break;
		}
	case ID_TODAYRM:
		if(CurType == RM_REPORT)
			m_RMReport.SetDateSection(CTime::GetCurrentTime(),CTime::GetCurrentTime());
		break;
	case ID_LAST_DATE:
		m_ZBReport.SetDate(2);
		break;
	case ID_NEXT_DATE:
		m_ZBReport.SetDate(1);
		break;
	case ID_QR_SETBASE:	//��Ϊ��׼
		m_QRReport.SetItBase();
		break;
	case ID_QR_OPENBASE:
		{
			CSelectGPDlg dlg;//��ѡ���Ʊ�Ի���
			dlg.bHasDS = FALSE;
			dlg.bSingleSelect = TRUE;
			dlg.bOverlapGpType = 2;
			if(dlg.DoModal() == IDOK)
			{
				if(dlg.SelectedGPNum > 0)
					m_QRReport.SetBase(dlg.SelectedGPIndex[0]);
			}
		}
		break;
	case ID_INDEXRANK:		//ָ������
		{
			CIndexSortDlg dlg;
			CurIndexSort = IndexSortNum-1; //�趨��ǰ���ڴ����ָ��Ϊ���
			if( dlg.DoModal() == IDOK )
			{
				m_ZBReport.InitReport();
			}
		}
		break;
	case ID_ADDINDEXRANK:	//����ָ������
		{
			CIndexSortDlg dlg;
			CurIndexSort = IndexSortNum; //�趨��ǰ���ڴ����ָ��Ϊ��һ��
			if( dlg.DoModal() == IDOK )
			{
				IndexSortNum++;
				m_ZBReport.InitReport();
			}
		}
		break;
	case ID_QR_SETPARAM:	//�����趨
		{
			CSetQRParamDlg dlg;
			int tempQRParam[5],i;
			for(i = 0;i < 5;i++)	tempQRParam[i] = QRParam[i];
			BOOL bChanged = FALSE;
			if(dlg.DoModal() == IDOK)//����û�ȷ��
			{
				for(i = 0;i < 5;i++)
				{
					if(tempQRParam[i] != QRParam[i])//��������б仯
					{
						bChanged = TRUE;break;
					}
				}
				if(bChanged) 
					m_QRReport.InitReport();
			}
		}
		break;
	case ID_OPENBLOCKHQ:
		m_RMReport.OpenBlockHQ();
		break;
	default:
		break;
	}
}

void CHisView::OnUpdateClickRight(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case ID_ADDINDEXRANK:
		pCmdUI->Enable(IndexSortNum != 4);
		break;
	default:
		break;
	}
}

void CHisView::OnDelIndexRank()
{
	--IndexSortNum;
	CurIndexSort = IndexSortNum-1;
	m_ZBReport.InitReport();
}

void CHisView::OnUpdateDelIndexRank(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IndexSortNum > 1 );
}

void CHisView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	//������еķ�Χ�У��򵯳��е��Ҽ��˵�.
	if(CurType == RM_REPORT)
		m_RMReport.OnClick (&dc,point,TRUE);
	if(CurType == ZB_REPORT)
		m_ZBReport.OnClick (&dc,point,TRUE);
	if(CurType == QR_REPORT)
		m_QRReport.OnClick (&dc,point,TRUE);
	if(CurType == QJ_REPORT)
		m_QJReport.OnClick (&dc,point,TRUE);

	CView::OnRButtonUp(nFlags, point);
}

//˽�к���:����һ������
void CHisView::WakeUp(int WhichReport,int WhichQJType)
{
	//ע��:����Gridλ��ʱ��Ӧ�޸�rc�Ĵ�С������OnSize�еĴ�С���Ӧ
	CRect rc;
	GetClientRect(&rc);
	OldCurType = CurType;  //��¼��һ�εı�������
	switch(WhichReport)
	{
	case RM_REPORT:
		m_QRReport.Sleep();
		m_QJReport.Sleep();
		m_ZBReport.Sleep();
		CurType = RM_REPORT; //λ�ò��ܵ���
		m_RMReport.SetRect(rc);
		if(!bHasRM)	//���û������һ������������֮,������֮
		{
			m_RMReport.InitReport(g_GH.nCurBlockFlag); //ȱʡ�����а��
			m_RMReport.SetCurLabel(g_GH.nCurBlockFlag);
			bHasRM = TRUE;
		}
		else
			m_RMReport.WakeUp();
		break;
	case ZB_REPORT:
		m_QRReport.Sleep();
		m_QJReport.Sleep();
		m_RMReport.Sleep();
		CurType = ZB_REPORT; //λ�ò��ܵ���
		m_ZBReport.SetRect(rc);
		if(!bHasZB)	//���û������һ������������֮,������֮
		{
			m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
			bHasZB = TRUE;
		}
		else
			m_ZBReport.WakeUp();
		break;
	case QR_REPORT:
		m_ZBReport.Sleep();
		m_QJReport.Sleep();
		m_RMReport.Sleep();
		CurType = QR_REPORT; //λ�ò��ܵ���
		m_QRReport.SetRect(rc);
		if(!bHasQR)//���û������һ������������֮,������֮
		{
			m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType); //��һ�¾䲻�ܵ���
			bHasQR = TRUE;
		}
		else
			m_QRReport.WakeUp();
		break;
	case QJ_REPORT:
		m_ZBReport.Sleep();
		m_QRReport.Sleep();
		m_RMReport.Sleep();
		CurType = QJ_REPORT; //λ�ò��ܵ���
		m_QJReport.SetRect(rc);
		if(!bHasQJ)//���û������һ������������֮,������֮
		{
			m_QJReport.CurQJType = WhichQJType;
			m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
			bHasQJ = TRUE;
		}
		else if(m_QJReport.CurQJType != WhichQJType)//����뵱ǰ���Ͳ�һ�£������������������������
			m_QJReport.SetQJType(WhichQJType);
		else m_QJReport.WakeUp();	
	}
}

void CHisView::OnChangeReport(UINT nID)
{
	AfxGetApp()->OnIdle(-1);
	if(nID < ID_QJTYPE1)	WakeUp(nID-ID_RMREPORT,0); //����һ������
	else	WakeUp(QJ_REPORT,nID-ID_QJTYPE1);	//����һ�������������
	AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_His1+CurType);
}


void CHisView::OnQjreport() 
{
	
}

void CHisView::OnUpdateQjreport(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(CurType == QJ_REPORT);
}

void CHisView::OnUpdateChangeReport(CCmdUI *pCmdUI)
{
	int tempInt = pCmdUI->m_nID - ID_RMREPORT;
	switch(tempInt)
	{
	case 0:
		pCmdUI->SetCheck(CurType == RM_REPORT);
		break;
	case 1:
		pCmdUI->SetCheck(CurType == ZB_REPORT);
		break;
	case 2:
		pCmdUI->SetCheck(CurType == QR_REPORT);
		break;
	default:
		pCmdUI->SetCheck( CurType==QJ_REPORT && 
						tempInt-3 == m_QJReport.CurQJType );
		break;
	}
}

void CHisView::OnSelBlockFX(UINT nID)
{	
	m_RMReport.InitReport(nID-ID_ALLBLOCKFX);
	m_RMReport.SetCurLabel(nID-ID_ALLBLOCKFX);
}

void CHisView::OnUpdateSelBlockFX(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_RMReport.CurBlockFlag==pCmdUI->m_nID-ID_ALLBLOCKFX);
}

void CHisView::OnChangeGPType() 
{
	ChangeGPType(TRUE);
}

short CHisView::GetCurrentGPIndex()
{
	if(CurType == ZB_REPORT)
		return m_ZBReport.GetCurrentGPIndex();
	else if(CurType == QR_REPORT)
		return m_QRReport.GetCurrentGPIndex();
	else if(CurType == QJ_REPORT)
		return m_QJReport.GetCurrentGPIndex();
	else
		return -1;
}

short CHisView::GetCurrentGPSet(short *pPageGPIndex)
{
	short nPageGPNum = 0;
	short *pTempIndex = NULL;
	if(CurType == ZB_REPORT)
	{
		pTempIndex = m_ZBReport.lpnUsedStkIndex;
		nPageGPNum = m_ZBReport.nTotalRow;
	}
	if(CurType == QR_REPORT)
	{
		pTempIndex = m_QRReport.lpnUsedStkIndex;
		nPageGPNum = m_QRReport.nTotalRow;
	}
	if(CurType == QJ_REPORT)
	{	
		pTempIndex = m_QJReport.lpnUsedStkIndex;
		nPageGPNum = m_QJReport.nTotalRow;
	}

	if(nPageGPNum<1)	
		return(0);	//���û�����飬�򷵻�-1;
	memcpy(pPageGPIndex, pTempIndex, nPageGPNum*sizeof(short));
	return nPageGPNum;
}

void CHisView::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1 && bAutoPage)  //�Զ���ҳ��ʱ�¼�
	{
		if(CurType == ZB_REPORT)
			m_ZBReport.ProcessAutoPage();
		if(CurType == QR_REPORT)
			m_QRReport.ProcessAutoPage();
		if(CurType == QJ_REPORT)
			m_QJReport.ProcessAutoPage();
	}
	if(nIDEvent == 2)
	{
		if(OnLine && !g_bDowning && theViewManager.pActiveView==this && CurType == RM_REPORT && Is_Jy_Time())
		{
			if(m_RMReport.m_bTodayRM) 
				m_RMReport.Start_Calc();
		}
	}
	CView::OnTimer(nIDEvent);
}

void CHisView::OnAutopage()
{
	if(!bAutoPage && !SureAutoPage(this))
		return;	
	bAutoPage = !bAutoPage;
}

void CHisView::OnUpdateAutopage(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(bAutoPage);
}

void CHisView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect CornerRect;
	if(CurType == ZB_REPORT)
		m_ZBReport.GetCornerRect(CornerRect);
	if(CurType == QR_REPORT)
		m_QRReport.GetCornerRect(CornerRect);
	if(CurType == QJ_REPORT)
		m_QJReport.GetCornerRect(CornerRect);
	if(CurType == RM_REPORT)
		m_RMReport.GetCornerRect(CornerRect);

	if(point.y < CornerRect.bottom)	return;

	if(CurType == RM_REPORT)
		m_RMReport.OpenBlockHQ();
	else if(GetCurrentGPIndex() != -1)
		ToGPVip(FALSE);
	CView::OnLButtonDblClk(nFlags, point);
}

BOOL CHisView::ProcessGPCode(const char *code,short setcode)
{
	if(CurType == ZB_REPORT)
		return m_ZBReport.ProcessGPCode(code,setcode);
	if(CurType == QR_REPORT)
		return m_QRReport.ProcessGPCode(code,setcode);
	if(CurType == QJ_REPORT)
		return m_QJReport.ProcessGPCode(code,setcode);
	return FALSE;
}

void CHisView::ToGPVip(BOOL bNewView)
{
	MemStkInfo *tempStkInfo;
	int tempCurrentNo = GetCurrentGPIndex();
	if(tempCurrentNo == -1) return;
	tempStkInfo=(*g_pStockIO)[tempCurrentNo];
	if(CurType == ZB_REPORT)
		ShowGpInCom(tempStkInfo->Code,tempStkInfo->setcode,bNewView,m_ZBReport.lpnUsedStkIndex,m_ZBReport.nTotalRow,tempCurrentNo);
	if(CurType == QR_REPORT)
		ShowGpInCom(tempStkInfo->Code,tempStkInfo->setcode,bNewView,m_QRReport.lpnUsedStkIndex,m_QRReport.nTotalRow,tempCurrentNo);
	if(CurType == QJ_REPORT)
		ShowGpInCom(tempStkInfo->Code,tempStkInfo->setcode,bNewView,m_QJReport.lpnUsedStkIndex,m_QJReport.nTotalRow,tempCurrentNo);
}

void CHisView::OnDestroy() 
{
	FirstHisGPType = m_WhichGPType;	
	FirstHisBlockFlag = m_nBlockFlag;
	m_bQuitting = TRUE;
	CView::OnDestroy();
}

void CHisView::OnKeyGuyMessage(WPARAM wParam,LPARAM lParam)
{
	if( wParam >= 400 && wParam <= 406)
		OnChangeReport(ID_RMREPORT+wParam-400);
}

//����֤ȯ����������Ϣ
void CHisView::OnTreeMessage(WPARAM wParam, LPARAM lParam)
{
	DWORD ItemData = (DWORD)wParam;
	WORD LoWord = LOWORD(ItemData);
	WORD HiWord = HIWORD(ItemData);
	switch(HiWord)
	{
	case 0://�����
		m_nBlockFlag = -1;
		m_WhichGPType = LoWord;
		break;
	case 1://�Զ�����
		m_nBlockFlag = LoWord;
		m_WhichGPType = -1;
		break;
	case 2://�������
		m_nBlockFlag = DY_START+LoWord;
		m_WhichGPType = -1;
		break;
	default://��ҵ���
		m_nBlockFlag = HY_START+LoWord;
		m_WhichGPType = -1;
		break;
	}
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(m_nBlockFlag,m_WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(m_nBlockFlag,m_WhichGPType);
}

//������̾��鷢������Ϣ�������Ϣ
void CHisView::OnChangeBlock(WPARAM wParam, LPARAM lParam)
{
	int WhichGPType,nBlockFlag;
	if(wParam==-1)
	{
		nBlockFlag = -1;
		WhichGPType = TYPE_TJG;
	}
	else if(wParam==-2)
	{
		nBlockFlag = -1;
		WhichGPType = TYPE_ZXG;
	}
	else if(wParam < DY_START)//�Զ�����
	{
		nBlockFlag = wParam;
		WhichGPType = -1;
	}
	else			//�ط��������ҵ���
	{
		nBlockFlag = wParam;
		WhichGPType = -1;
	}
	if(CurType == ZB_REPORT)
		m_ZBReport.SetDomain(nBlockFlag,WhichGPType);
	if(CurType == QR_REPORT)
		m_QRReport.SetDomain(nBlockFlag,WhichGPType);
	if(CurType == QJ_REPORT)
		m_QJReport.SetDomain(nBlockFlag,WhichGPType);
}

void CHisView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt(point);
	g_pToolTipMark->ShowTool(pt, this);
	
	ClientToScreen(&pt);
	if(m_bStartDrag)
	{
		SetCursor(m_hCannotDrop);
		m_DragImage.BeginDrag(0,CPoint(12,12));
		m_DragImage.DragEnter(GetDesktopWindow(),pt);
		m_bDragging = TRUE;
		m_bStartDrag = FALSE;
		SetCapture();
	}
	else if(m_bDragging)
	{
		m_DragImage.DragMove(pt);
		CWnd* pDropWnd = WindowFromPoint (pt);
		BOOL bCanDrop = FALSE;	//���ÿ�Drop������
		if(pDropWnd && (pDropWnd->IsKindOf (RUNTIME_CLASS (CTxtView)) || pDropWnd->IsKindOf (RUNTIME_CLASS (CComView))) )
			bCanDrop = TRUE;
		if(bCanDrop) SetCursor(m_hCanDrop);
		else		 SetCursor(m_hCannotDrop);
	}	
	CView::OnMouseMove(nFlags, point);
}

void CHisView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bStartDrag) m_bStartDrag = FALSE;
	if(m_bDragging)
	{
		CPoint pt(point);
		ClientToScreen( &pt);
		CWnd* pDropWnd = WindowFromPoint (pt);
		if (pDropWnd && pDropWnd->IsKindOf (RUNTIME_CLASS (CTxtView)) )
		{
			if(DraggingGPNo!=-1)
			{
				TxtCurGPCode = (*g_pStockIO)[DraggingGPNo]->Code;
				pDropWnd->SendMessage(WM_RICHEDIT_MESSAGE);
			}
		}
		else if(pDropWnd && pDropWnd->IsKindOf (RUNTIME_CLASS (CComView)) )
		{
			if(DraggingGPNo!=-1)
			{
				ComCurGPCode = (*g_pStockIO)[DraggingGPNo]->Code;
				ComCurSetCode = (*g_pStockIO)[DraggingGPNo]->setcode;
				m_DragImage.DragLeave(GetDesktopWindow ());
				m_DragImage.EndDrag();
				ReleaseCapture();
				m_bDragging = FALSE;
				pDropWnd->SendMessage(WM_CHANGEGP_MESSAGE);
				if(!((CComView *)pDropWnd)->IsInPageGP(ComCurSetCode,ComCurGPCode))
					((CComView *)pDropWnd)->SetPageGP(NULL,-1,-1);
				return;
			}
		}
		m_DragImage.DragLeave(GetDesktopWindow ());
		m_DragImage.EndDrag();
		ReleaseCapture();
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		m_bDragging = FALSE;
	}

	CView::OnLButtonUp(nFlags, point);
}

void CHisView::OnInBlock()
{
	int tempCurrentNo = GetCurrentGPIndex();
	if(tempCurrentNo == -1)	return;
	AfxGetMainWnd()->SendMessage(UM_SEARCH_BLOCK,tempCurrentNo);
}

void CHisView::OnRightAddblock() 
{
	Func_RightAddblock(GetCurrentGPIndex(),TRUE);
	Invalidate(FALSE);
}

void CHisView::OnRightAddZxg()
{
	Func_RightAddZxg(GetCurrentGPIndex(),TRUE);
	Invalidate(FALSE);
}

void CHisView::OnDelFromBlock() 
{
	Func_DelFromBlock(GetCurrentGPIndex(),TRUE);
	Invalidate(FALSE);
}

void CHisView::OnUpdateRightAddblock(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CurType!=RM_REPORT);
}

#include "BatchDlg.h"
void CHisView::OnBatch()
{
	CBatchDlg dlg;
	int i,stknum = 0;
	if(TDX_MessageBox (m_hWnd,"ѡ������������ʽ��\r\n\r\nѡ��[��]�����������е����й�Ʊ��ѡ��[��]������������Ʊ",MB_ICONQUESTION|MB_YESNO)==IDYES)
		dlg.m_bAllData=TRUE;

	if(CurType == ZB_REPORT)
	{
		if(dlg.m_bAllData)
		{
			for(i=0;i <= m_ZBReport.nTotalRow-1;i++)
				dlg.m_nStkIndex[stknum++] = m_ZBReport.lpnUsedStkIndex[i];
		}
		else
		{
			for(i=m_ZBReport.nStartRow;i <= min(m_ZBReport.nEndRow,m_ZBReport.nTotalRow-1);i++)
				dlg.m_nStkIndex[stknum++] = m_ZBReport.lpnUsedStkIndex[i];
		}
	}
	else if(CurType == QR_REPORT)
	{
		if(dlg.m_bAllData)
		{
			for(i=0;i <= m_QRReport.nTotalRow-1;i++)
				dlg.m_nStkIndex[stknum++] = m_QRReport.lpnUsedStkIndex[i];
		}
		else
		{
			for(i=m_QRReport.nStartRow;i <= min(m_QRReport.nEndRow,m_QRReport.nTotalRow-1);i++)
				dlg.m_nStkIndex[stknum++] = m_QRReport.lpnUsedStkIndex[i];
		}
	}
	else if(CurType == QJ_REPORT)
	{
		if(dlg.m_bAllData)
		{
			for(i=0;i <= m_QJReport.nTotalRow-1;i++)
				dlg.m_nStkIndex[stknum++] = m_QJReport.lpnUsedStkIndex[i];
		}
		else
		{
			for(i=m_QJReport.nStartRow;i <= min(m_QJReport.nEndRow,m_QJReport.nTotalRow-1);i++)
				dlg.m_nStkIndex[stknum++] = m_QJReport.lpnUsedStkIndex[i];
		}
	}
	dlg.m_nStkNum = stknum;
	dlg.m_bDisableDelete = TRUE;
	dlg.m_nBlockFlag = m_nBlockFlag;
	dlg.m_WhichGPType = m_WhichGPType;
	dlg.DoModal();	
}

void CHisView::OnUpdateBatch(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CurType!=RM_REPORT);
}

void CHisView::OnGPFL()
{
}

#include "ExportDlg.h"
void CHisView::OnWritetotxtfile() 
{
	CExportDlg dlg;
	dlg.m_bGrid = TRUE;
	dlg.m_strInitFileName = _F("�������");
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_nTabSel == 0 || dlg.m_nTabSel == 1)
		{
			if(CurType == ZB_REPORT)
			{
				m_ZBReport.m_bWriteAllHQ = dlg.m_bAllHQ;
				m_ZBReport.WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel);
				m_ZBReport.m_bWriteAllHQ = FALSE;
			}
			else if(CurType == QR_REPORT)
			{
				m_QRReport.m_bWriteAllHQ = dlg.m_bAllHQ;
				m_QRReport.WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel);
				m_QRReport.m_bWriteAllHQ = FALSE;
			}
			else if(CurType == QJ_REPORT)
			{
				m_QJReport.m_bWriteAllHQ = dlg.m_bAllHQ;
				m_QJReport.WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel);
				m_QJReport.m_bWriteAllHQ = FALSE;
			}
			else
			{
				m_RMReport.m_bWriteAllHQ = dlg.m_bAllHQ;
				m_RMReport.WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel);
				m_RMReport.m_bWriteAllHQ = FALSE;
			}
		}
		else
		{
			CRect rect;
			GetClientRect(&rect);
			if(CurType == ZB_REPORT)
				m_ZBReport.DrawFlag = DRAW_NOREADDATA;
			else if(CurType == QR_REPORT)
				m_QRReport.DrawFlag = DRAW_NOREADDATA;
			else if(CurType == QJ_REPORT)
				m_QJReport.DrawFlag = DRAW_NOREADDATA;
			else
				m_RMReport.DrawFlag = DRAW_NOREADDATA;
			SendMessage(WM_PAINT);
			HDC hdcScreen = ::GetDC(m_hWnd);
			HDC hdc = CreateCompatibleDC(hdcScreen);
			HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen,rect.Width(),rect.Height());
			HBITMAP holdbm=(HBITMAP)SelectObject(hdc,hbmp);
			BitBlt(hdc,0,0,rect.Width(),rect.Height(),hdcScreen,rect.left,rect.top,SRCCOPY);
			CDC *pDC = CDC::FromHandle(hdc);
			CPen *oldpen = g_d.SelectPen(pDC,AXIS_PEN);
			g_d.DrawRect(pDC,&rect);
			g_d.RestorePen(pDC,oldpen);
			SaveToBMPFile(hbmp,dlg.m_strFileName);
			SelectObject(hdc,holdbm);
			DeleteObject(holdbm);
			DeleteDC(hdc);
			::ReleaseDC(m_hWnd,hdcScreen);
		}		
		if(TDX_MessageBox (m_hWnd,"�����ɹ�,�Ƿ�򿪴��ļ�?",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			CString tmpStr;
			tmpStr.Format("\"%s\"",dlg.m_strFileName);
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
		}
	}
}

BOOL CHisView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta < 0)
		OnKeyDown(VK_DOWN,1,0);
	else
		OnKeyDown(VK_UP,1,0);
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CHisView::OnCancelMark()
{
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	MarkStockFlag(tempStkInfo, 0);
}

#include "TipMarkDlg.h"
void CHisView::OnMarkStockFlag(UINT nID)
{
	int nFlag = nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	if(nFlag==MAX_GROUPMARK)
	{
		CTipMarkDlg dlg;
		dlg.m_pStock = tempStkInfo;
		if(dlg.DoModal()==IDOK)
			MarkStockTipFlag(tempStkInfo, dlg.m_strTip);
	}
	else MarkStockFlag(tempStkInfo, (char)nFlag);
	Invalidate(FALSE);
}

void CHisView::OnUpdateMarkStockFlag(CCmdUI* pCmdUI)
{
	char nFlag = pCmdUI->m_nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	pCmdUI->SetCheck(nFlag==tempStkInfo->fl_flag);
}

//////////////////////////////////////////////////////////////////////////
//��ӡ
BOOL CHisView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
	{
		if(TDX_MessageBox (m_hWnd,"����̫��,���齫ϵͳ����ɫ���ڰ׵��ٴ�ӡ,Ҫǿ�ƴ�ӡ��?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			return FALSE;
	}

	PRINTDLG pd; //�ýṹ������ӡ�Ի����е�������Ϣ 
	LPDEVMODE lpDevMode; 
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd)) //���Ĭ�ϵĴ�ӡ������Ϣ 
	{ 
		lpDevMode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpDevMode) 
			lpDevMode->dmOrientation=DMORIENT_LANDSCAPE; //����ӡ������Ϊ�����ӡ�� 
		GlobalUnlock(pd.hDevMode); 
	}
	return DoPreparePrinting(pInfo);
}

void CHisView::OnFilePrintPreview()
{
	UpdateWindow();
	
	CView::OnFilePrintPreview();
}

void CHisView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rect;
	GetClientRect(rect);
	CClientDC dc(this);
	SpecialReDraw(DRAW_NOREADDATA);

	if (m_pMemDC->GetSafeHdc()) m_pMemDC->DeleteDC();
	m_pMemDC->CreateCompatibleDC(&dc);	
	m_pMemDC->SetMapMode(MM_ANISOTROPIC);
	m_pMemDC->SetWindowExt(dc.GetDeviceCaps(HORZRES),dc.GetDeviceCaps(VERTRES));
	m_pMemDC->SetViewportExt(m_pMemDC->GetDeviceCaps(HORZRES),m_pMemDC->GetDeviceCaps(VERTRES));

	if (m_pBm->GetSafeHandle()) m_pBm->DeleteObject();
	m_pBm->CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	m_pMemDC->SelectObject(m_pBm);
	dc.DPtoLP(rect); //Convert to Logical Coordinates
	m_PrintRect= rect;   //Save Logical Coordinates
	m_pMemDC->BitBlt(0,0,rect.Width(),rect.Height(),&dc,0,0,SRCCOPY);
	g_d.SelectPen(m_pMemDC,LEVEL_PEN);
	g_d.DrawRect(m_pMemDC,1,1,rect.Width()-1,rect.Height()-1);
}

void CHisView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	int nPrintCx = pDC->GetDeviceCaps(HORZRES);
	int nPrintCy = pDC->GetDeviceCaps(VERTRES);
	CClientDC dc(this);
	int nScreenCx = dc.GetDeviceCaps(HORZRES);
	int nScreenCy = dc.GetDeviceCaps(VERTRES);
	float fRatio = (float)nPrintCy/(float)nPrintCx;
	nScreenCx = nScreenCy/fRatio;

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(nScreenCx, nScreenCy);
	pDC->SetViewportExt(nPrintCx, nPrintCy);
	
	pDC->StretchBlt(0,0,nScreenCx,nScreenCy,m_pMemDC,0,0,m_PrintRect.Width(),m_PrintRect.Height(),SRCCOPY);
}

void CHisView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnEndPrinting(pDC, pInfo);
}
