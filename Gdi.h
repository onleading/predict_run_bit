#ifndef GDI_H_
#define GDI_H_
#include "interface.h"

#define MAX_PEN_NUM		24
#define MAX_BRUSH_NUM	12
#define MAX_FONT_NUM	11

#define AXIS_PEN		0
#define AXIS2_PEN		1
#define DOTAXIS_PEN		2
#define TIME_PEN		3
#define TXT_PEN			4
#define KP_PEN			5
#define KN_PEN			6
#define UP_PEN			7
#define DOWN_PEN		8
#define LEVEL_PEN		9
#define US_PEN			10
#define VOL_PEN			11
#define ZB1_PEN			12
#define ZB2_PEN			13
#define ZB3_PEN			14
#define ZB4_PEN			15
#define ZB5_PEN			16
#define ZB6_PEN			17
#define ZST_UP_PEN		18
#define ZST_DOWN_PEN	19
#define ZST_LEVEL_PEN	20
#define GRIDLINE_PEN	21
#define GRIDSEL_PEN		22
#define OVERLAP_PEN		23

#define KP_BRUSH		0
#define KN_BRUSH		1
#define UP_BRUSH		2
#define LEVEL_BRUSH		3
#define DOWN_BRUSH		4
#define INVERSE_BRUSH	5
#define BACK_BRUSH		6
#define OVERLAP_BRUSH	7
#define FIX_RED_BRUSH	8
#define FIX_GREEN_BRUSH	9
#define GRAYBACK_BRUSH	10
#define AXIS_BRUSH		11

#define LIST_FONT		0
#define GRAPH_FONT		1
#define BOLDGRAPH_FONT	2
#define INFO_FONT		3
#define SMALL_FONT		4
#define SMALL2_FONT		5
#define MEDIUM_FONT		6
#define BIGHQ1_FONT		7
#define BIGHQ2_FONT		8
#define BIGHQ3_FONT		9
#define BIGHQ4_FONT		10

extern COLORREF l_KLineColor[17];/*从第六个开始是叠加股票的颜色*/

class Gdi
{
public:
	Gdi();
	~Gdi();
private:
	CPen	m_Pen[MAX_PEN_NUM + MAXKCOLOR];
	CBrush	m_Brush[MAX_BRUSH_NUM + MAXKCOLOR];
	CFont	m_Font[MAX_FONT_NUM];
	COLORREF BRUSH_PENCOLOR[11];
private:
	BOOL ClipTest(double p,double q,double *u1,double *u2);
	void GetYzmOffset(int &nRetX,int &nRetY,int nX,int nY);
public:
	void CreateGdiObject();
	void ReleaseGdiObject();
	void ReCreateGdiObject();

	CPen* SelectPen(CDC *pDC,int i);
	void RestorePen(CDC *pDC,CPen *oldpen);
	CBrush* SelectBrush(CDC *pDC,int i);
	void RestoreBrush(CDC *pDC,CBrush *oldbrush);
	CFont * SelectFont(CDC *pDC,int i);
	void RestoreFont(CDC *pDC,CFont *oldfont);

	BOOL IsBlackColorScheme(BOOL bBkColor);
	void DrawLine(CDC * pDC, int x1, int y1, int x2, int y2);
	void DrawLine(CDC * pDC, int x1, int y1, int x2, int y2,COLORREF color,int style=PS_SOLID,int width=1);
	void DotLine(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF fColor,int space = 4);
	void DrawRect(CDC *pDC,int left,int top,int right,int bottom);
	void DrawRect(CDC *pDC,int left,int top,int right,int bottom,COLORREF color,int style=PS_SOLID,int width=1);
	void DrawRect(CDC * pDC,LPCRECT lpRect);
	void DrawRect(CDC *pDC,LPCRECT lpRect,COLORREF color,int style=PS_SOLID,int width=1);
	void DrawDotRect(CDC *pDC,LPCRECT lpRect,COLORREF color,int space = 4);
	void DrawDotRect(CDC *pDC,int left,int top,int right,int bottom,COLORREF color,int space = 4);

	void Bar(CDC *pDC,int left,int top,int right,int bottom,int whichBrush);
	void Bar(CDC *pDC,LPCRECT lpRect,int whichBrush);
	void Bar(CDC *pDC,int left,int top,int right,int bottom,COLORREF color);

	CSize DisplayText(CDC *pDC,int x,int y,COLORREF fColor,
					   const char *fmt,...);
	CSize DisplayText(CDC *pDC,int x,int y,COLORREF fColor,
					   int rectwidth,const char *fmt,...);
	CSize DisplayColorVol(CDC *pDC,int x,int y,int inoutflag,int rectwidth,DWORD lVol);
	CSize DisplayText(CDC *pDC,CRect rect,COLORREF fColor,
						UINT nFormat,const char *fmt,...);
	CSize DisplaySameWidtchText(CDC *pDC,int x,int y,COLORREF fColor,
					   const char *fmt,...);
	//postfix_flag后缀标记,为0表示无后缀,1表示后面有%
	CSize NoBjPrintf(CDC * pDC,int x,int y,COLORREF fColor,
					   float f1,int xsflag,int rectwidth = 0,int postfix_flag=0);
	CSize BjPrintf(CDC *pDC,int x,int y,
					   float f1,float f2,int xsflag,int rectwidth = 0,int postfix_flag=0);
	CSize BjPrintf(CDC *pDC,int x,int y,
						long l1,long l2,int rectwidth = 0,int postfix_flag=0);
	void DrawO(CDC *pDC,int x,int y, COLORREF *clrFill = NULL, int nHGS=3);
	void DrawX(CDC *pDC,int x,int y, int nHGS=3);
	void DrawSwim(CDC *pDC,CRect rect,char *Content);

	void ClipLine(CDC *pDC,int x1,int y1,int x2,int y2,CRect rc,COLORREF color,int nDrawMode=R2_COPYPEN,int style=PS_SOLID,int width=1);

	void DrawYZMNumber(CDC *pDC,CRect rect,int flag,char nColorYzm);
	
	void Draw3DBar(CDC *pDC,CRect rect,COLORREF baseclr,BOOL bVertical=TRUE,BOOL bEmpty=FALSE);
	void Draw3DBar2(CDC *pDC,CRect rect,COLORREF baseclr,BOOL bVertical=TRUE,BOOL bEmpty=FALSE);
	void Draw3DBar3(CDC *pDC,CRect rect,COLORREF baseclr,BOOL bVertical=TRUE,BOOL bEmpty=FALSE,BOOL b3D=TRUE);
	void DrawFilledRect(CDC *pDC,CRect rect,float FillRate,COLORREF crRect,COLORREF crFilled,BOOL bVertical=TRUE,BOOL bEmpty=FALSE);
};

#endif
