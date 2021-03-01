#include "StdAfx.h"
#include "jy.h"

int			g_nEmbedTabSel=LOGIN_HQWT;
BOOL		g_bSaveEmbedTabSel=FALSE;
int			g_nLastZhType=0;
int			g_nLastAqfs=0;
int			g_nLastLoginType=REGISTER_LOGIN;
char		g_strLastZhInfo[31]={0};
char		g_strSaveZhInfo[MAX_SAVE_ZHNUM][31];
int			g_nSaveZhNum = 0;
int			g_nLastYYBSel=0;
char		g_strLastYYBID[6]={0};

char		wtbt[MAX_WTTYPE][15]={
	"����ɶ�",
	"�����ɶ�",
	"��¹ɶ�",
	"���¹ɶ�",
	"���ڹ�ծ�ڻ�",
	"�Ϻ���ծ�ڻ�",
	"�ط���̨",
	"��Ʒ�ڻ�",	
	"�ʽ��ʺ�",
	"�ͻ���",	
	"���һ",
	"��ҵ֤ȯί��",
	"�������",
	"������Ԫ", 
	"����۱�",	
	"����ʽ����",
	"���ڻع��ʺ�",
	"�Ϻ��ع��ʺ�",
	"�����ʺ�",
	"�����ʺ�",	
	"�ڻ��ʺ�",
	"֣���ڻ��ʺ�",
	"�Ϻ��ڻ��ʺ�",
	"�����ڻ��ʺ�",
	"�ͻ�����",
	"�����ʺ�",
	"",
	"",
	"�����ʺ�"
};

struct	wtdef_new_info wtdef;

void GetWTDef()
{
	int i;
	CString	Ini = HomePath+"embwt.cfg";
	char tmpstr[80]={0};
	char tmpIniText[80]={0};
	////////////////////////////////////////////////////////////
	wtdef.bShowSSL=GetPrivateProfileInt("SECURE","ShowSSL",1,Ini);
	wtdef.bAuthPass=GetPrivateProfileInt("SECURE","AuthPass",0,Ini);
	wtdef.bTDXCA=GetPrivateProfileInt("SECURE","TDXCA",0,Ini);
	char szItemString[256]={0},szItemData[32]={0};
	wtdef.yybnum=GetPrivateProfileInt("YYB","YYBNum",0,Ini);
	wtdef.yybnum=min(wtdef.yybnum,300);
	wtdef.yybnum=max(0,wtdef.yybnum);
	for(i=0;i<wtdef.yybnum;i++)
	{
		sprintf(tmpstr,"YYB_TDXID%d",i+1);
		GetPrivateProfileString("YYB",tmpstr,"",szItemString,sizeof(szItemString),Ini);

		GetStr(szItemString,szItemData,sizeof(szItemData),1,',');
		strncpy(wtdef.yyb_tdxid[i],szItemData,sizeof(wtdef.yyb_tdxid[i]));

		GetStr(szItemString,szItemData,sizeof(szItemData),2,',');
		wtdef.yyb_bhfs[i] = atol(szItemData);

		GetStr(szItemString,szItemData,sizeof(szItemData),3,',');
		strncpy(wtdef.yyb_desc[i],szItemData,sizeof(wtdef.yyb_desc[i]));

		GetStr(szItemString,szItemData,sizeof(szItemData),4,',');
		wtdef.nyybType[i] = atol(szItemData);

		GetStr(szItemString,szItemData,sizeof(szItemData),5,',');
		if(szItemData[0]==0)
			wtdef.nyyb_clitype[i]=-1;
		else
			wtdef.nyyb_clitype[i]=atol(szItemData);

		GetStr(szItemString,szItemData,sizeof(szItemData),6,',');
		wtdef.yyb_noprefix[i] = atol(szItemData);
	}
	//
	wtdef.scnum = GetPrivateProfileInt( "sc","scnum",0, Ini );
	wtdef.scnum = min(wtdef.scnum,10);
	wtdef.scnum = max(0,wtdef.scnum);
	for(i=0;i<wtdef.scnum;i++)
	{
		sprintf(tmpstr,"SC_%02d",i+1);
		wtdef.sc[i] = GetPrivateProfileInt( "sc",tmpstr, 0, Ini );
		if(wtdef.sc[i]>=0 && wtdef.sc[i]<26)
		{
			if(strlen(g_OEM.ComboBoxKHWTName)>0 && wtdef.sc[i]==KHWT)
				strcpy(wtdef.scname[i],g_OEM.ComboBoxKHWTName);
			else if(strlen(g_OEM.ComboBoxSHHYName)>0 && wtdef.sc[i]==SHHY)
				strcpy(wtdef.scname[i],g_OEM.ComboBoxSHHYName);			
			else
				strcpy(wtdef.scname[i],wtbt[wtdef.sc[i]]);
		}
		else
			wtdef.scname[i][0]=0;		
		wtdef.bankcode[i][0]=0;
		wtdef.bankyyb[i][0]=0;
		wtdef.bankgdtype[i][0]=0;
	}
	wtdef.yh_scnum = GetPrivateProfileInt( "SC","BANKNUM",0, Ini );
	wtdef.yh_scnum = min(wtdef.yh_scnum,90);
	wtdef.yh_scnum = max(0,wtdef.yh_scnum);
	for(i=0;i<wtdef.yh_scnum;i++)
	{
		sprintf(tmpstr,"BANK_%d",i+1);
		GetPrivateProfileString( "SC", tmpstr, "δ֪����",tmpIniText,78, Ini );
		ParseMessageStr(tmpIniText,wtdef.scname[wtdef.scnum+i],",",1);
		ParseMessageStr(tmpIniText,wtdef.bankcode[wtdef.scnum+i],",",2);
		ParseMessageStr(tmpIniText,wtdef.bankyyb[wtdef.scnum+i],",",3);
		ParseMessageStr(tmpIniText,wtdef.bankgdtype[wtdef.scnum+i],",",4);
		wtdef.sc[wtdef.scnum+i]=0;
	}
	wtdef.scnum+=wtdef.yh_scnum;
	if(g_nLastZhType>=wtdef.scnum) g_nLastZhType=0;

	wtdef.nColorYzm=GetPrivateProfileInt("Other","ColorYzm",0,Ini);
	wtdef.bSwapCombo=GetPrivateProfileInt("Other","SwapCombo",0,Ini);
	wtdef.ZJZHMaxLen=GetPrivateProfileInt("Other","ZJZHMaxLen",0,Ini);
	wtdef.bMustDigitPwd=GetPrivateProfileInt("Other","MustDigitPwd",0,Ini);
	wtdef.nMustAccountLen=GetPrivateProfileInt("VYYB","MUSTACCOUNTLEN",0,Ini);
	wtdef.nMaxAccountLen=GetPrivateProfileInt("VYYB","MAXACCOUNTLEN",0,Ini);
	wtdef.AqfsCombo=GetPrivateProfileInt("Secure","AqfsCombo",0,Ini);
	//�������Ʊ��������ݴ�
	if(wtdef.bAuthPass || wtdef.bTDXCA)
		wtdef.AqfsCombo=1;
	GetPrivateProfileString("Other","APPLYAUTHPASS","",wtdef.UrlApplyAuthPass,250,Ini);
	GetPrivateProfileString("Other","MODIFYAUTHPASS","",wtdef.UrlModifyAuthPass,250,Ini);
}

extern long GetExploreExeDate();
BOOL ReadTCCookie(struct JyCookie_Info *pJyInfo)
{
	memset(pJyInfo,0,sizeof(struct JyCookie_Info));
	BOOL bRet = FALSE;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"tqcookie.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		char *buf = new char[sizeof(struct Zjzh_Cookie)];
		theFile.Read(buf,sizeof(struct Zjzh_Cookie));
		char *Password = "tdx_zjzh117cookie";
		CBlowfish  fish((BYTE *)Password,17);
		fish.Decrypt((BYTE *)buf,sizeof(struct Zjzh_Cookie));
		struct Zjzh_Cookie *pInfo = (struct Zjzh_Cookie *)buf;
		pInfo->qsid[10]=0;
		pInfo->yybinfo[20]=0;	
		pInfo->zjzh[40]=0;
		pInfo->LocalPath[90]=0;
		CTime nowTime=LongToTime(g_iToday);
		CTime lastverifyTime=LongToTime(pInfo->writedate);
		CTimeSpan timeSpan=nowTime-lastverifyTime;
		if(GetExploreExeDate()==pInfo->exploredate && timeSpan.GetDays()<g_VipVerify_Info.ReVerifySpan && stricmp(HomePath,pInfo->LocalPath)==0)
		{
			pJyInfo->khqx = pInfo->khqx;
			pJyInfo->vipuserflag = pInfo->vipuserflag;
			pJyInfo->canqslevel2 = pInfo->canqslevel2;
			pJyInfo->canadvjy = pInfo->canadvjy;
			pJyInfo->yybid = atol(pInfo->yybinfo);
			strcpy(pJyInfo->qsid,pInfo->qsid);
			strcpy(pJyInfo->zjzh,pInfo->zjzh);
			pJyInfo->canjgb = pInfo->canjgb;
			bRet=TRUE;
		}
		delete buf;
		theFile.Close();
	}
	return bRet;
}

int ReadZXCookie(struct ZxCache_Info *pZxInfo,int nMaxNum,BYTE *pZXSecurePackage/*1024*/)
{
	//�ײ���1024�ֽ����գ����ڹ��ŵ�ȯ�̣���ż��ܴ������ڹ���ȯ�̣������Ѷ��������Ϣ
	int nNum = 0;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"zxcache.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		int nLen = theFile.GetLength();
		if(nLen>0)
		{
			char *buf = new char[nLen+8];
			theFile.Read(buf,nLen);
			char *Password = "tdx_zjzh117cookie";
			CBlowfish  fish((BYTE *)Password,17);
			fish.Decrypt((BYTE *)buf,nLen);

			int nItem = (nLen-1024)/sizeof(struct ZxCache_Info);
			nNum = min(nItem,nMaxNum);
			if(pZxInfo && nNum>0)
				memcpy(pZxInfo,buf+1024,nNum*sizeof(struct ZxCache_Info));
			if(pZXSecurePackage)
				memcpy(pZXSecurePackage,buf,1024);
			delete buf;
		}
		theFile.Close();
	}
	return nNum;
}

int ReadCCCookie(struct CcCache_Info *pCcInfo,int nMaxNum)
{
	//�ײ���1024�ֽ�����
	int nNum = 0;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"cccache.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		int nLen = theFile.GetLength();
		if(nLen>0)
		{
			char *buf = new char[nLen+8];
			theFile.Read(buf,nLen);
			char *Password = "tdx_zjzh117cookie";
			CBlowfish  fish((BYTE *)Password,17);
			fish.Decrypt((BYTE *)buf,nLen);

			int nItem = (nLen-1024)/sizeof(struct CcCache_Info);
			nNum = min(nItem,nMaxNum);
			if(nNum>0)
				memcpy(pCcInfo,buf+1024,nNum*sizeof(struct CcCache_Info));
			delete buf;
		}
		theFile.Close();
	}
	return nNum;
}

BOOL LoadOtherCookie(struct OtherCache_Info *pInfo)
{
	memset(pInfo,0,sizeof(struct OtherCache_Info));

	BOOL bRet = FALSE;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"otcache.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		char *buf = new char[sizeof(struct OtherCache_Info)];
		theFile.Read(buf,sizeof(struct OtherCache_Info));
		char *Password = "l2wl_verify0";
		CBlowfish  fish((BYTE *)Password,12);
		fish.Decrypt((BYTE *)buf,sizeof(struct OtherCache_Info));
		memcpy(pInfo,buf,sizeof(struct OtherCache_Info));
		delete buf;
		theFile.Close();
		bRet=TRUE;
	}
	return bRet;
}

BOOL MakeOtherCookie(struct OtherCache_Info *pInfo)
{
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"otcache.dat",CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		char *Password = "l2wl_verify0";
		CBlowfish  fish((BYTE *)Password,12);
		fish.Encrypt((BYTE *)(pInfo),sizeof(struct OtherCache_Info));
		
		theFile.Write((BYTE *)(pInfo),sizeof(struct OtherCache_Info));
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}
