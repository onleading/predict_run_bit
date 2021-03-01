#include "StdAfx.h"
#include "Ad.h"

ADHEAD		g_AdHead;
ADINFOSET*	g_pAdList = NULL;

void GetNeedNoteInfo()
{
	CString	Ini;
	Ini.Format("%sLData\\cache\\neednote.dat",g_WSXHStr);
	GetPrivateProfileString("F10","ADSTR1","",g_AdvertInfo.F10AdvertStr1,255,Ini);
	GetPrivateProfileString("F10","ADSTR2","",g_AdvertInfo.F10AdvertStr2,255,Ini);
	
	CString tempStr;
	g_AdvertInfo.nAdvBt=0;
	CFont NewFont;
	NewFont.CreateFont(15,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
	CDC dc;
	dc.CreateDC("DISPLAY",NULL,NULL,NULL);
	CFont *OldFont = dc.SelectObject(&NewFont);
	char txtBuf[9];
	int btNum=GetPrivateProfileInt("VITAL","ADVERT_BUTTON_NUM",0,Ini);
	for(int i=0;i < btNum; i++)
	{
		tempStr.Format("ADVERT_BUTTON_TXT%02d",i+1);
		GetPrivateProfileString("VITAL",tempStr,"",g_AdvertInfo.AdvButton[i].btTxt,20,Ini);
		tempStr.Format("ADVERT_BUTTON_URL%02d",i+1);
		GetPrivateProfileString("VITAL",tempStr,"",g_AdvertInfo.AdvButton[i].url,198,Ini);
		tempStr=g_AdvertInfo.AdvButton[i].url;
		if(tempStr.Find("http")==-1)
		{
			tempStr.Format("file:///%sLData\\cache\\%s",g_WSXHStr,g_AdvertInfo.AdvButton[i].url);
			tempStr.Replace('\\','/');
			strcpy(g_AdvertInfo.AdvButton[i].url,tempStr);
		}

		CSize cs=dc.GetTextExtent(g_AdvertInfo.AdvButton[i].btTxt,strlen(g_AdvertInfo.AdvButton[i].btTxt));
		g_AdvertInfo.AdvButton[i].rect.left=0;
		g_AdvertInfo.AdvButton[i].rect.right=cs.cx+6;
		
		tempStr.Format("ADVERT_BUTTON_TXTCOR%02d",i+1);
		memset(txtBuf,0,sizeof(txtBuf));
		GetPrivateProfileString("VITAL",tempStr,"0x0000ff",txtBuf,sizeof(txtBuf),Ini);
		sscanf(txtBuf,"%x",&g_AdvertInfo.AdvButton[i].txtCor);

		tempStr.Format("ADVERT_BUTTON_ADVDLG_CX%02d",i+1);
		g_AdvertInfo.AdvButton[i].advdlg_cx=GetPrivateProfileInt("VITAL",tempStr,500,Ini);
		tempStr.Format("ADVERT_BUTTON_ADVDLG_CY%02d",i+1);
		g_AdvertInfo.AdvButton[i].advdlg_cy=GetPrivateProfileInt("VITAL",tempStr,200,Ini);

		g_AdvertInfo.nAdvBt++;
	}
	dc.SelectObject(OldFont);
	dc.DeleteDC();
}

void FromStringToCOLORREF(CString strref,COLORREF& rgb)
{
	BYTE byRed = 0,byGreen = 0,byBlue = 0;
	int nFind;
	CString strtmp = strref;
	strtmp.TrimLeft("RGB");
	strtmp.TrimLeft("(");
	strtmp.TrimRight(")");
	nFind = strtmp.Find(",");
	if(nFind <= 0)
		return;
	CString strLeft;
	strLeft = strtmp.Left(nFind);
	byRed = (BYTE)atoi(strLeft);
	strtmp = strtmp.Mid(nFind+1);
	nFind = strtmp.Find(",");
	if (nFind <= 0)
		return;
	strLeft = strtmp.Left(nFind);
	byGreen = (BYTE)atoi(strLeft);
	strtmp = strtmp.Mid(nFind+1);
	byBlue = (BYTE)atoi(strtmp);
	rgb = RGB(byRed,byGreen,byBlue);
}

void GetNeedAdInfo()
{
	CString	Ini;
	Ini.Format("%sLData\\cache\\%s",g_WSXHStr,g_strNeedAdName);
	
	//读广告总体信息
	g_AdHead.m_nADNum = GetPrivateProfileInt("AdHead","AdNum",0,Ini);
	g_AdHead.m_nADNum = max(g_AdHead.m_nADNum,0);
	g_AdHead.m_nADNum = min(g_AdHead.m_nADNum,AD_MAXNUM);
	//
	g_AdHead.m_nADMinRandPopTime = GetPrivateProfileInt("AdHead","AdMinRandPopTime",10,Ini);
	if(g_AdHead.m_nADMinRandPopTime<1) 
		g_AdHead.m_nADMinRandPopTime=1;
	//
	g_AdHead.m_nADMaxRandPopTime = GetPrivateProfileInt("AdHead","AdMaxRandPopTime",60,Ini);
	if(g_AdHead.m_nADMaxRandPopTime<g_AdHead.m_nADMinRandPopTime) 
		g_AdHead.m_nADMaxRandPopTime=g_AdHead.m_nADMinRandPopTime;
	//
	g_AdHead.m_nADGUID = GetPrivateProfileInt("AdHead","GUID",0,Ini);

	TDEL(g_pAdList);
	g_pAdList = new ADINFOSET[g_AdHead.m_nADNum+1];
	//读每条广告具体信息
	ADINFOSET ADSet;
	CString strSection;
	CString strValue;
	int nValidAdNum=0;
	for(int i=0;i<g_AdHead.m_nADNum;i++)
	{
		strSection.Format("Ad%d",i+1);
		//广告名
		GetPrivateProfileString(strSection,"AdName","",ADSet.m_szADName,AD_MAXNAME,Ini);
		ADSet.m_szADName[AD_MAXNAME-1] = 0;
		//广告内容
		GetPrivateProfileString(strSection,"AdContent","",ADSet.m_szADCont,AD_MAXCONTENT,Ini);
		ADSet.m_szADCont[AD_MAXCONTENT-1] = 0;
		//
		strValue=ADSet.m_szADCont;
		strValue.Replace("<br>","\r\n");
		strncpy(ADSet.m_szADCont,strValue,AD_MAXCONTENT-1);
		ADSet.m_szADCont[AD_MAXCONTENT-1] = 0;
		//广告URL
		GetPrivateProfileString(strSection,"AdUrl","",ADSet.m_szADUrl,AD_MAXURL,Ini);
		ADSet.m_szADUrl[AD_MAXURL-1] = 0;
		//广告背景色1
		GetPrivateProfileString(strSection,"AdBackRGB1","RGB(220,234,245)",strValue.GetBuffer(20),20,Ini);
		strValue.ReleaseBuffer();
		FromStringToCOLORREF(strValue,ADSet.m_rgbBack1);
		//广告背景色2
		GetPrivateProfileString(strSection,"AdBackRGB2","RGB(194,218,220)",strValue.GetBuffer(20),20,Ini);
		strValue.ReleaseBuffer();
		FromStringToCOLORREF(strValue,ADSet.m_rgbBack2);
		//内容背景色
		GetPrivateProfileString(strSection,"AdTxtBkRGB","RGB(255,255,255)",strValue.GetBuffer(20),20,Ini);
		strValue.ReleaseBuffer();
		FromStringToCOLORREF(strValue,ADSet.m_rgbTxtBk);
		//内容前景色
		GetPrivateProfileString(strSection,"AdFontRGB","RGB(0,0,0)",strValue.GetBuffer(20),20,Ini);
		strValue.ReleaseBuffer();
		FromStringToCOLORREF(strValue,ADSet.m_rgbFront);
		//截至日期
		ADSet.m_lExpireDate = GetPrivateProfileInt(strSection,"AdLastTime",20080101,Ini);		
		//广告频率
		ADSet.m_nStaySeconds = GetPrivateProfileInt(strSection,"AdStaySeconds",20,Ini);
		if(ADSet.m_nStaySeconds<1)
			ADSet.m_nStaySeconds=1;
		//广告窗口高度
		ADSet.m_nHeight = GetPrivateProfileInt(strSection,"AdWinHeight",220,Ini);
		//广告窗口宽度
		ADSet.m_nWidth = GetPrivateProfileInt(strSection,"AdWinWidth",320,Ini);
		//广告窗口风格（暂不支持）
		ADSet.m_nStyle = GetPrivateProfileInt(strSection,"AdWinStyle",0,Ini);
		if(ADSet.m_lExpireDate>=g_iToday)
		{
			g_pAdList[nValidAdNum]=ADSet;
			nValidAdNum++;
		}
	}	
	g_AdHead.m_nADNum=nValidAdNum;
}

