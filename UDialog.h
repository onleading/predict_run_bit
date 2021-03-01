#ifndef UDialog_H_
#define UDialog_H_

#include "Ubase.h"
#include "vesselwnd.h"
#include "BtnST.h"
#include "GradientProgressCtrl.h"

class UDialog : public UBase
{
	int			m_nUnitType;
	int			m_nSubModule;			//功能模块
	DWORD		m_GlobalUnitID;
	BOOL		m_bShowed; //是否开始显示部件
	BOOL		m_bCreateSuccess;
	BOOL		m_bSmallSize;

	CString		m_strGnIni, m_strAppName;
	//
	IMODULE	*	m_pModuleInfo;
	//
	char		m_PadCode[16];
public:
	UDialog(LPCOMINFO pComInfo,char *PadCode);
	~UDialog();

	void	SetUnitPath(char *lpAppName, char *lpFileName)
	{
		m_strAppName = lpAppName;
		m_strGnIni = lpFileName;
	}

	void	ShowUnit(char *lpAppName, char *lpFileName);

	void	DeleteUnit();
	void	Resize();
	
	CVesselWnd	*m_pVessel;

	CString	m_strVideoFile,m_strURL;
	void	HideSelf();
	DWORD   GetGlobalID();
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif