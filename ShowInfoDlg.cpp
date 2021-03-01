// ShowInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ShowInfoDlg.h"
#include "Keyguy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowInfoDlg dialog


CShowInfoDlg::CShowInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowInfoDlg)
	//}}AFX_DATA_INIT
}


void CShowInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowInfoDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CShowInfoDlg)
	ON_BN_CLICKED(IDC_SEEURGENT, OnSeeurgent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

long GetVipDocLastDateFromCode(short setcode,char *code,long &firstdate)
{
	long lRet=0;
	firstdate=0;
	CFile FxFile;
	CString tmpFileName;
	tmpFileName.Format("%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(setcode),GetSetStr(setcode),code);
	g_HisDataReader.Lock();
	if(FxFile.Open(tmpFileName,CFile::modeRead))
	{
		if(FxFile.GetLength()>=sizeof(ANALYDATA))
		{
			ANALYDATA tmpAnalyData;
		
			FxFile.Read(&tmpAnalyData,sizeof(ANALYDATA));
			firstdate = tmpAnalyData.Time.Date;
			
			FxFile.Seek(-sizeof(ANALYDATA),SEEK_END);		
			FxFile.Read(&tmpAnalyData,sizeof(ANALYDATA));
			lRet = tmpAnalyData.Time.Date;
		}
		FxFile.Close();
	}
	g_HisDataReader.Unlock();
	return lRet;
}

long GetLocalVipDocDateNum()
{
	long lTmpEndDate,lTmpStartDate;
	lTmpEndDate=GetVipDocLastDateFromCode(SH,"000001",lTmpStartDate);
	CTime endTime=LongToTime(lTmpEndDate);
	CTime startTime=LongToTime(lTmpStartDate);
	CTimeSpan tp=endTime-startTime;
	return tp.GetDays();
}

BOOL CShowInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString tmpStr,tmpContent;
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_INFO);
	m_ImageList.Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_INFO2);
	m_ImageList.Add(hIcon);
	m_List.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_List.SetExtendedStyle (LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	//������
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.pszText = "��Ŀ";
	lvColumn.cx = 120;
	m_List.InsertColumn (0,&lvColumn);
	lvColumn.pszText = "״̬";
	lvColumn.cx = 250;
	m_List.InsertColumn (1,&lvColumn);
	//
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;
	int nPos=0;
	
	//---------------------------------------------------------
	//������
	lvItem.iItem = nPos;
	lvItem.iImage = 0;
	lvItem.pszText = _F("������");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(g_strTitleName));
	nPos++;
	//��������ʱ��
	tmpStr.Format("%08d %s",g_SysInfo.Program_StartYmd,GetTimeStrFromLong(g_SysInfo.Program_StartHms));
	lvItem.iItem = nPos;
	lvItem.iImage = 0;
	lvItem.pszText = _F("��������ʱ��");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(tmpStr));
	nPos++;

	//��ɫͨ��
	if(g_ProxyInfo.bTdxProxy)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("��ɫͨ��");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F("��������ѿ���"));
		nPos++;	
	}
	//����״̬
	if(OnLine)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("������վ");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_HQNameStr));
		nPos++;
	}
	if(OnLine)
		tmpStr.Format("����%s[%d]",g_HQIPStr,g_HQPort);
	else
		tmpStr.Format("û��������վ");
	lvItem.iItem = nPos;
	lvItem.iImage = 1;
	lvItem.pszText = _F("�����ַ");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(tmpStr));
	nPos++;
	//��������ʱ��
	if(OnLine)
	{
		tmpStr.Format("%08d %s",g_SysInfo.ConnectHQ_LastYmd,GetTimeStrFromLong(g_SysInfo.ConnectHQ_LastHms));
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("��������ʱ��");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;
	}
	//�������г�
	if(g_bHasDS)
	{
		if(DSOnLine)
			tmpStr.Format("����%s[%d]",g_DSHQIPStr,g_DSHQPort);
		else
			tmpStr.Format("û��������վ");
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		char tmpHintInfo[MAX_PATH];
		sprintf(tmpHintInfo,"%s��վ",g_DS.DSOEMName);
		lvItem.pszText = _F(tmpHintInfo);
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;
		//������������ʱ��
		if(DSOnLine)
		{
			tmpStr.Format("%08d,%s",g_SysInfo.ConnectDSHQ_LastYmd,GetTimeStrFromLong(g_SysInfo.ConnectDSHQ_LastHms));
			lvItem.iItem = nPos;
			lvItem.iImage = 0;
			sprintf(tmpHintInfo,"%s����ʱ��",g_DS.DSOEMName);
			lvItem.pszText = _F(tmpHintInfo);
			m_List.InsertItem(&lvItem);
			m_List.SetItemText(nPos,1,_F(tmpStr));
			nPos++;
		}
	}	
	//����ϵͳ
	if(g_OEM.bEmbedWTVersion && TC_GetJyStatus_)
	{
		char hostip[255]={0};
		UINT port = 0;
		DWORD dwStatus=TC_GetJyStatus_(hostip,&port,NULL);
		if(!(dwStatus&JY_LOGINED))
			tmpStr.Format("����δ��¼");
		else if(dwStatus&JY_LOCKING)
			tmpStr.Format("������ ����%s[%d]",hostip,port);
		else if(dwStatus&JY_LOGINED)
			tmpStr.Format("�ѵ�¼ ����%s[%d]",hostip,port);
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("����ϵͳ");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;	
	}
	//�������Ϣ
	if(g_OEM.bEmbedWTVersion && g_MngRight>=0 && g_MngRight<=9)
	{
		tmpStr.Format("[%d]�� %s",g_MngRight,g_RightStr);
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("�û��ȼ�");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;	
		if(g_OEM.bFormatAndUseYYBID)
		{
			if(g_JyInfo.yybid==0)
				tmpStr.Format("[δ��¼]");
			else
				tmpStr.Format("[%04d]",g_JyInfo.yybid);
			lvItem.iItem = nPos;
			lvItem.iImage = 0;
			lvItem.pszText = _F("Ӫҵ��ID");
			m_List.InsertItem(&lvItem);
			m_List.SetItemText(nPos,1,_F(tmpStr));
			nPos++;	
		}
	}
	//����ʱ��
	if(g_bUserHostEverConnect && g_ExpireDate>0)
	{
		tmpStr.Format("%04d��%02d��%02d��",g_ExpireDate/10000,(g_ExpireDate%10000)/100,g_ExpireDate%100);
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("ʹ�õ���");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;	
	}
	//�û����ʹ�
	if(g_bUserHostEverConnect && strlen(g_strUserType)>0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("�û�����");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_strUserType));
		nPos++;	
	}
	//�û����ʴ�
	if(g_bUserHostEverConnect && strlen(g_strUserCharacter)>0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("�û�����");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_strUserCharacter));
		nPos++;	
	}
	//�û�Ȩ�޴�
	if(g_bUserHostEverConnect && strlen(g_strUserLimits)>0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("�û�Ȩ��");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_strUserLimits));
		nPos++;	
	}
	if(g_nNetCondition==0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("������������");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F("����������������,��Щ���ݿ���ά����ȫ��"));
		nPos++;	
	}
	if(g_nTQFlag==2 || (g_nTQFlag==3 && g_nPartTq==1))
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("��ǰ��Ȩ����");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F("���óɺ�Ȩ,���·������ݲ���̬ά��"));
		nPos++;	
	}
	//��������
	long	lTmpStartDate,lTmpEndDate;
	lTmpEndDate=GetVipDocLastDateFromCode(SH,"000001",lTmpStartDate);
	if(lTmpStartDate==0 && lTmpEndDate==0)
		tmpStr.Format("�ޱ�������");
	else
		tmpStr.Format("%ld-%ld",lTmpStartDate,lTmpEndDate);
	lvItem.iItem = nPos;
	lvItem.iImage = 1;
	lvItem.pszText = _F("����������������");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(tmpStr));
	nPos++;	

	return TRUE;
}

void CShowInfoDlg::OnSeeurgent() 
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_URGENT);
	CDialog::OnCancel();
}
