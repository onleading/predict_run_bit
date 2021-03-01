//:Ignore
// XTMemDC.h interface for the CXTMemDC class.
//
// This file is a part of the Xtreme Toolkit for MFC.
// ©1998-2002 Codejock Software, All Rights Reserved.
//
// This source code can only be used under the terms and conditions 
// outlined in the accompanying license agreement.
//
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////
//:End Ignore

#if !defined(__XTMEMDC_H__)
#define __XTMEMDC_H__

//:Ignore
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

/////////////////////////////////////////////////////////////////////////////
// CXTMemDC is a CDC extension that helps eliminate screen flicker when windows
// are resized, by painting to an off screen bitmap.  The class then uses 
// CDC::BitBlt to copy the bitmap back into the current device context after all
// items have been painted.

class  CXTMemDC : public CDC  
{
public:
    DECLARE_DYNAMIC(CXTMemDC);
    // Constructs a CXTMemDC object.
    CXTMemDC(
        // Points to the current device context. 
        CDC* pDC,
        // Represents the size of the area to paint.
        const CRect& rect,
        // RGB value that represents the background color
        // of the area to paint, defaults to COLOR_3DFACE. Pass in
        // a value of -1 to disable background painting.
        COLORREF clrColor/*=xtAfxData.clr3DFace*/);

    // Destroys a CXTMemDC object, handles cleanup and de-allocation.
    virtual ~CXTMemDC();

	void Discard()
	{
		m_bValid = FALSE;
	}

    // Get content from the given DC
	void FromDC()
	{
		BitBlt(0, 0, m_rc.Width(), m_rc.Height(), m_pDC, 
            m_rc.left, m_rc.top, SRCCOPY);            
	}

    CBitmap& GetBitmap() { return m_bitmap; }
	
protected:
    CDC*     m_pDC;        // Saves CDC passed in constructor
    CRect    m_rc;         // Rectangle of drawing area.
    CBitmap  m_bitmap;     // Offscreen bitmap
    CBitmap* m_pOldBitmap; // Original GDI object
	BOOL	 m_bValid;	   // flag for autodraw in dtor
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTMEMDC_H__)
