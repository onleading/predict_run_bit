#if !defined(AFX_MOVINGINFODLG_H__02AFE9F1_8D19_4A77_A844_8D6FA5088AB0__INCLUDED_)
#define AFX_MOVINGINFODLG_H__02AFE9F1_8D19_4A77_A844_8D6FA5088AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MovingInfoDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "iconbutton.h"
#include "struct.h"

struct POS  //ĳһ�����ֵ���ʼλ�úͽ���λ��
{
	int head;
	int tail;
};

class CMainFrame;

//�������ֵ���ʾԭ��
/*
  �������ִ�������ʾ�������ǾͰ����е����ּ����ų�һ�ţ�������ÿһ��Ҫ��ʾ�����ֶ���һ����ʼ���꣬���Ƕ���һ�����Σ�
  ������δ��������ֵ��������ƣ�������ֵ��֣���β����������ڣ���Ӧ����ʾ������
  ���ϵ���Ҳһ��
 */

class CMovingInfoDlg : public CDialog
{
// Construction
public:
	void AddSysInfo(SYS_INFO si);
	void DeleteAllSysInfo();
	void InitNext();
private:
	void SetScrollStyle(bool bEnableStop);
	void SetScrollInfo(bool bShowSysInfo);
	void SetScrollDirection(bool bLeftToRight);
	void ArrangeCtrl();
	int	  m_iHeight;//�Ի���߶�
	bool m_bMaximized;//�Ƿ������״̬
	bool m_bLeftToRight;//�Ƿ��������ʾ,���Ǵ����Ҿ��Ǵ��µ���,����ʾ������Ϣʱֻ�ܴ��µ���
    bool m_bShowSysInfo;//�Ƿ���ʾϵͳ��Ϣ,������ʾϵͳ��Ϣ������ʾ��Ʊ��Ϣ,
	bool m_bEnableStop;//��ʾ����Ƿ�ĳ�е����׺���ͣһ�� 
    bool m_bTextStopped;//�Ƿ����������������ֹͣ״̬
	int  m_iStopCount;//ֹͣ������ָ��Ӧ��ֹͣ�೤����ʱ������

	int	 m_nMovingSpaceTime,m_nMovingStopTime;
	vector<SYS_INFO>  m_vtSysInfo;
	vector<SYS_INFO>  m_vtNowSysInfo;
	vector<POS>       m_vtPos;

	POS  m_posNow;//�����ܹ����ü������ֵ���ʼλ�úͽ���λ��
	bool m_bRestart;//�Ƿ񱾴���ʾ�Ѿ�������Ӧ�ÿ�ʼ�´���ʾ��	
	
    int m_iTextBoxHeight;
    int m_iDlgWidth;

	CRect	m_rcSingleLine,m_rcMultiLine;
	CMainFrame * m_pMF;

	BOOL	m_bLayoutFlag;
public:	
	CMovingInfoDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CMovingInfoDlg)
	enum { IDD = IDD_MOVINGINFO };
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMovingInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMovingInfoDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg BOOL OnShowToolTip(UINT id,NMHDR *pTTTStruct ,LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVINGINFODLG_H__02AFE9F1_8D19_4A77_A844_8D6FA5088AB0__INCLUDED_)
