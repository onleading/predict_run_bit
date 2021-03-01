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
	BOOL	m_bFxMin;		//����K��
	BOOL	m_bMin;
	BOOL	m_bJbm;
	CTime	m_Time1;
	CTime	m_Time2;
	BOOL	m_bDownAllGp;
	//}}AFX_DATA

	static	BOOL m_bLastRqDown; //�Ƿ�ȫ���Ѿ�������н���(û����,������,����,������)����������
	BOOL	m_bTmpDownLast;

	CImageList	m_ImageList;
	int		AddedGPIndex[5][MAXGPNUM];
	int		AddedGPNum[5];

	char	NeedWant_Jbm[MAXGPNUM];	//�ǲ�����Ҫ�������������

	long	m_startdate;		//����������ʼ����
	long	m_enddate;			//�������ؽ�������
	long	m_used_startdate;	//��ǰ������ʼ����
	long	m_used_enddate;		//��ǰ���ؽ�������

	//��չ����(ÿ����1��������ת��һ��,ÿ����EXFILE_SAMENUM�������մ���һ��)
	char	*m_fpdateday[EXFILE_SAMENUM];	//�ļ�����(LPEXDAYDATA)
	long	m_datedaylen[EXFILE_SAMENUM];	//�ļ�ʹ�ó���
	long	m_datedayspc[EXFILE_SAMENUM];	//�����С
	long	m_used_reqdate[EXFILE_SAMENUM];	//��������
	//
	short	m_exdaysetcode;			//��ǰ���ص��г�
	//
	char	*m_exfilercvbuf;	//���ջ�����
	long	m_exfilercvspc;		//���ջ���������(32*1024B)
	//
	long	m_startday;			//����������ʼ����
	long	m_endday;			//�������ؽ�������
	long	m_used_startday;	//��ǰ������ʼ����
	long	m_used_endday;		//��ǰ���ؽ�������
	//
	long	m_reqdatenum;		//�����������
	long	m_used_curdate;		//��ǰ��������
	long	m_used_downday;		//���ص����
	//
	long	m_filepos;			//�ļ�����λ��
	long	m_filercvlen;		//���ݰ��յ�����
	long	m_filelen;			//���ݰ��а������ļ�����
	//////////////////////////////////////////////
	long	m_startdateFm;		//5����������ʼ����
	long	m_enddateFm;		//5�������ؽ�������
	long	m_used_startdateFm;
	long	m_used_enddateFm;

	short	m_nMainID;
	short	m_nCurGPNo;
	long	nCurRemotePos;
	int		m_nTabSel;
	CFile	theRemoteFile;
	BOOL	bRemoteFileOpen;

	BOOL	m_bModal;
	BOOL	m_bDown_AtOnce; //�Ƿ�������������
	BOOL	m_bSegmentTxt;
	short	m_whichfl, m_ntofl;
	char	FileBuffer[32*1024+200];

	BOOL	m_bExDay;		//�Ƿ�����������չ����
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
	BOOL m_bDownToken;	//�Ƿ��������ر�ǹ�Ʊ
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
