// HKNewsGet.h: interface for the CHKNewsGet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HKNEWSGET_H__86847788_8062_4B4A_8C58_3E78293F7610__INCLUDED_)
#define AFX_HKNEWSGET_H__86847788_8062_4B4A_8C58_3E78293F7610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//�۹�����Э��
//������Ѷ
//5. ȡ��ѯ���˵�
//��ѯ�˵���Ŀ�ṹ
typedef struct _MENUINFOSTRUCT
{
	char               iFatherOuterID[64];          //���˵����
	int                iInnerID;                    //�����˵����
	char			   OuterID[64];					//����ID
	unsigned short     uiTitleLen;                  //���ⳤ��
	char               chTitle[1];					//�˵���������
}MENUINFOSTRUCT;

//ȡ��ѯ���˵�
typedef struct _GETMAININFO_REQ
{
	short               iRetCode;                       //������Ϣ
	char			   OuterID[64];						//��ϢID��
}GETMAININFO_REQ;

//ȡ��ѯ���˵�����
typedef struct _GETMAININFO_ANS
{
	int                 iInfoNum;                       //���صĲ˵��ĸ���
	MENUINFOSTRUCT      cInfoRet[1];                    //����Ĳ˵���Ϣ
}GETMAININFO_ANS;

//ȡ��ѯ����
typedef struct _GETTITLEINFO_REQ
{
	short               iRetCode;                       //������Ϣ
	char			   OuterID[64];						//��ϢID��
	int					 iOffSet;
}GETTITLEINFO_REQ;

//ȡ��ѯ���ⷵ��
typedef struct _GETTITLEINFO_ANS
{
	int					 iOffSet;
	int                 iInfoNum;                       //���صĲ˵��ĸ���
	MENUINFOSTRUCT      cInfoRet[1];                    //����Ĳ˵���Ϣ
}GETTITLEINFO_ANS;

// ȡĳ����ѯ�ľ�����Ϣ
typedef struct _GETONEINFO_REQ
{
	short               iRetCode;                       //������Ϣ
	char                iFatherOuterID[64];          //���˵�����
	int                iInnerID;                    //�����˵����
	char			   OuterID[64];					//����ID
	int				   iOffSet;						//����ƫ��
}GETONEINFO_REQ;

// ȡĳ����ѯ�ľ�����Ϣ����
typedef struct _GETONEINFO_ANS
{
	int					 iOffSet;
	int                  iInfoLen;                         //��ѯ��Ϣ����
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
	
	char	m_pTransfer[REQBUFFER_LEN+1024];	//���ͻ�����
	char	m_pTcpBuf[ANSBUFFER_LEN+1024];	//���ջ�����

};

#endif // !defined(AFX_HKNEWSGET_H__86847788_8062_4B4A_8C58_3E78293F7610__INCLUDED_)
