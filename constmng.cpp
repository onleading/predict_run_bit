#include "StdAfx.h"
#include "ConstMng.h"

ConstMng::ConstMng()
{
}

ConstMng::~ConstMng()
{
}

//////////////////////////////////////////////////////////////////////////
void ConstMng::LoadInConst()
{
	FILE *fp = _fsopen(HomePath+"incon.dat","rt",_SH_DENYNO);
	if(!fp) return;

	m_mZJHHY.clear();
	char	*strToKen = "ZJHHY";
	char	tempLine[256],tempContent[256];
	BOOL	nStart=FALSE;
	while(!feof(fp))
	{	
		if(fgets(tempLine,255,fp))
		{	
			if(tempLine[0]=='#' && tempLine[1]=='#') break;
			if(nStart)
			{
				ZJHHY_Cfg cfg={0};
				memset(tempContent,0,255);
				ParseMessageStr(tempLine,tempContent,"|",1);
				strncpy(cfg.HYCode,tempContent,9);
				ParseMessageStr(tempLine,tempContent,"|",2);
				strncpy(cfg.HYName,tempContent,49);
				AllTrimEx(cfg.HYName);

				sprintf(tempContent,"%d%s",cfg.HYCode[0]-'A'+10,cfg.HYCode+1);
				long tmpLong = atol(tempContent);
				if(tmpLong>0 && strlen(cfg.HYCode)==3)
				{
					cfg.HYXH = m_vZJHHY.size();
					cfg.HYID = tmpLong;
					m_vZJHHY.push_back(cfg);
					m_mZJHHY[tmpLong] = cfg;
				}
			}
			if(tempLine[0]=='#' && !nStart && strnicmp(tempLine+1,strToKen,strlen(strToKen))==0)
				nStart=TRUE;
		}
	}
	fclose(fp);	
}

ZJHHY_Cfg*	ConstMng::GetHyFromHYID(float J_zjhhy)
{
	long lZjhhy = (long)(J_zjhhy+0.5f);
	char strID[50]={0}, strSub2ID[6]={0};
	sprintf(strID, "%d", lZjhhy);
	strncpy(strSub2ID, strID, 4);
	long nConstHYID = atol(strSub2ID);
	map<long, ZJHHY_Cfg, greater<long> >::iterator iter = m_mZJHHY.find(nConstHYID);
	if(iter!=m_mZJHHY.end())
	{
		ZJHHY_Cfg hyinfo = (*iter).second;
		if(hyinfo.HYXH>=0&&hyinfo.HYXH<m_vZJHHY.size()&&hyinfo.HYID>0)
			return &m_vZJHHY[hyinfo.HYXH];
	}
	return NULL;
}

long	ConstMng::GetHyXHFromHYID(float J_zjhhy)
{
	long lZjhhy = (long)(J_zjhhy+0.5f);
	char strID[50]={0}, strSub2ID[6]={0};
	
	sprintf(strID, "%d", lZjhhy);
	strncpy(strSub2ID, strID, 4);
	long nConstHYID = atol(strSub2ID);
	map<long, ZJHHY_Cfg, greater<long> >::iterator iter = m_mZJHHY.find(nConstHYID);
	if(iter!=m_mZJHHY.end())
	{
		ZJHHY_Cfg hyinfo = (*iter).second;
		if(hyinfo.HYXH>=0&&hyinfo.HYXH<m_vZJHHY.size()&&hyinfo.HYID>0)
			return hyinfo.HYXH;
	}
	return -1;
}


ZJHHY_Cfg*	ConstMng::GetHyFromHYXH(int nIDXH)
{
	if(nIDXH<0||nIDXH>=m_vZJHHY.size()) return NULL;
	return &m_vZJHHY[nIDXH];
}

BOOL	ConstMng::BeInZJHHYBlock(CString blockname)
{
	for(int i=0;i<m_vZJHHY.size();i++)
	{
		if(m_vZJHHY[i].HYName==blockname)
			return TRUE;
	}
	return FALSE;
}

char *	ConstMng::GetHYFromCommandIDXH(long nIDXH)
{
	if(nIDXH<0||nIDXH>=m_vZJHHY.size()) return "Пе";
	return m_vZJHHY[nIDXH].HYName;
}

int		ConstMng::GetStockNumInHY(long nHYID)
{
	if(!g_pStockIO) return 0;
	int nRet = 0, stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	//
	MemStkInfo *tmpInfo=NULL;
	for (int i=0;i<stocknum;i++)
	{
		tmpInfo=(*g_pStockIO)[i];
		ZJHHY_Cfg* pHyCfg = GetHyFromHYID(tmpInfo->J_zjhhy);
		if(pHyCfg&&nHYID==pHyCfg->HYID)
			nRet++;
	}
	return nRet;
}