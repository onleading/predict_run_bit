#ifndef   __FUNC_INI_H__
#define   __FUNC_INI_H__

extern int  LoadSomeString(char deststr[][30],char *type,char *inisection,int maxcount,char *totalstr,const char *inifile);
extern void SaveSomeString(char deststr[][30],char *type,char *inisection,int count,char *totalstr,const char *inifile);

extern void		Dectect_OverWrite_ProxyInfo();
extern BOOL		GetFromComteFile(int nFlag,BOOL bQs,BOOL bUseBak,const char *Pwd);
extern void		GetHostInfoFromFile(TDXWHOST *HqHost,TDXWHOST *InfoHost,TDXWHOST *WtHost,TDXWHOST *VipWtHost,TDXWHOST *DsHost,PROXYINFO *TdxProxyHost,struct VipVerify_Info *pVerifyInfo,BOOL bGetPrimary,CString Ini,BOOL bReplaceLevel2=FALSE);
extern void		WriteHostInfoToFile(TDXWHOST *HqHost,TDXWHOST *InfoHost,TDXWHOST *WtHost,TDXWHOST *VipWtHost,TDXWHOST *DsHost,PROXYINFO *TdxProxyHost,struct VipVerify_Info *pVerifyInfo,BOOL bSetPrimary,CString Ini);
extern BOOL		GetPrimaryHQServerInfo(CString &strAddress,UINT &nPort,BOOL bSwitchNext);
extern void		GetEmbOEMCfg(struct Global_EmbOem *pCfg,CString Ini);
extern void		GetDSOtherCfg();
extern void		GetDSMarketCfg(CString DS_Ini);
extern void		LoadSeatData();
extern BOOL		GetSeatNameFromSID(long nSeatID, char *strSeatNameS,char *strSeatNameL=NULL);

extern void		LoadShortCutCfg();
extern void		SaveShortCutCfg();

extern void		GetPasswordFromRegister(const char *pKeyString, CString & strUserName, CString & strPassword, long &nLastValidDate, short &nLastValidValue);
extern void		WritePasswordToRegister(const char *pKeyString, char *strUserName, char *strPassword, long &nLastValidDate, short &nLastValidValue);
extern void		GetExecPathFromRegister(const char *pKeyPath,const char *pKeyString,CString & strExecPath);

extern void		NotifyShell(LONG wEventId,LPSTR szPath);
extern BOOL		CreateLink(LPSTR szPath,LPSTR szLink);

#endif
