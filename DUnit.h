#ifndef __DDRAW__DUNIT__H__
#define __DDRAW__DUNIT__H__

class DUnit
{
	//data members
protected:
	long			m_nLeft;						//������߽�,�ⲿ����
	long			m_nWidth;						//������,�ⲿ����
	long			m_nTop;							//�����ϱ߽�,�ⲿ����
	long			m_nHeight;						//����߶�,�ⲿ����
	long			m_nLeftBlank;					//��߿�ȱ����(��������)
	long			m_nLeftNo;						//��߽�ͷһ��(�볡)
	long			m_nWillNum;						//��������������

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
