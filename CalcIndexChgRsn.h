#ifndef __CALCINDEXCHANGEREASON__H__
#define __CALCINDEXCHANGEREASON__H__

//上证综合指数:上交所全部上市股票,包括A股和B股,以1990年12月19日为基日,以该日的市价总值为基期(权重为总股本)
//深证综合指数:深交所全部上市股票,权重为总股本
//深证成分指数:深交所40支样本股票,权重为流通股本
//沪深300指数:深沪300支样本股票,权重为总股本
//贡献度=INDEXC*(N-C)*V/ΣCV,其中C为昨日收盘价格,V为总股本或者流通股本,N,C,V为同一除权模式下的数值,N为现价,C为参考昨收,V来源于Base.dbf

typedef struct StkWeight
{
	short	SetCode;										//市场代码
	char	StkCode[14];									//证券代码
	float	fClose;											//收盘价格
	float	fNow;
	float	fCapital;										//股本(流通或者总,根据指数算法设置)
	double	dWeight;										//权重
	float	fResult;										//贡献度
}STKWEIGHT,*LPSTKWEIGHT;

class TCalcIndexChgReason
{
public:
	TCalcIndexChgReason();
	~TCalcIndexChgReason();
	BOOL SetInitData(char *szIndexName,float fIndexC,int nMaxGpNum = 6000);	//设置指数简称,指数昨收,权重模式
	BOOL AddOne(short SetCode,char *StkCode,float fNow,float fClose,float fCapital);	//设置成分股票,股票股本,股票昨收
	BOOL ReCalcWeight();													//计算权重
	//设置股票现价,计算贡献度
	BOOL UpdateResult(short SetCode,char *StkCode,float fNow);
	//取得股票贡献度
	BOOL GetOneResult(short SetCode,char *StkCode,float &fResult);
	int GetAllResult(LPSTKWEIGHT &pStkWeight);
private:
	BOOL		m_bInited;					//已经初始化指数信息
	BOOL		m_bWeightCaled;				//已经计算好个股权重
	//
	char		m_IndexName[26];			//指数名称
	float		m_IndexClose;				//指数昨收
	//
	StkWeight	*m_pStkWeight;				//个股信息
	int			m_nStkWeight;				//个股个数
	int			m_nMaxStkNum;				//最大股票个数
	int			m_nStkSpace;				//个股信息空间大小
};

#endif
