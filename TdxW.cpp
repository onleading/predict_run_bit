#include "stdafx.h"
#include <dos.h>
#include <direct.h>
#include "Splash.h"
#include "Credits.h"
#include "TdxW.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ComDoc.h"
#include "NewsView.h"
#include "BigMenuView.h"
#include "HomePageView.h"
#include "GridView.h"
#include "TxtView.h"
#include "ComView.h"
#include "ComView2.h"
#include "IEBrowserView.h"
#include "JbfxView.h"
#include "AdvHqView.h"
#include "JJView.h"
#include "TvView.h"
#include "RssView.h"
#include "LogoDlg.h"
#include "verify.h"
#include "ad.h"
#include "KCLogoDlg.h"

extern long CALLBACK CalcCallBackFunc(char * Code,short nSetCode,short DataType,void * pData/*�ڲ�����*/,long nDataNum,NTime tFrom,NTime tEnd,BYTE nTQ,unsigned long nReserved);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTdxWApp, CWinApp)
	//{{AFX_MSG_MAP(CTdxWApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


CTdxWApp::CTdxWApp()
{
	bExistInstance = FALSE;
	m_hMutex = NULL;
}

CTdxWApp theApp;

void SetWebMenuValue()		//�õ�TopView.ini�����õ���վ��Ϣ
{
	if(g_OEM.bZszqEmbedWTVersion)	//����֤ȯȫ�ܰ治֧��������˵���ҳ
		return;
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	CString tempStr;
	int i;
	//
	TDX_GetPrivateProfileString("Other","MainWebTitle","��Ʒ����",g_strMainWebTitle,98,Ini);	
	g_nWebPageNum = GetPrivateProfileInt("Other","WebPageNum",0,Ini);
	g_nWebPageNum = max(g_nWebPageNum,0);
	g_nWebPageNum = min(g_nWebPageNum,40);
	for(i = 0;i < g_nWebPageNum;i++)
	{
		tempStr.Format("WebPageStr%02d",i+1);
		TDX_GetPrivateProfileString("Other",tempStr,"",g_strWebPageStr[i],48,Ini);
		tempStr.Format("WebPageURL%02d",i+1);
		GetPrivateProfileString("Other",tempStr,"",g_strWebPageURL[i],148,Ini);
		if(g_nUseDNSUrl==1)
		{
			CString tmpURL(g_strWebPageURL[i]);
			tmpURL.Replace(g_strDNSUrl[0], g_strDNSUrl[1]);
			strncpy(g_strWebPageURL[i], (char*)(LPCSTR)tmpURL, 148);
		}
	}
	if(strlen(g_strMainWebTitle)==0)
		g_nWebPageNum=0;
	if(g_nWebPageNum==0) return;
	//
	TDX_GetPrivateProfileString("Other","MainWebTitle2","ȯ����",g_strMainWebTitle2,98,Ini);	
	g_nWebPageNum2 = GetPrivateProfileInt("Other","WebPageNum2",0,Ini);
	g_nWebPageNum2 = max(g_nWebPageNum2,0);
	g_nWebPageNum2 = min(g_nWebPageNum2,40);
	for(i = 0;i < g_nWebPageNum2;i++)
	{
		tempStr.Format("WebPageStr2%02d",i+1);
		TDX_GetPrivateProfileString("Other",tempStr,"",g_strWebPageStr2[i],48,Ini);
		tempStr.Format("WebPageURL2%02d",i+1);
		GetPrivateProfileString("Other",tempStr,"",g_strWebPageURL2[i],148,Ini);
	}
	if(strlen(g_strMainWebTitle2)==0)
		g_nWebPageNum2=0;
}

void GetScheme_Hold(int nNo)
{
	switch(nNo)
	{
	case 0:			//�̰�
		Scheme_Hold.VipColor.BACKCOLOR = RGB(255,255,255);//��ɫ
		Scheme_Hold.VipColor.TIMECOLOR = RGB(128,128,128);
		Scheme_Hold.VipColor.AXISCOLOR = RGB(128,128,128);
		Scheme_Hold.VipColor.KPCOLOR = RGB(255,0,0);
		Scheme_Hold.VipColor.KNCOLOR = RGB(0,128,0);
		Scheme_Hold.VipColor.LEVELCOLOR = RGB(0,0,0);
		Scheme_Hold.VipColor.DOWNCOLOR = RGB(0,128,0);
		Scheme_Hold.VipColor.DRAWLINETOOLCOLOR = RGB(0,0,255);

		Scheme_Hold.VipColor.ZBCOLOR[0] = RGB(0,0,132);
		Scheme_Hold.VipColor.ZBCOLOR[1] = RGB(255,0,255);
		Scheme_Hold.VipColor.ZBCOLOR[2] = RGB(128,128,128);
		Scheme_Hold.VipColor.ZBCOLOR[3] = RGB(128,0,0);
		Scheme_Hold.VipColor.ZBCOLOR[4] = RGB(0,128,0);
		Scheme_Hold.VipColor.ZBCOLOR[5] = RGB(0,255,255);

		Scheme_Hold.VipColor.VOLCOLOR = RGB(230,80,10);
		Scheme_Hold.VipColor.USCOLOR = RGB(0,0,255);
		Scheme_Hold.VipColor.FLOATCURSORCOLOR = RGB(0,255,255);
		Scheme_Hold.VipColor.BIGTITLECOLOR = RGB(0,0,0);
		Scheme_Hold.VipColor.TXTCOLOR = RGB(0,0,0);
		Scheme_Hold.GridColor.GridBackColor = RGB(255,255,255);
		Scheme_Hold.GridColor.GridLineColor = RGB(210,210,210);
		Scheme_Hold.GridColor.GridHighLineColor = RGB(0,0,250);
		Scheme_Hold.GridColor.Grid2GridColor = RGB(255,255,255);
		Scheme_Hold.GridColor.GridTitleTextColor = RGB(0,0,0);
		Scheme_Hold.GridColor.GridTitleArrowColor = RGB(255,0,0);
		Scheme_Hold.GridColor.Grid_Up_Color = RGB(255,0,0);
		Scheme_Hold.GridColor.Grid_Down_Color = RGB(0,128,0);
		Scheme_Hold.GridColor.Grid_Level_Color = RGB(0,0,0);
		Scheme_Hold.GridColor.Grid_CodeName_Color = RGB(0,0,0);
		Scheme_Hold.GridColor.Grid_Volume_Color = RGB(220,120,60);
		Scheme_Hold.GridColor.Grid_Other_Color = RGB(0,128,128);
		Scheme_Hold.TxtColor.TxtForeColor = RGB(0,0,0);
		Scheme_Hold.TxtColor.TxtBackColor = RGB(255,255,255);
		Scheme_Hold.TreeColor.TreeForeColor = RGB(0,0,0);
		Scheme_Hold.TreeColor.TreeBackColor = RGB(255,255,255);
		break;
	case 1:			//���1
		Scheme_Hold.VipColor.BACKCOLOR = RGB(0,0,0);
		Scheme_Hold.VipColor.TIMECOLOR = RGB(192,0,0);
		Scheme_Hold.VipColor.AXISCOLOR = RGB(160,0,0);
		Scheme_Hold.VipColor.KPCOLOR = RGB(255,84,84);
		Scheme_Hold.VipColor.KNCOLOR = RGB(84,255,255);
		Scheme_Hold.VipColor.LEVELCOLOR = RGB(192,192,192);
	//	Scheme_Hold.VipColor.DOWNCOLOR = RGB(84,255,255);
		Scheme_Hold.VipColor.DOWNCOLOR = RGB(0,192,0);
		Scheme_Hold.VipColor.DRAWLINETOOLCOLOR = RGB(255,255,0);

		Scheme_Hold.VipColor.ZBCOLOR[0] = RGB(250,250,250);
		Scheme_Hold.VipColor.ZBCOLOR[1] = RGB(255,255,0);
		Scheme_Hold.VipColor.ZBCOLOR[2] = RGB(255,0,255);
		Scheme_Hold.VipColor.ZBCOLOR[3] = RGB(0,255,0);
		Scheme_Hold.VipColor.ZBCOLOR[4] = RGB(192,192,192);
		Scheme_Hold.VipColor.ZBCOLOR[5] = RGB(0,0,255);

		Scheme_Hold.VipColor.VOLCOLOR = RGB(192,192,0);
		Scheme_Hold.VipColor.USCOLOR = RGB(0,255,255);
		Scheme_Hold.VipColor.FLOATCURSORCOLOR = RGB(10,36,106);
		Scheme_Hold.VipColor.BIGTITLECOLOR = RGB(192,192,192);
		Scheme_Hold.VipColor.TXTCOLOR = RGB(192,192,192);
		Scheme_Hold.GridColor.GridBackColor = RGB(0,0,0);
		Scheme_Hold.GridColor.GridLineColor = RGB(128,0,0);
		Scheme_Hold.GridColor.GridHighLineColor = RGB(192,192,0);
		Scheme_Hold.GridColor.Grid2GridColor = RGB(24,24,24);
		Scheme_Hold.GridColor.GridTitleTextColor = RGB(192,192,192);
		Scheme_Hold.GridColor.GridTitleArrowColor = RGB(255,0,0);

		Scheme_Hold.GridColor.Grid_Up_Color = RGB(255,84,84);
		Scheme_Hold.GridColor.Grid_Down_Color = Scheme_Hold.VipColor.DOWNCOLOR;
		Scheme_Hold.GridColor.Grid_Level_Color = RGB(192,192,192);
		Scheme_Hold.GridColor.Grid_CodeName_Color = RGB(192,192,192);
		Scheme_Hold.GridColor.Grid_Volume_Color = RGB(192,192,192);
		Scheme_Hold.GridColor.Grid_Other_Color = RGB(0,255,255);
		Scheme_Hold.TxtColor.TxtForeColor = RGB(192,192,0);
		Scheme_Hold.TxtColor.TxtBackColor = RGB(0,0,0);
		Scheme_Hold.TreeColor.TreeForeColor = RGB(192,192,192);
		Scheme_Hold.TreeColor.TreeBackColor = RGB(0,0,0);
		break;
	case 2:			//���2
		Scheme_Hold.VipColor.BACKCOLOR = RGB(8,0,0);
		Scheme_Hold.VipColor.TIMECOLOR = RGB(0,192,0);
		Scheme_Hold.VipColor.AXISCOLOR = RGB(160,0,0);
		Scheme_Hold.VipColor.KPCOLOR = RGB(255,0,0);
		Scheme_Hold.VipColor.KNCOLOR = RGB(0,255,0);
		Scheme_Hold.VipColor.LEVELCOLOR = RGB(192,192,192);
		Scheme_Hold.VipColor.DOWNCOLOR = RGB(0,255,0);
		Scheme_Hold.VipColor.DRAWLINETOOLCOLOR = RGB(255,255,0);
		Scheme_Hold.VipColor.COMVIEWINFOBC = RGB(96,64,64);

		Scheme_Hold.VipColor.ZBCOLOR[0] = RGB(250,250,250);
		Scheme_Hold.VipColor.ZBCOLOR[1] = RGB(255,255,0);
		Scheme_Hold.VipColor.ZBCOLOR[2] = RGB(255,0,255);
		Scheme_Hold.VipColor.ZBCOLOR[3] = RGB(0,255,0);
		Scheme_Hold.VipColor.ZBCOLOR[4] = RGB(192,192,192);
		Scheme_Hold.VipColor.ZBCOLOR[5] = RGB(0,0,255);

		Scheme_Hold.VipColor.VOLCOLOR = RGB(192,192,0);
		Scheme_Hold.VipColor.USCOLOR = RGB(0,255,255);
		Scheme_Hold.VipColor.FLOATCURSORCOLOR = RGB(10,36,106);
		Scheme_Hold.VipColor.BIGTITLECOLOR = RGB(192,192,192);
		Scheme_Hold.VipColor.TXTCOLOR = RGB(216,216,243);
		Scheme_Hold.GridColor.GridBackColor = RGB(16,0,0);
		Scheme_Hold.GridColor.GridLineColor = RGB(225,0,0);
		Scheme_Hold.GridColor.GridHighLineColor = RGB(192,192,0);
		Scheme_Hold.GridColor.Grid2GridColor = RGB(32,32,32);
		Scheme_Hold.GridColor.GridTitleTextColor = RGB(192,192,192);
		Scheme_Hold.GridColor.GridTitleArrowColor = RGB(255,0,0);

		Scheme_Hold.GridColor.Grid_Up_Color = RGB(255,0,0);
		Scheme_Hold.GridColor.Grid_Down_Color = RGB(0,255,0);
		Scheme_Hold.GridColor.Grid_Level_Color = RGB(192,192,192);
		Scheme_Hold.GridColor.Grid_CodeName_Color = RGB(255,222,0);
		Scheme_Hold.GridColor.Grid_Volume_Color = RGB(192,192,192);
		Scheme_Hold.GridColor.Grid_Other_Color = RGB(0,255,255);
		Scheme_Hold.TxtColor.TxtForeColor = RGB(192,192,0);
		Scheme_Hold.TxtColor.TxtBackColor = RGB(0,0,0);
		Scheme_Hold.TreeColor.TreeForeColor = RGB(192,192,192);
		Scheme_Hold.TreeColor.TreeBackColor = RGB(0,0,0);
		break;
	}
}

/*
void GetScheme()
{
	CString SchemeIni = g_WSXHStr+"scheme.dat";
	GetScheme_Hold(0);
	SchemeNum = GetPrivateProfileInt("Color","SchemeNum",0,SchemeIni);
	if(SchemeNum < 3)
	{
		Scheme[0] = Scheme[1] = Scheme_Hold;
		GetScheme_Hold(1);
		Scheme[2] = Scheme_Hold;
		GetScheme_Hold(2);
		Scheme[3] = Scheme_Hold;
		GetScheme_Hold(3);
		Scheme[4] = Scheme_Hold;
		GetScheme_Hold(4);
		Scheme[5] = Scheme_Hold;
		GetScheme_Hold(5);
		Scheme[6] = Scheme_Hold;
		SchemeStr[0] = "�Զ���";
		SchemeStr[1] = "�̰�";
		SchemeStr[2] = "���";
		SchemeStr[3] = "���2";
		SchemeStr[4] = "���";
		SchemeStr[5] = "����";
		SchemeStr[6] = "��ͳ";
		SchemeNum = 7;
		CurrentScheme = 2;
		return;
	}
	if(SchemeNum > 20) SchemeNum = 20; //ֻ����20������(�����Զ��巽��)
	//��ǰ����
	CurrentScheme = GetPrivateProfileInt("Color","CurrentScheme",2,SchemeIni);
	if(CurrentScheme < 0 || CurrentScheme > SchemeNum-1) 
		CurrentScheme = 2;
		
	char tempSchemeTopic[50],tempSchemeStr[21];//������ֻ��Ϊ20�ֽڳ�
	for(int i = 0;i < SchemeNum;i++)
	{
		sprintf(tempSchemeTopic,"SchemeName%d",i);
		GetPrivateProfileString("Color",tempSchemeTopic,"��������",tempSchemeStr,20,SchemeIni);
		SchemeStr[i] = tempSchemeStr;
		sprintf(tempSchemeTopic,"Scheme%d",i);
		if( !GetPrivateProfileStruct("Color",tempSchemeTopic,&Scheme[i],sizeof(SYSTEMCOLOR),SchemeIni) )
			Scheme[i] = Scheme_Hold;
	}
}
*/

short nPos_123[32] = {613,210,146,20, 613,241,146,20, 627,269,12,12, 702,269,12,12, 461,397,318,10, 516,343,88,26, 653,343,88,26, 589,293,78,26};
short nPos_4[28] = {379,163,160,23, 379,199,160,23, 425,242,10,9, 326,242,10,9, 296,324,242,8, 310,278,66,24, 418,278,66,24};

void GetScheme_King()
{
	CString TableName, VerString;
	TableName.LoadString(IDS_TABLENAME);
	VerString.LoadString(IDS_VERSTRING);

//	strncpy(g_Scheme_King.QSID, (char*)(LPCSTR)TableName, 10);
//	g_strPreName.LoadString(IDR_MAINFRAME);
//	CString strAppName;
//	strAppName.LoadString(IDS_MAINTITLE);//��Ʒ����
//	g_strTitleName.Format("%s--�����г�ʵս�͹�Ʊ���ݷ��������", strAppName);//��Ʒ����
//	g_strAdName.Format("%s��������ߵ�֤ȯͶ�ʷ���ר�ң�", strAppName);
//	g_strMoving.Format("��%s����%s����з����ĳ�Ʒ������������֤ȯ�������֮��������һ��Windows XPƽ̨�Ͻ��и���֤ȯ������ϵͳ������ǿ�󡢲������㡢�����Ѻã�֧�ֻ���������ʵʱ���飬�ṩ���ص�ָ�����������������Ͷ��Ʒ�֣��ʺϸ���֤ȯͶ����ʹ�á�", strAppName, g_strPreName);

	strncpy(g_Scheme_King.VersionString, (char*)(LPCSTR)VerString, 20);
	g_Scheme_King.nVersion = 3;
	g_bHasHomePage = FALSE;
	g_bHasDS = FALSE;
	g_bShow3DKLine = FALSE;
	g_bZBEditable = FALSE;

	g_Scheme_King.LoginDB_ID = IDB_BITMAP_LOGO_III;
	g_Scheme_King.MainMenuID = IDR_SIM_MENU_III;
	g_Scheme_King.ToolBarID = IDR_NEWBARN_III;
	g_Scheme_King.ToolBarID_Hot = IDR_NEWBARN_III;
	g_Scheme_King.ToolBarID_Disable = IDR_NEWBARD_III;
	memcpy(g_uBigToolBarBtn, g_uBigToolBarBtn_III, g_nBigToolBarBtn_III*sizeof(UINT));
	g_nBigToolBarBtn = g_nBigToolBarBtn_III;
	memcpy(&g_Scheme_King.nLup, nPos_123, sizeof(nPos_123));
//	strncpy(g_DefCusPadName, "ȫ�����", 63);

	g_Scheme_King.Scheme_Hold_No = 1;
	g_Scheme_King.bShowToolBar = TRUE;
	g_InitDefHomePage = g_Scheme_King.HomePage = "http://html5test.com";
//	strncpy(g_Scheme_King.PopFileUrl, "http://www.kaifa888.com/pop.txt", MAX_PATH-1);
	g_Scheme_King.nVertiOff = 24;
	g_Scheme_King.nParaOff = -1;
	g_Scheme_King.StartDownFile = "";
	g_Scheme_King.YxhjPath = "100nian";
	g_Scheme_King.bStatusSwitch = FALSE;
}

void GetScheme()
{
	GetScheme_Hold(0);
	Scheme[0] = Scheme_Hold;
	GetScheme_Hold(g_Scheme_King.Scheme_Hold_No);
	Scheme[1] = Scheme_Hold;
	SchemeStr[0] = "�����̰�";
	SchemeStr[1] = "��ͳ���";
	SchemeNum = 2;
	CurrentScheme = 1;
}

void SaveScheme()
{
	CString SchemeIni = g_WSXHStr + "scheme.dat";

	WritePrivateProfileString("Color","SchemeNum",IntToStr(SchemeNum),SchemeIni);
	WritePrivateProfileString("Color","CurrentScheme",IntToStr(CurrentScheme),SchemeIni);
	char tempSchemeTopic[50];
	for(int i = 0;i < SchemeNum;i++)
	{
		sprintf(tempSchemeTopic,"SchemeName%d",i);
		WritePrivateProfileString("Color",tempSchemeTopic,SchemeStr[i],SchemeIni);
		sprintf(tempSchemeTopic,"Scheme%d",i);
		WritePrivateProfileStruct("Color",tempSchemeTopic,&Scheme[i],sizeof(SYSTEMCOLOR),SchemeIni);
	}
}

vector<_FY_GROUP_INFO> g_vTmp;
void GetFYInfo()
{
	g_vTmp.clear();
	g_vfyGroup.clear();
	if(!g_GC.bHasTend) return;
	//���OnlyJYTend,��ǽ����û����ܿ������Ƶ���(���ǵ�ǰ��L2)
	if(g_OEM.OnlyJYTend && !g_bUserHostEverConnect && strlen(g_JyInfo.zjzh)==0)
		return;
		
	CString	Ini = g_WSXHStr+g_strTendCfg;
	char tempStr[256]={0};
	char strTmp[128]={0};
	CString strItem1,strItem2;
	
	int nGroup = GetPrivateProfileInt("GROUP","Num",0,Ini);
	if(nGroup>0)
	{
		for(int i=0;i<nGroup;i++)
		{
			sprintf(strTmp, "Name%02d", i+1);
			GetPrivateProfileString("GROUP",strTmp,"δ֪",tempStr,256,Ini);
			_FY_GROUP_INFO sf;
			sf.strGroupName=tempStr;
			sprintf(strTmp,"Group%02d",i+1);
			GetPrivateProfileString(strTmp,"ParentName","",tempStr,256,Ini);
			sf.parentName=tempStr;
			g_vTmp.push_back(sf);
		}
	}
	nGroup=g_vTmp.size();
	for(int i=0;i<nGroup;i++)
	{
		FY_GROUP_INFO gygi;
		gygi.strGroupName=g_vTmp[i].strGroupName;
		gygi.nIndex_file=i+1;
		if(g_vTmp[i].parentName=="")
		{
			g_vfyGroup.push_back(gygi);
			g_vTmp[i].nIndex=g_vfyGroup.size()-1;
		}
		else
		{
			BOOL bFound=FALSE;
			for(int j=0;j<i;j++)
			{
				if(g_vTmp[j].parentName==g_vTmp[i].parentName)
				{
					bFound=TRUE;
					break;
				}
			}
			if(!bFound)
			{
				FY_GROUP_INFO gygi2;
				gygi2.strGroupName=g_vTmp[i].parentName;
				g_vfyGroup.push_back(gygi2);
				UINT topGroup=g_vfyGroup.size()-1;
				g_vTmp[i].nIndex=topGroup;
				g_vfyGroup[topGroup].subGroup.push_back(topGroup+1);
				g_vTmp[i].nSubIndex=0;
				g_vfyGroup.push_back(gygi);
				g_vfyGroup[topGroup+1].belong=topGroup;
			}
			else
			{
				g_vTmp[i].nIndex=g_vTmp[j].nIndex;
				g_vfyGroup.push_back(gygi);
				UINT subGroup=g_vfyGroup.size()-1;
				g_vfyGroup[g_vTmp[i].nIndex].subGroup.push_back(subGroup);
				g_vfyGroup[subGroup].belong=g_vTmp[i].nIndex;
				g_vTmp[i].nSubIndex=g_vfyGroup[g_vTmp[i].nIndex].subGroup.size()-1;
			}
		}
	}
	for(i=0;i<nGroup;i++)
	{
		BOOL bTop=g_vTmp[i].parentName=="";
		int nIndex;
		char strKey[128]={0};
		if(bTop)
			nIndex=g_vTmp[i].nIndex;
		else
			nIndex=g_vfyGroup[g_vTmp[i].nIndex].subGroup[g_vTmp[i].nSubIndex];

		sprintf(strKey, "GROUP%02d", i+1);
		int bEncrypted=GetPrivateProfileInt(strKey,"BENCRYPTED",0,Ini);
		g_vfyGroup[nIndex].bEncrypted=(BOOL)bEncrypted;
		g_vfyGroup[nIndex].groupAttribute=GetPrivateProfileInt(strKey,"GroupAttribute",0,Ini);
		g_vfyGroup[nIndex].bActive=FALSE;
		g_vfyGroup[nIndex].nType=GetPrivateProfileInt(strKey,"Type",0,Ini);
		int nItem = GetPrivateProfileInt(strKey,"Num",0,Ini);
		for(int j=0;j<nItem;j++)
		{
			sprintf(strTmp, "Name%02d", j+1);
			GetPrivateProfileString(strKey,strTmp,"δ֪",tempStr,256,Ini);
			strItem1=tempStr;
			g_vfyGroup[nIndex].strItemName.push_back(strItem1);
			sprintf(strTmp, "Title%02d", j+1);
			GetPrivateProfileString(strKey,strTmp,"",tempStr,256,Ini);
			strItem2=tempStr;
			if(strItem2.GetLength()<1)
				strItem2=strItem1;
			g_vfyGroup[nIndex].strShowTitle.push_back(strItem2);
			sprintf(strTmp,"ItemAttribute%02d",j+1);
			int itemAttribute=GetPrivateProfileInt(strKey,strTmp,0,Ini);
			g_vfyGroup[nIndex].itemAttribute.push_back(itemAttribute);
		}
		if(!bTop)
			g_vfyGroup[g_vTmp[i].nIndex].groupAttribute=g_vfyGroup[g_vfyGroup[g_vTmp[i].nIndex].subGroup[0]].groupAttribute;
	}
}

//�õ�ȯ����ص�����
void GetNewJyCfg(char *qsid)
{
	CString	Ini;
	Ini.Format("%s%s.cfg",g_WSXHStr,qsid);
	//���������Ͻǵĵ�ַ
	GetPrivateProfileString("CaptionWeb","URL1","",g_NewJyCfg.CaptionWeb1,198,Ini);
	if(strlen(g_NewJyCfg.CaptionWeb1)>0)
		strcpy(g_OEM.strWebButtonURL,g_NewJyCfg.CaptionWeb1);
	//�����Ѷ
	GetPrivateProfileString("RelativeInfo","URL","",g_NewJyCfg.RelativeInfo,198,Ini);	
	GetPrivateProfileString("QSRelativeInfo","URL","",g_NewJyCfg.QSRelativeInfo,198,Ini);
	GetPrivateProfileString("QSRelativeInfo","Name","",g_NewJyCfg.QSRelativeName,19,Ini);
	g_NewJyCfg.bHasQSRelativeInfo=(strlen(g_NewJyCfg.QSRelativeInfo)>0 && strlen(g_NewJyCfg.QSRelativeName)>0);
	//�����ί�г���
	GetPrivateProfileString("AddedWT","AddedWT_Name","��Ƕ����",g_NewJyCfg.AddedWT_Name,19,Ini);
	GetPrivateProfileString("AddedWT","AddedWT1_Name","",g_NewJyCfg.AddedWT1_Name,19,Ini);
	GetPrivateProfileString("AddedWT","AddedWT1_Path","",g_NewJyCfg.AddedWT1_Path,198,Ini);
	GetPrivateProfileString("AddedWT","AddedWT2_Name","",g_NewJyCfg.AddedWT2_Name,19,Ini);
	GetPrivateProfileString("AddedWT","AddedWT2_Path","",g_NewJyCfg.AddedWT2_Path,198,Ini);
	//�Ƿ�ǿ���û���ʾ������Ѷ
	g_NewJyCfg.bAlwaysStatusRoll=GetPrivateProfileInt("RollInfo","AlwaysStatusRoll",0,Ini);
	if(g_NewJyCfg.bAlwaysStatusRoll)
	{
		g_GH.Roll.bRollOpen[0]=TRUE;
		if(g_GC.bSupportSlowStatusRoll)
			g_GH.Roll.bRollOpen[1]=TRUE;
		g_GH.Roll.bShow=TRUE;
	}
	//��Ѷ��ҳ(��F7���ֵ���ҳ)
	GetPrivateProfileString("HomePage","ZXHomePage","",g_NewJyCfg.ZXHomePage,198,Ini);
}

void GetFromUserINI()
{
	CString	Ini = g_WSXHStr+g_strUserCfg;
	if(!TestFileExist(Ini))	//���û��user.ini,����user_def.iniϵͳȱʡ����ļ�
		::CopyFile(g_WSXHStr+"user_def.ini",Ini,FALSE);

	char tempStr[256];
	//������
	g_bHasCaption = GetPrivateProfileInt("Other","SimpleMode",0,Ini);
	g_bHasCaption = 1;
	g_nNetCondition = GetPrivateProfileInt("Other","NetCondition",1,Ini);
	if(g_nNetCondition<0 || g_nNetCondition>3)
		g_nNetCondition = 1;
	g_nNetCondition = 3;
	RECENTNUM = (g_nNetCondition+1)*140; //��������������������RECENTNUM
	//�Ƿ�ʼ��ÿ��һ��,�����ļ��е�λ��
	bStartTips = GetPrivateProfileInt("Other","StartTips",0,Ini);
	nTipFilePos = GetPrivateProfileInt("Other","TipFilePos",0,Ini); 

	//�Զ���ҳʱ��
	AutoPageTime = GetPrivateProfileInt("Other","AutoPageTime",8,Ini);
	if(AutoPageTime < 4 || AutoPageTime > 600) AutoPageTime = 8;
	//�������ʱ��
	HQRefreshTime = GetPrivateProfileInt("Other","HQRefreshTime",4,Ini);
	if(HQRefreshTime < 4 || HQRefreshTime > 60) HQRefreshTime = 4;
	//״̬������ʱ��
	StatusRefreshTime = GetPrivateProfileInt("Other","StatusRefreshTime",5,Ini);
	if(StatusRefreshTime < 5 || StatusRefreshTime > 60) StatusRefreshTime = 5;
	g_nZFFzNum = GetPrivateProfileInt("Other","ZFPeriod",5,Ini);
	if(g_nZFFzNum < 1 || g_nZFFzNum > 60) g_nZFFzNum = 5;
	//�������Ƿ���ʾ
	bTreeShow = GetPrivateProfileInt("Other","bTreeShow",0,Ini);
	bTreeShow = FALSE;
	//�������Ƿ���ʾ
	bRestShow = GetPrivateProfileInt("Other","bRestShow",0,Ini); 
	bRestShow = FALSE;
	//�������뽻�����ϴ�ѡ�е�Tab
	nLastTreeSel = GetPrivateProfileInt("Other","LastTreeSel",0,Ini); 
	nLastRestSel = GetPrivateProfileInt("Other","LastRestSel",0,Ini); 
	//״̬���Ƿ���ʾ
	bStatusBarShow = GetPrivateProfileInt("Other","bStatusBarShow",1,Ini); 
	//�����Ʊ����(������ѡ�ɺ͹�Ʊ��)
	FirstGPType = GetPrivateProfileInt("Other","FirstGPType",TYPE_AG,Ini);
	if(FirstGPType < -1 || FirstGPType > TOTAL_TYPENUM) FirstGPType = TYPE_AG; //���Ե���-1
	FirstGPType = TYPE_AG;
	//����־(���ɣ���ҵ���࣬�ط�����)
	FirstBlockFlag = GetPrivateProfileInt("Other","FirstBlockFlag",0,Ini);

	//�����Ʊ����(������ѡ�ɺ͹�Ʊ��)
	FirstHisGPType = GetPrivateProfileInt("Other","FirstHisGPType",TYPE_AG,Ini);
	if(FirstHisGPType < -1 || FirstHisGPType > TOTAL_TYPENUM) FirstHisGPType = TYPE_AG; //���Ե���-1
	//����־(���ɣ���ҵ���࣬�ط�����)
	FirstHisBlockFlag = GetPrivateProfileInt("Other","FirstHisBlockFlag",0,Ini);

	GetPrivateProfileString ( "RATE", "DOLLAR_RATE", "6.800",tempStr ,40,Ini);
	g_my_rate = atof ( tempStr );
	GetPrivateProfileString ( "RATE", "HONKONG_RATE", "0.870",tempStr ,40,Ini);
	g_gy_rate = atof ( tempStr );
	g_CurrentJBM = GetPrivateProfileInt("Other","CurrentJBM",0,Ini);
	//ǿ��������������
	QRParam[0] = GetPrivateProfileInt("Other","QRParam1",3,Ini);
	QRParam[1] = GetPrivateProfileInt("Other","QRParam2",5,Ini);
	QRParam[2] = GetPrivateProfileInt("Other","QRParam3",10,Ini);
	QRParam[3] = GetPrivateProfileInt("Other","QRParam4",20,Ini);
	QRParam[4] = GetPrivateProfileInt("Other","QRParam5",60,Ini);
	for(int i = 0;i < 5;i++)
	{
		if(QRParam[i] > 299) QRParam[i] = 299;
		if(QRParam[i] < 1) QRParam[i] = 1;
	}
	//
	g_bBigFontGrid = GetPrivateProfileInt("Other","BigFontGrid",1,Ini);
	//InfoDlg����������Ϣ
	g_InfoDlgPosx = GetPrivateProfileInt("Other","HQX",600,Ini);
	g_InfoDlgPosy = GetPrivateProfileInt("Other","HQY",500,Ini);
	g_bInfoMinimize = GetPrivateProfileInt("Other","HQMINIMIZE",0,Ini);
	//����������Ϣ
	g_nMainZbFlag = GetPrivateProfileInt("Other","MainZb",0,Ini);
	if(g_nMainZbFlag<0 || g_nMainZbFlag>4) g_nMainZbFlag=0;
	g_GH.bShowKGap = GetPrivateProfileInt("Other","ShowKGap",0,Ini);
	g_GH.bShowMmpChg = GetPrivateProfileInt("Other","ShowMmpChg",1,Ini);
	g_GH.bRealUPK = GetPrivateProfileInt("Other","RealUPK",0,Ini);
	g_GH.bRealUPK = FALSE;
	g_GH.bLocalData = GetPrivateProfileInt("Other","LocalData",0,Ini);
	g_GH.nInitKNum = GetPrivateProfileInt("Other","InitKNum",-1,Ini);
	g_GH.nInitKNum2 = GetPrivateProfileInt("Other","InitKNum2",-1,Ini);
	g_GH.bOnlyDownAGData = GetPrivateProfileInt("Other","OnlyDownAGData",0,Ini);
	g_GH.bFullScreenNotify = GetPrivateProfileInt("Other","FullScreenNotify",1,Ini);
	g_GH.bAddBlockPrompt = GetPrivateProfileInt("Other","AddBlockPrompt",0,Ini);
	g_GH.bLockHQSort = GetPrivateProfileInt("Other","LockHQSort",0,Ini);
	g_GH.bShowBS = GetPrivateProfileInt("Other","ShowBS",1,Ini);
	g_GH.bShowBS = 0;
	g_GH.bTick2Priority = GetPrivateProfileInt("Other","Tick2Priority",0,Ini);
	g_GH.bZSTLevel2Face = 0;
	g_GH.bFXTLevel2Face = 0;
	g_GH.StatBigVol=GetPrivateProfileInt("Other","BigVol",500,Ini);
	g_GH.bPinAssistPad = GetPrivateProfileInt("Other","PinAssistPad",1,Ini);
	g_GH.bCodeCanSort = GetPrivateProfileInt("Other","CodeCanSort",1,Ini);
	g_GH.bShowTPPriceInZST = GetPrivateProfileInt("Other","ShowTPPriceInZST",1,Ini);
	g_GH.lCheckUrgentDate = GetPrivateProfileInt("Other","CheckUrgentDate",0,Ini);
	g_GH.bCheckUrgent = GetPrivateProfileInt("Other","CheckUrgent",0,Ini);

	g_nTxtFontSize = GetPrivateProfileInt("Other","TxtFontSize",8,Ini);
	if(g_nTxtFontSize>50) g_nTxtFontSize=50;
	if(g_nTxtFontSize<1) g_nTxtFontSize=1;
	g_nTxtColor = COLORREF(GetPrivateProfileInt("Other","TxtColor",0xffffff,Ini));
	//
	g_bHasXHidden = GetPrivateProfileInt("Other","XHidden",0,Ini);
	g_bHasYHidden = GetPrivateProfileInt("Other","YHidden",1,Ini);

	GetPrivateProfileString("Other","FirstChartZb","MA",g_strZbMain,ZBCODE_LEN-1,Ini);
	GetPrivateProfileString("Other","LastExp","",g_strLastExp,ZBCODE_LEN-1,Ini);	
	GetPrivateProfileString("Other","LastKLine","",g_strLastKLine,ZBCODE_LEN-1,Ini);
	GetPrivateProfileString("Other","LastZBSort","",g_strLastZBSort,ZBCODE_LEN-1,Ini);
	for(i=0;i<11;i++)
	{
		sprintf(tempStr,"FavUseZb%02d",i);
		GetPrivateProfileString("Other",tempStr,g_strFavUseZb[i],g_strFavUseZb[i],ZBCODE_LEN-1,Ini);
	}
	g_nFixedWidth = GetPrivateProfileInt("Other","FixedWidth",198,Ini);
	if(g_nFixedWidth < 198 || g_nFixedWidth > 300) g_nFixedWidth=198;
	g_nFixedWidth2 = GetPrivateProfileInt("Other","FixedWidth2",214,Ini);
	if(g_nFixedWidth2 < 214 || g_nFixedWidth2 > 400) g_nFixedWidth2=214;

	g_nNewMulNum = GetPrivateProfileInt("Other","MulNum",9,Ini);
	if(g_nNewMulNum!=9 && g_nNewMulNum!=16 && g_nNewMulNum!=25 && g_nNewMulNum!=6 && g_nNewMulNum!=12 && g_nNewMulNum!=20)
		g_nNewMulNum = 4;
	g_nMulNum = g_nNewMulNum;
	g_bZBudFlag = GetPrivateProfileInt("Other","HasZBFlag",1,Ini);

	g_nMinnNum = GetPrivateProfileInt("Other","MinNNum",10,Ini);
	g_nDaynNum = GetPrivateProfileInt("Other","DayNNum",45,Ini);

	DWORD dwVersion = GetVersion();//Windows NT/2000/XP
	g_b2000XPSys=(dwVersion < 0x80000000);
	
	g_nHligtNowVol = GetPrivateProfileInt("Other","HLIGHTNOWVOL",500,Ini);

	GetPrivateProfileString("Other","ProfitFee","6.5",tempStr,20,Ini);
	g_fProfitFee = atof(tempStr);

	g_bVolTQ = GetPrivateProfileInt("Other","VOLTQ",0,Ini);

	g_nGzPerMin =  GetPrivateProfileInt("Other","GZPERMIN",120,Ini);
	g_nZBWinNum = GetPrivateProfileInt("Other","ZBWINNUM",3,Ini);
	g_bAllWarnSwitch = GetPrivateProfileInt("Other","WarnSwitch",0,Ini);
	g_bTjWarnSwitch = GetPrivateProfileInt("Other","TjWarnSwitch",0,Ini);
	//�˳�ϵͳ���Ƿ���ʾ��������
	g_bNotifyDownload = GetPrivateProfileInt("Other","NotifyDownLoad",0,Ini);
	//�����Զ�д��
	g_bDay_WriteDisk = GetPrivateProfileInt("Other","Day_WriteDisk",0,Ini);
	g_bDay_WriteDisk = 0;
	//��������Ƿ񾫳���Ȩ
	g_bHisViewTQ = GetPrivateProfileInt("Other","HisViewTQ",1,Ini);
	//���ܼ���������һ��
	g_nFindRadio = GetPrivateProfileInt("Other","FindRadio",0,Ini);
	if(g_nFindRadio != 0 && g_nFindRadio != 1)
		g_nFindRadio = 0;
	//��������λ��
	g_nToolBarPos = GetPrivateProfileInt("Other","ToolBarPos",0,Ini);
	if(g_nToolBarPos < 0 || g_nToolBarPos > 3)	g_nToolBarPos = 0 ;
	g_bAutoHideBar = GetPrivateProfileInt("Other","AutoHideBar",1,Ini);
	g_bAutoHideBar = FALSE;
	//����Ǵ󹤾�����ʽ,������ϲ��Զ�����
	if(g_bShowBigToolBar)
	{
		g_nToolBarPos=2;
	//	g_bAutoHideBar=TRUE;
	}
	//�Ƿ����������ʾ���ױ��
	g_bShowHqMine = GetPrivateProfileInt("Other","ShowHqMine",0,Ini);
	//�Ƿ���ɫ����
	g_bDrawBlue = GetPrivateProfileInt("Other","DrawBlue",1,Ini);
	//�Ƿ��ǳ��ĸ�����
	g_bLongHighLine = GetPrivateProfileInt("Other","LongHighLine",0,Ini);
	//�ǲ��ǲ��������Ѷ�͸������
	g_bInsideOpenPage = GetPrivateProfileInt("Other","InsideOpenPage",0,Ini);
	g_bExtern_Info = GetPrivateProfileInt("Other","EXTERNINFO",0,Ini);
	g_bExtern_Invest = GetPrivateProfileInt("Other","ExternInvest",0,Ini);
	//��������
	g_virtkey=(WORD)GetPrivateProfileInt("HOTKEY","VIRTKEY",88,Ini);
	g_comkey=(WORD)GetPrivateProfileInt("HOTKEY","COMBOKEY",4,Ini);
	g_hideinfo=(BOOL)GetPrivateProfileInt("HOTKEY","INFO",0,Ini);
	g_bBossKey=(BOOL)GetPrivateProfileInt("HOTKEY","BOSSKEY",0,Ini);
	//�˵����
	g_nMenuMode = GetPrivateProfileInt("Other","MenuMode",1,Ini);
	if(g_nMenuMode!=0 && g_nMenuMode!=1) g_nMenuMode = 0;
	g_bExtendXPStyle = GetPrivateProfileInt("Other","ExtendXPStyle",1,Ini);
	g_bClassXP = GetPrivateProfileInt("Other","ClassXP",1,Ini);
	if(!g_bEnableClassXP) g_bClassXP=FALSE;

	g_bMin3Tray = GetPrivateProfileInt("Other","Min2Tray",0,Ini);
	//����ͼ
	GetPrivateProfileString("Other","JiangEnStep","0.1",tempStr,10,Ini);
	g_fJiangEnStep = atof(tempStr);
	g_nJiangEnMode = GetPrivateProfileInt("Other","JiangEnMode",0,Ini);
	if(g_nJiangEnMode != 0 && g_nJiangEnMode != 1)
		g_nJiangEnMode = 0;
	g_nCBSwitch = GetPrivateProfileInt("Other","CBSwitch",1,Ini);
	if(g_nCBSwitch < 0 || g_nCBSwitch > 2) g_nCBSwitch = 1;
	//�Ƿ�ʹ���Զ�����
	g_bUseAutoConnect = GetPrivateProfileInt("Other","UseAutoConnect",1,Ini);
	//�Ƿ�����
	g_bActTogether = GetPrivateProfileInt("Other","ActTogether",1,Ini);
	//��ʮ����ʱ�Ƿ���ʾ����С����
	g_bFxtShowInfoDlg = GetPrivateProfileInt("Other","FxtShowInfoDlg",1,Ini);
	g_bFxtShowInfoDlg = FALSE;
	//�Ƿ���ʾ����ͼ�е�ָ���л�TAB
#if defined(PERSONAL) || defined(LEVEL2)
	g_bFxtShowTab = GetPrivateProfileInt("Other","FxtShowTab",1,Ini);
#else
	g_bFxtShowTab = GetPrivateProfileInt("Other","FxtShowTab",1,Ini);
#endif
	g_bFxtShowTab=TRUE;
	//�Ƿ�ͨ���л�TAB
	g_bTranparentTab = GetPrivateProfileInt("Other","TranparentTab",1,Ini);
	g_bTranparentTab = FALSE;
	//�ϴεĵ�¼��ʽ
	g_nLastLoginType = GetPrivateProfileInt("Other","LastLoginType",REGISTER_LOGIN,Ini);
	//һЩ�û�ϰ��
	g_GH.AWeekNoHint=GetPrivateProfileInt("Other","AWeekNoHint",0,Ini);
	g_GH.NoHintStartDate=GetPrivateProfileInt("Other","NoHintStartDate",0,Ini);
	g_GH.NoXHCol=GetPrivateProfileInt("Other","NoXHCol",0,Ini);
	g_GH.NoVScroll=GetPrivateProfileInt("Other","NoVScroll",1,Ini);
	g_GH.nShowJUNJ = GetPrivateProfileInt("Other","ShowJUNJ",0,Ini);
	g_GH.nCurBlockFlag = GetPrivateProfileInt("Other","CurBlockFlag",0,Ini);
	g_GH.nPerScale = GetPrivateProfileInt("Other","PerScale",10,Ini);
	g_GH.nPerSect = GetPrivateProfileInt("Other","PerSect",4,Ini);
	//�Ƿ�����״������
	g_GH.bPillarBSChart = GetPrivateProfileInt("Other","PillarBSChart",0,Ini);
	//��ʱ���Ʋ��ô���
	g_GH.bBoldZSTLine = GetPrivateProfileInt("Other","BoldZSTLine",0,Ini);
	//�Ƿ��Ǵ��ֲ˵�����
	g_GH.bBigHq = GetPrivateProfileInt("Other","BigHQ",1,Ini);	
	//�Ƿ�������Ϊ������Ϣ��ʽ
	g_GH.bLeftHQInfo = GetPrivateProfileInt("Other","LeftHQInfo",0,Ini);	
	//�Ƿ��Ǵ���״̬��
	g_GH.bBigStatusBar = GetPrivateProfileInt("Other","BigStatusBar",0,Ini);
	g_GH.bBigStatusBar = TRUE;
	//��ɫ���ֳɽ�����ʾ
	g_GH.bDiffVolColor = GetPrivateProfileInt("Other","DiffVolColor",1,Ini);
	//��ʱͼ�Ҳ��᲻��ʾ����
	g_GH.bEmptyZSTRight = GetPrivateProfileInt("Other","EmptyZSTRight",0,Ini);
	//���̷�ʱͼ����ʾ����,����ʾ����Ȩָ����
	g_GH.bZsShowAverage = GetPrivateProfileInt("Other","ZsShowAverage",0,Ini);
	g_GH.bZsShowLX = GetPrivateProfileInt("Other","ZsShowLX",1,Ini);
	//���ý�ϸ�ķ�ʱͼ����
	g_GH.bDenseZSTAxis = GetPrivateProfileInt("Other","DenseZSTAxis",1,Ini);
	//Level2ʮ����������²�
	g_GH.bHq10Bottom = GetPrivateProfileInt("Other","Hq10Bottom",0,Ini);
	//ȱʡ������
	g_GH.nDefPeriod = GetPrivateProfileInt("Other","DefPeriod",PER_DAY,Ini);
	g_GH.bHlight_HighLow = GetPrivateProfileInt("Other","Hlight_HighLow",0,Ini);

	g_GH.bCheckRisk = GetPrivateProfileInt("Other","CheckRisk",0,Ini);
	//Ȩ֤�̿����Ƿ�ʹ��10��ģʽ
	g_GH.bKQS_Use10 = GetPrivateProfileInt("Other","KQS_Use10",1,Ini);
	//Ȩ֤�̿����Ƿ������Ʊ
	g_GH.bKQS_RelationGP = GetPrivateProfileInt("Other","KQS_RelationGP",0,Ini);
	//������Ѷ��ص�����
	g_GC.bSupportStatusRoll = TRUE;
	if(g_GC.bSupportStatusRoll)
	{
		g_GH.Roll.bRollOpen[0] = GetPrivateProfileInt("Roll","RollOpen",0,Ini);
		if(g_GC.bSupportSlowStatusRoll)	
			g_GH.Roll.bRollOpen[1] = GetPrivateProfileInt("Roll","RollOpen_Slow",1,Ini);
	}
	g_GH.Roll.nScrollType[0] = GetPrivateProfileInt("Roll","ScrollType",0,Ini);
	g_GH.Roll.nScrollType[1] = GetPrivateProfileInt("Roll","ScrollType_Slow",0,Ini);
	g_GH.Roll.nInfoType[0] = GetPrivateProfileInt("Roll","InfoType",0,Ini);
	g_GH.Roll.nInfoType[1] = GetPrivateProfileInt("Roll","InfoType_Slow",0,Ini);
	g_GH.Roll.nMovingSpaceTime[0] = GetPrivateProfileInt("Roll","MovingSpaceTime",16,Ini);
	g_GH.Roll.nMovingSpaceTime[1] = GetPrivateProfileInt("Roll","MovingSpaceTime_Slow",36,Ini);
	g_GH.Roll.nMovingStopTime[0] = GetPrivateProfileInt("Roll","MovingStopTime",50,Ini);
	g_GH.Roll.nMovingStopTime[1] = GetPrivateProfileInt("Roll","MovingStopTime_Slow",50,Ini);
	g_GH.Roll.crColor[0] = GetPrivateProfileInt("Roll","Color",RGB(0,0,255),Ini);
	g_GH.Roll.crColor[1] = GetPrivateProfileInt("Roll","Color_Slow",RGB(0,0,255),Ini);

	g_GH.Roll.bShow = g_GH.Roll.bRollOpen[0] || g_GH.Roll.bRollOpen[1];
	g_GH.Roll.bShow = TRUE;
	g_GH.Roll.bFloat = !g_GC.bSupportStatusRoll;
	g_GH.Roll.bTopMost = GetPrivateProfileInt("Roll","TopMost",0,Ini);
	//
	g_nTQFlag = GetPrivateProfileInt("Other","TQType",1,Ini);
	g_nTQFlag = 1;
	g_bFixedXAxis = GetPrivateProfileInt("Other","BeFixedXAxis",0,Ini);
	g_nFixedXAxis = GetPrivateProfileInt("Other","FixedXAxisNum",20,Ini);
#if defined(OEM_STAR) || defined(OEM_STAR_HM)
	g_nTQFlag = 1;
#endif
	g_nPartTq = GetPrivateProfileInt("Other","PartTQ",0,Ini);
	if(g_nPartTq!=0 || g_nPartTq!=1) g_nPartTq=0;
	g_nTqStart = GetPrivateProfileInt("Other","TqStart",20060101,Ini);
	g_nMainAxis = GetPrivateProfileInt("Other","MainAxis",0,Ini);
	//��֧���Զ������İ汾,���ö�����ʾ�ķ�ʽ
	g_GH.nLastHintAutoupTime = GetPrivateProfileInt("LevelUp","LastTime",0,Ini);
	if(g_GH.nLastHintAutoupTime==0)
	{
		g_GH.nLastHintAutoupTime = g_iToday;
		WritePrivateProfileString("LevelUp","LastTime",IntToStr(g_GH.nLastHintAutoupTime),Ini);
	}
	g_GH.bDrawMainZb = (BOOL)GetPrivateProfileInt("Other","DrawMainZb",1,Ini);
	g_GH.bDynaTJBlock = (BOOL)GetPrivateProfileInt("Other","DynaTJBlock",1,Ini);
	//ʹ�ô���
	g_GH.lUseCount = GetPrivateProfileInt("Other","UseCount",0,Ini);
	g_GH.lUseCount++;
	//
//	GetPrivateProfileString("Other","UserDef_CudPad",_F("ӥ�۶���"),g_DefCusPadName,64,Ini);
	char PPath[MAX_PATH]={0};
	GetPrivateProfileString("Other","PersonalPath",g_WSXHStr+"export\\",PPath,MAX_PATH,Ini);
	g_PersonalPath = g_WSXHStr;
	
	g_nHZQZFlag = GetPrivateProfileInt("Other","GridHZType",0,Ini);
	g_nZHStyle = GetPrivateProfileInt("Other","ZHStyle",STYLE_3X3,Ini);
	g_nZHStyle = STYLE_3X3;
	g_nZBReportSortIndex = GetPrivateProfileInt("Other","ZBReportSortIndex",2,Ini);
	g_bShowCustomPadSwitch = GetPrivateProfileInt("Other","ShowCustomPadSwitch",1,Ini);
	g_bRightShow = GetPrivateProfileInt("Other","RightShow",1,Ini);
	//״̬��ָ��
	GetPrivateProfileString("Other","StatusGP1_Script",_F("��֤"),g_StatusGP[0].lpScript,9,Ini);
	GetPrivateProfileString("Other","StatusGP2_Script",_F("��֤"),g_StatusGP[1].lpScript,9,Ini);
	GetPrivateProfileString("Other","StatusGP3_Script",_F("��ҵ"),g_StatusGP[2].lpScript,9,Ini);
	g_StatusGP[0].setcode=GetPrivateProfileInt("Other","StatusGP1_SetCode",SH,Ini);
	g_StatusGP[1].setcode=GetPrivateProfileInt("Other","StatusGP2_SetCode",SZ,Ini);
	g_StatusGP[2].setcode=GetPrivateProfileInt("Other","StatusGP3_SetCode",SZ,Ini);
	GetPrivateProfileString("Other","StatusGP1_Code",SH_DPCODE,g_StatusGP[0].Code,CODE_SIZE,Ini);
	g_StatusGP[0].Code[SH_CODE_LEN]=0;
	GetPrivateProfileString("Other","StatusGP2_Code",SZ_DPCODE,g_StatusGP[1].Code,CODE_SIZE,Ini);
	g_StatusGP[1].Code[SH_CODE_LEN]=0;
	GetPrivateProfileString("Other","StatusGP3_Code",CYB_DPCODE,g_StatusGP[2].Code,CODE_SIZE,Ini);
	g_StatusGP[2].Code[SH_CODE_LEN]=0;
	//
	g_nLastZhType=GetPrivateProfileInt("Other","Embed_LastZhType",0,Ini);
	g_nLastYYBSel=GetPrivateProfileInt("Other","Embed_LastYYBSel",0,Ini);
	GetPrivateProfileString("Other","Embed_LastYYBID","",g_strLastYYBID,5,Ini);	
	g_bSaveZH=GetPrivateProfileInt("OTHER","SAVEZH",1,g_WSXHStr+"newmodem.ini");
	if(g_bSaveZH)
	{
		GetPrivateProfileString("Other","Embed_LastZhInfo","",g_strLastZhInfo,30,Ini);
		if(!g_OEM.bYhzqEmbedWTVersion)
		{
			g_nSaveZhNum=GetPrivateProfileInt("Other","Embed_SaveZhNum",0,Ini);
			g_nSaveZhNum=max(g_nSaveZhNum,0);
			g_nSaveZhNum=min(g_nSaveZhNum,MAX_SAVE_ZHNUM);
			for(i=0;i<g_nSaveZhNum;i++)
			{
				sprintf(tempStr,"Embed_SaveZH%02d",i+1);
				GetPrivateProfileString("Other",tempStr,"",g_strSaveZhInfo[i],30,Ini);
			}
		}
	}
	g_bProtectZH=GetPrivateProfileInt("OTHER","ENCRYPTACCOUNT",0,g_WSXHStr+"newmodem.ini");
	g_nLastAqfs=GetPrivateProfileInt("OTHER","Embed_LastAqfs",0,Ini);

	//--------------------------------------------------------------------------------------------
	//�û���ϵͳ���ܸĶ���INI
	//�Ƿ��и��ؾ���
	g_bNLB = GetPrivateProfileInt("Other","NLB",0,g_WSXHStr+"syscomm.ini");
	g_bNLB = TRUE;
}

BOOL GetFromINI()
{
	int i;
	CString	Ini = g_WSXHStr+(CString)g_strYxhjCfg;

	//���л���,��������Ŀ¼
	sprintf(g_TdxCfg.DocPath,"%sLData\\",g_WSXHStr);
	//����ʱ������
	g_TdxCfg.GPFZNUM = (unsigned short)GetPrivateProfileInt("File","gpfznum",250,Ini);
	g_TdxCfg.QHFZNUM = (unsigned short)GetPrivateProfileInt("File","qhfznum",360,Ini);
	for(i = 0;i < 6;i++)
	{   
		sz[i] = (short)GetPrivateProfileInt("TIME", lpszTimeStr[i] , (int)(sz[i]),Ini);
 		sh[i] = (short)GetPrivateProfileInt("TIME", lpszTimeStr[i+6],(int)(sh[i]),Ini);
	}
	sz[6] = sz[7] = sz[5];
	sh[6] = sh[7] = sh[5];

	//���������
	g_TdxCfg.JbmNum = 1;
	strcpy(g_TdxCfg.Jbmname[0],"��������1");
	strcpy(g_TdxCfg.SZBASE[0],HomePath+"RemoteSZ\\");
	strcpy(g_TdxCfg.SHBASE[0],HomePath+"RemoteSH\\");
	strcpy(g_TdxCfg.Jbmname[1],"��������2");
	strcpy(g_TdxCfg.SZBASE[1],HomePath+"RemoteSZ\\");
	strcpy(g_TdxCfg.SHBASE[1],HomePath+"RemoteSH\\");
	strcpy(g_TdxCfg.Jbmname[2],"��������3");
	strcpy(g_TdxCfg.SZBASE[2],HomePath+"RemoteSZ\\");
	strcpy(g_TdxCfg.SHBASE[2],HomePath+"RemoteSH\\");
	//�û���INI����
	GetFromUserINI();
	//�û��������
	LoadShortCutCfg();
	return TRUE;
}

void WriteToUserINI()
{
	CString	Ini = g_WSXHStr+g_strUserCfg;
	char tempStr[256];

	//��������
	WritePrivateProfileString("Other","NetCondition",IntToStr(g_nNetCondition),Ini);

	WritePrivateProfileString("Other","StartTips",bStartTips?"1":"0" ,Ini);
	nTipFilePos += 1; //Tip�ļ�λ�ü�1
	WritePrivateProfileString("Other","TipFilePos",IntToStr(nTipFilePos),Ini);

	WritePrivateProfileString("Other","bTreeShow",bTreeShow?"1":"0" ,Ini);
	WritePrivateProfileString("Other","bRestShow",bRestShow?"1":"0" ,Ini);
	WritePrivateProfileString("Other","LastTreeSel",IntToStr(nLastTreeSel),Ini);
	WritePrivateProfileString("Other","LastRestSel",IntToStr(nLastRestSel),Ini);

	WritePrivateProfileString("Other","AutoPageTime",IntToStr(AutoPageTime),Ini);
	WritePrivateProfileString("Other","HQRefreshTime",IntToStr(HQRefreshTime),Ini);
	WritePrivateProfileString("Other","StatusRefreshTime",IntToStr(StatusRefreshTime),Ini);
	WritePrivateProfileString("Other","ZFPeriod",IntToStr(g_nZFFzNum),Ini);
	
	WritePrivateProfileString("Other","FirstGPType",IntToStr(FirstGPType),Ini);
	WritePrivateProfileString("Other","FirstBlockName",FirstBlockName,Ini);
	WritePrivateProfileString("Other","FirstBlockFlag",IntToStr(FirstBlockFlag),Ini);

	WritePrivateProfileString("Other","FirstHisGPType",IntToStr(FirstHisGPType),Ini);
	WritePrivateProfileString("Other","FirstHisBlockName",FirstHisBlockName,Ini);
	WritePrivateProfileString("Other","FirstHisBlockFlag",IntToStr(FirstHisBlockFlag),Ini);

	WritePrivateProfileString("Other","CurrentJBM",IntToStr(g_CurrentJBM),Ini);

	//ǿ��������������
	WritePrivateProfileString("Other","QRParam1",IntToStr(QRParam[0]),Ini);
	WritePrivateProfileString("Other","QRParam2",IntToStr(QRParam[1]),Ini);
	WritePrivateProfileString("Other","QRParam3",IntToStr(QRParam[2]),Ini);
	WritePrivateProfileString("Other","QRParam4",IntToStr(QRParam[3]),Ini);
	WritePrivateProfileString("Other","QRParam5",IntToStr(QRParam[4]),Ini);

	//
	WritePrivateProfileString("Other","BigFontGrid",g_bBigFontGrid?"1":"0",Ini); 

	//HQDialog����������Ϣ
	WritePrivateProfileString("Other","HQX",IntToStr(g_InfoDlgPosx),Ini);
	WritePrivateProfileString("Other","HQY",IntToStr(g_InfoDlgPosy),Ini);
	WritePrivateProfileString("Other","HQMINIMIZE",g_bInfoMinimize?"1":"0",Ini);

	//����������Ϣ
	WritePrivateProfileString("Other","RealUPK",IntToStr(int(g_GH.bRealUPK)),Ini);
	WritePrivateProfileString("Other","LocalData",IntToStr(int(g_GH.bLocalData)),Ini);
	WritePrivateProfileString("Other","InitKNum",IntToStr(int(g_GH.nInitKNum)),Ini);
	WritePrivateProfileString("Other","InitKNum2",IntToStr(int(g_GH.nInitKNum2)),Ini);
	WritePrivateProfileString("Other","ShowKGap",IntToStr(int(g_GH.bShowKGap)),Ini);
	WritePrivateProfileString("Other","ShowMmpChg",IntToStr(int(g_GH.bShowMmpChg)),Ini);
	WritePrivateProfileString("Other","OnlyDownAGData",g_GH.bOnlyDownAGData?"1":"0",Ini);
	WritePrivateProfileString("Other","FullScreenNotify",g_GH.bFullScreenNotify?"1":"0",Ini);
	WritePrivateProfileString("Other","AddBlockPrompt",g_GH.bAddBlockPrompt?"1":"0",Ini);
	WritePrivateProfileString("Other","LockHQSort",g_GH.bLockHQSort?"1":"0",Ini);
	WritePrivateProfileString("Other","ShowBS",g_GH.bShowBS?"1":"0",Ini);
	WritePrivateProfileString("Other","Tick2Priority",g_GH.bTick2Priority?"1":"0",Ini);
	WritePrivateProfileString("Other","BigVol",IntToStr(g_GH.StatBigVol),Ini);
	WritePrivateProfileString("Other","PinAssistPad",g_GH.bPinAssistPad?"1":"0",Ini);
	WritePrivateProfileString("Other","CodeCanSort",g_GH.bCodeCanSort?"1":"0",Ini);
	WritePrivateProfileString("Other","ShowTPPriceInZST",g_GH.bShowTPPriceInZST?"1":"0",Ini);

	WritePrivateProfileString("Other","CheckUrgentDate",IntToStr(g_GH.lCheckUrgentDate),Ini);
	WritePrivateProfileString("Other","CheckUrgent",g_GH.bCheckUrgent?"1":"0",Ini);

	WritePrivateProfileString("Other","MainZb",IntToStr(g_nMainZbFlag),Ini);
	WritePrivateProfileString("Other","XHidden",g_bHasXHidden?"1":"0",Ini);
	WritePrivateProfileString("Other","YHidden",g_bHasYHidden?"1":"0",Ini);
	WritePrivateProfileString("Other","FirstChartZb",g_strZbMain,Ini);
	WritePrivateProfileString("Other","LastExp",g_strLastExp,Ini);
	WritePrivateProfileString("Other","LastKLine",g_strLastKLine,Ini);
	WritePrivateProfileString("Other","LastZBSort",g_strLastZBSort,Ini);
	for(int i=0;i<11;i++)
	{
		sprintf(tempStr,"FavUseZb%02d",i);
		WritePrivateProfileString("Other",tempStr,g_strFavUseZb[i],Ini);
	}
	WritePrivateProfileString("Other","FixedWidth",IntToStr(g_nFixedWidth),Ini);
	WritePrivateProfileString("Other","FixedWidth2",IntToStr(g_nFixedWidth2),Ini);

	WritePrivateProfileString("Other","MulNum",IntToStr(g_nNewMulNum),Ini);

	WritePrivateProfileString("Other","HasZBFlag",g_bZBudFlag?"1":"0",Ini);
	WritePrivateProfileString("Other","MinNNum",IntToStr(g_nMinnNum),Ini);
	WritePrivateProfileString("Other","DayNNum",IntToStr(g_nDaynNum),Ini);
	
	WritePrivateProfileString("Other","HLIGHTNOWVOL",IntToStr(g_nHligtNowVol),Ini);
	
	sprintf(tempStr,"%.f",g_fProfitFee);
	WritePrivateProfileString("Other","ProfitFee",tempStr,Ini);

	WritePrivateProfileString("Other","VOLTQ",g_bVolTQ?"1":"0",Ini);
	WritePrivateProfileString ("Other","GZPERMIN",IntToStr(g_nGzPerMin),Ini);
	WritePrivateProfileString ("Other","ZBWINNUM",IntToStr(g_nZBWinNum),Ini);

	WritePrivateProfileString ("Other","WarnSwitch",g_bAllWarnSwitch?"1":"0",Ini);
	WritePrivateProfileString ("Other","TjWarnSwitch",g_bTjWarnSwitch?"1":"0",Ini);
	
	//�˳�ϵͳ���Ƿ���ʾ��������
//	WritePrivateProfileString ("Other","NotifyDownLoad",g_bNotifyDownload?"1":"0",Ini);
	//�����Զ�д��
//	WritePrivateProfileString ("Other","Day_WriteDisk",g_bDay_WriteDisk?"1":"0",Ini);
	//��������Ƿ񾫳���Ȩ
//	WritePrivateProfileString ("Other","HisViewTQ",g_bHisViewTQ?"1":"0",Ini);
	//���ܼ���������һ��
//	WritePrivateProfileString ("Other","FindRadio",g_nFindRadio?"1":"0",Ini);	
	//��������λ��
//	WritePrivateProfileString ("Other","ToolBarPos",IntToStr(g_nToolBarPos),Ini);	
//	WritePrivateProfileString ("Other","AutoHideBar",g_bAutoHideBar?"1":"0",Ini);	
	//�Ƿ����������ʾ���ױ��
//	WritePrivateProfileString ("Other","ShowHqMine",g_bShowHqMine?"1":"0",Ini);	
	//�Ƿ���ɫ����
//	WritePrivateProfileString("Other","DrawBlue",g_bDrawBlue?"1":"0",Ini);
	//�Ƿ��ǳ��ĸ�����
//	WritePrivateProfileString ("Other","LongHighLine",g_bLongHighLine?"1":"0",Ini);	
	//��������
//	WritePrivateProfileString ("HOTKEY","VIRTKEY",IntToStr(g_virtkey),Ini);	
//	WritePrivateProfileString ("HOTKEY","COMBOKEY",IntToStr(g_comkey),Ini);	
//	WritePrivateProfileString ("HOTKEY","INFO",g_hideinfo?"1":"0",Ini);	
//	WritePrivateProfileString ("HOTKEY","BOSSKEY",g_bBossKey?"1":"0",Ini);	
	//�˵����
//	WritePrivateProfileString ("Other","MenuMode",g_nMenuMode?"1":"0",Ini);	
//	WritePrivateProfileString ("Other","ExtendXPStyle",g_bExtendXPStyle?"1":"0",Ini);	
//	WritePrivateProfileString ("Other","ClassXP",g_bClassXP?"1":"0",Ini);	

	WritePrivateProfileString ("Other","Min2Tray",g_bMin3Tray?"1":"0",Ini);
	//����ͼ
	WritePrivateProfileString("Other","JiangEnStep",FloatToStr(g_fJiangEnStep),Ini);
	WritePrivateProfileString("Other","JiangEnMode",IntToStr(g_nJiangEnMode),Ini);
	//�ɱ�ͼ����
	WritePrivateProfileString("Other","CBSwitch",IntToStr(g_nCBSwitch),Ini);
	//�Ƿ�ʹ���Զ�����
	WritePrivateProfileString("Other","UseAutoConnect",g_bUseAutoConnect?"1":"0",Ini);
	//�Ƿ�����
	WritePrivateProfileString("Other","ActTogether",g_bActTogether?"1":"0",Ini);
	//�Ƿ񵯳�����С����
//	WritePrivateProfileString("Other","FxtShowInfoDlg",g_bFxtShowInfoDlg?"1":"0",Ini);
	//�Ƿ���ʾ����ͼ�е�ָ���л�TAB
	WritePrivateProfileString("Other","FxtShowTab",g_bFxtShowTab?"1":"0",Ini);
	//�Ƿ�ͨ���л�TAB
	WritePrivateProfileString("Other","TranparentTab",g_bTranparentTab?"1":"0",Ini);
	//�ϴεĵ�¼��ʽ
//	WritePrivateProfileString("Other","LastLoginType",IntToStr(g_nLastLoginType),Ini);
	//һЩ�û�ϰ��
	WritePrivateProfileString("Other","AWeekNoHint",g_GH.AWeekNoHint?"1":"0",Ini);
	WritePrivateProfileString("Other","NoHintStartDate",IntToStr(g_GH.NoHintStartDate),Ini);
	WritePrivateProfileString("Other","NoXHCol",g_GH.NoXHCol?"1":"0",Ini);
	WritePrivateProfileString("Other","NoVScroll",g_GH.NoVScroll?"1":"0",Ini);
	WritePrivateProfileString("Other","UserDef_CudPad",g_DefCusPadName,Ini);
	WritePrivateProfileString("Other","ShowJUNJ",IntToStr(g_GH.nShowJUNJ),Ini);
	WritePrivateProfileString("Other","CurBlockFlag",IntToStr(g_GH.nCurBlockFlag),Ini);
	WritePrivateProfileString("Other","PerScale",IntToStr(g_GH.nPerScale),Ini);
	WritePrivateProfileString("Other","PerSect",IntToStr(g_GH.nPerSect),Ini);
	WritePrivateProfileString("Other","PersonalPath",g_PersonalPath,Ini);
	WritePrivateProfileString("Other","TQType",IntToStr(g_nTQFlag),Ini);
	WritePrivateProfileString("Other","PartTQ",IntToStr(g_nPartTq),Ini);
	WritePrivateProfileString("Other","TqStart",IntToStr(g_nTqStart),Ini);
	WritePrivateProfileString("Other","MainAxis",IntToStr(g_nMainAxis),Ini);
	WritePrivateProfileString("Other","BeFixedXAxis",IntToStr(g_bFixedXAxis),Ini);
	WritePrivateProfileString("Other","FixedXAxisNum",IntToStr(g_nFixedXAxis),Ini);
	WritePrivateProfileString("Other","DrawMainZb",IntToStr(int(g_GH.bDrawMainZb)),Ini);
	WritePrivateProfileString("Other","DynaTJBlock",IntToStr(int(g_GH.bDynaTJBlock)),Ini);
	//ʹ�ô���
	WritePrivateProfileString("Other","UseCount",IntToStr(g_GH.lUseCount),Ini);
	//�Ƿ�����״������
	WritePrivateProfileString("Other","PillarBSChart",g_GH.bPillarBSChart?"1":"0",Ini);
	//��ʱ���Ʋ��ô���
	WritePrivateProfileString("Other","BoldZSTLine",g_GH.bBoldZSTLine?"1":"0",Ini);
	//�������Ϊ������Ϣ��ʽ
	WritePrivateProfileString("Other","LeftHQInfo",g_GH.bLeftHQInfo?"1":"0",Ini);
	//�Ƿ��Ǵ��ֲ˵�����
	WritePrivateProfileString("Other","BigHQ",g_GH.bBigHq?"1":"0",Ini);
	//��ɫ���ֳɽ�����ʾ
	WritePrivateProfileString ("Other","DiffVolColor",g_GH.bDiffVolColor?"1":"0",Ini);	
	//��ʱͼ�Ҳ��᲻��ʾ����
	WritePrivateProfileString("Other","EmptyZSTRight",g_GH.bEmptyZSTRight?"1":"0",Ini);
	//���̷�ʱͼ����ʾ����,����ʾ����Ȩָ����
	WritePrivateProfileString("Other","ZsShowAverage",g_GH.bZsShowAverage?"1":"0",Ini);
	WritePrivateProfileString("Other","ZsShowLX",g_GH.bZsShowLX?"1":"0",Ini);
	//���ý�ϸ�ķ�ʱͼ����
	WritePrivateProfileString("Other","DenseZSTAxis",g_GH.bDenseZSTAxis?"1":"0",Ini);	
	//ȱʡ������
	WritePrivateProfileString("Other","DefPeriod",IntToStr(g_GH.nDefPeriod),Ini);
	//����������ǵ�ͣ��λ�Ĺ�Ʊ�ڱ��۱���߼ۻ���ͼ۴�������ʾ
	WritePrivateProfileString("Other","Hlight_HighLow",g_GH.bHlight_HighLow?"1":"0",Ini);
	//����ѡ��
	WritePrivateProfileString("Other","CheckRisk",g_GH.bCheckRisk?"1":"0",Ini);
	//������Ѷ��ص�����
	WritePrivateProfileString("Roll","RollOpen",g_GH.Roll.bRollOpen[0]?"1":"0",Ini);
	WritePrivateProfileString("Roll","RollOpen_Slow",g_GH.Roll.bRollOpen[1]?"1":"0",Ini);
	WritePrivateProfileString("Roll","ScrollType",IntToStr(g_GH.Roll.nScrollType[0]),Ini);
	WritePrivateProfileString("Roll","ScrollType_Slow",IntToStr(g_GH.Roll.nScrollType[1]),Ini);
	WritePrivateProfileString("Roll","InfoType",IntToStr(g_GH.Roll.nInfoType[0]),Ini);
	WritePrivateProfileString("Roll","InfoType_Slow",IntToStr(g_GH.Roll.nInfoType[1]),Ini);
	WritePrivateProfileString("Roll","MovingSpaceTime",IntToStr(g_GH.Roll.nMovingSpaceTime[0]),Ini);
	WritePrivateProfileString("Roll","MovingSpaceTime_Slow",IntToStr(g_GH.Roll.nMovingSpaceTime[1]),Ini);
	WritePrivateProfileString("Roll","MovingStopTime",IntToStr(g_GH.Roll.nMovingStopTime[0]),Ini);
	WritePrivateProfileString("Roll","MovingStopTime_Slow",IntToStr(g_GH.Roll.nMovingStopTime[1]),Ini);
	WritePrivateProfileString("Roll","Color",IntToStr(g_GH.Roll.crColor[0]),Ini);
	WritePrivateProfileString("Roll","Color_Slow",IntToStr(g_GH.Roll.crColor[1]),Ini);

	WritePrivateProfileString("Roll","TopMost",g_GH.Roll.bTopMost?"1":"0",Ini);
	//Level2ʮ����������²�
	WritePrivateProfileString("Other","Hq10Bottom",g_GH.bHq10Bottom?"1":"0",Ini);
	//�б����ƽ��ֵȨ��
	WritePrivateProfileString("Other","GridHZType",IntToStr(g_nHZQZFlag),Ini);
	WritePrivateProfileString("Other","ZHStyle",IntToStr(g_nZHStyle),Ini);
	WritePrivateProfileString("Other","ZBReportSortIndex",IntToStr(g_nZBReportSortIndex),Ini);
	WritePrivateProfileString("Other","ShowCustomPadSwitch",g_bShowCustomPadSwitch?"1":"0",Ini);
	WritePrivateProfileString("Other","RightShow",g_bRightShow?"1":"0",Ini);
	//״̬��
	WritePrivateProfileString("Other","StatusGP1_Script",g_StatusGP[0].lpScript,Ini);
	WritePrivateProfileString("Other","StatusGP2_Script",g_StatusGP[1].lpScript,Ini);
	WritePrivateProfileString("Other","StatusGP3_Script",g_StatusGP[2].lpScript,Ini);
	WritePrivateProfileString("Other","StatusGP1_SetCode",IntToStr(g_StatusGP[0].setcode),Ini);
	WritePrivateProfileString("Other","StatusGP2_SetCode",IntToStr(g_StatusGP[1].setcode),Ini);
	WritePrivateProfileString("Other","StatusGP3_SetCode",IntToStr(g_StatusGP[2].setcode),Ini);
	WritePrivateProfileString("Other","StatusGP1_Code",g_StatusGP[0].Code,Ini);
	WritePrivateProfileString("Other","StatusGP2_Code",g_StatusGP[1].Code,Ini);
	WritePrivateProfileString("Other","StatusGP3_Code",g_StatusGP[2].Code,Ini);

	WritePrivateProfileString("Other","Embed_LastZhType",IntToStr(g_nLastZhType),Ini);
	WritePrivateProfileString("Other","Embed_LastYYBSel",IntToStr(g_nLastYYBSel),Ini);
	WritePrivateProfileString("Other","Embed_LastYYBID",g_strLastYYBID,Ini);	
	if(g_bSaveZH)
	{
		WritePrivateProfileString("Other","Embed_LastZhInfo",g_strLastZhInfo,Ini);
		if(!g_OEM.bYhzqEmbedWTVersion)
		{
			WritePrivateProfileString("Other","Embed_SaveZhNum",IntToStr(g_nSaveZhNum),Ini);
			for(i=0;i<g_nSaveZhNum;i++)
			{
				sprintf(tempStr,"Embed_SaveZH%02d",i+1);
				WritePrivateProfileString("Other",tempStr,g_strSaveZhInfo[i],Ini);
			}
		}
	}
	WritePrivateProfileString("Other","Embed_LastAqfs",IntToStr(g_nLastAqfs),Ini);

	WritePrivateProfileString("Other","TxtFontSize",IntToStr(g_nTxtFontSize),Ini);
	WritePrivateProfileString("Other","TxtColor",IntToStr(int(g_nTxtColor)),Ini);

	//-----------------------------------------------------------------------------
	//�û���ϵͳ���ܸĶ���INI
	//�Ƿ��и��ؾ���
//	WritePrivateProfileString("Other","NLB",g_bNLB?"1":"0",g_WSXHStr+"syscomm.ini");

	//ע��:��ǰ��ʲô��¼ģʽ,����TopView.ini��
	if(g_bSaveEmbedTabSel)
		WritePrivateProfileString("Other","EmbedTabSel",IntToStr(g_nEmbedTabSel),g_WSXHStr+g_strYxhjCfg);
}

void GetFromOther()
{
	int i;
	CFile ColWarnFile, ColWarnTjExFile,ColWarnFile2,ColWarnSelfFile;
	BOOL bHasError = TRUE, bHasNoTj = TRUE;
	
	memcpy(g_aHKGPIdx,g_aHKGPIdx_Def,sizeof(long)*HKGP_COL_NUM);
	memcpy(g_aFOREGPIdx,g_aFOREGPIdx_Def,sizeof(long)*FOREGP_COL_NUM);
	memcpy(g_aFEIdx,g_aFEIdx_Def,sizeof(long)*FE_COL_NUM);
	memcpy(g_aQHIdx,g_aQHIdx_Def,sizeof(long)*QH_COL_NUM);
	memcpy(g_aIDXIdx,g_aIDXIdx_Def,sizeof(long)*IDX_COL_NUM);
	memcpy(g_aDDEIdx,g_aDDEIdx_Def,sizeof(long)*DDE_COL_NUM);
	memcpy(g_aA2HIdx,g_aA2HIdx_Def,sizeof(long)*A2H_COL_NUM);
	memcpy(g_aJSZJIdx,g_aJSZJIdx_Def,sizeof(long)*JSZJ_COL_NUM);
	memcpy(g_aSignalIdx,g_aSignalIdx_Def,sizeof(long)*SIGNAL_COL_NUM);

	g_aAllItemWidth = new float[TOTAL_COL];
	memcpy(g_aAllItemWidth,g_aAllItemWidth_Def,TOTAL_COL*sizeof(float));

	g_pAbsCondition = new PWARNINGCND[MAX_ABSWARNMAXNUM];
	memset(g_pAbsCondition,0,MAX_ABSWARNMAXNUM*sizeof(PWARNINGCND));
	if(ColWarnFile.Open(g_WSXHStr+"warna.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		//��Ŀ˳��
		ColWarnFile.Read(g_aABGPIdx,ABGPHQ_COL_NUM*sizeof(long));
		if(g_aABGPIdx[1] != 0)	//�����Ʊ���������⣬���ʾ�д���
		{
			//����ȫ�ֱ���
			ColWarnFile.Read(&g_WarnParam,sizeof(WARNGLOBALPARAM));
			//������Ա�������
			ColWarnFile.Read(&g_pAbsConditionNum,sizeof(g_pAbsConditionNum));
			g_pAbsConditionNum = max(g_pAbsConditionNum,0);
			g_pAbsConditionNum = min(MAX_ABSWARNMAXNUM,g_pAbsConditionNum);
			ColWarnFile.Read(g_pAbsCondition, g_pAbsConditionNum*sizeof(PWARNINGCND));
			bHasError = FALSE;
		}
		ColWarnFile.Close();
	}
	if(ColWarnTjExFile.Open(g_WSXHStr+"warnb.dat", CFile::modeRead|CFile::shareDenyNone))
	{
		ColWarnTjExFile.Read(&g_aWarnTjEx[0], MAX_TJWARN*sizeof(TJ_WARN));
		bHasNoTj = FALSE;
		ColWarnTjExFile.Close();
	}
	if(bHasError)
	{
		//������ȱʡֵ
		memcpy(g_aABGPIdx,g_aABGPIdx_Def,sizeof(long)*ABGPHQ_COL_NUM);

		g_WarnParam.nLB = 20;
		g_WarnParam.fUpRatio = 0.05f;
		g_WarnParam.fDownRatio = 0.05f;
		g_WarnParam.fTickAmount = 1500000.0f;
		g_WarnParam.fTickDiff = 0.012f;
		g_WarnParam.fTickVol = 0.001f;
		g_WarnParam.bLowHigh = TRUE;
		g_WarnParam.bHighLow = TRUE;
		g_WarnParam.bLB			= TRUE;
		g_WarnParam.bUpRatio	= TRUE;
		g_WarnParam.bDownRatio	= TRUE;
		g_WarnParam.bTickAmount = TRUE;
		g_WarnParam.bTickDiff	= TRUE;
		g_WarnParam.bTickVol	= TRUE;
		g_WarnParam.bNewMaxWarn = FALSE;
		g_WarnParam.bNewMinWarn = FALSE;
		g_WarnParam.bStopClose  = TRUE;
		g_WarnParam.bFzZF		= TRUE;
		g_WarnParam.bHasBigBuy = TRUE;
		g_WarnParam.bHasBigSell = TRUE;
		g_WarnParam.bPopup = TRUE;
		g_WarnParam.bTickHugeAmo = TRUE;
		g_WarnParam.nTickAmoMinute = 5;
		g_WarnParam.nTickAmoTime = 3;
		g_WarnParam.nBeepFlag = 1;
		g_WarnParam.WavFile[0]=0;
	}
	if(bHasNoTj)
	{
		memset(g_aWarnTjEx, 0, MAX_TJWARN*sizeof(TJ_WARN));
		for(i = 0;i < MAX_TJWARN;i++)
		{
			g_aWarnTjEx[i].nTjIndexNo = -1;
			g_aWarnTjEx[i].nPeriod = DAY_ST;
		}
	}
	//���������ݵ���Ŀ����g_aABGPIdx��ȥ
	for(i = ABGPHQ_COL_NUM;i < ABGP_COL_NUM;i++)
		g_aABGPIdx[i] = g_aABGPIdx_Def[i];
	//�����Ԥ����Ϣ
	g_WarnParam2.bTJPopup = TRUE;
	g_WarnParam2.nTJBeepFlag = 1;
	g_WarnParam2.nWarnFlag = 0;
	g_WarnParam2.TJWavFile[0]=0;		
	if(ColWarnFile2.Open(g_WSXHStr+"warnpara.dat", CFile::modeRead|CFile::shareDenyNone))
	{
		ColWarnFile2.Read(&g_WarnParam2,sizeof(WARNGLOBALPARAM2));
		ColWarnFile2.Close();
	}
	if(ColWarnSelfFile.Open(g_WSXHStr+"warnc.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		PWARNINGCND wg={0};
		while(ColWarnSelfFile.Read(&wg,sizeof(PWARNINGCND))==sizeof(PWARNINGCND))
			g_aSelfWarnGp.push_back(wg);
		ColWarnSelfFile.Close();
	}
	
}

void WriteToOther()
{
	return;
	CFile ColFile,ColWarnFile,ColWarnFile2,ColWarnTjExFile,ColWarnSelfFile;
	
	if(ColWarnFile.Open(g_WSXHStr+"warna.dat",CFile::modeCreate|CFile::modeWrite))
	{
		ColWarnFile.Write(g_aABGPIdx,ABGPHQ_COL_NUM*sizeof(long));
		ColWarnFile.Write(&g_WarnParam,sizeof(WARNGLOBALPARAM));
		ColWarnFile.Write(&g_pAbsConditionNum,sizeof(g_pAbsConditionNum));
		ColWarnFile.Write(g_pAbsCondition,g_pAbsConditionNum * sizeof(PWARNINGCND));
		ColWarnFile.Close();
	}
	if(ColWarnTjExFile.Open(g_WSXHStr+"warnb.dat",CFile::modeCreate|CFile::modeWrite))
	{
		ColWarnTjExFile.Write(&g_aWarnTjEx, MAX_TJWARN*sizeof(TJ_WARN));
		ColWarnTjExFile.Close();
	}
	if(ColWarnFile2.Open(g_WSXHStr+"warnpara.dat",CFile::modeCreate|CFile::modeWrite))
	{
		ColWarnFile2.Write(&g_WarnParam2,sizeof(WARNGLOBALPARAM2));
		ColWarnFile2.Close();
	}
	if(ColWarnSelfFile.Open(g_WSXHStr+"warnc.dat",CFile::modeCreate|CFile::modeWrite))
	{
		ColWarnSelfFile.Write(&g_aSelfWarnGp[0],g_aSelfWarnGp.size()*sizeof(PWARNINGCND));
		ColWarnSelfFile.Close();
	}

}

void LoadBlockCfg()
{
	g_nBlockNum = 0;
	CFile theFile;
	if(theFile.Open(HomePath+g_strBlockNew+"slfeblock.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		int i = 0;
		while(theFile.Read(g_BlockInfo+i,sizeof(BlockInfo_Struct)) && i < USERBLOCK_NUM) //ֻ������USERBLOCK_NUM�����
			i++;
		g_nBlockNum = i;
		theFile.Close();
	}
	if(theFile.Open(g_WSXHStr+"calcblock.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		int i = 0;
		while(theFile.Read(g_BlockInfo+i,sizeof(BlockInfo_Struct)) && i < USERBLOCK_NUM) //ֻ������USERBLOCK_NUM�����
			i++;
		g_nBlockNum = i;
		theFile.Close();
	}
}

void SaveBlockCfg()
{
	CFile theFile;
	if(theFile.Open(HomePath+g_strBlockNew+"slfeblock.dat",CFile::modeCreate|CFile::modeWrite))
	{
		for(int i = 0;i < g_nBlockNum;i++)
			theFile.Write(g_BlockInfo+i,sizeof(BlockInfo_Struct));
		theFile.Close();
	}
	else
		TDX_MessageBox(NULL,"���ܽ���������ļ�����!",MB_OK|MB_ICONEXCLAMATION);
	if(theFile.Open(g_WSXHStr+"calcblock.dat",CFile::modeCreate|CFile::modeWrite))
	{
		for(int i = 0;i < g_nBlockNum;i++)
			theFile.Write(g_BlockInfo+i,sizeof(BlockInfo_Struct));
		theFile.Close();
	}
	else
		TDX_MessageBox(NULL,"���ܽ���������ļ�����!",MB_OK|MB_ICONEXCLAMATION);
}

void LoadBlockColorCfg(LPBLOCKCOLOR pBlockColor)
{
	//ȱʡֵ
	memset(pBlockColor,0,8*sizeof(BLOCKCOLOR));
	strcpy(pBlockColor[0].BlockName,"��ѡ��");
	pBlockColor[0].clrBlock=GridColor.Grid_Other_Color;
	pBlockColor[1].clrBlock=GridColor.Grid_CodeName_Color;
	pBlockColor[2].clrBlock=GridColor.Grid_CodeName_Color;
	pBlockColor[3].clrBlock=GridColor.Grid_CodeName_Color;
	pBlockColor[4].clrBlock=GridColor.Grid_CodeName_Color;
	pBlockColor[5].clrBlock=GridColor.Grid_CodeName_Color;
	pBlockColor[6].clrBlock=GridColor.Grid_CodeName_Color;
	pBlockColor[7].clrBlock=GridColor.Grid_CodeName_Color;

	return;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"block.clr",CFile::modeRead|CFile::shareDenyNone))
	{
		theFile.Read(pBlockColor,8*sizeof(BLOCKCOLOR));
		theFile.Close();
	}
}

void SaveBlockColorCfg(LPBLOCKCOLOR pBlockColor)
{
	return;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"block.clr",CFile::modeCreate|CFile::modeWrite))
	{
		theFile.Write(pBlockColor,8*sizeof(BLOCKCOLOR));
		theFile.Close();
	}
	else
		TDX_MessageBox(NULL,"���ܽ������ɫ���ô���!",MB_OK|MB_ICONEXCLAMATION);
}

void LoadOverlap()
{
	memset(g_Overlap,0,2*sizeof(OverlapInfo));
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"selfolap.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		theFile.Read(g_Overlap,2*sizeof(OverlapInfo));
		theFile.Close();
		//����
		g_Overlap[0].pNowStk=NULL;
		g_Overlap[0].pOverlapStk[0]=NULL;
		g_Overlap[0].pOverlapStk[1]=NULL;
		g_Overlap[0].pOverlapStk[2]=NULL;
		g_Overlap[1].pNowStk=NULL;
		g_Overlap[1].pOverlapStk[0]=NULL;
		g_Overlap[1].pOverlapStk[1]=NULL;
		g_Overlap[1].pOverlapStk[2]=NULL;
	}
}

void SaveOverlap()
{
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"selfolap.dat",CFile::modeCreate|CFile::modeWrite))
	{
		theFile.Write(g_Overlap,2*sizeof(OverlapInfo));
		theFile.Close();
	}
}

BOOL CTdxWApp::SetInitValue()
{
	memset(&g_NoTime,0,sizeof(NTime));
	time_t t = time(NULL);
	struct tm *today = localtime(&t);
	if(!today)
	{
		TDX_MessageBox(NULL,"����ʱ������������,����",MB_OK|MB_ICONERROR);
		return FALSE;
	}

	strcpy(g_ManyHostInfo.strHostName,_F(g_strPreName));
	strcpy(g_ManyHostInfo.NotifyStr,"");

	g_strLogoAdvert[0] = 0;
	memset(&g_GH,0,sizeof(struct Global_Habit));
	memset(&g_GC,0,sizeof(struct Global_Cfg));
	memset(&g_OEM,0,sizeof(struct Global_EmbOem));
	memset(&g_Url,0,sizeof(struct Global_Url));
	memset(&g_SysInfo,0,sizeof(struct Global_SysInfo));
	memset(&g_DS,0,sizeof(struct Global_DSCfg));
	memset(&g_JyInfo,0,sizeof(struct JyCookie_Info));
	memset(&g_AntiHack,0,sizeof(struct AntiHack_Cfg));
	memset(&g_NetParam,0,sizeof(struct NetParam_Cfg));
	memset(&g_F11WebPage,0,sizeof(CUSWEBPAGE));
	memset(&g_VipVerify_Info,0,sizeof(struct VipVerify_Info));
	g_VipVerify_Info.ReVerifySpan=80;
	memset(&g_L2HQHost,0,sizeof(TDXWHOST));
	memset(&g_VipHQHost,0,sizeof(TDXWHOST));

	memset(g_Overlap,0,2*sizeof(OverlapInfo));

#ifdef TESTVER
	strcpy(g_GC.Level2_SepcComte,"testcomte.dat");
#else
	strcpy(g_GC.Level2_SepcComte,"tdxcomte.dat");
#endif
	
	CTime tmpTime = CTime::GetCurrentTime();
	g_SysInfo.Program_StartYmd=tmpTime.GetYear()*10000L+100L*tmpTime.GetMonth()+tmpTime.GetDay();
	g_SysInfo.Program_StartHms=tmpTime.GetHour()*10000L+tmpTime.GetMinute()*100L+tmpTime.GetSecond();
	g_tNowTime = tmpTime;
	g_iToday=g_SysInfo.Program_StartYmd;

	//
	strcpy(g_OEM.SysTestCaption,"ϵͳ���");
	strcpy(g_OEM.Sel1Caption,"����+����");
	strcpy(g_OEM.Sel2Caption,"��������");
	strcpy(g_OEM.Sel3Caption,"��������");
	strcpy(g_OEM.CustomQsZxTreeName,"�о���Ѷ");
	strcpy(g_OEM.ResDllName,"res_std.dll");
	strcpy(g_OEM.SpecialCfgFileName,"stdqs");
	strcpy(g_OEM.SSLAqfsName,"SSL");
	strcpy(g_OEM.AuthPassAqfsName,"��֤����");
	strcpy(g_OEM.TdxCAAqfsName,"CA֤��");
	g_OEM.nLogoMaxPassordLen=30;
	g_OEM.nLogoMaxAuthPassLen=30;
	g_OEM.bUrgentUseQSIcon=TRUE;

	strcpy(g_Url.Corp,"������վ");
	strcpy(g_Url.CorpWeb,TDX_WEBPAGE);

	g_WTMsgID = RegisterWindowMessage("Stock");
	g_WTMsgID2 = RegisterWindowMessage("Query_Trade_Stock_Code");
	g_uRefreshAllHQ = RegisterWindowMessage("Tdx_Refresh_AllHQ");
	g_uCloseMsg = RegisterWindowMessage("_CLOSECLIENT");	//ע��رտͻ�����Ϣ

	g_pOverlapStkInfo[0] = g_pOverlapStkInfo[1] = g_pOverlapStkInfo[2] = NULL;
	g_WTVerifyIP = NULL;
	g_WtVerifyIPNum = 0;	
	//�ⲿET�ı�������ֵ
	g_ET.GPSetCode = 0;
	g_ET.GPCode[0] = 0;
	g_ET.Ready=0;
	g_ET.HWnd=NULL;
	memset(&g_ET.HQ,0,sizeof(ExternHQ));
	//�����û���ص��������
	g_UserHostQSID[0] = 0;
	g_MngRight = 0;				//��Ϊ���Ȩ��
	g_UserType = 0;				//��Ϊ��ͨ�ͻ�
	g_strUserType[0] = 0;		//�û����ʹ�
	g_strUserCharacter[0] = 0;	//�û����ʴ�
	//Level2�û�
	g_L2UserName[0]=0;
	g_L2Password[0]=0;
	g_nL2Type=0;
	////////////////////////
	memset(&g_ChatArea,0,sizeof(CHATAREA));
	//////////////////////// ���������һЩ��������ֵ
	memset(&g_GuiMarketSet,0,MAX_MKTGUI_NUM*sizeof(GUIMARKET));
	//��ʱָ�����������
	memset(g_ZSHQData,0,10*sizeof(CurrStockData_Code));
	//���׵���Ŀ���
	memset(g_nMineNum,0,MAXGPNUM*sizeof(short));
	//�г�������Ϣ����
	memset(&g_Mtl,0,sizeof(struct Server_MarketInfo));
	//���̵�OEM���������Ϣ����
	memset(&g_NewJyCfg,0,sizeof(struct NewJy_CFG));
	//
	memset(&g_DWarnInf,0,sizeof(DWARNINF));
	g_DWarnInf.nSZIDPos=-1;
	g_DWarnInf.nSHIDPos=-1;
	//��ʼ�������µ�,ϵͳ�������ĸ���ͼ��
	CBitmap tmpbm,tmpbm2,tmpbm3;
	g_GridImage.Create(16,16,ILC_COLOR24|ILC_MASK,11,1);
	tmpbm3.LoadBitmap(IDB_IMAGELIST_GRID);
	g_GridImage.Add(&tmpbm3,RGB(0,0,0));

	g_LevinImage.Create(12,12,ILC_COLOR24|ILC_MASK,10,1);
	tmpbm.LoadBitmap(IDB_IMAGELIST_LEVIN);
	g_LevinImage.Add(&tmpbm,RGB(0,255,0));	

	g_BtnImage.Create(12,12,ILC_COLOR24|ILC_MASK,3,1);
	tmpbm2.LoadBitmap(IDB_IMAGELIST_HQ);
	g_BtnImage.Add(&tmpbm2,RGB(0,0,0));	

	//��û������һ������
	FirstWindow = 0;
	BigMenuWindow = 0;
	//װ�ع��
	LOAD_CURSOR(m_hCanPressB,IDC_CANPRESSB)
	LOAD_CURSOR(m_hCanPressS,IDC_CANPRESSS)
	LOAD_CURSOR(m_hCanPress,IDC_CANPRESS)
	LOAD_CURSOR(m_hCanDrop,IDC_CANDROP)
	LOAD_CURSOR(m_hCanDrop_Overlap,IDC_CANDROP_OVERLAP)
	LOAD_CURSOR(m_hCannotDrop,IDC_CANNOTDROP)
	LOAD_CURSOR(m_hZoomCur,IDC_DYNAZOOM)
	LOAD_CURSOR(m_hZoomCur2,IDC_DYNAZOOM2)
	LOAD_CURSOR(m_hZoomCur3,IDC_DYNAZOOM3)
	LOAD_CURSOR(m_hVerSplit,IDC_VERSPLIT)
	LOAD_CURSOR(m_hHorSplit,IDC_HORSPLIT)
	LOAD_CURSOR(m_hMove,IDC_MOVE)
	LOAD_CURSOR(m_hCursor2,IDC_CURSOR2)
	LOAD_CURSOR(m_hDraw,IDC_DRAW_CURSOR)
	LOAD_CURSOR(m_hDrawMove,IDC_DRAW_MOVE)
	LOAD_CURSOR(m_hDrawTurn,IDC_DRAW_TURN)
	LOAD_CURSOR(m_hFxtMove,IDC_FXTMOVE)
	LOAD_CURSOR(m_hToBig,IDC_TOBIG_CURSOR)
	LOAD_CURSOR(m_hToSmall,IDC_TOSMALL_CURSOR)
	//װ��ͼ��
	LOAD_ICON(m_hExpUpShow,IDI_EXP_UP)
	LOAD_ICON(m_hExpDownShow,IDI_EXP_DOWN)
	LOAD_ICON(m_hKLineShow,IDI_KLINESHOW)
	LOAD_ICON(m_hExpUpShow_Sel,IDI_EXP_UP_SEL)
	LOAD_ICON(m_hExpDownShow_Sel,IDI_EXP_DOWN_SEL)
	LOAD_ICON(m_hKLineShow_Sel,IDI_KLINESHOW_SEL)
	LOAD_ICON(m_hGBBQShow,IDI_GUBEN)
	LOAD_ICON(m_hGGBWShow,IDI_GGBW)
	LOAD_ICON(m_hOneThunder,IDI_OTHUNDER)
	LOAD_ICON(m_hMoreThunder,IDI_MTHUNDER)
	LOAD_ICON(m_hMyThunder[0],IDI_MYTHUNDER0)
	LOAD_ICON(m_hMyThunder[1],IDI_MYTHUNDER1)
	LOAD_ICON(m_hMyThunder[2],IDI_MYTHUNDER2)
	LOAD_ICON(m_hMyThunder[3],IDI_MYTHUNDER3)
	LOAD_ICON(m_hMyThunder[4],IDI_MYTHUNDER4)
	LOAD_ICON(m_hMyThunder[5],IDI_MYTHUNDER5)
	LOAD_ICON(m_hIcon_Pu[0],IDI_PU)
	LOAD_ICON(m_hIcon_Pu[1],IDI_PU1)
	LOAD_ICON(m_hIcon_Pu[2],IDI_PU2)
	LOAD_ICON(m_hIcon_Pu[3],IDI_PU3)
	LOAD_ICON(m_hIcon_Pu[4],IDI_PU4)
	LOAD_ICON(m_hIcon_Pu[5],IDI_PU5)
	LOAD_ICON(m_hIcon_Pu[6],IDI_PU6)
	LOAD_ICON(m_hIcon_Pu[7],IDI_PU7)
	LOAD_ICON(m_hFCIcon,IDI_FCICON)
	LOAD_ICON(m_hRedUP,IDI_REDUP)
	LOAD_ICON(m_hRedDown,IDI_REDDOWN)
	LOAD_ICON(m_hGreenUP,IDI_GREENUP)
	LOAD_ICON(m_hGreenDown,IDI_GREENDOWN)
	LOAD_ICON(m_hIconUp,IDI_PUP)
	LOAD_ICON(m_hIconDown,IDI_PDOWN)
	LOAD_ICON(m_hIconPBuy[0],IDI_PBUY0)
	LOAD_ICON(m_hIconPBuy[1],IDI_PBUY1)
	LOAD_ICON(m_hIconPBuy[2],IDI_PBUY2)
	LOAD_ICON(m_hIconPSell[0],IDI_PSELL0)
	LOAD_ICON(m_hIconPSell[1],IDI_PSELL1)
	LOAD_ICON(m_hIconPSell[2],IDI_PSELL2)
	LOAD_ICON(m_hZstFlushIcon,IDI_FLUSH)

	return TRUE;
}

char *l_NeedCopyFile_To002[]={g_strUserCfg,"simuser.ini","user_def.ini","syscomm.ini","Scheme.dat","selfpar.dat","selfidx.dat","morehint.htm","stdqs.cfg","celltab.dat",""};

void CTdxWApp::CopySomeFileToT0002()
{
	//һЩ��������,���Զ������Ĳ���
	////////////////////////
	CFile AutoupFile,YYYFile,OtherFile;

	if(AutoupFile.Open(HomePath+"autoup1.exe",CFile::modeRead|CFile::shareDenyNone))
	{
		AutoupFile.Close();
		::CopyFile(HomePath+"autoup1.exe",HomePath+"autoupex.exe",FALSE);
		unlink(HomePath+"autoup1.exe");
	}
	if(YYYFile.Open(HomePath+"yyy.ini", CFile::modeRead|CFile::shareDenyNone))
	{
		YYYFile.Close();
		char tmpWinDir[MAX_PATH];
		GetWindowsDirectory(tmpWinDir,MAX_PATH-1);
		if(::CopyFile(HomePath+"yyy.ini",tmpWinDir+(CString)"\\yyy.ini",FALSE))
			unlink(HomePath+"yyy.ini");
	}
	//
	int i=0;
	CString tmpPath1,tmpPath2;
	while(strlen(l_NeedCopyFile_To002[i]))
	{
		tmpPath1.Format("%s%s",HomePath,l_NeedCopyFile_To002[i]);
		if(OtherFile.Open(tmpPath1,CFile::modeRead|CFile::shareDenyNone))
		{
			OtherFile.Close();
			tmpPath2.Format("%s%s",g_WSXHStr,l_NeedCopyFile_To002[i]);
			if(::CopyFile(tmpPath1,tmpPath2,FALSE))
			{
				//!!!
				unlink(tmpPath1);
			}
		}
		i++;
	}

	long handle;
	int  done;
	struct _finddata_t ffblk;
	char filename[_MAX_PATH],string[_MAX_PATH];
	//--�������е�*.sp��T0002�����pad����
	sprintf(string,"%s*.sp",HomePath);
	handle = _findfirst(string,&ffblk);
	if (handle==-1) done=-1;
	else            done=0;
    while (!done)
	{
		sprintf(filename,"%s%s",HomePath,ffblk.name);
		//�������Ŀ¼���޳�ʼ����,�򿽱���ȥ
		sprintf(string,"%ssheet\\%s",g_WSXHStr,ffblk.name);
		if(OtherFile.Open(string,CFile::modeRead|CFile::shareDenyNone))
			OtherFile.Close();
		else
			::CopyFile(filename,string,FALSE);
		//
		done = _findnext(handle,&ffblk);
	}
	if(handle != -1)
		_findclose(handle);
	//--�������е�*tree*.cfg��T0002����
	sprintf(string,"%s*tree*.cfg",HomePath);
	handle = _findfirst(string,&ffblk);
	if (handle==-1) done=-1;
	else            done=0;
    while (!done)
	{
		sprintf(filename,"%s%s",HomePath,ffblk.name);
		sprintf(string,"%s%s",g_WSXHStr,ffblk.name);
		if(OtherFile.Open(string,CFile::modeRead|CFile::shareDenyNone))
			OtherFile.Close();
		else
			::CopyFile(filename,string,FALSE);
		//
		done = _findnext(handle,&ffblk);
	}
	if(handle != -1)
		_findclose(handle);

	//��������������Ϣ
	Dectect_OverWrite_ProxyInfo();	
}

void CTdxWApp::CopyResDLLFromT0002()
{
	//�ٽ��������õ�����Դ�ļ�������ȥ����
	CFile OtherFile;
	if(OtherFile.Open(g_WSXHStr+g_OEM.ResDllName,CFile::modeRead|CFile::shareDenyNone))
	{
		OtherFile.Close();
		::CopyFile(g_WSXHStr+g_OEM.ResDllName,HomePath+g_OEM.ResDllName,FALSE);
		unlink(g_WSXHStr+g_OEM.ResDllName);
	}
}

BOOL CTdxWApp::InitInstance()
{
#ifndef _DEBUG
	if(stricmp(m_lpCmdLine,"debug"))
	{
		if(IsDebuggerPresent())	//������
			return FALSE;
	}
#endif

	//���ؽ��淽��
	GetScheme_King();
	//�õ�ϵͳ��ǰĿ¼
	char path[256];
	GetCurDir(path,255);
	HomePath = path;
	if(HomePath.Right(1) != "\\")
		HomePath += "\\";
	/*
	BOOL bOnlyOne=GetPrivateProfileInt("Other","OnlyOne",0,g_WSXHStr+g_strYxhjCfg);
	CString tmpMutexStr=HomePath+"Tdxw Mutex";
	tmpMutexStr.Replace("\\"," ");
	m_hMutex = CreateMutex(NULL,FALSE,tmpMutexStr);
	//���ظ�����
	if(strcmp(m_lpCmdLine,"nodetect") && (m_hMutex==NULL || GetLastError() == ERROR_ALREADY_EXISTS))
	{
		if(bOnlyOne)
		{
			TDX_MessageBox(NULL,"һ��Ŀ¼��ֻ������һ����ϵͳ!",MB_OK|MB_ICONERROR);
			bExistInstance = TRUE;
			return FALSE;
		}
		else if(TDX_MessageBox(NULL,"���Ѿ���ͬһĿ¼����������ͬ���͵ı�ϵͳ,�������п��ܻ���ɳ�ͻ.\n�Ƿ����?",MB_ICONEXCLAMATION|MB_OKCANCEL)==IDCANCEL)
		{
			bExistInstance = TRUE;
			return FALSE;
		}
	}
	*/
	/*
	g_pSplashWnd = new CSplashWnd();		//����һ������ʵ��
	g_pSplashWnd ->CenterWindow();			//������ʾ����Ļ����
	g_pSplashWnd ->ShowWindow(m_nCmdShow);	//��ʾ����
	g_pSplashWnd ->UpdateWindow();
	*/

	//!!!
	/*
	char strPath[MAX_PATH]={0};
	if(!SHGetSpecialFolderPath(NULL, strPath, CSIDL_APPDATA, TRUE))
		strcpy(strPath, HomePath);
	g_WSXHStr.Format("%s\\%s\\",strPath,g_Scheme_King.YxhjPath);
	CreateDirectory(g_WSXHStr,NULL);
	*/
	g_WSXHStr = HomePath;

	//AfxOleInit����Ҫ��,��Ȼ��������ʾ��ҳ����ֲ��ܿ����ȹ�����
	AfxOleInit();
	//��ʼ��RichEdit�ؼ�
	AfxInitRichEdit();
	AfxEnableControlContainer();
	#ifdef _AFXDLL
		Enable3dControls();
	#else
		Enable3dControlsStatic();
	#endif
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings();	
	//
	BOOL bDirectConnect = FALSE;

	if(!SetInitValue())
		return FALSE;
	//�����Ļ�ֱ���
	ScreenCx = GetSystemMetrics(SM_CXSCREEN);
	ScreenCy = GetSystemMetrics(SM_CYSCREEN);
	if(ScreenCx < 800 || ScreenCy < 600)
		MessageBox(NULL,"��ʾ�����齫��ʾ������Ļ��������Ϊ800x600��������(�Ƽ�Ϊ800X600,16λ������ǿɫ)",NULL,MB_OK|MB_ICONEXCLAMATION);
	
	CreateDirectory(HomePath+g_strBlockNew,NULL);
	CreateDirectory(g_WSXHStr+"sheet",NULL);
	CopySomeFileToT0002();
	///////
	//��INI�ж�������,һ��Ҫ����GetScheme
	if(!GetFromINI())
	{
		PostQuitMessage(0);
		return FALSE;
	}
#if defined(OEM_STAR) || defined(OEM_STAR_HM) 
	g_bHasCaption=1;
#elif defined(OEM_NEWJY) || defined(LEVEL2) || defined(PERSONAL)	
	g_bHasCaption=0;
#endif
#if defined(OEM_STAR_HM) 
	g_strNeedAdName="needad_hm.dat";	
#endif
	memset(g_HisTickCacheHdr, 0, MAX_HISCACHE*sizeof(HisTickCacheHead));
	g_nNextHTCHPos = 0;
	//ɾ���������
	unlink(g_WSXHStr+"tmp\\TxtFind.dat");
	unlink(g_WSXHStr+"tmp\\NewsFind.dat");
	//�������ļ��ж���������Ϣ
	GetFromOther();
	//������ɫ����
	GetScheme();
	SystemColor = Scheme[CurrentScheme];
	GridColor = SystemColor.GridColor;
	VipColor = SystemColor.VipColor;
	TreeColor = SystemColor.TreeColor;
	TxtColor = SystemColor.TxtColor;
	MenuColor.MenuBackColor = VipColor.BACKCOLOR;
	MenuColor.MenuTitleColor = VipColor.KNCOLOR;
	MenuColor.MenuColor1 = VipColor.BIGTITLECOLOR;
	MenuColor.MenuColor2 = VipColor.LEVELCOLOR;
	MenuColor.MenuColor3 = VipColor.ZBCOLOR[2];
	MenuColor.MenuColor4 = VipColor.ZBCOLOR[3];
	//װ���������ļ�
	LoadBlockCfg();
	LoadBlockColorCfg(g_BlockColor);
	//��ʼ����ϵͳGDI��Դ
	g_d.CreateGdiObject();
	g_DrawLineTool.LoadLine();
	LoadOverlap();

	CreateDirectory(g_WSXHStr+"LData",NULL);
	CreateDirectory(g_WSXHStr+"LData\\sz",NULL);
	CreateDirectory(g_WSXHStr+"LData\\sh",NULL);
	CreateDirectory(g_WSXHStr+"LData\\sz\\day",NULL);
	CreateDirectory(g_WSXHStr+"LData\\sh\\day",NULL);
	CreateDirectory(g_WSXHStr+"LData\\cache",NULL);
	g_pComm	= new CHQComm;
	g_pComm3 = new CHQComm(1);
	g_pGetData	= new CGetData;
	if(!g_pComm||!g_pComm3||!g_pGetData)
	{
		PostQuitMessage(0);
		return FALSE;
	}
	if(!g_pGetData->Create())
	{
		PostQuitMessage(0);
		return FALSE;
	}
//	GetPrivateProfileString("UserHost","QSID","",g_QSID,10,g_WSXHStr+g_strYxhjCfg);
	//����п�����ҳ������ʾ֮
	if(TestFileExist(HomePath+"pages\\kc.htm"))
	{
		CKCLogoDlg KCLogo;
		KCLogo.m_Url = "##homepath##pages\\kc.htm";
		KCLogo.DoModal();
	}
#ifdef LEVEL2
	strcpy(g_Scheme_King.QSID,"tdxlevel2");
	g_GC.bHasTdxAdv = TRUE;
#endif
#ifdef OEM_NEWJY //��ʼ��Ƕ��ʽ����
	DeleteMatchFile(g_WSXHStr+"wt_cache\\","*zzc.html");
	GetNewJyCfg(g_OEM.SpecialCfgFileName);	
	if(!InitTC(g_Scheme_King.QSID,HomePath,g_WSXHStr,g_WSXHStr+g_strYxhjCfg))
	{
		TDX_MessageBox(NULL,"����ģ��װ��ʧ��!�������л����Ƿ�����.",MB_OK|MB_ICONERROR);
		PostQuitMessage(0);
		return FALSE;
	}
	bDirectConnect=FALSE;
#elif defined(PERSONAL) || defined(LEVEL2)
	bDirectConnect=FALSE;
#endif

	//�ݴ�,LogoDlg��ContinueModal����
	try
	{	
		SkinStart("newskin.urf",WINDOW_TYPE_VC,"",GTP_LOAD_FILE,NULL,NULL);
		
		g_pConnectWnd = new CLogoDlg;
		g_pMainWin = g_pConnectWnd;
		((CLogoDlg *)g_pConnectWnd)->m_bDirectConnect = bDirectConnect;
		if(((CDialog *)g_pConnectWnd)->DoModal() == IDCANCEL)
		{
			PostQuitMessage(0);
			TDEL(g_pConnectWnd);
			return FALSE;
		}
		g_pMainWin = NULL;
		TDEL(g_pConnectWnd);
	}
	catch(...)
	{
		TDX_MessageBox(NULL,"δ֪��������������!",MB_OK|MB_ICONEXCLAMATION);
		PostQuitMessage(0);
		TDEL(g_pConnectWnd);
		return FALSE;
	}

	/*
	g_pSplashWnd->SendMessage(WM_CLOSE, 0, 0);
	TDEL(g_pSplashWnd);
	*/

	/*
	for (int i = 0; i < 2; i++)
	{   
		head[i].reserve(MAX_STOCK_NUM);
		head[i].clear();
		for(int k=0;k<MAX_STOCK_NUM;k++)
		{
			MemStkInfo iInfo={0};
			head[i].push_back(iInfo);
		}
	}
	*/

	////���ڸ��˰棬�����ṩ�������
#if defined(OEM_STAR) || defined(OEM_STAR_HM) || defined(PERSONAL)
	unlink(g_WSXHStr+"LData\\cache\\neednote.dat");
#endif
	OnLine = FALSE;
	if(!OffLineMode)
		OnLine = TRUE;	//ConnectWnd�رպ�,���ܽ�OnLine��ֵ
	
	//����ȫ��ʵ��
	g_pStockIO = (StockDataIo * ) new StockDataIo;
	if (!g_pStockIO->TcpipInit(OffLineMode))
	{
		PostQuitMessage(0);
		return FALSE;
	}
	if(DSOnLine)
		g_pStockIO->AfterDSLoginSuccess();

	//���س���
	g_pConstMng = new ConstMng;
	g_pConstMng->LoadInConst();
	//��չ����
	g_pExdayMng = new ExdayMng;
	//���ش���Ʒ��,��g_pStockIO->TcpipInit֮ǰload,�Ա��ڼ��뵽������
	g_pLcIdxMng	= new LcIdxMng;
	g_pLcIdxMng->LoadAll(TRUE);

	if(g_pStockIO)
	{
		g_pStockIO->ReAddLc(SZ);
		g_pStockIO->ReAddLc(SH);
	}

	InitCalcInterface(g_pCalc);		//��ʼ����ʽ��������ģ��

	g_CF.GetFormulaPack();		//��ȡ����ͼ��ʽ���ģ��
	g_CF.GetFormulaBandStock();	//��ȡ����ͼ��ʽ���ģ��͹�Ʊ�İ�

	g_GNPlug.LoadGNPlugin();

	LoadCusGridTab(TRUE);	//ȡ��ǩ����
	GetFYInfo();			//ȡ���Ƶ�������
	GetNeedNoteInfo();		//ȡͨ���Ź������
	GetNeedAdInfo();		//ȡ�����������
	GetDynaCusPadInfo();	//�õ����ư�������
				
	//��������Ԥ����Ϣ
	struct MemStkInfo *tmpInfo;
	for(int i=0;i<g_pAbsConditionNum;i++)
	{
		tmpInfo = g_pStockIO->Get_MemStkInfo(g_pAbsCondition[i].code,g_pAbsCondition[i].setcode);
		if(!tmpInfo)
		{ 
			memcpy(g_pAbsCondition+i,g_pAbsCondition+i+1,sizeof(PWARNINGCND)*(MAX_ABSWARNMAXNUM-i-1));
			g_pAbsConditionNum--;
		}
	}
	//�õ���ɫ���Ĺ�Ʊ
	LoadColorfulBlockGP();

	//�õ������ҳ��Ϣ����ʼ������ҳ�˵�
	SetWebMenuValue();
	//ע���ĵ�ģ��
	pBigMenuTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_BIGMENU, //���ֲ˵�
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CBigMenuView));
	AddDocTemplate(pBigMenuTemplate);

	//ע���ĵ�ģ��
	pHomePageTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_HOMEPAGE, //ϵͳ��ҳ
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CHomePageView));
	AddDocTemplate(pHomePageTemplate);

	//ע���ĵ�ģ��
	pRssTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_RSS, //Rss
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CRssView));
	AddDocTemplate(pRssTemplate);
	
	pGridTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_GRID, //����ͼ
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CGridView));
	AddDocTemplate(pGridTemplate);

	pNewsTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_NEWS,//��������
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CNewsView));
	AddDocTemplate(pNewsTemplate);

	pTxtTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_TXT,//��������
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CTxtView));
	AddDocTemplate(pTxtTemplate);

	pComTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_COM,//ComView����
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CComView));
	AddDocTemplate(pComTemplate);

	pCom2Template = new CNewMultiDocTemplate(
		IDR_TYPE_COM2,//ComView2����
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CComView2));
	AddDocTemplate(pCom2Template);

	pWebInfoTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_WEB,//ȯ�̷�����Ѷ
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CIEBrowserView));
	AddDocTemplate(pWebInfoTemplate);

	pJbfxTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_JBFX,//��������
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CJbfxView));
	AddDocTemplate(pJbfxTemplate);

	pAdvHqTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_ADVHQ,//�߼����鹦��
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CAdvHqView));
	AddDocTemplate(pAdvHqTemplate);

	pJjTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_JJ,//������
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CJJView));
	AddDocTemplate(pJjTemplate);

	pTvTemplate = new CNewMultiDocTemplate(
		IDR_TYPE_TV,//TopView����
		RUNTIME_CLASS(CComDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CTvView));
	AddDocTemplate(pTvTemplate);

	//��������ܴ���	
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	m_pMainWnd = pMainFrame;
	
#if !defined(OEM_NEWJY)
	HMENU tmpMenu = pMainFrame->NewMenu(IDR_MAINFRAME);
	pGridTemplate->m_hMenuShared=tmpMenu;
//	pGridTemplate->m_NewMenuShared.LoadMenu(tmpMenu);
//	pGridTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME);
	pNewsTemplate->m_hMenuShared=tmpMenu;
	pTxtTemplate->m_hMenuShared=tmpMenu;
	pComTemplate->m_hMenuShared=tmpMenu;
	pCom2Template->m_hMenuShared=tmpMenu;
	pWebInfoTemplate->m_hMenuShared=tmpMenu;
	pBigMenuTemplate->m_hMenuShared=tmpMenu;
	pJbfxTemplate->m_hMenuShared=tmpMenu;
	pAdvHqTemplate->m_hMenuShared=tmpMenu;
	pMainFrame->m_hMenuDefault=tmpMenu;
	pMainFrame->OnUpdateFrameMenu(pMainFrame->m_hMenuDefault);
#endif
	
	//����ܴ������ߴ紦��
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if(g_nEmbedTabSel!=LOGIN_ONLYWT)
		m_nCmdShow=SW_SHOWMAXIMIZED;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();	

	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	//ȱʡ������ͼ
	if(g_bHasHomePage)
	{
		OpenNewView("HomePage");
		g_bFirstCusPad=FALSE;
	}
	else if(GetCusPadTypeParam(g_DefCusPadName)>=0) 
	{
		OpenNewView("Com2");
		g_bFirstCusPad=TRUE;
	}
	else 
	{
		OpenNewView("Grid");
		g_bFirstCusPad=FALSE;
	}
	//ʹ��һ��View���ܹر�
	CNewMenu* pSysMenu = (CNewMenu *)(((CFrameWnd *)m_pMainWnd)->GetActiveFrame())->GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
		pSysMenu->EnableMenuItem (SC_CLOSE,MF_GRAYED);
	//��һ�����ڵľ��
	FirstWindow = (((CFrameWnd *)m_pMainWnd)->GetActiveFrame())->m_hWnd;

	/*
	//����Ǵ�ͳ��ģʽ���������ֲ˵�
	if(g_bHasCaption && !g_bFirstCusPad)
	{
		//�򿪴��ֲ˵�ͼ
		OpenNewView("BigMenu");
		//ʹ���ֲ˵����ܹر�
		CNewMenu* pSysMenu = (CNewMenu *)(((CFrameWnd *)m_pMainWnd)->GetActiveFrame())->GetSystemMenu(FALSE);
		if (pSysMenu != NULL)
			pSysMenu->EnableMenuItem (SC_CLOSE,MF_GRAYED);
		BigMenuWindow = (((CFrameWnd *)m_pMainWnd)->GetActiveFrame())->m_hWnd;
	}
	*/

	if(m_pMainWnd)
		m_pMainWnd->SetActiveWindow();

	//���ò˵���ʾ���
//	CNewMenu::SetMenuDrawMode((g_nMenuMode==0)?CNewMenu::STYLE_ORIGINAL_NOBORDER:
//		(g_bExtendXPStyle?CNewMenu::STYLE_XP_NOBORDER:CNewMenu::STYLE_XP));

	
	//��¼�Լ���վ��
	if(!g_pHqSrc)
		g_pHqSrc = new CHqSrcMgn;
	char strMsg[128]={0};
	int nDefThreadNum = GetPrivateProfileInt("POOL", "THREADNUM", 1, g_WSXHStr+"PoolInfo_Bit.ini");
	nDefThreadNum = 1;
	for(i=0;i<nDefThreadNum;i++)
		g_pHqSrc->RegisterOneHqSource(strMsg, 0x0200);
	g_pHqSrc->StartHqThread();
	
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
#include "Label.h"
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	~CAboutDlg();
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_SoftWareNameStatic;
	CStatic	m_WarnStatic;
	CLabel	m_CompanyNameStatic;
	//}}AFX_DATA
	CDialog *m_pDlg;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAboutCredits();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_pDlg = NULL;
}

CAboutDlg::~CAboutDlg()
{
	TDEL(m_pDlg);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_SOFTWARENAME_STATIC, m_SoftWareNameStatic);
	DDX_Control(pDX, IDC_WARN_STATIC, m_WarnStatic);
	DDX_Control(pDX, IDC_COMPANYNAME_STATIC, m_CompanyNameStatic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ABOUT_CREDITS, OnAboutCredits)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTdxWApp::OnAppAbout()
{
	if(strlen(g_Url.AboutURL)>0)
	{
		ShellExecute(NULL, _T("open"), g_Url.AboutURL, NULL, NULL, SW_SHOW);	
		return;
	}
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CTdxWApp::PreTranslateMessage(MSG* pMsg)
{
	return CWinApp::PreTranslateMessage(pMsg) ||
		(m_pXiaDanDllInfo && m_pXiaDanDllInfo->m_pfnFilterDllMsg && (m_pXiaDanDllInfo->m_pfnFilterDllMsg)(pMsg));
}

BOOL CTdxWApp::OnIdle(LONG lCount) 
{
	if (CWinApp::OnIdle(lCount))
		return TRUE;
	if (m_pXiaDanDllInfo && m_pXiaDanDllInfo->m_pfnProcessDllIdle)
	{
		(m_pXiaDanDllInfo->m_pfnProcessDllIdle)();
	}
	return FALSE; 
}

int CTdxWApp::ExitInstance() 
{
	int nRet = CWinApp::ExitInstance();
	if(bExistInstance)
		return nRet;

	if(g_pHqSrc) g_pHqSrc->KillAllHqThread();
	TDEL(g_pHqSrc)

	if(m_hMutex)
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
	}

	//clean
	DeleteDirInfo(g_WSXHStr+"diary");	RemoveDirectory(g_WSXHStr+"diary");
	DeleteDirInfo(g_WSXHStr+"info_cache");	RemoveDirectory(g_WSXHStr+"info_cache");
	DeleteDirInfo(g_WSXHStr+"tmp");	RemoveDirectory(g_WSXHStr+"tmp");
	unlink(g_WSXHStr+"user.ini");
	//
	g_DrawLineTool.CleanZstLine();
	g_DrawLineTool.SaveLine();
	g_d.ReleaseGdiObject();

	SaveOverlap();

	//ɾ��ȫ��ʵ��
	TDEL(g_pStockIO);
	TDEL(g_pLcIdxMng);
	TDEL(g_pConstMng);
	TDEL(g_pExdayMng);
	TDEL(g_ZHBBlock);
	TDEL(g_pCalc);
	TDEL(g_pGetData);
	TDEL(g_pComm);
	TDEL(g_pComm3);
	
	TDEL(g_WTVerifyIP);
	TDEL(l_pYYBInfo);
	TDEL(g_aAllItemWidth);
	TDEL(g_pAbsCondition);
	TDEL(g_pAdList);

	TDELA(g_DWarnInf.pWarnAtom);

	TDELA(g_pFundHoldStock);
	TDELA(g_pFundBase);

	for(int i=0;i<g_aUrgentInfo.size();i++)
		TDELA(g_aUrgentInfo[i].Info);

	if(g_resModule)
		AfxFreeLibrary(g_resModule);

#ifdef OEM_NEWJY
	try
	{
		UninitTC();
	}
	catch(...)
	{
	}
#endif
	UnLoadOtherDLL();
	UnLoadQSRunDLL();
	ExitHexinAndTdxJy();
	ExitLevinCJ();

	WSACleanup();			//���������������Ӻ�

	if(g_bReLoginStatus)
	{
		char szFilename[256];
		GetModuleFileName(NULL,szFilename,sizeof(szFilename));
		ShellExecute(NULL,NULL,szFilename,NULL,NULL,SW_SHOWNORMAL);		
	}

	return nRet;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_CompanyNameStatic.SetTextColor(RGB(0,0,255));
	m_CompanyNameStatic.SetWindowText(_F(g_strPreName));
	SetTimer(0x20,1800*1000,NULL);

	CString tmpStr,tmpStr2;
	CString NameStr = g_strTitleName;
	if(g_GC.nShareMode==1)
		NameStr += "(���̰�-��������Ŀ¼)";
	else if(g_GC.nShareMode==2)
		NameStr += "(���̰�-��������Ŀ¼)";
	else if(g_GC.bLocalData)
		NameStr += "(�������ݰ�)";
	if(g_bShowVersoin)
	{
		tmpStr.Format("V%s ",g_szReleaseVersion);
		NameStr += tmpStr;
	}
	m_SoftWareNameStatic.SetWindowText(_F(NameStr));
	//
	tmpStr2="";
#ifdef ZSZQJZ
	tmpStr2="����";
#endif
	tmpStr.Format("%s ����ģ��:%s Build:%s",tmpStr2,g_szMainExeVersion,MakeBuildNo(__DATE__,__TIME__));
	if(g_OEM.bEmbedWTVersion && TC_GetVersion_)
	{
		float fVersion=0;
		char strBuild[255]={0};
		TC_GetVersion_(fVersion,strBuild);
		tmpStr2.Format(" ���� Build:%s",strBuild);
		tmpStr+=tmpStr2;
	}
	if(g_OEM.bHasQSRun)
	{
		char	CustomName[255]={0};
		float	fVersion=0;
		char	strBuild[255]={0};
		QS_GetCustomInfo_(CustomName,fVersion,strBuild);
		tmpStr2.Format("\r\n\r\n%s %.2f Build:%s",CustomName,fVersion,strBuild);
		tmpStr+=tmpStr2;
	}

	SetDlgItemText(IDC_SERIALSTATIC,_F(tmpStr));
	if(!g_bRiskPost)
		GetDlgItem(IDC_WARN_STATIC)->ShowWindow(SW_HIDE);
	else
	{
		CFile WarnFile;
		char *pBuf = NULL;
		if(WarnFile.Open(HomePath+"risk.txt",CFile::modeRead|CFile::shareDenyNone))
		{
			long filelen = WarnFile.GetLength();
			pBuf = new char[filelen+1];
			WarnFile.Read(pBuf,filelen);
			pBuf[filelen] = 0;
			_FL(pBuf);
			GetDlgItem(IDC_WARN_STATIC)->SetWindowText(pBuf);
			delete pBuf;
			WarnFile.Close();
		}
	}
	//�õ�ϵͳ��Ϣ(�ڴ桢����Ӳ�̴�С)
	{
		CString strFreeDiskSpace;
		CString strFreeMemory;
		CString strFmt;

		//�õ������ڴ�
		MEMORYSTATUS MemStat;
		MemStat.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&MemStat);
		strFmt.LoadString(CG_IDS_PHYSICAL_MEM);
		strFreeMemory.Format(strFmt, MemStat.dwTotalPhys / 1024L /1024L);

		//�õ�������Ϣ
		struct _diskfree_t diskfree;
		int nDrive = _getdrive(); 
		if (_getdiskfree(nDrive, &diskfree) == 0)
		{
			float dwTmp = 1.0 * diskfree.avail_clusters * diskfree.sectors_per_cluster * diskfree.bytes_per_sector;
			dwTmp = dwTmp/ 1024L /1024L;
			strFmt.LoadString(CG_IDS_DISK_SPACE);
			strFreeDiskSpace.Format(strFmt,nDrive-1 + _T('A'),(DWORD)dwTmp);
		}
		else
			strFreeDiskSpace.LoadString(CG_IDS_DISK_SPACE_UNAVAIL);

		 SetDlgItemText(IDC_DISK_SPACE, strFreeDiskSpace);
		 SetDlgItemText(IDC_PHYSICAL_MEM, strFreeMemory);
	}

	SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

	return TRUE;
}

void CAboutDlg::OnPaint() 
{
	CDialog::OnPaint();
	return;
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0x20)
	{
		KillTimer(0x20);
		OnAboutCredits();
	}

	CDialog::OnTimer(nIDEvent);
}

void CAboutDlg::OnAboutCredits() 
{
	if(m_pDlg) 
	{
		TDEL(m_pDlg);
		GetDlgItem(IDC_ABOUT_CREDITS)->SetWindowText(_F("Э���Ŷ�"));
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_COMPANYNAME_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SERIALSTATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_WARN_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SOFTWARENAME_STATIC)->ShowWindow(SW_SHOW);		
	}
	else
	{
		m_pDlg = new CCredits();
		((CCredits*)m_pDlg)->Create(this);
		m_pDlg->SetWindowPos(NULL,8,8,0,0,SWP_NOSIZE);
		m_pDlg->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ABOUT_CREDITS)->SetWindowText(_F("��Ȩ��Ϣ"));
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMPANYNAME_STATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SERIALSTATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_WARN_STATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SOFTWARENAME_STATIC)->ShowWindow(SW_HIDE);		
	}
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (pWnd->GetDlgCtrlID() == IDC_WARN_STATIC)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
