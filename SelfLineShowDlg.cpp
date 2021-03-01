// SelfLineShowDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SelfLineShowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelfLineShowDlg dialog


CSelfLineShowDlg::CSelfLineShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelfLineShowDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelfLineShowDlg)
	//}}AFX_DATA_INIT
	m_aDrawLine	= NULL;
	m_nDrawLine = 0;

	LPDRAWLINE pDrawLine;
	long lDrawLine = g_DrawLineTool.GetAllLine(pDrawLine);

	if(lDrawLine>0)
	{
		BOOL bSLRet = TRUE;
		LPDRAWLINE tmpDL = NULL;
		long tmpSpace = 0;
		CHECKSELFLINESPACE(bSLRet,m_aDrawLine,tmpDL,tmpSpace,lDrawLine,SELFLINESTEP,DRAWLINE);
		if(bSLRet)
		{
			memset(m_aDrawLine,0,tmpSpace*sizeof(DRAWLINE));
			memcpy(m_aDrawLine,pDrawLine,lDrawLine*sizeof(DRAWLINE));
			m_nDrawLine = lDrawLine;
		}
		else m_nDrawLine = 0;
	}
}

CSelfLineShowDlg::~CSelfLineShowDlg()
{
	TDELA(m_aDrawLine);
}

void CSelfLineShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelfLineShowDlg)
	DDX_Control(pDX, IDC_LIST_SELFLINE, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelfLineShowDlg, CDialog)
	//{{AFX_MSG_MAP(CSelfLineShowDlg)
	ON_BN_CLICKED(IDC_DELLINE, OnDelline)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelfLineShowDlg message handlers

void CSelfLineShowDlg::OnDelline() 
{
	int i=0,j=0;
	int nSelNo = 0;
	int nItemNo = 0;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"�����б��н���ѡ��!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	nSelNo = m_List.GetNextSelectedItem(pos);
	if(nSelNo<0 || nSelNo>=m_List.GetItemCount()) return;
	nItemNo = m_List.GetItemData(nSelNo);

	if(nItemNo<m_nDrawLine-1)
		memmove(&(m_aDrawLine[nItemNo]),&(m_aDrawLine[nItemNo+1]),(m_nDrawLine-1-nItemNo)*sizeof(DRAWLINE));
	m_nDrawLine--;
	memset(&(m_aDrawLine[m_nDrawLine]),0,sizeof(DRAWLINE));
	
	FillList();
	if(nSelNo>=m_List.GetItemCount()) 
		nSelNo = m_List.GetItemCount()-1;
	if(nSelNo>=0)
		m_List.SetItemState(nSelNo,LVIS_SELECTED,LVIS_SELECTED);
}

void CSelfLineShowDlg::OnOK() 
{
	if(m_aDrawLine)
	{
		g_DrawLineTool.SetAllLine(m_aDrawLine,m_nDrawLine);
		g_DrawLineTool.SaveLine();
	}
	CDialog::OnOK();
}

void CSelfLineShowDlg::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL CSelfLineShowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CImageList tmpImageList;
	tmpImageList.Create(1,16,ILC_COLOR8|ILC_MASK,1,1);
	m_List.SetImageList(&tmpImageList,LVSIL_SMALL);
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.pszText = "֤ȯ";
	lvColumn.cx = 60;
	lvColumn.iSubItem = 0;
	lvColumn.fmt = LVCFMT_LEFT;
	m_List.InsertColumn (0,&lvColumn);
	lvColumn.pszText = "ָ��";
	lvColumn.cx = 120;
	lvColumn.iSubItem = 1;
	m_List.InsertColumn (1,&lvColumn);
	lvColumn.pszText = "����";
	lvColumn.cx = 60;
	lvColumn.iSubItem = 2;
	m_List.InsertColumn (2,&lvColumn);
	lvColumn.pszText = "����";
	lvColumn.cx = 80;
	lvColumn.iSubItem = 3;
	m_List.InsertColumn (3,&lvColumn);
	lvColumn.pszText = "��λֵ";
	lvColumn.cx = 320;
	lvColumn.iSubItem = 4;
	m_List.InsertColumn (4,&lvColumn);

	FillList();
	
	return TRUE;
}

void CSelfLineShowDlg::FillList()
{
	int i=0,j=0;
	int nItem = 0;
	int nPoint= 0;
	CString tmpStr1,tmpStr2;
	long	lDate;
	union long_short lMinute;
	float	fVal;
	m_List.SetRedraw(FALSE);
	m_List.DeleteAllItems();
	for(i=0;i<m_nDrawLine;i++)
	{
		if(m_aDrawLine[i].SetCode==SZ) tmpStr1.Format("SZ%s",m_aDrawLine[i].Code);
		else if(m_aDrawLine[i].SetCode==SH) tmpStr1.Format("SH%s",m_aDrawLine[i].Code);
		else if(m_aDrawLine[i].SetCode>2) tmpStr1.Format("DS%s",m_aDrawLine[i].Code);
		else tmpStr1 = "�޷�ʶ��";
		nItem = m_List.InsertItem(i,tmpStr1,0);
		m_List.SetItemData(nItem,i);
		if(strnicmp(m_aDrawLine[i].acCode,"STANDK",9)==0) m_List.SetItemText(nItem,1,"STANDK(K��)");
		else if(strnicmp(m_aDrawLine[i].acCode,"ZST_PX",9)==0) m_List.SetItemText(nItem,1,"ZST_PX(��ʱ����)");
		else m_List.SetItemText(nItem,1,m_aDrawLine[i].acCode);
		if(m_aDrawLine[i].nPeriod==LINEPERIOD_ZST)
			m_List.SetItemText(nItem,2,"��ʱ����");
		else m_List.SetItemText(nItem,2,GetTdxPeriodStr(m_aDrawLine[i].nPeriod,"�޷�ʶ��"));
		if(m_aDrawLine[i].bDelete) 
			m_List.SetItemText(nItem,3,"�Ѿ�ɾ��");
		else
		{
			switch(m_aDrawLine[i].nWay)
			{
			case ID_LDXD:		m_List.SetItemText(nItem,3,"�߶�");			nPoint = 2;	break;
			case ID_LDZX:		m_List.SetItemText(nItem,3,"ֱ��");			nPoint = 2;	break;
			case ID_ARROW:		m_List.SetItemText(nItem,3,"��ͷ");			nPoint = 2;	break;
			case ID_RAY:		m_List.SetItemText(nItem,3,"����");			nPoint = 2;	break;
			case ID_PXX:		m_List.SetItemText(nItem,3,"�۸�ͨ����");	nPoint = 3;	break;
			case ID_PXZX:		m_List.SetItemText(nItem,3,"ƽ��ֱ��");		nPoint = 3;	break;
			case ID_YHX:		m_List.SetItemText(nItem,3,"Բ����");		nPoint = 2;	break;
			case ID_GOLDPRICE:	m_List.SetItemText(nItem,3,"�ƽ��λ��");	nPoint = 2;	break;
			case ID_GOLDAIM:	m_List.SetItemText(nItem,3,"�ƽ�Ŀ����");	nPoint = 3;	break;
			case ID_GOLD:		m_List.SetItemText(nItem,3,"�ƽ�ָ�");		nPoint = 2;	break;
			case ID_DFBX:		m_List.SetItemText(nItem,3,"�ٷֱ���");		nPoint = 2;	break;
			case ID_BDX:		m_List.SetItemText(nItem,3,"������");		nPoint = 2;	break;
			case ID_XXHGD:		m_List.SetItemText(nItem,3,"���Իع��");	nPoint = 2;	break;
			case ID_XXHGC:		m_List.SetItemText(nItem,3,"�ӳ��ع��");	nPoint = 2;	break;
			case ID_XXHG:		m_List.SetItemText(nItem,3,"���Իع�");		nPoint = 2;	break;
			case ID_ZQX:		m_List.SetItemText(nItem,3,"������");		nPoint = 2;	break;
			case ID_FBLQ:		m_List.SetItemText(nItem,3,"�Ѳ�������");	nPoint = 2;	break;
			case ID_GANNTIME:	m_List.SetItemText(nItem,3,"����ʱ����");	nPoint = 2;	break;
			case ID_ZSX:		m_List.SetItemText(nItem,3,"������");		nPoint = 2;	break;
			case ID_SSGS:		m_List.SetItemText(nItem,3,"�����Ƕ���");	nPoint = 2;	break;
			case ID_ZX:			m_List.SetItemText(nItem,3,"����");			nPoint = 2;	break;
			case ID_UPFLAG:		m_List.SetItemText(nItem,3,"�Ǳ��");		nPoint = 1;	break;
			case ID_DOWNFLAG:	m_List.SetItemText(nItem,3,"�����");		nPoint = 1;	break;
			case ID_TXTTOOLS:	m_List.SetItemText(nItem,3,"����ע��");		nPoint = 1;	break;
			default:			m_List.SetItemText(nItem,3,"�޷�ʶ��");		nPoint = 0;	break;
			}
			tmpStr1 = "";
			for(j=0;j<nPoint;j++)
			{
				if(j==0)
				{
					lDate	= m_aDrawLine[i].TimeS;
					fVal	= m_aDrawLine[i].fPriceS;
				}
				else if(j==1)
				{
					lDate	= m_aDrawLine[i].TimeE;
					fVal	= m_aDrawLine[i].fPriceE;
				}
				else
				{
					lDate	= m_aDrawLine[i].TimeR[j-2];
					fVal	= m_aDrawLine[i].fPriceR[j-2];
				}
				
				memcpy(&lMinute,&lDate,sizeof(long));
				switch(m_aDrawLine[i].nPeriod)
				{
				case PER_MIN1:
				case PER_MIN5:
				case PER_MIN15:
				case PER_MIN30:
				case PER_HOUR:
				case PER_MINN:
					tmpStr2.Format("��λ%d:ֵ:%.3f/ʱ:%04d/%02d:%02d;",(j+1),fVal,lMinute.Daye.Day,lMinute.Daye.Minute/60,lMinute.Daye.Minute%60);
					break;
				case PER_DAY:
				case PER_WEEK:
				case PER_MONTH:
				case PER_DAYN:
				case PER_SEASON:
				case PER_YEAR:
					tmpStr2.Format("��λ%d:ֵ:%.3f/ʱ:%d;",(j+1),fVal,lMinute.Date);
					break;
				case LINEPERIOD_ZST:
					tmpStr2.Format("��λ%d:ֵ:%.3f/ʱ:%d;",(j+1),fVal,lMinute.Date);
					break;
				default:
					break;
				}
				tmpStr1 += tmpStr2;
			}
			m_List.SetItemText(nItem,4,tmpStr1);
		}
	}
	m_List.SetRedraw(TRUE);
}
