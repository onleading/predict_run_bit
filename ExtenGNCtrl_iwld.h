// ExtenGNCtrl.h: interface for the CExtenGNCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTENGNCTRL_H__348F158B_BA98_45F3_82E7_0382FC745700__INCLUDED_)
#define AFX_EXTENGNCTRL_H__348F158B_BA98_45F3_82E7_0382FC745700__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "loaddll.h"

#define MAX_SUBMODULE_NUM		100
#define MAX_REGEDMESSAGE_NUM	20
#define MAX_ACCEPTABLE_NUM		20

struct ModuleMessageInfo 
{
	UINT	uRegedDataMsg;										//ϵͳ�������Ϣ����������
	UINT	uRegMsgID;											//ͳһ�ɿ��ע��
	char	strRegMesage[21];									//ע�����Ϣ����
	char	strAcceptableSourceModule[MAX_ACCEPTABLE_NUM][11];	//�ɽ�����Ϣ����Դ
};

struct SubModuleInfo
{
	UINT 	nSubModuleType;										//��ģ������ͣ�0-��ϵͳ���ĸ����� 1-��ϵͳ�o���Ĵ���
	char	strSubModuleID[11];									//ʶ������
	char 	strSubModuleName[41];								//ģ����,��Ϊ����������֦���Ͳ˵���
	ModuleMessageInfo	ModuleMsgInfo[MAX_REGEDMESSAGE_NUM];	//��ģ��ע�����Ϣ
};

struct ModuleInfo
{
	UINT 	nModuleType;		//��MODULE_STOCK, MODULE_ETF, MODULE_FUTURES�Ⱥ�
	char	strModuleID[11];	//ʶ������
	char 	strModuleName[41];	//ģ����,��Ϊ����������֦���Ͳ˵���
	char 	strProvider[41];	//ģ���Ȩ��Ϣ
	float 	fVersion;			//��ģ��İ汾��
	UINT 	nSubModuleNum;		//��ģ��ĸ���
	SubModuleInfo	SubModules[MAX_SUBMODULE_NUM];	//��ģ��
	char	strSerialNo[33];	//���к�
	char	unUsed[222];
};

//ȡģ����Ϣ
typedef void	(*_TDXWEXTGN_GETMODULEINFO)(struct ModuleInfo *pInfo);
//����ģ��ص������ĵ�ַ
typedef void	(*_TDXWEXTGN_REGISTERCALLBACK)(PDATAIOFUNC pDataFn,PSWITCHINFOFUNC pFuncFn);
//��ʼ��ģ��
typedef BOOL	(*_TDXWEXTGN_INITMODULE)(const char *PubDir=".\\",const char *PriDir=".\\", DWORD unUsed=0);
//��ɫ����
typedef	void	(*_TDXWEXTGN_Scheme)(SCHEME_COLOR color, SCHEME_FONT font, SCHEME_MENU menu);
//�ڿ����������ʾ��ģ��Ľ���,nWhichSubΪ��ģ����
typedef BOOL	(*_TDXWEXTGN_SHOWMODULE)(UINT nUnitID,UINT nWhichSub, HWND hVessel,char *lpAppName,char *lpFileName,char *errMsg,DWORD unUsed=0);
//��С�ı仯����Ӧ
typedef void 	(*_TDXWEXTGN_ARRANGEWND)(UINT nUnitID,UINT nWhichSub,UINT cx,UINT cy);
//���ݺ���Ϣ����
typedef BOOL	(* _TDXWEXTGN_SETVALUE)(int nUnitID,UINT nWhichSub,int DataType,int DataLen,void *pData);
//�˳�ģ��
typedef void	(*_TDXWEXTGN_EXITMODLE)(UINT nUnitID,UINT nWhichSub,char *lpAppName,char *lpFileName);			//-1��ʾȫ���ӹ����˳�

///////////////////////////////////////////////////////////////////////////////////////////

typedef	struct	tagModuleIndex						//	ģ����غ�ʹ����Ϣ
{
	HMODULE					hModule;				//	��̬���ӿ���
	ModuleInfo				iModuleInfo;			//	ģ����Ϣ
	_TDXWEXTGN_INITMODULE	ModuleInit;				
	_TDXWEXTGN_SHOWMODULE	ModuleShow;
	_TDXWEXTGN_Scheme		ModuleScheme;
	_TDXWEXTGN_ARRANGEWND	ModuleResize;
	_TDXWEXTGN_SETVALUE		ModuleDataTrans;
	_TDXWEXTGN_EXITMODLE	ModuleExit;
	short					StatusFlag;				//	�Ƿ���ɳ�ʼ��
	int						nSubStartID;			//	��ģ��ID����ӦUnitType
	//
	HWND					ParentWnd[MAX_SUBMODULE_NUM];
	short					SubStatusFlag[MAX_SUBMODULE_NUM];
}IMODULE;

extern DWORD l_GlobalUnitID;

class CNewMenu;
//////////////////////////////////////////////////////////////////////////
class CExtenGNCtrl  
{
public:
	CExtenGNCtrl();
	virtual ~CExtenGNCtrl();

	long	LoadExternModule(void);
	BOOL	ShowThis(HWND hParentWnd, long nNo, char *errMsg);
	void	ResizeThis(CWnd *pWnd,UINT cx,UINT cy);
	void	ExitThis(CWnd *pWnd);

	short	GetModuleNum(void)
	{
		return m_ModuleEntries.size();
	};
	BOOL	GetModuleInfo(short nModule, ModuleInfo &iModuleInfo)
	{
		if(nModule<0||nModule>=m_ModuleEntries.size()) return FALSE;
		memcpy(&iModuleInfo, &m_ModuleEntries[nModule].iModuleInfo, sizeof(ModuleInfo));
		return m_ModuleEntries[nModule].hModule!=NULL;
	};

	BOOL	LoadModule(int nUnitStyle, int &nSubModule, IMODULE* &pModule);

private:
	BOOL	LoadModuleDll(LPCSTR  DllName, int nModule, IMODULE *iModule);

private:
	vector<IMODULE> m_ModuleEntries;
};

#endif // !defined(AFX_EXTENGNCTRL_H__348F158B_BA98_45F3_82E7_0382FC745700__INCLUDED_)
