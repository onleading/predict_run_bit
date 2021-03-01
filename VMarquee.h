#ifndef VMARQUEE_H_
#define VMARQUEE_H_

#ifdef	TMARQUEE_EXPORT
	#define TMARQUEEDLL_API extern "C" __declspec(dllexport)
#else
	#define TMARQUEEDLL_API extern "C" __declspec(dllimport)
#endif
 
typedef enum 
{
	SM_HSCROLL		= 1, //�������
	SM_VSCROLL		= 2, //�������
}SCROLLMODE;

typedef enum
{
	DT_INFO			= 1, //��Ϣ
	DT_CUSTOMSTOCK	= 2, //��Ʊ
}DISPLAYTYPE;

typedef struct tagPos
{
	int head;
	int tail;
}VIEWPORT;

typedef enum
{
	IT_LINK = 1,//����
	IT_INFO = 2,//��Ϣ
	IT_GP	= 3,//��Ʊ
}INFO_TYPE;

typedef struct tagMarqueeInfo
{
	INFO_TYPE	infotype;		//1 ���� 2��Ϣ 3��Ʊ
	DWORD		nInfoID;		//��ϢID
	char		content[256];	//ϵͳ��Ϣ�����ݣ��255���ַ�
	char		url[256];		//���ָ��ĳһ�����ӣ���ӵ�ַ������url[0]=='\0';
	int			weight;			//��Ϣ��Ȩ��
	COLORREF	color;			//��Ϣ����ɫ
}MARQUEEINFO,LPMARQUEEINFO;

typedef struct tagMarqueeStock
{
	short	setcode;
	char	Code[20];
	char	Name[20];	//��Ʊ��
	long	nGPIndex;
}MARQUEESTOCK,*LPMARQUEESTOCK;

struct MQ_HQInfo
{
	short	xsFlag;		//С��λ��
	float	fNow;		//�ּ�
	float	fClose;		//���̼�
	float	fVol;		//�ɽ���
	float	fHsl;		//������
	float	fLB;		//����
};

typedef long (CALLBACK * PMQ_HQINFO)(const char * Code,short setcode,MQ_HQInfo *pHQInfo);

class VMarquee : public CWnd
{
public:
	//����Բ���������Ե
	virtual void EnableArcEdgeStyle(BOOL bFlag) = 0;
	//���ô��ڴ�С
	virtual void ResizeWindowPos(int x,int y,int cx,int cy) = 0;
	//���÷�����Ϣ
	virtual void SetFeedBackMsg(HWND hMsgWnd,UINT uMsgID,DWORD Reserved=0) = 0;
	//���ù�Ʊ���ݻص�����
	virtual void SetHQCallBack(PMQ_HQINFO pfn) = 0;
	//���ó����ģʽ
	virtual void SetRandom(BOOL bFlag) = 0;
	//��������Ʊ���ɫ
	virtual void SetBackColor(COLORREF crBack) = 0;
	//������ѡ��������ɫ
	virtual void SetCustomStockTxtColor(COLORREF cr1,COLORREF cr2,COLORREF cr3,COLORREF cr4,COLORREF cr5,COLORREF cr6) = 0;
	//����ϵͳ��Ϣ������ɫ
	virtual void SetSysInfoTextColor(const COLORREF crTxt) = 0;
	//������ʾģʽ:ϵͳ��Ϣ or ��ѡ��
	virtual void SetDisplayType(DISPLAYTYPE nDt) = 0;
	//���ù���ģʽ: ���� or ��ֱ
	virtual void SetScrollMode(SCROLLMODE nMode) = 0;
	//����һ����ͣʱ��
	virtual void SetStopTime(int nCount) = 0;
	//����������ٶ�
	virtual void SetMoveSpeed(const UINT& nSpd) = 0;
	//�˽ӿ��޸�Ϊ������� 0 ������ 1 ���� 2 �л����� 
	virtual void SetMoveStyle(int nStyle) = 0;

	//���ϵͳ��Ϣ
	virtual void ClearSysInfo() = 0;
	//�����ѡ��
	virtual void ClearCustomStock() = 0;
	//���ϵͳ��Ϣ
	virtual void AddSysInfo(const MARQUEEINFO &si) = 0;
	//�����ѡ����Ϣ
	virtual void AddCustomStock(const MARQUEESTOCK& si) = 0;
	//��ʼ��
	virtual void InitPos() = 0;
	//ֹͣ
	virtual void Stop(void) = 0;
	//��ʼ bSwitch���������Ƿ�������ʾ
	virtual void StartScroll(UINT nEventID,UINT nInterval) = 0;	
	//���ùرհ�ť
	virtual BOOL SetCloseMsg(HWND hMsgWnd,UINT uMsgID,BOOL bEnable = FALSE) = 0;
};

TMARQUEEDLL_API VMarquee* NewMarquee(CWnd *pParent,UINT nID);
TMARQUEEDLL_API void DelMarquee(VMarquee * p);

#endif
