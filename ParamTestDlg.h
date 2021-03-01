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
	/*******总交易数和盈亏系数可以通过计算得到*********/
	float           m_fTJBonus[8];
	BOOL            m_showhat[13];   //收益测试项目
	float			m_curBonus;     //当前参数的收益
	float			m_fMarkup;		//同期涨幅
	float			m_fTotalBonus;	//最大收益
	float			m_fAverageBonus;//平均收益
	float			m_fDivHeadBonus;//去顶收益
	float			m_fAverageYL;	//盈利平均
	float			m_fAverageKS;	//亏损平均
	float			m_fMaxBonus;	//最大收益
	float			m_fMaxKS;		//最大亏损
	float			m_YKCoeff;		//盈亏系数
	int             m_toTimes;      //交易次数
	int				m_nYLTimes;		//盈利次数
	int				m_nKSTimes;		//亏损次数
	/**************************************************/
	float			*m_pCache;		//保存每次计算的累加值
	int				m_nCalcPeriod;	//保存测试周期(日线,周线...)
	LPHISDAT		m_pAnalyData;	//分析数据
	short			m_fParaValue[MAX_PARAMNUM];//最大收益时的各个参数取值
	BOOL			m_IsCarrying;	//正在执行的过程中(主要用于在强制关闭主程序时通知父关闭它
	BOOL			m_IsCarried;	//已执行完毕
	CImageList		m_ImageList;	//用来保存m_List使用的图标
	TCHAR			m_crCurrGPCode[10];	//当前股票代码
	short			m_crCurrSetCode;
	int				m_nCurrPos;		//用来记录当前ListCtrl中的拥有焦点的位置
	void            FillLvc(void);
	void			FillGPReport(void);
	void            SetParam(void);
	void            OptimizeParam(void);
	void            CalcWithParam(void);
	double           getsxf(const char *code,short setcode);
	BOOL			PeekMyMsg(void);//消息循环,防止界面冻住
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
