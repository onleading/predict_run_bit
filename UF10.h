#ifndef UF10_H_
#define UF10_H_

#include "UBase.h"
#include "MyRichEdit.h"
#include "FlatButton.h"
#include "webbrowser2.h"

class UF10Ctrl;
class CUF10CtrlWnd;
class UF10
{
	void SetSelColor();									
	void UpdateButton();								
	void UncheckAllButton();							
	int  GetCurSelBtn();								
	void JustifyButton(CRect rcdlg);									

	void GetRemoteFile(const char *Code,short setcode);		

public:
	UF10(LPCOMINFO pComInfo, UF10Ctrl *pF10Ctrl);
	virtual ~UF10();

	BOOL			InitShow(HWND hVessel);				
	void			ArrangeWnd(CRect &rc);				

	int				GetCurrentJBM() {return CurrentJBM;}
	short	GetCurrentGPIndex();
	void	SetStock(MemStkInfo *pStock);
	
	void	Resize();	
	//////////////////////////////////////////////////////////////////////////
	void	OnShowTxt();
	void	OnSelectJBM(UINT nID);								
	void	OnCreate();			
	void	OnRichEditMessage();	
	void	OnClickFLButton(UINT nID);							

	void	OnDraw(CDC* pDC);									
	BOOL	OnEraseBkgnd(CDC* pDC);							

	void	OnGetDataAck(WPARAM wParam,LPARAM lParam);
	void	OnLButtonDown(UINT nFlags, CPoint point);			
protected:
	void	OnSize(UINT nType, int cx, int cy);	
	void	OnDestroy();
public:
	CMyRichEdit m_RichEdit;

	CFont  *m_pFont;
	BOOL	KeyGuyFlag;
	BOOL	m_bQuitting;
	CString m_TxtCurGPCode;
	short	m_TxtCurSetCode;
	int		CurrentJBM;		//CurrentJBM为-1表示通达信资讯

	//用于远程
	short	m_nMainID;
	CFile	theRemoteFile;
	BOOL	bRemoteFileOpen;
	DWORD	nCurRemotePos;

	CFlatButton m_btnF10[MAX_F10CAPTION];	//所有的f10每次最大能同时提供16条信息
	BOOL	m_bLayoutFlag;
	BOOL	m_bCanClickFL;
	char	*m_pTmpBuf;

	short	m_whichfl;
	char	FileBuffer[32*1024+200];
	
	//
	int		m_nMostLeft;
	CRect	m_PreNext_Rect[5];
	
	COLORREF m_SelColor,m_ForeColor,m_BackColor;
	int		 m_nRichEditTop;

	BOOL	m_bDS;

private:
	UF10Ctrl		*m_pUF10Ctrl;
	CUF10CtrlWnd	*m_pUF10CtrlWnd;
	COMINFO			m_ComInfo;
	HWND			m_hVessel;
};

#endif
