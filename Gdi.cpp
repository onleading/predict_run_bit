#include "stdafx.h"
#include "Gdi.h"

COLORREF l_KLineColor[17] = {RGB(168,0,0),RGB(0,0,168),RGB(0,168,168),RGB(0,168,168),RGB(128,128,128),RGB(168,0,168),/*从这里开始是叠加股票的颜色*/
							 RGB(150,150,150),RGB(168,64,0),RGB(0,150,84),
							 RGB(0,192,192),RGB(150,150,150),RGB(168,64,0),
							 RGB(252,84,84),RGB(252,84,252),RGB(252,252,84),RGB(192,192,192),RGB(0,255,255)
							};
Gdi::Gdi()
{
	for(int i = MAX_PEN_NUM;i < MAX_PEN_NUM + MAXKCOLOR;i++)
		m_Pen[i].CreatePen(PS_SOLID,1,l_KLineColor[i-MAX_PEN_NUM]);
	for(i = MAX_BRUSH_NUM;i < MAX_BRUSH_NUM + MAXKCOLOR;i++)
		m_Brush[i].CreateSolidBrush(l_KLineColor[i-MAX_BRUSH_NUM]);
}

Gdi::~Gdi()
{
	for(int i = MAX_PEN_NUM;i < MAX_PEN_NUM + MAXKCOLOR;i++)
		m_Pen[i].DeleteObject();
	for(i = MAX_BRUSH_NUM;i < MAX_BRUSH_NUM + MAXKCOLOR;i++)
		m_Brush[i].DeleteObject();
}

//构造所有系统所用GDI资源
void Gdi::CreateGdiObject()
{
	BRUSH_PENCOLOR[0] = VipColor.KPCOLOR;
	BRUSH_PENCOLOR[1] = VipColor.KNCOLOR;
	BRUSH_PENCOLOR[2] = VipColor.KPCOLOR;
	BRUSH_PENCOLOR[3] = VipColor.LEVELCOLOR;
	BRUSH_PENCOLOR[4] = VipColor.DOWNCOLOR;
	BRUSH_PENCOLOR[5] = VipColor.FLOATCURSORCOLOR;
	BRUSH_PENCOLOR[6] = VipColor.BACKCOLOR;
	BRUSH_PENCOLOR[7] = RGB(128, 128, 128);
	BRUSH_PENCOLOR[8] = RGB(255, 0, 0);
	BRUSH_PENCOLOR[9] = RGB(0, 128, 0);
	BRUSH_PENCOLOR[10] = RGB(128, 128, 128);

	m_Pen[AXIS_PEN].CreatePen(PS_SOLID,1,VipColor.AXISCOLOR);
	m_Pen[AXIS2_PEN].CreatePen(PS_SOLID,2,VipColor.AXISCOLOR);
	m_Pen[DOTAXIS_PEN].CreatePen(PS_DOT,1,VipColor.AXISCOLOR);
	m_Pen[TIME_PEN].CreatePen(PS_SOLID,1,VipColor.TIMECOLOR);
	m_Pen[TXT_PEN].CreatePen(PS_SOLID,1,VipColor.TXTCOLOR);
	m_Pen[KP_PEN].CreatePen(PS_SOLID,1,VipColor.KPCOLOR);
	m_Pen[KN_PEN].CreatePen(PS_SOLID,1,VipColor.KNCOLOR);
	m_Pen[UP_PEN].CreatePen(PS_SOLID,1,VipColor.KPCOLOR);
	m_Pen[DOWN_PEN].CreatePen(PS_SOLID,1,VipColor.DOWNCOLOR);
	m_Pen[LEVEL_PEN].CreatePen(PS_SOLID,1,VipColor.LEVELCOLOR);
	m_Pen[US_PEN].CreatePen(PS_SOLID,1,VipColor.USCOLOR);
	if(g_GH.bBoldZSTLine)
		m_Pen[ZB1_PEN].CreatePen(PS_SOLID,2,VipColor.ZBCOLOR[0]);
	else
		m_Pen[ZB1_PEN].CreatePen(PS_SOLID,1,VipColor.ZBCOLOR[0]);
	m_Pen[VOL_PEN].CreatePen(PS_SOLID,1,VipColor.VOLCOLOR);
	m_Pen[ZST_UP_PEN].CreatePen(PS_SOLID,1,VipColor.KPCOLOR);
	m_Pen[ZST_DOWN_PEN].CreatePen(PS_SOLID,1,VipColor.DOWNCOLOR);
	m_Pen[ZST_LEVEL_PEN].CreatePen(PS_SOLID,1,VipColor.LEVELCOLOR);
	m_Pen[ZB2_PEN].CreatePen(PS_SOLID,1,VipColor.ZBCOLOR[1]);
	m_Pen[ZB3_PEN].CreatePen(PS_SOLID,1,VipColor.ZBCOLOR[2]);
	m_Pen[ZB4_PEN].CreatePen(PS_SOLID,1,VipColor.ZBCOLOR[3]);
	m_Pen[ZB5_PEN].CreatePen(PS_SOLID,1,VipColor.ZBCOLOR[4]);
	m_Pen[ZB6_PEN].CreatePen(PS_SOLID,1,VipColor.ZBCOLOR[5]);
	m_Pen[GRIDLINE_PEN].CreatePen(PS_SOLID,1,GridColor.GridLineColor);
	m_Pen[GRIDSEL_PEN].CreatePen(PS_SOLID,1,GridColor.GridHighLineColor);
	m_Pen[OVERLAP_PEN].CreatePen(PS_SOLID,1,RGB(128, 128, 128));

	m_Brush[KP_BRUSH].CreateSolidBrush(VipColor.KPCOLOR);
	m_Brush[KN_BRUSH].CreateSolidBrush(VipColor.KNCOLOR);
	m_Brush[UP_BRUSH].CreateSolidBrush(VipColor.KPCOLOR);
	m_Brush[LEVEL_BRUSH].CreateSolidBrush(VipColor.LEVELCOLOR);
	m_Brush[DOWN_BRUSH].CreateSolidBrush(VipColor.DOWNCOLOR);
	m_Brush[INVERSE_BRUSH].CreateSolidBrush(VipColor.FLOATCURSORCOLOR);
	m_Brush[BACK_BRUSH].CreateSolidBrush(VipColor.BACKCOLOR);
	m_Brush[OVERLAP_BRUSH].CreateSolidBrush(RGB(128, 128, 128));
	m_Brush[FIX_RED_BRUSH].CreateSolidBrush(RGB(255, 0, 0));
	m_Brush[FIX_GREEN_BRUSH].CreateSolidBrush(RGB(0, 128, 0));
	if(IsBlackColorScheme(VipColor.BACKCOLOR))
		m_Brush[GRAYBACK_BRUSH].CreateSolidBrush(g_GUI.LightenScaleColor(50,VipColor.BACKCOLOR));
	else
		m_Brush[GRAYBACK_BRUSH].CreateSolidBrush(g_GUI.DarkenScaleColor(50,VipColor.BACKCOLOR));
	m_Brush[AXIS_BRUSH].CreateSolidBrush(VipColor.AXISCOLOR);

	if(!g_bBigFontGrid)
		m_Font[LIST_FONT].CreateFont(g_nFontSize[0],0,0,0,g_nFontWeigth[0],0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[0]);
	else
		m_Font[LIST_FONT].CreateFont(g_nFontSize[6],0,0,0,g_nFontWeigth[6],0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[6]);
	m_Font[GRAPH_FONT].CreateFont(g_nFontSize[1],0,0,0,g_nFontWeigth[1],0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[1]);
	m_Font[BOLDGRAPH_FONT].CreateFont(g_nFontSize[2],0,0,0,g_nFontWeigth[2],0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[2]);
	m_Font[INFO_FONT].CreateFont(g_nFontSize[3],0,0,0,g_nFontWeigth[3],0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[3]);
	
	m_Font[SMALL_FONT].CreateFont(g_nFontSize[4],0,0,0,g_nFontWeigth[4],0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[4]);
	m_Font[SMALL2_FONT].CreateFont(g_nFontSize[5],0,0,0,g_nFontWeigth[5], 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[5]);
	m_Font[MEDIUM_FONT].CreateFont(g_nFontSize[6],0,0,0,g_nFontWeigth[6],0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[6]);

	m_Font[BIGHQ1_FONT].CreateFont(g_nFontSize[7], 0, 0, 0, g_nFontWeigth[7], 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE, g_strFontName[7]);
	m_Font[BIGHQ2_FONT].CreateFont(g_nFontSize[8], 0, 0, 0, g_nFontWeigth[8], 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[8]);	
	m_Font[BIGHQ3_FONT].CreateFont(g_nFontSize[9], 0, 0, 0, g_nFontWeigth[9], 0, 0, 0, ANSI_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[9]);	
	m_Font[BIGHQ4_FONT].CreateFont(g_nFontSize[10], 0, 0, 0, g_nFontWeigth[10], 0, 0, 0, ANSI_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[10]);
}

void Gdi::ReleaseGdiObject()
{
	for(int i = 0;i < MAX_PEN_NUM;i++)
		m_Pen[i].DeleteObject();
	for(i = 0;i < MAX_BRUSH_NUM;i++)
		m_Brush[i].DeleteObject();
	for(i = 0;i < MAX_FONT_NUM;i++)
		m_Font[i].DeleteObject();
}

//重新构造系统所用GDI资源
void Gdi::ReCreateGdiObject()
{
	ReleaseGdiObject();
	CreateGdiObject();
}

//资源的选择和恢复
CPen * Gdi::SelectPen(CDC *pDC,int i)
{
	return pDC->SelectObject(&m_Pen[i]);
}

void Gdi::RestorePen(CDC *pDC,CPen *oldpen)
{
	pDC->SelectObject(oldpen);
}

CBrush * Gdi::SelectBrush(CDC *pDC,int i)
{
	return pDC->SelectObject(&m_Brush[i]);
}

void Gdi::RestoreBrush(CDC *pDC,CBrush *oldbrush)
{
	pDC->SelectObject(oldbrush);
}

CFont * Gdi::SelectFont(CDC *pDC,int i)
{
	return pDC->SelectObject(&m_Font[i]);
}

void Gdi::RestoreFont(CDC *pDC,CFont *oldfont)
{
	pDC->SelectObject(oldfont);
}

//判断是否是暗色调
BOOL Gdi::IsBlackColorScheme(BOOL bBkColor)
{
	DWORD tmpWord=bBkColor;
	return ((tmpWord&0x000000FF)<100 && ((tmpWord&0x0000FF00)>>8)<100 && ((tmpWord&0x00FF0000)>>16)<100);
}

//各种画法
void Gdi::DrawLine(CDC * pDC, int x1, int y1, int x2, int y2)
{
	pDC->MoveTo(x1,y1);
	pDC->LineTo(x2,y2);
}

//各种画法,提供一种颜色
void Gdi::DrawLine(CDC * pDC, int x1, int y1, int x2, int y2,COLORREF color,int style,int width)
{
	CPen Pen;
	Pen.CreatePen(style,width,color);
	CPen *OldPen =pDC->SelectObject(&Pen);
	pDC->MoveTo(x1,y1);
	pDC->LineTo(x2,y2);
	pDC->SelectObject(OldPen);
	Pen.DeleteObject();
}

void Gdi::DotLine(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF fColor,int space)
{
	if(x1 == x2)
	{
		for(int i=min(y1,y2);i<max(y1,y2);i+=space)
			pDC->SetPixel(x1,i,fColor);
	}
	else if(y1 == y2)
	{
		for(int i=min(x1,x2);i<max(x1,x2);i+=space)
			pDC->SetPixel(i,y1,fColor);
	}
	else
	{
		CPen *oldpen = SelectPen(pDC,DOTAXIS_PEN);
		pDC->MoveTo(x1,y1);
		pDC->LineTo(x2,y2);
		RestorePen(pDC,oldpen);
	}
}

void Gdi::DrawRect(CDC *pDC,int left,int top,int right,int bottom)
{
	POINT aPoint[5];
	aPoint[0].x = left, aPoint[0].y = top;
	aPoint[1].x = right, aPoint[1].y = top;
	aPoint[2].x = right, aPoint[2].y = bottom;
	aPoint[3].x = left, aPoint[3].y = bottom;
	aPoint[4].x = left, aPoint[4].y = top;
	pDC->Polyline(aPoint,5);
}

void Gdi::DrawRect(CDC *pDC,int left,int top,int right,int bottom,COLORREF color,int style,int width)
{
	CPen Pen;
	Pen.CreatePen(style,width,color);
	CPen *OldPen =pDC->SelectObject(&Pen);
	DrawRect(pDC,left,top,right,bottom);
	pDC->SelectObject(OldPen);
	Pen.DeleteObject();
}
void Gdi::DrawRect(CDC * pDC,LPCRECT lpRect)
{
	DrawRect(pDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

void Gdi::DrawRect(CDC *pDC,LPCRECT lpRect,COLORREF color,int style,int width)
{
	CPen Pen;
	Pen.CreatePen(style,width,color);
	CPen *OldPen =pDC->SelectObject(&Pen);
	DrawRect(pDC,lpRect);
	pDC->SelectObject(OldPen);
	Pen.DeleteObject();
}

void Gdi::DrawDotRect(CDC *pDC,LPCRECT lpRect,COLORREF color,int space)
{
	DrawDotRect(pDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,color,space);
}

void Gdi::DrawDotRect(CDC *pDC,int left,int top,int right,int bottom,COLORREF color,int space)
{
	DotLine(pDC,left,top,right,top,color,space);
	DotLine(pDC,right,top,right,bottom,color,space);
	DotLine(pDC,right,bottom,left,bottom,color,space);
	DotLine(pDC,left,bottom,left,top,color,space);
}

void Gdi::Bar(CDC *pDC,int left,int top,int right,int bottom,int whichBrush)
{
	right+=1;	//对位置进行修正
	CRect rc;
	if(abs(right-left)>2)
	{
		if(top==bottom) //如果bottom与top相等,则调节一下,否则FillRect不能画出
			bottom+=1;
		rc.SetRect(left,top,right,bottom);
		pDC->FillRect(&rc,&m_Brush[whichBrush]);
	}
	else
	{
		if(whichBrush >= MAX_BRUSH_NUM)
			DrawLine(pDC,(right+left)/2,top,(right+left)/2,bottom,l_KLineColor[whichBrush-MAX_BRUSH_NUM]);
		else
			DrawLine(pDC,(right+left)/2,top,(right+left)/2,bottom,BRUSH_PENCOLOR[whichBrush]);
	}
}

void Gdi::Bar(CDC *pDC,LPCRECT lpRect,int whichBrush)
{
	Bar(pDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,whichBrush);
}

void Gdi::Bar(CDC *pDC,int left,int top,int right,int bottom,COLORREF color)
{
	CRect rc(left,top,right,bottom);
	pDC->FillSolidRect(&rc,color);
}

CSize Gdi::DisplayText(CDC *pDC,int x,int y,COLORREF fColor,
					   const char *fmt,...)
{
    va_list arg_ptr;
    char tmp[800];
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);
//	AllTrim(tmp);//没有rectwidth的函数不Trim
	pDC->SetTextColor(fColor);
	pDC->TextOut(x,y,tmp);
	return pDC->GetTextExtent(tmp);
}

CSize Gdi::DisplayText(CDC *pDC,int x,int y,COLORREF fColor,
					   int rectwidth,const char *fmt,...)
{
    va_list arg_ptr;
    char tmp[800];
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);
	if(tmp[0]!='-')
		AllTrim(tmp);
	CSize size = pDC->GetTextExtent(tmp);
	if(rectwidth > 0)
		x = x+rectwidth-size.cx;
	pDC->SetTextColor(fColor);
	pDC->TextOut(x,y,tmp);
	return size;
}

CSize Gdi::DisplayColorVol(CDC *pDC,int x,int y,int inoutflag,int rectwidth,DWORD lVol)
{
	if(inoutflag==0)
		return DisplayText(pDC,x,y,VipColor.KPCOLOR,rectwidth,"%s",MakeVol3(lVol));
	else if(inoutflag==1)
		return DisplayText(pDC,x,y,VipColor.DOWNCOLOR,rectwidth,"%s",MakeVol3(lVol));
	return DisplayText(pDC,x,y,VipColor.VOLCOLOR,rectwidth,"%s",MakeVol3(lVol));
}

CSize Gdi::DisplayText(CDC *pDC,CRect rect,COLORREF fColor,
					   UINT nFormat,const char *fmt,...)
{
	va_list arg_ptr;
    char tmp[800];
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);
	if(tmp[0]!='-')
		AllTrim(tmp);
	CSize size = pDC->GetTextExtent(tmp);
	pDC->SetTextColor(fColor);
	pDC->DrawText(tmp, strlen(tmp), rect, nFormat);
	return size;
}

CSize Gdi::DisplaySameWidtchText(CDC *pDC,int x,int y,COLORREF fColor,
					   const char *fmt,...)
{
    va_list arg_ptr;
    char tmp[800],tmpStr[800];
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);

	pDC->SetTextColor(fColor);
	int i,cx=0,lastpos=-1,tmpLen;
	CSize size = pDC->GetTextExtent("笔");
	for(i=0;i<strlen(tmp);i++)
	{
		if(tmp[i]==' ')
		{
			if(lastpos==-1)
			{
				tmpLen=i+1;
				strncpy(tmpStr,tmp,tmpLen);
			}
			else
			{
				tmpLen=i-lastpos;
				strncpy(tmpStr,tmp+lastpos+1,tmpLen);
			}
			tmpStr[tmpLen]=0;
			pDC->TextOut(x+cx,y,tmpStr);
			cx+=tmpLen*size.cx/2;
			lastpos=i;
		}
	}
	if(lastpos==-1)
	{
		tmpLen=i;
		strncpy(tmpStr,tmp,tmpLen);
	}
	else
	{
		tmpLen=i-1-lastpos;
		strncpy(tmpStr,tmp+lastpos+1,tmpLen);
	}
	tmpStr[tmpLen]=0;
	pDC->TextOut(x+cx,y,tmpStr);
	cx+=tmpLen*size.cx/2;
	
	size.cx=cx;
	return size;
}

CSize Gdi::NoBjPrintf(CDC * pDC,int x,int y,COLORREF fColor,
					   float f1,int xsflag,int rectwidth,int postfix_flag)
{
	char buf[100];
	switch(xsflag)
	{
	case 0:
		sprintf(buf,"%9.0f",f1);
		break;
	case 1:
		sprintf(buf,"%9.1f",f1);
		break;
	case 3:
		sprintf(buf,"%9.3f",f1);
		break;
	case 4:
		sprintf(buf,"%9.4f",f1);
		break;
	case 5:
		sprintf(buf,"%9.5f",f1);
		break;
	default:
		if(postfix_flag>=1 && f1>100.0)
			sprintf(buf,"%9.1f",f1);
		else
			sprintf(buf,"%9.2f",f1);
		break;
	}
	Format_PostFix(postfix_flag,(f1>COMPPREC),buf,sizeof(buf));
	return DisplayText(pDC,x,y,fColor,rectwidth,buf);
}

CSize Gdi::BjPrintf(CDC *pDC,int x,int y,
					   float f1,float f2,int xsflag,int rectwidth,int postfix_flag)
{
	int fcolor;
	if (f1 > f2+0.00001)
		fcolor = VipColor.KPCOLOR;
	else if (f1 < f2-0.00001) 
		fcolor = VipColor.DOWNCOLOR;
	else            
		fcolor = VipColor.LEVELCOLOR;	
	return NoBjPrintf(pDC,x,y,fcolor,f1,xsflag,rectwidth,postfix_flag);
}

CSize Gdi::BjPrintf(CDC *pDC,int x,int y,
					long l1,long l2,int rectwidth,int postfix_flag)
{
	char buf[100];
	int fcolor;
	if (l1 > l2)
		fcolor = VipColor.KPCOLOR;
	else if (l1 < l2) 
		fcolor = VipColor.DOWNCOLOR;
	else            
		fcolor = VipColor.LEVELCOLOR;
	sprintf(buf,"%9d",l1);
	Format_PostFix(postfix_flag,(l1>0),buf,sizeof(buf));
	return DisplayText(pDC,x,y,fcolor,rectwidth,buf);
}

void Gdi::DrawO(CDC *pDC,int x,int y, COLORREF *clrFill, int nHGS)  //画O,画前先设Pen和Brush
{
	BOOL  bFill = (clrFill!=NULL);
	pDC->Ellipse(x-nHGS,y-nHGS,x+nHGS,y+nHGS);
	if (bFill)
	{
		CRgn  reg;
		reg.CreateEllipticRgn(x-nHGS,y-nHGS,x+nHGS,y+nHGS);
		CBrush Brush(*clrFill);
		pDC->FillRgn(&reg,&Brush);
		Brush.DeleteObject();
		reg.DeleteObject();
	}
}

void Gdi::DrawX(CDC *pDC,int x,int y, int nHGS)  //画X,画前先设Pen
{
	DrawLine(pDC,x-nHGS,y-nHGS,x+nHGS,y+nHGS);
	DrawLine(pDC,x+nHGS,y-nHGS,x-nHGS,y+nHGS);
}

//画游标
void Gdi::DrawSwim(CDC *pDC,CRect rect,char *Content)
{
//	Bar(pDC,&rect,INVERSE_BRUSH);
//	DrawRect(pDC,&rect);
	g_d.Draw3DBar3(pDC,rect,VipColor.AXISCOLOR,FALSE);
	DisplayText(pDC,rect.left+1,rect.top,VipColor.BIGTITLECOLOR,0,Content);
}

BOOL Gdi::ClipTest(double p,double q,double *u1,double *u2)
{
	double r;
	BOOL retVal=TRUE;
	if(p<0.0)
	{
		r=q/p;
		if(r>*u2)
			retVal=FALSE;
		else if(r>*u1)
			*u1=r;
	}
	else if(p>0.0)
	{
		r=q/p;
		if(r<*u1)
			retVal=FALSE;
		else if(r<*u2)
			*u2=r;
	}
	else
		if(q<0.0)
			retVal=FALSE;
	return retVal;
}

void Gdi::ClipLine(CDC *pDC,int x1,int y1,int x2,int y2,CRect rc,COLORREF color,int nDrawMode,int style,int width)
{
	double u1=0.0,u2=1.0,dx=double(x2-x1),dy;
	if(ClipTest(-dx,x1-rc.left,&u1,&u2))
	{
		if(ClipTest(dx,rc.right-x1,&u1,&u2))
		{
			dy=double(y2-y1);
			if(ClipTest(-dy,y1-rc.top,&u1,&u2))
			{
				if(ClipTest(dy,rc.bottom-y1,&u1,&u2))
				{
					if(u2<1.0)
					{
						x2=int(x1+u2*dx);
						y2=int(y1+u2*dy);
					}
					if(u1>0.0)
					{
						x1=int(x1+u1*dx);
						y1=int(y1+u1*dy);
					}
					if(nDrawMode == R2_XORPEN) //如果是XOR
					{
						int tempMode = pDC->SetROP2 (R2_XORPEN);
						DrawLine(pDC,x1,y1,x2,y2,color^VipColor.BACKCOLOR,style,width);
						pDC->SetROP2 (tempMode);
					}
					else
						DrawLine(pDC,x1,y1,x2,y2,color,style,width);
				}
			}
		}
	}
}


//为了避免抓图程序破解验证码,验证码支持小范围偏移
void Gdi::GetYzmOffset(int &nRetX,int &nRetY,int nX,int nY)
{
	int nAddFlag=1;
	if(nX==0 || nY==0)	return;

	nRetX = rand()%nX;
	nRetY = rand()%nY;
	if(nRetX<nRetY)	nAddFlag=-1;

	nRetX = nRetX*nAddFlag;
	nRetY = nRetY*nAddFlag;
}

//nColorYzm为0:黑色 1:变色 2:红色
COLORREF yzmColor[6]={RGB(255,0,0),RGB(238,0,255),RGB(0,0,255),RGB(0,153,255),RGB(255,128,0),RGB(0,128,0)};
void Gdi::DrawYZMNumber(CDC *pDC,CRect rect,int flag,char nColorYzm)
{
	COLORREF tmpyzmColor=RGB(0,0,0);
	if(nColorYzm==1)
		tmpyzmColor=yzmColor[rand()%6];
	else if(nColorYzm==2)
		tmpyzmColor=RGB(255,0,0);
	CPen pen;
	pen.CreatePen(PS_SOLID,1,tmpyzmColor);
	CPen *oldpen=pDC->SelectObject(&pen);
	int nX = 0,nY = 0;
	switch(flag)
	{
	case 0:
		{ 
			GetYzmOffset(nX,nY,2,3);
			int x=rect.left+rect.Width()/2+nX;
			int y=rect.top+3+nY;
		
			/////////////////////左半边//////////////////
			for(int i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x--;
				y++;
			}
			pDC->MoveTo(x,y);
			pDC->LineTo(x,rect.bottom-6);
			y=rect.bottom-6;
			x++;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x++;
				y++;
			}
			/////////////////////右半边//////////////////
			x=rect.left+rect.Width()/2;
			y=rect.top+3;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x++;
				y++;
			}
			pDC->MoveTo(x,y);
			pDC->LineTo(x,rect.bottom-6);
			y=rect.bottom-6;
			x--;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x--;
				y++;
			}
		}
		break;
	case 1:
		{
			GetYzmOffset(nX,nY,2,3);
			int x=rect.left+rect.Width()/2+nX;
			int y=rect.top+nY;

			pDC->MoveTo(x-1,y+4);
			pDC->LineTo(x,y+3);
			pDC->LineTo(x,rect.bottom-3);
			pDC->LineTo(x-1,rect.bottom-3);
			pDC->LineTo(x+2.5,rect.bottom-3);
		}
		break;
	case 2:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart-3,yStart+6);
			pDC->MoveTo(xStart-4,yStart+4);
			pDC->LineTo(xStart-4,yStart+6);
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart+3,yStart+3);
			pDC->MoveTo(xStart+3,yStart+3);
			pDC->LineTo(xStart+3,yStart+7);
			pDC->MoveTo(xStart+4,yStart+4);
			pDC->LineTo(xStart+4,yStart+7);

			int x=xStart+3;
			int y=yStart+7;
			
			for(int i=0;i<7;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+1);
				x--;
				y++;
			}

			pDC->MoveTo(x,y);
			pDC->LineTo(x,y+2);
			pDC->LineTo(xStart+5,y+2);
			pDC->LineTo(xStart+5,y);
		}
		break;
	case 3:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart-3,yStart+6);
			pDC->MoveTo(xStart-4,yStart+4);
			pDC->LineTo(xStart-4,yStart+6);
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart+3,yStart+3);
			pDC->MoveTo(xStart+3,yStart+3);
			pDC->LineTo(xStart+3,yStart+rect.Height()/2);
			pDC->MoveTo(xStart+4,yStart+4);
			pDC->LineTo(xStart+4,yStart+rect.Height()/2-1);

			pDC->MoveTo(xStart+3,yStart+rect.Height()/2);
			pDC->LineTo(xStart-3,yStart+rect.Height()/2);

			pDC->MoveTo(xStart+3,yStart+rect.Height()/2);
			pDC->LineTo(xStart+3,rect.bottom-3);
			pDC->MoveTo(xStart+4,yStart+rect.Height()/2+1);
			pDC->LineTo(xStart+4,rect.bottom-4);

			pDC->MoveTo(xStart+3,rect.bottom-3);
			pDC->LineTo(xStart-3,rect.bottom-3);

			pDC->MoveTo(xStart-3,rect.bottom-3);
			pDC->LineTo(xStart-3,rect.bottom-6);

			pDC->MoveTo(xStart-4,rect.bottom-4);
			pDC->LineTo(xStart-4,rect.bottom-6);

		}
		break;
	case 4:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart-3,yStart+rect.Height()/2-1);
			pDC->MoveTo(xStart-4,yStart+3);
			pDC->LineTo(xStart-4,yStart+rect.Height()/2);
			pDC->MoveTo(xStart-5,yStart+rect.Height()/2-1);
			pDC->LineTo(xStart-5,yStart+rect.Height()/2);

			pDC->MoveTo(xStart-5,yStart+rect.Height()/2);
			pDC->LineTo(xStart+5,yStart+rect.Height()/2);

			pDC->MoveTo(xStart,yStart+5);
			pDC->LineTo(xStart,rect.bottom-2);

			pDC->MoveTo(xStart+1,yStart+5);
			pDC->LineTo(xStart+1,rect.bottom-2);
		}
		break;
	case 5:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart-3,yStart+rect.Height()/2);
			pDC->MoveTo(xStart-4,yStart+3);
			pDC->LineTo(xStart-4,yStart+rect.Height()/2);
			
			pDC->MoveTo(xStart-2,yStart+3);
			pDC->LineTo(xStart+3,yStart+3);
			pDC->MoveTo(xStart-2,yStart+rect.Height()/2);
			pDC->LineTo(xStart+2,yStart+rect.Height()/2);

			pDC->LineTo(xStart+2,yStart+rect.Height()/2+3);
			pDC->MoveTo(xStart+3,yStart+rect.Height()/2+1);
			pDC->LineTo(xStart+3,yStart+rect.Height()/2+3);

			int x=xStart+2;
			int y=yStart+rect.Height()/2+3;

			for(int i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+1);
				x--;
				y++;
			}

			pDC->LineTo(xStart-4,y);
		}
		break;
	case 6:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			int x=xStart;
			int y=yStart+8;
			for(int i=0;i<5;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y-2);
				x++;
				y--;
			}
			/////////////////////左半边//////////////////
			x=xStart;
			y=yStart+8;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x--;
				y++;
			}
			pDC->MoveTo(x,y);
			pDC->LineTo(x,rect.bottom-4);
			y=rect.bottom-4;
			x+=1;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x++;
				y++;
			}
			/////////////////////右半边//////////////////
			x=xStart;
			y=yStart+8;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x++;
				y++;
			}
			pDC->MoveTo(x,y);
			pDC->LineTo(x,rect.bottom-4);
			y=rect.bottom-4;
			x--;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x--;
				y++;
			}
		}
		break;
	case 7:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			pDC->MoveTo(xStart-4,yStart+2);
			pDC->LineTo(xStart+4,yStart+2);
			
			int x=xStart+4;
			int y=yStart+2;

			for(int i=0;i<5;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+4);
				x--;
				y+=3;
			}
		}
		break;
	case 8:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart+3,yStart+3);
			pDC->LineTo(xStart+3,yStart+rect.Height()/2);
			pDC->MoveTo(xStart+4,yStart+4);
			pDC->LineTo(xStart+4,yStart+rect.Height()/2-1);
			pDC->MoveTo(xStart-3,yStart+3);
			pDC->LineTo(xStart-3,yStart+rect.Height()/2);
			pDC->MoveTo(xStart-4,yStart+4);
			pDC->LineTo(xStart-4,yStart+rect.Height()/2-1);
			
			pDC->MoveTo(xStart-3,yStart+rect.Height()/2);
			pDC->LineTo(xStart+3,yStart+rect.Height()/2);

			pDC->MoveTo(xStart-3,rect.bottom-3);
			pDC->LineTo(xStart+3,rect.bottom-3);
			pDC->LineTo(xStart+3,rect.bottom-rect.Height()/2);
			pDC->MoveTo(xStart+4,rect.bottom-4);
			pDC->LineTo(xStart+4,rect.bottom-rect.Height()/2+1);
			pDC->MoveTo(xStart-3,rect.bottom-3);
			pDC->LineTo(xStart-3,rect.bottom-rect.Height()/2);
			pDC->MoveTo(xStart-4,rect.bottom-4);
			pDC->LineTo(xStart-4,rect.bottom-rect.Height()/2+1);
		}
		break;
	case 9:
		{
			GetYzmOffset(nX,nY,2,3);
			int xStart=rect.left+rect.Width()/2+nX;
			int yStart=rect.top+nY;
			int x=xStart;
			int y=yStart+2;
			/////////////////////左半边//////////////////
			for(int i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x--;
				y++;
			}
			pDC->MoveTo(x,y);
			pDC->LineTo(x,rect.bottom-9);
			y=rect.bottom-9;
			x+=1;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x++;
				y++;
			}
			/////////////////////右半边//////////////////
			x=xStart;
			y=yStart+2;
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+2);
				x++;
				y++;
			}
			pDC->MoveTo(x,y);
			pDC->LineTo(x,rect.bottom-9);
			y=rect.bottom-9;
			x--;
			for(i=0;i<4;i++)
			{
				pDC->MoveTo(x,y);
				pDC->LineTo(x,y+3);
				x--;
				y+=2;
			}
		}
		break;
	}
	pDC->SelectObject(oldpen);
}

void Gdi::Draw3DBar(CDC *pDC,CRect rect,COLORREF baseclr,BOOL bVertical,BOOL bEmpty)
{
	if(rect.Height()<=0 || rect.Width()<=0) return;

	if(bEmpty)
	{
		DrawLine(pDC,rect.left,rect.top,rect.right,rect.top,baseclr);
		DrawLine(pDC,rect.right,rect.top,rect.right,rect.bottom,baseclr);
		DrawLine(pDC,rect.right,rect.bottom,rect.left,rect.bottom,baseclr);
		DrawLine(pDC,rect.left,rect.bottom,rect.left,rect.top,baseclr);
		return;
	}

	int colorr	= int(baseclr)%256;
	int colorg	= int(baseclr)/256%256;
	int colorb	= int(baseclr)/256/256%256;
	int colorvalue	= 1<<24-1;
	double tmppi = 3.1415926535898;
	double rate = 0;
	int maxr	= max(224,colorr);
	int maxg	= max(224,colorg);
	int maxb	= max(224,colorb);
	int nWidth	= 0;
	if(bVertical) nWidth = rect.Width();
	else nWidth = rect.Height();
	double off	= nWidth/3.0f;
	int i		= 0;
	for(i=0;i<nWidth;i++)
	{
		if(i<=off)
		{					
			rate		= 1-sin(tmppi/2-tmppi/2*i/off);
			colorvalue	= int((maxr-colorr)*rate+colorr)+256*int((maxg-colorg)*rate+colorg)+65536*int((maxb-colorb)*rate+colorb);
		}
		else
		{
			rate		= sin(tmppi/2*(i-off)/off/2);
			colorvalue	= int(maxr-(maxr-colorr)*rate)+256*int(maxg-(maxg-colorg)*rate)+65536*int(maxb-(maxb-colorb)*rate);
		}
		if(bVertical)
		{
			DrawLine(pDC,rect.left+i,rect.top,rect.left+i,rect.bottom,COLORREF(colorvalue));
			DrawLine(pDC,rect.left+i,rect.bottom,rect.left+i,rect.top,COLORREF(colorvalue));
		}
		else
		{
			DrawLine(pDC,rect.left,rect.top+i,rect.right,rect.top+i,COLORREF(colorvalue));
			DrawLine(pDC,rect.right,rect.top+i,rect.left,rect.top+i,COLORREF(colorvalue));
		}
	}
}

void Gdi::Draw3DBar2(CDC *pDC,CRect rect,COLORREF baseclr,BOOL bVertical,BOOL bEmpty)
{
	if(rect.Height()<=0 || rect.Width()<=0) return;

	if(bEmpty)
	{
		DrawLine(pDC,rect.left,rect.top,rect.right,rect.top,baseclr);
		DrawLine(pDC,rect.right,rect.top,rect.right,rect.bottom,baseclr);
		DrawLine(pDC,rect.right,rect.bottom,rect.left,rect.bottom,baseclr);
		DrawLine(pDC,rect.left,rect.bottom,rect.left,rect.top,baseclr);
		return;
	}

	int colorr	= int(baseclr)%256;
	int colorg	= int(baseclr)/256%256;
	int colorb	= int(baseclr)/256/256%256;
	int colorvalue	= 1<<24-1;
	double tmppi = 3.1415926535898;
	double rate = 0;
	int maxr	= colorr/2;
	int maxg	= colorg/2;
	int maxb	= colorb/2;
	int nWidth	= 0;
	if(bVertical) nWidth = rect.Width();
	else nWidth = rect.Height();
	double off	= nWidth/3.0f;
	int i		= 0;
	for(i=0;i<nWidth;i++)
	{
		if(i<=off)
		{					
			rate		= sin(tmppi/2*i/off);
			colorvalue	= int((maxr-colorr)*rate+colorr)+256*int((maxg-colorg)*rate+colorg)+65536*int((maxb-colorb)*rate+colorb);
		}
		else
		{
			rate		= 1-sin(tmppi/2-tmppi/2*(i-off)/off/2);
			colorvalue	= int(maxr-(maxr-colorr)*rate)+256*int(maxg-(maxg-colorg)*rate)+65536*int(maxb-(maxb-colorb)*rate);
		}
		if(bVertical)
		{
			DrawLine(pDC,rect.left+i,rect.top,rect.left+i,rect.bottom,COLORREF(colorvalue));
			DrawLine(pDC,rect.left+i,rect.bottom,rect.left+i,rect.top,COLORREF(colorvalue));
		}
		else
		{
			DrawLine(pDC,rect.left,rect.top+i,rect.right,rect.top+i,COLORREF(colorvalue));
			DrawLine(pDC,rect.right,rect.top+i,rect.left,rect.top+i,COLORREF(colorvalue));
		}
	}
}

void Gdi::Draw3DBar3(CDC *pDC,CRect rect,COLORREF baseclr,BOOL bVertical,BOOL bEmpty,BOOL b3D)
{
	if(rect.Height()<=0 || rect.Width()<=0) return;

	if(!b3D)
	{
		DrawFilledRect(pDC, rect, 1.0f, baseclr, baseclr, bVertical, bEmpty);
		return;
	}
	
	if(bEmpty)
	{
		DrawLine(pDC,rect.left,rect.top,rect.right,rect.top,baseclr);
		DrawLine(pDC,rect.right,rect.top,rect.right,rect.bottom,baseclr);
		DrawLine(pDC,rect.right,rect.bottom,rect.left,rect.bottom,baseclr);
		DrawLine(pDC,rect.left,rect.bottom,rect.left,rect.top,baseclr);
		return;
	}
	
	int colorr	= int(baseclr)%256;
	int colorg	= int(baseclr)/256%256;
	int colorb	= int(baseclr)/256/256%256;
	int colorvalue	= 1<<24-1;
	double tmppi = 3.1415926535898;
	double rate = 0;
	int maxr	= colorr/2;
	int maxg	= colorg/2;
	int maxb	= colorb/2;
	int nWidth	= 0;
	if(bVertical) nWidth = rect.Width();
	else nWidth = rect.Height();
	double off	= nWidth/2.0;
	int i		= 0;
	for(i=0;i<=nWidth;i++)
	{
		if(i<=off)
		{					
			rate		= sin(tmppi/2*i/off);
			rate = pow(rate,1.618);
			colorvalue	= int((maxr-colorr)*rate+colorr)+256*int((maxg-colorg)*rate+colorg)+65536*int((maxb-colorb)*rate+colorb);

			if(bVertical)
				DrawLine(pDC,rect.left+off-i,rect.top,rect.left+off-i,rect.bottom,COLORREF(colorvalue));
			else DrawLine(pDC,rect.left,rect.top+off-i,rect.right,rect.top+off-i,COLORREF(colorvalue));
		}
		else
		{
			rate		= sin(tmppi/2*(i-off)/off);
			rate = pow(rate,1.618);
			colorvalue	= int((maxr-colorr)*rate+colorr)+256*int((maxg-colorg)*rate+colorg)+65536*int((maxb-colorb)*rate+colorb);

			if(bVertical)
				DrawLine(pDC,rect.left+i,rect.top,rect.left+i,rect.bottom,COLORREF(colorvalue));
			else DrawLine(pDC,rect.left,rect.top+i,rect.right,rect.top+i,COLORREF(colorvalue));
		}
	}
}

void Gdi::DrawFilledRect(CDC *pDC,CRect rect,float FillRate,COLORREF crRect,COLORREF crFilled,BOOL bVertical,BOOL bEmpty)
{
	CPen Pen1,Pen2;
	int i = 0;
	Pen1.CreatePen(PS_SOLID,1,crRect);
	Pen2.CreatePen(PS_SOLID,1,crFilled);
	CPen *OldPen =pDC->SelectObject(&Pen2);

	if(rect.Height()<=0 || rect.Width()<=0)
	{
		pDC->SelectObject(&Pen1);
		if(bVertical)
		{
			pDC->MoveTo(rect.left,rect.bottom);
			pDC->LineTo(rect.right,rect.bottom);
			pDC->LineTo(rect.left,rect.bottom);
		}
		else
		{
			pDC->MoveTo(rect.left,rect.top);
			pDC->LineTo(rect.left,rect.bottom);
			pDC->LineTo(rect.left,rect.top);
		}
	}
	else
	{
		pDC->SelectObject(&Pen2);
		long lFillLen = 1;
		if(bVertical)
		{
			lFillLen = max(rect.Height()*FillRate,1);
			for(i=lFillLen;i>=0;i--)
			{
				pDC->MoveTo(rect.left,rect.bottom-i);
				pDC->LineTo(rect.right,rect.bottom-i);
			//	pDC->LineTo(rect.left,rect.bottom-i);
				if(bEmpty) break;		//空心的只画1条线
			}
		}
		else
		{
			lFillLen = max(rect.Width()*FillRate,1);
			for(i=lFillLen;i>=0;i--)
			{
				pDC->MoveTo(rect.left+i,rect.top);
				pDC->LineTo(rect.left+i,rect.bottom);
				pDC->LineTo(rect.left+i,rect.top);
				if(bEmpty) break;		//空心的只画1条线
			}
		}
		pDC->SelectObject(&Pen1);
		pDC->MoveTo(rect.left,rect.top);
		pDC->LineTo(rect.left,rect.bottom);
		pDC->LineTo(rect.right,rect.bottom);
		pDC->LineTo(rect.right,rect.top);
		pDC->LineTo(rect.left,rect.top);
	}
	pDC->SelectObject(OldPen);
	Pen1.DeleteObject();
	Pen2.DeleteObject();
}
