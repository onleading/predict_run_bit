#if !defined(AFX_COLORPAGE_H__722C1225_1F72_11D3_98A5_0088CC032229__INCLUDED_)
#define AFX_COLORPAGE_H__722C1225_1F72_11D3_98A5_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorPage dialog

class CColorPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CColorPage)

	void DrawColorBox(CDC *pDC,CPoint point = CPoint(0,0));
	void SetColor(COLORREF Color);
// Construction
public:
	CColorPage();
	~CColorPage();

// Dialog Data
	//{{AFX_DATA(CColorPage)
	enum { IDD = IDD_COLOR_PAGE };
	CListBox	m_SchemeItem_List;
	CComboBox	m_Scheme_Combo;
	CButton	m_Preview_Box;
	CButton	m_Color_Box;
	//}}AFX_DATA
	struct SYSTEMCOLOR UI;				//当前配置的方案
	int	   nCurSelItem;
	int	   nCurSelScheme;
	CRect  PreviewRect;
	BOOL   bNoSelf;  //是否非自定义方案
	BOOL   bModify;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnOthercolor();
	afx_msg void OnSelchangeSchemeCombo();
	afx_msg void OnSelchangeSchemeitemList();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSchemeSaveas();
	afx_msg void OnDeleteScheme();
	afx_msg void OnDefaultfont();
	afx_msg void OnFontCfg(UINT nID);
	afx_msg void OnFont();
	afx_msg void OnHomepageBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPAGE_H__722C1225_1F72_11D3_98A5_0088CC032229__INCLUDED_)
