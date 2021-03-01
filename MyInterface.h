#if !defined(AFX_MYINTERFACE_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_)
#define AFX_MYINTERFACE_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyInterface.h : header file
//

#ifndef TDEL
#define TDEL(a) if(a){delete a;a=NULL;}
#endif

#ifndef CHECK_CRGN_RESULT
#define CHECK_CRGN_RESULT(r) ( ((r)==ERROR || (r)==NULLREGION)?FALSE:TRUE )
#endif

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif
#ifndef LWA_COLORKEY
#define LWA_COLORKEY            0x00000001
#endif
#ifndef LWA_ALPHA
#define LWA_ALPHA               0x00000002
#endif

typedef BOOL (WINAPI* FktGradientFill)( IN HDC, IN PTRIVERTEX, IN ULONG, IN PVOID, IN ULONG, IN ULONG);
typedef BOOL(WINAPI*lpfn)(HWND hWnd,COLORREF cr,BYTE bAlpha,DWORD dwFlags);

enum Win32Type
{
	Win32s,
	WinNT3,
	Win95,
	Win98,
	WinME,
	WinNT4,
	Win2000,
	WinXP
};

#define SRGB(a) RGB(a,a,a)

/////////////////////////////////////////////////////////////////////////////
//class CMyInterface

class CMyInterface
{
// Construction
public:
	CMyInterface();

// Attributes
public:
	FktGradientFill pGradientFill;
	lpfn pSetLayeredWindowAttributes;

// Operations
public:

	//画渐进色
	void DrawGradient(CDC* pDC,CRect& Rect,
		COLORREF StartColor,COLORREF EndColor,BOOL bHorizontal);

	// Function splits a color into its RGB components and
	// transforms the color using a scale 0..255
	COLORREF LightenScaleColor( long lScale, COLORREF lColor);
	COLORREF DarkenScaleColor( long lScale, COLORREF lColor);
	
	//将ColA和ColB按照百分之nScale和100-nScale的比例混合
	COLORREF MixedColor(COLORREF ColA,UINT nScale, COLORREF ColB);
	
	//去色
	COLORREF GrayColor(COLORREF crColor);

	//获得位图尺寸
	CSize GetBitmapSize(long ID);
	CSize GetBitmapSize(LPCTSTR resName);
	CSize GetBitmapSize(CBitmap *psrcbm);
	void GetBitmapSize(long ID,CSize &sz);
	void GetBitmapSize(LPCTSTR resName,CSize &sz);
	void GetBitmapSize(CBitmap *psrcbm,CSize &sz);

	void CopyBitmap(CBitmap &src_bm,CBitmap &new_bm);
	//将位图画到CDC上
	//参数是:CDC,位图左上角要画到的窗口坐标,位图要拉伸成的尺寸,pt2中某数为0时按照原始尺寸
	CSize DrawXPBmp(CDC *pDC,int ID,CPoint pt1,CPoint pt2);
	CSize DrawXPBmp(CDC *pDC,LPCTSTR resName,CPoint pt1,CPoint pt2);
	CSize DrawXPBmp(CDC *pDC,CBitmap *pbm,CPoint pt1,CPoint pt2);
	void DrawXPBmp(CDC *pDC,int ID,CPoint pt1,CPoint pt2,CSize &sz);
	void DrawXPBmp(CDC *pDC,LPCTSTR resName,CPoint pt1,CPoint pt2,CSize &sz);
	void DrawXPBmp(CDC *pDC,CBitmap *pbm,CPoint pt1,CPoint pt2,CSize &sz);
	
	//将一个CDC按照透明模式画到另一个CDC上
	//参数是:目标CDC,顶点坐标,要画成的尺寸,原CDC,原CDC尺寸,透明色
	void DrawTransparencyDC(CDC *pDC,CPoint pt,CSize dSize,CDC *pSrc,CSize sSize,COLORREF rgbMask);

	//将位图按透明模式画到CDC上
	//参数是:CDC,位图左上角要画到的窗口坐标,要画成的尺寸,CDC尺寸,透明色,资源ID
	void DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,UINT ID);
	void DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,LPCTSTR resName);
	void DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,CBitmap *pbm);

	//根据位图取非透明色区域
	//参数是:未初始化的CRgn,位图资源ID,透明色
	CSize GetBitmapRgn(CRgn &rgn,CBitmap *psrcbm,COLORREF maskcolor);
	CSize GetBitmapRgn(CRgn &rgn,UINT nResource,COLORREF maskcolor);
	CSize GetBitmapRgn(CRgn &rgn,LPCTSTR strName,COLORREF maskcolor);
	//弹出对话框显示最近的错误信息
	void ShowLastError();

	//判断操作系统的版本号
	Win32Type IsShellType();
	
	//在指定区域截屏
	HBITMAP CopyScreen(RECT rect);
	
	//拷贝位图到剪贴板
	void CopyToClipboard(HBITMAP hbitmap,HWND hWnd);

	//DIB位图(设备无关)转DDB位图(设备相关)
	HBITMAP DIBToDDB( HANDLE hDIB );

	//取位图的一部分
	HBITMAP CopyRectBitmap(HBITMAP hsrcBitmap,CRect rect);
	
	//画线
	void DrawLine(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF color,int style=PS_SOLID,int width=1);

	//画主框架右上角的按钮
	void DrawVitalCoolBtn(CDC *pDC,CRect rect,char *str,int downflag,COLORREF txtCor=RGB(0,45,45));
	
	//画分时图等地方的按钮
	void DrawColorfulBtn(CDC *pDC,CRect rect,const char *str,COLORREF bkColor,COLORREF txtCor);

	//分析一个配置项
	void GetStrFromLine(CString str,LPCSTR itemName,LPSTR itemValue);
	//从资源里的配置文件中读取一项颜色配置的值
	COLORREF GetColorCfgFromResource(HINSTANCE resModule,LPCSTR resName,LPCSTR itemName,int nIndex=-1);

// Implementation
public:
	virtual ~CMyInterface();
};

extern CMyInterface g_GUI;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYINTERFACE_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_)
