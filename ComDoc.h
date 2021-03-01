#if !defined(AFX_COMDOC_H__65DE8562_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_COMDOC_H__65DE8562_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComDoc document

class CComDoc : public CDocument
{
protected:
	CComDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CComDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CComDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMDOC_H__65DE8562_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_)
