#if !defined(AFX_QJSTATDLG_H__7D33D4F4_6CDC_4716_A31D_C2A4B1753FCB__INCLUDED_)
#define AFX_QJSTATDLG_H__7D33D4F4_6CDC_4716_A31D_C2A4B1753FCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QjStatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQjStatDlg dialog

class CQjStatDlg : public CDialog
{
// Construction
public:
	CQjStatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQjStatDlg)
	enum { IDD = IDD_QJSTAT_DIALOG };
	CString	m_strAmount;
	CString	m_strClose;
	CString	m_strHsl;
	CString	m_strMax;
	CString	m_strMin;
	CString	m_strOpen;
	CString	m_strTime1;
	CString	m_strValue;
	CString	m_strVolume;
	CString	m_strJunJa;
	CString	m_strZhangFu;
	CString	m_strZhenFu;
	CString	m_strDataNum;
	CTime	m_time1;
	CTime	m_time2;
	//}}AFX_DATA
	void	SetDataAndPeriod(MemStkInfo *pStkInfo,struct AnalyData *pData,int ItemNum,int nFirstPos,int nEndPos,BYTE Period);
	float	GetOneHsl(long lDate,float tmpVolume,int vol_factor);
private:
	BOOL				m_bPhaseEnable;
	BOOL				m_bZs,m_bQz;

	struct tag_InfoDlg	m_Info;
	BYTE				m_nPeriod;
	MemStkInfo *		m_pStkInfo;
	struct AnalyData *	m_pAnalyData;
	int					m_nItemNum;
	int					m_nFirstPos,m_nEndPos;

	void 				GetData(int nFirstPos,int nEndPos);
	void				ShowIt();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQjStatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQjStatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechangeDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBoardsort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QJSTATDLG_H__7D33D4F4_6CDC_4716_A31D_C2A4B1753FCB__INCLUDED_)
