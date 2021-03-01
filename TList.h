#ifndef __TLIST_H__
#define __TLIST_H__

#include "GridCtrl.h"
#include "inc.h"

typedef map<int,sg_cell> ROWCELL;

class VList : public CGridCtrl
{
public:
	VList(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0, int nAutoLineWarp=60);
	virtual ~VList();

	BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	int InsertHead(sg_head *head, BOOL bRefresh = TRUE);

	//������������
	BOOL SetCellData(sg_cell cell, int col, int row);
	
	//��õ�Ԫ��ͱ�ͷ����,col:��������row:������
	BOOL GetCellData(sg_cell& cell, int col, int row);
	BOOL GetHeadData(sg_head& head, int pos);
	
protected:
	map<int,sg_head> m_Heads;//keyΪcol
	map<int,ROWCELL> m_Cells;//keyΪcol,ROWCELL��keyΪrow

	//���ز����col
	void mergeFixedCellsBytitle();
	void showHead();

	int m_nFixRow;
	int m_nAutoLineWarp;//�Զ����е��ַ�����
};
#endif