#include "stdafx.h"
#include "tdxw.h"
#include "CWCalc.h"
#include "MathParser.h"

CCWCalc::CCWCalc(long nClue)
{
	m_nClue = nClue;
	memset(m_KeyCache, 0, sizeof(m_KeyCache));
	m_fRet = 0.0f;
}

CCWCalc::~CCWCalc()
{
	m_strCalcFunc = _T("");
}

void	CCWCalc::SetConfig(CString strCalcFunc, OutTitle_Cfg *pConfig, short nSetcode, char *Code)
{
	m_strCalcFunc = strCalcFunc;
	m_pConfig = pConfig;
	m_nSetCode = nSetcode;
	strncpy(m_Code, Code, CODE_SIZE);
}

//////////////////////////////////////////////////////////////////////////
static char cItemHead = _T('@');
static char *strExcludeCalcChar = _T(":+-*/><=;!\"\'[](){}.,|");
short	CCWCalc::ChangeOneKeyword()
{
	if(m_strCalcFunc.GetLength()<1||!m_pConfig) return -1;

	static char lpFuncString[1024]={0}, strKeyWord[255]={0}, strExclude[255]={0};
	memset(lpFuncString, 0, 1024);
	memset(strKeyWord, 0, 255);
	memset(strExclude, 0, 255);
	int nLen = m_strCalcFunc.GetLength();
	strncpy(lpFuncString, (char*)(LPCSTR)m_strCalcFunc, min(nLen, 1023));
	int nsepLen = strlen(strExcludeCalcChar);
	int nHpos = -1, nFlag = 0;
	for(int i=0;i<nLen;i++)
	{
		if(nFlag==0&&lpFuncString[i]=='@')			//找到了!
		{
			if(i<nLen-1) nHpos = ++i;
			while(i<nLen&&memchr(strExcludeCalcChar,lpFuncString[i],nsepLen+1)==NULL)
				i++;
			if(i==nLen) 
			{
				strncpy(strKeyWord, &lpFuncString[nHpos], i-nHpos);
				break;	//这个是简单Item
			}
			else 
			{
				strncpy(strKeyWord, &lpFuncString[nHpos], i-nHpos);
				break;
			}
		}
	}
	//没找到
	if(strKeyWord[0]==0) return 0;
	
	sprintf(strExclude, "%c%s", cItemHead, strKeyWord);

	BOOL bSuccess = FALSE;
	int nRet; float fRet;
	switch(m_pConfig->OutType) 
	{
	case 'D'://int
		bSuccess=TD_GetIntData_(m_nClue,strKeyWord,nRet);
		if(bSuccess)
			m_strCalcFunc.Replace(strExclude, IntToStr(nRet));
		break;
	case 'F'://float
		bSuccess=TD_GetFloatData_(m_nClue,strKeyWord,fRet);
		if(bSuccess)
			m_strCalcFunc.Replace(strExclude, FloatToStr(fRet));
		break;
	default:
		return -1;
	}
	if(bSuccess==FALSE) return -1;

	return 1;
}

BOOL	CCWCalc::ChangeCalcString()
{
	int nRet = 0;

	while ((nRet = ChangeOneKeyword()) == 1)
	{
	}
	return nRet;
}

BOOL	CCWCalc::MathParse()
{
	CMathParser Parser(m_strCalcFunc);
	Parser.Parse();
	
	if(Parser.ParseError == TRUE)
		return FALSE;
	else
		m_fRet = (float)Parser.ParseValue;

	return TRUE;
}

BOOL	CCWCalc::TCalcParse()
{
	m_fRet = g_pCalc->_CallDTopViewCalc_5((char*)(LPCSTR)m_strCalcFunc, m_nSetCode, m_Code);
	
	return TRUE;
}

BOOL	CCWCalc::ParseCalcString()
{
	if(ChangeCalcString()==-1) return FALSE;

	return MathParse()||TCalcParse();
}
