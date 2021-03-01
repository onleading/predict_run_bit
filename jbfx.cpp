#include "StdAfx.h"
#include "jbfx.h"

JBFX::JBFX()
{	
}

JBFX::~JBFX()
{
}

BOOL JBFX::GetJBScheme()
{
	memset(&g_JBScheme,0,sizeof(struct JBFX_Scheme));
	//
	g_JBScheme.FrameColor = RGB(192,192,192);
	g_JBScheme.BkColor = GridColor.GridBackColor;
	g_JBScheme.DataBkColor = GridColor.GridBackColor;
	g_JBScheme.DataBkColor2 = RGB(128,128,128);
	g_JBScheme.DataTextColor = GridColor.Grid_Level_Color;
	g_JBScheme.ListLineColor = GridColor.GridLineColor;
	g_JBScheme.SelectedColor = GridColor.GridHighLineColor^GridColor.GridBackColor;
	g_JBScheme.HeadTextColor = RGB(0,0,0);
	g_JBScheme.HeadBkColor = RGB(192,192,192);
	g_JBScheme.ScrollBarColor = VipColor.AXISCOLOR;
	g_JBScheme.CodeNameColor = GridColor.Grid_CodeName_Color;
	g_JBScheme.VolumeColor = GridColor.Grid_Volume_Color;
	g_JBScheme.UpColor = GridColor.Grid_Up_Color;
	g_JBScheme.DownColor = GridColor.Grid_Down_Color;
	g_JBScheme.StateColor[0] = RGB(145, 255, 0);
	g_JBScheme.StateColor[1] = RGB(10, 0, 220);
	g_JBScheme.StateColor[2] = RGB(192, 0, 141);
	g_JBScheme.StateColor[3] = RGB(38,38,166);
	g_JBScheme.StateColor[4] = RGB(255, 192, 0);
	//
	g_JBScheme.ListFont.lfHeight = 14;
	g_JBScheme.ListFont.lfWeight = FW_MEDIUM;
	strcpy(g_JBScheme.ListFont.lfFaceName,SONG_FONT);
	g_JBScheme.TitleFont = g_JBScheme.ListFont;
	
	return TRUE;
}

//基本分析类的初始化
BOOL JBFX::InitJBFX()
{
	memset(&g_MenuGN, 0, sizeof(struct MenuGN_Cfg));
	memset(&g_MainGN,0,sizeof(struct MainGN_Cfg));
	g_pGN=NULL;
	return TRUE;
}

//基本分析类的回收
void JBFX::UnInitJBFX()
{
	EmptyCwCfg();
}

//得到一个具体功能的配置信息
void JBFX::GetOneGNCfg(struct GN_Cfg *pGN,const char *GNIni)
{
	CString tmpDomainStr;
	pGN->GNNum=GetPrivateProfileInt("GN","GNNum",0,GNIni);
	pGN->GNNum=max(0,pGN->GNNum);
	pGN->GNNum=min(JBFX_MAX_SUBGN,pGN->GNNum);
	
	pGN->GNList = new struct One_GN_Cfg[pGN->GNNum+1];
	memset(pGN->GNList,0,(pGN->GNNum+1)*sizeof(struct One_GN_Cfg));
	for(int i=0;i<pGN->GNNum;i++)
	{		
		tmpDomainStr.Format("GN%d",i+1);
		GetPrivateProfileString(tmpDomainStr,"GNCode","N/A",pGN->GNList[i].GNCode,18,GNIni);
		GetPrivateProfileString(tmpDomainStr,"GNName","N/A",pGN->GNList[i].GNName,28,GNIni);
		GetPrivateProfileString(tmpDomainStr,"GNDesc","N/A",pGN->GNList[i].GNDesc,98,GNIni);
		pGN->GNList[i].SwitchColor=GetPrivateProfileInt(tmpDomainStr,"SwitchColor",RGB(0,0,0),GNIni);
	}
}

//得到配置信息
void JBFX::GetCwCfg()
{
	EmptyCwCfg();
	//
	GetJBScheme();
	//
	CString tmpStr;
	int		i;
	CString MenuGNIni=HomePath+"cwcfg\\maingn.ini";
	g_MenuGN.MenuGNNum = GetPrivateProfileInt("MainMenu", "MenuNum", 0, MenuGNIni);
	g_MenuGN.MenuGNNum=max(0,g_MenuGN.MenuGNNum);
	g_MenuGN.MenuGNNum=min(JBFX_MAX_MENUGN,g_MenuGN.MenuGNNum);
	int nMenuNum = 0;
	for(i=0;i<g_MenuGN.MenuGNNum;i++)
	{
		tmpStr.Format("MenuCode%d",i+1);
		char TmpCode[2]={0};
		GetPrivateProfileString("MainMenu", tmpStr, "", TmpCode, 2, MenuGNIni);
		if(strlen(TmpCode)!=1) continue;
		tmpStr.Format("UserUse%d",i+1);
		int nUse = GetPrivateProfileInt("MainMeun", tmpStr, 1, MenuGNIni);
		if(!nUse) continue;
		g_MenuGN.MenuGNCode[nMenuNum] = TmpCode[0];
		g_MenuGN.UseIt[nMenuNum]=nUse;
		tmpStr.Format("MenuName%d",i+1);
		GetPrivateProfileString("MainMenu", tmpStr, "", g_MenuGN.MenuGNName[nMenuNum], 30, MenuGNIni);
		tmpStr.Format("MenuGuide%d",i+1);
		GetPrivateProfileString("MainMenu", tmpStr, "", g_MenuGN.MenuGuide[nMenuNum], MAX_GUIDE_NAMELEN-1, MenuGNIni);

		++nMenuNum;
	}
	g_MenuGN.MenuGNNum = nMenuNum;
	//
	CString MainGNIni=HomePath+"cwcfg\\maingn.ini",GNIni;
	g_MainGN.GNNum=GetPrivateProfileInt("MAINGN","ItemNum",0,MainGNIni);
	g_MainGN.GNNum=max(0,g_MainGN.GNNum);
	g_MainGN.GNNum=min(JBFX_MAX_MAINGN,g_MainGN.GNNum);
	int nMainGNNum = 0;
	for(i=0;i<g_MainGN.GNNum;i++)
	{
		tmpStr.Format("UserUse%d",i+1);
		int nUse = GetPrivateProfileInt("MAINGN", tmpStr, 1, MainGNIni);
		if(!nUse) continue;
		
		tmpStr.Format("ItemCode%d",i+1);
		GetPrivateProfileString("MAINGN",tmpStr,"",g_MainGN.GNCode[nMainGNNum],18,MainGNIni);
		tmpStr.Format("ItemName%d",i+1);
		GetPrivateProfileString("MAINGN",tmpStr,"N/A",g_MainGN.GNName[nMainGNNum],28,MainGNIni);
		tmpStr.Format("ItemDesc%d",i+1);
		GetPrivateProfileString("MAINGN",tmpStr,"N/A",g_MainGN.GNDesc[nMainGNNum],98,MainGNIni);
		tmpStr.Format("GuideUrl%d",i+1);
		GetPrivateProfileString("MAINGN",tmpStr,"",g_MainGN.GNGuide[nMainGNNum],MAX_GUIDE_NAMELEN-1,MainGNIni);
		tmpStr.Format("SwitchColor%d",i+1);
		g_MainGN.SwitchColor[nMainGNNum]=GetPrivateProfileInt("MAINGN",tmpStr,RGB(0,0,0),MainGNIni);
		
		++nMainGNNum;
	}
	g_MainGN.GNNum = nMainGNNum;
	//
	g_pGN = new struct GN_Cfg[nMainGNNum+1];
	memset(g_pGN,0,(nMainGNNum+1)*sizeof(struct GN_Cfg));
	for(i=0;i<nMainGNNum;i++)
	{
		GNIni.Format("%scwcfg\\%s.ini",HomePath,g_MainGN.GNCode[i]);
		GetOneGNCfg(g_pGN+i,GNIni);
	}	
}

//清空配置信息
void JBFX::EmptyCwCfg()
{
	for(int i=0;i<g_MainGN.GNNum;i++)
	{
		TDEL(g_pGN[i].GNList);
	}
	g_MainGN.GNNum=0;
	//
	TDEL(g_pGN);	
}
