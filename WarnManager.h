#ifndef   __FUNC_WARN_H__
#define   __FUNC_WARN_H__

#include "Struct.h"

class CMainFrame;
class CWarnGPDlg;

class CWarnManager
{
public:
	CWarnManager();
	virtual ~CWarnManager();

	BOOL InitWarnManager(CMainFrame *pMainFrame);

	void DeleteDlg(CDialog *tempDialog);
	void OnAllWarngp();
	void OnTjWarngp();
	void UpdateWarnGP();
	void UpdateTJGP(); 

	void StartAllWarn(struct MemStkInfo *info_ptr);
	void StartTJWatch(struct MemStkInfo *info_ptr);

	void GetWarnGpSet();
private:
	void CalcPriceWarn(MemStkInfo *pMemStockInf, CurrStockData &CurrStockInf, int nConditionNo);
	BOOL NeedCalcNowKa(short nTjXh, long nIDIndex, CurrStockData *pNewHq);
	void CalcTjWarn(MemStkInfo *pInfo, int nConditionNo);
	void ProcessPopWarn(MemStkInfo *pMemStockInf, int nReasonID, int nConditionNo);
	void GetStopCloseReason(int &nRet,struct MemStkInfo* pInfo,struct CurrStockData* pHq);
	int	 GetReason(short nStockNo,WARN_STRU &WarnStru);
	BOOL ProcTimeRangeWarn(WARN_STRU &WarnStru);
	int	 RefreshAllBuf(short nStockNo);
	int  InAbsTJBuffer(short nStockNo);
public:
	static char *	GetStrFromReason(short nReason);
	static int		GetColorFromReason(short nReason,short InOutFlag);
private:
	WarnHQCache *	m_pHqCache;

	CMainFrame	*	m_pMainFrame;
	CWarnGPDlg	*	m_pWarnGPDlg;
	CWarnGPDlg	*	m_pTJGPDlg;
};

#endif
