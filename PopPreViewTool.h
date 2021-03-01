#ifndef POPPREVIEWTOOL_H_
#define POPPREVIEWTOOL_H_
/////////////////////////////////////////////Define Tip
// CTipWindow window
class CPopPreViewTool : public CToolTipCtrl
{	
// Construction
public:
	CPopPreViewTool();

// Attributes
private:
	CWnd	* m_pWnd;
	CRect	m_InToolRect;

public:
	BOOL	m_Activate;
	BOOL    m_NeedZoom;

	CString	m_title_str;
	CString	m_text_str;
// Operations
public:
	void Activate( BOOL bActivate );
	BOOL IsActive();
	void TrackPosition(int xPos, int yPos);
	void TrackPosition(LPPOINT lppt);
	void TrackActivate(BOOL bActivate, LPTOOLINFO lpti);
	int  SetMaxTipWidth(int nWidth);
	RECT SetMargin(LPRECT lpRect);
	int  GetMaxTipWidth();
	void GetMargin(LPRECT lpRect);
	int  GetDelayTime(DWORD dwType);
	int  SetDelayTime(DWORD dwType, int nTime);

	void UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT nIDTool = 0);
	void UpdateTipText(UINT nIDText, CWnd* pWnd, UINT nIDTool = 0);
	void DealText(LPCSTR lpszText);
	BOOL AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool = NULL,UINT nIDTool = 0);
	BOOL AddTool(CWnd* pWnd, LPCTSTR lpszText = LPSTR_TEXTCALLBACK,LPCRECT lpRectTool = NULL, UINT nIDTool = 0);

	void InitPreviewTool(CWnd *pWnd);
	void UpdatePreviewTool(CWnd *pWnd,CString strTipInfo);

	void AddPreviewTool(CWnd *pWnd,CString strTipInfo,LPCRECT pRect,UINT nID);
	void DelPreviewTool(CWnd *pWnd);

	void ShowPreviewTool();

	CRect GetInToolRect() { return m_InToolRect; }
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopPreViewTool)
//	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	CPopPreViewTool(CWnd *pParent);
	virtual ~CPopPreViewTool();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPopPreViewTool)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
