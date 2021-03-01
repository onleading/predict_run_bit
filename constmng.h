#ifndef __CONSTMNG__H__
#define __CONSTMNG__H__

struct ZJHHY_Cfg
{
	long HYXH;
	long HYID;
	char HYCode[10];
	char HYName[50];
};

class ConstMng
{
public:	
	ConstMng();
	virtual ~ConstMng();

	void		LoadInConst();
	ZJHHY_Cfg*	GetHyFromHYID(float J_zjhhy);
	ZJHHY_Cfg*	GetHyFromHYXH(int nIDXH);
	long		GetHyXHFromHYID(float J_zjhhy);
	BOOL		BeInZJHHYBlock(CString blockname);
	char	*	GetHYFromCommandIDXH(long nIDXH);
	int			GetZJHHYBlockNum() { return m_vZJHHY.size(); }
	int			GetStockNumInHY(long nHYID);
	
private:
	vector<ZJHHY_Cfg> m_vZJHHY;
	map<long, ZJHHY_Cfg, greater<long> > m_mZJHHY;
	
};

#endif