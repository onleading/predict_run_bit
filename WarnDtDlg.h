#if !defined(AFX_WARNDTDLG_H__85040F79_1117_4E58_A9F2_0A2F994368DD__INCLUDED_)
#define AFX_WARNDTDLG_H__85040F79_1117_4E58_A9F2_0A2F994368DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WarnDtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWarnDtDlg dialog
//预警类型
#define WARN_NONE			0
#define WARN_LINE			0x0001				//拖拉机单(A,B,C:A+C=B+B,A-B=D*10^N,abs(D)<10)
#define WARN_ORDERSHOCK		0x0002				//委托异动(大委大撤)
//
#define ORDERSHOCK_BORDER	0x00
#define ORDERSHOCK_BCANCEL	0x01
#define ORDERSHOCK_SORDER	0x02
#define ORDERSHOCK_SCANCEL	0x03

class CWarnDtDlg : public CDialog
{
// Construction
public:
	CWarnDtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWarnDtDlg)
	enum { IDD = IDD_WARNDT_DIALOG };
	CEdit	m_Code;
	CStatic	m_Static;
	CComboBox	m_WarnType;
	CButton	m_EnterBtn;
	CButton	m_ExportBtn;
	CButton	m_StartBtn;
	CListCtrl	m_List;
	CString	m_CodeStr;
	//}}AFX_DATA
	//对话框定位
	int m_nListLeft,m_nListTop,m_nListRight,m_nListBottom;
	int m_nBtnTop,m_nBtnHeight,m_nComHeight,m_nBtnWidth,m_nComWidth,m_nStaticWidth,m_nEditWidth;
	int m_nStartLeft,m_nExportLeft,m_nEnterLeft,m_nComLeft,m_nStaticLeft,m_nEditLeft;
	//预警内容
	MemStkInfo	*m_pStkInfo;
	int			m_nWarnFlag;	//过滤类型
	int			m_nShowNum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWarnDtDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void FillList();
	void FillListHead();
	// Generated message map functions
	//{{AFX_MSG(CWarnDtDlg)
	virtual void OnCancel();
	afx_msg void OnDtwarnStart();
	afx_msg void OnButtonExport();
	afx_msg void OnButtonEnter();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSelchangeComboWarntype();
	afx_msg void OnDblclkDtwarnList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WARNDTDLG_H__85040F79_1117_4E58_A9F2_0A2F994368DD__INCLUDED_)
