// WriteUrgent.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "WriteUrgent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWriteUrgent dialog


CWriteUrgent::CWriteUrgent(CWnd* pParent /*=NULL*/)
	: CDialog(CWriteUrgent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWriteUrgent)
	m_time = 0;
	m_strSource = _T("");
	m_Content = _T("");
	//}}AFX_DATA_INIT
}


void CWriteUrgent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWriteUrgent)
	DDX_Control(pDX, IDC_GROUP_COMBO, m_GroupCombo);
	DDX_Control(pDX, IDC_CONTENT_EDIT, m_Content_Edit);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER, m_time);
	DDX_Text(pDX, IDC_URGENTSOURCE_EDIT, m_strSource);
	DDV_MaxChars(pDX, m_strSource, 30);
	DDX_Text(pDX, IDC_CONTENT_EDIT, m_Content);
	DDV_MaxChars(pDX, m_Content, 2000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWriteUrgent, CDialog)
	//{{AFX_MSG_MAP(CWriteUrgent)
	ON_BN_CLICKED(IDC_IMPORTFILE, OnImportfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteUrgent message handlers

char *l_UserGroupStr[10]={"普通用户","体验用户","VIP用户","核心用户","超级用户/总编","用户组6","用户组7","用户组8","用户组9","用户组10"};
BOOL CWriteUrgent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER));
	
	m_GroupCombo.SetItemHeight(0,16);
	m_GroupCombo.SetItemHeight(-1,16);
	for(int i=0;i<10;i++)
		m_GroupCombo.AddString(l_UserGroupStr[i]);
	m_GroupCombo.SetCurSel(0);

	m_time = CTime::GetCurrentTime();
	CTimeSpan tempSpan(1,0,0,0);		//下一天
	m_time += tempSpan;

	UpdateData(FALSE);
	return TRUE;
}

void CWriteUrgent::OnImportfile() 
{
	CString m_strFilePath;
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							"文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	if (FileDialog.DoModal() == IDOK)
	{
		m_strFilePath = FileDialog.GetPathName();
		CFile theFile;
		if(theFile.Open(m_strFilePath,CFile::modeRead))
		{
			DWORD tmpLen = theFile.GetLength();
			short readlen = MIN(tmpLen,6000);
			char *buf = (char *)malloc(readlen+1);
			theFile.Read(buf,readlen);
			buf[readlen] = 0;
			m_Content_Edit.SetWindowText(buf);
			free(buf);
			theFile.Close();
		}
	}	
}

void CWriteUrgent::OnOK() 
{
	UpdateData(TRUE);
	int nSel=m_GroupCombo.GetCurSel();
	if(nSel==-1) return;

	if(m_Content.GetLength()>2000)
		return;

	g_pWriteUrgent = new struct WriteUrgentInfo;
	g_pWriteUrgent->userright=nSel+1;
	g_pWriteUrgent->username[0] = 0;
	g_pWriteUrgent->ldate = TimeToLong(m_time);
	strcpy(g_pWriteUrgent->source,m_strSource);
	strcpy(g_pWriteUrgent->buf,m_Content);

	CDialog::OnOK();
}
