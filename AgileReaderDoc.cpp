// AgileReaderDoc.cpp : implementation of the CAgileReaderDoc class
//

#include "stdafx.h"
#include "AgileReaderDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderDoc

IMPLEMENT_DYNCREATE(CAgileReaderDoc, CDocument)

BEGIN_MESSAGE_MAP(CAgileReaderDoc, CDocument)
	//{{AFX_MSG_MAP(CAgileReaderDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderDoc construction/destruction

CAgileReaderDoc::CAgileReaderDoc()
{
	// TODO: add one-time construction code here

}

CAgileReaderDoc::~CAgileReaderDoc()
{
}

BOOL CAgileReaderDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAgileReaderDoc serialization

void CAgileReaderDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderDoc diagnostics

#ifdef _DEBUG
void CAgileReaderDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAgileReaderDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderDoc commands
