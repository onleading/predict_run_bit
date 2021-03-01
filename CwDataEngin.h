// CwDataEngin.h: interface for the CCwDataEngin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CWDATAENGIN_H__8FFC109E_C5F8_4A51_84A7_851B97E86691__INCLUDED_)
#define AFX_CWDATAENGIN_H__8FFC109E_C5F8_4A51_84A7_851B97E86691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define	UM_ASYN_FST_MSG		WM_USER+1350
#define UM_FEEDBACK_MSG		WM_USER+1351

enum
{
	INNER_CODE,				//股票内部代码
	COM_CODE,				//公司代码
	STOCK_CODE				//股票代码
};
//////////////////////////////////////////////////////////////////////////
class CSQLCmdCtrl;
typedef map<long, MemConstInfo, greater<long> > ConstMap;
//////////////////////////////////////////////////////////////////////////

class CCwDataEngin  
{
public:
	CCwDataEngin();
	virtual ~CCwDataEngin();

public:
	BOOL	InitFastVSQL(CWnd *pDataWnd);
	void	GetData(const char * ReqCommand,long ReqType,long ReqOption,long reserved);
	long	GetDataACK(WPARAM wParam,LPARAM lParam);

	BOOL	GetMemFieldInfo(char *FieldID, MemFieldInfo* &pFieldInfo);

	BOOL	GetIntData(int nRec,MemFieldInfo *pFieldInfo,int &nRet);
	BOOL	GetFloatData(int nRec,MemFieldInfo *pFieldInfo,double &fRet);
	BOOL	GetStringData(int nRec,MemFieldInfo *pFieldInfo,char *buf,int wantlen);

	BOOL	GetIntData(int nRec,char *FieldID,int &nRet);
	BOOL	GetFloatData(int nRec,char *FieldID,double &fRet);
	BOOL	GetStringData(int nRec,char *FieldID,char *buf,int wantlen);

	short	GetCodeIDType(char *FieldID);
	long	GetTypeIDFromCode(short nTypeID, char *Code);
	
	BOOL	GetStockCode(int nRec, char *FieldID, short nCodeIDType, char *CodeBuf, int wantlen);
	BOOL	GetStockName(int nRec, char *FieldID, short nCodeIDType, char *NameBuf, int wantlen);
	
	long	GetRowCount() {return m_nRecNum;}

	long	GetReqPassedTime() {return m_nReqPassedTime;}

	void	InitHyInfo();
	void	InitSecurityInfo();
	BOOL	GetHyConstInfo(long ConstID, MemConstInfo &iConstInfo);

private:
	static	long	m_nCount;
	long			m_nReqPassedTime;

	CWnd			*m_pMsgWnd;

	//得到的结果集
	long			m_nReqID;
	char			*m_pAckBuf;
	long			m_nRecNum;

	MemFieldInfo m_FieldInfo[CW_FIELD_NUM];
	//整理的信息
	long	m_nFieldLen;
	map<long, MemFieldInfo*, greater<long> > m_FieldMap;
	MemFieldInfo	*pFieldIndex[CW_FIELD_NUM];
	ConstMap m_mConstData[C_EnumEnd+1];

	long	m_nInnerCodeID;				//证券内部编码
	long	m_nComCodeID;				//公司内部编码
	long	m_nStockCodeID;				//股票代码


	//行业idmap
	static MemSecurityInfo m_SecurityInfo[CW_SECURITY_NUM];
	static map<long, MemConstInfo, greater<long> > m_mHyConstMap;
	static map<long, MemSecurityInfo*, greater<long> > m_mInnerCodeMap, m_mComCodeMap;
};

#endif // !defined(AFX_CWDATAENGIN_H__8FFC109E_C5F8_4A51_84A7_851B97E86691__INCLUDED_)
