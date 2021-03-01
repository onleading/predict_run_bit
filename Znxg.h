#ifndef ZNXG_H_
#define ZNXG_H_

#define ZNXG_VERSION	1	//智能选股版本

#include "stdafx.h"

class CZnxg
{
public:
	CZnxg(CWnd *pWnd = NULL);
	~CZnxg();
	BOOL	CalcAllStockStyle();
	short	GetKLineStyleGP(short Type,short *lpnGPIndex);

	short	GetOtherStyleGP(short Type,short *lpnGPIndex);
private:
	CWnd	*m_pParent;
	short	m_pStkXh[MAXGPNUM];				//形态选股
	long	m_pStkAttrib[MAXGPNUM];			//其它选股
	char	m_pGPCode[MAXGPNUM][SH_CODE_LEN+1];

	struct AnalyData* TJ_AnalyData;
	
	short	m_pABGXh[MAXGPNUM];
	short	m_nABGNum;
	
private:
	short	GetKLineStyleSet(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nNo,short * npoutStyle,short nMaxNum = 1);
	float	LineRegress(float *fpOrigin,short nDataNum,float &fDiff,float &fBase);
	void	MaxMinVal(float *fpOrigin,short nDataNum,float &fMax,float &fMin);
	short	E_OddK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nUpDown,short nSize,short nShadow,short nWeight,short nMode);
	short	E_TwinK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nKStyle,short nWeight,short nMode);
	short	isTrendK_Style(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nKStyle);
	short	GetKLineStyleInside(struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nTrendKStyle);
	short	OddK_UpDown(struct AnalyData *pAnalyData,short nNo);
	short	OddK_Size(struct AnalyData *pAnalyData,short nNo);
	short	OddK_Shadow(struct AnalyData *pAnalyData,short nNo);
	short	isTwinK_Style(struct AnalyData *pAnalyData,short nNo,short nKStyle);

	short	Is_X_Type(short Type, long Stk_Attrib);
	long	GetGP_Tj_Attrib(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum);

	BOOL	Is_BIAS_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum);
	BOOL	Is_SAR_Buy(short  Index,struct AnalyData  * pTJ_AnalyData,short ItNum);
	BOOL	Is_KDJ_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum);
	BOOL	Is_RSI_Buy(short  Index,struct AnalyData  * pTJ_AnalyData,short ItNum);
	BOOL	Is_MACD_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum);
	BOOL	Is_MTM_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum);
};

#endif