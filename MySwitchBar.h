/*
*	分上,下,左,右,四种风格
*	水平风格时只有一级按钮,按钮互相重叠,垂直风格时有两级按钮,次级按钮以缩进表示
*	水平风格默认左边按钮遮住右边按钮
*	WM_USER+1001 到 WM_USER+1003 段的消息id被使用
*	0xfffe,0xffff被回翻和后翻按钮占用,标签不能使用这两个ID
*	当标签被切换时,父窗口获得UM_MYSWITCHBAR_CLICK消息,LPARAM参数是MySwitchBarMsg结构的指针
*
*
*	插入/删除操作:
*	插入/删除item于m_vData
*	重新计算page,若需要则改变m_nCurPage
*	计算显示的标签的Rgn
*	显示/隐藏翻页按钮,控制翻页按钮的状态(enable)
*	根据m_nCurPage移动所有标签,控制显示/隐藏
*
*	批量插入/删除操作:
*	外部调用SetMyRedraw(FALSE)
*	插入/删除item于m_vData
*	外部调用SetMyRedraw(TRUE)
*	在SetMyRedraw(TRUE)中进行
*	重新计算page,若需要则改变m_nCurPage
*	计算显示的标签的Rgn
*	显示/隐藏翻页按钮,控制翻页按钮的状态(enable)
*	根据m_nCurPage移动所有标签,控制显示/隐藏
* 
*	翻页操作:
*	改变m_nCurPage
*	显示/隐藏翻页按钮,控制翻页按钮的状态(enable)
*	计算显示的标签的Rgn
*	根据m_nCurPage移动所有标签,控制显示/隐藏
*	
*	切换操作:
*	改变m_nIndex_sel
*	1.鼠标点击:
*	计算显示的标签的Rgn
*	往父窗口发消息
*	2.外部调用SetSel:
*	若原始选中和当前设置选中都不在显示标签部分
*	nothing
*	否则
*	计算显示的标签的Rgn
*	
*	尺寸改变:
*	重新计算并移动翻页按钮
*	重新计算page,若需要则改变m_nCurPage
*	显示/隐藏翻页按钮,控制翻页按钮的状态(enable)
*	根据m_nCurPage移动所有标签,控制显示/隐藏
*	计算显示的标签的Rgn
*			
*	清空操作:
*	清空m_vData
*	清空m_vPage
*	重置m_nIndex_sel=-1
*	重置m_nCurPage=-1
*	隐藏翻页按钮
*	
*	刷新背景
*	计算当前页中标签占用的rgn以及翻页按钮的rgn,并从刷新区域中去掉
*
*/

#ifndef _TDX_MYSWITCHBAR_
#define _TDX_MYSWITCHBAR_

#include "MyBitmapBt.h"
#include <vector>

const UINT BT_PAGE_UP	= 0xfffe;
const UINT BT_PAGE_DOWN	= 0xffff;

const UINT UM_MYSWITCHBAR_CLICK = WM_USER+1003;	//当标签切换时发送此消息

typedef MBB_OwnerDraw MSB_OwnerDraw;			//每个标签的绘制修改函数

typedef struct tagMySwitchBarMsg	//当标签被点击切换时,消息的第二参数将是此结构的指针
{
	UINT id;				//SwitchBar的ID
	UINT last_sel;			//前一个被选中的标签的索引号
	UINT last_id;			//前一个被选中的标签的ID
	UINT cur_sel;			//当前被选中的标签的索引号
	UINT cur_id;			//当前被选中的标签的ID
}MySwitchBarMsg,*pMySwitchBarMsg;

enum MSB_BorderState_ToRight		//本按钮和它右边按钮的重叠状态
{	
	MSBBS_OVER=0,			//在右边按钮之上,遮住右边按钮
	MSBBS_UNDER				//在右边按钮之下,被右边按钮遮住
};

typedef struct tagMySwitchBarData
{
	MyBitmapBt *pBt;
	CRect		rect;
	UINT		nID;		//按钮ID

	LONG		level;		//0.一级标签 1.二级标签
	CRgn		rgn;		//按钮窗口边界

	MSB_BorderState_ToRight borderState;	
							//本按钮和它右边按钮的重叠状态,水平风格时有效

	void*		pItemData;	//扩展信息
	
	tagMySwitchBarData()
	{
		level = 0;
		nID	= 0;
		pBt = NULL;
		borderState = MSBBS_OVER;
		pItemData = NULL;
		rect.SetRectEmpty();
	}
	~tagMySwitchBarData()
	{
		if(pBt && pBt->m_hWnd && ::IsWindow(pBt->m_hWnd))
			pBt->DestroyWindow();
		TDEL(pBt);
	}
}MySwitchBarData,*pMySwitchBarData;

typedef std::vector<pMySwitchBarData> vMySwitchBarData;

typedef struct tagMySwitchBarPage	//一个可见页的开始和结束标签的索引号
{
	UINT	nShowFirst;
	UINT	nShowLast;
	int		nPageSize;	//水平风格时表示宽度,垂直风格时表示高度
}MySwitchBarPage;

typedef std::vector<MySwitchBarPage> vMySwitchBarPage;

enum MySwitchBarStyle	//风格
{
	MSBS_H_T=0,
	MSBS_H_B,
	MSBS_V_L,
	MSBS_V_R
};

enum MSB_EraseBkgndStyle	//重绘背景的风格
{
	MSB_EBS_BITMAP=0,		//用位图填充
	MSB_EBS_GRADIENT,		//用渐进色
	MSB_EBS_OWNERDRAW,		//通过派生子类自绘
};

/////////////////////////////////////////////////////////////////////////////
// CMySwitchBar window

class CMySwitchBar : public CWnd
{
// Construction
public:
	CMySwitchBar();
	CMySwitchBar(UINT id,MySwitchBarStyle style=MSBS_H_T,int marge_left=0,int marge_top=0);
// Attributes
protected:
	vMySwitchBarData m_vData;
	MySwitchBarStyle m_style;		//风格

	UINT			m_nID;
	CBitmap *		m_BK[12];		//0-8为标签三状态的左右中,9-11为主背景的左右中

	MyBitmapBt		m_pageBt[2];	//回翻按钮和后翻按钮
	MyBitmapBt *	ptest;
	CRect			m_bt_rect[2];
	CRect			m_btsRect;		//两按钮一起占的区域
	vMySwitchBarPage m_vPage;		//分页信息,每个成员为一页,没有任何标签时为空
	int				m_nCurPage;		//当前显示的页号,-1表示当前没有任何可显示页

	MSB_OwnerDraw	m_pfOwnerDraw;	//若此函数指针不为空,则在绘制每个按钮背景后调用此函数自绘
	MSB_EraseBkgndStyle m_EraseBkgndStyle;	
									//绘制背景的风格
	//当用渐进色绘制时的参数
	COLORREF		m_startColor,m_endColor,m_cor_bk;
	BOOL			m_bHorizontal;

	//文本参数
	COLORREF		m_cor_text_n,m_cor_text_d,m_cor_text_o;
	TextStyle		m_text_style_n,m_text_style_d,m_text_style_o;
	int				m_text_height_n,m_text_height_d,m_text_height_o;
	CPoint			m_offset[3];	//文字在不同状态时显示的偏移量

	int				m_item_offset;	//相邻两按钮重叠的部分,垂直风格时默认为1
	int				m_level_offset;	//垂直风格时,次级标签的缩进
	int				m_marge_left,m_marge_top;	
									//边缘留白大小
	int				m_nRedraw;		// <0表示不刷新,>=0表示刷新
	BOOL			m_bBatchCalc;	//是否正在进行批量运算
	int				m_nIndex_sel;	//当前选中状态的按钮索引
	std::vector<UINT>	m_vtButtons;

// Operations
public:
	//使用无参数构造函数时,需要调用此函数初始化
	void Init(UINT id,MySwitchBarStyle style=MSBS_H_T,int marge_left=0,int marge_top=0);
	//设置文字在不同状态下的颜色,指定nIndex时设置一个标签,否则设置所有标签
	void SetBkColor(COLORREF cor_bk,COLORREF startColor,COLORREF endColor);
	void SetTextColor(COLORREF cor_text_n,COLORREF cor_text_d=0,COLORREF cor_text_o=0,int nIndex=-1);
	//设置文字在不同状态下的风格(粗体,斜体等),指定nIndex时设置一个标签,否则设置所有标签
	void SetTextStyle(TextStyle style_n,TextStyle style_d,TextStyle style_o,int nIndex=-1);	
	//设置文字在不同状态下的大小,指定nIndex时设置一个标签,否则设置所有标签
	void SetTextHeight(int height_n,int height_d=-1,int height_o=-1,int nIndex=-1);
	//设置不同状态时文字的偏移,指定nIndex时设置一个标签,否则设置所有标签
	void SetTextOffset(int x_n,int y_n,int x_d,int y_d,int x_o=0,int y_o=0,int nIndex=-1);	

	//设置自绘函数
	void SetCustomFunc(MSB_OwnerDraw pFunc);

	//非选中状态的普通,选中状态的普通,鼠标滑过,3种状态,每种状态由左右中3张位图拼接出按钮背景,最后3个是整体背景的左右中
	void SetBK(long bk[12]);
	void SetBK(LPCTSTR bk[12]);
	void SetBK(CBitmap* bk[12]);
	//设置翻页按钮背景,0-2:回翻按钮3状态,3-5:后翻按钮3状态
	void SetButtonBK(long bk[6]);
	void SetButtonBK(LPCTSTR bk[6]);
	void SetButtonBK(CBitmap* bk[6]);

	//设置标签重叠量
	void SetItemOffset(int offset);
	//设置垂直风格时次级标签的缩进量
	void SetLevelOffset(int offset);

	//添加标签,在nIndex前插入标签,若nIndex==-1,则在末尾添加
	BOOL InsertSwitch(int nIndex,UINT id,LPCSTR text,LPCSTR tooltip="",LONG level=0);
	//删除标签,若nIndex为-1则按id查找,否则按照nIndex查找
	BOOL DeleteSwitch(int nIndex,UINT id=0);
	//清空所有标签
	void Clear();

	//设置选中状态,nIndex==-1时按id查找,否则按nIndex
	BOOL SetSel(int nIndex,UINT id=0);
	//返回选中状态
	void GetSel(int &nIndex,UINT &id);
	//设置当前显示页号
	BOOL SetPage(UINT nPage);
	//返回当前显示的页号
	void GetPage(int &nCurPage,UINT &nTotalPage);
	//附加信息
	BOOL  SetItemData(int nIndex,UINT id,void *pItemData);
	void* GetItemData(int nIndex,UINT id=0);
	
	//设置是否重新计算不规则形状标签边界,在批量插入按钮时使用,提高效率
	void SetMyRedraw(BOOL bRedraw);

	//指定主背景按照指定颜色用渐进色绘制
	void SetEraseBackGroundParam(COLORREF StartColor,COLORREF EndColor,BOOL bHorizontal);
	//参数为TRUE时通过派生子类来实现自绘,为FALSE时用位图填充(默认方式,可以不用调用此函数)
	void SetEraseBackGroundParam(BOOL bOwnerDraw);
	//用来改变Label被单击时的反应变为按钮风格
	void ChangeLabelBehavior(UINT nIndex);
	//判断一个Label的点击反应是否按钮风格
	BOOL IsBehaviorAsButton(UINT nIndex);
	
protected:
	MyBitmapBt* _CreateNewBt(UINT id,LPCSTR text,LPCSTR tooltip = "");
	pMySwitchBarData _CreateNewItem(UINT id,LPCSTR text,LPCSTR tooltip = "",LONG level = 0);
	BOOL _InsertSwitch(pMySwitchBarData newItem,int nIndex=-1);
	
	//计算标签边界
	BOOL _ModifyRgn(CRgn *rgn_l,int offset_l,MSB_BorderState_ToRight bs0,CRgn *rgn_m,MSB_BorderState_ToRight bs1,CRgn *rgn_r,int offset_r);
	BOOL _CalcItemRgn(UINT nIndex,UINT nStart,UINT nEnd,CRgn &rgn);
	void CalcRgn();	//计算当前显示页的各标签边界

	//计算页
	void _CalcPage(UINT nStart,UINT &nEnd,int &pageSize);	//给定开始索引,返回一页能显示的最后一个索引和此页的尺寸
	void CalcPage();
	//根据m_nCurPage移动所有标签,控制显示/隐藏
	void MoveAndShow();
	//显示/隐藏翻页按钮,改变翻页按钮状态
	void ShowPageButtons();

	//通过按钮ID,返回按钮的索引号,返回-1表示没有找到
	int _GetIndexFromID(UINT id);
	//通过按钮索引号,返回按钮ID,返回-1表示没有找到
	int _GetIDFromIndex(UINT nIndex);

	//控制是否刷新
	void _SetMyRedraw(BOOL bRedraw);
	
	//刷新背景时最后调用此函数,若有自绘需求,则派生一个子类,重载此虚函数
	virtual void MyEraseBkgnd(CDC *pDC);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySwitchBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMySwitchBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMySwitchBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnClick(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
