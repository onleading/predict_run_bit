#ifndef UBASE_H_
#define UBASE_H_

#include "struct.h"
#include "custompad.h"

#define CREATE_MEMDC(theRc)	\
	CDC		MemDC;		\
	CBitmap Bitmap,*OldBitmap;	\
	MemDC.CreateCompatibleDC(pDC);	\
	Bitmap.CreateCompatibleBitmap(pDC,theRc##.Width(),theRc##.Height());	\
	OldBitmap = MemDC.SelectObject(&Bitmap);		\
	MemDC.FillSolidRect(0,0,theRc##.Width(),theRc##.Height(),VipColor.BACKCOLOR);	\
	CFont *oldfont= g_d.SelectFont(&MemDC,GRAPH_FONT);		\
	MemDC.SetBkMode(TRANSPARENT);

#define DELETE_MEMDC		\
	g_d.RestoreFont(&MemDC,oldfont);	\
	MemDC.SelectObject(OldBitmap);		\
	Bitmap.DeleteObject();			\
	MemDC.DeleteDC();

class UBase : public CRect
{
protected:
	CWnd	*	m_pView;
	DWORD		m_nInitPad;
	int			m_nWhichUnit;
	LPUNITS		m_pUnits;
	int			m_nInCom2;
	int			m_nInHisDlg;
	int			m_nInMultiPeriod;

	int			m_nType;
	int			m_nStyle;
	CRect		m_rcIn;

	short		m_nCurGP;	//��Ʒ����� С��10000��ʾ�ǻ�����Ʊ ����10000������Ʒ��
	MemStkInfo *m_pStkInfo;	//�ù�Ʊ��MemStkInfo

	short		m_Domain;	//�ù�Ʊ���г�
	int			m_XsFlag;	//С��λ��
	BOOL		m_bLcIdx;	//�Զ����Ʒ��
	BOOL		m_bLcExt;	//�Զ��ⲿƷ��
	BOOL		m_bLcPt;
	BOOL		m_bZs;
	BOOL		m_bHKZs;
	BOOL		m_bGz;		//�Ƿ��ǹ�ծ
	BOOL		m_bQyzq;	//�Ƿ�����ծ
	BOOL		m_bQz;		//�Ƿ���Ȩ֤
	BOOL		m_bABG;		//�Ƿ���AB��
	BOOL		m_bSZSHZS;	//�Ƿ����ͳһָ��
	BOOL		m_bETF;		//�Ƿ���ETF
	BOOL		m_bSZKFSJJ;	//�Ƿ������ڿ���ʽ
	BOOL		m_bSHKFSJJ;	//�Ƿ����Ϻ�����ʽ
	BOOL		m_bCloseJJ;	//�Ƿ��Ƿ��ʽ����
	long		m_lDate;
	BOOL		m_bIPO;		//�Ƿ��ǵ�һ������
	
	BOOL		m_bMinuteTick_Special,m_bAnalyData_Special;
	
	BOOL		m_bDSFlag;		//	�����г���Ʒ��־
	short		TimeSec[8];		//	ʱ�����Ϣ
	PTIMEINF	m_pTimer;		//	ʱ������

	int			m_nGxdFlag;
	BOOL		m_bLevel2Property;
public:
	UBase(LPCOMINFO pComInfo);
	virtual ~UBase();

	int		GetMyType() { return m_nType; }

	virtual int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif
