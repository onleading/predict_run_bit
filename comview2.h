#if !defined(AFX_COMVIEW2_H__65DE8561_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_COMVIEW2_H__65DE8561_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "macro.h"
#include "struct.h"
#include "globalvar.h"
#include "ComDoc.h"
#include "UnitView.h"
#include "custompad.h"
#include "UFxt.h"
#include "UZst.h"
#include "UHq.h"
#include "UFjb.h"
#include "UFsb.h"
#include "UTick.h"
#include "UCdp.h"
#include "UMmp.h"
#include "UCb.h"
#include "UBigVol.h"
#include "URadar.h"
#include "UHq10.h"
#include "UTick2.h"
#include "UQueue.h"
#include "UTickStat.h"
#include "UWTRank.h"
#include "USCancel.h"
#include "UTCancel.h"
#include "UHycj.h"
#include "UTVGraph.h"
#include "UTickZst.h"
#include "MySwitchBar.h"

#define MAX_NODE			1024
#define CURSOR_WIDTH		2
#define TOPSWITCH_HEIGHT	20

#define HQGRID_REQ			99		//Comview2行情UNIT
#define MAX_COM2_GP			20		//最大的组数

#define ID_SWITCHBAR_H	WM_USER+0x2301	
#define ID_SWITCHBAR_V	WM_USER+0x2302
//////////////////////////////////////////////////////////////////////////
//Unit Property
#define BE_VALID_UNIT		0x00000001
#define BE_INVALID_UNIT		0x00000002
#define BE_NOHIDE_UNIT		0x00000004
#define BE_HIDE_UNIT		0x00000008
#define BE_NONULL_UNIT		0x00000010
#define BE_NULL_UNIT		0x00000020
#define BE_CHANGEGP_UNIT	0x00000040
#define BE_NOCHANGEGP_UNIT	0x00000080
#define BE_CONTRALL_UNIT	0x00000100
#define BE_NOCONTRALL_UNIT	0x00000200
#define BE_LOCKED_UNIT		0x00000400
#define BE_UNLOCKED_UNIT	0x00000800
#define BE_GPINAC_UNIT		0x00001000
#define BE_GPNOINAC_UNIT	0x00002000
#define BE_DS_SC			0x00004000
#define BE_NODS_SC			0x00008000
#define BE_DS_UNIT			0x00010000
#define BE_NODS_UNIT		0x00020000
#define BE_HASMENU_UNIT		0x00040000
#define BE_NOMENU_UNIT		0x00080000
#define BE_SEE_UNIT			0x00100000
#define BE_NOSEE_UNIT		0x00200000

void	SetStrMap(void);
UnitTreeNode* FindLeaf(UnitTreeNode* pNode, int nLOrR);
/////////////////////////////////////////////////////////////////////////////

class CComView2 : public CUnitView
{
protected:
	CComView2();
	DECLARE_DYNCREATE(CComView2)
public:
	CComDoc* GetDocument();
protected:
	UnitTreeNode* m_apNode[MAX_NODE];
	UnitTreeNode *m_pRootUnit;		//UNIT树
	vector<SplitManInfo> m_aSplitInfo;
	SplitManInfo*	m_pMovingSplit;
	vector<SplitActInfo> m_apSplitSeq;

	CMySwitchBar	m_CustomPad_Bar;
	int				m_nCurSel;

	int		m_WhichGPType[MAX_NODE], m_nBlockFlag[MAX_NODE], m_bJSZJFlag[MAX_NODE], m_bSignalFlag[MAX_NODE], m_nConFlag[MAX_NODE],m_nConFiltCount[MAX_NODE],m_nSetFlag[MAX_NODE];		//HQGrid的板块信息
	int		m_tmpNf;												//

	BOOL	m_bInSplit;
	BOOL	m_Fixed;

	short	m_aCurGPSet[MAX_COM2_GP];			//当前所分析股票集合
	short	m_aLastCUnit[MAX_COM2_GP];			//上次控制组的党员
	short	m_nSZZSIndex,m_nSHZSIndex;
	UINT	m_nNowPad;				//当前的版面类型
	char	m_Extern_GPSwitch;		//是否有外部股票切换
	char    m_PadName[40];			//中文名
	char	m_PadCode[16];			//英文名
	DWORD	m_NowPadSet;
	DWORD	m_NowPadCmdID;

	int		m_oldhit;
	int		m_bCanMoveSplit;
	BOOL	m_bMoving;
protected:
	//Help Functions
	inline	int		Index2NodeNo(int iIndex);
	inline	BOOL	TestUnitProperty(int iUnit, UINT nFlag);
	
	void	OnSwitchCustomPad();
	void	InsertCustomPadSwitch(int nCurSel);
	
	void    GetDefaultGP(CString padini);
	void    WriteDefaultGP(CString padini);
	void	WritePadInfo(int nCmdNum);
	BOOL	GetCustomPadInfo(char *m_PadName);
	void	WriteCustomPadInfo(int nCmdNum);

	void	InitMenuMap(void);										//初始化菜单
	void	CreateUnit(int initGPNo,int type,int style,int nUnit,UBase **pUnit0);//构造一个Unit
	BOOL	ChangeUnitGP(UBase *pUnit,int iNo);

	void	ShowHideDynBtn();
	void	SetComTitle();
	void	PaintNullUnit(int nUnit);

	BOOL	HasVarGPIndex(int nVar_GP);
	void	RefreshCurrPADHQ();

	void	ConstructPad();

	void	SetUnitReadAndPaint(int i,CRect rc,int flag=0);
	void	ResizePad(int flag=COM_OnlyDraw);

	int		HitTest(CPoint point);

	void	ReadAndPaint(int allread=1,int Var_GPIndex=0);
	void	OnReadAndPaint(WPARAM wParam,LPARAM lParam);
	void	DrawBorder();

	void	DispatchMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	BOOL	GetCustomPadInfo(void);						//由m_apNode组织m_Units
	void	ConstructFirstUnit(int newUnit, int style, int nGpNo, BOOL bContrallUnit, BOOL bLocked);
	BOOL	SplitPad(int nWhich, int newUnit, int style, int splitType=1, float fToSplit=50.0f, int nGpNo=GPINDEX_VAR1, BOOL bContrallUnit=FALSE, BOOL bLocked=FALSE);
	BOOL	SplitPad(UnitTreeNode *nodeInTree, int newUnit, int style, int splitType, float fToSplit, int nGpNo, BOOL bContrallUnit, BOOL bLocked);
	void	SwapUnit(int ua, int ub, BOOL bSwapSplit = FALSE);

	void	SplitRect(UnitNodeInfo *lNode, UnitNodeInfo *rNode, UnitNodeInfo *pNode);
	void	ReJustJHMin(UnitNodeInfo *pNode);
	void	ReSplitPad(UnitTreeNode* pNode, int flag);			//重新分割部分PAD,从某父节点起
	UnitTreeNode* 	HideOneUnit(UnitTreeNode* pNode);
	void	GetSplitInfo(UnitTreeNode* pNode);					//保存现有PAD的分割信息
	BOOL	Proc_Special_MessageCode(int tmpMessageCode, int &unitID);

	void	SavePadInfo(int nCmdNum = -1);
	void	OnUpdateSavepad(CCmdUI* pCmdUI);

	void	DestroyTree(void);

	int		GetNextGP(int nGPIndex, BOOL bNext=TRUE);
	BOOL	InGroupGP(int nGpIndex);
public:
	void	OnF5();
	void	SetControlSize();
	int		GetCurrentGPIndex();
	int		GetGP_NeedRefreshStatHQ(short *lpnStkIndex,int nMaxNum,int nType);
	UINT	GetPadType() { return m_nNowPad; }
	int		GetUnitNum() { return m_Units.Num; }

	DWORD	GetNowPadSet() { return m_NowPadSet; }
	DWORD	GetNowPadCmdID() { return m_NowPadCmdID; }
	
	BOOL	GetBeInSplit() { return m_bInSplit; }

	BOOL	HasNoFixedHQGrid();

	BOOL	Get_Extern_GPSwitch();
	int		EscHandle();

	void	SetCurPadGP(int iNo);
	BOOL	ChangeSyncUnitGP(int setGPIndex);	//同步更换股票

	BOOL	m_bDS;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComView2)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~CComView2();
protected:
	//{{AFX_MSG(CComView2)
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnBatch();
	afx_msg void OnGotojbzl();
	afx_msg void OnChangeGPType();
	afx_msg void OnAutoPage();
	afx_msg void OnUpdateRightClickHead(CCmdUI* pCmdUI);

	afx_msg void OnTypeID(UINT nID);
	afx_msg void OnBlockID(UINT nID);
	afx_msg void OnDSMarket(UINT nID);
	//////////////////////////////////////////////////////////////////////////
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnInsertUnit(UINT nID);
	afx_msg void OnUpdateInsertUnit(CCmdUI *pCmdUI);
	
	afx_msg void OnSplitPad(UINT nID);
	afx_msg void OnUpdateSplitPad(CCmdUI *pCmdUI);
	afx_msg void OnDeleteunit();
	afx_msg void OnUpdateDeleteunit(CCmdUI* pCmdUI);
	afx_msg void OnSavepad();
	afx_msg void OnContrallunit();
	afx_msg void OnUpdateContrallunit(CCmdUI* pCmdUI);
	afx_msg void OnSetGpVar(UINT nID);
	afx_msg void OnUpdateSetGpVar(CCmdUI* pCmdUI);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnNewpad();
	afx_msg void OnUpdateNewPad(CCmdUI* pCmdUI);
	afx_msg void OnBeginsplit();
	afx_msg void OnUpdateBeginsplit(CCmdUI* pCmdUI);
	afx_msg void OnLockunit();
	afx_msg void OnUpdateLockunit(CCmdUI* pCmdUI);
	afx_msg void OnDeletecurpad();
	afx_msg void OnUpdateDeletecurpad(CCmdUI* pCmdUI);
	afx_msg void OnLiminWidth();
	afx_msg void OnUpdateLiminWidth(CCmdUI* pCmdUI);
	afx_msg void OnLiminHeight();
	afx_msg void OnUpdateLiminHeight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetunitoption(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChangeGpType(CCmdUI *pCmdUI);
	afx_msg void OnSetunitoption();
	afx_msg void OnOpenURL();
	afx_msg void OnChangeZhSortSC(UINT nID);
	afx_msg void OnUpdateChangeZhSortSC(CCmdUI *pCmdUI);

	afx_msg void OnRightClickRow(UINT nID);
	afx_msg void OnUpdateRightClickRow(CCmdUI* pCmdUI);
	
	afx_msg void OnLinkage();
	afx_msg void OnUpdateLinkage(CCmdUI* pCmdUI);

	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	afx_msg void OnUpdateMarkStockFlag(CCmdUI* pCmdUI);

	afx_msg void OnUpdateDYBlock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMDFHkHyClick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDSMarket(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHYBlock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConceptClick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZHBClick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuBlockClick(CCmdUI *pCmdUI);
	afx_msg void OnUpdateZJHHYClick(CCmdUI *pCmdUI);
	afx_msg void OnUpdateClickCorner(CCmdUI *pCmdUI);
	//}}AFX_MSG
	afx_msg void OnRefreshCustomBarMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSwitchBarMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void ConstructPadMessage(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CComDoc* CComView2::GetDocument()
   { return (CComDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_COMVIEW2_H__65DE8561_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_)
