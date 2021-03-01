// ExportExtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ExportExtDlg.h"
#include "SelectGPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportExtDlg dialog

char *ExportGpColStr[2] = {"股票代码","股票名称"};
int ExportGpColWidth[2] = {60,65};
char *Separator_Str[4] = {"逗号(,)","分号(;)","TAB(\t)","空格( )"};
char *Separator_Value[4] = {",",";","\t"," "};
char *DateRule_Str[9] = {"MM/DD/YYYY","YYYY/MM/DD","DD/MM/YYYY","MM-DD-YYYY","YYYY-MM-DD","DD-MM-YYYY","MMDDYYYY","YYYYMMDD","DDMMYYYY"};

CExportExtDlg::CExportExtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportExtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportExtDlg)
	m_strFileNameRule = _T("");
	m_strPath = _T("");
	m_nWhichDataRule = 0;
	m_nWhichSeparator = 0;
	m_bTQ = TRUE;
	m_bMakeTitle = TRUE;
	//}}AFX_DATA_INIT
	AddedGPNum=0;
	m_lTmpStartDate=0;
	m_lTmpEndDate=0;
	m_bCancel=TRUE;
}


void CExportExtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportExtDlg)
	DDX_Control(pDX, IDC_SEPARATOR_COMBO, m_Separator_Combo);
	DDX_Control(pDX, IDC_DATERULE_COMBO, m_DateRule_Combo);
	DDX_Text(pDX, IDC_FILENAMERULE, m_strFileNameRule);
	DDX_Text(pDX, IDC_PATH, m_strPath);
	DDX_CBIndex(pDX, IDC_DATERULE_COMBO, m_nWhichDataRule);
	DDX_CBIndex(pDX, IDC_SEPARATOR_COMBO, m_nWhichSeparator);
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	DDX_Control(pDX, IDC_PHASE_STATIC, m_Phase_Static);
	DDX_Control(pDX, IDC_EXPORTGP_LIST, m_GpReport_List);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Check(pDX, IDC_TQ_CHECK, m_bTQ);
	DDX_Check(pDX, IDC_MAKETITLE_CHECK, m_bMakeTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportExtDlg, CDialog)
	//{{AFX_MSG_MAP(CExportExtDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportExtDlg message handlers

extern long GetVipDocLastDateFromCode(short setcode,char *code,long &firstdate);
BOOL CExportExtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Static.SetTextColor(RGB(0,64,64));
	m_Phase_Static.SetTextColor(RGB(0,0,255));
	m_lTmpEndDate=GetVipDocLastDateFromCode(SH,"000001",m_lTmpStartDate);

	CString tmpStr;
	if(m_lTmpStartDate==0 && m_lTmpEndDate==0)
		tmpStr.Format("本地日线数据:无");
	else
		tmpStr.Format("本地日线数据:%ld-%ld",m_lTmpStartDate,m_lTmpEndDate);
	m_Phase_Static.SetText(tmpStr);

	struct ExportExt_Habit tmpHabit;
	GetPrivateProfileString("Export","ExportPath",g_WSXHStr+"export",tmpHabit.strPath,MAX_PATH-1,g_WSXHStr+g_strUserCfg);	
	GetPrivateProfileString("Export","FileNameRule","YYXXXXXX.TXT",tmpHabit.strFileNameRule,98,g_WSXHStr+g_strUserCfg);	
	tmpHabit.nWhichDataRule=GetPrivateProfileInt("Export","WhichDataRule",0,g_WSXHStr+g_strUserCfg);
	tmpHabit.nWhichSeparator=GetPrivateProfileInt("Export","WhichSeparator",2,g_WSXHStr+g_strUserCfg);
	tmpHabit.bMakeTitle=GetPrivateProfileInt("Export","MakeTitle",1,g_WSXHStr+g_strUserCfg);
	tmpHabit.bTQ=GetPrivateProfileInt("Export","TQ",1,g_WSXHStr+g_strUserCfg);
	m_strPath = tmpHabit.strPath;
	m_strFileNameRule = tmpHabit.strFileNameRule;
	m_bTQ = tmpHabit.bTQ;
	m_bMakeTitle = tmpHabit.bMakeTitle;
	m_nWhichSeparator = tmpHabit.nWhichSeparator;
	m_nWhichDataRule = tmpHabit.nWhichDataRule;
	int i;
	for(i=0;i<4;i++)
		m_Separator_Combo.AddString(Separator_Str[i]);
	m_Separator_Combo.SetCurSel(m_nWhichSeparator);
	for(i=0;i<9;i++)
		m_DateRule_Combo.AddString(DateRule_Str[i]);
	m_DateRule_Combo.SetCurSel(m_nWhichDataRule);
	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	m_GpReport_List.SetImageList(&m_ImageList,LVSIL_SMALL);
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(i = 0; i < 2;i++)
	{
		lvColumn.pszText = _F(ExportGpColStr[i]);
		lvColumn.cx = ExportGpColWidth[i];
		lvColumn.iSubItem = i;
		m_GpReport_List.InsertColumn (i,&lvColumn);
	}

	UpdateData(FALSE);
	return TRUE;
}

void CExportExtDlg::FillGPReport()
{
	m_GpReport_List.SetRedraw(FALSE);	
	m_GpReport_List.DeleteAllItems();

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	int i;
	for(i = 0;i < AddedGPNum;i++)
	{
		lvItem.iItem = i;
		lvItem.iImage = 0;
		lvItem.pszText = (*g_pStockIO)[AddedGPIndex[i]]->Code;
		m_GpReport_List.InsertItem(&lvItem);
		m_GpReport_List.SetItemText(i,1,(*g_pStockIO)[AddedGPIndex[i]]->Name);
		m_GpReport_List.SetItemData(i,AddedGPIndex[i]);//仅为了拖曳
	}

	m_GpReport_List.SetRedraw(TRUE);
}

CString GetDateStr(long lDate,int nWhichDataRule)
{
	CString tmpStr="";
	switch(nWhichDataRule)
	{
	case 0:
		tmpStr.Format("%02d/%02d/%04d",(lDate%10000)/100,lDate%100,lDate/10000);break;
	case 1:
		tmpStr.Format("%04d/%02d/%02d",lDate/10000,(lDate%10000)/100,lDate%100);break;
	case 2:
		tmpStr.Format("%02d/%02d/%04d",lDate%100,(lDate%10000)/100,lDate/10000);break;
	case 3:
		tmpStr.Format("%02d-%02d-%04d",(lDate%10000)/100,lDate%100,lDate/10000);break;
	case 4:
		tmpStr.Format("%04d-%02d-%02d",lDate/10000,(lDate%10000)/100,lDate%100);break;
	case 5:
		tmpStr.Format("%02d-%02d-%04d",lDate%100,(lDate%10000)/100,lDate/10000);break;
	case 6:
		tmpStr.Format("%02d%02d%04d",(lDate%10000)/100,lDate%100,lDate/10000);break;
	case 7:
		tmpStr.Format("%04d%02d%02d",lDate/10000,(lDate%10000)/100,lDate%100);break;
	case 8:
		tmpStr.Format("%02d%02d%04d",lDate%100,(lDate%10000)/100,lDate/10000);break;
	}
	return tmpStr;
}

void CExportExtDlg::MakeLineStr(CString &tmpLine,struct AnalyData *pTmp,CString &tmpSep,short xsflag)
{
	if(xsflag==3)
		tmpLine.Format("%s%s%.3f%s%.3f%s%.3f%s%.3f%s%ld%s%.3f\n",GetDateStr(pTmp->Time.Date,m_nWhichDataRule),tmpSep,pTmp->Open,tmpSep,pTmp->High,tmpSep,pTmp->Low,tmpSep,pTmp->Close,tmpSep,pTmp->Volume,tmpSep,pTmp->Amount);
	else
		tmpLine.Format("%s%s%.2f%s%.2f%s%.2f%s%.2f%s%ld%s%.3f\n",GetDateStr(pTmp->Time.Date,m_nWhichDataRule),tmpSep,pTmp->Open,tmpSep,pTmp->High,tmpSep,pTmp->Low,tmpSep,pTmp->Close,tmpSep,pTmp->Volume,tmpSep,pTmp->Amount);
}

void CExportExtDlg::OnOK() 
{
	UpdateData(TRUE);
	TrimString(m_strPath);
	TrimString(m_strFileNameRule);
	if(m_strPath.GetLength()<1)
	{
		GetDlgItem(IDC_PATH)->SetFocus();
		TDX_MessageBox(m_hWnd,"请输入导出目录!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(m_strPath.Right(1) != "\\")
		m_strPath += "\\";
	if(m_strFileNameRule.GetLength()<1)
	{
		GetDlgItem(IDC_FILENAMERULE)->SetFocus();
		TDX_MessageBox(m_hWnd,"请输入文件名规则!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(AddedGPNum<1)
	{
		TDX_MessageBox(m_hWnd,"请添加股票!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	MemStkInfo * scode=NULL;
	CurrStockData tmpHQ;
	CStdioFile theFile;
	CFileException ex;
	CString strFileName;
	CString tmpSep;
	if(m_nWhichSeparator>=0&&m_nWhichSeparator<4)
		tmpSep=Separator_Value[m_nWhichSeparator];
	else
		tmpSep="\t";
	long TotalNum,nReadNum;
	int nTodayNum,xsflag=2,nCnt=0;
	m_Progress.SetRange32(0,AddedGPNum);
	m_Progress.SetPos(0);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_bCancel=FALSE;
	MSG msg;
	for(int i=0;i<AddedGPNum;i++)
	{
		scode=(*g_pStockIO)[AddedGPIndex[i]];
		if(scode)
		{
			while(::PeekMessage(&msg,NULL,0,0,PM_REMOVE)) //使界面保持活动
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if(m_bCancel) break;
			xsflag=GetXSFlag(scode);
			TotalNum = g_pStockIO->GetAnalyDataNum(scode,PER_DAY);
			AnalyData * ptmp = new AnalyData[TotalNum+1];	//防止PT的叠加缺少股票。
			memset(ptmp,0,sizeof(AnalyData)*(TotalNum+1));
			nReadNum = g_pStockIO->ReadAnalyDataAndCurrHQ(scode,ptmp,TotalNum,PER_DAY,&tmpHQ,nTodayNum);
			if(nReadNum > 0 && m_bTQ)
				g_pStockIO->TQData(scode,ptmp,0,nReadNum-1,nReadNum,1,PER_DAY);
			CString tmpFileName=m_strFileNameRule;
			CString tmpLine;
			tmpFileName.Replace("XXXXXX",scode->Code);
			tmpFileName.Replace("YY",(scode->setcode==SZ)?"SZ":"SH");
			strFileName.Format("%s%s",m_strPath,tmpFileName);
			if(theFile.Open(strFileName,CFile::modeCreate|CFile::modeWrite,&ex))
			{
				nCnt++;
				if(m_bMakeTitle)
				{
					char  Buffer[1024];
					sprintf(Buffer,"%s %s 日线\n",scode->Code,scode->Name);
					theFile.Write(Buffer,strlen(Buffer));
					sprintf(Buffer,"%10s\t%8s\t%8s\t%8s\t%8s\t%10s\t%10s\n","日期","开盘","最高","最低","收盘","成交量","成交额");
					theFile.Write(_F(Buffer),strlen(Buffer));
				}
				for(int j=0;j<nReadNum;j++)
				{
					if(ptmp[j].Time.Date<18000101 || ptmp[j].Time.Date>22000101) 
						continue;
					MakeLineStr(tmpLine,ptmp+j,tmpSep,xsflag);
					theFile.WriteString(tmpLine);
				}
				theFile.Close();
			}
			else
			{
				char szError[1024];
				ex.GetErrorMessage(szError, 1024);
				if(TDX_MessageBox(NULL,"文件创建错误:\n"+(CString)szError+(CString)"\n确定继续吗?",MB_OKCANCEL|MB_ICONEXCLAMATION)==IDCANCEL)
				{
					delete ptmp;
					break;
				}
			}
			delete ptmp;
		}
		m_Progress.SetPos(i+1);
	}
	m_Progress.SetPos(0);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	m_bCancel=TRUE;
	CString tmpStr;
	tmpStr.Format("共导出成功%d个文件.请检查",nCnt);
	TDX_MessageBox(m_hWnd,tmpStr,MB_OK|MB_ICONEXCLAMATION);
}

void CExportExtDlg::OnAdd() 
{
	CSelectGPDlg dlg;
	dlg.bHasDS = FALSE;
	if( dlg.DoModal() == IDOK )
	{
		int i,j;
		BOOL bFind;
		for(i = 0;i < dlg.SelectedGPNum;i++)
		{
			if(dlg.SelectedGPIndex[i]>=DS_FROM) continue; //暂时屏蔽三方行情
			bFind = FALSE;
			for(j = 0;j < AddedGPNum;j++)
			{
				if(AddedGPIndex[j] == dlg.SelectedGPIndex[i])
				{
					bFind = TRUE; 
					break;
				}
			}
			if(!bFind)  AddedGPIndex[AddedGPNum++] = dlg.SelectedGPIndex[i];
		}
		FillGPReport();
	}	
}

void CExportExtDlg::OnDelete() 
{
	POSITION pos = m_GpReport_List.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int nItem,i,j;
		while (pos) //先置删除标记-1
		{
			nItem = m_GpReport_List.GetNextSelectedItem(pos);
			AddedGPIndex[nItem] = -1;
		}
		for(i = AddedGPNum - 1;i >= 0;i--)
		{
			if(AddedGPIndex[i] == -1)
			{
				for(j = i+1;j < AddedGPNum;j++)
					AddedGPIndex[j-1] = AddedGPIndex[j];
				AddedGPNum--;
			}
		}
		FillGPReport();
	}		
}

void CExportExtDlg::OnBrowse() 
{
	BROWSEINFO BrInfo ;		
	ZeroMemory( &BrInfo, sizeof(BrInfo)) ;
	BrInfo.hwndOwner = this->m_hWnd ;
	BrInfo.lpszTitle = "选择导出目录";
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	//浏览文件夹
	LPITEMIDLIST ppidlDestination= SHBrowseForFolder(&BrInfo);
	//用户选择了取消按钮
	if(NULL == ppidlDestination)
		return;		
	char szPath[ MAX_PATH] ;
	SHGetPathFromIDList( ppidlDestination, szPath);
	m_strPath=szPath;
	UpdateData(FALSE);
}

void CExportExtDlg::OnCancel() 
{
	if(!m_bCancel)
	{
		m_bCancel=TRUE;
		return;
	}
	UpdateData(TRUE);
	if(m_nWhichDataRule>=0 && m_nWhichSeparator>=0)
	{
		struct ExportExt_Habit tmpHabit;
		strcpy(tmpHabit.strPath,m_strPath);
		strcpy(tmpHabit.strFileNameRule,m_strFileNameRule);
		tmpHabit.nWhichDataRule=m_nWhichDataRule;
		tmpHabit.nWhichSeparator=m_nWhichSeparator;
		tmpHabit.bTQ=m_bTQ;
		tmpHabit.bMakeTitle=m_bMakeTitle;

		WritePrivateProfileString("Export","ExportPath",tmpHabit.strPath,g_WSXHStr+g_strUserCfg);	
		WritePrivateProfileString("Export","FileNameRule",tmpHabit.strFileNameRule,g_WSXHStr+g_strUserCfg);	
		WritePrivateProfileString("Export","WhichDataRule",IntToStr(tmpHabit.nWhichDataRule),g_WSXHStr+g_strUserCfg);
		WritePrivateProfileString("Export","WhichSeparator",IntToStr(tmpHabit.nWhichSeparator),g_WSXHStr+g_strUserCfg);
		WritePrivateProfileString("Export","MakeTitle",tmpHabit.bMakeTitle?"1":"0",g_WSXHStr+g_strUserCfg);
		WritePrivateProfileString("Export","TQ",tmpHabit.bTQ?"1":"0",g_WSXHStr+g_strUserCfg);
	}

	CDialog::OnCancel();
}
