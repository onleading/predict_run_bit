#ifndef UWEB_H_
#define UWEB_H_

#include "Ubase.h"
#include "webbrowser2.h"
#include "BtnST.h"

#define WEB_HEAD_HEIGHT		16

#define GPWEB_STD			0
#define GPWEB_VIEWTHEM		1

#define GPWEB_VIEWTHEM_RP	0	//研报
#define GPWEB_VIEWTHEM_PJ	1	//评级
#define GPWEB_VIEWTHEM_HY	2	//行业
#define GPWEB_VIEWTHEM_GG	3	//公告
#define GPWEB_VIEWTHEM_XW	4	//新闻
#define GPWEB_VIEWTHEM_GW	5	//公告和新闻合并

class UWeb : public UBase
{
	BOOL			m_bCreated;
	CButtonST		m_btnOpen;

	BOOL			m_bSimpleWeb;
	BOOL			m_bGPRelation;
	int				m_nLastCurGP;
protected:
	CWebBrowser2   *m_pExplorer;
	CString			m_URL;
	BOOL			m_bIsWebURL;

	BOOL			StartNavigate();
	BOOL			IsHideStatus();
public:
	UWeb(LPCOMINFO pComInfo);
	virtual ~UWeb();

	virtual BOOL GPChangeNavigate();
	virtual void SendDataReq(){};
	virtual void AfterdDataACK(){};
	virtual void OnTimer(){};
	void SetURL(CString tmpURL,BOOL bAtOnceRun,BOOL bMustGPRelation=FALSE);
	void SetSimpleWeb(BOOL bSimpleWeb);
	CString GetURL();

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif
