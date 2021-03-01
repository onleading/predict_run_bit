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

	//������ɫ
	void DrawGradient(CDC* pDC,CRect& Rect,
		COLORREF StartColor,COLORREF EndColor,BOOL bHorizontal);

	// Function splits a color into its RGB components and
	// transforms the color using a scale 0..255
	COLORREF LightenScaleColor( long lScale, COLORREF lColor);
	COLORREF DarkenScaleColor( long lScale, COLORREF lColor);
	
	//��ColA��ColB���հٷ�֮nScale��100-nScale�ı������
	COLORREF MixedColor(COLORREF ColA,UINT nScale, COLORREF ColB);
	
	//ȥɫ
	COLORREF GrayColor(COLORREF crColor);

	//���λͼ�ߴ�
	CSize GetBitmapSize(long ID);
	CSize GetBitmapSize(LPCTSTR resName);
	CSize GetBitmapSize(CBitmap *psrcbm);
	void GetBitmapSize(long ID,CSize &sz);
	void GetBitmapSize(LPCTSTR resName,CSize &sz);
	void GetBitmapSize(CBitmap *psrcbm,CSize &sz);

	void CopyBitmap(CBitmap &src_bm,CBitmap &new_bm);
	//��λͼ����CDC��
	//������:CDC,λͼ���Ͻ�Ҫ�����Ĵ�������,λͼҪ����ɵĳߴ�,pt2��ĳ��Ϊ0ʱ����ԭʼ�ߴ�
	CSize DrawXPBmp(CDC *pDC,int ID,CPoint pt1,CPoint pt2);
	CSize DrawXPBmp(CDC *pDC,LPCTSTR resName,CPoint pt1,CPoint pt2);
	CSize DrawXPBmp(CDC *pDC,CBitmap *pbm,CPoint pt1,CPoint pt2);
	void DrawXPBmp(CDC *pDC,int ID,CPoint pt1,CPoint pt2,CSize &sz);
	void DrawXPBmp(CDC *pDC,LPCTSTR resName,CPoint pt1,CPoint pt2,CSize &sz);
	void DrawXPBmp(CDC *pDC,CBitmap *pbm,CPoint pt1,CPoint pt2,CSize &sz);
	
	//��һ��CDC����͸��ģʽ������һ��CDC��
	//������:Ŀ��CDC,��������,Ҫ���ɵĳߴ�,ԭCDC,ԭCDC�ߴ�,͸��ɫ
	void DrawTransparencyDC(CDC *pDC,CPoint pt,CSize dSize,CDC *pSrc,CSize sSize,COLORREF rgbMask);

	//��λͼ��͸��ģʽ����CDC��
	//������:CDC,λͼ���Ͻ�Ҫ�����Ĵ�������,Ҫ���ɵĳߴ�,CDC�ߴ�,͸��ɫ,��ԴID
	void DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,UINT ID);
	void DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,LPCTSTR resName);
	void DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,CBitmap *pbm);

	//����λͼȡ��͸��ɫ����
	//������:δ��ʼ����CRgn,λͼ��ԴID,͸��ɫ
	CSize GetBitmapRgn(CRgn &rgn,CBitmap *psrcbm,COLORREF maskcolor);
	CSize GetBitmapRgn(CRgn &rgn,UINT nResource,COLORREF maskcolor);
	CSize GetBitmapRgn(CRgn &rgn,LPCTSTR strName,COLORREF maskcolor);
	//�����Ի�����ʾ����Ĵ�����Ϣ
	void ShowLastError();

	//�жϲ���ϵͳ�İ汾��
	Win32Type IsShellType();
	
	//��ָ���������
	HBITMAP CopyScreen(RECT rect);
	
	//����λͼ��������
	void CopyToClipboard(HBITMAP hbitmap,HWND hWnd);

	//DIBλͼ(�豸�޹�)תDDBλͼ(�豸���)
	HBITMAP DIBToDDB( HANDLE hDIB );

	//ȡλͼ��һ����
	HBITMAP CopyRectBitmap(HBITMAP hsrcBitmap,CRect rect);
	
	//����
	void DrawLine(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF color,int style=PS_SOLID,int width=1);

	//����������Ͻǵİ�ť
	void DrawVitalCoolBtn(CDC *pDC,CRect rect,char *str,int downflag,COLORREF txtCor=RGB(0,45,45));
	
	//����ʱͼ�ȵط��İ�ť
	void DrawColorfulBtn(CDC *pDC,CRect rect,const char *str,COLORREF bkColor,COLORREF txtCor);

	//����һ��������
	void GetStrFromLine(CString str,LPCSTR itemName,LPSTR itemValue);
	//����Դ��������ļ��ж�ȡһ����ɫ���õ�ֵ
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
