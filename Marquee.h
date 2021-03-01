#ifndef MARQUEE_H_
#define MARQUEE_H_

typedef enum
{
	MARQUEE_POPINFO = 1,
	MARQUEE_SHOWGP	= 2,
}MARQUEE_MSG;

typedef enum 
{
	SM_HSCROLL	= 1, //�������
	SM_VSCROLL	= 2, //�������
}SCROLLMODE;

typedef enum
{
	DT_SYSINFO		= 1, //ϵͳ��Ϣ
	DT_CUSTOMSTOCK	= 2, //��Ʊ
}DISPLAYTYPE;

typedef struct tagMarqueeStock
{
	CString	sStkName;		//��Ʊ����
	UINT	nStkCode;		//��Ʊ����
	FLOAT	fStkCurrPrice;	//�ּ�
	FLOAT	fStkClsPrice;	//���̼�
	CString	sStkVolumn;		//�ɽ���
	FLOAT	fStkTR;			//������
	FLOAT	fStkVR;			//����
}MARQUEESTOCK,*LPMARQUEESTOCK;

typedef struct tagPos
{
	int head;
	int tail;
}VIEWPORT;

typedef enum
{
	IT_LINK = 1,	//����
	IT_POP	= 2,	//��������
}INFO_TYPE;

typedef struct tagMarqueeInfo
{
	char		content[256];	//ϵͳ��Ϣ�����ݣ��255���ַ�
	char		url[256];		//���ָ��ĳһ�����ӣ���ӵ�ַ������url[0]=='\0';
	INFO_TYPE	flag;			//1 ������ 2��������
	int			weight;			//��Ϣ��Ȩ��
}MARQUEEINFO,LPMARQUEEINFO;

class CMarquee : public CWnd
{
public:
	CMarquee();
	virtual ~CMarquee();
protected:
	BOOL			m_bEnableStop;			//�Ƿ������ͣ
	int				m_nDisplayType;			//��ʾϵͳ��Ϣ�����Թ�Ʊ
	int				m_nScrollMode;			//������������������
	COLORREF		m_crBackColor;			//�ؼ�����ɫ
	COLORREF		m_crSysInfoTxt;			//ϵͳ��Ϣ��ɫ
	COLORREF		m_crCustomStockTxt[6];	//��ѡ����ɫ
	CString			m_strText;				//
	UINT			m_nEventID;				//TimerID
	VIEWPORT		m_stViewport;			//�ӿ�
	UINT			m_nMoveSpd;				//ʱ�Ӽ��
	BOOL			m_bTimerFlag;			//

	std::vector<MARQUEESTOCK>	m_vecCustomStk; //��ѡ��Ʊ
	std::vector<VIEWPORT>		m_vecCustomPos;
	std::vector<MARQUEEINFO>	m_vecSysInfo;	//ϵͳ��Ϣ
	std::vector<VIEWPORT>		m_vecSysInfoPos;
	int							m_nLength;

	//{{AFX_MSG(CMarquee)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//���÷�����Ϣ
	void SetFeedBackMsg(HWND hMsgWnd,UINT uMsgID,DWORD Reserved=0);
	//���ó����ģʽ
	void SetRandom(BOOL bFlag);
	//��������Ʊ���ɫ
	void SetBackColor(COLORREF crBack);
	//������ѡ��������ɫ
	void SetCustomStockTxtColor(COLORREF cr1,COLORREF cr2,COLORREF cr3,COLORREF cr4,COLORREF cr5,COLORREF cr6);
	//����ϵͳ��Ϣ������ɫ
	void SetSysInfoTextColor(const COLORREF crTxt);
	//������ʾģʽ:ϵͳ��Ϣ or ��ѡ��
	void SetDisplayType(DISPLAYTYPE nDt);
	//���ù���ģʽ: ���� or ��ֱ
	void SetScrollMode(SCROLLMODE nMode);
	//����һ����ͣʱ��
	void SetStopTime(int nCount);
	//����������ٶ�
	void SetMoveSpeed(const UINT& nSpd);
	//�Ƿ���һ����ͣ
	void EnableStop(BOOL bFlag);

	//���ϵͳ��Ϣ
	void ClearSysInfo();
	//�����ѡ��
	void ClearCustomStock();
	//���ϵͳ��Ϣ
	void AddSysInfo(const MARQUEEINFO &si);
	//�����ѡ����Ϣ
	void AddCustomStock(const MARQUEESTOCK& si);
	//��ʼ��
	void InitPos();
	//ֹͣ
	void Stop(void);
	//��ʼ
	void StartScroll(UINT nEventID,UINT nInterval);	
	//Size��Ϣ
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	HWND	m_hMsgWnd;		//������Ϣ�Ĵ���
	UINT	m_uMsgID;		//��������Ϣ

	BOOL	m_bRandom;		//�Ƿ������״̬

	CRect	m_rcBtn;		//�Ҳ�С��ť��״
	CRect	m_rcClient;

	int		m_nStopTime;
	int		m_nStopCount; 
private:
	BOOL RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName);
	void DrawIt();
};

#endif
