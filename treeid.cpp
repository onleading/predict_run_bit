#include "stdafx.h"
#include "treeid.h"
#include "resource.h"
#include "MainFrm.h"

struct TreeID_Map g_pTreeIDMap[]=
{
	//ֱ�ӵ�Command
	{"HQ",			CMD_DIRECT,	ID_SHOWHQ},				//��ʾ����
	{"JY",			CMD_DIRECT,	ID_FUNC_WT},			//��������
	{"ZX",			CMD_DIRECT,	IDC_ZXTREE},			//��ʾ��Ѷ��
	{"HOTBLOCK",	CMD_DIRECT,	ID_SHOWHISTORY},		//���Ű�����
	{"SETZXG",		CMD_DIRECT,	ID_BLOCKOPTION},		//���ɲ���
	{"MYPAD",		CMD_DIRECT,	IDC_RECENTPAD},			//����Ķ��ư���
	{"HK",			CMD_DIRECT,	IDM_DS1+31},			//��ʾ�۹�
	{"QH",			CMD_DIRECT,	IDM_DS1+30},			//��ʾ�ڻ�
	{"QZSDS",		CMD_DIRECT,	IDC_QZSDS},				//������
	{"SORT",		CMD_DIRECT,	IDC_6787},				//����(67+87)
	{"PAD1",		CMD_DIRECT,	IDC_SPEC_CUSTOMPAD1},	//��һ���ض�����
	{"PAD2",		CMD_DIRECT,	IDC_SPEC_CUSTOMPAD2},	//�ڶ����ض�����
	{"URGENT",		CMD_DIRECT,	ID_URGENT},				//����ͨ��Ի���
	{"JJREPORT",	CMD_DIRECT,	ID_JJREPORT},			//�������ƽ̨
	
	{"SORT67",			CMD_HQ,		67},				//����(67)
	{"SORT87",			CMD_HQ,		87},				//����(87)
	//���鴰��
	{"AG",			CMD_HQ,		TYPE_AG+1},				//A���б�
	{"BG",			CMD_HQ,		TYPE_BG+1},				//B���б�
	{"ZXB",			CMD_HQ,		TYPE_ZXB+1},			//��С���б�
	{"CYB",			CMD_HQ,		TYPE_CYB+1},			//��ҵ���б�
	{"QZ",			CMD_HQ,		TYPE_QZ+1},				//Ȩ֤�б�
	{"ZXG",			CMD_HQ,		TYPE_ZXG+1},			//��ѡ���б�	
	{"BLOCKZS",		CMD_HQ,		TYPE_SPECINDEX+1},		//���ָ��
	

	//��������
	{"F3",			CMD_COM,	102},					//��֤����
	{"F4",			CMD_COM,	108},					//��֤����
	
	//���׶Ի���
	{"REGUSER",		CMD_TCDLG,	DLG_REGUSER},			//�����û�ע��
	{"ACTIVEUSER",	CMD_TCDLG,	DLG_ACTIVEUSER},		//���������û�
	{"UPGRADEUSER",	CMD_TCDLG,	DLG_UPGRADEUSER},		//��������
	{"LEAVEWORD",	CMD_TCDLG,	DLG_LEAVEWORD},			//��ʾ������Ϣ
	//
	{"",		0,			0}
};

void CarryOut_WebInfoTreeID(const char *pInfo)
{
	int companyno=0,infotreeid=0,daynum=0;
	//
	char tempContent[255]={0};
	ParseMessageStr(pInfo,tempContent,"_",2);
	companyno=atol(tempContent);
	ParseMessageStr(pInfo,tempContent,"_",3);
	infotreeid=atol(tempContent);
	ParseMessageStr(pInfo,tempContent,"_",4);
	daynum=atol(tempContent);
	
	//�򿪱�����
	SwitchToView("Cjzx");
	DWORD tmpItemData = infotreeid*100+companyno;
	theViewManager.pActiveView->SendMessage(UM_CJZX_DAY_MESSAGE,tmpItemData,daynum);
}

//ִ�к���
void TreeID_CarryOut(const char *pIDStr)
{
	CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
	if(strnicmp(pIDStr,"info_",5)==0)
	{
		CarryOut_WebInfoTreeID(pIDStr);
		return;
	}
	else if(strnicmp(pIDStr,"gpcode_",7)==0)
	{
		MemStkInfo *tmpInfo=g_pStockIO->Get_MemStkInfo(pIDStr+7);
		if(tmpInfo)	ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);	
		return;
	}
	else if(strnicmp(pIDStr,"cmdid_",6)==0)
	{
		UINT nID = atol(pIDStr+6);
		pMainWnd->SendMessage(WM_COMMAND, nID, 0);
		return;
	}
	else if(strnicmp(pIDStr,"ietitle/",8)==0)	//�Ի���� http://www.treeid/ietitle/����http://��ַ
	{
		CString tmpLine = pIDStr+8;
		int nHttpPos = tmpLine.Find("http");
		if(nHttpPos>0)
		{
			CString tmpIWTitle=tmpLine.Left(nHttpPos);
			char tmpUrlCode[255]={0};
			UrlDecode(tmpIWTitle,tmpUrlCode);
			g_strIWTitle = tmpUrlCode;
			//
			g_strIWURL=tmpLine.Mid(nHttpPos);
			if(pMainWnd)
				pMainWnd->PostMessage(UM_OPENIWCONTENT,1,0);
		}
		return;
	}	
	else if(strnicmp(pIDStr,"http",4)==0)	//�ⲿ��	http://www.treeid/http://.......
	{
		OpenCusWebPage(g_pMainWin,pIDStr,FALSE);
		return;
	}
	int i=0;
	while(strlen(g_pTreeIDMap[i].IDStr)>0)
	{
		if(stricmp(pIDStr,g_pTreeIDMap[i].IDStr)==0)
		{
			switch(g_pTreeIDMap[i].CommandType)
			{
			case CMD_DIRECT:	//ֱ�ӵ�Command
				{
					if(g_pTreeIDMap[i].CommandID==IDC_ZXTREE && g_OEM.bOutStandZXGHY)
					{
						OpenCusWebPage(g_pMainWin,g_OEM.strWebButtonURL,g_OEM.bMainWebPageInsideOpen);
						return;
					}
					DWORD tmpID = g_pTreeIDMap[i].CommandID;
					if(tmpID==IDC_SPEC_CUSTOMPAD1)	//�ر�ĵ�һ��ҳ,������Ҫ������
					{
						tmpID = IDC_CUSTOMPAD1;
						CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
						pMainWnd->ShowTabCtrlBar(0,FALSE);
						DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
						if(dwStatus&JY_LOGINED)
							pMainWnd->SwitchToRestSpace(AREA_WT);
					}
					if(tmpID==IDC_SPEC_CUSTOMPAD2)	//�ر�ĵڶ���ҳ,������Ҫ����
					{
						tmpID = IDC_CUSTOMPAD2;
						CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
						pMainWnd->ShowTabCtrlBar(0,FALSE);
						pMainWnd->ShowTabCtrlBar(1,FALSE);
					}
					CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
					if(pMainWnd)
						pMainWnd->PostMessage(WM_COMMAND,tmpID);
				}
				break;
			case CMD_MAIN:		//������
			case CMD_HQ:		//���鴰��
			case CMD_COM:		//��������
			case CMD_JBM:		//�������ϴ���
			case CMD_NEWS:		//������Ϣ����
			case CMD_HIS:		//��ʷ��������
			case CMD_ADVHQ:		//�߼����鴰��
			case CMD_JBFX:		//������������
			case CMD_JJ:		//�����������
				{
					CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
					if(pMainWnd)
						pMainWnd->OnCommCommand(g_pTreeIDMap[i].CommandType,g_pTreeIDMap[i].CommandID);
				}
				break;
			case CMD_TCDLG:		//���׶Ի���
				if(TC_GetDlg_)
					TC_GetDlg_(g_pTreeIDMap[i].CommandID);
				break;
			}
			break;
		}
		i++;
	}
}

//IE�ؼ����ڲ��ر����Ӵ���
BOOL BeforeNavigate_Process(char *url)
{
	if(memicmp(url,"HTTP://WWW.ZHIYINGID/",21)==0)
	{
		TreeID_CarryOut(url+21);
		return TRUE;
	}
	else if(memicmp(url,"HTTP://ZHIYINGID/",17)==0)
	{
		TreeID_CarryOut(url+17);
		return TRUE;
	}
	else if(memicmp(url,"HTTP://WWW.TREEID/",18)==0)
	{
		TreeID_CarryOut(url+18);
		return TRUE;
	}
	else if(memicmp(url,"HTTP://TREEID/",14)==0)
	{
		TreeID_CarryOut(url+14);
		return TRUE;
	}
	return FALSE;	
}
