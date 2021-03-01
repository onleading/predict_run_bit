#if !defined(AFX_BKDATADLG_H__619750F1_D7B0_4BE3_A9CB_D9B8D8D4463F__INCLUDED_)
#define AFX_BKDATADLG_H__619750F1_D7B0_4BE3_A9CB_D9B8D8D4463F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BkDataDlg.h : header file
//

#include "BkData.h"

#define BK_TIMEOUT_SECONDS		20

class CBkDataDlg : public CDialog
{
// Construction
public:
	CBkDataDlg(CWnd* pParent = NULL);   // standard constructor
	void SetBkDataObject(CBkData *pBkData);
// Dialog Data
	//{{AFX_DATA(CBkDataDlg)
	enum { IDD = IDD_BKDATA_DIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBkDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
private:
	int		data_filelen;
	int		data_pos;
	char *	FileBuffer;
	char	m_strSyncFile[100];
	char	m_strSyncSaveFile[100];
protected:
	BOOL				m_bOneProcessing;
	CBkData	*			m_pBkData;
	struct BkData_Req	m_CurReq;

	short				m_nMainID;
	BOOL				m_bGetDSData;
	short				m_nCurGPNo;

	void				ProcessOneSubscrible();
	void				AfterGetData();
	void				SetTimeOutTimer();
	void				StopTimeOutTimer();
	void				ContinueWant();
	BOOL				TryFileLenReq();
	BOOL				TryIWTitleReq();
	BOOL				TryExtTAReq(BOOL bTA2,int nDateNum);
	// Generated message map functions
	//{{AFX_MSG(CBkDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BKDATADLG_H__619750F1_D7B0_4BE3_A9CB_D9B8D8D4463F__INCLUDED_)
