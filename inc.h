#ifndef __INC_H__
#define __INC_H__

#include <AfxWin.h>
#include <afxcmn.h>
#include <vector>


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
	char				name[101];		//唯一标识某一个表头
	short				width;			//表头宽度,以像素为单位,为实际显示的宽度
	int					decimaldigits;	//小数位数
    SG_ALIGN_TYPE		align;			//表体数据对齐类型
	SG_ALIGN_TYPE		headalign;		//表头数据对齐类型
	SG_HEAD_DATA_TYPE	datatype;		//数据类型
	SG_FORMAT_TYPE		formattype;		//显示格式类型
	SG_FORMAT_ZERO		formatzero;		//如何显示0
	SG_FORMAT_INVALID	formatinvalid;	//如何显示invalid
	SG_DATACOLOR_MODE	datacolormode;	//数据颜色
	char				outname_bj[101];	//比较列
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
	LOGFONT	 HeaderFont;
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

#ifdef   _DEBUG
  #pragma   comment(lib,"TDXGridD.lib")
  #pragma   message("链接到调试版 ------> TDXGridD.lib")
#else
  #pragma   comment(lib,"TDXGrid.lib")
  #pragma   message("链接到发行版 ------> TDXGrid.lib")
#endif

#endif//__INC_H__