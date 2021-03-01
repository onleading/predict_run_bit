#if !defined(AFX_TESTYIELDWND_H__9B868CDB_20AE_4FB9_AD37_2206C999A5EB__INCLUDED_)
#define AFX_TESTYIELDWND_H__9B868CDB_20AE_4FB9_AD37_2206C999A5EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestYieldWnd.h : header file
//
#pragma warning(disable:4786)
#pragma warning(disable:4788)
#include <map>
using namespace std;

#include "MyDetailDlg.h" 
/////////////////////////////////////////////////////////////////////////////
// CTestYieldWnd window
#define MAX_PER_PAGE	60
struct KInfo
{
	int xPos;
	int yHighPos;
	int yOpenPos;
	int yLowPos;
	int yClosePos;
	int yVolPos;
};

class CTestYieldWnd : public CWnd
{
// Construction
public:
	CTestYieldWnd();

// Attributes
public:
	CMyDetailDlg *m_pShowDetail;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestYieldWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestYieldWnd();

	void SetData(map<long, testDetail, less_equal<long> > &dataMap, vector<testDetail> &dataDetails);
	void SetOrigin(CDC *pDC);
	void DrawProfit(CDC *pDC);
	void DrawKLine(CDC *pDC, int nk, int nOff);
	void DrawVol(CDC *pDC, int nk, int nOff);
	void DrawMainMa(CDC *pDC);
	void DrawSubMa(CDC *pDC);

	// Generated message map functions

private:
	int m_maxPerPage;
	CDC m_memDC;	//作出内存DC
	float m_fSplitRadio;		//区域分割 
	CRect m_drawBorder;
	CRect m_mainBorder;
	CRect m_volBorder;
	int m_nStart, m_nEnd;
	int m_nCursorPos;
	map<long, testDetail, less_equal<long> > m_allTestData;
	vector<testDetail> m_sTestData;
	vector<KInfo> m_aPt;
	vector<KInfo> m_allPt;
	BOOL m_bHiLight;
	int m_iCurPos;

	void GetWndHW(int &height, int &withd);
	void ShowCurInfo(CDC *pDC);
	void ShowHelpTip(CDC* pDC);

protected:
	//{{AFX_MSG(CTestYieldWnd)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTYIELDWND_H__9B868CDB_20AE_4FB9_AD37_2206C999A5EB__INCLUDED_)
