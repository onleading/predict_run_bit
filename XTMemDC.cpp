// XTMemDC.cpp : implementation of the CXTMemDC class.
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

#include "StdAfx.h"
#include "XTMemDC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CXTMemDC, CDC);

//////////////////////////////////////////////////////////////////////
// CXTMemDC - implementation

CXTMemDC::CXTMemDC(CDC* pDC, const CRect& rect, COLORREF clrColor/*=xtAfxData.clr3DFace*/)
{
	ASSERT(pDC != NULL);
	m_pDC = pDC;
	
	// If rect is NULL, use the device context's clip box.
	if (rect.IsRectEmpty())
		m_pDC->GetClipBox(&m_rc);
	else
		m_rc.CopyRect(&rect);

    // Create the memory DC, set Map Mode
	if (CreateCompatibleDC(m_pDC))
	{
		SetMapMode(m_pDC->GetMapMode());
		
		// Create a bitmap big enough to hold the window's image
		m_bitmap.CreateCompatibleBitmap(m_pDC, m_rc.Width(), m_rc.Height());
		
		// Select the bitmap into the memory DC
		m_pOldBitmap = SelectObject(&m_bitmap);
		
		// Repaint the background, this takes the place of WM_ERASEBKGND.
		if (clrColor != -1) 
			FillSolidRect(m_rc, clrColor);
		
		m_bValid = TRUE;
	}

	// Something bad happened, could be GDI leak, didn't create device context.
	else
	{
		m_bValid = FALSE;
		m_pOldBitmap = NULL;
	}
}

CXTMemDC::~CXTMemDC()
{
	if (m_bValid)
	{
		// Blt it
		m_pDC->BitBlt(m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(),
			this, 0, 0, SRCCOPY);            
	}

	// Select the original bitmap back in
	if (m_pOldBitmap != NULL)
		SelectObject(m_pOldBitmap);

	DeleteDC();
}

