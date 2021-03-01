#if !defined(AFX_RMREPORT_H__E013E624_7352_11D4_BB99_00E04C66B1EB__INCLUDED_)
#define AFX_RMREPORT_H__E013E624_7352_11D4_BB99_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RMReport.h : header file

#include "Grid.h"

#define TOTAL_BLOCKNUM TOTAL_HY_NUM+TOTAL_DY_NUM+200

typedef struct
{
	char	strBlockName[50];
	short	nBlockNo;
}RMBLOCKINFO;

class RMReport : public Grid
{
	BOOL		m_bJSZJFlag;
	long		m_lQJBgnDate;				//��ʼ����
	long		m_lQJEndDate;				//��������

	double		fData[TOTAL_BLOCKNUM][100];
	RMBLOCKINFO m_BlockInf[TOTAL_BLOCKNUM];
	short		nBlockNo[TOTAL_HY_NUM+TOTAL_DY_NUM+200];
	struct		AnalyData *pZsAnalyData;
	int			ZsAnalyDataNum;

	float		*m_lpnWidth;
	int			*m_pColToXh;
private:
	void	GetSomeSize(CDC *pDC);
	int		GetCellLeft(int nCol,int & nWidth);
	void	GetStockQJInfo(MemStkInfo *pInfo, DWORD &dwSumVolume, float &fSumAmount, float &fSumZf, char CacheFlag=0);
	float   fGetSortValue(short xh);
	void	DrawTitle(CDC *pDC);
	void	DrawItem(CDC *pDC,int row,int col);
	void    OnCtrlLeft();
public:
	RMReport(LPCOMINFO pComInfo);
	~RMReport();
	int		CurBlockFlag;
	BOOL	m_bTodayRM;
	void	Sleep();			//ʹ�������˯��
	void	WakeUp();			//��������Ѿ�����ı���

	void	InitReport(int flag);//��ʼ��������������
	void	Start_Calc();

	void	GetData();
	void	Sort();

	void	OpenBlockHQ();
	void	SetDateSection(CTime date1,CTime date2);
	CTime	GetDate(BOOL bFirst) 
	{
		if(bFirst) return(LongToTime(m_lQJBgnDate));
		else return(LongToTime(m_lQJEndDate));
	}
	void	Com2Switch1();
	
	void	DrawReport(CDC *pDC); //����ͼ����

	void	OnSize(UINT nType, int cx, int cy);
	void	OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void	OnLButtonDown(UINT nFlags, CPoint point);

	void	InsertSwitch(int nSCType);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	void	SetCurLabel(int n);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RMREPORT_H__E013E624_7352_11D4_BB99_00E04C66B1EB__INCLUDED_)