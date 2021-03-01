// externpage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "externpage.h"
#include "OTHERWTDLG.h"
#include "WebPageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExternPage property page
IMPLEMENT_DYNCREATE(CExternPage, CPropertyPage)

CExternPage::CExternPage() : CPropertyPage(CExternPage::IDD)
{
	//{{AFX_DATA_INIT(CExternPage)
	m_strInfoExe = _T("");
	m_strInvestExe = _T("");
	m_strWtPath = "";
	//}}AFX_DATA_INIT
	m_bInsideOpenPage = g_bInsideOpenPage;
	m_bExtern_Info = g_bExtern_Info;
	m_bExtern_Invest = g_bExtern_Invest;
}

CExternPage::~CExternPage()
{
}

void CExternPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExternPage)
	DDX_Control(pDX, IDC_WEBPAGE_COMB, m_webpage_comb);
	DDX_Control(pDX, IDC_OTHERWT_COMB, m_otherwt_comb);
	DDX_Check(pDX, IDC_EXTERN_INFO, m_bExtern_Info);
	DDX_Check(pDX, IDC_EXTERN_INVEST, m_bExtern_Invest);
	DDX_Text(pDX, IDC_INFOEXE_EDIT, m_strInfoExe);
	DDV_MaxChars(pDX, m_strInfoExe, 255);
	DDX_Text(pDX, IDC_INVESTEXE_EDIT, m_strInvestExe);
	DDV_MaxChars(pDX, m_strInvestExe, 255);
	DDX_Text(pDX, IDC_WTPATH_EDIT, m_strWtPath);
	DDV_MaxChars(pDX, m_strWtPath, 255);
	DDX_Check(pDX, IDC_INSIDEOPENPAGE_CHECK, m_bInsideOpenPage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExternPage, CPropertyPage)
	//{{AFX_MSG_MAP(CExternPage)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_EXTERN_INVEST, OnExternInvest)
	ON_BN_CLICKED(IDC_EXTERN_INFO, OnExternInfo)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_OTHERWT_ADD, OnOtherwtAdd)
	ON_BN_CLICKED(IDC_OTHERWT_MOD, OnOtherwtMod)
	ON_BN_CLICKED(IDC_OTHERWT_DEL, OnOtherwtDel)
	ON_BN_CLICKED(IDC_WEBPAGE_ADD, OnWebpageAdd)
	ON_BN_CLICKED(IDC_WEBPAGE_MOD, OnWebpageMod)
	ON_BN_CLICKED(IDC_WEBPAGE_DEL, OnWebpageDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExternPage message handlers

BOOL CExternPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	char tmppath[256];	
	GetPrivateProfileString("EXTERN","INFOPATH","*****",tmppath,255,g_WSXHStr+g_strYxhjCfg);	
	m_strInfoExe = tmppath;
	GetPrivateProfileString("OTHER","INVESTPATH","*****",tmppath,255,g_WSXHStr+g_strUserCfg);	
	m_strInvestExe = tmppath;

	GetDlgItem(IDC_INVESTEXE_EDIT)->EnableWindow(m_bExtern_Invest);
	GetDlgItem(IDC_BROWSE1)->EnableWindow(m_bExtern_Invest);
	GetDlgItem(IDC_INFOEXE_EDIT)->EnableWindow(m_bExtern_Info);
	GetDlgItem(IDC_BROWSE2)->EnableWindow(m_bExtern_Info);
	
	if(!g_OEM.bEmbedWTVersion)
	{
		GetDlgItem(IDC_WTPATH_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BROWSE4)->EnableWindow(TRUE);
		GetDlgItem(IDC_OTHERWT_COMB)->EnableWindow(TRUE);
		GetDlgItem(IDC_OTHERWT_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_OTHERWT_MOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_OTHERWT_DEL)->EnableWindow(TRUE);
		CString cfgpath=g_WSXHStr+g_strYxhjCfg;
		GetPrivateProfileString("EXTERN","WTPATH","*****",tmppath,255,cfgpath);
		m_strWtPath = tmppath;

		DWORD nGetRes = GetPrivateProfileString("EXTERN","WTNAME1","",tmppath,255,cfgpath);
		int nExeIndex=1;
		char wtStr[64];
		OTHERWT tmpOtherWt;
		while (nGetRes>0 && nExeIndex<10)
		{
			tmpOtherWt.wtName=tmppath;
			sprintf(wtStr,"WTPATH%d", nExeIndex);
			nGetRes = GetPrivateProfileString("EXTERN",wtStr,"",tmppath,255,cfgpath);
			tmpOtherWt.wtPath=tmppath;
			m_vOtherWt.push_back(tmpOtherWt);

			m_otherwt_comb.InsertString(nExeIndex-1,tmpOtherWt.wtName);

			sprintf(wtStr,"WTNAME%d", ++nExeIndex);
			nGetRes = GetPrivateProfileString("EXTERN",wtStr,"",tmppath,255,cfgpath);
		}
		if(nExeIndex>1)
			m_otherwt_comb.SetCurSel(0);
	}
	else
	{
		GetDlgItem(IDC_WTPATH_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE4)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_COMB)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_MOD)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_DEL)->EnableWindow(FALSE);
	}
	
	if(g_GC.nShareMode)
	{
		GetDlgItem(IDC_EXTERN_INFO)->EnableWindow(FALSE);
		GetDlgItem(IDC_INFOEXE_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE2)->EnableWindow(FALSE);

		GetDlgItem(IDC_WTPATH_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE4)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_COMB)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_MOD)->EnableWindow(FALSE);
		GetDlgItem(IDC_OTHERWT_DEL)->EnableWindow(FALSE);
	}
	
	m_vWebpage.clear();
	for (int i=0;i<g_nCusWebPageNum;i++)
	{
		WEBPAGE tmpWebpage;
		tmpWebpage.name=g_CusWebPage[i].PageName;
		tmpWebpage.url=g_CusWebPage[i].PageUrl;
		m_webpage_comb.InsertString(i,_F(tmpWebpage.name));
		m_vWebpage.push_back(tmpWebpage);
	}
	if(g_nCusWebPageNum>0)
		m_webpage_comb.SetCurSel(0);
	UpdateData(FALSE);
	return TRUE;
}

void CExternPage::OnBrowse1() 
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							_F("理财程序(*.exe)|*.exe||"));
	if (FileDialog.DoModal() == IDOK)
	{
		m_strInvestExe = FileDialog.GetPathName();
		UpdateData(FALSE);
	}	
}

void CExternPage::OnBrowse2() 
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							_F("资讯程序(*.exe)|*.exe||"));
	if (FileDialog.DoModal() == IDOK)
	{
		m_strInfoExe = FileDialog.GetPathName();
		UpdateData(FALSE);
	}		
}

void CExternPage::OnExternInvest() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_INVESTEXE_EDIT)->EnableWindow(m_bExtern_Invest);
	GetDlgItem(IDC_BROWSE1)->EnableWindow(m_bExtern_Invest);
}

void CExternPage::OnExternInfo() 
{
	UpdateData(TRUE);	
	GetDlgItem(IDC_INFOEXE_EDIT)->EnableWindow(m_bExtern_Info);
	GetDlgItem(IDC_BROWSE2)->EnableWindow(m_bExtern_Info);
}

void CExternPage::WriteOtherWT()
{
	int n=m_vOtherWt.size();
	CString strWt,cfgpath;
	cfgpath=g_WSXHStr+g_strYxhjCfg;
	for(int i=0;i<max(10,n);i++)
	{
		strWt.Format("WTNAME%d",i+1);
		WritePrivateProfileString("EXTERN",strWt,(i<n?m_vOtherWt[i].wtName:""),cfgpath);
		strWt.Format("WTPATH%d",i+1);
		WritePrivateProfileString("EXTERN",strWt,(i<n?m_vOtherWt[i].wtPath:""),cfgpath);
	}
	m_vOtherWt.clear();
}

void CExternPage::OnOK() 
{
	UpdateData(TRUE);

	g_bInsideOpenPage = m_bInsideOpenPage;
	g_bExtern_Info = m_bExtern_Info;
	g_bExtern_Invest = m_bExtern_Invest;
	WritePrivateProfileString("Other","InsideOpenPage",g_bInsideOpenPage?"1":"0",g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("Other","EXTERNINFO",g_bExtern_Info?"1":"0",g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("Other","INFOPATH",m_strInfoExe,g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("Other","ExternInvest",g_bExtern_Invest?"1":"0",g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("OTHER","INVESTPATH",m_strInvestExe,g_WSXHStr+g_strUserCfg);
	
	//////
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	if(!g_OEM.bEmbedWTVersion)	//如果是提供委托的远程版
	{
		WritePrivateProfileString("EXTERN","WTPATH",m_strWtPath,Ini);
		WriteOtherWT();
	}
	CString	CusWebIni=g_WSXHStr+g_strUserCfg;
	CString tempStr;
	g_nCusWebPageNum=m_vWebpage.size();
	for(int i=0;i<max(MAX_CUSWEB_NUM,g_nCusWebPageNum);i++)
	{
		if(i<g_nCusWebPageNum)
		{
			strcpy(g_CusWebPage[i].PageName,m_vWebpage[i].name);
			strcpy(g_CusWebPage[i].PageUrl,m_vWebpage[i].url);
		}
		tempStr.Format("WebPageName%d",i);
		WritePrivateProfileString("EXTERN",tempStr,(i<g_nCusWebPageNum?m_vWebpage[i].name:""),CusWebIni);
		tempStr.Format("WebPageURL%d",i);
		WritePrivateProfileString("EXTERN",tempStr,(i<g_nCusWebPageNum?m_vWebpage[i].url:""),CusWebIni);
	}
	CPropertyPage::OnOK();
}

void CExternPage::OnBrowse4() 
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,NULL,
		OFN_NOCHANGEDIR,
		_F("委托下单程序(*.exe)|*.exe||"));
	if (FileDialog.DoModal() == IDOK)
	{
		m_strWtPath = FileDialog.GetPathName();
		UpdateData(FALSE);
	}
}

void CExternPage::OnOtherwtAdd() 
{
	if(m_otherwt_comb.GetCount()>=10)
	{
		TDX_MessageBox(NULL,"最多只能加入10个委托程序!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	COtherWTDlg dlg;
	dlg.m_title=_F("添加新委托");
	if(dlg.DoModal()==IDOK)
	{
		OTHERWT tmpOtherWt;
		tmpOtherWt.wtName=dlg.m_strWtName;
		tmpOtherWt.wtPath=dlg.m_strWtPath;
		m_vOtherWt.push_back(tmpOtherWt);
		m_otherwt_comb.AddString(tmpOtherWt.wtName);
		m_otherwt_comb.SetCurSel(m_otherwt_comb.GetCount()-1);
	}
}

void CExternPage::OnOtherwtMod() 
{
	COtherWTDlg dlg;
	int nIndex=m_otherwt_comb.GetCurSel();
	if(nIndex<0) return;
	dlg.m_title=_F("修改委托");
	dlg.m_strWtName=m_vOtherWt[nIndex].wtName;
	dlg.m_strWtPath=m_vOtherWt[nIndex].wtPath;
	if(dlg.DoModal()==IDOK)
	{
		m_vOtherWt[nIndex].wtName=dlg.m_strWtName;
		m_vOtherWt[nIndex].wtPath=dlg.m_strWtPath;
		m_otherwt_comb.DeleteString(nIndex);
		m_otherwt_comb.InsertString(nIndex,dlg.m_strWtName);
		m_otherwt_comb.SetCurSel(nIndex);
	}
}

void CExternPage::OnOtherwtDel() 
{
	int nIndex=m_otherwt_comb.GetCurSel();
	if(nIndex<0) return;
	if(TDX_MessageBox(m_hWnd,"您确定删除此外挂委托吗?", MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL) 
		return;
	m_otherwt_comb.DeleteString(nIndex);
	m_vOtherWt.erase(m_vOtherWt.begin()+nIndex);
	int n=m_otherwt_comb.GetCount();
	m_otherwt_comb.SetCurSel(nIndex>(n-1)?n-1:nIndex);
}

void CExternPage::OnWebpageAdd() 
{
	if(m_vWebpage.size()>=MAX_CUSWEB_NUM)
	{
		TDX_MessageBox(NULL,"超过了能加入的网页链接数!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CWebPageDlg dlg;
	dlg.m_title=_F("添加个股相关网站链接");
	if(dlg.DoModal()==IDOK)
	{
		WEBPAGE tmpWebpage;
		tmpWebpage.name=dlg.m_name;
		tmpWebpage.url=dlg.m_url;
		m_vWebpage.push_back(tmpWebpage);
		m_webpage_comb.AddString(dlg.m_name);
		m_webpage_comb.SetCurSel(m_webpage_comb.GetCount()-1);
	}
}

void CExternPage::OnWebpageMod() 
{
	CWebPageDlg dlg;
	int nIndex=m_webpage_comb.GetCurSel();
	if(nIndex<0) return;
	dlg.m_title=_F("修改个股相关网站链接");
	dlg.m_name=m_vWebpage[nIndex].name;
	dlg.m_url=m_vWebpage[nIndex].url;
	if(dlg.DoModal()==IDOK)
	{
		m_vWebpage[nIndex].name=dlg.m_name;
		m_vWebpage[nIndex].url=dlg.m_url;
		m_webpage_comb.DeleteString(nIndex);
		m_webpage_comb.InsertString(nIndex,dlg.m_name);
		m_webpage_comb.SetCurSel(nIndex);
	}
}

void CExternPage::OnWebpageDel() 
{
	int nIndex=m_webpage_comb.GetCurSel();
	if(nIndex<0) return;
	if(TDX_MessageBox(m_hWnd,"您确定删除此个股相关网站链接?", MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL) 
		return;
	m_webpage_comb.DeleteString(nIndex);
	m_vWebpage.erase(m_vWebpage.begin()+nIndex);
	int n=m_webpage_comb.GetCount();
	m_webpage_comb.SetCurSel(nIndex>(n-1)?n-1:nIndex);
}
