#if !defined(AFX_LOGODLG_H__224ADFD0_EB8D_4CC0_BCB9_3AC9853280FF__INCLUDED_)
#define AFX_LOGODLG_H__224ADFD0_EB8D_4CC0_BCB9_3AC9853280FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogoDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "Label.h"
#include "HyperLink.h"
#include "GradientProgressCtrl.h"
#include "MyBitmapBt.h"
#include "SpeedTestDlg.h"
#include "webbrowser2.h"

#include "SafeEdit.h"
#include "SafeComboBox.h"

#define AUTOBASE_COUNT	100
#define AUTOGBBQ_COUNT	40
#define YZMLENGTH		4

struct BakHostInfo
{
	TDXWHOST BakHost;
	char	 WhichHost;
};

class DrawInterface
{
public:
	CBitmap m_bmWnd;
	CWnd *m_pParentWnd;
public:
	CRect m_CloseBtRect;
	CRect m_Bt0Rect,m_Bt1Rect,m_Bt2Rect;
	CRect m_Bt3Rect,m_Bt4Rect,m_Bt5Rect;
	CRect m_Bt6Rect,m_Bt7Rect;
	CRect m_Static0Rect,m_Static1Rect,m_Static2Rect,m_StatusStaticRect;
	CRect m_comboboxRect1,m_comboboxRect2,m_comboboxRect_yyb,m_comboboxRect3,m_edit0Rect,m_edit1Rect,m_edit2Rect;
	CRect m_Bt8Rect,m_Bt9Rect;
	CRect m_CheckSaveUsernameRect,m_CheckProtectUsernameRect;
	CRect m_BtOffLineRect;
	CRect		m_yzmrect,m_L2CfgRect,m_HelpURLRect;
	CMyStatic	m_Yzm_Static,m_L2Cfg_Static,m_HelpURL_Static;
	CMyStatic m_Static0,m_Static1,m_Static2,m_Static3,m_StaticStatus,m_StaticSaveUsername,m_StaticProtectUsername;
public:
	DrawInterface();
	void Init();
	void GetWndRgn(CRgn &wndRgn);
	void OnInitDialog(CWnd *pParentWnd=NULL);
	void OnEraseBkgnd(CDC *pDC,CRect &rc);
};

//////////////////////////////////////////////////////////////////////////
class CLogoDlg : public CDialog
{
	void GetFromUserHostCfg();
	void InitOtherLogoInfo();
	void AfterLoginTypeSel();
	void Reset();
	void EnableOther(BOOL bEn);
	BOOL NeedTranInfo(int which);
	BOOL SaveIt(int which);
	BOOL TryBlockDatReq();
	BOOL TryZHBZipReq();
	BOOL TryJGDataReq();
	BOOL TryOtherFileReq();
	BOOL TryBaseReq();
	BOOL TryGbbqReq();
	void WriteOrAppend(DBH_Head *pDbf,MemStkInfo *pStkInfo,BOOL bWrite,int nWhich=1);
	BOOL SaveToBaseDbf();
	void SaveMapFile(BOOL bBase);
	void NormalEnd();
	BOOL SwitchNextHost();
	BOOL ProcessUserHost();
	void EnableEmbedWTSel();
	void _EnableCommbox(BOOL bEn);
	void _SetStatusTxt(LPCSTR str);
	void Offline();
	void GenYzm();
	BOOL CheckYzm(int nLength,char* yzm,char *str);
	void SetNewYzm();
	void SetEditPSAttribute(CSafeComboBox *pCombo,BOOL PSStyle);
	void GetTCJyInfo();	

	void DrawLogoBmp(CDC *pDC,CRect rect,HANDLE hOEMBitmap);
public:
	CLogoDlg(CWnd* pParent = NULL);   // standard constructor
	~CLogoDlg();

#ifdef OEM_NEWJY
	CRgn m_wndRgn;
	DrawInterface m_draw;
	MyBitmapBt *m_pCloseBt;
	MyBitmapBt *m_pCfgBt,*m_pHelpBt,*m_pSysTestBt;
	MyBitmapBt *m_pSel1Bt,*m_pSel2Bt,*m_pSel3Bt;
	MyBitmapBt *m_pOKBt,*m_pCancelBt,*m_pCABt,*m_pSoftKeyBt;
	MyBitmapBt *m_pOffline;

	//安全控件
	CSafeComboBox	m_embzh_Combo;
	CSafeEdit		m_edit1;
	CSafeEdit		m_edit2;
	
	BOOL		m_bShowYzm;
	char		m_yzm[YZMLENGTH];
	CString		m_strYzm;
#endif
	BOOL  m_bOnlyLoginWT;
	CSpeedTestDlg *m_pSpeedTestDlg;

// Dialog Data
	//{{AFX_DATA(CLogoDlg)
#ifdef OEM_STAR
	enum { IDD = IDD_STAR_LOGO };
#elif OEM_STAR_HM
	enum { IDD = IDD_STAR_NHM_LOGO };
#elif PERSONAL
	enum { IDD = IDD_QD_LOGO };
#elif LEVEL2
	enum { IDD = IDD_QD_LOGO };
#elif OEM_NEWJY
	enum { IDD = IDD_LOGO_NEWJY };
#else
	enum { IDD = IDD_LOGO_DIALOG_A };
#endif
	CComboBox	m_Aqfs_Combo;
	CButton	m_ConnectCFGButton;
	CButton m_SaveZHButton;
	CButton m_ProtectZHButton;
	CEdit	m_PassEdit;
	CHyperLink  m_SpeedTestLink;
	CComboBox	m_LoginType_Combo;
	CComboBox	m_LoginType_Combo_yh;
	CGradientProgressCtrl	m_Progress;
	CComboBox	m_HqHost_Combo;
	CLabel	m_Status_Static;
	CString	m_strPassword;
	BOOL	m_bSavePass;
	BOOL	m_bNLB;
	CString	m_strUserName;
	int		m_nMode;
	BOOL	m_bSaveZH;
	BOOL	m_bProtectZH;
	//}}AFX_DATA
	
	CString		m_L2UserName;
	CString		m_L2Password;
	int			m_nL2Type;
	
	CHyperLink	m_RegExit, m_RegLogin, m_ProxyInfo;
	CHyperLink	m_StarLink1;
	CHyperLink	m_StarLink2;
	CHyperLink	m_StarLink3;
	CHyperLink	m_StarLink4;
	CTabCtrl	m_Tab;

	short		m_nMainID;
	BOOL		m_bHasOffMode;
	BOOL		m_bDisableNLB;
	int			bFirstUsedHostIndex;
	int			UsedHostIndexNum;

	BOOL		m_bDirectConnect;
	BOOL		m_bSecondConnect;
	long		szinfdate,shinfdate,szinfhms,shinfhms;	//主站端INF的修改日期和时分秒
	HICON		m_hYhzqIcon,m_hZxjtIcon;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogoDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	HICON	m_hIcon;
	CImageList m_imagelist;
	short	szstknum,shstknum,szstartxh,shstartxh;
	struct	remote_TmpStkInfo3 *m_pSZTmp,*m_pSHTmp;
	struct  MemStkInfo	*m_pSZMem,*m_pSHMem;
	
	long	blockdat_filelen,blockdat_pos,zhbzip_filelen,zhbzip_pos,jgdata_filelen,jgdata_pos,otherfile_filelen,otherfile_pos;
	char	*FileBuffer;

	long	basedifnum,gbbqdifnum;	//基本资料和股本变迁不同的记录数
	long	basestartxh,gbbqstartxh;
	struct  tag_autopos
	{
		short setcode;	//在那个MemStkInfo中
		short pos;		//第几只股票
	}basepos,gbbqpos;

	HANDLE  hOEMBitmap;
	BOOL	m_bAuthSuccess;

	CBitmap m_bitmap_i;
	HANDLE  hBitmap_i;

	int		m_nTry3Num;
	int		m_nReConnectNum;

	BOOL	m_bHasPackTimer,m_bHasPackTimer3;

	BOOL	m_bFirstShow;
	struct BakHostInfo m_BakHostInfo;
	BOOL	m_bEnableNLB;

	CRect	m_OEMBmpRect;

	CWebBrowser2   *m_pADIE;
	int				m_SoftFocus;
	// Generated message map functions
	//{{AFX_MSG(CLogoDlg)
	afx_msg LRESULT OnCheckHexinWT(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCheckTDXWT(WPARAM wParam,LPARAM lParam);
	afx_msg void OnConnectcfgButton();
	afx_msg void OnProxycfgButton();
	afx_msg void OnLevel2Cfg();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGetDataACK(WPARAM wParam,LPARAM lParam);
	afx_msg void StartLogin(WPARAM wParam,LPARAM lParam);
	afx_msg void StartConnect(WPARAM wParam,LPARAM lParam);
	afx_msg void OnConnectResult(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSelchangeHqhostCombo();
	afx_msg void OnOffline();
	afx_msg void OnCA();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLogoEnableMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void SwitchNextHostMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeLogintypeCombo();
	afx_msg void OnSpeedTest(WPARAM wParam,LPARAM lParam);
	afx_msg void OnNLBCheckClick();
	afx_msg void OnSelchangeComboAqfs();
	afx_msg void OnSoftkeyMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnSetfocusZhInfo();
	afx_msg void OnSetfocusPassword();
	afx_msg void OnSetfocusAuthpwd();
	afx_msg void OnKillfocusGddmCombo();
	afx_msg void OnCheckProtectZH();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On_Jy_RegUser();
	afx_msg void On_Jy_ActiveUser();
	afx_msg void OnRegExitMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRegLoginMessage(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGODLG_H__224ADFD0_EB8D_4CC0_BCB9_3AC9853280FF__INCLUDED_)
