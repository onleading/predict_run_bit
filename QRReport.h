// QRReport.h: interface for the QRReport class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QRREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_QRREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Grid.h"

class QRReport : public Grid
{
	long	m_lNowDate;			//����з������ݵ�����

	float	fData[MAXGPNUM][12];	//7��������Ŀ+5����ʷ����
	float	fBaseData[12];		//��׼������

	int		BaseGPNo;			//��׼��Ʊ����ţ�Ϊ-1��ʾû�л�׼
	int		OldBaseGPNo;		//��һ�λ�׼��Ʊ����ţ�Ϊ-1��ʾû�л�׼
								//OldBaseGPNo��Ҫ���ڻָ�fData����
private:
	void	GetSomeSize(CDC *pDC);
	int		GetCellLeft(int nCol,int & nWidth);
	float   fGetSortValue(short xh);
	void	DrawTitle(CDC *pDC);
	void	DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp);
	void    OnCtrlLeft();
public:
	QRReport();

	void	Sleep();			//ʹ�������˯��
	void	WakeUp();			//��������Ѿ�����ı���

	//�����г������lpnUsedStkIndex����
	void	SetDomain(int nBlockFlag,int WhichType);

	void	SetBase(int GPNo);
	void	SetItBase();

	void	InitReport();		//��ʼ��������������
	void	BaseCalc();
	void	GetSingleGPData(int GPNo,float (*Data)[12]);
	void	GetData();
	void	Sort();
	
	void	DrawReport(CDC *pDC); //����ͼ����

	void	OnSize(UINT nType, int cx, int cy);
	void	OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void	OnLButtonDown(UINT nFlags, CPoint point);

	void	InsertSwitch(int nSCType);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif // !defined(AFX_QRREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
