#if !defined(AFX_TESTZBDLG_H__C04AB602_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_)
#define AFX_TESTZBDLG_H__C04AB602_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestZBDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestZBDlg dialog
const int MAXCOUNT = 50;
#include "Label.h"
#include "interface.h"
#include "DragListCtrl.h"
#include "GradientProgressCtrl.h"

class CTestZBDlg : public CDialog
{
// Construction
public:
	CTestZBDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nAddedZBIndex[MAXCOUNT];
	int m_nAddedZBNum;

// Dialog Data
	//{{AFX_DATA(CTestZBDlg)
	enum { IDD = IDD_TESTZBDLG };
	CEdit	m_TestTotal;
	CEdit	m_GPTotal;
	CEdit	m_HaveDoneTotal;
	CLabel	m_OutInfo;
	CComboBox	m_AimCombo;
	CComboBox	m_PeriodCombo;
	CDragListCtrl	m_List;
	CGradientProgressCtrl	m_Progress;
	CTime	m_BeginTime;
	CTime	m_EndTime;
	int		m_nDontUseRecentData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestZBDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestZBDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectall();
	virtual void OnOK();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnRadio14();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int				m_nPeriod;		//�����������(����,����...)
	LPHISDAT		m_pAnalyData;	//��������
	float			m_fMaxBonus;	//������¼�ڲ�ͬ�ļ��������µ����bonus(��ͬ��bonus��˼������ͬ)
	int				m_nYLTimes;		//ӯ������
	int				m_nKSTimes;		//�������
	short			m_fParaValue[MAX_PARAMNUM];//�������ʱ�ĸ�������ȡֵ
	int				m_nCurrentGPNo;	//��ǰ��Ʊ���
	int				m_nCurrTestZB;	//��ǰ���ڲ��Ե�ָ��
	BOOL			m_bSelectAll;	//��ǰ�Ƿ���ȫѡ״̬
	BOOL			m_IsCarrying;	//����ִ�еĹ�����(��Ҫ������ǿ�ƹر�������ʱ֪ͨ���ر���
	BOOL			m_IsCarried;	//��ִ�����
	int				m_bSelected[MAXCOUNT];	//���浱ǰ��Щָ�걻ѡ��
	CImageList		m_ImageList;
	void			FillGPReport();	//���ø�item��ͼ��
	BOOL			PeekMyMsg(int TimeOut);	//��Ϣѭ��,��ֹ���涳ס
	void			DisableOther();
	void			EnableOther();
	void			CalcBonus(TINDEXINFO *pIndex,long ReadNum, int nType);
	void			CalcFormula0(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula1(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula2(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula3(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula4(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula5(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula6(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula7(float **OutData, long lReadNum, TINDEXINFO *pIndex);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTZBDLG_H__C04AB602_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_)
