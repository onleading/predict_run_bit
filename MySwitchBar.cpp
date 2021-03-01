// MySwitchBar.cpp : implementation file
//

#include "stdafx.h"
#include "MySwitchBar.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySwitchBar
CMySwitchBar::CMySwitchBar()
{
	memset(m_BK,0,sizeof(m_BK));
}

CMySwitchBar::CMySwitchBar(UINT id,MySwitchBarStyle style/* =MSBS_H_T */,int marge_left/* =0 */,int marge_top/* =0 */)
{
	Init(id,style,marge_left,marge_top);
}

void CMySwitchBar::Init(UINT id,MySwitchBarStyle style/* =MSBS_H_T */,int marge_left/* =0 */,int marge_top/* =0 */)
{
	m_nID=id;
	m_style=style;
	m_item_offset=1;
	m_level_offset=0;
	memset(m_BK,0,sizeof(m_BK));
	m_cor_text_n=RGB(0,0,0);
	m_cor_text_d=RGB(0,0,0);
	m_cor_text_o=RGB(0,0,0);
	m_text_style_n=TS_Normal;
	m_text_style_d=TS_Normal;
	m_text_style_o=TS_Normal;
	m_text_height_n=12;
	m_text_height_d=12;
	m_text_height_o=12;
	m_pfOwnerDraw=NULL;
	m_offset[0]=m_offset[1]=m_offset[2]=CPoint(0,0);
	m_nIndex_sel=-1;
	m_nRedraw=0;
	m_bBatchCalc=FALSE;
	//设置边缘留白大小
	m_marge_left=marge_left;
	m_marge_top=marge_top;

	m_EraseBkgndStyle=MSB_EBS_GRADIENT;
	m_cor_bk=RGB(192,192,192);
	m_startColor=m_endColor=RGB(192,192,192);
	m_bHorizontal=FALSE;

	m_pageBt[0].Init(BT_PAGE_UP,"",NormalBt,0,TRUE);
	m_pageBt[1].Init(BT_PAGE_DOWN,"",NormalBt,0,TRUE);
	
	m_bt_rect[0].SetRectEmpty();
	m_bt_rect[1].SetRectEmpty();
	m_btsRect.SetRectEmpty();
	m_nCurPage=-1;
}

CMySwitchBar::~CMySwitchBar()
{
	int n=m_vData.size();

	for(int i=0;i<n;i++)
		TDEL(m_vData[i]);
	m_vData.clear();

	for(i=0;i<12;i++)
	{
		if(m_BK[i])
		{
			m_BK[i]->DeleteObject();
			TDEL(m_BK[i]);
		}
	}
}


BEGIN_MESSAGE_MAP(CMySwitchBar, CWnd)
	//{{AFX_MSG_MAP(CMySwitchBar)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_MYBITMAPBUTTON,OnClick)
END_MESSAGE_MAP()

void CMySwitchBar::SetEraseBackGroundParam(COLORREF StartColor,COLORREF EndColor,BOOL bHorizontal)
{
	m_EraseBkgndStyle=MSB_EBS_GRADIENT;
	m_startColor=StartColor;
	m_endColor=EndColor;
	m_bHorizontal=bHorizontal;
}

void CMySwitchBar::SetEraseBackGroundParam(BOOL bOwnerDraw)
{
	m_EraseBkgndStyle=bOwnerDraw?MSB_EBS_OWNERDRAW:MSB_EBS_BITMAP;
}

void CMySwitchBar::SetBK(long bk[12])
{
	for(int i=0;i<12;i++)
	{
		TDEL(m_BK[i]);
		if(bk[i])
		{
			m_BK[i]=new CBitmap;
			if(!m_BK[i]->LoadBitmap(bk[i]))
				TDEL(m_BK[i]);
		}
	}
	int n=m_vData.size();
	for (i=0;i<n;i++)
	{
		if(m_vData[i]->pBt)
			m_vData[i]->pBt->SetBK(m_BK);
	}
}
void CMySwitchBar::SetBK(LPCTSTR bk[12])
{
	for(int i=0;i<12;i++)
	{
		TDEL(m_BK[i]);
		if(bk[i])
		{
			m_BK[i]=new CBitmap;
			if(!m_BK[i]->LoadBitmap(bk[i]))
				TDEL(m_BK[i]);
		}
	}
	int n=m_vData.size();
	for (i=0;i<n;i++)
	{
		if(m_vData[i]->pBt)
			m_vData[i]->pBt->SetBK(m_BK);
	}
}

void CMySwitchBar::SetBK(CBitmap* bk[12])
{
	for(int i=0;i<12;i++)
	{
		TDEL(m_BK[i]);
		if(!bk[i]) continue;
		m_BK[i]=new CBitmap;
		BITMAP tempbitmap;
		bk[i]->GetBitmap(&tempbitmap);
		long tbuffsize=tempbitmap.bmWidthBytes*tempbitmap.bmHeight;
		tempbitmap.bmBits=new BYTE[tbuffsize];
		bk[i]->GetBitmapBits(tbuffsize,tempbitmap.bmBits);
		m_BK[i]->CreateBitmapIndirect(&tempbitmap);
		TDEL(tempbitmap.bmBits);
	}
	int n=m_vData.size();
	for (i=0;i<n;i++)
	{
		if(m_vData[i]->pBt)
			m_vData[i]->pBt->SetBK(m_BK);
	}
}

void CMySwitchBar::SetButtonBK(long bk[6])
{
	for(int i=0;i<2;i++)
	{
		int j=i*3;
		long tmpbk[9]={0,0,bk[j],
			0,0,bk[j+1],
			0,0,bk[j+2]
		};
		m_pageBt[i].SetBK(tmpbk);
		CSize sz=g_GUI.GetBitmapSize(bk[j]);
		m_bt_rect[i].SetRect(0,0,sz.cx,sz.cy);
	}
}

void CMySwitchBar::SetButtonBK(LPCTSTR bk[6])
{
	for(int i=0;i<2;i++)
	{
		int j=i*3;
		LPCTSTR tmpbk[9]={0,0,bk[j],
			0,0,bk[j+1],
			0,0,bk[j+2]
		};
		m_pageBt[i].SetBK(tmpbk);
		CSize sz=g_GUI.GetBitmapSize(bk[j]);
		m_bt_rect[i].SetRect(0,0,sz.cx,sz.cy);
	}
}

void CMySwitchBar::SetButtonBK(CBitmap* bk[6])
{
	for(int i=0;i<2;i++)
	{
		int j=i*3;
		CBitmap* tmpbk[9]={0,0,bk[j],
			0,0,bk[j+1],
			0,0,bk[j+2]
		};
		m_pageBt[i].SetBK(tmpbk);
		CSize sz=g_GUI.GetBitmapSize(bk[j]);
		m_bt_rect[i].SetRect(0,0,sz.cx,sz.cy);
	}
}

void CMySwitchBar::SetBkColor(COLORREF cor_bk,COLORREF startColor,COLORREF endColor)
{
	m_cor_bk = cor_bk;
	m_startColor = startColor;
	m_endColor = endColor;
}

void CMySwitchBar::SetTextColor(COLORREF cor_text_n,COLORREF cor_text_d,COLORREF cor_text_o,int nIndex)
{
	int n=m_vData.size();
	if(nIndex>=n) return;
	if(nIndex<0)
	{
		m_cor_text_n=cor_text_n;
		m_cor_text_d=cor_text_d;
		m_cor_text_o=cor_text_o;
		for (int i=0;i<n;i++)
		{
			if(m_vData[i]->pBt)
				m_vData[i]->pBt->SetTextCor(m_cor_text_n,m_cor_text_d,m_cor_text_o);
		}
	}
	else
	{
		if(m_vData[nIndex]->pBt)
			m_vData[nIndex]->pBt->SetTextCor(cor_text_n,cor_text_d,cor_text_o);
	}
}

void CMySwitchBar::SetTextStyle(TextStyle style_n,TextStyle style_d,TextStyle style_o,int nIndex)
{
	int n=m_vData.size();
	if(nIndex>=n) return;
	if(nIndex<0)
	{
		m_text_style_n=style_n;
		m_text_style_d=style_d;
		m_text_style_o=style_o;
		for (int i=0;i<n;i++)
		{
			if(m_vData[i]->pBt)
				m_vData[i]->pBt->SetTextStyle(m_text_style_n,m_text_style_d,m_text_style_o);
		}
	}
	else
	{
		if(m_vData[nIndex]->pBt)
				m_vData[nIndex]->pBt->SetTextStyle(style_n,style_d,style_o);
	}
}

void CMySwitchBar::SetTextHeight(int height_n,int height_d,int height_o,int nIndex)
{
	int n=m_vData.size();
	if(nIndex>=n) return;
	if(nIndex<0)
	{
		m_text_height_n=height_n;
		m_text_height_d=(height_d==-1)?height_n:height_d;
		m_text_height_o=(height_o==-1)?height_n:height_o;
		for (int i=0;i<n;i++)
		{
			if(m_vData[i]->pBt)
				m_vData[i]->pBt->SetTextHeight(m_text_height_n,m_text_height_d,m_text_height_o);
		}
	}
	else
	{
		if(m_vData[nIndex]->pBt)
			m_vData[nIndex]->pBt->SetTextHeight(height_n,height_d,height_o);
	}
}

void CMySwitchBar::SetTextOffset(int x_n,int y_n,int x_d,int y_d,int x_o,int y_o,int nIndex)
{
	int n=m_vData.size();
	if(nIndex>=n) return;
	if(nIndex<0)
	{
		m_offset[0].x=x_n;
		m_offset[0].y=y_n;
		m_offset[1].x=x_d;
		m_offset[1].y=y_d;
		m_offset[2].x=x_o;
		m_offset[2].y=y_o;
		for(int i=0;i<n;i++)
		{
			if(m_vData[i]->pBt)
				m_vData[i]->pBt->SetTextOffset(x_n,y_n,x_d,y_d,x_o,y_o);
		}
	}
	else
	{
		if(m_vData[nIndex]->pBt)
			m_vData[nIndex]->pBt->SetTextOffset(x_n,y_n,x_d,y_d,x_o,y_o);
	}
}

void CMySwitchBar::SetCustomFunc(MSB_OwnerDraw pFunc)
{
	m_pfOwnerDraw=pFunc;
	int n=m_vData.size();
	for (int i=0;i<n;i++)
	{
		if(m_vData[i]->pBt)
			m_vData[i]->pBt->SetCustomFunc(m_pfOwnerDraw);
	}
}

MyBitmapBt* CMySwitchBar::_CreateNewBt(UINT id,LPCSTR text,LPCSTR tooltip)
{
	//不允许有重复ID的按钮出现
	int n=m_vData.size();
	BOOL bExist=FALSE;
	for(int i=0;i<n;i++)
	{
		if(m_vData[i]->nID==id)
		{
			bExist=TRUE;
			break;
		}
	}
	if(bExist) return NULL;

	MyBitmapBt *pNewBt=(MyBitmapBt *)new MyBitmapBt(id,text,SwitchLabelBt,DT_VCENTER|DT_CENTER,TRUE);
	if(!pNewBt) return NULL;
	pNewBt->SetBK(m_BK);
	pNewBt->SetTooltip(tooltip);
	pNewBt->SetTextCor(m_cor_text_n,m_cor_text_d,m_cor_text_o);
	pNewBt->SetTextHeight(m_text_height_n,m_text_height_d,m_text_height_o);
	pNewBt->SetTextOffset(m_offset[0].x,m_offset[0].y,m_offset[1].x,m_offset[1].y,m_offset[2].x,m_offset[2].y);	
	pNewBt->SetTextStyle(m_text_style_n,m_text_style_d,m_text_style_o);

	pNewBt->SetCustomFunc(m_pfOwnerDraw);
	CRect rc,rcWnd;
	GetClientRect(&rcWnd);
	UINT height,width;
	switch(m_style)
	{
	case MSBS_V_L:
	case MSBS_V_R:
		{
			width=rcWnd.Width()-m_marge_left;
			height=g_GUI.GetBitmapSize(m_BK[2]).cy;
		}
		break;
	case MSBS_H_T:
	case MSBS_H_B:
		{
			CDC dc;
			dc.CreateDC("DISPLAY",NULL,NULL,NULL);
			CFont NewFont;
			pNewBt->GetTextFont(NewFont);
			CFont *OldFont = dc.SelectObject(&NewFont);
			width=dc.GetTextExtent(text,strlen(text)).cx+4;
			dc.SelectObject(OldFont);
			NewFont.DeleteObject();
			dc.DeleteDC();
			width+=g_GUI.GetBitmapSize(m_BK[0]).cx;
			width+=g_GUI.GetBitmapSize(m_BK[1]).cx;
			height=rcWnd.Height()-m_marge_top;
		}
		break;
	}

	rc.SetRect(0,0,width,height);
	if(!(pNewBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,id)))
		TDEL(pNewBt);
	return pNewBt;
}

pMySwitchBarData CMySwitchBar::_CreateNewItem(UINT id,LPCSTR text,LPCSTR tooltip,LONG level)
{
	MyBitmapBt *pbt=_CreateNewBt(id,text,tooltip);
	if(!pbt) return NULL;
	pMySwitchBarData pData=(MySwitchBarData*)new MySwitchBarData();
	if(!pData)
	{
		pbt->DestroyWindow();
		TDEL(pbt);		
	}
	pData->nID=id;
	pData->pBt=pbt;
	pData->level=level;
	pData->pBt->GetOriginalWndRgn(pData->rgn);
	return pData;
}

BOOL CMySwitchBar::_InsertSwitch(pMySwitchBarData newItem,int nIndex)
{
	if(!newItem || !newItem->pBt) return FALSE;
	int n=m_vData.size();
	if(nIndex<0 || nIndex>n)
		nIndex=n;

	newItem->pBt->GetClientRect(newItem->rect);
	int x=0,y=0,offset_x=0,offset_y=0;
	switch(m_style)
	{
	case MSBS_H_T:
	case MSBS_H_B:
		{
			if(n>0)
				x=m_vData[nIndex-1]->rect.right-m_item_offset;
			else
				x=m_marge_left;
			y=m_marge_top;
			offset_x=newItem->rect.Width()-m_item_offset;
		}
		break;
	case MSBS_V_L:
		{
			if(newItem->level==0)
				x=m_marge_left;
			else 
			{
				x=m_level_offset+m_marge_left;
				//目标矩形
				CRect rect(newItem->rect);
				rect.SetRect(rect.right-m_level_offset,rect.top,rect.right,rect.bottom);
				newItem->rect.right-=m_level_offset;
				//rgn 目标区域 rgn2 镂空区域
				CRgn rgn,rgn2;
				rgn.CreateRectRgnIndirect(rect);
				rgn2.CreateRectRgnIndirect(rect);
				rgn.CombineRgn(&rgn,&newItem->rgn,RGN_AND);
				rgn.OffsetRgn(-m_level_offset,0);
				rgn2.OffsetRgn(-m_level_offset,0);
				newItem->rgn.CombineRgn(&newItem->rgn,&rgn2,RGN_DIFF); //镂空
				newItem->rgn.CombineRgn(&newItem->rgn,&rgn,RGN_OR); //补上
			}
			y=(nIndex==0)?m_marge_top:m_vData[nIndex-1]->rect.bottom-m_item_offset;
			offset_y=newItem->rect.Height()-m_item_offset;
		}
		break;
	case MSBS_V_R:
		{
			y=(nIndex==0)?m_marge_top:m_vData[nIndex-1]->rect.bottom-m_item_offset;
			x=m_marge_left;
			if(newItem->level>0)
			{
				//目标巨型
				CRect rect(newItem->rect);
				rect.SetRect(rect.right-m_level_offset,rect.top,rect.right,rect.bottom);
				newItem->rect.right-=m_level_offset;
				//rgn  目标区域 rgn2 镂空区域
				CRgn rgn,rgn2;
				rgn.CreateRectRgnIndirect(rect);
				rgn2.CreateRectRgnIndirect(rect);
				rgn.CombineRgn(&rgn,&newItem->rgn,RGN_AND);
				rgn.OffsetRgn(-m_level_offset,0);
				newItem->rgn.CombineRgn(&newItem->rgn,&rgn2,RGN_DIFF); //镂空
				rgn2.OffsetRgn(-m_level_offset,0);
				newItem->rgn.CombineRgn(&newItem->rgn,&rgn2,RGN_DIFF); //镂空
				newItem->rgn.CombineRgn(&newItem->rgn,&rgn,RGN_OR); //补上
			}
			offset_y=newItem->rect.Height()-m_item_offset;
		}
		break;
	}

	_SetMyRedraw(FALSE);
	
	newItem->rect.OffsetRect(-newItem->rect.left+x,-newItem->rect.top+y);
	newItem->pBt->MoveWindow(newItem->rect);

	for(int i=nIndex;i<n;i++)
	{
		m_vData[i]->rect.OffsetRect(offset_x,offset_y);
		m_vData[i]->pBt->MoveWindow(m_vData[i]->rect);
	}	

	int tmp=_GetIDFromIndex(m_nIndex_sel);
	m_vData.insert(m_vData.begin()+nIndex,newItem);
	if(n==0)
	{
		m_nIndex_sel=0;
		m_vData[0]->pBt->SetSel(TRUE);
	}
	else
	{
		m_nIndex_sel=_GetIndexFromID(tmp);
	}

	//若正在批量添加则先暂不计算
	if(!m_bBatchCalc)
	{
		CalcPage();
		CalcRgn();
		ShowPageButtons();
		MoveAndShow();
	}
	_SetMyRedraw(TRUE);
	
	return TRUE;
}
void CMySwitchBar::_SetMyRedraw(BOOL bRedraw)
{
	m_nRedraw+=bRedraw?1:-1;
	if(m_nRedraw>=0)
	{
		SetRedraw(TRUE);
		Invalidate();
	}
	else
		SetRedraw(FALSE);
}
void CMySwitchBar::SetMyRedraw(BOOL bRedraw)
{
	if(!bRedraw)
	{
		_SetMyRedraw(FALSE);
		m_bBatchCalc=TRUE;
	}
	else
	{
		CalcPage();
		CalcRgn();
		ShowPageButtons();
		MoveAndShow();
		_SetMyRedraw(TRUE);
		m_bBatchCalc=FALSE;
	}
}

BOOL CMySwitchBar::InsertSwitch(int nIndex,UINT id,LPCSTR text,LPCSTR tooltip,LONG level)
{
	pMySwitchBarData newItem=_CreateNewItem(id,text,tooltip,level);
	if(!newItem) return FALSE;
	if(!_InsertSwitch(newItem,nIndex)) return FALSE;
	return TRUE;
}

BOOL CMySwitchBar::OnEraseBkgnd(CDC* pDC) 
{
	CRect rcWnd;
	GetClientRect(&rcWnd);
	if(m_nCurPage>=0)
	{
		int nStart=m_vPage[m_nCurPage].nShowFirst;
		int nEnd=m_vPage[m_nCurPage].nShowLast;
		CRgn tmpRgn,tmpRgn2;
		tmpRgn.CreateRectRgn(0,0,0,0);
		tmpRgn2.CreateRectRgn(0,0,0,0);
		for (int i=nStart;i<=nEnd;i++)
		{
			m_vData[i]->pBt->GetWindowRgn((HRGN)tmpRgn2);
			tmpRgn2.OffsetRgn(m_vData[i]->rect.left,m_vData[i]->rect.top);
			tmpRgn.CombineRgn(&tmpRgn,&tmpRgn2,RGN_OR);
		}
		if(::IsWindow(m_pageBt[0].m_hWnd) && ::IsWindow(m_pageBt[1].m_hWnd))
		{
			for(i=0;i<2;i++)
			{
				if(!m_pageBt[i].IsWindowVisible()) continue;
				m_pageBt[i].GetWindowRgn((HRGN)tmpRgn2);
				tmpRgn2.OffsetRgn(m_bt_rect[i].left,m_bt_rect[i].top);
				tmpRgn.CombineRgn(&tmpRgn,&tmpRgn2,RGN_OR);
			}
		}
		pDC->SelectClipRgn(&tmpRgn,RGN_DIFF);
		tmpRgn.DeleteObject();
		tmpRgn2.DeleteObject();
	}

	CDC memdc;
	CBitmap membm,*poldbm;
	memdc.CreateCompatibleDC(pDC);
	membm.CreateCompatibleBitmap(pDC,rcWnd.Width(),rcWnd.Height());
	poldbm=memdc.SelectObject(&membm);

	switch(m_EraseBkgndStyle)
	{
	case MSB_EBS_BITMAP:
		{
			CSize left=g_GUI.GetBitmapSize(m_BK[9]);
			CSize right=g_GUI.GetBitmapSize(m_BK[10]);
			if(left.cx!=0 && left.cy!=0)
				g_GUI.DrawXPBmp(&memdc,m_BK[9],CPoint(0,0),CPoint(left.cx,rcWnd.Height()));
			if(right.cx!=0 && right.cy!=0)
				g_GUI.DrawXPBmp(&memdc,m_BK[10],CPoint(rcWnd.Width()-right.cx,0),CPoint(right.cx,rcWnd.Height()));
			g_GUI.DrawXPBmp(&memdc,m_BK[11],CPoint(left.cx,0),CPoint(rcWnd.Width()-left.cx-right.cx,rcWnd.Height()));
		}
		break;
	case MSB_EBS_GRADIENT:
		g_GUI.DrawGradient(&memdc,rcWnd,m_startColor,m_endColor,m_bHorizontal);
		break;
	case MSB_EBS_OWNERDRAW:
		break;
	}
	MyEraseBkgnd(&memdc);

	pDC->BitBlt(0,0,rcWnd.Width(),rcWnd.Height(),&memdc,0,0,SRCCOPY);
	memdc.SelectObject(poldbm);
	membm.DeleteObject();
	memdc.DeleteDC();
	
	return TRUE;
}

void CMySwitchBar::MyEraseBkgnd(CDC *pDC)
{
	//刷新背景时最后调用此函数,若有自绘需求,则派生一个子类,重载此虚函数
}

void CMySwitchBar::SetItemOffset(int offset)
{	
	m_item_offset=offset;
}

void CMySwitchBar::SetLevelOffset(int offset)
{
	m_level_offset=offset;
}

BOOL CMySwitchBar::_ModifyRgn(CRgn *rgn_l,int offset_l,MSB_BorderState_ToRight bs0,CRgn *rgn_m,MSB_BorderState_ToRight bs1,CRgn *rgn_r,int offset_r)
{
	if(!rgn_m || !(HRGN)(*rgn_m)) return FALSE;
	if(rgn_l && !(HRGN)(*rgn_l)) return FALSE;
	if(rgn_r && !(HRGN)(*rgn_r)) return FALSE;

	int result;
	if(rgn_l && bs0==MSBBS_OVER)
	{
		CRgn tmpRgn;
		tmpRgn.CreateRectRgn(0,0,0,0);
		result=tmpRgn.CopyRgn(rgn_l);
		if(!CHECK_CRGN_RESULT(result)) return FALSE;
		result=tmpRgn.OffsetRgn(offset_l,0);
		if(!CHECK_CRGN_RESULT(result)) return FALSE;
		result=rgn_m->CombineRgn(rgn_m,&tmpRgn,RGN_DIFF);
		if(!CHECK_CRGN_RESULT(result)) return FALSE;
	}
	if(rgn_r && bs1==MSBBS_UNDER)
	{
		CRgn tmpRgn;
		tmpRgn.CreateRectRgn(0,0,0,0);
		result=tmpRgn.CopyRgn(rgn_r);
		if(!CHECK_CRGN_RESULT(result)) return FALSE;
		result=tmpRgn.OffsetRgn(offset_r,0);
		if(!CHECK_CRGN_RESULT(result)) return FALSE;
		result=rgn_m->CombineRgn(rgn_m,&tmpRgn,RGN_DIFF);
		if(!CHECK_CRGN_RESULT(result)) return FALSE;
	}
	return TRUE;
}

BOOL CMySwitchBar::_CalcItemRgn(UINT nIndex,UINT nStart,UINT nEnd,CRgn &rgn)
{
	HRGN tmphrgn=(HRGN)rgn;
	if(tmphrgn)
		rgn.DeleteObject();
	else
		rgn.CreateRectRgn(0,0,0,0);

	int result=rgn.CopyRgn(&m_vData[nIndex]->rgn);
	if(!CHECK_CRGN_RESULT(result)) return FALSE;

	//当只有一个标签或者为垂直风格时直接返回
	if(nStart==nEnd  || m_style==MSBS_V_L || m_style==MSBS_V_R) 
		return TRUE;
	
	if(nIndex==nStart)
	{
		return _ModifyRgn(NULL,0,MSBBS_UNDER,
			&rgn,m_vData[nStart]->borderState,
			&m_vData[nStart+1]->rgn,m_vData[nStart]->rect.Width()-m_item_offset);
	}else
	if(nIndex==nEnd)
	{
		return _ModifyRgn(&m_vData[nIndex-1]->rgn,-(m_vData[nIndex-1]->rect.Width()-m_item_offset),m_vData[nIndex-1]->borderState,
			&rgn,m_vData[nIndex]->borderState,
			NULL,0);
	}
	else
	{
		return _ModifyRgn(&m_vData[nIndex-1]->rgn,-(m_vData[nIndex-1]->rect.Width()-m_item_offset),m_vData[nIndex-1]->borderState,
			&rgn,m_vData[nIndex]->borderState,
			&m_vData[nIndex+1]->rgn,m_vData[nIndex]->rect.Width()-m_item_offset);
	}
}

void CMySwitchBar::CalcRgn()
{
	if(m_nCurPage<0) return;
	_SetMyRedraw(FALSE);
	int nStart=m_vPage[m_nCurPage].nShowFirst;
	int nEnd=m_vPage[m_nCurPage].nShowLast;
	for(int i=nStart;i<=nEnd;i++)
		m_vData[i]->borderState=MSBBS_OVER;
	if(m_nIndex_sel-1>=nStart && m_nIndex_sel<=nEnd)
		m_vData[m_nIndex_sel-1]->borderState=MSBBS_UNDER;
	for(i=nStart;i<=nEnd;i++)
	{
		CRgn rgn;
		if(_CalcItemRgn(i,nStart,nEnd,rgn))
			m_vData[i]->pBt->SetWindowRgn((HRGN)rgn,FALSE);
	}
	_SetMyRedraw(TRUE);
}

int CMySwitchBar::_GetIndexFromID(UINT id)
{
	int n=m_vData.size();
	for(int i=0;i<n;i++)
		if(m_vData[i]->nID==id) return i;
	return -1;
}
int CMySwitchBar::_GetIDFromIndex(UINT nIndex)
{
	int n=m_vData.size();
	if(nIndex>=n || nIndex<0) return -1;
	return m_vData[nIndex]->nID;
}


void CMySwitchBar::OnClick(WPARAM wParam,LPARAM lParam)
{
	UINT id=lParam;
	
	if(id==BT_PAGE_UP || id==BT_PAGE_DOWN)
	{
		m_nCurPage+=(id==BT_PAGE_UP)?-1:1;
		SetPage(m_nCurPage);
		return;
	}

	int nIndex=_GetIndexFromID(id);
	if(nIndex<0 || (nIndex==m_nIndex_sel))
	{
		if(IsBehaviorAsButton(nIndex));
		else return;
	}
	

	pMySwitchBarMsg pMsg=new MySwitchBarMsg;
	pMsg->id=m_nID;
	pMsg->last_id=_GetIDFromIndex(m_nIndex_sel);
	pMsg->last_sel=m_nIndex_sel;
	pMsg->cur_sel=nIndex;
	pMsg->cur_id=id;

	SetSel(nIndex);
	::SendMessage(GetParent()->m_hWnd,UM_MYSWITCHBAR_CLICK,0,(LPARAM)pMsg);
	TDEL(pMsg);
}

BOOL CMySwitchBar::SetSel(int nIndex,UINT id)
{
	int n=m_vData.size();
	if(n==0 || m_vPage.size()==0 || m_nCurPage<0) return FALSE;
	
	if(nIndex<0)
		nIndex=_GetIndexFromID(id);
	if(nIndex<0) return FALSE;

	int oldSel=m_nIndex_sel;
	m_nIndex_sel=nIndex;
	for(int i=0;i<n;i++)
	{
		m_vData[i]->pBt->SetSel(nIndex==i);
	}
	int nStart=m_vPage[m_nCurPage].nShowFirst;
	int nEnd=m_vPage[m_nCurPage].nShowLast;
	if((nIndex>=nStart && nIndex<=nEnd) || (oldSel>=nStart && oldSel<=nEnd))
		CalcRgn();
	return TRUE;
}

void CMySwitchBar::GetSel(int &nIndex,UINT &id)
{
	nIndex=m_nIndex_sel;
	if(m_nIndex_sel<0) id=0;
	else id=m_vData[m_nIndex_sel]->nID;
}

BOOL CMySwitchBar::SetPage(UINT nPage)
{
	int n=m_vPage.size();
	if(n==0 || nPage>=n) return FALSE;
	m_nCurPage=nPage;
	_SetMyRedraw(FALSE);
	ShowPageButtons();
	CalcRgn();
	MoveAndShow();
	_SetMyRedraw(TRUE);
	return TRUE;
}

void CMySwitchBar::GetPage(int &nCurPage,UINT &nTotalPage)
{
	nCurPage=m_nCurPage;
	nTotalPage=m_vPage.size();
}

BOOL  CMySwitchBar::SetItemData(int nIndex,UINT id,void *pItemData)
{
	int n=m_vData.size();
	if(n==0 || m_vPage.size()==0 || m_nCurPage<0) return FALSE;
	
	if(nIndex<0)
		nIndex=_GetIndexFromID(id);
	if(nIndex<0) return FALSE;

	m_vData[nIndex]->pItemData=pItemData;
	return TRUE;
}

void* CMySwitchBar::GetItemData(int nIndex,UINT id)
{
	int n=m_vData.size();
	if(n==0 || m_vPage.size()==0 || m_nCurPage<0) return NULL;
	
	if(nIndex<0)
		nIndex=_GetIndexFromID(id);
	if(nIndex<0) return NULL;
	return m_vData[nIndex]->pItemData;
}

BOOL CMySwitchBar::DeleteSwitch(int nIndex,UINT id)
{
	int n=m_vData.size();
	if(nIndex>n-1) return FALSE;
	int tmp,oldSel;
	pMySwitchBarData pData=NULL;
	if(nIndex<0)
	{
		tmp=_GetIndexFromID(id);
		if(tmp<0) return FALSE;
	}
	else
	{
		tmp=nIndex;
	}
	oldSel=_GetIDFromIndex(m_nIndex_sel);
	pData=m_vData[tmp];
	m_vData.erase(m_vData.begin()+tmp);
	
	int offset_x=0,offset_y=0;
	switch(m_style)
	{
	case MSBS_H_T:
	case MSBS_H_B:
		{
			offset_x=-(pData->rect.Width()-m_item_offset);
		}
		break;
	case MSBS_V_L:
	case MSBS_V_R:
		{
			offset_y=-(pData->rect.Height()-m_item_offset);
		}
		break;
	}
	_SetMyRedraw(FALSE);
	TDEL(pData);
	for(int i=tmp;i<n-1;i++)
	{
		m_vData[i]->rect.OffsetRect(offset_x,offset_y);
		m_vData[i]->pBt->MoveWindow(m_vData[i]->rect,FALSE);
	}
	if(m_nIndex_sel==tmp && n>1)
	{
		m_nIndex_sel=0;
		m_vData[0]->pBt->SetSel(TRUE);
	}
	else if(n==1)
	{
		m_nIndex_sel=-1;
	}
	else
	{
		m_nIndex_sel=_GetIndexFromID(oldSel);
	}

	//若正在批量删除则先暂不计算
	if(!m_bBatchCalc)
	{
		CalcPage();
		CalcRgn();
		ShowPageButtons();
		MoveAndShow();
	}
	_SetMyRedraw(TRUE);
	return TRUE;
}

void CMySwitchBar::Clear()
{
	int n=m_vData.size();
	pMySwitchBarData pData=NULL;
	_SetMyRedraw(FALSE);
	for(int i=0;i<n;i++)
	{
		pData=m_vData[i];
		TDEL(pData);
	}
	m_vData.clear();
	m_vPage.clear();
	m_nIndex_sel=-1;
	m_nCurPage=-1;
	ShowPageButtons();
	_SetMyRedraw(TRUE);
}

int CMySwitchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pageBt[0].Create(NULL,"Backward",WS_CHILD,m_bt_rect[0],this,BT_PAGE_UP);
	m_pageBt[1].Create(NULL,"Forward",WS_CHILD,m_bt_rect[1],this,BT_PAGE_DOWN);

	return 0;
}

void CMySwitchBar::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	switch(m_style)
	{
	case MSBS_H_T:
	case MSBS_H_B:
		{
			m_bt_rect[1].OffsetRect(-m_bt_rect[1].left+cx-1-m_bt_rect[1].Width(),-m_bt_rect[1].top+m_marge_top);
			m_bt_rect[0].OffsetRect(-m_bt_rect[0].left+m_bt_rect[1].left-1-m_bt_rect[0].Width(),-m_bt_rect[0].top+m_marge_top);
			m_btsRect.SetRect(m_bt_rect[0].left-1,m_marge_top,cx,m_bt_rect[0].bottom);
		}
		break;
	case MSBS_V_L:
	case MSBS_V_R:
		{
			int space=(cx-m_marge_left-m_bt_rect[0].Width()-m_bt_rect[1].Width())/3;
			space=space>=0?space:0;
			m_bt_rect[0].OffsetRect(-m_bt_rect[0].left+m_marge_left+space,-m_bt_rect[0].top+cy-1-m_bt_rect[0].Height());
			m_bt_rect[1].OffsetRect(-m_bt_rect[1].left+m_bt_rect[0].right+space,-m_bt_rect[1].top+cy-1-m_bt_rect[1].Height());
			m_btsRect.SetRect(m_marge_left+1,m_bt_rect[0].top-1,cx-1,cy);
		}
		break;
	}
	if(m_style==MSBS_V_L || m_style==MSBS_V_R)
	{
		int n=m_vData.size();
		int right=0;
		for(int i=0;i<n;i++)
		{
			if(m_style==MSBS_V_L)
				right=cx;
			else
			{
				if(m_vData[i]->level==0)
					right=cx;
				else
					right=cx-m_level_offset;
			}
			m_vData[i]->rect.right=right;
		}
	}
	if(::IsWindow(m_pageBt[0].m_hWnd) && ::IsWindow(m_pageBt[1].m_hWnd))
	{
		m_pageBt[0].MoveWindow(m_bt_rect[0]);
		m_pageBt[1].MoveWindow(m_bt_rect[1]);
	}
	CalcPage();
	ShowPageButtons();
	MoveAndShow();
	CalcRgn();
}

void CMySwitchBar::_CalcPage(UINT nStart,UINT &nEnd,int &pageSize)
{
	int n=m_vData.size();
	int offset;
	if(m_style==MSBS_H_T || m_style==MSBS_H_B)
	{
		offset=m_marge_left;
		for (int i=nStart;i<n;i++)
		{
			int tmp=offset+m_vData[i]->rect.Width();
			if(tmp>m_btsRect.left)	//如果当前标签越过了翻页按钮区域
			{
				if(m_vPage.size()>0)	//如果前面有页了,即一定会需要显示翻页按钮
				{
					nEnd=(i==nStart)?nStart:i-1;
					pageSize=(i==nStart)?(m_vData[i]->rect.Width()):(offset+m_item_offset-m_marge_left);
					return;
				}
				//否则就是计算首页,还不确定是否需要显示翻页按钮
				BOOL bBeyond=FALSE;
				int tmpOffset=tmp-m_item_offset;
				//查看后面是否有越出窗口的标签(需要显示标签)
				for(int j=i+1;j<n;j++)
				{
					if(tmpOffset+m_vData[j]->rect.Width()>m_btsRect.right)
					{
						bBeyond=TRUE;
						break;
					}
					tmpOffset+=m_vData[j]->rect.Width()-m_item_offset;
				}
				if(bBeyond || tmp>m_btsRect.right)//后面有标签超过了边界,或者当前标签不但超过了翻页按钮边界,也超过了窗口边界
				{
					nEnd=(i==nStart)?nStart:i-1;
					pageSize=(i==nStart)?(m_vData[i]->rect.Width()):(offset+m_item_offset-m_marge_left);
					return;
				}
				//若经过上面检测,所有标签都不会越出窗口,则不需要显示翻页按钮
				nEnd=n-1;
				pageSize=-1;	//只有一页时不需要翻页,pageSize无意义
				return;
			}
			offset+=m_vData[i]->rect.Width()-m_item_offset;
		}
		nEnd=n-1;
		pageSize=-1;
	}
	else if(m_style==MSBS_V_L || m_style==MSBS_V_R)
	{
		offset=m_marge_top;
		for (int i=nStart;i<n;i++)
		{
			int tmp=offset+m_vData[i]->rect.Height();
			if(tmp>m_btsRect.top)	//如果当前标签越过了翻页按钮区域
			{
				if(m_vPage.size()>0)	//如果前面有页了,即一定会需要显示翻页按钮
				{
					nEnd=(i==nStart)?nStart:i-1;
					pageSize=(i==nStart)?(m_vData[i]->rect.Height()):(offset+m_item_offset-m_marge_top);
					return;
				}
				//否则就是计算首页,还不确定是否需要显示翻页按钮
				BOOL bBeyond=FALSE;
				int tmpOffset=tmp-m_item_offset;
				//查看后面是否有越出窗口的标签(需要显示标签)
				for(int j=i+1;j<n;j++)
				{
					if(tmpOffset+m_vData[j]->rect.Height()>m_btsRect.bottom)
					{
						bBeyond=TRUE;
						break;
					}
					tmpOffset+=m_vData[j]->rect.Height()-m_item_offset;
				}
				if(bBeyond || tmp>m_btsRect.bottom)//后面有标签超过了边界,或者当前标签不但超过了翻页按钮边界,也超过了窗口边界
				{
					nEnd=(i==nStart)?nStart:i-1;
					pageSize=(i==nStart)?(m_vData[i]->rect.Height()):(offset+m_item_offset-m_marge_top);
					return;
				}
				//若经过上面检测,所有标签都不会越出窗口,则不需要显示翻页按钮
				nEnd=n-1;
				pageSize=-1;	//只有一页时不需要翻页,pageSize无意义
				return;
			}
			offset+=m_vData[i]->rect.Height()-m_item_offset;
		}
		nEnd=n-1;
		pageSize=-1;
	}
}

void CMySwitchBar::CalcPage()
{
	int n=m_vData.size();
	m_vPage.clear();
	if(n==0)
	{
		m_nCurPage=-1;
		return;
	}

	int nShowFirst=0;
	while(nShowFirst<n)
	{
		MySwitchBarPage page;
		page.nShowFirst=nShowFirst;
		_CalcPage(nShowFirst,page.nShowLast,page.nPageSize);
		m_vPage.push_back(page);
		nShowFirst=page.nShowLast+1;
	}
	n=m_vPage.size();
	if(n>0)
	{
		if(m_nCurPage<0) m_nCurPage=0;
		if(m_nCurPage>=n) m_nCurPage=n-1;
	}
	else
		m_nCurPage=-1;
}

void CMySwitchBar::ShowPageButtons()
{
	int n=m_vPage.size();
	if(::IsWindow(m_pageBt[0].m_hWnd) && ::IsWindow(m_pageBt[1].m_hWnd))
	{
		if(n>1)
		{
			m_pageBt[0].ShowWindow(m_nCurPage>0?SW_SHOW:SW_HIDE);
			m_pageBt[1].ShowWindow((m_nCurPage<n-1)?SW_SHOW:SW_HIDE);
		}
		else
		{
			m_pageBt[0].ShowWindow(SW_HIDE);
			m_pageBt[1].ShowWindow(SW_HIDE);
		}
	}
}

void CMySwitchBar::MoveAndShow()
{
	if(m_nCurPage<0) return;
	int nStart=m_vPage[m_nCurPage].nShowFirst;
	int nEnd=m_vPage[m_nCurPage].nShowLast;
	int n=m_vData.size();
	int offset=0;
	for (int i=0;i<m_nCurPage;i++)
	{
		offset+=m_vPage[i].nPageSize-m_item_offset;
	}
	if(m_style==MSBS_H_T || m_style==MSBS_H_B)
	{
		offset=-m_vData[0]->rect.left-offset+m_marge_left;
		for (i=0;i<n;i++)
		{
			m_vData[i]->rect.OffsetRect(offset,0);
			m_vData[i]->pBt->MoveWindow(m_vData[i]->rect,FALSE);
			if(i>=nStart && i<=nEnd)
				m_vData[i]->pBt->ShowWindow(SW_SHOW);
			else
				m_vData[i]->pBt->ShowWindow(SW_HIDE);
		}
	}
	else if(m_style == MSBS_V_L || m_style ==MSBS_V_R)
	{
		offset=-m_vData[0]->rect.top-offset+m_marge_top;
		for (i=0;i<n;i++)
		{
			m_vData[i]->rect.OffsetRect(0,offset);
			m_vData[i]->pBt->MoveWindow(m_vData[i]->rect,FALSE);
			if(i>=nStart && i<=nEnd)
				m_vData[i]->pBt->ShowWindow(SW_SHOW);
			else
				m_vData[i]->pBt->ShowWindow(SW_HIDE);
		}
	}
}

void CMySwitchBar::ChangeLabelBehavior(UINT nIndex)
{
	if (nIndex < 0 || std::find(m_vtButtons.begin(),m_vtButtons.end(),nIndex) != m_vtButtons.end())
		return;

	m_vtButtons.push_back(nIndex);
}

BOOL CMySwitchBar::IsBehaviorAsButton(UINT nIndex)
{
	if (std::find(m_vtButtons.begin(),m_vtButtons.end(),nIndex) != m_vtButtons.end())
		return TRUE;
	else
		return FALSE;
}
