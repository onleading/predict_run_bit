#ifndef INVESTEXP_H_
#define INVESTEXP_H_

#ifdef	INVEST_EXPORT
	#define INVESTDLL	_declspec(dllexport)
#else
	#define INVESTDLL	_declspec(dllimport)
#endif

//���ݴ���Ĺ�Ʊ���룬�õ���Ʊ���ƣ����ּۣ����յȣ����Name,fNow,fCloseΪNULL,�򲻸�ֵ
//����0����ʾʧ�ܣ�����ֵ�����ܹ�Ʊ������Ч����0��ʾ�ɹ�
typedef long (CALLBACK * PGPINFOHQ)(const char * Code,short setcode,char *Name,float *fNow,float *fClose);

INVESTDLL void SetPrivateInvestDir(char *path);
INVESTDLL void RegisterInvestGPInfoHq(PGPINFOHQ pfn);
INVESTDLL void PopupInvestDlg();//�������ضԻ���


#endif