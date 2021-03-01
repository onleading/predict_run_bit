// LocalPtStkSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "LocalPtStkSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalPtStkSetDlg dialog


CLocalPtStkSetDlg::CLocalPtStkSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalPtStkSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLocalPtStkSetDlg)
	m_BaseCode = _T("");
	m_Code = _T("");
	m_Name = _T("");
	m_BaseSetcode = 0;
	m_Error = _T("");
	//}}AFX_DATA_INIT
	m_bInited = FALSE;
}


void CLocalPtStkSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocalPtStkSetDlg)
	DDX_Control(pDX, IDC_PTSTK_LIST, m_StkList);
	DDX_Text(pDX, IDC_LCPTSTKBASECODE_EDIT, m_BaseCode);
	DDX_Text(pDX, IDC_LCPTSTKCODE_EDIT, m_Code);
	DDX_Text(pDX, IDC_LCPTSTKNAME_EDIT, m_Name);
	DDX_Radio(pDX, IDC_PTSTK_RADIO_SZ, m_BaseSetcode);
	DDX_Text(pDX, IDC_LCPTSTKERROR_STATIC, m_Error);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalPtStkSetDlg, CDialog)
	//{{AFX_MSG_MAP(CLocalPtStkSetDlg)
	ON_LBN_DBLCLK(IDC_PTSTK_LIST, OnDblclkPtstkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalPtStkSetDlg message handlers

void CLocalPtStkSetDlg::SetPtSet(LPLCPTSET pLcPtSet,long lPtNo)
{
	if(pLcPtSet)
	{
		memcpy(&m_LcPtSet,pLcPtSet,sizeof(LCPTSET));
		m_LcPtNo	= lPtNo;
		m_bInited	= TRUE;
	}
}

BOOL CLocalPtStkSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_PTFIX)->SetWindowText(LCPT_FIX);

	if(m_bInited)
	{
		m_BaseCode	= m_LcPtSet.BaseCode;
		m_Code		= m_LcPtSet.Code+3;
		m_Name		= m_LcPtSet.Name;
		m_BaseSetcode	= m_LcPtSet.BaseSetcode;
		m_Error		= "";
	}
	else
	{
		m_BaseCode	= "";
		m_Code		= "";
		m_Name		= "";
		m_BaseSetcode	= 0;
		m_Error		= "";
	}
	m_StkList.SetItemHeight(0,17);
	m_StkList.SetColumnWidth(140);
	FillList();
	UpdateData(FALSE);

	return TRUE;
}

void CLocalPtStkSetDlg::OnOK() 
{
	UpdateData(TRUE);
	BOOL bNoError = TRUE;
	if(bNoError)
	{
		m_Code.TrimLeft();
		m_Code.TrimRight();
		if(m_Code.GetLength()!=SH_CODE_LEN-3)
		{
			m_Error = _F("请填写完整品种代码后3位.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		m_Name.TrimLeft();
		m_Name.TrimRight();
		if(m_Name.GetLength()<=0 || m_Name.GetLength()>NAME_LEN)
		{
			m_Error = _F("请填写品种简称,要求不多于8个字符或4个汉字.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		m_BaseCode.TrimLeft();
		m_BaseCode.TrimRight();
		if(m_BaseCode.GetLength()!=SH_CODE_LEN)
		{
			m_Error = _F("请填写完整6位品种代码.");
			bNoError = FALSE;
		}
	}

	CString tmpstr = LCPT_FIX+m_Code;
	strcpy(m_LcPtSet.Code,tmpstr);
	strcpy(m_LcPtSet.Name,m_Name);
	strcpy(m_LcPtSet.BaseCode,m_BaseCode);
	m_LcPtSet.BaseSetcode	= m_BaseSetcode;
	if(bNoError)
	{
		if(g_pLcIdxMng->TestPtExist(m_LcPtSet.Code,m_LcPtNo))
		{
			m_Error = _F("已经有相同代码的品种,请作修改.");
			bNoError = FALSE;
		}
	}
	if(!bNoError)
	{
		UpdateData(FALSE);
		return;
	}
	CDialog::OnOK();
}

void CLocalPtStkSetDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CLocalPtStkSetDlg::FillList()
{
	long i = 0;
	long setcode = SZ;
	long LineCount = 0;
	char LineStr[100];
	char WordStr[100];
	CString FilePath = g_WSXHStr+"hq_cache\\pttab.dat";
	CString ListItemStr;
	FILE *fpt = fopen(LPCSTR(FilePath),"rb");
	if(!fpt) return;

	m_StkList.SetRedraw(FALSE);
	m_StkList.ResetContent();

	while(!feof(fpt))
	{
		memset(LineStr,0,100*sizeof(char));
		memset(WordStr,0,100*sizeof(char));
		fgets(LineStr,80,fpt);
		AllTrim(LineStr);
		if(0==LineStr[0]) continue;
		ParseMessageStr(LineStr,WordStr,",",1);
		if(0==WordStr[0]) continue;
		setcode = atoi(WordStr);
		switch(setcode)
		{
		case SZ:
			ListItemStr = _F("深圳");
			break;
		case SH:
			ListItemStr = _F("上海");
			break;
		default:
			continue;
		}
		ParseMessageStr(LineStr,WordStr,",",2);
		if(0==WordStr[0]) continue;
		WordStr[SH_CODE_LEN] = '\0';
		ListItemStr += ",";
		ListItemStr += WordStr;
		ParseMessageStr(LineStr,WordStr,",",3);
		if(0==WordStr[0]) continue;
		WordStr[NAME_LEN] = '\0';
		ListItemStr += ",";
		ListItemStr += _F(WordStr);
		m_StkList.InsertString(LineCount,ListItemStr);
		LineCount++;
	}
	
	m_StkList.SetRedraw(TRUE);

	fclose(fpt);
}

//双击进行设置
void CLocalPtStkSetDlg::OnDblclkPtstkList() 
{
	int nSel = m_StkList.GetCurSel();
	if(nSel>=0)
	{
		UpdateData(TRUE);
		
		CString tmpLine;
		m_StkList.GetText(nSel,tmpLine);
		char tmpContent[100];
		ParseMessageStr(tmpLine,tmpContent,",",1);
		if(stricmp(tmpContent,_F("上海"))==0)
			m_BaseSetcode = SH;
		else
			m_BaseSetcode = SZ;
		ParseMessageStr(tmpLine,tmpContent,",",2);
		m_BaseCode = tmpContent;
		ParseMessageStr(tmpLine,tmpContent,",",3);
		m_Name = tmpContent;

		UpdateData(FALSE);
	}
}
