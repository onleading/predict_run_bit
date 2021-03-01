#include "StdAfx.h"
#include "TList.h"
#define ROWHIGHT 25

void reverse(char *dotPos,char *end)
{
	while(dotPos < end)
	{
		char c = *dotPos;
		*dotPos = *end;
		*end = c;
		dotPos++,end--;
	}
}

char * dotPos(char *str,int len)
{
	char *p;
	return (p = strchr(str,'.') ) ? p - 1 : str + len -1;
}

char *sectionMark(char *dest,char *source,int len)
{
	char *iter = dotPos(source,len);
	char *q = dest;
	int i = 0;
	for(char *p = source + len - 1; p != source - 1; p--)
	{
		if ( p <= iter && ( !(i++ % 3) && i-1))
			*q++ = ',';
		*q++ = *p;
	}
	*q = 0;
	//TRACE("%s\n",dest);
	reverse(dest,q-1);
	return dest;
}

//形如DWORD time=20070112;
CTime DwTime2CTime(DWORD time)
{
	int day=time%100;
	int month=(time%10000-day)/100;
	int year=(time-month*100-day)/10000;
	CTime ctime(year,month,day,0,0,0);
	return ctime;
}


VList::VList(int nRows, int nCols, int nFixedRows, int nFixedCols, int nAutoLineWarp)
:CGridCtrl(nRows,nCols,nFixedRows,nFixedCols),m_nFixRow(1),m_nAutoLineWarp(nAutoLineWarp)
{
}

VList::~VList()
{
}

BOOL VList::Create(const RECT& rect, CWnd* parent, UINT nID, DWORD dwStyle)
{
	BOOL bRet=FALSE;
	bRet = CGridCtrl::Create(rect,parent,nID,dwStyle);

	SetHeaderSort();
	
	AutoSize();
	SetGridBkColor(RGB(0,0,0));
	
	GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0, 0, 0));
	GetDefaultCell(FALSE, FALSE)->SetTextClr(RGB(255, 255, 255));

	SetAlternationNoFixedCellClr(RGB(247,243,247),RGB(255,255,255));
	
	SetTrackFocusCell(FALSE);
	SetFrameFocusCell(FALSE);
	SetListMode(TRUE);
	SetEditable(FALSE);
	
	//SetFlateHead(TRUE);
	SetFixedColumnSelection(FALSE);
	SetRowResize(FALSE);
	SetArrowDisplayRow(0);

	EnableSelectFixedColumn();
	SetSelectedNoChangeTextClr(TRUE);
	SetIndependentTitleTip(TRUE);
	SetRowCount(2);
	SetFixedRowCount(1);

	SetRowHeight(0,ROWHIGHT);
	SetRowHeight(1,ROWHIGHT);

	return bRet;
}

int VList::InsertHead(sg_head *head, BOOL bRefresh)
{
	if(!head)
		return -1;

	m_Heads[m_Heads.size()]=*head;

	//showHead会调整m_Heads中col的顺序
	showHead();

	return m_Heads.size();
}

void VList::showHead()
{
	CString str;
	GV_ITEM Item;
	int i=0;
	string strTitle;
	string strTemp;
	string::size_type posStr;

	//如果有表头有'&'并且没有'|',那么nScale=2
	int nScale=1;

	//处理head中有|的复合表头
	//按照head的标题文字对m_Heads重新排序
	//过滤了相同标题的问题
	//通过map的key自动排序功能进行string的排序
	
	map<string,int> titles;//key保存的是title的字符串，valve保存的是对应m_Heads的col，相对于m_Heads的一个索引
	map<string,int>::iterator itTitle;
	map<int,sg_head>::iterator itHead;

	map<int,sg_head> temHeadMap=m_Heads;

	for (itHead = m_Heads.begin(); itHead!=m_Heads.end(); itHead++)
	{
		strTitle = itHead->second.title;
		titles[strTitle]=itHead->first;
		if(strTitle.find('|')!=string::npos)
			m_nFixRow=2;		
	}

	if(m_nFixRow==1)
	{
		for (itHead = m_Heads.begin(); itHead!=m_Heads.end(); itHead++)
		{
			strTitle = itHead->second.title;
			if(strTitle.find('&')!=string::npos)
				nScale=2;		
		}
	}

	m_Heads.clear();

	for (itTitle=titles.begin(),i=0; itTitle!=titles.end(); itTitle++,i++)
	{
		m_Heads[i]=temHeadMap[itTitle->second];
	}

	//保证在全部单元格删除后可以有效的插入col
	SetRowCount(m_nFixRow);
	SetFixedRowCount(m_nFixRow);
	SetArrowDisplayRow(m_nFixRow-1);
	for (i=0;i<m_nFixRow;i++)
	{
		SetRowHeight(i,nScale*ROWHIGHT);
	}

	SetColumnCount(m_Heads.size());

	//合并单元格，显示head

	for(itHead=m_Heads.begin(); itHead!=m_Heads.end(); itHead++)
	{
		for (i=0; i<m_nFixRow; i++)
		{
			Item.row = i;
			Item.col = itHead->first;
			Item.mask = GVIF_TEXT | GVIF_ALIGN;
			switch(itHead->second.headalign)
			{
			case SG_ALIGN_LEFT:
				Item.nTextAlign=DT_LEFT;
				break;
			case SG_ALIGN_RIGHT:
				Item.nTextAlign=DT_RIGHT;
				break;
			case SG_ALIGN_CENTER:
				Item.nTextAlign=DT_CENTER;
				break;
			default:
				break;
			}
			strTemp=itHead->second.title;
			posStr=strTemp.find('|');
			if(posStr!=string::npos)
			{
				if(Item.row==0)
				{
					strTemp=strTemp.substr(0,posStr);
				}
				else
				{
					strTemp=strTemp.substr(posStr+1);
				}
			}
			str.Format("%s",strTemp.c_str());
			Item.strText = str;
			SetItem(&Item);
		}
		
		switch(itHead->second.datatype)
		{
		case SG_TYPE_STRING:
			{
				SetColCompareFunction(itHead->first, pfnCellTextCompare);
			}
			break;
		case SG_TYPE_DOUBLE:
			{
				SetColCompareFunction(itHead->first, pfnCellFloatCompare);
			}
			break;
		case SG_TYPE_INT:
			{
				SetColCompareFunction(itHead->first, pfnCellNumericCompare);
			}
			break;
		default:
				SetColCompareFunction(itHead->first, pfnCellFloatCompare);
			break;
		}
	}

	for (i=0; i<m_Heads.size(); i++)
	{
		SetColumnWidth(i,100);
	}

	mergeFixedCellsBytitle();
}

BOOL VList::SetCellData(sg_cell cell, int col, int row)
{
	CString str;
	GV_ITEM Item;
	sg_head head;
	map<int,ROWCELL>::iterator itCol;
	ROWCELL::iterator itRow;
	ROWCELL rowCell;
	char buf1[256], buf2[256], buf3[256];
	CTime ctime;

	memset(buf1,0,256);
	memset(buf2,0,256);
	memset(buf3,0,256);

	row = row + GetFixedRowCount();

	if(GetColumnCount() <= col)
		return FALSE;

	if(!GetHeadData(head,col))
		return FALSE;

	if(GetRowCount() <= row)
	{
		SetRowCount(row+1);
		SetRowHeight(row,ROWHIGHT);
	}

	Item.row = row;
	Item.col = col;
	Item.crFgClr=cell.txtcolor;
	if(head.datatype==SG_TYPE_STRING && cell.stringdata.GetLength()>m_nAutoLineWarp)
	{
		Item.mask = GVIF_TEXT|GVIF_ALIGN|GVIF_FGCLR | GVIF_FORMAT;
		Item.nFormat = DT_EDITCONTROL|DT_WORDBREAK;
	}
	else
	{
		Item.mask = GVIF_TEXT|GVIF_ALIGN|GVIF_FGCLR;
	}

	switch(head.align)
	{
	case SG_ALIGN_LEFT:
		Item.nTextAlign=DT_LEFT;
		break;
	case SG_ALIGN_RIGHT:
		Item.nTextAlign=DT_RIGHT;
		break;
	case SG_ALIGN_CENTER:
		Item.nTextAlign=DT_CENTER;
		break;
	default:
	    break;
	}

	switch(head.datatype)
	{
	case SG_TYPE_STRING:
		{
			str.Format("%s",cell.stringdata);
		}
		break;
	case SG_TYPE_DOUBLE:
		{
			memset(buf1,0,256);
			memset(buf2,0,256);
			memset(buf3,0,256);
			if(head.decimaldigits<=0 || head.decimaldigits>=10)
				head.decimaldigits=6;
			
			sprintf(buf3,"%d",head.decimaldigits);

			str+="%0.";
			str+=buf3;
			str+="f";
			strcpy(buf3,str.GetBuffer(0));
			str.ReleaseBuffer();

			switch(head.formattype)
			{
			//千分位逗号分隔
			case SG_FORMAT_COMMA_SEP:
				sprintf(buf1,buf3,cell.data.doubledata);
				sectionMark(buf2,buf1,strlen(buf1));
				str.Format("%s",buf2);
				break;
			//半角百分号
			case SG_PERCENT_BANJIAO:
				str.Format(buf3,cell.data.doubledata);
				str+="%";
			    break;
			//全角百分号
			case SG_PERCENT_QUANJIAO:
				str.Format(buf3,cell.data.doubledata);
				str+="％";
			    break;
			//千分号
			case SG_ONE_THOUSAND:
				str.Format(buf3,cell.data.doubledata);
				str+="‰";
			    break;
			//显示为XXX.XX万
			case SG_FORMAT_WAN:
				str.Format("%0.2f万",(float)cell.data.doubledata/(float)10000);
			    break;
			//显示为XXX.XX亿
			case SG_FORMAT_YI:
				str.Format("%0.2f亿",(float)cell.data.doubledata/(float)100000000);
			    break;
			//XXX,XXX,XXX,XXX.XX万
			case SG_FORMAT_COMMA_SEP_WAN:
				sprintf(buf1,"%0.2f",(float)cell.data.doubledata/(float)10000);
				sectionMark(buf2,buf1,strlen(buf1));
				str.Format("%s万",buf2);
			    break;
			//XXX,XXX,XXX,XXX.XX亿
			case SG_FORMAT_COMMA_SEP_YI:
				sprintf(buf1,"%0.2f",(float)cell.data.doubledata/(float)100000000);
				sectionMark(buf2,buf1,strlen(buf1));
				str.Format("%s亿",buf2);
			    break;
			//自动万亿表示法
			case SG_FORMAT_WAN_YI:
				if(cell.data.intdata>=100000000)
				{
					str.Format("%0.2f亿",(float)cell.data.doubledata/(float)100000000);
				}
				else
				{
					str.Format("%0.2f万",(float)cell.data.doubledata/(float)10000);
				}
			    break;
			//带千分位的自动万亿表示法
			case SG_FORMAT_COMMA_SEP_WAN_YI:
				if(cell.data.intdata>=100000000)
				{
					sprintf(buf1,"%0.2f",(float)cell.data.doubledata/(float)100000000);
					sectionMark(buf2,buf1,strlen(buf1));
					str.Format("%s亿",buf2);
				}
				else
				{
					sprintf(buf1,"%0.2f",(float)cell.data.doubledata/(float)10000);
					sectionMark(buf2,buf1,strlen(buf1));
					str.Format("%s万",buf2);
				}
			    break;
			default:
				str.Format(buf3,cell.data.doubledata);//同SG_FORMAT_NORMAL=0 不作任何处理
			    break;
			}
		}
	    break;
	case SG_TYPE_INT:
		{
			memset(buf1,0,256);
			memset(buf2,0,256);
			switch(head.formattype)
			{
			//千分位逗号分隔
			case SG_FORMAT_COMMA_SEP:
				sprintf(buf1,"%d",cell.data.intdata);
				sectionMark(buf2,buf1,strlen(buf1));
				str.Format("%s",buf2);
				break;
			//半角百分号
			case SG_PERCENT_BANJIAO:
				str.Format("%d",cell.data.intdata);
				str+="%";
			    break;
			//全角百分号
			case SG_PERCENT_QUANJIAO:
				str.Format("%d％",cell.data.intdata);
			    break;
			//千分号
			case SG_ONE_THOUSAND:
				str.Format("%d‰",cell.data.intdata);
			    break;
			//显示为XXX.XX万
			case SG_FORMAT_WAN:
				str.Format("%0.2f万",(float)cell.data.intdata/(float)10000);
			    break;
			//显示为XXX.XX亿
			case SG_FORMAT_YI:
				str.Format("%0.2f亿",(float)cell.data.intdata/(float)100000000);
			    break;
			//XXX,XXX,XXX,XXX.XX万
			case SG_FORMAT_COMMA_SEP_WAN:
				sprintf(buf1,"%0.2f",(float)cell.data.intdata/(float)10000);
				sectionMark(buf2,buf1,strlen(buf1));
				str.Format("%s万",buf2);
			    break;
			//XXX,XXX,XXX,XXX.XX亿
			case SG_FORMAT_COMMA_SEP_YI:
				sprintf(buf1,"%0.2f",(float)cell.data.intdata/(float)100000000);
				sectionMark(buf2,buf1,strlen(buf1));
				str.Format("%s亿",buf2);
			    break;
			//自动万亿表示法
			case SG_FORMAT_WAN_YI:
				if(cell.data.intdata>=100000000)
				{
					str.Format("%0.2f亿",(float)cell.data.intdata/(float)100000000);
				}
				else
				{
					str.Format("%0.2f万",(float)cell.data.intdata/(float)10000);
				}
			    break;
			//带千分位的自动万亿表示法
			case SG_FORMAT_COMMA_SEP_WAN_YI:
				if(cell.data.intdata>=100000000)
				{
					sprintf(buf1,"%0.2f",(float)cell.data.intdata/(float)100000000);
					sectionMark(buf2,buf1,strlen(buf1));
					str.Format("%s亿",buf2);
				}
				else
				{
					sprintf(buf1,"%0.2f",(float)cell.data.intdata/(float)10000);
					sectionMark(buf2,buf1,strlen(buf1));
					str.Format("%s万",buf2);
				}
			    break;
			//斜杠分隔时间yyyy/mm/dd
			case SG_DATE_SLASH:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%Y/%m/%d");
			    break;
			//斜杠分隔时间mm/dd/yyyy
			case SG_DATE_SLASH2:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%m/%d/%Y");
			    break;
			//短横线分隔时间yyyy-mm-dd
			case SG_DATE_SHORTLINE:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%Y-%m-%d");
			    break;
			//YYYYMMDD
			case SG_DATE_NORMAL:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%Y%m%d");
			    break;
			//YY/MM/DD
			case SG_DATE_SLASH_SHORT:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%y/%m/%d");
			    break;
			//MM/DD/YY
			case SG_DATE_SLASH2_SHORT:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%m/%d/%y");
			    break;
			//YY-MM-DD
			case SG_DATE_SHORTLINE_SHORT:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%y-%m-%d");
			    break;
			//YYMMDD
			case SG_DATE_NORMAL_SHORT:
				ctime=DwTime2CTime(cell.data.intdata);
				str=ctime.Format("%y%m%d");
			    break;
			default:
				str.Format("%d",cell.data.intdata);//同SG_FORMAT_NORMAL=0 不作任何处理
			    break;
			}
		}
		break;
	default:
	    break;
	}

	Item.strText = str;
	SetItem(&Item);

	itCol=m_Cells.find(col);
	if(itCol==m_Cells.end())
	{
		rowCell[row]=cell;
		m_Cells[col]=rowCell;
	}
	else
	{
		(itCol->second)[row]=cell;
	}

	return TRUE;
}

BOOL VList::GetCellData(sg_cell& cell, int col, int row)
{
	map<int,ROWCELL>::iterator itCol;
	ROWCELL::iterator itRow;

	itCol=m_Cells.find(col);
	if(itCol!=m_Cells.end())
	{
		itRow=itCol->second.find(row);
		if(itRow!=itCol->second.end())
		{
			cell=itRow->second;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL VList::GetHeadData(sg_head& head, int pos)
{
	map<int,sg_head>::iterator it;

	if(m_Heads.size()<=pos)
		return FALSE;

	it=m_Heads.find(pos);
	if(it!=m_Heads.end())
	{
		head = it->second;
		return TRUE;
	}
	return FALSE;
}

void VList::mergeFixedCellsBytitle()
{
	map<int,sg_head>::iterator itHead, itHeadNext;

	int colMergeBegin=0, colMergeEnd=0;
	itHead=m_Heads.begin();
	string curTitleTop;//title竖杠前面的部分
	string strTemp;

	//itHeadNext的肯定要比itHead title大，因为已经排过顺序了。可以应对 open  open|title 这2个标题的问题

	UnMergeAllFixedCells();

	while (itHead!=m_Heads.end())
	{
		strTemp=itHead->second.title;
		if(strTemp.find('|')==string::npos)
		{
			MergeFixedCells(0,colMergeBegin,m_nFixRow-1,colMergeEnd,m_nFixRow-1,colMergeEnd);
			colMergeEnd++;
			colMergeBegin=colMergeEnd;
			itHead++;
			curTitleTop="";
			continue;
		}
		
		if(!curTitleTop.size())
		{
			curTitleTop=itHead->second.title;
			curTitleTop=curTitleTop.substr(0,curTitleTop.find_first_of('|'));
		}

		itHeadNext=itHead;
		itHeadNext++;
		while(itHeadNext!=m_Heads.end())
		{
			strTemp=itHeadNext->second.title;
			strTemp=strTemp.substr(0,strTemp.find_first_of('|'));
			if(strTemp!=curTitleTop)
			{
				curTitleTop=strTemp;
				break;
			}
			else
			{
				colMergeEnd++;
			}
			itHeadNext++;
		}
		if(colMergeEnd!=colMergeBegin)
			MergeFixedCells(0,colMergeBegin,0,colMergeEnd,0,colMergeEnd);
		
		colMergeEnd++;
		colMergeBegin=colMergeEnd;
		itHead=itHeadNext;
		curTitleTop="";
	}
}