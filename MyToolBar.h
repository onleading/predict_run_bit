#ifndef MYTOOLBAR_H
#define MYTOOLBAR_H

class CMyToolBar : public CToolBar
{
// Constructor
public:
	CMyToolBar();
	void SetColumns(UINT nColumns);
	UINT GetColumns() { return m_nColumns; };

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CMyToolBar();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	UINT m_nColumns;

// Generated message map functions
protected:
	//{{AFX_MSG(CMyToolBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
