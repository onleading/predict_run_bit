#ifndef _BKDATA_H
#define _BKDATA_H

extern CWnd *  g_pBKDataDlg;

#define MAX_BKDATA_REQNUM	10

struct IW_Req
{
	char	search_type;			//0:���ݹ�Ʊ���� 1:������ҵ���� 2:���ݷ���ID 3:ȡ�۲�Ա����
	long	from_order;				//���Ǹ�˳��ʼ���ȡ����
	short	wantnum;				//���80��
	short	setcode;				//֤ȯ�г�
	char	code[11];				//����
	long	hy_id;					//��ҵID
	long	type_id;				//����ID
};

struct BkData_Req
{
	HWND		hWnd;
	UINT		nMsgID;
	WPARAM		wParam;
	LPARAM		lParam;
	int			nType;	
	char		strCode[11];
	short *		lpnUsedStkIndex;
	short		nStkNum;
	IW_Req		IWReq;
	int			nCancelFlag;
};

class CBkData
{
public:
	CBkData();
	~CBkData();
	BOOL	Create(CWnd *pMainWnd);
	BOOL	Subscrible(HWND hWnd,UINT nMsgID,UINT wParam,UINT lParam,int nType,short *lpnUsedStkIndex,short nStkNum,int nCancelFlag=0,char *strCode=NULL,struct IW_Req *pIWReq=NULL);
	BOOL	GetOneSubscrible(struct BkData_Req *pReq);
private:
	struct BkData_Req	m_Req[MAX_BKDATA_REQNUM];
	int					m_nReqNum;
};

extern CWnd *  g_pBKDataDlg;

#endif
