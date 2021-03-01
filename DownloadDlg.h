#if !defined(AFX_DOWNLOADDLG_H__4BED5A38_706F_4603_A11F_1C5E3B32CFBC__INCLUDED_)
#define AFX_DOWNLOADDLG_H__4BED5A38_706F_4603_A11F_1C5E3B32CFBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DownloadDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "Label.h"
#include "GradientProgressCtrl.h"

#define EXFILE_SAMENUM		30
#define EXFILE_DOWNBUFLEN	32*1024

#define NEWEXDAYSPACE(bRet,pt,req) {\
	if((req)<=0) (bRet) = FALSE;\
	if(bRet)\
	{\
		TDELA(pt);\
		(pt) = new type[req];\
		if((pt)==NULL) (bRet) = FALSE;\
	}\
}

#define CHECKEXDAYSPACE(bRet,pt,spc,req,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && (pt==NULL || spc<req))\
	{\
		TDELA(pt);\
		spc = 0;\
		(pt) = new type[req];\
		if((pt)==NULL) bRet = FALSE;\
		else spc = req;\
	}\
}

#define CHECKEXDAYSTEPSPC(bRet,pt,tmppt,spc,req,step,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && ((pt)==NULL || (spc)<(req)))\
	{\
		(tmppt) = new type[(req)+(step)];\
		if((tmppt)==NULL) (bRet) = FALSE;\
		else\
		{\
			memset((tmppt),0,((req)+(step))*sizeof(type));\
			if((spc)>0 && (pt)!=NULL) memcpy((tmppt),(pt),(spc)*sizeof(type));\
			TDELA(pt);\
			(pt) = (tmppt);\
			(spc) = (req)+(step);\
		}\
	}\
}

class CDownloadDlg : public CDialog
{
	void AfterTabChange();
	void EnableOther(BOOL bEnable);
//	BOOL OneStock_PH(MemStkInfo *pStkInfo);

	void Down_HqData();
	void Down_FxMinData();
	void Down_ExDayFile(short setcode);
	void Down_DayData();
	void Down_MinData();
	void Down_JbmFileLength();
	void Down_JbmData();
	void DownFinished();
	void FillGPReport();
public:
	CDownloadDlg(CWnd* pParent = NULL);   // standard constructor
	~CDownloadDlg();

// Dialog Data
	//{{AFX_DATA(CDownloadDlg)
	enum { IDD = IDD_DOWNLOAD_DIALOG };
	CLabel	m_Hint_Status;
	CListCtrl	m_GpReport_List;
	CLabel	m_Status_Static;
	CGradientProgressCtrl	m_Progress;
	BOOL	m_bDay;
	BOOL	m_bFxMin;		//分钟K线
	BOOL	m_bMin;
	BOOL	m_bJbm;
	CTime	m_Time1;
	CTime	m_Time2;
	BOOL	m_bDownAllGp;
	//}}AFX_DATA

	static	BOOL m_bLastRqDown; //是否全局已经完毕所有今天(没闭市,到昨天,闭市,到今天)的最新数据
	BOOL	m_bTmpDownLast;

	CImageList	m_ImageList;
	int		AddedGPIndex[5][MAXGPNUM];
	int		AddedGPNum[5];

	char	NeedWant_Jbm[MAXGPNUM];	//是不是需要请求基本面数据

	long	m_startdate;		//日线下载起始日期
	long	m_enddate;			//日线下载结束日期
	long	m_used_startdate;	//当前下载起始日期
	long	m_used_enddate;		//当前下载结束日期

	//扩展数据(每下载1个交易日转档一次,每下载EXFILE_SAMENUM个交易日存盘一次)
	char	*m_fpdateday[EXFILE_SAMENUM];	//文件缓存(LPEXDAYDATA)
	long	m_datedaylen[EXFILE_SAMENUM];	//文件使用长度
	long	m_datedayspc[EXFILE_SAMENUM];	//缓存大小
	long	m_used_reqdate[EXFILE_SAMENUM];	//请求日期
	//
	short	m_exdaysetcode;			//当前下载的市场
	//
	char	*m_exfilercvbuf;	//接收缓冲区
	long	m_exfilercvspc;		//接收缓冲区长度(32*1024B)
	//
	long	m_startday;			//日线下载起始日期
	long	m_endday;			//日线下载结束日期
	long	m_used_startday;	//当前下载起始日期
	long	m_used_endday;		//当前下载结束日期
	//
	long	m_reqdatenum;		//本批请求个数
	long	m_used_curdate;		//当前请求日期
	long	m_used_downday;		//下载的序号
	//
	long	m_filepos;			//文件更新位置
	long	m_filercvlen;		//数据包收到长度
	long	m_filelen;			//数据包中包含的文件长度
	//////////////////////////////////////////////
	long	m_startdateFm;		//5分钟下载起始日期
	long	m_enddateFm;		//5分钟下载结束日期
	long	m_used_startdateFm;
	long	m_used_enddateFm;

	short	m_nMainID;
	short	m_nCurGPNo;
	long	nCurRemotePos;
	int		m_nTabSel;
	CFile	theRemoteFile;
	BOOL	bRemoteFileOpen;

	BOOL	m_bModal;
	BOOL	m_bDown_AtOnce; //是否进入后立刻下载
	BOOL	m_bSegmentTxt;
	short	m_whichfl, m_ntofl;
	char	FileBuffer[32*1024+200];

	BOOL	m_bExDay;		//是否下载日线扩展数据
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bDownToken;	//是否正在下载标记股票
	int	 m_nWhichJBM;
	// Generated message map functions
	//{{AFX_MSG(CDownloadDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDownDayCheck();
	afx_msg void OnDownFxMinCheck();
	afx_msg void OnDownallCheck();
	afx_msg void OnBrowseVipdoc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOADDLG_H__4BED5A38_706F_4603_A11F_1C5E3B32CFBC__INCLUDED_)
