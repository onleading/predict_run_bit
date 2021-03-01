#ifndef _BKDATA_H
#define _BKDATA_H

extern CWnd *  g_pBKDataDlg;

#define MAX_BKDATA_REQNUM	10

struct IW_Req
{
	char	search_type;			//0:根据股票代码 1:根据行业代码 2:根据分类ID 3:取观察员数据
	long	from_order;				//从那个顺序开始向后取数据
	short	wantnum;				//最多80个
	short	setcode;				//证券市场
	char	code[11];				//代码
	long	hy_id;					//行业ID
	long	type_id;				//分类ID
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
