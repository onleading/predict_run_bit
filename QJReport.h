// QJReport.h: interface for the QJReport class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QJREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_QJREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FLOAT_VALUE		0
#define FLOAT2_VALUE	1
#define VOLUME_VALUE	2
#define AMOUNT_VALUE	3

#include "Grid.h"

class QJReport: public Grid
{
	long		m_lQJBgnDate;				//��ʼ����
	long		m_lQJEndDate;				//��������

	struct AnalyData *pZsAnalyData;
	int		ZsAnalyDataNum;

	struct QJData_Struct
	{
		double	f1;
		int		f1Type;
		double	f2;
		int		f2Type;
	}fData[MAXGPNUM][11];				//�������ݽṹ

public:
	int			CurQJType;           //����,��ͬ���������ı�־�ֶ�
									 //Ϊ0��ʾ�������֮�ǵ����ȱȽϱ�
									 //Ϊ1��ʾ�������֮�����������Ƚϱ�
									 //Ϊ2��ʾ�������֮������ȶ��Ƚϱ�
									 //Ϊ3��ʾ�������֮�𵴷��ȱȽϱ�
private:
	void	GetSomeSize(CDC *pDC);
	int		GetCellLeft(int nCol,int & nWidth);
	float   fGetSortValue(short xh);

	void	DrawTitle(CDC *pDC);
	void	DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp);

	void    OnCtrlLeft();

	float	QJMax			(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//���������ֵ�ĺ���
	long	QJMaxDate		(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//���������ֵ���������ĺ���
	float	QJMin			(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//���������ֵ�ĺ���
	long	QJMinDate		(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//���������ֵ���������ĺ���
	int		GetColIndex(int col);	//�õ�ʵ����Ŀ
public:
	QJReport();
	~QJReport();

	void Sleep();				//ʹ�������˯��
	void WakeUp();				//��������Ѿ�����ı���

	//�����г������lpnUsedStkIndex����
	void	SetDomain(int nBlockFlag,int WhichType);
	void	SetQJType(int type);	//�趨��������
	int		GetQJType(){return CurQJType;}
	void	SetDateSection(CTime date1,CTime date2);	//������������
	CTime	GetDate(BOOL bFirst) 
	{
		if(bFirst) return(LongToTime(m_lQJBgnDate));
		else return(LongToTime(m_lQJEndDate));
	}

	//��ʼ��������������
	void	InitReport();
	void	GetData();
	void	Sort();
	//����ͼ����
	void	DrawReport(CDC *pDC); 

	void	OnSize(UINT nType, int cx, int cy);
	void	OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void	OnLButtonDown(UINT nFlags, CPoint point);

	void	InsertSwitch(int nSCType);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif // !defined(AFX_QJREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)