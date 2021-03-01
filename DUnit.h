#ifndef __DDRAW__DUNIT__H__
#define __DDRAW__DUNIT__H__

class DUnit
{
	//data members
protected:
	long			m_nLeft;						//区域左边界,外部传入
	long			m_nWidth;						//区域宽度,外部传入
	long			m_nTop;							//区域上边界,外部传入
	long			m_nHeight;						//区域高度,外部传入
	long			m_nLeftBlank;					//左边空缺个数(多日数据)
	long			m_nLeftNo;						//左边界头一个(半场)
	long			m_nWillNum;						//区域最多允许个数

	//member functions
public:
	DUnit();
	virtual ~DUnit();
public:
	//
	void SetRectRgn(long nLeft,long nWidth,long nTop,long nHeight);
	RECT GetRect();
	void SetWillNum(long nWillNum,long nLeftBlank,long nLeftNo);
public:
	virtual BOOL SetData(void *pData,long nDataNum);

	virtual BOOL Calc();
	virtual BOOL Draw(CDC *pDC,long nType);
};

#endif
