#ifndef TDX_V_LISTCTRLEX__
#define TDX_V_LISTCTRLEX__

#include <AfxWin.h>
#include <afxcmn.h>
#include <vector>
#pragma pack(push,1)
#pragma warning(disable:4786)

#ifdef VLIST_EXPORT
	#define VLIST_PREFIX extern "C" _declspec(dllexport)
#else
	#define VLIST_PREFIX extern "C" _declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////////////
//常用结构

union VARDATA
{
	int		intdata;
	double	doubledata;
};

enum  SG_HEAD_DATA_TYPE//数据类型
{
	SG_TYPE_STRING = 0,			//字符串类型
	SG_TYPE_DOUBLE,				//浮点类型
	SG_TYPE_INT,				//整数类型
};

enum SG_HEAD_CALC_TYPE//计算类型
{
	SG_CALC_STATIC=0,			//载入静态
	SG_CALC_DYNA,				//实时变化
	SG_CALC_COM,				//综合计算
};

enum SG_ALIGN_TYPE//对齐类型
{
	SG_ALIGN_LEFT=0,			//左对齐
	SG_ALIGN_RIGHT,				//右对齐
	SG_ALIGN_CENTER				//中间对齐
};

enum SG_FORMAT_TYPE//显示格式类型
{
	SG_FORMAT_NORMAL=0,			//不作任何处理
	SG_FORMAT_COMMA_SEP,		//千分位逗号分隔

	SG_PERCENT_BANJIAO,			//半角百分号
	SG_PERCENT_QUANJIAO,		//全角百分号
	SG_ONE_THOUSAND,			//千分号

	SG_FORMAT_WAN,				//显示为XXX.XX万
	SG_FORMAT_YI,				//显示为XXX.XX亿
	SG_FORMAT_COMMA_SEP_WAN,	//XXX,XXX,XXX,XXX.XX万
    SG_FORMAT_COMMA_SEP_YI,		//XXX,XXX,XXX,XXX.XX亿
	SG_FORMAT_WAN_YI,			//自动万亿表示法
	SG_FORMAT_COMMA_SEP_WAN_YI,	//带千分位的自动万亿表示法

	SG_DATE_SLASH,				//斜杠分隔时间yyyy/mm/dd
	SG_DATE_SLASH2,				//斜杠分隔时间mm/dd/yyyy
	SG_DATE_SHORTLINE,			//短横线分隔时间yyyy-mm-dd
	SG_DATE_NORMAL,				//YYYYMMDD
	SG_DATE_SLASH_SHORT,		//YY/MM/DD
	SG_DATE_SLASH2_SHORT,		//MM/DD/YY
	SG_DATE_SHORTLINE_SHORT,	//YY-MM-DD
	SG_DATE_NORMAL_SHORT,		//YYMMDD
	
	SG_DATE_REPORT,				//年报形式
	SG_DATE_REPORT_SHORT,		//短年报格式

	SG_FORMAT_FIXED_2,			//固定宽度显示,显示2位								
	SG_FORMAT_FIXED_16 = (SG_FORMAT_FIXED_2+14)
								//固定宽度显示,显示16位,对于股票,可表示位SG_FORMAT_FIXED_2+4;
};

enum SG_FORMAT_ZERO//如何显示0
{
	SG_FORMAT_ZERO_NORMAL = 0,   //无特别要求
	SG_FORMAT_ZERO_BLANK,	     //显示为空格
	SG_FORMAT_ZERO_SHORT_LINE,	 //显示为短横线
};

enum SG_FORMAT_INVALID//如何显示invalid
{
	SG_FORMAT_INVALID_BLANK = 0, //显示为空格
	SG_FORMAT_INVALID_SHORT_LINE,//显示为短横线
};

enum SG_DATACOLOR_MODE//数据颜色
{
	SG_DATACOLOR_MODE_V=0,		//成交量颜色;
	SG_DATACOLOR_MODE_W,		//纯粹中性显示，客户端的列表文本颜色;
	SG_DATACOLOR_MODE_P,		//价格显示法，与昨日收盘价比较，分别取颜色配置集合的涨跌平颜色显示;
	SG_DATACOLOR_MODE_B,		//比例显示法，根据OutName_BJ指示字段来比较，分别取颜色配置集合的涨跌平颜色显示;
	SG_DATACOLOR_MODE_Z,		//基点显示法，与0.0比较高低，分别取颜色配置集合的涨跌平颜色显示
	SG_DATACOLOR_MODE_F			//根据行的state变量确定取m_OtherInfo.StateColor[]中的哪种颜色
};

enum SG_SUMARY//汇总格式
{
	SG_SUMARY_BLANK=0,
	SG_SUMARY_SHORT_LINE,
	SG_SUMARY_STRING,
	SG_SUMARY_SUM,
	SG_SUMARY_COUNT,
	SG_SUMARY_MIN,
	SG_SUMARY_MAX,
};

struct sg_head//表头的抽象
{
   	char				title[101];		//显示的标题
	char				name[31];		//唯一标识某一个表头
	short				width;			//表头宽度,以像素为单位,为实际显示的宽度
	int					decimaldigits;	//小数位数
    SG_ALIGN_TYPE		align;			//表体数据对齐类型
	SG_ALIGN_TYPE		headalign;		//表头数据对齐类型
	SG_HEAD_DATA_TYPE	datatype;		//数据类型
	SG_FORMAT_TYPE		formattype;		//显示格式类型
	SG_FORMAT_ZERO		formatzero;		//如何显示0
	SG_FORMAT_INVALID	formatinvalid;	//如何显示invalid
	SG_DATACOLOR_MODE	datacolormode;	//数据颜色
	char				outname_bj[31];	//比较列
	UINT				nFlag;			//标志位，便于以后扩展;
};

struct sg_cell//表格中单元格数据的抽象
{
	VARDATA				data;			//如果是整形,双浮点型;
	CString				stringdata;		//如果是字符串型
	COLORREF			txtcolor;		//文本颜色
	int					iImage;			//在第一列显示的图片,若为-1,表示不显示;否则调用ImageList
	int					iSelectedImage;	//当选中时显示的图片
};

class sg_cell_line//表格中的一行数据
{
public:
	sg_cell_line(){}
	sg_cell_line(const sg_cell_line& line){}

	std::vector<sg_cell> line_cell;
	char	state;				//外部使用用于标志此行的外部状态,如
	DWORD	data;				//外部可以调用SetItemData(...来指定的值,用来保存此行数据与外部某数据的关系
	UINT	nFlag;				//标志位，便于以后扩展关于行的信息
};

struct SuperList_ColorFont//关于颜色和字体的结构
{
	COLORREF BkColor;
	COLORREF DataBkColor;
	COLORREF DataBkColor2;
	COLORREF DataTextColor;
	COLORREF ListLineColor;
	COLORREF SelectedColor;
	COLORREF HeadTextColor;
	COLORREF HeadBkColor;
	COLORREF ScrollBarColor;
	COLORREF CodeNameColor;
	COLORREF VolumeColor;
	COLORREF UpColor;
	COLORREF DownColor;
	COLORREF StateColor[5];

	LOGFONT	 ListFont;
	LOGFONT	 ListFontOfNum;
};

struct SuperList_OtherInfo//除了颜色,字体之外的信息的设定
{
	int		HeadHeight;			//表头单位行高 
	int		LineHeight;			//数据每行高度 
	char	strSumary[20];		//缺省为合计 
	float	InvalidFloat;		//无效浮点数,缺省为7654321.00,支持尚不完整
	int		InvalidInt;			//无效整数,缺省为87654321,支持尚不完整
	UINT	SelChangeMsg;		//当选择变化时发送的消息;缺省值为0,不发送消息; 
	UINT	ColSortMsg;			//对列排序
	UINT	LButtonDblClkMsg;	//双击某行时发送的消息;缺省值为0,不发送消息; 
	UINT	ReturnKeyMsg;		//选中某行按回车键时发送的消息;缺省值为0,不发送消息;
	char	Unused[100];		//备用
}; 
 
/////////////////////////////////////////////////////////////////////////////////////
//常用宏

#define TDX_LISTCTRLEX_IMAGELIST_LARGE  0 
#define TDX_LISTCTRLEX_IMAGELIST_NORMAL 1 

//下面4个宏用于AppendMessage(...,在鼠标右键单击表头或某行时弹出的右键菜单中添加菜单项
#define TDX_LISTCTRLEX_RBTN_LISTITEM	0X00000001
#define TDX_LISTCTRLEX_RBTN_LISTHEADER	0X00000002
#define TDX_LISTCTRLEX_SEPARATOR_MASK	0XF0000000 
#define TDX_LISTCTRLEX_ITEM_MASK		0X0FFFFFFF 

//用于定义列表控件的样式
#define TLVS_AutoArrange		0X00000002	//自适应列宽 
#define TLVS_ColumnHeader		0X00000020	//显示列头 
#define TLVS_SortColumn			0X00000040	//列可排序 
#define TLVS_HeaderOnTop		0X00004000	//列头在上，横排模式 
#define TLVS_HasListGrid		0X00010000	//列表带有网格 
#define TLVS_ChangeMode			0X00020000	//可以切换横排纵排模式 
#define TLVS_SaveOut			0X00040000	//可以保存列表内容到文件，或截屏 
#define TLVS_CanFilter			0X00080000	//是否显示过滤菜单 
#define TLVS_Summary			0X00200000	//带汇总模式

class VList : public CWnd
{
public:
	//设置flag
	virtual BOOL  SetStyle(DWORD dwStyle, DWORD dwStyleMask = 0XFFFFFFFF) = 0;
	virtual DWORD GetStyle(DWORD dwStyleMask = 0XFFFFFFFF) = 0;
	virtual BOOL  ChangeStyle(DWORD dwRemove= 0, DWORD dwAdd = 0, UINT nFlag = 0) = 0;

	//设置列表标题
	virtual void  SetListTitle(const CString& ListTitle) = 0;

	//设置图标列
	virtual BOOL  SetImageList(CImageList* pil, int nFlag)= 0;
	virtual const CImageList* GetImageList(int nFlag) const= 0;

	//添加NotifyMessage消息
	virtual BOOL InsertReflectMsg(int pos, int nType, UINT nId, const CString& strMsg, HWND hWnd) = 0;
	virtual BOOL AppendReflectMsg(int nType, UINT nId, const CString& strMsg, HWND hWnd) = 0;
	virtual BOOL AppendReflectMsg(int nType) = 0;//添加分割线

	//设置颜色，字体
	virtual BOOL SetColorFont(const SuperList_ColorFont& cf) = 0;
	virtual SuperList_ColorFont& GetColorFont() = 0;

	//初始化表头
	virtual void InitHead(sg_head *head) = 0;
	//设置可见表头
	virtual void SetVisualHeads(std::vector<sg_head>& vHeads) = 0;
	//插入一个表头
	virtual int  InsertHead(sg_head *head, BOOL bRefresh = TRUE) = 0;
	//插入缺省的全部表头
	virtual int  InsertDefaultHead(sg_head *head) = 0;

	//得到共有多少表头
	virtual int GetHeadCount() = 0;
	//得到总行数
	virtual int   GetItemCount() = 0;

	//删除列,如果name为NULL,则删除所有
	virtual void DeleteColByName(const  char*name, BOOL bReDraw = TRUE) = 0;
	//删除列,如果index=-1,则删除所有
	virtual void DeleteColByIndex(int index, BOOL bReDraw = TRUE) = 0;
	
	//预分配存储空间
	virtual BOOL Reserve(int count) = 0;
	
	//插入数据(bRefresh==TRUE会计算一些额外的东西，在大量插入数据时，只在最后一次使用参数TRUE)
	virtual BOOL InsertItem(const sg_cell_line& line, int pos, BOOL bRefresh = TRUE) = 0;
	virtual BOOL AppendItem(const sg_cell_line& line, BOOL bRefresh = TRUE) = 0;
	virtual BOOL AppendItem(const std::vector<sg_cell_line>& lines, BOOL bRefresh = TRUE) = 0;

	//删除数据,如果index为-1,则删除所有数据
	virtual void  DeleteItem(int index = -1, BOOL bReDraw = TRUE) = 0;
	//设置数据
	virtual BOOL  SetItemData(int index, DWORD data) = 0;
	virtual DWORD GetItemData(int index) const = 0;
	
	//获得单元格和表头数据,col:列索引，row:行索引
	virtual BOOL  GetCellData(sg_cell& cell, int col, int row) = 0;
	virtual BOOL  GetHeadData(sg_head& head, int pos) = 0;
	
	//修改某行
	virtual BOOL  ModifyItem(const sg_cell_line& line, int pos, BOOL bRedraw = TRUE) = 0;
	virtual int   GetPosFromItemData(DWORD nData) const = 0;
	
	//选择某一行
	virtual void SetCurSel(int index) = 0;
	virtual int  GetCurSel() = 0;

	//选择某一列
	virtual void SetCurSelCol(int index) = 0;
	virtual int  GetCurSelCol() = 0;

	//本屏第一个
	virtual int  GetFirstIndex() = 0;
	//本屏最后一个
	virtual int  GetLastIndex() = 0;

	//设置表头的一些其他的信息
	//表头单位行高, 数据每行高度,是否有排序线;被选择行变动时发送的消息ID,右键按下时发送的消息ID;
	virtual void  SetOtherInfo(const SuperList_OtherInfo& otherInfo) = 0;
	virtual const SuperList_OtherInfo& GetOtherInfo() const = 0;

	//得到list的区域
	virtual void GetListRect(CRect &rect) = 0;

	//关于固定行列,固定列不会随着水平滚动条移动
	virtual int SetFixedColumn(int nFixedCol) = 0;
	//排序列,sortmode:-1,降序;1,升序
	virtual void SortColumn(const char *name,int sortmode) = 0;

	DECLARE_MESSAGE_MAP()	
};
	
//创建对象和窗口
VLIST_PREFIX VList* CreateNewTList(DWORD dwStyle, CWnd* pParent, CRect rect, UINT nId, CWnd* pWndNotify = NULL);
//使用这个函数来释放此列表
VLIST_PREFIX void DeleteTList(VList** ppList);

#pragma pack(pop)
#endif