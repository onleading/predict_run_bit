#ifndef U_BASEINNER__H_
#define U_BASEINNER__H_

#include "UBase.h"

class UBaseCtrl;
class CVesselWnd;
class UBaseInner : public UBase  
{
public:
	UBaseInner(LPCOMINFO pComInfo,char *ModuleCode);
	virtual ~UBaseInner();

	void		ShowUnit();
	void		DeleteUnit();
	void		Resize();	
	int			ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	CVesselWnd *m_pVessel;
private:
	char		m_ModuleCode[41];
	UBaseCtrl *	m_pUBaseCtrl;
	IGNPLUGIN *	m_pModule;
	BOOL		m_bCreateSuccess;
	BOOL		m_bSmallSize;
};

#endif
