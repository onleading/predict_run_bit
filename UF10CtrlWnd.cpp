// CUF10CtrlWnd.cpp: implementation of the CUF10CtrlWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "UF10CtrlWnd.h"
#include "UF10.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUF10CtrlWnd::CUF10CtrlWnd(UF10 *pUF10)
{
	m_pUf10 = pUF10;
	m_bLayoutFlag = FALSE;
	m_bQuitting = FALSE;
}

CUF10CtrlWnd::~CUF10CtrlWnd()
{

}

BEGIN_MESSAGE_MAP(CUF10CtrlWnd, CWnd)
	ON_COMMAND(ID_INBLOCK,OnInBlock)
	ON_MESSAGE(WM_RICHEDIT_MESSAGE,OnRichEditMessage)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_COMMAND_RANGE(ID_TDX_JBZL,ID_JBM_MENU8,OnSelectJBM)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TDX_JBZL,ID_JBM_MENU8,OnUpdateSelectJBM)
	ON_COMMAND_RANGE(IDC_BASEF10ID,IDC_BASEF10ID_END,OnClickFLButton)
	
	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnUpdateMarkStockFlag)
	//{{AFX_MSG_MAP(CTxtView)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_RIGHT_ADDBLOCK, OnRightAddblock)
	ON_COMMAND(ID_RIGHT_ADDZXG, OnRightAddZxg)
	ON_COMMAND(ID_DELFROMBLOCK,OnDelFromBlock)
	ON_COMMAND(ID_OTHERJBM, OnOtherjbm)
	ON_UPDATE_COMMAND_UI(ID_OTHERJBM, OnUpdateOtherjbm)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
int CUF10CtrlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pUf10->OnCreate();
	m_bLayoutFlag = TRUE;
	return 0;
}

//响应"股票资料"的消息
void CUF10CtrlWnd::OnRichEditMessage(WPARAM wParam, LPARAM lParam)
{
	if(m_pUf10)
		m_pUf10->OnRichEditMessage();
}

void CUF10CtrlWnd::OnDestroy() 
{
	m_bQuitting = TRUE;
	CWnd::OnDestroy();	
}

void CUF10CtrlWnd::OnClickFLButton(UINT nID)
{
	m_pUf10->OnClickFLButton(nID);
}


//改变JBM配置
void CUF10CtrlWnd::OnSelectJBM(UINT nID)
{
	m_pUf10->OnSelectJBM(nID);
	Resize();
	SendMessage(WM_RICHEDIT_MESSAGE);
}

void CUF10CtrlWnd::OnUpdateSelectJBM(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_pUf10->GetCurrentJBM()  == (pCmdUI->m_nID - ID_JBM_MENU1));
}

void CUF10CtrlWnd::Resize()
{
	if(!m_bLayoutFlag) return;
	m_pUf10->Resize();
	Invalidate(TRUE);
}

void CUF10CtrlWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	Resize();
}

void CUF10CtrlWnd::OnPaint() 
{	
	CPaintDC dc(this);
	m_pUf10->OnDraw(&dc);
}

BOOL CUF10CtrlWnd::OnEraseBkgnd(CDC* pDC) 
{
	return m_pUf10->OnEraseBkgnd(pDC);
}

void CUF10CtrlWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonUp(nFlags, point);
}

void CUF10CtrlWnd::OnInBlock()
{
	int tempCurrentNo = m_pUf10->GetCurrentGPIndex();
	if(tempCurrentNo == -1)	return;
	AfxGetMainWnd()->SendMessage(UM_SEARCH_BLOCK,tempCurrentNo);
}

void CUF10CtrlWnd::OnRightAddblock() 
{
	Func_RightAddblock(m_pUf10->GetCurrentGPIndex(),TRUE);
}

void CUF10CtrlWnd::OnRightAddZxg()
{
	Func_RightAddZxg(m_pUf10->GetCurrentGPIndex(),TRUE);
}

void CUF10CtrlWnd::OnDelFromBlock()
{
	Func_DelFromBlock(m_pUf10->GetCurrentGPIndex(),TRUE);
}

void CUF10CtrlWnd::OnOtherjbm() 
{
	if(g_TdxCfg.JbmNum == 1)
		OnSelectJBM(ID_JBM_MENU1);
}

void CUF10CtrlWnd::OnUpdateOtherjbm(CCmdUI* pCmdUI) 
{
	if(g_TdxCfg.JbmNum == 1)
		pCmdUI->SetCheck(m_pUf10->GetCurrentJBM()==0);
	else if(g_TdxCfg.JbmNum == 0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);		
}

void CUF10CtrlWnd::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	m_pUf10->OnGetDataAck(wParam, lParam);
}

void CUF10CtrlWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!(m_bLayoutFlag && OnLine && g_bHasF10Cfg && m_pUf10->GetCurrentJBM() >= 0))
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}
	
	m_pUf10->OnLButtonDown(nFlags, point);
	CWnd::OnLButtonDown(nFlags, point);
}

void CUF10CtrlWnd::OnCancelMark()
{
	int nCurGpIndex = m_pUf10->GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	MarkStockFlag(tempStkInfo, 0);
}

#include "TipMarkDlg.h"
void CUF10CtrlWnd::OnMarkStockFlag(UINT nID)
{
	int nFlag = nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = m_pUf10->GetCurrentGPIndex();
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

void CUF10CtrlWnd::OnUpdateMarkStockFlag(CCmdUI* pCmdUI)
{
	char nFlag = pCmdUI->m_nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = m_pUf10->GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	pCmdUI->SetCheck(nFlag==tempStkInfo->fl_flag);
}
