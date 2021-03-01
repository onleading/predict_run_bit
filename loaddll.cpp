#include "stdafx.h"
#include "loaddll.h"

#define GET_DLL_FUNCTION(module,funcname)	\
	funcname##_ = ( funcname ) ::GetProcAddress(module,#funcname);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//嵌入式委托
TC_Init_Environment		TC_Init_Environment_ = NULL;
TC_Login				TC_Login_  = NULL;
TC_GetLoginRet			TC_GetLoginRet_  = NULL;
TC_DoLevinJy			TC_DoLevinJy_  = NULL;
TC_CreateAll			TC_CreateAll_  = NULL;
TC_ArrangeWnd			TC_ArrangeWnd_  = NULL;
TC_RegisterCallBack		TC_RegisterCallBack_  = NULL;
TC_PreTransMsg			TC_PreTransMsg_  = NULL;
TC_GetJyStatus			TC_GetJyStatus_  = NULL;
TC_BeforeHideWT			TC_BeforeHideWT_ = NULL;
TC_GetRightInfo			TC_GetRightInfo_ = NULL;
TC_GetVersion			TC_GetVersion_ = NULL;
TC_GetClientInfo		TC_GetClientInfo_ = NULL;
TC_GetDlg				TC_GetDlg_ = NULL;
TC_Uninit				TC_Uninit_  = NULL;
TC_LevinGN				TC_LevinGN_ = NULL;
TC_GetExternLevinCfg	TC_GetExternLevinCfg_ = NULL;
TC_SetFeedBackMsg		TC_SetFeedBackMsg_ = NULL;
TC_GetYmdData			TC_GetYmdData_ = NULL;
TC_GetL2Info			TC_GetL2Info_ = NULL;

HMODULE					tc_mod  = NULL;

BOOL LoadTC_DLL()
{
	char filename[_MAX_PATH];
	sprintf(filename,"%stc.dll",HomePath);
	if(!tc_mod)
	{
		tc_mod = AfxLoadLibrary(filename);
		if(!tc_mod)	
			return FALSE;
		GET_DLL_FUNCTION(tc_mod,TC_Init_Environment)
		GET_DLL_FUNCTION(tc_mod,TC_Login)
		GET_DLL_FUNCTION(tc_mod,TC_GetLoginRet)
		GET_DLL_FUNCTION(tc_mod,TC_DoLevinJy)
		GET_DLL_FUNCTION(tc_mod,TC_CreateAll)
		GET_DLL_FUNCTION(tc_mod,TC_ArrangeWnd)
		GET_DLL_FUNCTION(tc_mod,TC_RegisterCallBack)
		GET_DLL_FUNCTION(tc_mod,TC_PreTransMsg)
		GET_DLL_FUNCTION(tc_mod,TC_GetJyStatus)
		GET_DLL_FUNCTION(tc_mod,TC_BeforeHideWT)
		GET_DLL_FUNCTION(tc_mod,TC_GetRightInfo)
		GET_DLL_FUNCTION(tc_mod,TC_GetVersion)
		GET_DLL_FUNCTION(tc_mod,TC_GetClientInfo)
		GET_DLL_FUNCTION(tc_mod,TC_GetDlg)
		GET_DLL_FUNCTION(tc_mod,TC_Uninit)
		GET_DLL_FUNCTION(tc_mod,TC_LevinGN)
		GET_DLL_FUNCTION(tc_mod,TC_GetExternLevinCfg)
		GET_DLL_FUNCTION(tc_mod,TC_SetFeedBackMsg)
		GET_DLL_FUNCTION(tc_mod,TC_GetYmdData)
		//此下函数可以没有
		GET_DLL_FUNCTION(tc_mod,TC_GetL2Info);
		
		if(!TC_Init_Environment_ || !TC_Login_ || !TC_GetLoginRet_ || !TC_DoLevinJy_ || !TC_CreateAll_ ||
		   !TC_ArrangeWnd_ || !TC_RegisterCallBack_ || !TC_PreTransMsg_ || !TC_GetJyStatus_ || !TC_BeforeHideWT_||!TC_GetRightInfo_ ||!TC_GetVersion_ ||!TC_GetClientInfo_ ||!TC_GetDlg_ || !TC_Uninit_ || 
		   !TC_LevinGN_ || !TC_GetExternLevinCfg_ || !TC_SetFeedBackMsg_ || !TC_GetYmdData_)
		   return FALSE;
	}
	return TRUE;
}

BOOL InitTC(char QsId[11],const char *PubDir,const char *PriDir,const char *CfgFile)
{
	BOOL bRet=FALSE;
	if(!tc_mod)
	{
		if(!LoadTC_DLL()) return FALSE;
		if(tc_mod)
		{
			bRet = TC_Init_Environment_(QsId,PubDir,PriDir,CfgFile,0);
			TC_RegisterCallBack_(CalcCallBackFunc,SwitchCallBackFunc);
		}
	}
	return bRet;
}

void UninitTC()
{
	if(tc_mod)
	{
		if(TC_Uninit_) TC_Uninit_();
		AfxFreeLibrary(tc_mod);
		tc_mod=NULL;
	}
}

/////////////////////////////////////////////////////////////////////////
//外部数据DLL

getEncryptParameter	getEncryptParameter_ = NULL;
JGXEncrypt			JGXEncrypt_ = NULL;
JGXDecrypt			JGXDecrypt_ = NULL;
HMODULE				otherdll_mod=NULL;

BOOL LoadOtherDLL()
{
	if(g_OEM.bGxzqEmbedWTVersion)
	{
		char filename[_MAX_PATH];
		sprintf(filename,"%surlhandle.dll",HomePath);
		if(!otherdll_mod)
		{
			otherdll_mod = AfxLoadLibrary(filename);
			if(!otherdll_mod)	
				return FALSE;
			GET_DLL_FUNCTION(otherdll_mod,getEncryptParameter)
		}
		if(!getEncryptParameter_)
			return FALSE;
	}
	else if(g_OEM.bDgzqEmbedWTVersion)
	{
		char filename[_MAX_PATH];
		sprintf(filename,"%sjgx_dll.dll",HomePath);
		if(!otherdll_mod)
		{
			otherdll_mod = AfxLoadLibrary(filename);
			if(!otherdll_mod)	
				return FALSE;
			JGXEncrypt_ = ( JGXEncrypt ) ::GetProcAddress(otherdll_mod,"_jgx_encrypt@4");
			JGXDecrypt_ = ( JGXDecrypt ) ::GetProcAddress(otherdll_mod,"_jgx_decrypt@4");
		}
		if(!JGXEncrypt_ || !JGXDecrypt_)
			return FALSE;
	}
	return TRUE;
}

void UnLoadOtherDLL()
{
	if(otherdll_mod)
	{
		AfxFreeLibrary(otherdll_mod);
		otherdll_mod = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////
//券商定制DLL

QS_Init				QS_Init_ = NULL;
QS_RegisterCallBack	QS_RegisterCallBack_ = NULL;
QS_MainFrameCreated	QS_MainFrameCreated_ = NULL;
QS_BeforeLoginJY	QS_BeforeLoginJY_ = NULL;
QS_AfterLoginJY		QS_AfterLoginJY_ = NULL;
QS_TransferURL		QS_TransferURL_ = NULL;
QS_Option			QS_Option_ = NULL;
QS_GetCustomInfo	QS_GetCustomInfo_ = NULL;
QS_MainFrameClosed	QS_MainFrameClosed_ = NULL;
QS_Uninit			QS_Uninit_ = NULL;

HMODULE				QSRun_mod=NULL;

BOOL LoadQSRunDLL()
{
	char filename[_MAX_PATH];
	sprintf(filename,"%sTQSRun.dll",HomePath);
	if(!QSRun_mod)
	{
		QSRun_mod = AfxLoadLibrary(filename);
		if(!QSRun_mod)	
			return FALSE;
		GET_DLL_FUNCTION(QSRun_mod,QS_Init)
		GET_DLL_FUNCTION(QSRun_mod,QS_RegisterCallBack)
		GET_DLL_FUNCTION(QSRun_mod,QS_MainFrameCreated)
		GET_DLL_FUNCTION(QSRun_mod,QS_BeforeLoginJY)
		GET_DLL_FUNCTION(QSRun_mod,QS_AfterLoginJY)
		GET_DLL_FUNCTION(QSRun_mod,QS_TransferURL)
		GET_DLL_FUNCTION(QSRun_mod,QS_Option)
		GET_DLL_FUNCTION(QSRun_mod,QS_GetCustomInfo)
		GET_DLL_FUNCTION(QSRun_mod,QS_MainFrameClosed)
		GET_DLL_FUNCTION(QSRun_mod,QS_Uninit)
	}
	if(!QS_Init_ || !QS_RegisterCallBack_ || !QS_MainFrameCreated_ || !QS_BeforeLoginJY_ || !QS_AfterLoginJY_ || !QS_TransferURL_ || !QS_Option_ || !QS_GetCustomInfo_ || !QS_MainFrameClosed_ || !QS_Uninit_)
		return FALSE;
	return TRUE;
}

void UnLoadQSRunDLL()
{
	if(QSRun_mod)
	{
		QS_Uninit_();
		AfxFreeLibrary(QSRun_mod);
		QSRun_mod = NULL;
	}
}
