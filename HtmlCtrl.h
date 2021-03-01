////////////////////////////////////////////////////////////////
// Vckbase Online Journal -- Feb 2000
// Compiles with Visual C++ 6.0, runs on Windows 98 and probably NT too.
//
//
#include "mshtml.h"

class CHtmlCtrl : public CHtmlView {
public:
	CHtmlCtrl();
	virtual ~CHtmlCtrl();

	void	SetOuterExplorer(CHtmlCtrl *pExplorer) { m_pOuterExplorer = pExplorer; }

	BOOL CreateFromWnd(HWND hWnd, UINT nID, CWnd* pParent);

	// Normally, CHtmlView destroys itself in PostNcDestroy,
	// but we don't want to do that for a control since a control
	// is usually implemented as a stack object in a dialog.
	//
	virtual void PostNcDestroy() {  }

	// overrides to bypass MFC doc/view frame dependencies
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// override to trap "app:" pseudo protocol
	virtual void OnBeforeNavigate2( LPCTSTR lpszURL,
		DWORD nFlags,
		LPCTSTR lpszTargetFrameName,
		CByteArray& baPostedData,
		LPCTSTR lpszHeaders,
		BOOL* pbCancel );

	virtual void OnNewWindow2( LPDISPATCH* ppDisp, BOOL* Cancel );

	// override to handle links to "app:mumble...". lpszWhere will be "mumble"
	virtual void OnAppCmd(LPCTSTR lpszWhere);

	DECLARE_MESSAGE_MAP();
	DECLARE_DYNAMIC(CHtmlCtrl)

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void	NavigateEmptyWebPage();
	void	WriteIEFromContent(int infoformat,char *pContent,long &ContentLen);
	void	WriteIE(const char *str);

private:
	HWND		m_hStatic;
	CHtmlCtrl	*m_pInnerExplorer, *m_pOuterExplorer;

public:
	BOOL		m_bNeedTrans2Outer;
};

