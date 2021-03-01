#ifndef _SWITCHEX_H_
#define _SWITCHEX_H_

typedef void (* PMENUCALLBACK)(CWnd *pWnd,CWnd *ProcWnd,int left,int top,int right,int bottom,UINT message,UINT menuid,long whichmenu);

struct	RgnInfoEx
{
	CRect		rcIn;
	CRect		rcOut;
	CString		caption;	//ÿ�������������Ϣ
	CString		tooltip;	//��꾭��ĳ����ǩ��ʱ����ʾ��tooltip����
	BOOL		isMenu;		//�Ƿ���в˵�
	UINT		message;
	union
	{	
		DWORD	wparam;		//��������
		UINT	menuid;		//�˵�ID
	};
	union
	{							//����ǲ˵����Ϳ��������Ǹ���
		long	lparam;
		long	whichmenu;
	};
	long		everclick;

	RgnInfoEx()
	{
		isMenu		= 0;
		menuid		= 0;
		wparam		= 0;
		lparam		= 0;
		everclick	= 0;
		rcIn.SetRect(0,0,0,0);
		rcOut.SetRect(0,0,0,0);
	}
};

typedef enum
{
	SwitchStyle_default=0,			//��ͨ���
	SwitchStyle_samebutton,			//�ȿ�ť���
	SwitchStyle_switchbutton,		//��ť���
	SwitchStyle_switchbutton2,		//��ť���(��һ�ַ��,�ҵ�)
	SwitchStyle_custom1,			//�������а���
	SwitchStyle_Vdefault,			//��ֱ���
	SwitchStyle_Vswitchbutton,
	SwitchStyle_Vsamebutton,
	SwitchStyle_Vcustom1,			
}SwitchStyle;

typedef struct  specialtextcolor
{
	COLORREF	cor;
	BOOL		bSet;
	specialtextcolor()
	{
		bSet = FALSE;
	}
}SpecialTextColor;

const	int		TIP_W		= 13;

class CNewZfjsTab : public CWnd 
{
private:
	SwitchStyle				m_SwitchStyle;		//���
	int						m_nCurNo;			//��ǰѡ�еڼ���
	int						m_nLastNo;
	BOOL					bFirst;				//���ս��봰������
	CToolTipCtrl			m_ToolTip;
	vector<RgnInfoEx>		m_aRgn;				//����
	vector<SpecialTextColor>m_vTextColor;		//ÿ����ǩ�ر�ָ�����ı���ɫ

	//����ɫ ѡ��ɫ û��ѡ��ɫ ����ɫ ѡ������ɫ �ȸб���ɫ �ȸ�����ɫ �߿�ɫ
	COLORREF				m_crBk,m_crMk,m_crUMk,m_crTxt,m_crMTxt,m_crHot,m_crHTxt,m_crLine,m_crTipLine,m_crBtnSelBk;

	HWND					m_MsghWnd;
	CWnd				*	m_pMainWnd;
	PMENUCALLBACK			m_pSwitchMenuCallBack;

public:
	CNewZfjsTab(HWND hWnd,CWnd *pMainWnd = NULL);

	void				SetStyle(SwitchStyle ss=SwitchStyle_default);
	void				SetFontColorSchema(	COLORREF Background,COLORREF SelectedBackground,
											COLORREF unSelectedBackground,COLORREF HotBackground,
											COLORREF LineColor,COLORREF NormalTxtColor,
											COLORREF SelectTxtColor,COLORREF HotTxtColor,COLORREF TipLineColor,
											int Fontsize = 14,char* FontName = "Arial");

	void				SetSpecialTextColor(UINT nIndex,COLORREF cor);
	void				SetMenuCallBack(PMENUCALLBACK pFunc);

	void				InsertSwitchLabel(int index,const char *caption,UINT msg_menu,WPARAM wparam_menuid=0,LPARAM lparam_menuno=0,BOOL isMenu=0,long everclick=0);
	void				InsertSwitchLabel(int index,const char *caption,const char * tooltip,UINT msg_menu,WPARAM wparam_menuid=0,LPARAM lparam_menuno=0,BOOL isMenu=0,long everclick=0);
	void				ModifySwitchLabel(int iWhich,const char *caption,const char * tooltip);
	int					GetNeedWidth();

	int					GetCurLabel() {	return m_nCurNo; }
	int					GetTabNum()	  { return m_aRgn.size(); }
	void				SetWindowHandle(HWND hWnd) { m_MsghWnd = hWnd; }
	CString				GetCaption(int nNo);
	void				SetCurLabel(int nCurNo = -1);
	void				SetRect(int x1,int y1,int x2,int y2);
	void				SetRectV(int x1,int y1,int x2,int y2);
	BOOL				SetWindowPos(const CWnd* pWndInsertAfter, int x,int y,int cx,int cy,UINT nFlags); 

	int					ProcessMsg(UINT Msg,WPARAM wParam=0,LPARAM lParam=0);
	void				OnMouseLeave(WPARAM w,LPARAM l); 

	RgnInfoEx	&		aRgnAt(int index);
	void				ClearAllLabel(void);
public:
	BOOL				PtInRect(POINT point);
	BOOL				m_bhasFLBtn;
	BOOL				m_bhasDPBtn;
	BOOL				IsBlackColorScheme(BOOL bBkColor);
protected:
	CRect				m_rcWnd;
	CRect				m_aTipRct[2];
	CRgn				m_aTipRgn[2];
	CFont				m_Font;
	CBrush				m_brUMk,m_brLine,m_brMk,m_brHot,m_brBk,m_brTipLine;
	int					m_nLeftNo;
	int					m_nAngleDx;
	int					m_nOffset;
	BOOL				m_IsMore;
	int					m_nHotNo;
	vector<CRect>		m_aBackRC[2];
private:
	void				SendOutMsg();
	void				DrawSwitch();
	void				PaintSwitch();
	void				PaintButton();
	void				PaintSwitchV();
	void				PaintButtonV();
	void				PaintSpecialSwitch();
	void				PaintSpecialSwitchV();
	int					CalcRgn(CDC *pDC);
	void				FreeGDIObject();
	int					left,top,right,bottom;
	int					Width( ) const { return (right-left); };
	int					Height() const { return (bottom-top); };
	void				DrawLine(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF color,int style=PS_SOLID,int width=1);
	BOOL				RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName);
// Implementation
public:
	virtual ~CNewZfjsTab();
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Generated message map functions
protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewZfjsTab)
	virtual LRESULT	WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	virtual BOOL	PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNewZfjsTab)
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnPaint();
	afx_msg void	OnTimer(UINT nIDEvent);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
