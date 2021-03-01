#ifndef UGPWEB_H_
#define UGPWEB_H_

#include "Ubase.h"
#include "UWeb.h"

class UGPWeb : public UWeb
{
public:
	UGPWeb(LPCOMINFO pComInfo);
	virtual ~UGPWeb();
	void SetMode(int nMode,int nViewthemType);
	void SetRoll(BOOL bRoll,short setcode, char* code);

	BOOL NavigateIt();
	BOOL GPChangeNavigate();
	void SendDataReq();
	void AfterdDataACK();
private:
	int		m_nMode;
	int		m_nViewthemType;

	BOOL	m_bRoll;
	short	m_Spec_Setcode;
	char	m_Spec_Code[SH_CODE_LEN+1];
	long	m_LastRollSeconds;
};


#endif
