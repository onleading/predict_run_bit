// MyInterface.cpp : implementation file
//
#include "stdafx.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HMODULE hinst_msimg32=NULL;
HMODULE hinst_User32=NULL;

CMyInterface g_GUI;

/////////////////////////////////////////////////////////////////////////////
// CMyInterface

CMyInterface::CMyInterface()
{
	hinst_msimg32=AfxLoadLibrary(_T("msimg32.dll"));
	if( hinst_msimg32 )
		pGradientFill = ((FktGradientFill) GetProcAddress( hinst_msimg32, "GradientFill" ));
	
	hinst_User32=AfxLoadLibrary(_T("USER32.DLL"));
	if( hinst_User32 )
		pSetLayeredWindowAttributes=(lpfn)GetProcAddress(hinst_User32,"SetLayeredWindowAttributes");
}

CMyInterface::~CMyInterface()
{
	if(hinst_msimg32) AfxFreeLibrary(hinst_msimg32);
	if(hinst_User32) AfxFreeLibrary(hinst_User32);
}

void CMyInterface::DrawGradient(CDC* pDC,CRect& Rect,
                  COLORREF StartColor,COLORREF EndColor, 
                  BOOL bHorizontal)
{
	if(pGradientFill)
	{
		TRIVERTEX vert[2] ;
		GRADIENT_RECT gRect;
		
		vert [0].y = Rect.top;
		vert [0].x = Rect.left;
		
		vert [0].Red    = COLOR16(COLOR16(GetRValue(StartColor))<<8);
		vert [0].Green  = COLOR16(COLOR16(GetGValue(StartColor))<<8);
		vert [0].Blue   = COLOR16(COLOR16(GetBValue(StartColor))<<8);
		vert [0].Alpha  = 0x0000;
		
		vert [1].y = Rect.bottom;
		vert [1].x = Rect.right;
		
		vert [1].Red    = COLOR16(COLOR16(GetRValue(EndColor))<<8);
		vert [1].Green  = COLOR16(COLOR16(GetGValue(EndColor))<<8);
		vert [1].Blue   = COLOR16(COLOR16(GetBValue(EndColor))<<8);
		vert [1].Alpha  = 0x0000;
		
		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;
		
		if(bHorizontal)
			pGradientFill(pDC->m_hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
		else
			pGradientFill(pDC->m_hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
	}
	else	// for running under win95 and WinNt 4.0 without loading Msimg32.dll
	{
		BYTE StartRed   = GetRValue(StartColor);
		BYTE StartGreen = GetGValue(StartColor);
		BYTE StartBlue  = GetBValue(StartColor);
		
		BYTE EndRed    = GetRValue(EndColor);
		BYTE EndGreen  = GetGValue(EndColor);
		BYTE EndBlue   = GetBValue(EndColor);
		
		int n = (bHorizontal)?Rect.Width():Rect.Height();
		
		// only need for the rest, can be optimized
		{
			// We need a brush (can be doted)
			CBrush TempBrush(EndColor);
			pDC->FillRect(Rect,&TempBrush);
		}
		n-=4;
		for(int dn=0;dn<=n;dn+=4)
		{
			BYTE ActRed = (BYTE)(MulDiv(int(EndRed)-StartRed,dn,n)+StartRed);
			BYTE ActGreen = (BYTE)(MulDiv(int(EndGreen)-StartGreen,dn,n)+StartGreen);
			BYTE ActBlue = (BYTE)(MulDiv(int(EndBlue)-StartBlue,dn,n)+StartBlue);
			
			CRect TempRect; 
			if(bHorizontal)
			{
				TempRect = CRect(CPoint(Rect.left+dn,Rect.top),CSize(4,Rect.Height()));
			}
			else
			{
				TempRect = CRect(CPoint(Rect.left,Rect.top+dn),CSize(Rect.Width(),4));
			}
			CBrush TempBrush(RGB(ActRed,ActGreen,ActBlue));
			pDC->FillRect(TempRect,&TempBrush);
		}
	}
}

COLORREF CMyInterface::LightenScaleColor( long lScale, COLORREF lColor)
{ 
	long R = MulDiv(255-GetRValue(lColor),lScale,255)+GetRValue(lColor);
	long G = MulDiv(255-GetGValue(lColor),lScale,255)+GetGValue(lColor);
	long B = MulDiv(255-GetBValue(lColor),lScale,255)+GetBValue(lColor);
	
	return RGB(R, G, B); 
}

COLORREF CMyInterface::DarkenScaleColor( long lScale, COLORREF lColor)
{ 
	long R = MulDiv(GetRValue(lColor),(255-lScale),255);
	long G = MulDiv(GetGValue(lColor),(255-lScale),255);
	long B = MulDiv(GetBValue(lColor),(255-lScale),255);
	
	return RGB(R, G, B); 
}

COLORREF CMyInterface::GrayColor(COLORREF crColor)
{ 
	int Gray  = (((int)GetRValue(crColor)) + GetGValue(crColor) + GetBValue(crColor))/3;
	return RGB( Gray,Gray,Gray);
}

CSize CMyInterface::GetBitmapSize(CBitmap *psrcbm)
{
	if(!psrcbm || !psrcbm->m_hObject) return CSize(0,0);

	BITMAP szbitmap;
	psrcbm->GetBitmap(&szbitmap);
	return CSize(szbitmap.bmWidth,szbitmap.bmHeight);
}

//根据位图资源ID获得其尺寸
CSize CMyInterface::GetBitmapSize(long ID)
{
	if(ID==0) return CSize(0,0);
	CBitmap bitmap;
	
	try
	{
		if(!bitmap.LoadBitmap(ID))
			throw 1;
	}
	catch (...)
	{
		bitmap.DeleteObject();
		return CSize(0,0);
	}
	CSize sz=GetBitmapSize(&bitmap);	
	bitmap.DeleteObject();
	return sz;
}
CSize CMyInterface::GetBitmapSize(LPCTSTR resName)
{
	if(!resName) return CSize(0,0);
	CBitmap bitmap;
	try
	{
		if(!bitmap.LoadBitmap(resName))
			throw 1;
	}
	catch (...)
	{
		bitmap.DeleteObject();
		return CSize(0,0);
	}
	CSize sz=GetBitmapSize(&bitmap);
	bitmap.DeleteObject();
	return sz;
}

//参数是:CDC,位图左上角要画到的窗口坐标,位图要拉伸成的尺寸,pt2中某数为0时按照原始尺寸
CSize CMyInterface::DrawXPBmp(CDC *pDC,int ID,CPoint pt1,CPoint pt2)
{
	if(ID==0) return CSize(0,0);
	CBitmap bm;
	if(!bm.LoadBitmap(ID)) return CSize(0,0);
	CSize sz=DrawXPBmp(pDC,&bm,pt1,pt2);
	bm.DeleteObject();
	return sz;
}

CSize CMyInterface::DrawXPBmp(CDC *pDC,LPCTSTR resName,CPoint pt1,CPoint pt2)
{
	if(!resName) return CSize(0,0);
	CBitmap bm;
	if(!bm.LoadBitmap(resName)) return CSize(0,0);
	CSize sz=DrawXPBmp(pDC,&bm,pt1,pt2);
	bm.DeleteObject();
	return sz;
}

CSize CMyInterface::DrawXPBmp(CDC *pDC,CBitmap *pbm,CPoint pt1,CPoint pt2)
{
	if(!pbm || !pbm->m_hObject) return CSize(0,0);

	CDC *memdc=new CDC;
	CRect rect;
	BITMAP szbitmap;
	
	pbm->GetBitmap(&szbitmap);
	CSize size(szbitmap.bmWidth,szbitmap.bmHeight);
	memdc->CreateCompatibleDC(pDC);
	CBitmap *oldbitmap=memdc->SelectObject(pbm);
	if(pt2.x==0 && pt2.y==0)
		pDC->BitBlt(pt1.x,pt1.y,size.cx,size.cy,memdc,0,0,SRCCOPY);
	else
		pDC->StretchBlt(pt1.x,pt1.y,pt2.x?pt2.x:size.cx,pt2.y?pt2.y:size.cy,memdc,0,0,size.cx ,size.cy ,SRCCOPY);
	memdc->SelectObject(oldbitmap);
	memdc->DeleteDC();
	TDEL(memdc);
	return CSize(pt2.x?pt2.x:size.cx,pt2.y?pt2.y:size.cy);
}

extern "C"
{
	void CMyInterface::GetBitmapSize(long ID,CSize &sz)
	{
		sz=GetBitmapSize(ID);
	}

	void CMyInterface::GetBitmapSize(LPCTSTR resName,CSize &sz)
	{
		sz=GetBitmapSize(resName);
	}
	
	void CMyInterface::GetBitmapSize(CBitmap *pbm,CSize &sz)
	{
		sz=GetBitmapSize(pbm);
	}
	
	void CMyInterface::DrawXPBmp(CDC *pDC,int ID,CPoint pt1,CPoint pt2,CSize &sz)
	{
		sz=DrawXPBmp(pDC,ID,pt1,pt2);
	}
	
	void CMyInterface::DrawXPBmp(CDC *pDC,LPCTSTR resName,CPoint pt1,CPoint pt2,CSize &sz)
	{
		sz=DrawXPBmp(pDC,resName,pt1,pt2);
	}
	
	void CMyInterface::DrawXPBmp(CDC *pDC,CBitmap *pbm,CPoint pt1,CPoint pt2,CSize &sz)
	{
		sz=DrawXPBmp(pDC,pbm,pt1,pt2);
	}
}

void CMyInterface::CopyBitmap(CBitmap &src_bm,CBitmap &new_bm)
{
	if((HBITMAP)new_bm)
		new_bm.DeleteObject();
	CDC memdc,*pDC;
	pDC=CDC::FromHandle(::GetDC(NULL));
	memdc.CreateCompatibleDC(pDC);
	BITMAP tmpbp;
	src_bm.GetBitmap(&tmpbp);
	new_bm.CreateCompatibleBitmap(pDC,tmpbp.bmWidth,tmpbp.bmHeight);
	CBitmap *poldbm= memdc.SelectObject(&new_bm);
	DrawXPBmp(&memdc,&src_bm,CPoint(0,0),CPoint(0,0));
	memdc.SelectObject(poldbm);
	memdc.DeleteDC();
}

//参数是:CDC,位图左上角要画到的窗口坐标,要画成的尺寸,CDC尺寸,透明色,资源ID
void CMyInterface::DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,UINT ID)
{
	if(ID==0) return;
	CBitmap bm;
	bm.LoadBitmap(ID);
	DrawTransparencyBitmap(pDC,pt,size,wndSize,rgbMask,&bm);
	bm.DeleteObject();
}
void CMyInterface::DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,LPCTSTR resName)
{
	if(!resName) return;
	CBitmap bm;
	bm.LoadBitmap(resName);
	DrawTransparencyBitmap(pDC,pt,size,wndSize,rgbMask,&bm);
	bm.DeleteObject();
}

void CMyInterface::DrawTransparencyBitmap(CDC *pDC,CPoint pt,CSize size,CSize wndSize,COLORREF rgbMask,CBitmap *pbm)
{
	if(!pbm || !pbm->m_hObject) return;

	CDC maskdc,bmdc,bmdc2;
	CBitmap maskdcbm,*old1,*old2,*old3,tmpbm;
	CSize bitmapsize=GetBitmapSize(pbm);
	
	maskdc.CreateCompatibleDC(pDC);
	maskdcbm.CreateBitmap(wndSize.cx,wndSize.cy,1,1,NULL);
	old2=maskdc.SelectObject(&maskdcbm);
	
	bmdc2.CreateCompatibleDC(pDC);
	old1=bmdc2.SelectObject(pbm);
	bmdc.CreateCompatibleDC(pDC);
	tmpbm.CreateCompatibleBitmap(pDC,bitmapsize.cx,bitmapsize.cy);
	old3=bmdc.SelectObject(&tmpbm);
	bmdc.BitBlt(0,0,bitmapsize.cx,bitmapsize.cy,&bmdc2,0,0,SRCCOPY);
	
	bmdc.SetBkColor(rgbMask);//背景色不复制过去
	maskdc.BitBlt(0,0,wndSize.cx,wndSize.cy,&bmdc,0,0,SRCCOPY);
	
	bmdc.SetBkColor(RGB(0,0,0));
	bmdc.SetTextColor(RGB(255,255,255));
	bmdc.BitBlt(0,0,wndSize.cx,wndSize.cy,&maskdc,0,0,SRCAND);
	
	pDC->StretchBlt(pt.x,pt.y,size.cx,size.cy,&maskdc,0,0,bitmapsize.cx,bitmapsize.cy,SRCAND);
	pDC->StretchBlt(pt.x,pt.y,size.cx,size.cy,&bmdc,0,0,bitmapsize.cx,bitmapsize.cy,SRCPAINT);
	
	bmdc.SelectObject(old3);
	bmdc.DeleteDC();
	bmdc2.SelectObject(old1);
	bmdc2.DeleteDC();
	tmpbm.DeleteObject();
	maskdc.SelectObject(old2);
	maskdcbm.DeleteObject();
	maskdc.DeleteDC();
}

//参数是:目标CDC,顶点坐标,要画成的尺寸,原CDC,原CDC尺寸,透明色
void CMyInterface::DrawTransparencyDC(CDC *pDC,CPoint pt,CSize dSize,CDC *pSrc,CSize sSize,COLORREF rgbMask)
{
	CDC maskdc,bmdc;
	CBitmap maskdcbm,bmdcbm,*old2,*old3;
	
	maskdc.CreateCompatibleDC(pDC);
	maskdcbm.CreateBitmap(dSize.cx,dSize.cy,1,1,NULL);
	old2=maskdc.SelectObject(&maskdcbm);
	
	bmdc.CreateCompatibleDC(pDC);
	bmdcbm.CreateCompatibleBitmap(pDC,sSize.cx,sSize.cy);
	old3=bmdc.SelectObject(&bmdcbm);
	bmdc.BitBlt(0,0,sSize.cx,sSize.cy,pSrc,0,0,SRCCOPY);
	
	bmdc.SetBkColor(rgbMask);//背景色不复制过去
	maskdc.BitBlt(0,0,dSize.cx,dSize.cy,pSrc,0,0,SRCCOPY);
	
	bmdc.SetBkColor(RGB(0,0,0));
	bmdc.SetTextColor(RGB(255,255,255));
	bmdc.BitBlt(0,0,dSize.cx,dSize.cy,&maskdc,0,0,SRCAND);
	
	pDC->BitBlt(pt.x,pt.y,dSize.cx,dSize.cy,&maskdc,0,0,SRCAND);
	pDC->BitBlt(pt.x,pt.y,dSize.cx,dSize.cy,&bmdc,0,0,SRCPAINT);
	
	bmdc.SelectObject(old3);
	bmdcbm.DeleteObject();
	bmdc.DeleteDC();
	maskdc.SelectObject(old2);
	maskdcbm.DeleteObject();
	maskdc.DeleteDC();
}

CSize CMyInterface::GetBitmapRgn(CRgn &rgn,CBitmap *psrcbm,COLORREF maskcolor)
{
	if((HRGN)rgn)
		rgn.DeleteObject();
	if(!psrcbm)
	{
		rgn.CreateRectRgn(0,0,0,0);
		return CSize(0,0);
	}
	CSize bmsz;
	CClientDC dc(NULL);
	CDC tempdc;
	CDC *p=&dc;
	tempdc.CreateCompatibleDC(p);
	CBitmap bm;
	bm.CreateCompatibleBitmap(p,1,1);
	tempdc.SelectObject(&bm);
	tempdc.FillSolidRect(0,0,1,1,maskcolor);
	
	BITMAP tempbitmap;
	psrcbm->GetBitmap(&tempbitmap);
	bmsz.cx=tempbitmap.bmWidth;
	bmsz.cy=tempbitmap.bmHeight;
	
	int bytes=tempbitmap.bmBitsPixel/8;//每像素占的字节数
	BYTE *maskbitbytes=new BYTE[bytes];
	bm.GetBitmapBits(bytes,maskbitbytes);
	
	int rowbytes=tempbitmap.bmWidthBytes;//每行占的字节数
	long tbuffsize=tempbitmap.bmWidthBytes*tempbitmap.bmHeight;
	BYTE *tbuff=new BYTE[tbuffsize];
	long unusedbytes=(tempbitmap.bmBitsPixel==32)?1:0;
	psrcbm->GetBitmapBits(tbuffsize,tbuff);
				
	//获取Rgn信息
	RGNDATA* data = (RGNDATA*)new BYTE[sizeof(RGNDATAHEADER)+sizeof(RECT)*tempbitmap.bmWidth*tempbitmap.bmHeight];;
	RECT* rect = (RECT*)data->Buffer;
	data->rdh.dwSize = sizeof(data->rdh);
	data->rdh.iType = RDH_RECTANGLES;
	data->rdh.rcBound.left = 0;
	data->rdh.rcBound.top = 0;
	data->rdh.rcBound.right = tempbitmap.bmWidth;
	data->rdh.rcBound.bottom = tempbitmap.bmHeight;
	data->rdh.nRgnSize = 0;
	data->rdh.nCount = 0;
	
	for(int y=0; y<tempbitmap.bmHeight; y++)
	{
		int tint=memcmp(tbuff+rowbytes*y,maskbitbytes,bytes-unusedbytes);
		int flag=(tint==0)?0:1;
		int s=1;
		for(int x=1; x<tempbitmap.bmWidth; x++)
		{
			if(flag==((memcmp(tbuff+rowbytes*y+bytes*x,maskbitbytes,bytes-unusedbytes)==0)?0:1))
			{
				s++;
			}
			else
			{
				if(flag==1)
				{
					rect->left = x-s;
					rect->top = y;
					rect->right = x ;
					rect->bottom = y + 1;
					rect++;
					data->rdh.nCount++;
					data->rdh.nRgnSize += sizeof(RECT);
				}
				flag=(flag+1)%2;
				s=1;
			}
		}
		if(flag==1)
		{
			rect->left = x-s;
			rect->top = y;
			rect->right = x ;
			rect->bottom = y + 1;
			rect++;
			data->rdh.nCount++;
			data->rdh.nRgnSize += sizeof(RECT);
		}
		
	}
	rgn.CreateFromData(NULL, sizeof(data->rdh) + data->rdh.nRgnSize, data);
	delete[] data;
	delete[] tbuff;
	delete[] maskbitbytes;
	bm.DeleteObject();
	tempdc.DeleteDC();
	return bmsz;
}

CSize CMyInterface::GetBitmapRgn(CRgn &rgn,UINT nResource,COLORREF maskcolor)
{
	if(nResource==0)
	{
		rgn.CreateRectRgn(0,0,0,0);
		return CSize(0,0);
	}
	CBitmap	srcbm;
	srcbm.LoadBitmap(nResource);
	CSize sz=GetBitmapRgn(rgn,&srcbm,maskcolor);
	srcbm.DeleteObject();
	return sz;
}

CSize CMyInterface::GetBitmapRgn(CRgn &rgn,LPCTSTR resourceName,COLORREF maskcolor)
{
	if(!resourceName)
	{
		rgn.CreateRectRgn(0,0,0,0);
		return CSize(0,0);
	}
	CBitmap srcbm;
	srcbm.LoadBitmap(resourceName);
	CSize sz=GetBitmapRgn(rgn,&srcbm,maskcolor);
	srcbm.DeleteObject();
	return sz;
}

void CMyInterface::ShowLastError()
{
	DWORD error = GetLastError();
	if(error)
	{
		LPVOID lpMsgBuf=NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL );
		if(lpMsgBuf)
		{
			// Display the string.
			AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK|MB_ICONINFORMATION );
			// Free the buffer.
			LocalFree( lpMsgBuf );
		}
		else
		{
			CString temp;
			temp.Format(_T("Error message 0x%lx not found"),error);
			// Display the string.
			AfxMessageBox(temp, MB_OK|MB_ICONINFORMATION );
		}
	}
}

Win32Type CMyInterface::IsShellType()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	DWORD winVer=GetVersion();
	if(winVer<0x80000000)
	{/*NT */
		if(!GetVersionEx(&osvi))
		{
			ShowLastError();
		}
		if(osvi.dwMajorVersion==4L)
		{
			return WinNT4;
		}
		else if(osvi.dwMajorVersion==5L && osvi.dwMinorVersion==0L)
		{
			return Win2000;
		}
		else if(osvi.dwMajorVersion==5L && osvi.dwMinorVersion==1L)
		{
			return WinXP;
		}
		return WinNT3;
	}
	else if (LOBYTE(LOWORD(winVer))<4)
	{
		return Win32s;
	}
	
	if(!GetVersionEx(&osvi))
	{
		ShowLastError();
	}
	if(osvi.dwMajorVersion==4L && osvi.dwMinorVersion==10L)
	{
		return Win98;
	}
	else if(osvi.dwMajorVersion==4L && osvi.dwMinorVersion==90L)
	{
		return WinME;
	}
	return Win95;
}

COLORREF CMyInterface::MixedColor(COLORREF ColA,UINT nScale, COLORREF ColB)
{
	int Red   = MulDiv(nScale,GetRValue(ColA),100) + MulDiv(100-nScale,GetRValue(ColB),100);
	int Green = MulDiv(nScale,GetGValue(ColA),100) + MulDiv(100-nScale,GetGValue(ColB),100);
	int Blue  = MulDiv(nScale,GetBValue(ColA),100) + MulDiv(100-nScale,GetBValue(ColB),100);
	return RGB( Red,Green,Blue);
}

HBITMAP CMyInterface::CopyScreen(RECT rect)
{
	HDC hscrdc,hmemdc;			  //屏幕DC
	HBITMAP hbitmap,hOldBitmap;   //位图句柄
	int nx,ny,nx2,ny2;
	int nWidth,nHeight;
	
	if(IsRectEmpty(&rect))
		return NULL;
	
	hscrdc=CreateDC("DISPLAY",NULL,NULL,NULL);
	hmemdc=CreateCompatibleDC(hscrdc);
	
	nx=rect.left;
	ny=rect.top;
	nx2=rect.right;
	ny2=rect.bottom;
	
	
	nWidth=nx2-nx;
	nHeight=ny2-ny;		
	
	hbitmap=CreateCompatibleBitmap(hscrdc,nWidth,nHeight);
	hOldBitmap=(HBITMAP)SelectObject(hmemdc,hbitmap);
	BitBlt(hmemdc,0,0,nWidth,nHeight,hscrdc,0,0,SRCCOPY);
	hbitmap=(HBITMAP)SelectObject(hmemdc,hOldBitmap);
	
	DeleteObject(hbitmap);
	DeleteObject(hscrdc);
	DeleteObject(hmemdc);
	
	return hbitmap;
}

void CMyInterface::CopyToClipboard(HBITMAP hbitmap,HWND hWnd)
{
	if(!OpenClipboard(hWnd)) return;
	EmptyClipboard();
	SetClipboardData(CF_BITMAP,hbitmap);
	CloseClipboard();
}

HBITMAP CMyInterface::DIBToDDB( HANDLE hDIB )
{
	LPBITMAPINFOHEADER	lpbi;
	HBITMAP 		hbm;
	CPalette		pal;
	CPalette*		pOldPal;
	CClientDC		dc(NULL);
	
	if (hDIB == NULL)
		return NULL;
	
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	
	int nColors = lpbi->biClrUsed ? lpbi->biClrUsed : 
	1 << lpbi->biBitCount;
	
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
		bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	// Create and select a logical palette if needed
	if( nColors <= 256 && dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;
		
		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}
		
		pal.CreatePalette( pLP );
		
		delete[] pLP;
		
		// Select and realize the palette
		pOldPal = dc.SelectPalette( &pal, FALSE );
		dc.RealizePalette();
	}
	
	
	hbm = CreateDIBitmap(dc.GetSafeHdc(),		// handle to device context
		(LPBITMAPINFOHEADER)lpbi,	// pointer to bitmap info header 
		(LONG)CBM_INIT,			// initialization flag
		lpDIBBits,			// pointer to initialization data 
		(LPBITMAPINFO)lpbi,		// pointer to bitmap info
		DIB_RGB_COLORS );		// color-data usage 
	
	if (pal.GetSafeHandle())
		dc.SelectPalette(pOldPal,FALSE);
	
	return hbm;
}

HBITMAP CMyInterface::CopyRectBitmap(HBITMAP hsrcBitmap,CRect rect)
{
	HDC hsrcdc,hmemdc;
	
	hsrcdc=CreateCompatibleDC(NULL);
	HBITMAP pOldBitmap2 = (HBITMAP)SelectObject (hsrcdc,hsrcBitmap);
	
	hmemdc=CreateCompatibleDC(NULL);
	HBITMAP	s_hbitmap=CreateCompatibleBitmap (hsrcdc,rect.Width(),rect.Height());
	HBITMAP pOldBitmap=(HBITMAP)SelectObject(hmemdc,s_hbitmap);
	
	BitBlt(hmemdc,0,0,rect.Width(),rect.Height(),hsrcdc,rect.left,rect.top,SRCCOPY);
	s_hbitmap=(HBITMAP)SelectObject(hmemdc,pOldBitmap);
	
	SelectObject(hsrcdc,pOldBitmap2);
	DeleteObject(s_hbitmap);
	DeleteObject(hmemdc);
	DeleteObject(hsrcdc);
	
	return s_hbitmap;
}

void CMyInterface::DrawLine(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF color,int style,int width)
{
	CPen Pen,*OldPen;
	Pen.CreatePen(style,width,color);
	OldPen =pDC->SelectObject(&Pen);
	pDC->MoveTo(x1,y1);
	pDC->LineTo(x2,y2);
	pDC->SelectObject(OldPen);
	Pen.DeleteObject();
}

void CMyInterface::DrawVitalCoolBtn(CDC *pDC,CRect rect,char *str,int downflag,COLORREF txtCor)
{
	CFont NewFont;
	NewFont.CreateFont(15,0,0,0,g_b2000XPSys?FW_NORMAL:FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
	CFont *OldFont = pDC->SelectObject(&NewFont);
	CBrush br(downflag==1? RGB(244,199,94) : ::GetSysColor(COLOR_3DFACE));
	CBrush *pOldBrush = (CBrush *)pDC->SelectObject(&br);
	CPen pen(PS_SOLID, 1, RGB(0,45,45));
	pDC->SetTextColor(txtCor);
	CPen *pOldPen = (CPen *)pDC->SelectObject(&pen);
	pDC->RoundRect(rect, CPoint(4, 4));
	pDC->DrawText(str, rect, DT_CENTER);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(OldFont);
	NewFont.DeleteObject();
	br.DeleteObject();
	pen.DeleteObject();
}

void CMyInterface::DrawColorfulBtn(CDC *pDC,CRect rect,const char *str,COLORREF bkColor,COLORREF txtCor)
{
	pDC->FillSolidRect(rect,bkColor);

	CFont NewFont;
	NewFont.CreateFont(14,0,0,0,g_b2000XPSys?FW_NORMAL:FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
	CFont *OldFont = pDC->SelectObject(&NewFont);
	pDC->SetTextColor(txtCor);
	pDC->DrawText(str, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	
	pDC->SelectObject(OldFont);
	NewFont.DeleteObject();
}

void CMyInterface::GetStrFromLine(CString str,LPCSTR itemName,LPSTR itemValue)
{
	*itemValue=0;
	str+="\r\n";
	int i,j;
	i=str.Find(itemName);
	if(i==-1) *itemValue=0;
	i+=strlen(itemName)+1;
	j=str.Find("\n",i);
	if(j==-1) *itemValue=0;
	CString	r=str.Mid(i,j-i);
	r.TrimLeft();
	r.TrimRight();
	strcpy(itemValue,r);
}

COLORREF CMyInterface::GetColorCfgFromResource(HINSTANCE resModule,LPCSTR resName,LPCSTR itemName,int nIndex)
{
	HRSRC hres=FindResource(resModule,resName,RT_HTML);
	if(!hres) return 0;
	HGLOBAL hglobal=LoadResource(resModule,hres);
	CString buff=(char *)LockResource(hglobal);
	char tmp[255];
	CString itemname_str;
	if(nIndex>=0)
		itemname_str.Format("%s_%d",itemName,nIndex);
	else
		itemname_str=itemName;
	GetStrFromLine(buff,itemname_str,tmp);
	long itemvalue=0;
	if(tmp[0]=='0' && tmp[1]=='x')
		sscanf(tmp,"%x",&itemvalue);
	else
		sscanf(tmp,"%d",&itemvalue);
	return itemvalue;
}
