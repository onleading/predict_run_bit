#ifndef INVESTEXP_H_
#define INVESTEXP_H_

#ifdef	INVEST_EXPORT
	#define INVESTDLL	_declspec(dllexport)
#else
	#define INVESTDLL	_declspec(dllimport)
#endif

//根据传入的股票代码，得到股票名称，其现价，昨收等，如果Name,fNow,fClose为NULL,则不赋值
//返回0，表示失败，不赋值，可能股票代码无效，非0表示成功
typedef long (CALLBACK * PGPINFOHQ)(const char * Code,short setcode,char *Name,float *fNow,float *fClose);

INVESTDLL void SetPrivateInvestDir(char *path);
INVESTDLL void RegisterInvestGPInfoHq(PGPINFOHQ pfn);
INVESTDLL void PopupInvestDlg();//弹出主控对话框


#endif