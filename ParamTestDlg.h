#if !defined(AFX_PARAMTESTDLG_H__C04AB601_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_)
#define AFX_PARAMTESTDLG_H__C04AB601_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParamTestDlg.h : header file
//
#pragma warning(disable:4786)
#pragma warning(disable:4788)
#include <map>
using namespace std;

#include "Label.h"
#include "interface.h"
#include "DragListCtrl.h"
#include "GradientProgressCtrl.h"
#include "MyDetailDlg.h" 
#include "AllTestMapDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CParamTestDlg dialog
#define CALC_ALL    0
#define CALC_SINGLE 1

class CParamTestDlg : public CDialog
{
// Construction
public:
	CParamTestDlg(CWnd* pParent = NULL);   // standard constructor
	~CParamTestDlg();
	int m_nTestType;
	int m_nAddedGPIndex[MAXGPNUM];
	int m_nAddedGPNum;
	CMyDetailDlg *m_pShowDetail;
//	CAllTestMapDlg *m_pAllTestMap;

// Dialog Data
	//{{AFX_DATA(CParamTestDlg)
	enum { IDD = IDD_TESTPARAMDLG };
	CButton	m_showMore;
	CEdit	m_GPTotal;
	CLabel	m_OutInfo;
	CComboBox	m_AimCombo;
	CComboBox	m_PeriodCombo;
	CComboBox	m_NameCombo;
	CDragListCtrl	m_List;
	CGradientProgressCtrl	m_Progress;
	CTime	m_BeginTime;
	CTime	m_EndTime;
	CString	m_sParam1;
	CString	m_sParam2;
	CString	m_sParam3;
	CString	m_sParam4;
	CString	m_sParam5;
	CString	m_sParam6;
	int		m_nDontNeedFee;
	int		m_nOptimizeParam;
	int		m_nTestPeriod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamTestDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParamTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDesc();
	afx_msg void OnAddgp();
	afx_msg void OnDelgp();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnOparam();
	afx_msg void OnTestparam();
	afx_msg void OnShowmore();
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowdetail();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelgpall();
	afx_msg void OnShowggdetail();
	afx_msg void OnModgs();
	//}}AFX_MSG
	void SingleDetail(void);
	void OnSelShowMore1(UINT nID);
	void OnSelShowMore2(UINT nID);
	DECLARE_MESSAGE_MAP()
private:
	map<long, testDetail, less_equal<long> > m_allTestDetails;
	vector<testDetail> m_sTestDetails;
	vector<testDetail> m_aDetails;
	/*******�ܽ�������ӯ��ϵ������ͨ������õ�*********/
	float           m_fTJBonus[8];
	BOOL            m_showhat[13];   //���������Ŀ
	float			m_curBonus;     //��ǰ����������
	float			m_fMarkup;		//ͬ���Ƿ�
	float			m_fTotalBonus;	//�������
	float			m_fAverageBonus;//ƽ������
	float			m_fDivHeadBonus;//ȥ������
	float			m_fAverageYL;	//ӯ��ƽ��
	float			m_fAverageKS;	//����ƽ��
	float			m_fMaxBonus;	//�������
	float			m_fMaxKS;		//������
	float			m_YKCoeff;		//ӯ��ϵ��
	int             m_toTimes;      //���״���
	int				m_nYLTimes;		//ӯ������
	int				m_nKSTimes;		//�������
	/**************************************************/
	float			*m_pCache;		//����ÿ�μ�����ۼ�ֵ
	int				m_nCalcPeriod;	//�����������(����,����...)
	LPHISDAT		m_pAnalyData;	//��������
	short			m_fParaValue[MAX_PARAMNUM];//�������ʱ�ĸ�������ȡֵ
	BOOL			m_IsCarrying;	//����ִ�еĹ�����(��Ҫ������ǿ�ƹر�������ʱ֪ͨ���ر���
	BOOL			m_IsCarried;	//��ִ�����
	CImageList		m_ImageList;	//��������m_Listʹ�õ�ͼ��
	TCHAR			m_crCurrGPCode[10];	//��ǰ��Ʊ����
	short			m_crCurrSetCode;
	int				m_nCurrPos;		//������¼��ǰListCtrl�е�ӵ�н����λ��
	void            FillLvc(void);
	void			FillGPReport(void);
	void            SetParam(void);
	void            OptimizeParam(void);
	void            CalcWithParam(void);
	double           getsxf(const char *code,short setcode);
	BOOL			PeekMyMsg(void);//��Ϣѭ��,��ֹ���涳ס
	void			DisableOther(void);
	void			EnableOther(void);
	void			FindOptParam(int nParam, float *aParam);
	void            CalcTJForm(float **OutData, long lReadNum, int nType, TINDEXINFO *pIndex);
	void            CalcValues(LPHISDAT pHisData,float *pValues,short nData);
	void			CalcBonus(TINDEXINFO *pIndex,long ReadNum, int nType=0,  long optIndex=-1);
	void            CalcFormula(float **OutData, long lReadNum);
	void			CalcFormula0(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);
	void			CalcFormula1(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);
	void			CalcFormula2(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);
	void			CalcFormula3(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);
	void			CalcFormula4(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);
	void			CalcFormula5(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);
	void			CalcFormula6(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);
	void			CalcFormula7(float **OutData, long lReadNum, TINDEXINFO *pIndex=NULL);

	void			CalcHighLowEx(float &high, float &low, const int nPreBuy, const int nNowSell, const long lReadNum);
	void            CalcDetails(float **OutData, long lReadNum, short iGp, short calcType);
	void            CalcTjDetails(float **OutData, long lReadNum);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMTESTDLG_H__C04AB601_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_)
