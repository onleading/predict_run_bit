// AgileReaderDoc.h : interface of the CAgileReaderDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AgileReaderDOC_H__AFE460C4_C1A0_4EE3_97AF_D3BB1281E4DB__INCLUDED_)
#define AFX_AgileReaderDOC_H__AFE460C4_C1A0_4EE3_97AF_D3BB1281E4DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAgileReaderDoc : public CDocument
{
protected: // create from serialization only
	CAgileReaderDoc();
	DECLARE_DYNCREATE(CAgileReaderDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgileReaderDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgileReaderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAgileReaderDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AgileReaderDOC_H__AFE460C4_C1A0_4EE3_97AF_D3BB1281E4DB__INCLUDED_)
