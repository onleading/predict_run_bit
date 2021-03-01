/*
* Copyright (c) 2002,ͨ���ſ�����
* All rights reserved.
* 
* �ļ����ƣ�UCb.h
* �ļ���ʶ��
* ժ    Ҫ������ƶ����롢����ɽ����.
*/

#ifndef UCB_H_
#define UCB_H_

#ifdef _DEBUG
#pragma warning(disable:4786) 
#endif

#include "Ubase.h"


const	int	NPRICE	= 1000;
const	int	CACHEA	= 100;
const	int	MAX_MON = 6;
const	int	CHNUM	= 50;
struct	POINTCOST					//�ֱ���ͬ���ڵĳɱ�ͼ
{	
	int					N;			//������
	double			*	pTabVol;	//���ղ�ͬ�ݶȷ��䣬��Ŷ�Ӧ��λ�ĳɽ�����
	double			*	pDrawVol;	//��������
	double				dCost;		//ռ�ĳɱ���
	POINTCOST()
	{
		N = 0;
		pTabVol = new double[NPRICE];
		pDrawVol= new double[NPRICE];
		memset(pTabVol,0,NPRICE*sizeof(double));
		memset(pDrawVol,0,NPRICE*sizeof(double));
	}
	void	Reset()
	{
		memset(pDrawVol,0,NPRICE*sizeof(double));
		memset(pTabVol,0,NPRICE*sizeof(double));
		dCost = 0;
	}
	void	ResetDraw()
	{
		memset(pTabVol,0,NPRICE*sizeof(double));
		memset(pDrawVol,0,NPRICE*sizeof(double));
	}
	~POINTCOST()
	{
		if ( pTabVol )
			delete [] pTabVol;
		if ( pDrawVol )
			delete [] pDrawVol;
	}
};
enum	COSTWAY				//�������ȳɱ��ķ���
{
	AVERAGE,				//ƽ��
	TRIANGLE,				//��
	NORMAL,					//��̬
};
struct	ENVIORSETUP			//��������
{
	float		fWeakK;			//��ʷ����˥��ϵ����//˥��ϵ�� 0.1 ~ 10
	COSTWAY		Way;			//���ճɱ��㷨
	BOOL		IsPrecious;		//�Ƿ�ȷ����
	float		fdHSL;			//ȱʡ����
	float		fJZD;			//���ж�
	short		nLine;			//������
	short		nSkip[2];			//���ٸ��������ͼ
	short		aN[2*MAX_MON];	//�й̶�����
	DWORD		reserve[3];		//
};
struct	DISPINFO
{
	long		Date;			//��ʾ����
	double		dGotRate;		//��������:�����м�Ϊ�ο���
	float		fPrice;			//ÿ������۸�
	double		dGot;			//ÿ������۸�Ļ�����
	float		dPJPrice;		//50%���봦�ļ۸�
	float		p90[2];			//90%����������۸�
	double		j90;			//90%����ļ��ж�
	float		p70[2];
	double		j70;
};
struct	COSTFILE
{
	long		num;		//���ٸ�double
	DWORD		offset;		//�ļ���ʼ��
	DWORD		end;		//�ļ�
};

class UCb : public UBase
{
protected:
	int						m_nAmple;			//�Ŵ�������ɵ�(��Ϊ����̫С)
	CRect					m_aRect[4];			//�ĸ����Ե��������
	double					m_dMaxVol;			//���ɽ���
	int						m_FxBottom,m_FxTop;
	float				*	m_fLTGB;
	struct AnalyData	*	m_pData;			//����Դ
	short					m_nDataNum;			//
	short					m_anCache[CHNUM];	//���浽�ڼ���
	double	*				m_aVolCache[CHNUM];	//Ϊ�˼��ټ�����������
	double	*				m_aVolNow;			//ֱ��ʹ�õ�
	double	*				m_aDrawVol;			//��ͼʱ��ȡ�Ĳ��ֳ���ֲ���	
	POINTCOST				m_aInCost[MAX_MON];	//�����ڵĳɱ�
	POINTCOST				m_aOutCost[MAX_MON];//����ǰ�ĳɱ�
	POINTCOST			*	m_pIn[MAX_MON];		//����õ�����
	POINTCOST			*	m_pOut[MAX_MON];
	ENVIORSETUP				m_Setup;		
	long					m_nMinPrice;	//�����ƶ�����ֲ���ͼ�
	long					m_nMaxPrice;    //�����ƶ�����ֲ���߼�
	double					m_dMaxPrice;	//K��ͼ�е���ֵ
	double					m_dMinPrice;	//K��ͼ�е���ֵ
	long					m_nPGap;		//�۸���Ծ
	long					m_nDotSpan;		//���������
	DISPINFO				m_DispInfo;		//��ʾ��Ϣ
	BOOL					m_bValid;		//�����ָ�����ѽ��й��ı�Ϊ��Ч
	int						m_nCurr;		//��ǰʱ���(������ݱ仯�����̷ϳ�)
	POINT					m_nGivePt;		//��λY���á�
	short					m_nSwitch;		// 0 -��ͨ 1����ǰ  2������
	double					m_dRatio;		////y =Bottom - [(Bottom - YC)/(Close-dMin)]*(P-dMin);
	double					m_dPRatio;		////P = i * (dMax-dMin)/nNewSize + dMin;
	float					m_fCBPrice;		//50%�ɱ�����
public:
	UCb(LPCOMINFO pComInfo);
	virtual	~UCb();
	void	ReadData();
	void	DrawEx(CDC *pDC);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
private:
	//���ߺ���
	inline	void	DispInfo(CDC * pDC);
	inline	void	DrawIt(CDC *pDC);
	void	DeleteIt();
	void	Reset();
	void	GetGotLostRate();
	void	BeforeCalc();
	//���Cache���ã����¼���
	void	MainCalc();
	//���ÿ������
	void	CalcEach();
	//ƽ���ɽ����ķ���
	inline	void	GetPerVol(double * pVol,double	dVol,long P1,long P2);
	inline	void	SubPerVol(double * pVol,double dVol,long H,long L);
	inline	int		ReadCache();
	inline	void	WriteCache(int	nPosNo);
	inline	double	GetHLP(float fPrice);
	void	LoadSetup();
	void	SaveSetup();
	double	GetHSL(int no);
	inline	void	SortIt();
};


#endif