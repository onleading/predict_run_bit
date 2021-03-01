// KeyWordSeach.cpp: implementation of the KeyWordSeach class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "Dataio.h"
#include "KeyWordSeach.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void	AbolishEmpty(char *pStr)	//去掉字符串中所有的空格
{
	int nLen=strlen(pStr),nNewLen=0;
	for(int i=0;i<nLen;i++)
	{
		if(pStr[i]!=' ')
			pStr[nNewLen++]=pStr[i];
	}
	pStr[nNewLen]=0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KeyWordSeach::KeyWordSeach()
{
	m_KeyWordSearchMap.clear();
}

KeyWordSeach::~KeyWordSeach()
{
	m_KeyWordSearchMap.clear();
}
//////////////////////////////////////////////////////////////////////////
void	KeyWordSeach::RegiseterStk(short nIDIndex, BOOL bNumCode, BOOL bRegName)
{
	MemStkInfo *pInfo = (*g_pStockIO)[nIDIndex];
	if(!pInfo||testzs(pInfo)) return;
	ContentPack cp = {0};
	cp.nIDIndex = nIDIndex;

	KeyCodePack kp={0};
	kp.SetCode = STK_NUMCODE_KEY;
	kp.CodeLen = strlen(pInfo->Code);
	if(kp.CodeLen>=MAX_CODE_SIZE) return;
	strncpy(kp.Code, pInfo->Code, MAX_CODE_SIZE-1);
	m_KeyWordSearchMap[kp] = cp;

	if(bRegName)
	{	
		//正规后的名称
		memset(&kp, 0, sizeof(KeyCodePack));
		kp.SetCode = STK_NAME_KEY;
		short nLen = strlen(pInfo->Name);
		if(nLen>=MAX_CODE_SIZE) return;
		strcpy(kp.Code,pInfo->Name);
		AbolishEmpty(kp.Code);
		kp.CodeLen = strlen(kp.Code);
		nLen = kp.CodeLen;
		for(int i=0;i<m_KeyCodeLen.size()&&m_KeyCodeLen[i]!=nLen;i++);
		if(i==m_KeyCodeLen.size()) m_KeyCodeLen.push_back(nLen);
		m_KeyWordSearchMap[kp] = cp;
	}
}

void	KeyWordSeach::InitResult()
{
	m_KeyWordOK.clear();
}

char*	KeyWordSeach::SearchKeyWord(DWORD nSearchType, char *pStart, char *pEnd)
{
	KeyCodePack kp={0};
	BOOL bNeedStepFor = TRUE;
	//找数字,可以步进
	if(nSearchType&(STK_NUMCODE_KEY&&pStart<=pEnd))
	{
		int i=0;
		memset(&kp, 0, sizeof(KeyCodePack));
		while (pStart[0]>='0'&&pStart[0]<='9'&&pStart<=pEnd&&i<MAX_CODE_SIZE) 
		{
			kp.Code[i++] = pStart[0];
			pStart++;
			bNeedStepFor = FALSE;
		}
		if(i>0)
		{
			kp.SetCode = STK_NUMCODE_KEY;
			kp.CodeLen = i;
			map<KeyCodePack, ContentPack, KeyCodeCmp>::iterator iter = m_KeyWordSearchMap.find(kp);
			BOOL bOldKeyFind = FALSE;
			for(int j=0;j<m_KeyWordOK.size();j++)
			{
				if(memcmp(&((*iter).second), &m_KeyWordOK[j], sizeof(ContentPack))==0)
					bOldKeyFind = TRUE;
			}
			if(!bOldKeyFind&&iter!=m_KeyWordSearchMap.end())
			{
				m_KeyWordOK.push_back((*iter).second);
			}
		}
	}

	//找字符串
	if(pStart<=pEnd&&(nSearchType&STK_NAME_KEY))
	{
		for(int i=0;i<m_KeyCodeLen.size();i++)
		{
			if(pEnd-pStart+1<m_KeyCodeLen[i]) continue;
			memset(&kp, 0, sizeof(KeyCodePack));
			kp.SetCode = STK_NAME_KEY;
			strncpy(kp.Code, pStart, 2*m_KeyCodeLen[i]);
			AbolishEmpty(kp.Code);
			kp.Code[m_KeyCodeLen[i]]=0;
			kp.CodeLen = strlen(kp.Code);
			map<KeyCodePack, ContentPack, KeyCodeCmp>::iterator iter = m_KeyWordSearchMap.find(kp);
			BOOL bOldKeyFind = FALSE;
			for(int j=0;j<m_KeyWordOK.size();j++)
			{
				if(memcmp(&((*iter).second), &m_KeyWordOK[j], sizeof(ContentPack))==0)
					bOldKeyFind = TRUE;
			}
			if(!bOldKeyFind&&iter!=m_KeyWordSearchMap.end())
			{
				m_KeyWordOK.push_back((*iter).second);
			}
		}
		++pStart;
		bNeedStepFor = FALSE;
	}
	if(bNeedStepFor) ++pStart;
	return pStart;
}