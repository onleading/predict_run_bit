#ifndef __TDX__TOPVIEW__GRAPH__H__
#define __TDX__TOPVIEW__GRAPH__H__

#include "Ubase.h"

class UTVGraph : public UBase
{
	CRect		m_TVIconRect;

	BOOL		m_bHasJGData;
	STKRELATION m_LinkData[100];
	int			m_nLinkGPNum;
public:
	UTVGraph(LPCOMINFO pComInfo);
	~UTVGraph();

	void	ReadData();
	void	DrawIt(CDC *pDC);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
private:
	void	DrawFrame(CDC *pDC);
};

#endif