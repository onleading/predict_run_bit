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
	CRect m_rc;	//��ť����Ч��Χ,�Դ������Ͻ�Ϊ����ԭ��
	int m_state;	//��ť��״̬��Ӧ��λͼ������
	BOOL m_bChecked;	//��������1ʱ��Ч,��ʾ��ť�Ƿ���check״̬
	BOOL m_bPressed;	//��ť�Ƿ���������,��������Ƿ��ڰ�ť������
	BOOL m_bShow;		//�Ƿ���ʾ��ť
	CImageList m_IL;	//��ť��λͼ��Դ
	int m_type;		//��ť������:0(��ͨ,over,down,disable4��״̬),1(��0���Ͷ���check��Ӧ��4��״̬,��8��״̬)
	CString m_str;	//��ť����

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
