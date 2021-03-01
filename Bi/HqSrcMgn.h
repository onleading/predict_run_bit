// HqSrcMgn.h: interface for the CHqSrcMgn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HQSRCMGN_H__3CA9212C_02E3_4795_AE1D_0065709389D0__INCLUDED_)
#define AFX_HQSRCMGN_H__3CA9212C_02E3_4795_AE1D_0065709389D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_HQ_COM	256
class	CHideNetWnd;

class CHqSrcMgn  
{
public:
	CHqSrcMgn();
	virtual ~CHqSrcMgn();

	BOOL	RegisterOneHqSource(LPSTR strError, UINT DataType,  char *strIPAddr="", char *strPort="", char *strMarket="");
	void	StartHqThread();
	void	KillAllHqThread();

	void	SuspendAll();
	void	ResumeAll();

private:
	BOOL	KillHqThread(int nThread, int nMaxKillSec);

private:
	int	m_nHqNetThreadNum;
	CHideNetWnd *m_ppNetThread[MAX_HQ_COM];
};

#endif // !defined(AFX_HQSRCMGN_H__3CA9212C_02E3_4795_AE1D_0065709389D0__INCLUDED_)
