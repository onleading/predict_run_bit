// ComDoc.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ComDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComDoc

IMPLEMENT_DYNCREATE(CComDoc, CDocument)

CComDoc::CComDoc()
{
}

BOOL CComDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CComDoc::~CComDoc()
{
}


BEGIN_MESSAGE_MAP(CComDoc, CDocument)
	//{{AFX_MSG_MAP(CComDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComDoc diagnostics

#ifdef _DEBUG
void CComDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CComDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CComDoc serialization

void CComDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CComDoc commands