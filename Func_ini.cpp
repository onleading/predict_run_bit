#include "stdafx.h"
#include "func_ini.h"

//用于批量读取INI
int LoadSomeString(char deststr[][30],char *type,char *inisection,int maxcount,char *totalstr,const char *inifile)
{
	int total = GetPrivateProfileInt(inisection,totalstr,0,inifile);
	if(total <= 0 || total > maxcount)
		total = maxcount;
	char	s[5];
	int		i,j;
	CString indexstr;
	for (j = 0;j < total;j++)
	{    
		sprintf(s,"%02d",j+1);
		indexstr = type;
		indexstr += s;
		GetPrivateProfileString(inisection, 
								indexstr, "",deststr[j],sizeof(deststr[j]),inifile);
		if(deststr[j][0] == '\0')
		{
			sprintf(s,"%d",j+1);//防止有的用户在配置时不按规范
			indexstr = type;
			indexstr += s;
			GetPrivateProfileString(inisection, 
									indexstr, "",deststr[j],sizeof(deststr[j]),inifile);
		}
		for(i = 0;i < strlen(deststr[j]);i++) 
		{
			if( (unsigned char)(deststr[j][i]) < 32 )
			{
				deststr[j][i] = '\0';
			    break;
			}
		}
	}
	return total;
}

void SaveSomeString(char deststr[][30],char *type,char *inisection,int count,char *totalstr,const char *inifile)
{
	char s[5];
	sprintf(s,"%02d",count);
	WritePrivateProfileString(inisection,totalstr,s,inifile);

	CString indexstr;
	for(int i = 0;i < count;i++)
	{
		sprintf(s,"%02d",i+1);
		indexstr = type;
		indexstr += s;
		WritePrivateProfileString(inisection, 
								  indexstr,deststr[i],inifile);
	}
}

void Dectect_OverWrite_ProxyInfo()
{
	CString	OldIni = g_WSXHStr+g_strYxhjCfg;
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	if(TestFileExist(Ini)) 
		return;
	char tmpStr[255];
	int tmpInt = GetPrivateProfileInt("PROXY","HasProxy",0,OldIni);
	int tmpInt2 = GetPrivateProfileInt("TDXPROXY","buse",0,OldIni);
	if(tmpInt || tmpInt2)
	{
		tmpInt = GetPrivateProfileInt("PROXY","HasProxy",0,OldIni);
		WritePrivateProfileString("PROXY","HasProxy",IntToStr(tmpInt),Ini);
		//
		tmpInt=GetPrivateProfileInt("PROXY","SockType",2,OldIni);
		WritePrivateProfileString("PROXY","SockType",IntToStr(tmpInt),Ini);
		//
		tmpInt=GetPrivateProfileInt("PROXY","ProxyPort",80,OldIni);
		WritePrivateProfileString("PROXY","ProxyPort",IntToStr(tmpInt),Ini);
		//
		GetPrivateProfileString("PROXY","ProxyIP","",tmpStr,99,OldIni);
		WritePrivateProfileString("PROXY","ProxyIP",tmpStr,Ini);
		//
		GetPrivateProfileString("PROXY","ProxyUser","",tmpStr,99,OldIni);
		WritePrivateProfileString("PROXY","ProxyUser",tmpStr,Ini);
		//
		GetPrivateProfileString("PROXY","ProxyPass","",tmpStr,99,OldIni);
		WritePrivateProfileString("PROXY","ProxyPass",tmpStr,Ini);
		//
		tmpInt=GetPrivateProfileInt("TDXPROXY","buse",0,OldIni);
		WritePrivateProfileString("TDXPROXY","buse",IntToStr(tmpInt),Ini);
	}
}

//读通达信格式的加密文件
BOOL GetFromComteFile(int nFlag,BOOL bQs,BOOL bUseBak,const char *Pwd)
{
	return FALSE;

	char *pIniBuf = NULL;
	char filename[255];
	if(bQs)
		sprintf(filename,"%sqscomte.dat",HomePath);
	else
	{
		if(!bUseBak)
			sprintf(filename,"%s%s",HomePath,g_GC.Level2_SepcComte);
		else
			sprintf(filename,"%s%s",HomePath,"tdxcomte.dbk");
	}
	pIniBuf=OpenTdxIni(filename,Pwd);
	if(!pIniBuf) return FALSE;
	CString tempStr;
	if(nFlag==COMTE_VIP_HOST)
	{
		//VIP行情主站
		g_VipHQHost.HostNum = GetTDXProfileInt("VIPHOST","HostNum",0,pIniBuf);
		g_VipHQHost.HostNum = MAX(g_VipHQHost.HostNum,0);
		g_VipHQHost.HostNum = MIN(g_VipHQHost.HostNum,150);
		for(int i = 0;i < g_VipHQHost.HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetTDXProfileString("VIPHOST",tempStr,"",g_VipHQHost.HostName[i],48,pIniBuf);
			tempStr.Format("IPAddress%02d",i+1);
			GetTDXProfileString("VIPHOST",tempStr,"",g_VipHQHost.IPAddress[i],48,pIniBuf);
			tempStr.Format("Port%02d",i+1);
			g_VipHQHost.Port[i] = GetTDXProfileInt("VIPHOST",tempStr,7709,pIniBuf);
			tempStr.Format("Weight%02d",i+1);
			g_VipHQHost.weight[i] = GetTDXProfileInt("VIPHOST",tempStr,0,pIniBuf);
		}
	}
	else if(nFlag==COMTE_L2_HOST)
	{
		//level2行情主站
		g_L2HQHost.HostNum = GetTDXProfileInt("L2HOST","HostNum",0,pIniBuf);
		g_L2HQHost.HostNum = MAX(g_L2HQHost.HostNum,0);
		g_L2HQHost.HostNum = MIN(g_L2HQHost.HostNum,150);
		for(int i = 0;i < g_L2HQHost.HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetTDXProfileString("L2HOST",tempStr,"",g_L2HQHost.HostName[i],48,pIniBuf);
			tempStr.Format("IPAddress%02d",i+1);
			GetTDXProfileString("L2HOST",tempStr,"",g_L2HQHost.IPAddress[i],48,pIniBuf);
			tempStr.Format("Port%02d",i+1);
			g_L2HQHost.Port[i] = GetTDXProfileInt("L2HOST",tempStr,7709,pIniBuf);
			tempStr.Format("Weight%02d",i+1);
			g_L2HQHost.weight[i] = GetTDXProfileInt("L2HOST",tempStr,0,pIniBuf);
		}
	}
	else if(nFlag==COMTE_L2_USERHOST)
	{
		//用户管理服务器的配置:主认证服务器和辅认证服务器
		GetTDXProfileString("USERHOST","IPAddress","",g_UserHostIP,48,pIniBuf);
		g_UserHostIP[48]=0;
		g_nUserHostPort = GetTDXProfileInt("USERHOST","Port",7715,pIniBuf);
		//
		GetTDXProfileString("USERHOST","BakIPAddress","",g_BakUserHostIP,48,pIniBuf);
		g_BakUserHostIP[48]=0;
		g_nBakUserHostPort = GetTDXProfileInt("USERHOST","BakPort",7715,pIniBuf);
		//
		GetTDXProfileString("USERHOST","BakIPAddress2","",g_BakUserHostIP2,48,pIniBuf);
		g_BakUserHostIP2[48]=0;
		g_nBakUserHostPort2 = GetTDXProfileInt("USERHOST","BakPort2",7715,pIniBuf);
	}
	else if(nFlag==COMTE_FHOST)
	{
		//快速小行情主站
		GetTDXProfileString("FHOST","IPAddress","",g_FHostIP,48,pIniBuf);
		g_FHostIP[48]=0;
		g_nFHostPort = GetTDXProfileInt("FHOST","Port",7715,pIniBuf);
	}
	else if(nFlag==COMTE_TVHOST)
	{
		//快速小行情主站
		GetTDXProfileString("TVHOST","IPAddress","",g_TVHostIP,48,pIniBuf);
		g_TVHostIP[48]=0;
		g_nTVHostPort = GetTDXProfileInt("TVHOST","Port",4010,pIniBuf);
	}
	else if(nFlag==COMTE_DS_HOST)
	{
		//实时扩展行情主站
		g_SSDSHost.HostNum = GetTDXProfileInt("DSHOST","HostNum",0,pIniBuf);
		g_SSDSHost.HostNum = MAX(g_SSDSHost.HostNum,0);
		g_SSDSHost.HostNum = MIN(g_SSDSHost.HostNum,150);
		for(int i = 0;i < g_SSDSHost.HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetTDXProfileString("DSHOST",tempStr,"",g_SSDSHost.HostName[i],48,pIniBuf);
			tempStr.Format("IPAddress%02d",i+1);
			GetTDXProfileString("DSHOST",tempStr,"",g_SSDSHost.IPAddress[i],48,pIniBuf);
			tempStr.Format("Port%02d",i+1);
			g_SSDSHost.Port[i] = GetTDXProfileInt("DSHOST",tempStr,7721,pIniBuf);
			tempStr.Format("Weight%02d",i+1);
			g_SSDSHost.weight[i] = GetTDXProfileInt("DSHOST",tempStr,0,pIniBuf);
		}		
	}
	CloseTdxIni(pIniBuf);
	return TRUE;
}

void GetHostInfoFromFile(TDXWHOST *HqHost,TDXWHOST *InfoHost,TDXWHOST *WtHost,TDXWHOST *VipWtHost,TDXWHOST *DsHost,PROXYINFO *TdxProxyHost,struct VipVerify_Info *pVerifyInfo,BOOL bGetPrimary,CString Ini,BOOL bReplaceLevel2)
{
	int i;
	CString tempStr;
	//行情主站信息
	if(HqHost)
	{
		HqHost->PrimaryHost = -1;
		HqHost->HostNum = GetPrivateProfileInt("HQHOST","HostNum",0,Ini);
		HqHost->HostNum = MAX(HqHost->HostNum,0);
		HqHost->HostNum = MIN(HqHost->HostNum,150);
		for(i = 0;i < HqHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetPrivateProfileString("HQHOST",tempStr,"",HqHost->HostName[i],48,Ini);
			tempStr.Format("IPAddress%02d",i+1);
			GetPrivateProfileString("HQHOST",tempStr,"",HqHost->IPAddress[i],48,Ini);
			tempStr.Format("Port%02d",i+1);
			HqHost->Port[i] = GetPrivateProfileInt("HQHOST",tempStr,7709,Ini);
			tempStr.Format("Weight%02d",i+1);
			HqHost->weight[i] = GetPrivateProfileInt("HQHOST",tempStr,0,Ini);
			if(strstr(HqHost->HostName[i], "金牛"))
				HqHost->PrimaryHost = i;
		}
		if(bReplaceLevel2)
			*HqHost=g_L2HQHost;
		if(bGetPrimary&&HqHost->PrimaryHost<0)
		{
			HqHost->PrimaryHost = GetPrivateProfileInt("HQHOST","PrimaryHost",0,Ini);
			HqHost->PrimaryHost = MAX(0,HqHost->PrimaryHost);
			HqHost->PrimaryHost = MIN(HqHost->HostNum-1,HqHost->PrimaryHost);			
		}
	}
	//资讯主站信息
	if(InfoHost)
	{
		InfoHost->HostNum = GetPrivateProfileInt("INFOHOST","HostNum",0,Ini);
		InfoHost->HostNum = MAX(InfoHost->HostNum,0);
		InfoHost->HostNum = MIN(InfoHost->HostNum,150);
		if(bGetPrimary)
		{
			InfoHost->PrimaryHost = GetPrivateProfileInt("INFOHOST","PrimaryHost",0,Ini);
			InfoHost->PrimaryHost = MAX(0,InfoHost->PrimaryHost);
			InfoHost->PrimaryHost = MIN(InfoHost->HostNum-1,InfoHost->PrimaryHost);
		}
		for(i = 0;i < InfoHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetPrivateProfileString("INFOHOST",tempStr,"",InfoHost->HostName[i],48,Ini);
			tempStr.Format("IPAddress%02d",i+1);
			GetPrivateProfileString("INFOHOST",tempStr,"",InfoHost->IPAddress[i],48,Ini);
			tempStr.Format("Port%02d",i+1);
			InfoHost->Port[i] = GetPrivateProfileInt("INFOHOST",tempStr,7711,Ini);
			tempStr.Format("Weight%02d",i+1);
			InfoHost->weight[i] = GetPrivateProfileInt("INFOHOST",tempStr,0,Ini);
		}
	}
	//交易中心信息
	if(WtHost)
	{
		WtHost->HostNum=GetPrivateProfileInt("WTHOST","HostNum",0,Ini);
		WtHost->HostNum = MAX(WtHost->HostNum,0);
		WtHost->HostNum = MIN(WtHost->HostNum,150);
		if(bGetPrimary)
		{
			WtHost->PrimaryHost = GetPrivateProfileInt("WTHOST","PrimaryHost",0,Ini);
			WtHost->PrimaryHost = MAX(0,WtHost->PrimaryHost);
			WtHost->PrimaryHost = MIN(WtHost->HostNum-1,WtHost->PrimaryHost);
		}
		for(int i = 0;i < WtHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetPrivateProfileString("WTHOST",tempStr,"",WtHost->HostName[i],48,Ini);
			tempStr.Format("IPAddress%02d",i+1);
			GetPrivateProfileString("WTHOST",tempStr,"",WtHost->IPAddress[i],48,Ini);
			tempStr.Format("Port%02d",i+1);
			WtHost->Port[i] = GetPrivateProfileInt("WTHOST",tempStr,7708,Ini);
			tempStr.Format("Weight%02d",i+1);
			WtHost->weight[i] = GetPrivateProfileInt("WTHOST",tempStr,0,Ini);
		}
	}
	//VIP交易中心信息
	if(VipWtHost)
	{
		VipWtHost->HostNum=GetPrivateProfileInt("VipWtHost","HostNum",0,Ini);
		VipWtHost->HostNum = MAX(VipWtHost->HostNum,0);
		VipWtHost->HostNum = MIN(VipWtHost->HostNum,150);
		if(bGetPrimary)
		{
			VipWtHost->PrimaryHost = GetPrivateProfileInt("VipWtHost","PrimaryHost",0,Ini);
			VipWtHost->PrimaryHost = MAX(0,VipWtHost->PrimaryHost);
			VipWtHost->PrimaryHost = MIN(VipWtHost->HostNum-1,VipWtHost->PrimaryHost);
		}
		for(int i = 0;i < VipWtHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetPrivateProfileString("VipWtHost",tempStr,"",VipWtHost->HostName[i],48,Ini);
			tempStr.Format("IPAddress%02d",i+1);
			GetPrivateProfileString("VipWtHost",tempStr,"",VipWtHost->IPAddress[i],48,Ini);
			tempStr.Format("Port%02d",i+1);
			VipWtHost->Port[i] = GetPrivateProfileInt("VipWtHost",tempStr,7708,Ini);
			tempStr.Format("Weight%02d",i+1);
			VipWtHost->weight[i] = GetPrivateProfileInt("VipWtHost",tempStr,0,Ini);
		}
	}	
	//第三方服务器
	if(DsHost)
	{
		DsHost->HostNum = GetPrivateProfileInt("DsHost","HostNum",0,Ini);
		DsHost->HostNum = MAX(DsHost->HostNum,0);
		DsHost->HostNum = MIN(DsHost->HostNum,150);
		if(bGetPrimary)
		{
			DsHost->PrimaryHost = GetPrivateProfileInt("DSHOST","PrimaryHost",0,Ini);
			DsHost->PrimaryHost = MAX(0,DsHost->PrimaryHost);
			DsHost->PrimaryHost = MIN(DsHost->HostNum-1,DsHost->PrimaryHost);
		}
		for(i = 0;i < DsHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			GetPrivateProfileString("DSHOST",tempStr,"",DsHost->HostName[i],48,Ini);
			tempStr.Format("IPAddress%02d",i+1);
			GetPrivateProfileString("DSHOST",tempStr,"",DsHost->IPAddress[i],48,Ini);
			tempStr.Format("Port%02d",i+1);
			DsHost->Port[i] = GetPrivateProfileInt("DSHOST",tempStr,7721,Ini);
			tempStr.Format("Weight%02d",i+1);
			DsHost->weight[i] = GetPrivateProfileInt("DSHOST",tempStr,0,Ini);
		}
	}
	if(TdxProxyHost)
	{
		memset(TdxProxyHost,0,sizeof(PROXYINFO));
		GetPrivateProfileString("TDXPROXY","IP","",TdxProxyHost->tdxip[0],49,Ini);
		GetPrivateProfileString("TDXPROXY","Desc","",TdxProxyHost->tdxdesc[0],49,Ini);
		TdxProxyHost->tdxport[0] = GetPrivateProfileInt("TDXPROXY","Port",443,Ini);
		for(i=0;i<9;i++)
		{
			tempStr.Format("IP%d",i+2);
			GetPrivateProfileString("TDXPROXY",tempStr,"",TdxProxyHost->tdxip[i+1],49,Ini);
			if(strlen(TdxProxyHost->tdxip[i+1])==0) 
				break;
			tempStr.Format("Desc%d",i+2);
			GetPrivateProfileString("TDXPROXY",tempStr,"",TdxProxyHost->tdxdesc[i+1],49,Ini);
			tempStr.Format("Port%d",i+2);
			TdxProxyHost->tdxport[i+1] = GetPrivateProfileInt("TDXPROXY",tempStr,443,Ini);
		}
	}
	if(pVerifyInfo)
	{
		pVerifyInfo->RemoteVerify=GetPrivateProfileInt("UserHost","RemoteVerify",0,Ini);
		pVerifyInfo->ReVerifySpan=GetPrivateProfileInt("UserHost","ReVerifySpan",80,Ini);
		pVerifyInfo->DenyInfo=GetPrivateProfileInt("UserHost","DenyInfo",0,Ini);
	}
}

void WriteHostInfoToFile(TDXWHOST *HqHost,TDXWHOST *InfoHost,TDXWHOST *WtHost,TDXWHOST *VipWtHost,TDXWHOST *DsHost,PROXYINFO *TdxProxyHost,struct VipVerify_Info *pVerifyInfo,BOOL bSetPrimary,CString Ini)
{
	int i;
	CString tempStr;
	//行情主站信息
	if(HqHost && HqHost->HostNum>0)
	{
		if(bSetPrimary)
			WritePrivateProfileString("HQHOST","PrimaryHost",IntToStr(HqHost->PrimaryHost),Ini);
		WritePrivateProfileString("HQHOST","HostNum",IntToStr(HqHost->HostNum),Ini);
		for(i = 0;i < HqHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			WritePrivateProfileString("HQHOST",tempStr,HqHost->HostName[i],Ini);
			tempStr.Format("IPAddress%02d",i+1);
			WritePrivateProfileString("HQHOST",tempStr,HqHost->IPAddress[i],Ini);
			tempStr.Format("Port%02d",i+1);
			WritePrivateProfileString("HQHOST",tempStr,IntToStr(HqHost->Port[i]),Ini);
		}
	}
	//资讯主站信息
	if(InfoHost && InfoHost->HostNum>0)
	{
		WritePrivateProfileString("INFOHOST","HostNum",IntToStr(InfoHost->HostNum),Ini);
		if(bSetPrimary)
			WritePrivateProfileString("INFOHOST","PrimaryHost",IntToStr(InfoHost->PrimaryHost),Ini);
		for(i = 0;i < InfoHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			WritePrivateProfileString("INFOHOST",tempStr,InfoHost->HostName[i],Ini);
			tempStr.Format("IPAddress%02d",i+1);
			WritePrivateProfileString("INFOHOST",tempStr,InfoHost->IPAddress[i],Ini);
			tempStr.Format("Port%02d",i+1);
			WritePrivateProfileString("INFOHOST",tempStr,IntToStr(InfoHost->Port[i]),Ini);
			if(InfoHost->weight[i]>0)
			{
				tempStr.Format("Weight%02d",i+1);
				WritePrivateProfileString("INFOHOST",tempStr,IntToStr(InfoHost->weight[i]),Ini);
			}
		}
	}
	if(WtHost && WtHost->HostNum>0)
	{
		WritePrivateProfileString("WTHOST","HostNum",IntToStr(WtHost->HostNum),Ini);
		if(bSetPrimary)
			WritePrivateProfileString("WTHOST","PrimaryHost",IntToStr(WtHost->PrimaryHost),Ini);
		for(int i = 0;i < WtHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			WritePrivateProfileString("WTHOST",tempStr,WtHost->HostName[i],Ini);
			tempStr.Format("IPAddress%02d",i+1);
			WritePrivateProfileString("WTHOST",tempStr,WtHost->IPAddress[i],Ini);
			tempStr.Format("Port%02d",i+1);
			WritePrivateProfileString("WTHOST",tempStr,IntToStr(WtHost->Port[i]),Ini);
			if(WtHost->weight[i]>0)
			{
				tempStr.Format("Weight%02d",i+1);
				WritePrivateProfileString("WTHOST",tempStr,IntToStr(WtHost->weight[i]),Ini);
			}
		}		
	}
	if(VipWtHost && VipWtHost->HostNum>0)
	{
		WritePrivateProfileString("VipWtHost","HostNum",IntToStr(VipWtHost->HostNum),Ini);
		if(bSetPrimary)
			WritePrivateProfileString("VipWtHost","PrimaryHost",IntToStr(VipWtHost->PrimaryHost),Ini);
		for(int i = 0;i < VipWtHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			WritePrivateProfileString("VipWtHost",tempStr,VipWtHost->HostName[i],Ini);
			tempStr.Format("IPAddress%02d",i+1);
			WritePrivateProfileString("VipWtHost",tempStr,VipWtHost->IPAddress[i],Ini);
			tempStr.Format("Port%02d",i+1);
			WritePrivateProfileString("VipWtHost",tempStr,IntToStr(VipWtHost->Port[i]),Ini);
			if(VipWtHost->weight[i]>0)
			{
				tempStr.Format("Weight%02d",i+1);
				WritePrivateProfileString("VipWtHost",tempStr,IntToStr(VipWtHost->weight[i]),Ini);
			}
		}		
	}
	if(DsHost && DsHost->HostNum>0)
	{
		WritePrivateProfileString("DSHOST","HostNum",IntToStr(DsHost->HostNum),Ini);
		if(bSetPrimary)
			WritePrivateProfileString("DSHOST","PrimaryHost",IntToStr(DsHost->PrimaryHost),Ini);
		for(i = 0;i < DsHost->HostNum;i++)
		{
			tempStr.Format("HostName%02d",i+1);
			WritePrivateProfileString("DSHOST",tempStr,DsHost->HostName[i],Ini);
			tempStr.Format("IPAddress%02d",i+1);
			WritePrivateProfileString("DSHOST",tempStr,DsHost->IPAddress[i],Ini);
			tempStr.Format("Port%02d",i+1);
			WritePrivateProfileString("DSHOST",tempStr,IntToStr(DsHost->Port[i]),Ini);
			if(DsHost->weight[i]>0)
			{
				tempStr.Format("Weight%02d",i+1);
				WritePrivateProfileString("DSHOST",tempStr,IntToStr(DsHost->weight[i]),Ini);
			}
		}
	}
	if(TdxProxyHost && strlen(TdxProxyHost->tdxip[0])>0)
	{
		WritePrivateProfileString("TDXPROXY","IP",TdxProxyHost->tdxip[0],Ini);
		WritePrivateProfileString("TDXPROXY","Desc",TdxProxyHost->tdxdesc[0],Ini);
		WritePrivateProfileString("TDXPROXY","Port",IntToStr(TdxProxyHost->tdxport[0]),Ini);
		for(i=0;i<9;i++)
		{
			if(strlen(TdxProxyHost->tdxip[i+1])==0) 
				break;
			tempStr.Format("IP%d",i+2);
			WritePrivateProfileString("TDXPROXY",tempStr,TdxProxyHost->tdxip[i+1],Ini);
			tempStr.Format("Desc%d",i+2);
			WritePrivateProfileString("TDXPROXY",tempStr,TdxProxyHost->tdxdesc[i+1],Ini);
			tempStr.Format("Port%d",i+2);
			WritePrivateProfileString("TDXPROXY",tempStr,IntToStr(TdxProxyHost->tdxport[i+1]),Ini);
		}
	}
	if(pVerifyInfo && pVerifyInfo->RemoteVerify)
	{
		WritePrivateProfileString("UserHost","ReVerifySpan",IntToStr(pVerifyInfo->ReVerifySpan),Ini);
		WritePrivateProfileString("UserHost","DenyInfo",pVerifyInfo->DenyInfo?"1":"0",Ini);
	}
}

BOOL GetPrimaryHQServerInfo(CString &strAddress,UINT &nPort,BOOL bSwitchNext)
{
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	TDXWHOST host;
	CString hname,iress,nport;
	memset(&host,0,sizeof(TDXWHOST));
	host.HostNum=GetPrivateProfileInt("HQHOST","HostNum",0,Ini);
	host.HostNum = max(host.HostNum,0);
	host.HostNum = min(host.HostNum,150);
	host.PrimaryHost=GetPrivateProfileInt("HQHOST","PrimaryHost",0,Ini);
	host.PrimaryHost = max(0,host.PrimaryHost);
	host.PrimaryHost = min(host.HostNum-1,host.PrimaryHost);
	for(int i=0;i<host.HostNum;i++)
	{
		hname.Format("HostName%02d",i+1);
		iress.Format("IPAddress%02d",i+1);
		nport.Format("Port%02d",i+1);
		TDX_GetPrivateProfileString("HQHOST",hname,"",host.HostName[i],50,Ini);
		TDX_GetPrivateProfileString("HQHOST",iress,"",host.IPAddress[i],50,Ini);
		host.Port[i]=GetPrivateProfileInt("HQHOST",nport,7709,Ini);
	}
	if(host.HostNum<1)
		return FALSE;
	if(bSwitchNext)
		host.PrimaryHost=(host.PrimaryHost+1)%host.HostNum;
	strAddress=host.IPAddress[host.PrimaryHost];
	nPort=host.Port[host.PrimaryHost];
	return TRUE;
}

void GetEmbOEMCfg(struct Global_EmbOem *pCfg,CString Ini)
{
	pCfg->bWebUseInputZjzh = GetPrivateProfileInt("EMBUI","WebUseInputZjzh",0,Ini);	
	pCfg->bHasTryout = GetPrivateProfileInt("EMBUI","HasTryout",0,Ini);
	pCfg->bHasOutStandTryOut = GetPrivateProfileInt("EMBUI","HasOutStandTryout",0,Ini);
	pCfg->bNoHelpBtnInLogo = GetPrivateProfileInt("EMBUI","NoHelpBtnInLogo",0,Ini);
	pCfg->bHasSoftKeyCaption = GetPrivateProfileInt("EMBUI","HasSoftKeyCaption",0,Ini);
	GetPrivateProfileString("EMBUI","SysTestCaption","系统检测",pCfg->SysTestCaption,39,Ini);
	GetPrivateProfileString("EMBUI","Sel1Caption","行情+交易",pCfg->Sel1Caption,39,Ini);
	GetPrivateProfileString("EMBUI","Sel2Caption","独立行情",pCfg->Sel2Caption,39,Ini);
	if(pCfg->bHasOutStandTryOut)
		strcpy(pCfg->Sel2Caption,"试用行情");
	GetPrivateProfileString("EMBUI","Sel3Caption","独立交易",pCfg->Sel3Caption,39,Ini);	
	pCfg->bWritePreNameInLogo = GetPrivateProfileInt("EMBUI","WritePreNameInLogo",1,Ini);
	pCfg->bWriteVersionInLogo = GetPrivateProfileInt("EMBUI","WriteVersionInLogo",0,Ini);
	pCfg->bMediumFontInLogo = GetPrivateProfileInt("EMBUI","MediumFontInLogo",0,Ini);
	pCfg->bCustomQsZXTree = GetPrivateProfileInt("EMBUI","CustomQsZXTree",0,Ini);
	GetPrivateProfileString("EMBUI","CustomQsZxTreeName","研究资讯",pCfg->CustomQsZxTreeName,39,Ini);
	pCfg->bCustomQsZxPosSecond = GetPrivateProfileInt("EMBUI","CustomQsZxPosSecond",0,Ini);
	pCfg->bStatusInfoLeftAlign = GetPrivateProfileInt("EMBUI","StatusInfoLeftAlign",0,Ini);
	GetPrivateProfileString("EMBUI","ResDllName","res_std.dll",pCfg->ResDllName,39,Ini);
	GetPrivateProfileString("EMBUI","SpecialCfgFileName","stdqs",pCfg->SpecialCfgFileName,39,Ini);
	pCfg->bNoExpandTitleInQsZXTree = GetPrivateProfileInt("EMBUI","NoExpandTitleInQsZXTree",0,Ini);
	pCfg->bOutStandCusWebPageBack = GetPrivateProfileInt("EMBUI","OutStandCusWebPageBack",0,Ini);
	pCfg->bOppositeMngRight = GetPrivateProfileInt("EMBUI","OppositeMngRight",0,Ini);
	pCfg->bFormatAndUseYYBID = GetPrivateProfileInt("EMBUI","FormatAndUseYYBID",0,Ini);
	pCfg->bHasRelativeInfo = GetPrivateProfileInt("EMBUI","HasRelativeInfo",0,Ini);
	pCfg->bRealInfoZstFlush = GetPrivateProfileInt("EMBUI","RealInfoZstFlush",0,Ini);
	GetPrivateProfileString("EMBUI","ComboBoxKHWTName","",pCfg->ComboBoxKHWTName,39,Ini);
	GetPrivateProfileString("EMBUI","ComboBoxSHHYName","",pCfg->ComboBoxSHHYName,39,Ini);
	GetPrivateProfileString("EMBUI","DescKHWTName","",pCfg->DescKHWTName,39,Ini);
	GetPrivateProfileString("EMBUI","DescSHHYName","",pCfg->DescSHHYName,39,Ini);	
	pCfg->bGgxxTopInZxTree = GetPrivateProfileInt("EMBUI","GgxxTopInZxTree",0,Ini);
	pCfg->bMainWebPageInsideOpen = GetPrivateProfileInt("EMBUI","MainWebPageInsideOpen",0,Ini);
	pCfg->bHasQsZXOnlyLoginJy = GetPrivateProfileInt("EMBUI","HasQsZXOnlyLoginJy",0,Ini);
	pCfg->bHasMetaCJZX = GetPrivateProfileInt("EMBUI","HasMetaCJZX",0,Ini);
	pCfg->bMetaUseWebProfile = GetPrivateProfileInt("EMBUI","MetaUseWebProfile",0,Ini);
	pCfg->nHintCannotUseMinMetaRight = GetPrivateProfileInt("EMBUI","HintCannotUseMinMetaRight",0,Ini);
	pCfg->bTopShowMetaRightInfo = GetPrivateProfileInt("EMBUI","TopShowMetaRightInfo",0,Ini);
	pCfg->bHintCannotUseMeta = GetPrivateProfileInt("EMBUI","HintCannotUseMeta",0,Ini);
	pCfg->bUrgentUseQSIcon = GetPrivateProfileInt("EMBUI","UrgentUseQSIcon",1,Ini);
	pCfg->bWebButtonInCaption = GetPrivateProfileInt("EMBUI","WebButtonInCaption",0,Ini);
	pCfg->bRightWebBtn = GetPrivateProfileInt("EMBUI","RightWebBtn",0,Ini);
	GetPrivateProfileString("EMBUI","WebButtonURL","",pCfg->strWebButtonURL,99,Ini);
	GetPrivateProfileString("EMBUI","CaptionQSWebURL","",pCfg->strCaptionQSWebURL,99,Ini);	
	GetPrivateProfileString("EMBUI","CaptionHQURL","",pCfg->strCaptionHQURL,99,Ini);
	GetPrivateProfileString("EMBUI","VsatHttp","",pCfg->strVsatHttp,99,Ini);
	pCfg->bShowPreNameInTopCenter = GetPrivateProfileInt("EMBUI","ShowPreNameInTopCenter",0,Ini);
	pCfg->bNoSwitchInLogo = GetPrivateProfileInt("EMBUI","NoSwitchInLogo",0,Ini);
	pCfg->bHasTdxTend = GetPrivateProfileInt("EMBUI","HasTdxTend",0,Ini);
	pCfg->bWhiteForeColor = GetPrivateProfileInt("EMBUI","WhiteForeColor",0,Ini);
	pCfg->bWhiteBtnForeColor = GetPrivateProfileInt("EMBUI","WhiteBtnForeColor",0,Ini);
	pCfg->bHideCloseBt = GetPrivateProfileInt("EMBUI","HideCloseBt",0,Ini);
	pCfg->bUseBtn2Back = GetPrivateProfileInt("EMBUI","UseBtn2Back",0,Ini);
	pCfg->nL2Cfg_Shift_left = GetPrivateProfileInt("EMBUI","L2Cfg_Shift_left",0,Ini);
	pCfg->nL2Cfg_Shift_right = GetPrivateProfileInt("EMBUI","L2Cfg_Shift_right",0,Ini);
	pCfg->nL2Cfg_Shift_top = GetPrivateProfileInt("EMBUI","L2Cfg_Shift_top",0,Ini);
	pCfg->nL2Cfg_Shift_bottom = GetPrivateProfileInt("EMBUI","L2Cfg_Shift_bottom",0,Ini);

	pCfg->bF9WTUseF12Key = GetPrivateProfileInt("EMBUI","F9WTUseF12Key",0,Ini);
	pCfg->bF12SelectWT = GetPrivateProfileInt("EMBUI","F12SelectWT",0,Ini);
	pCfg->bHideJYControl = GetPrivateProfileInt("EMBUI","HideJYControl",0,Ini);

	pCfg->nLogoMaxPassordLen = GetPrivateProfileInt("EMBUI","LogoMaxPassordLen",30,Ini);
	if(pCfg->nLogoMaxPassordLen>30 || pCfg->nLogoMaxPassordLen<6)
		pCfg->nLogoMaxPassordLen=30;
	pCfg->nLogoMaxAuthPassLen = GetPrivateProfileInt("EMBUI","LogoMaxAuthPassLen",30,Ini);
	if(pCfg->nLogoMaxAuthPassLen>30 || pCfg->nLogoMaxAuthPassLen<6)
		pCfg->nLogoMaxAuthPassLen=30;
	
	GetPrivateProfileString("EMBUI","TendName","",pCfg->TendName,39,Ini);	
	pCfg->OnlyJYTend=GetPrivateProfileInt("EMBUI","OnlyJYTend",0,Ini);

	pCfg->bHasQSRun = GetPrivateProfileInt("EMBUI","QSRun",0,Ini);
	pCfg->bDSMustJy = GetPrivateProfileInt("EMBUI","DSMustJy",0,Ini);
	pCfg->bShowTwoHostName = GetPrivateProfileInt("EMBUI","ShowTwoHostName",0,Ini);
	pCfg->bAlwaysSoftKey = GetPrivateProfileInt("EMBUI","AlwaysSoftKey",0,Ini);
	pCfg->bOutStandZXGHY = GetPrivateProfileInt("EMBUI","OutStandZXGHY",0,Ini);
	pCfg->bNoAdvert = GetPrivateProfileInt("EMBUI","NoAdvert",0,Ini);

	//
	GetPrivateProfileString("AQFS_DESC","tdxssl_desc","SSL",pCfg->SSLAqfsName,39,g_WSXHStr+g_strYxhjCfg);
	GetPrivateProfileString("AQFS_DESC","authpass_desc","认证口令",pCfg->AuthPassAqfsName,39,g_WSXHStr+g_strYxhjCfg);
	GetPrivateProfileString("AQFS_DESC","tdxca_desc","CA证书",pCfg->TdxCAAqfsName,39,g_WSXHStr+g_strYxhjCfg);
	GetPrivateProfileString("AQFS_DESC","sms_desc","短信口令",pCfg->SmsAqfsName,39,g_WSXHStr+g_strYxhjCfg);
	GetPrivateProfileString("AQFS_DESC","tokencard_desc","令牌卡",pCfg->TokenCardAqfsName,39,g_WSXHStr+g_strYxhjCfg);
	GetPrivateProfileString("AQFS_DESC","ukey_desc","EPASS独立登陆",pCfg->UKeyAqfsName,39,g_WSXHStr+g_strYxhjCfg);
	//以下是TC6特有的
	pCfg->bCanTryoutUser = GetPrivateProfileInt("EMBUI","CanTryoutUser",0,Ini);
	pCfg->bCanActiveUser = GetPrivateProfileInt("EMBUI","CanActiveUser",0,Ini);
	pCfg->bCanUpgradeUser = GetPrivateProfileInt("EMBUI","CanUpgradeUser",0,Ini);
	pCfg->bMetaRightFromZXCookie = GetPrivateProfileInt("EMBUI","MetaRightFromZXCookie",0,Ini);
	pCfg->bFirstHideWTFace = GetPrivateProfileInt("EMBUI","FirstHideWTFace",0,Ini);
	pCfg->bFirstOpenZXTree = GetPrivateProfileInt("EMBUI","FirstOpenZXTree",0,Ini);
}

void GetDSOtherCfg()
{
	CString	DS_Ini = HomePath+"dsmarket.dat";
	GetPrivateProfileString("OEM","DSOEMName","扩展行情",g_DS.DSOEMName,48,DS_Ini);
	g_DS.DSTabFirst=GetPrivateProfileInt("OEM","DSTabFirst",0,DS_Ini);
	g_DS.HasHKMarket=GetPrivateProfileInt("OEM","HasHKMarket",0,DS_Ini);
	g_DS.HasHKYSInfo=GetPrivateProfileInt("OEM","HasHKYSInfo",0,DS_Ini);
	g_DS.MustInGUI=GetPrivateProfileInt("OEM","MustInGUI",0,DS_Ini);

}

void GetDSMarketCfg(CString DS_Ini)
{
	g_GuiMarketNum = 0;
	memset(g_GuiMarketSet, 0, sizeof(g_GuiMarketSet));
	char SetStr[MAX_MKT_NUM*(MRK_NAME_SIZE+1)+1]={0};
	CString tempStr;
	tempStr.Format("GUIMarket%02d",g_GuiMarketNum+1);
	DWORD nameLen = GetPrivateProfileString("GUISet",tempStr,"",g_GuiMarketSet[0].BigsetName,31,DS_Ini);
	while(nameLen>0&&g_GuiMarketNum<=MAX_MKTGUI_NUM)
	{
		int  j,argc=0; 
		char *argv[MAX_MKT_NUM];
		//读取类型
		tempStr.Format("GUIMarketSet%02d",g_GuiMarketNum+1);
		GetPrivateProfileString("GUISet",tempStr,"",SetStr,MAX_MKT_NUM*(MRK_NAME_SIZE+1),DS_Ini);
		argc = GetCmdLine(SetStr,argv,MAX_MKT_NUM,",");
		for(j=0;j<argc;j++)
			g_GuiMarketSet[g_GuiMarketNum].MarketSet[j] = atoi(argv[j]);
		//读取类型名
		tempStr.Format("GUIMarketName%02d",g_GuiMarketNum+1);
		GetPrivateProfileString("GUISet",tempStr,"",SetStr,MAX_MKT_NUM*(MRK_NAME_SIZE+1),DS_Ini);
		argc = GetCmdLine(SetStr,argv,MAX_MKT_NUM,",");
		for(j=0;j<argc;j++)
			strcpy(g_GuiMarketSet[g_GuiMarketNum].MarketName[j],argv[j]);
		
		g_GuiMarketNum++;
		tempStr.Format("GUIMarket%02d",g_GuiMarketNum+1);
		nameLen = GetPrivateProfileString("GUISet",tempStr,"",g_GuiMarketSet[g_GuiMarketNum].BigsetName,31,DS_Ini);
	}
	g_LocalGuiMarketNum=g_GuiMarketNum;
}

//////////////////////////////////////////////////////////////////////////
//加载经纪席位数据
void	LoadSeatData()
{
	FILE *fp_brkseat = _fsopen(g_WSXHStr+"LData\\cache\\brkseat.dat","rb",_SH_DENYNO);
	if(!fp_brkseat) return;
	g_mSeat2Com.clear();
	int i=0,argc=0;
	char	lpString[256]={0},*argv[4]={0};
	for(rewind(fp_brkseat),i=0;!feof(fp_brkseat);++i)
	{
		memset(lpString,0,256);
		fgets(lpString,255,fp_brkseat);
		GetCmdLine(lpString,argv,4, ",|");

		Seat_Code2Com scn={0};
		long SeatID = -1;
		if(argv[0]) scn.ComID = atoi(argv[0]);
		if(argv[1]) scn.setCode = atoi(argv[1]);
		if(argv[2]) SeatID = atoi(argv[2]);
		if(SeatID>=0)
			g_mSeat2Com[SeatID] = scn;
	}
	fclose(fp_brkseat);

	FILE *fp_brkcomp = _fsopen(g_WSXHStr+"LData\\cache\\brkcomp.dat","rb",_SH_DENYNO);
	if(!fp_brkcomp) return;
	g_mSeatCode2Name.clear();
	for(rewind(fp_brkcomp),i=0;!feof(fp_brkcomp);++i)
	{
		memset(lpString,0,256);
		fgets(lpString,255,fp_brkcomp);
		GetCmdLine(lpString,argv,4, ",|");

		Seat_Com2Name scn2={0};
		long CompID = -1;
		if(argv[0]) CompID = atoi(argv[0]);
		if(argv[1]) strncpy(scn2.ComNameS, argv[1], 20);
		if(argv[2]) strncpy(scn2.ComNameL, argv[2], 60);
		if(CompID>=0)
			g_mSeatCode2Name[CompID] = scn2;
	}
	fclose(fp_brkseat);
}

BOOL	GetSeatNameFromSID(long nSeatID, char *strSeatNameS,char *strSeatNameL)
{
	map<long, Seat_Code2Com, greater<long> >::iterator iter = g_mSeat2Com.find(nSeatID);
	if(iter==g_mSeat2Com.end()) return FALSE;

	Seat_Code2Com scn = (*iter).second;
	long	CompID = scn.ComID;

	map<long, Seat_Com2Name, greater<long> >::iterator iter2 = g_mSeatCode2Name.find(CompID);
	if(iter2==g_mSeatCode2Name.end()) return FALSE;

	Seat_Com2Name scn2 = (*iter2).second;
	if(strSeatNameS)
		strcpy(strSeatNameS, scn2.ComNameS);
	if(strSeatNameL)
		strcpy(strSeatNameL, scn2.ComNameL);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//加载用户快捷设置

void	LoadShortCutCfg()
{
	FILE *fp_shortcut = _fsopen(g_WSXHStr+"shortcut.dat","rb",_SH_DENYNO);
	if(!fp_shortcut) return;
	g_mShortCut.clear();
	
	int i=0,argc=0;
	char	lpString[256]={0},*argv[4]={0};
	for(rewind(fp_shortcut),i=0;!feof(fp_shortcut);++i)
	{
		memset(lpString,0,256);
		fgets(lpString,255,fp_shortcut);
		if(strlen(lpString)>4)
		{
			GetCmdLine(lpString,argv,4, ",|");

			CString strShortCut=argv[0];
			ShortCut_Cfg scn={0};
			scn.setcode=atoi(argv[1]);
			strncpy(scn.RawCode,argv[2],SH_CODE_LEN);

			g_mShortCut[strShortCut] = scn;
		}
	}
	fclose(fp_shortcut);
}

void	SaveShortCutCfg()
{
	FILE *fp_shortcut = _fsopen(g_WSXHStr+"shortcut.dat","wb",_SH_DENYNO);
	if(!fp_shortcut) return;

	if(g_mShortCut.size()>0)
	{
		CString tmpLine;
		map<CString, ShortCut_Cfg, cmpstr >::iterator iter = g_mShortCut.begin();	
		for(; iter!=g_mShortCut.end(); ++iter)
		{
			ShortCut_Cfg scn = (*iter).second;
			tmpLine.Format("%s,%d,%s\r\n",(*iter).first,scn.setcode,scn.RawCode);
			fputs(tmpLine,fp_shortcut);
		}
	}
	fclose(fp_shortcut);
}

void GetPasswordFromRegister(const char *pKeyString, CString & strUserName,  CString & strPassword, long &nLastValidDate, short &nLastValidValue)
{
	HKEY  hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,HQ_SUB_KEY,0,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)
    {
		int len = 100,type = REG_NONE;
		char *pbData = new char[len];
		memset(pbData, 0, len);
		if(RegQueryValueEx(hKey,pKeyString,NULL,(unsigned long *)&type,(LPBYTE)pbData,(LPDWORD)&len) == ERROR_SUCCESS)
		{
			len = strlen(pbData);
			for(int i=0;i<len;i++)
				pbData[i]-=1;
			char *argv[4]={0};
			GetCmdLine(pbData, argv, 3, "|\r\n");
			if(argv[0]) strUserName = argv[0];
			if(argv[1]) strPassword = argv[1];
			if(argv[2]) 
			{
				long nVV = atol(argv[2]);
				nLastValidDate = nVV/10;
				nLastValidValue = nVV%10;
			}
		}
		delete pbData;
		RegCloseKey(hKey);			
	}
}

void WritePasswordToRegister(const char *pKeyString, char *strUserName, char *strPassword, long &nLastValidDate, short &nLastValidValue)
{
	HKEY  hKey;
	DWORD dwDisp;
	if(RegCreateKeyEx(HKEY_CURRENT_USER,HQ_SUB_KEY,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKey,&dwDisp) == ERROR_SUCCESS)
	{
		int namelen=strlen(strUserName);
		int passlen=strlen(strPassword);
		char *tempstr= new char[(namelen+1)+(passlen+1)+(8+1)+1];
		memset(tempstr, 0, namelen+1+passlen+1+8+1+1);
		for(int i=0;i<namelen;i++)
			tempstr[i]=strUserName[i]+1;
		tempstr[i++]='|'+1;
		for(int j=0;j<passlen;j++)
			tempstr[i++]=strPassword[j]+1;
		tempstr[i++]='|'+1;
		
		char LastValid[10]={0};
		sprintf(LastValid, "%08d%01d", nLastValidDate, nLastValidValue);
		for(j=0;j<9;j++)
			tempstr[i++] = LastValid[j]+1;
		tempstr[i++] = 0;
		
		RegSetValueEx(hKey,pKeyString,0,REG_NONE,(LPBYTE)tempstr,(namelen+1)+(passlen+1)+(8+1)+1);
		delete tempstr;
		RegCloseKey(hKey);
	}
}

void GetExecPathFromRegister(const char *pKeyPath,const char *pKeyString,CString & strExecPath)
{
	strExecPath = "";
	HKEY  tmpHKEY = HKEY_CURRENT_USER;
	char *pTmpPath = strstr(pKeyPath,"HKEY_LOCAL_MACHINE");
	if(pTmpPath)
	{
		tmpHKEY = HKEY_LOCAL_MACHINE;
		pTmpPath+=19;
	}
	else
	{
		pTmpPath = strstr(pKeyPath,"HKEY_CURRENT_USER");
		if(!pTmpPath)	return;
		pTmpPath+=18;
	}
	HKEY  hKey;
	if(RegOpenKeyEx(tmpHKEY,pTmpPath,0,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)
    {
		int		len = 1023,type = REG_SZ;
		char	pbData[1024]={0};
		if(RegQueryValueEx(hKey,pKeyString,NULL,(unsigned long *)&type,(LPBYTE)pbData,(LPDWORD)&len) == ERROR_SUCCESS)
			strExecPath = pbData;
		RegCloseKey(hKey);			
	}
}

//通知Shell有关变化
void NotifyShell(LONG wEventId,LPSTR szPath)
{
	SHChangeNotify(wEventId,SHCNF_FLUSH|SHCNF_PATH,szPath,0);
	//取得szPath的父目录
	char*  p;   
	for(p=szPath+lstrlen(szPath)-1;*p!='\\';p--);
	*p='\0';
	SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT,SHCNF_FLUSH|SHCNF_PATH,szPath,0);
}   
    
//创建快捷方式   
BOOL CreateLink(LPSTR szPath,LPSTR szLink)
{   
	HRESULT			hres;   
	IShellLink *	psl;   
	IPersistFile*	ppf;   
	WORD			wsz[MAX_PATH];
    
	//初始化COM
	CoInitialize(NULL);   
	//创建一个IShellLink实例   
    hres = CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void **)&psl);
	if(FAILED(hres))   
	{
		CoUninitialize();
		return FALSE;   
	}
	//设置目标应用程序   
    psl->SetPath(szPath);    
	//设置快捷键(此处设为Shift+Ctrl+'R')   
	//psl->SetHotkey(MAKEWORD('R',HOTKEYF_SHIFT|HOTKEYF_CONTROL));       
	char* p;   
    for(p=szPath+lstrlen(szPath)-1;*p!='\\';p--);   
	*p='\0';
	psl->SetWorkingDirectory(szPath);    
	//从IShellLink获取其IPersistFile接口
	//用于保存快捷方式的数据文件(*.lnk)
	hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);    
	if(FAILED(hres))
	{
		CoUninitialize();
		return FALSE;
	}    
	//确保数据文件名为ANSI格式   
	MultiByteToWideChar(CP_ACP,0,szLink,-1,wsz,MAX_PATH);   
    //调用IPersistFile::Save   
    //保存快捷方式的数据文件(*.lnk)   
	hres=ppf->Save(wsz,STGM_READWRITE);
	//释放IPersistFile和IShellLink接口
	ppf->Release();
	psl->Release();   
    
	CoUninitialize();   
	return TRUE;   
}

