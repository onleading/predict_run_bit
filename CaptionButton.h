// CaptionButton.h: interface for the CCaptionButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTIONBUTTON_H__266B77F5_777F_40BB_8A3C_3498783C8C66__INCLUDED_)
#define AFX_CAPTIONBUTTON_H__266B77F5_777F_40BB_8A3C_3498783C8C66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCaptionButton  
{
public:
	CCaptionButton();
	virtual ~CCaptionButton();
	void Init(UINT IDB_BM,int nImage,COLORREF mask,int bttype,CWnd *pParent,BOOL bChecked=FALSE,LPCTSTR str="");
	void Draw();
	void EraseBkgnd();
	void SetRect(int x1,int y1,int x2,int y2);
	void SetRect(CRect rc);
	void OffsetRect(int x,int y);
	BOOL IsInRect(CPoint pt);
	BOOL IsChecked();
	void SetState(int n);
	int  GetState();
	void SetPressed(BOOL bPressed=TRUE);
	BOOL IsPressed();
	void SetCheck(BOOL bCheck=TRUE);
	void EnableWindow(BOOL en);
	BOOL IsEnable();
	void ShowWindow(BOOL sw);
	BOOL IsShow();
	void SetWindowText(LPCTSTR str);

	CWnd *m_pParent;
	CRect m_rc;	//按钮的有效范围,以窗口左上角为坐标原点
	int m_state;	//按钮的状态对应的位图索引号
	BOOL m_bChecked;	//处于类型1时有效,表示按钮是否处于check状态
	BOOL m_bPressed;	//按钮是否正被按下,无论鼠标是否在按钮区域内
	BOOL m_bShow;		//是否显示按钮
	CImageList m_IL;	//按钮的位图资源
	int m_type;		//按钮的类型:0(普通,over,down,disable4种状态),1(比0类型多了check对应的4种状态,共8种状态)
	CString m_str;	//按钮文字

};


/////////////////////////////////////////////////////////////////////////////
// CCaptionButtonDlg dialog

class CCaptionButtonDlg : public CDialog
{
	DECLARE_DYNAMIC(CCaptionButtonDlg);
	// Construction
public:
	CCaptionButtonDlg();
//	CCaptionButtonDlg(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CCaptionButtonDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	void DrawCaptionButtons();
	void GetCursorPosToWnd(CPoint &pt);
	BOOL m_drawbk;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptionButtonDlg)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	CCaptionButton m_bt1,m_bt2;
	virtual void OnCaptionButton1();
	virtual void OnCaptionButton2();
	virtual void InitCaptionButtons();
	// Generated message map functions
	//{{AFX_MSG(CCaptionButtonDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CAPTIONBUTTON_H__266B77F5_777F_40BB_8A3C_3498783C8C66__INCLUDED_)
