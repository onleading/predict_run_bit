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

	//设置数据内容
	BOOL SetCellData(sg_cell cell, int col, int row);
	
	//获得单元格和表头数据,col:列索引，row:行索引
	BOOL GetCellData(sg_cell& cell, int col, int row);
	BOOL GetHeadData(sg_head& head, int pos);
	
protected:
	map<int,sg_head> m_Heads;//key为col
	map<int,ROWCELL> m_Cells;//key为col,ROWCELL的key为row

	//返回插入的col
	void mergeFixedCellsBytitle();
	void showHead();

	int m_nFixRow;
	int m_nAutoLineWarp;//自动换行的字符长度
};
#endif