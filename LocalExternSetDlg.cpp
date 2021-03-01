// LocalExternSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "LocalExternSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalExternSetDlg dialog


CLocalExternSetDlg::CLocalExternSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalExternSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLocalExternSetDlg)
	m_LcExtPath = _T("");
	m_LcExtCode = _T("");
	m_LcExtName = _T("");
	m_IgnoreLine = 0;
	m_Error = _T("");
	//}}AFX_DATA_INIT

	memset(&m_LcExtSet,0,sizeof(LCEXTSET));
	m_bInited = FALSE;
}


void CLocalExternSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocalExternSetDlg)
	DDX_Control(pDX, IDC_COMBO_VOLRATIO, m_VolRatio);
	DDX_Control(pDX, IDC_COMBO_FORMAT9, m_LcExtFormat9);
	DDX_Control(pDX, IDC_COMBO_FORMAT8, m_LcExtFormat8);
	DDX_Control(pDX, IDC_COMBO_FORMAT7, m_LcExtFormat7);
	DDX_Control(pDX, IDC_COMBO_FORMAT6, m_LcExtFormat6);
	DDX_Control(pDX, IDC_COMBO_FORMAT5, m_LcExtFormat5);
	DDX_Control(pDX, IDC_COMBO_FORMAT4, m_LcExtFormat4);
	DDX_Control(pDX, IDC_COMBO_FORMAT3, m_LcExtFormat3);
	DDX_Control(pDX, IDC_COMBO_FORMAT2, m_LcExtFormat2);
	DDX_Control(pDX, IDC_COMBO_FORMAT10, m_LcExtFormat10);
	DDX_Control(pDX, IDC_COMBO_FORMAT1, m_LcExtFormat1);
	DDX_Control(pDX, IDC_COMBO_SPACEMARK, m_SpaceMark);
	DDX_Control(pDX, IDC_COMBO_DATEFORMAT, m_DateFormat);
	DDX_Text(pDX, IDC_EDIT_PATH, m_LcExtPath);
	DDX_Text(pDX, IDC_LCINDEXCODE_EDIT, m_LcExtCode);
	DDX_Text(pDX, IDC_LCINDEXNAME_EDIT, m_LcExtName);
	DDX_Text(pDX, IDC_EDIT_IGNORELINE, m_IgnoreLine);
	DDX_Text(pDX, IDC_LCINDEXERROR_STATIC, m_Error);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalExternSetDlg, CDialog)
	//{{AFX_MSG_MAP(CLocalExternSetDlg)
	ON_BN_CLICKED(IDC_BUTTON_PATH, OnButtonPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalExternSetDlg message handlers

void CLocalExternSetDlg::OnOK() 
{
	UpdateData(TRUE);
	BOOL bNoError = TRUE;
	if(bNoError)
	{
		m_LcExtCode.TrimLeft();
		m_LcExtCode.TrimRight();
		if(m_LcExtCode.GetLength()!=SH_CODE_LEN-3)
		{
			m_Error = _F("请填写完整品种代码后3位.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		m_LcExtName.TrimLeft();
		m_LcExtName.TrimRight();
		if(m_LcExtName.GetLength()<=0 || m_LcExtName.GetLength()>NAME_LEN)
		{
			m_Error = _F("请填写品种简称,要求不多于8个字符或4个汉字.");
			bNoError = FALSE;
		}
	}
	CString tmpstr = LCEXT_FIX+m_LcExtCode;
	strncpy(m_LcExtSet.Code,tmpstr,SH_CODE_LEN);
	m_LcExtSet.Code[SH_CODE_LEN]= '\0';
	if(bNoError)
	{
		if(g_pLcIdxMng->TestExtExist(m_LcExtSet.Code,m_LcExtNo))
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
	strncpy(m_LcExtSet.Name,m_LcExtName,NAME_LEN);
	m_LcExtSet.Name[NAME_LEN]	= '\0';
	strncpy(m_LcExtSet.SrcFilePath,m_LcExtPath,255);
	m_LcExtSet.SrcFilePath[255]	= '\0';
	m_LcExtSet.lSkipLines	= m_IgnoreLine;

	m_LcExtSet.nDateFormat	= m_DateFormat.GetCurSel();
	m_LcExtSet.nSpaceType	= m_SpaceMark.GetCurSel();
	m_LcExtSet.nFieldFormat[0]	= m_LcExtFormat1.GetCurSel();
	m_LcExtSet.nFieldFormat[1]	= m_LcExtFormat2.GetCurSel();
	m_LcExtSet.nFieldFormat[2]	= m_LcExtFormat3.GetCurSel();
	m_LcExtSet.nFieldFormat[3]	= m_LcExtFormat4.GetCurSel();
	m_LcExtSet.nFieldFormat[4]	= m_LcExtFormat5.GetCurSel();
	m_LcExtSet.nFieldFormat[5]	= m_LcExtFormat6.GetCurSel();
	m_LcExtSet.nFieldFormat[6]	= m_LcExtFormat7.GetCurSel();
	m_LcExtSet.nFieldFormat[7]	= m_LcExtFormat8.GetCurSel();
	m_LcExtSet.nFieldFormat[8]	= m_LcExtFormat9.GetCurSel();
	m_LcExtSet.nFieldFormat[9]	= m_LcExtFormat10.GetCurSel();
	m_LcExtSet.nVolRatio		= m_VolRatio.GetCurSel();
	
	CDialog::OnOK();
}

void CLocalExternSetDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CLocalExternSetDlg::SetExtSet(LPLCEXTSET pLcExtSet,long lExtNo)
{
	if(pLcExtSet)
	{
		memcpy(&m_LcExtSet,pLcExtSet,sizeof(LCEXTSET));
		m_LcExtNo	= lExtNo;
		m_bInited	= TRUE;
	}
}

//日期格式
char lcExtDateFormatStr[][12]	= {"MM/DD/YYYY","DD/MM/YYYY","YYYY/MM/DD","MM-DD-YYYY","DD-MM-YYYY","YYYY-MM-DD","YYYYMMDD"};
//间隔符号
char lcextSpaceMarkStr[][12]	= {"逗号(,)","分号(;)","制表Tab(\t)","空格( )"};
//行格式
char lcExtFormatStr[][12]		= {"无效跳过","数据日期","开盘价格","最高价格","最低价格","收盘价格","成交数量","成交金额"};
//成交量比例
char lcExtVolRatioStr[][12]		= {"原始比例","1/10","1/100","1/1000","1/10000"};

BOOL CLocalExternSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_LCFIX)->SetWindowText(LCEXT_FIX);
	
	if(m_bInited)
	{
		SetFormat(m_DateFormat,lcExtDateFormatStr,7,m_LcExtSet.nDateFormat);
		SetFormat(m_SpaceMark,lcextSpaceMarkStr,4,m_LcExtSet.nSpaceType);
		SetFormat(m_LcExtFormat1,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[0]);
		SetFormat(m_LcExtFormat2,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[1]);
		SetFormat(m_LcExtFormat3,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[2]);
		SetFormat(m_LcExtFormat4,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[3]);
		SetFormat(m_LcExtFormat5,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[4]);
		SetFormat(m_LcExtFormat6,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[5]);
		SetFormat(m_LcExtFormat7,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[6]);
		SetFormat(m_LcExtFormat8,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[7]);
		SetFormat(m_LcExtFormat9,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[8]);
		SetFormat(m_LcExtFormat10,lcExtFormatStr,8,m_LcExtSet.nFieldFormat[9]);
		SetFormat(m_VolRatio,lcExtVolRatioStr,5,m_LcExtSet.nVolRatio);

		m_LcExtPath = m_LcExtSet.SrcFilePath;
		m_LcExtCode	= m_LcExtSet.Code+3;
		m_LcExtName	= m_LcExtSet.Name;
		m_IgnoreLine= m_LcExtSet.lSkipLines;
	}
	else
	{
		SetFormat(m_DateFormat,lcExtDateFormatStr,7,0);
		SetFormat(m_SpaceMark,lcextSpaceMarkStr,4,2);
		SetFormat(m_LcExtFormat1,lcExtFormatStr,8,1);
		SetFormat(m_LcExtFormat2,lcExtFormatStr,8,2);
		SetFormat(m_LcExtFormat3,lcExtFormatStr,8,3);
		SetFormat(m_LcExtFormat4,lcExtFormatStr,8,4);
		SetFormat(m_LcExtFormat5,lcExtFormatStr,8,5);
		SetFormat(m_LcExtFormat6,lcExtFormatStr,8,6);
		SetFormat(m_LcExtFormat7,lcExtFormatStr,8,7);
		SetFormat(m_LcExtFormat8,lcExtFormatStr,8,0);
		SetFormat(m_LcExtFormat9,lcExtFormatStr,8,0);
		SetFormat(m_LcExtFormat10,lcExtFormatStr,8,0);
		SetFormat(m_VolRatio,lcExtVolRatioStr,5,0);

		m_LcExtPath = "";
		m_LcExtCode	= "";
		m_LcExtName	= "";
		m_IgnoreLine= 0;
	}
	UpdateData(FALSE);
	
	return TRUE;
}

void CLocalExternSetDlg::OnButtonPath() 
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,m_LcExtPath,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_EXTENSIONDIFFERENT,_F("文本文件(*.TXT)|*.TXT||"));
	if (FileDialog.DoModal() == IDOK)
		m_LcExtPath = FileDialog.GetPathName();

	UpdateData(FALSE);
}

void CLocalExternSetDlg::SetFormat(CComboBox &LcExtFmtBox,char (*BoxStr)[12],long lStrNum,long lDefault)
{
	long lIndex = 0;
	long i = 0;
	LcExtFmtBox.SetRedraw(FALSE);
	LcExtFmtBox.ResetContent();
	for(i=0;i<lStrNum;i++)
	{
		lIndex = LcExtFmtBox.InsertString(i,_F(BoxStr[i]));
		LcExtFmtBox.SetItemData(lIndex,i);
	}
	LcExtFmtBox.SetRedraw(TRUE);
	if(lDefault<0 || lDefault>=lStrNum) LcExtFmtBox.SetCurSel(0);
	else LcExtFmtBox.SetCurSel(lDefault);
}
