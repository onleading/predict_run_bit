#if !defined(AFX_ZSTTICKDLG_H__549EB6CA_8074_4DFE_9307_882F8A4564A8__INCLUDED_)
#define AFX_ZSTTICKDLG_H__549EB6CA_8074_4DFE_9307_882F8A4564A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZstTickDlg.h : header file
//

#include "PieChartCtrl.h"
#include "SortListCtrl.h"

struct ZsTickCalcData
{
	short nItemNum;					//分笔笔数
	short nBigItemNum;
	DWORD dwBigTickVol;
	short nInBigItem;
	float dwInBigTickVol;
	short nOutBigItem;
	float dwOutBigTickVol;
	long High;
	long Open;
	long Low;
	long Close;
	float dwVolume;
	float fAmount;
	float dwInFlagVol;
	float dwOutFalgVol;
};

struct  PriceVol
{ 
	long Price;
	long Volume;
	long Kcl;
	long Pcl;
	short Num;
	short Knum;
	short Pnum;
};

class UZst;
/////////////////////////////////////////////////////////////////////////////
// ZstTickDlg dialog

class ZstTickDlg : public CDialog
{
// Construction
public:
	ZstTickDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ZstTickDlg)
	enum { IDD = IDD_ZSTTICKTJ };
	CEdit	m_bigVol;
	CTabCtrl	m_Tab;
	CSortListCtrl	m_List;
	CString	m_strClose;
	CString	m_strTickNum;
	CString	m_strInVol;
	CString	m_strHigh;
	CString	m_strLow;
	CString	m_strOpen;
	CString	m_strOutVol;
	CString	m_strTickVol;
	CString	m_strVol;
	CString	m_strSartTime;
	CString	m_strEndTime;
	CString	m_strZF;
	CString	m_strMinNum;
	CString	m_strAver;
	CString	m_strBigTickNum;
	CString	m_strInBigTick;
	CString	m_strOutBigTick;
	CString	m_strZenFu;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ZstTickDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPieChartCtrl m_Pie;

	// Generated message map functions
	//{{AFX_MSG(ZstTickDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangeZstticktab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnColumnclickZstticklist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonUpdate();
	afx_msg void OnRclickZstticklist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSaveBigVol();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	WantDataMayContinue(BYTE nWantDate, BOOL bFirstWant=FALSE);
	void	ShowIt(void);
	long	GetDate(short nBackDate);
	CString GetDateString(short nBackDate);
	CString GetMinuteString(short nMinuteXh);
	long	GetMinuteNum(void);
	
	void	HideNormal(BOOL bHide);
	void	FillList();

	short	ReadPriceVolTable(TickData *indapbase, int nBgnPos, int nEndPos);

	void	DrawYuCiTu(CDC *pDC);
	void	Init();
	void	ReCalcData();

	BOOL	SaveHisTickCache(BYTE nCurReqDate);
	void	CalcOneData(BYTE nCurReqDate);

public:
	UZst				*m_pUZst;				//统计的走势图
	short				m_nCurGPNo;				//外部传入的分析股票
	long				m_nCurDate;				//当前数据日期
	BYTE				m_nStartDate;			//外部传入的起始统计日期,0-当天,1-昨天...
	short				m_nStartMinuteXh;		//外部传入的起始统计分钟数
	BYTE				m_nEndDate;				//外部传入的结束统计日期
	short				m_nEndMinuteXh;			//外部传入的结束统计分钟数

	TICKDATA			*m_pTick[MAX_ZST_MANYDAY+1];
	long				m_nTickNum[MAX_ZST_MANYDAY+1];

private:
	short				m_nMainID;
	BOOL				m_bHeadTag[MAX_ZST_MANYDAY+1];	//数据是否要完了,要完整日数据时赋值
	long				m_HeadTagDate[MAX_ZST_MANYDAY+1];
	BYTE				m_nCurReqDate;					//当前要的日期数据,请求数据时赋值
	ZsTickCalcData		m_CalcData;						//统计数据

	map<long, PriceVol, less<long> > m_mapPriceVol;
	vector<TickData>	m_BigTick;
	vector<long>		m_BigTickDate;

	CRect				m_yucitu_rect;					//鱼刺图区域（取自list控件）
	CImageList			m_imagelist;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZSTTICKDLG_H__549EB6CA_8074_4DFE_9307_882F8A4564A8__INCLUDED_)
