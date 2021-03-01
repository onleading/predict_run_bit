#ifndef CUSTOMPAD_H_
#define CUSTOMPAD_H_
class UBase;

//Unit类型及其风格
#define ZST_UNIT				0
	#define ZST_BIG			0
	#define ZST_LB			1	//量比
	#define ZST_MMLD		2	//买卖力道
	#define	ZST_AUCTION		3	//集合竞价
	#define ZST_TOTALORDER	4	//总买总卖
	#define ZST_TOTALCANCEL	5	//净挂净撤
	#define ZST_CANCELSUM	6	//撤单总计
	#define ZST_IOVOL		7	//主买主卖
	#define ZST_LARGEVOL	8	//大单动向
	#define ZST_TICKNUM		9	//涨跌动因
	#define ZST_LARGETICK	10	//大单差分

	#define ZST_MULTI		50
	#define ZST_SMALL		51
	#define ZST_GPZS		52
	#define ZST_SHDP		53
	#define ZST_SZDP		54
#define FXT_UNIT				1
	#define FXT_BIG			100
	#define FXT_MULTI		101
	#define FXT_SHDP		102
	#define FXT_SZDP		103
#define HQ_UNIT					2
#define TICK_UNIT				3
	#define TICK_BIG		300
	#define TICK_SMALL		301
#define FJB_UNIT				4
	#define FJB_BIG			400
	#define FJB_SMALL		401
#define FSB_UNIT				5
	#define FSB_BIG			500
	#define FSB_SMALL		501
#define CDP_UNIT				6
#define MMP_UNIT				7
	#define MMP_BIG			700
	#define MMP_SMALL		701
#define HQGRID_UNIT				8
	#define HQGRID_ALL		800
	#define HQGRID_AG		801
	#define HQGRID_BG		802
	#define HQGRID_QZ		803
	#define HQGRID_ZQ		804
	#define HQGRID_JJ		805
	#define HQGRID_SB		806
	#define HQGRID_SPEC		807
	#define HQGRID_ZX		808
	#define HQGRID_DY		809
	#define HQGRID_HY		810
	#define HQGRID_GN		811
	#define HQGRID_ZH		812
	#define HQGRID_ZD		813
	#define HQGRID_ZJHHY	814

	#define HQGRID_SHAG		817
	#define HQGRID_SZAG		818
	#define HQGRID_ZXB		819
	#define HQGRID_CYB		820
	#define HQGRID_DYNA		821

	#define HQGRID_DS1		832
	#define HQGRID_DS64		895

#define CB_UNIT					9
#define HISK_UNIT				10
#define BIGVOL_UNIT				11
#define KLIST_UNIT				12
#define ZHSORT_UNIT				13
#define RADAR_UNIT				15
#define LOWHQ_UNIT				16
#define WEB_UNIT				17
#define TICKSTAT_UNIT			18
#define SHORTGUY_UNIT			19
#define SEAT_UNIT				20
#define	HKNEWS_UNIT				21
	#define HKNEWS_BIG			2100
	#define HKNEWS_SMALL		2101
#define TICKZST_UNIT			22
#define	F10_UNIT				23
#define	RMREPORT_UNIT			24
	#define	RMREPORT_HQ			2400
	#define	RMREPORT_ZJ			2401
#define	STAR_UNIT				25
#define MULINFO_UNIT			26
#define MORE_UNIT				27	//分时图下部组件群,可以包含很多种
#define GLHQ_UNIT				28	//关联行情报价
#define	MTSORT_UNIT				29	//多日排行

#define HQ10_UNIT				30
#define TICK2_UNIT				31
#define QUEUE_UNIT				32
#define SCANCEL_UNIT			33
#define TCANCEL_UNIT			34
#define WTRANK_UNIT				35
#define HYCJ_UNIT				36
#define TV_UNIT					37
#define	WTFB_UNIT				38

#define GPPAGE_UNIT				44

#define EXTERN1_UNIT			50	//外部Unit开始
#define EXTERN100_UNIT			149	//外部Unit结束

//ComView使用的大的版面类型MainPad
#define ZST_PAD				0x010000
#define FXT_PAD				0x020000
#define TICK_PAD			0x030000
#define FJB_PAD				0x040000
#define TICK2_PAD			0x050000
#define MMP_PAD				0x060000
#define MULZST_PAD			0x070000
#define MULFXT_PAD			0x080000

//AssisPad1,MainPad是ZST_PAD或FXT_PAD时出现
#define HQTICK_PAD			0x000100
#define HQFJB_PAD			0x000200
#define HQTICK2_PAD			0x000300
#define HQMMP_PAD			0x000400
#define HQZST_PAD			0x000500
#define HQZS_PAD			0x000600
#define HQCDP_PAD			0x000700
#define CB_PAD				0x000A00
#define BIGVOL_PAD			0x000B00
#define WEB_PAD				0x000C00
#define ZJ_PAD				0x000D00
#define YD_PAD				0x000E00

//AssisPad2,MainPad是ZST_PAD或FXT_PAD时出现
#define HQ10QUEUE_PAD		0x000001
#define HQ10TICKSTAT_PAD	0x000002
#define HQ10WTFB_PAD		0x000003
#define HQ10SCANCEL_PAD		0x000004
#define HQ10TCANCEL_PAD		0x000005
#define HQ10WTRANK_PAD		0x000006
#define HQ10HYCJ_PAD		0x000007

////////////////////////////////////////////////
//				系统定制的Pad信息
////////////////////////////////////////////////

#define GP_ZS_CPAD		0
#define BIGVOL_CPAD		1
#define TWOGP_ZST_CPAD	2
#define TWOGP_FXT_CPAD	3
#define GP_MANYMMP		4
#define ZHGRID_CPAD		5

#define DSZST_CPAD		50
#define DSFXT_CPAD		51

////////Unit的宽高信息/////////
#define LEN_FIX			0	
#define LEN_FULLPER		1
#define LEN_12PER		2
#define LEN_13PER		3
#define LEN_23PER		4
#define LEN_14PER		5
#define LEN_34PER		6
#define LEN_REST		7

//////Unit的股票信息//////////
#define GPINDEX_NONE	-1
#define GPINDEX_SHZS	2
#define GPINDEX_SZZS	3
#define GPINDEX_VAR1	4
#define GPINDEX_VAR20	23

//保存ComView中Units信息的结构
typedef struct tag_Unit
{
	short	Num;					//最多MAX_UNIT_NUM个Unit
	UBase*	pUnit[MAX_UNIT_NUM];	//各Unit的指针
	short	Type[MAX_UNIT_NUM];		//各Unit的类型
	short	Style[MAX_UNIT_NUM];	//此种Unit的风格
	CRect	Rect[MAX_UNIT_NUM];
	short	nStatus[MAX_UNIT_NUM];	//各Unit的显隐和其它状态
	short	Width_Len[MAX_UNIT_NUM];
	short	Height_Len[MAX_UNIT_NUM];
	short	X_Pos[MAX_UNIT_NUM];
	short	Y_Pos[MAX_UNIT_NUM];
	short	Var_GPIndex[MAX_UNIT_NUM];
}UNITS,*LPUNITS;

typedef struct tag_ComInfo
{
	CWnd *			pWnd;
	UINT			nNowPad;
	LPUNITS			pUnits;
	int				nWhichUnit;
	int				nUnitType;
	int				nUnitStyle;
	int				nMoreFlag;
	int				nInCom2;
	int				nInDlg;
	int				nInMultiPeriod;
	char			bPlugin;
	char			bPlugin_GPRel;
}COMINFO,*LPCOMINFO;

#endif
