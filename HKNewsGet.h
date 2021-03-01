// HKNewsGet.h: interface for the CHKNewsGet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HKNEWSGET_H__86847788_8062_4B4A_8C58_3E78293F7610__INCLUDED_)
#define AFX_HKNEWSGET_H__86847788_8062_4B4A_8C58_3E78293F7610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//港股新闻协议
//新闻资讯
//5. 取咨询主菜单
//咨询菜单项目结构
typedef struct _MENUINFOSTRUCT
{
	char               iFatherOuterID[64];          //父菜单编号
	int                iInnerID;                    //本级菜单序号
	char			   OuterID[64];					//标题ID
	unsigned short     uiTitleLen;                  //标题长度
	char               chTitle[1];					//菜单标题内容
}MENUINFOSTRUCT;

//取咨询主菜单
typedef struct _GETMAININFO_REQ
{
	short               iRetCode;                       //返回信息
	char			   OuterID[64];						//信息ID串
}GETMAININFO_REQ;

//取咨询主菜单返回
typedef struct _GETMAININFO_ANS
{
	int                 iInfoNum;                       //返回的菜单的个数
	MENUINFOSTRUCT      cInfoRet[1];                    //具体的菜单信息
}GETMAININFO_ANS;

//取咨询标题
typedef struct _GETTITLEINFO_REQ
{
	short               iRetCode;                       //返回信息
	char			   OuterID[64];						//信息ID串
	int					 iOffSet;
}GETTITLEINFO_REQ;

//取咨询标题返回
typedef struct _GETTITLEINFO_ANS
{
	int					 iOffSet;
	int                 iInfoNum;                       //返回的菜单的个数
	MENUINFOSTRUCT      cInfoRet[1];                    //具体的菜单信息
}GETTITLEINFO_ANS;

// 取某个咨询的具体信息
typedef struct _GETONEINFO_REQ
{
	short               iRetCode;                       //返回信息
	char                iFatherOuterID[64];          //父菜单索引
	int                iInnerID;                    //本级菜单序号
	char			   OuterID[64];					//标题ID
	int				   iOffSet;						//内容偏移
}GETONEINFO_REQ;

// 取某个咨询的具体信息返回
typedef struct _GETONEINFO_ANS
{
	int					 iOffSet;
	int                  iInfoLen;                         //咨询信息长度
	char                 chInfo[1];
}GETONEINFO_ANS;

typedef struct _MENUINFOSTRUCT_PLUS
{
	MENUINFOSTRUCT		mis;
	CString				strShowName;
}MENUINFOSTRUCT_PLUS;

//////////////////////////////////////////////////////////////////////////

class CHKNewsGet  
{
public:
	CHKNewsGet();
	virtual ~CHKNewsGet();

public:
	BOOL	GetConnectStatus() { return m_bConnected2News; }
	BOOL	GetNewsConfig(vector<MENUINFOSTRUCT_PLUS> &vNewsConfigList);
	BOOL	GetTitleList(vector<MENUINFOSTRUCT_PLUS> &vTitleList, MENUINFOSTRUCT_PLUS *pInfo_title, char setcode=0, char  *code="");
	BOOL	GetContent(CString &Content, MENUINFOSTRUCT_PLUS *pInfo_title, char setcode=0, char  *code="");

private:
	BOOL	InitNewsThread(void);
	void	UninitNewsThread(void);
	BOOL	StartSocket(LPSTR strError);
	BOOL	ConnectWithNews(LPSTR strError,const char *hostname,UINT hostport);

	BOOL	SendIt(void *lpBuf, int nBufLen);

	BOOL	ReqNewsConfig();
	BOOL	ReqNewsList(char *OuterID, int iOffSet);
	BOOL	ReqNewsContent(MENUINFOSTRUCT_PLUS *pMis, short nOffset);

	DWORD	OnRecvHead(ANSHEADER *pHeader);
	BOOL	OnRecvConfig(ANSHEADER *pHeader, vector<MENUINFOSTRUCT_PLUS> &config);
	BOOL	OnRecvTitle(ANSHEADER *pHeader, vector<MENUINFOSTRUCT_PLUS> &list);
	BOOL	OnRecvContent(ANSHEADER *pHeader, CString &Content);
//
private:
	SOCKET	m_hSocket;
	long	m_bConnected2News;
	
	char	m_pTransfer[REQBUFFER_LEN+1024];	//发送缓冲区
	char	m_pTcpBuf[ANSBUFFER_LEN+1024];	//接收缓冲区

};

#endif // !defined(AFX_HKNEWSGET_H__86847788_8062_4B4A_8C58_3E78293F7610__INCLUDED_)
