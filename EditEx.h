#ifndef __EDITED__H__
#define __EDITED__H__
#if _MSC_VER > 1000
#pragma once
#endif

class CEditEx : public CEdit
{
public:
	CEditEx();
	void setPermitStyle(int style);
	virtual ~CEditEx();
	enum enumPERMITSTYLE{ PMTSTY_ANY,PMTSTY_NUM,PMTSTY_NAME,PMTSTY_FILEPATH,PMTSTY_FILENAME,PMTSTY_HEX };
public:
	//{{AFX_VIRTUAL(CEditEx)
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CEditEx)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	int m_nPermitStyle;
};

//{{AFX_INSERT_LOCATION}}
#endif
