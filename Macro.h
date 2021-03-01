#ifndef  _MACRO_H
#define  _MACRO_H

#define TMALLOC(ptr,size,type)  { ptr = new type[size/sizeof(type)];\
									memset(ptr,0,size);}
#define TDEL(a) {if (a) {delete a;a=NULL;}}
#define TDELA(a) {if (a) {delete[] a;a=NULL;}}
#define	IMemcpy(pDest,pSrc,DataSize) if((DataSize)>0) memcpy(pDest,pSrc,DataSize)

#define chDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))

#define TEA_KEY			"tdx_hq_fy"
#define TEA_KEY_LEN		10

#define TOTAL_TYPENUM	18

#define TYPE_SHAG		0
#define TYPE_SHBG		1
#define TYPE_SZAG		2
#define TYPE_SZBG		3
#define TYPE_SHZQ		4
#define TYPE_SZZQ		5
#define TYPE_AG			6
#define TYPE_BG			7
#define TYPE_ZQ			8
#define TYPE_JJ			9
#define TYPE_ALLGOODS	10
#define TYPE_ALLZS		11
#define TYPE_ZXB		12
#define TYPE_QZ			13
#define TYPE_CYB		14

#define TYPE_ZXG		15
#define TYPE_TJG		16
#define TYPE_SB			17
#define TYPE_SPECINDEX	18

#define	TYPE_AJH		19
#define	TYPE_LC			20
#define TYPE_HKR		21

#define TYPE_JC_POOL	22
#define TYPE_ZJ_POOL	23
#define TYPE_PZ_POOL	24
#define TYPE_TJ_POOL	25
#define TYPE_EXP_POOL	26

#define	TYPE_DEFTJ_POOL	30

#define DS_START		50
#define FY_START		100


#define USERBLOCK_NUM	400		//自定义板块是400个,其它均为100个
#define ZJHHYBLOCK_NUM	200		//证监会行业板块数
#define HKHYBLOCK_NUM	200		//港股行业板块数

#define DY_START 		400
#define HY_START 		500
#define CON_START		600
#define ZHB_START		700
#define ZJHHY_START		800
#define HKHY_START		1000

#define	USER_SIZE		400		//概念板块和组合板块中股票的最大数目

//板块DoMain类型
#define DOMAIN_TYPE			0	//分类
#define DOMAIN_DY			1	//地域
#define DOMAIN_HY			2	//行业
#define DOMAIN_CON			3	//概念
#define DOMAIN_ZHB			4	//组合
#define DOMAIN_USERBLOCK	5	//板块类型

#define MRK_CODE_SIZE	8
#define MRK_NAME_SIZE	24
#define MRK_JC_SIZE		8

#define CODE_SIZE		9
#define NAME_SIZE		17
#define SPELLCODE_SIZE	9

#define MAX_SEATNUM		40
#define MAXGPNUM		15000
#define	MAXKGITEM		1000

#define	DYNAKTYPE		100

#define SUCCESS			0
#define FAIL			-1
#define PI				3.14159

#define MEANLESS_DATA   7654321.00
#define COMPPREC		0.0001f
#define UPDATE_COMPPREC	0.001
#define MEANLESS_STR	"--- "

#define KAI_FONT		"楷体_GB2312"
#ifndef BIG5
	#define SONG_FONT "宋体"
#else
	#define SONG_FONT "灿砰"
#endif

#define STD_NEWJY		0	//标准
#define ZSZQ_NEWJY		1	//招商
#define ZXZQ_NEWJY		2	//中信
#define ZXJT_NEWJY		3	//中信建投
#define DGZQ_NEWJY		4	//东莞证券
#define GXZQ_NEWJY		5	//国信证券
#define CJZQ_NEWJY		6	//长江证券
#define YHZQ_NEWJY		7	//银河证券
#define GTJA_NEWJY		8	//国泰君安
#define SHZQ_NEWJY		9	//上海证券
#define HYZQ_NEWJY		10	//宏源证券
#define XYZQ_NEWJY		11	//兴业证券
#define GDZQ_NEWJY		12	//光大证券
#define HLZQ_NEWJY		13	//华林证券
#define MAX_DOMAINNUM	100

//用于行情的DrawGrid
#define DRAW_FRAME		0
#define DRAW_ALL        1
#define DRAW_NOREADDATA 2

#define ALL_SZ			0
#define SZ_GP			3
#define ALL_SH			4
#define SH_GP			7
#define ALL_ALL			10
#define DS_ALL			11

const short SZ		=	0;
const short SH		=	1;
const short SZ_SH	=	3;

//系统支持的基本面类型
const short TEXTXX	=	117;	//大文本文件
const short YCQSXX	=	124;	//远程券商信息

//报警原因
#define HASBIGBUY				0           //有大买单
#define HASBIGSELL				1           //有大卖单
#define VOLRATIOBREAK			2           //量比突破
#define PRANGEBREAK				3			//价幅突破
#define PRANGEDROP				4			//价幅跌破
#define HUGEVOL					5			//大量
#define HUGEAMOUNT				6			//大笔
#define TICKDIFFHIGH			7           //单笔价差高
#define TICKDIFFLOW				8           //单笔价差低
#define LOWHIGH					9           //低开高走
#define HIGHLOW					10          //高开低走
#define NEWMAX					11			//创新高
#define NEWMIN					12          //创新低
#define ABSHIGHWARN				13			//绝对上破
#define ABSLOWWARN				14			//绝对下破
#define ABSHANDCHG				15			//换手率突破(条件预警)
#define	ABSAPRATE				16			//折溢价率(条件预警)
#define UPCLOSE					17			//涨停板
#define DOWNCLOSE				18			//跌停板
#define UPOPEN					19          //打开涨停
#define DOWNOPEN				20			//打开跌停
#define UPDOWN					21			//涨停至跌停
#define DOWNUP					22			//跌停至涨停
#define TICKHUGEAMO				23			//连续大笔
#define ABSHIGHWARN2			24			//无效预警
#define ABSLOWWARN2				25			//无效预警
#define ABSHANDCHG2				26			//无效预警
#define ABSAPRATE2				27			//无效预警

#define TJWARNBASE				100			//条件预警的基数

#define MAX_ABSWARNMAXNUM		2000		//绝对价位报警的股票数为2000个
#define MAX_TJWARN				20			//条件预警个数最大为20	
#define	WARN_BUFFER_SIZE		2000		//预警缓冲区的大小为2000

#define MAX_CUSWEB_NUM			20
#define MAX_TJCOMB_NUM			30

#define TOTAL_DY_NUM			32
#define TOTAL_HY_NUM			52

#define WIDEDATA_HQ			0		//完全行情数据
#define WIDEDATA_SORT		1		//完全行情数据(排序用)
#define WIDEDATA_IMG_STAT	2		//完全统计行情数据
#define WIDEDATA_TCK_STAT	3		//完全统计行情数据
#define WIDEDATA_CWVIEW		4		//财务透视数据
#define WIDEDATA_ZNJP		5		//智能解盘数据
#define WIDEDATA_IWDATA_GP	6		//资讯工场数据
#define WIDEDATA_IWDATA_HY	7		//资讯工场数据
#define WIDEDATA_IWDATA_OB	8		//资讯工场数据
#define WIDEDATA_IWDATA_FL	9		//资讯工场数据
#define WIDEDATA_TA0		10		//TA分笔统计(当天)
#define WIDEDATA_TA4		11		//TA分笔统计(4天统计)
#define WIDEDATA_TA04		12		//同时含当天和4天统计的
//////////////////////////////////////////////////////////////////
//自定义消息宏
#define UM_HKNEWSSWITCH_MSG			WM_USER+1103
#define UM_FILLREALINFO_MSG			WM_USER+1104

#define UM_HSCROLLBAR				WM_USER+1105
#define UM_VSCROLLBAR				WM_USER+1106
#define UM_TABMESSAGE				WM_USER+1107
#define UM_GRIDREINIT				WM_USER+1108
#define UM_FILLRQLIST				WM_USER+1109
#define UM_UDPRECEIVE				WM_USER+1110
#define UM_FILLDOMAIN				WM_USER+1111
#define UM_AFTERFORMULA				WM_USER+1112
#define UM_DRAGZBTREE				WM_USER+1113
#define UM_UPDATE_STATUSBAR			WM_USER+1114
#define UM_CHANGEGRIDWIDTH			WM_USER+1115

#define WM_WEBINFO_MESSAGE			WM_USER+1109
#define UM_RESETSCROLLBAR			WM_USER+1150

#define UM_OPENIWCONTENT			WM_USER+1158
#define UM_OPENWEBWIN				WM_USER+1159
#define UM_ONLINEENTERMSG			WM_USER+1160
#define UM_PROCESSAUTOUPINFO		WM_USER+1161
#define	UM_POPMYIE					WM_USER+1162

#define UM_PROGRESS					WM_USER+1200
#define UM_INFO						WM_USER+1201
#define WM_RICHEDIT_MESSAGE			WM_USER+1202

#define SOCKET_MESSAGE              WM_USER+1203
#define WM_TREE_MESSAGE             WM_USER+1204
#define WM_KEYGUY_MESSAGE           WM_USER+1205
#define WM_TREEZS_MESSAGE			WM_USER+1206
#define WM_CHANGEGP_MESSAGE			WM_USER+1207
#define ASYN_STATHQ_MESSAGE			WM_USER+1208
#define ASYN_HQ_MESSAGE             WM_USER+1209
#define WM_CHANGEALL_MESSAGE        WM_USER+1210

#define SHOWINFO_MESSAGE            WM_USER+1212

#define UM_REPAINTCOM_MESSAGE		WM_USER+1214

#define UM_SET_PLAY					WM_USER+1220
#define UM_GET_PLAY					WM_USER+1221
#define UM_CONTINUEPLAY				WM_USER+1222

#define UM_CALCANDPAINT				WM_USER+1223

#define UM_SET_BIGBSP				WM_USER+1224
#define UM_SET_FXPERIOD				WM_USER+1225
#define UM_SET_OFTENZB				WM_USER+1226
#define UM_CLOSESYNC				WM_USER+1227

#define UM_POPINFODLG				WM_USER+1235

#define UM_TBBD_MESSAGE				WM_USER+1236
#define UM_GRIDSETTITLE				WM_USER+1237	

#define UM_POPGGINFO				WM_USER+1238
#define UM_SWITCHLINK				WM_USER+1239
#define UM_POPZSTICKDLG				WM_USER+1240
#define UM_SWITCHTOPVIEW			WM_USER+1241

#define UM_CONNECT_SUCCESS			WM_USER+1310

#define UM_AUTOCONNECT				WM_USER+0x1410	//自动重连的消息
#define UM_READANDPAINT				WM_USER+0x1411	//让ComView重新请求数据
#define UM_MOUSELEAVE				WM_USER+0x1412
#define UM_AUTOCONNECTDS			WM_USER+0x1413	//自动连接扩展行情主站

#define UM_TREEVIEW_MESSAGE			WM_USER+0x1500
#define UM_TREEREFLECT_MESSAGE		WM_USER+0x1501

#define UM_CJZX_MESSAGE				WM_USER+0x1502
#define UM_CJZX_DAY_MESSAGE			WM_USER+0x1503

#define UM_JBFX_MESSAGE				WM_USER+0x1510
#define UM_JBFX_FEEDBACK_MESSAGE	WM_USER+0x1511
#define UM_FILLZXTREE_MESSAGE		WM_USER+0x1513
#define UM_CJZX2_TITLE_MESSAGE		WM_USER+0x1514
#define UM_TIMEFILLZX2_MESSAGE		WM_USER+0x1515
#define UM_REGEXIT_MESSAGE			WM_USER+0x1516
#define UM_REGLOGIN_MESSAGE			WM_USER+0x1517
#define UM_PROXY_MESSAGE			WM_USER+0x1518

#define UM_STARTFUNC_MESSAGE		WM_USER+0x1520

#define UM_SWITCHCALLBACK			WM_USER+0x1522

#define UM_IE_BACK_MESSAGE			WM_USER+0x1540
#define UM_IE_FORWARD_MESSAGE		WM_USER+0x1541
#define UM_IE_MAINPAGE_MESSAGE		WM_USER+0x1542

#define UM_REGLINK_MESSAGE			WM_USER+0x1543
#define UM_REGLINK_MESSAGE2			WM_USER+0x1544

#define UM_DOLEVINBUY_MESSAGE		WM_USER+0x1547
#define UM_DOLEVINSELL_MESSAGE		WM_USER+0x1548
#define UM_DOWTCANCEL_MESSAGE		WM_USER+0x1549
#define UM_DOQZSDS_MESSAGE			WM_USER+0x1550

#define UM_STAR_URL1				WM_USER+0x1551
#define UM_STAR_URL2				WM_USER+0x1552
#define UM_STAR_URL3				WM_USER+0x1553

#define UM_LEVIN_FEEDBACK_MSG		WM_USER+0x1660

#define UM_CONSTRUCTPAD_MESSAGE		WM_USER+0x1601
#define UM_REFRESHCUSTOMBAR			WM_USER+0x1602

#define UM_HQCONNECTTIMER_MESSAGE	WM_USER+0x1701
#define UM_NETWORK_ERR				WM_USER+0x1702

#define UM_TDXWSHOW					WM_USER+0x1803
#define UM_SHOWURGENT_MESSAGE		WM_USER+0x1804

#define UM_ROLLINFOCFG_MSG			WM_USER+0x1903
#define UM_SETROLLINFO_MSG			WM_USER+0x1904
#define UM_ROLLINFOPOP_MSG			WM_USER+0x1905
#define UM_SLOWROLLINFOPOP_MSG		WM_USER+0x1906
#define UM_CLOSEROLL_MSG			WM_USER+0x1907
#define UM_CLOSESLOWROLL_MSG		WM_USER+0x1908

#define UM_SETPERIOD				WM_USER+100
#define UM_SETAXIS					WM_USER+101
#define UM_SELECTMANY				WM_USER+102		//专家指示
#define UM_OVERLAP					WM_USER+103
#define UM_ADJUSTPARAM				WM_USER+104
#define UM_MENUZB					WM_USER+105
#define UM_HXZBDLG					WM_USER+106
#define UM_CHANGECOLOR				WM_USER+107
#define UM_CHANGEPARAM				WM_USER+108
#define UM_CHANGEZB					WM_USER+109
#define UM_CHANGESPEZB				WM_USER+110
#define UM_CHANGEZS					WM_USER+111
#define UM_MODIFYFORMULA			WM_USER+112
#define UM_INDEXHELP				WM_USER+113
#define UM_DELZS					WM_USER+114
#define UM_DYNCHGPARAM				WM_USER+115
#define UM_RESTORE					WM_USER+116		//通知FormView恢复
#define UM_HISDLG					WM_USER+118
#define UM_QJSTAT					WM_USER+119
#define UM_UPDATEEDITDATA			WM_USER+120
#define UM_SELNUM					WM_USER+121
#define UM_TQ						WM_USER+125
#define UM_NOTQ						WM_USER+126
#define UM_UNOVERLAP				WM_USER+128
#define UM_MANYDAYZST				WM_USER+129
#define UM_AUTOVERLAP				WM_USER+130
#define UM_ADDFORMULAPACK			WM_USER+131
#define UM_USEFORMULAPACK			WM_USER+132
#define UM_UNUSEFMLPACK				WM_USER+133
#define UM_LOADCOM2FML				WM_USER+134
#define UM_TABSWITCHCHG				WM_USER+135
#define UM_TICKZSTUNDO				WM_USER+136
#define	UM_AUTOCALCTJBLOCK			WM_USER+137

#define UM_SET_CUR_DATE				WM_USER+4059
#define UM_SET_STOCK				WM_USER+4060
#define UM_GET_STOCK				WM_USER+4061
#define UM_READDATA					WM_USER+4062
#define UM_REFLECT					WM_USER+4063
#define UM_UPDATE					WM_USER+4064
#define UM_SETSEL					WM_USER+4065
#define UM_GETSEL					WM_USER+4066
#define UM_FORCEUPDATE				WM_USER+4067

#define UM_SWITCHSTYLE				WM_USER+4070
#define UM_SET_DATE					WM_USER+4071
#define UM_GET_DATE					WM_USER+4072

#define UM_GET_FXTDATE				WM_USER+4073
#define UM_GET_FXTDATA				WM_USER+4074

#define UM_KQS_ACK					WM_USER+4075

#define UM_TOZSTIME					WM_USER+4080
#define UM_ZSTSYNCH					WM_USER+4081

#define UM_ZH_READSORT				WM_USER+4088
#define UM_CB_FXTSTART				WM_USER+4090
#define UM_CB_FXTEND				WM_USER+4091
#define UM_CB_DATE					WM_USER+4092

#define UM_NOTIFY_INFODLG			WM_USER+4110
#define UM_DELETE_INFODLG			WM_USER+4112

#define UM_HIDESHOWBUTTON			WM_USER+4113
#define UM_ADVLFM					WM_USER+4200

#define UM_GLHQ_SWITCH				WM_USER+4420

#define UM_TV_DATE					WM_USER+4300
#define UM_TV_PERIOD				WM_USER+4301

#define UM_OPENBLOCKHQ				WM_USER+4400
#define UM_COM2SWITCH1				WM_USER+4401

#define COMM_DISCONNECT				WM_USER+3020
#define UM_GETDATA_ACK				WM_USER+3021
#define UM_STATUS_REQ				WM_USER+3022
#define UM_STARTCONNECT				WM_USER+3023
#define UM_STARTLOGIN				WM_USER+3024
#define UM_GETMAINID				WM_USER+3026
#define UM_NOTIFYRESULT				WM_USER+3027	//通知选股结果可用
#define UM_LOGOENBALE				WM_USER+3029	//通知LogoDlg Enable
#define UM_BLOCKMODIFIED			WM_USER+3030	//板块股有变化时,通知GridView
#define UM_SHOWGUIDE				WM_USER+3031	//显示Guide对话框
#define UM_SWITCHNEXTHOST			WM_USER+3032	//通过LogoDlg 切换主站
#define UM_RESETFYSWITCHBAR			WM_USER+3033	//复位行情图中的切换区
#define UM_WIDEDATA_SORT			WM_USER+3034	//刷新排序行情
#define UM_WIDEDATA_HQSTAT			WM_USER+3035	//刷新统计行情

#define UM_POPMSG					WM_USER+3040

#define UM_BK_START					WM_USER+3050	//后台行情->开始
#define UM_BK_HQ_OK					WM_USER+3051	//后台行情就绪
#define UM_BK_SORTHQ_OK				WM_USER+3052	//后台行情就绪(收到后需排序)
#define UM_BK_RMHQ_OK				WM_USER+3053	//热门板块分析中的后台行情就绪(收到后需排序)
#define UM_BK_IMGSTATHQ_OK			WM_USER+3054	//统计行情就绪(收到后需排序)
#define UM_BK_TCKSTATHQ_OK			WM_USER+3055	//统计行情就绪(收到后需排序)
#define UM_BK_SWICTH_OK				WM_USER+3056	//后台行情(接口部分)
#define UM_BK_GLHQ_OK				WM_USER+3057	//关联行情
#define UM_BK_INFODATA_OK			WM_USER+3058	//后台信息类数据
#define UM_BK_WTBKDATA_OK			WM_USER+3059	//后台信息类数据(委托)
#define UM_BK_END					WM_USER+3100	//后台行情->结束

#define UM_SHOW_PAD					WM_USER+5010	//显示PAD，分读数据与不读数据两种
#define UM_HIDE_PAD					WM_USER+5011	//隐去PAD
#define UM_SWAP_MENU				WM_USER+5012	//在菜单状态下回车时菜单窗口的ID发生变化
#define UM_CHANGE_MENU_COLOR		WM_USER+5013	//改变菜单的颜色
#define UM_MENU_SETBLOCK			WM_USER+5014	//设置板块
#define UM_MENU_INITPOS				WM_USER+5015	//回到初始的大字菜单
#define UM_SEARCH_BLOCK				WM_USER+5016	//搜索某只股票的所属板块
#define UM_RESETGRIDTAB				WM_USER+5017	//重新设置行情表标签
#define UM_SEARCH_GPGXD				WM_USER+5019	//搜索某只指数的贡献度
#define UM_SEARCH_HKSEAT			WM_USER+5020	//搜索港股席位

#define UM_SPECIALZX_MSG			WM_USER+5030	//资讯树中特别的资讯功能的处理
#define UM_RELOAD_MSG				WM_USER+5031	//重新装载的通知消息
#define UM_TIMERMORE_REQ			WM_USER+5032

#define UM_MDITABSELCHANGED			WM_USER+5040
#define	UM_RSSDATETIMESORT			WM_USER+5041
//////////////////////////////////////////////////////////////////

#define ELAPSE_SECOND		5			//分时等数据刷新
#define TICK2_SECOND		1			//逐笔成交等数据刷新
#define QUEUE_SECOND		1			//买卖队列等数据刷新
#define MARKETINFO_SECOND	12			//市场信息刷新
#define DDEHQ_SECOND		40			//A+H等统计行情刷新时间
#define STATHQ_SECOND		20			//DDE等统计行情刷新时间
#define RMREPORT_SECOND		30			//热门板块分析刷新时间

#define FOR_UNIT(i) for( i=0;i<m_Units.Num && m_Units.pUnit[i];i++)

#define MAX_FMLPACK_NUM		50
#define MAX_BANDSTOCK_NUM	200

#define	MAX		max
#define MIN		min

#define NOXS	0		//没有小数
#define ZS		1		//指数
#define XS2		2		//2位小数
#define	XS3		3		//3位小数
#define XS4		4		//4位小数
#define XS5		5		//5位小数

#define FZNUM 360
#define equfz(x,y) (fabs(x-y) < 0.00001)

#define  CLEAR_FXTOOL      100    //  清除画线
#define  DRAW_FXTOOL       101    //  显示画线
#define  SAVE_FXTOOL       102    //  储存画线
#define  CLEARONE_FXTOOL   103    //  清除画线
#define  LDXD_FXTOOL        0     //  两点线段
#define  LDLX_FXTOOL        1     //  两点连线
#define  PXGP_FXTOOL        2     //  平行股票箱
#define  GOLD_FXTOOL        3     //  黄金分割
#define  DFBX_FXTOOL        4     //  百分比线
#define  BDX_FXTOOL         5     //  波段线
#define  ZQX_FXTOOL         6     //  周期线
#define  FBLQ_FXTOOL        7     //  费波拉契
#define  SSGS_FXTOOL        8     //  上升甘氏
#define  XJGS_FXTOOL        9     //  下降甘氏
#define  ZSX_FXTOOL        10     //  阻速线
#define  XXHG_FXTOOL       11     //  线形回归线
#define  XXHGD_FXTOOL      12     //  线形回归带
#define  XXHGC_FXTOOL      13     //  线形回归延长

///////////////////////////////////////////////////////////////
#define MAX_ANDSTR		10
#define MAX_FINDRESULT	500

#define MAX_FJNUM		800

#define MAX_UNIT_NUM	50
#define MAX_ZBT_NUM		16

#define RATIONNUM	((MAX_ZBT_NUM*(MAX_ZBT_NUM+2))>>1)

#define FIXED_HEIGHT	401
#define FIXED_LOWHEIGHT	240
#define HQ_TEXTH		19
#define HQ_XSPACE		3
#define SWITCH_HEIGHT	21
#define SWITCH_HEIGHT2	18

extern int  RECENTNUM;

#define WANT_TICKS		180
#define ADDED_TICKS		200
#define WANT_MMPS		20
#define ADDED_MMPS		100
#define WANT_BIGVOL		500
#define ADDED_BIGVOL	100

#define	WANT_ZSTTICKS	1000
#define	ADDED_ZSTTICKS	2000

const int HideShowBtn_Hq	= 0;
const int HideShowBtn_Zst	= 1;
const int HideShowBtn_Fxt	= 2;
const int HideShowBtn_Mul	= 3;
const int HideShowBtn_Other	= 4;
const int HideShowBtn_His1	= 5;
const int HideShowBtn_His2	= 6;
const int HideShowBtn_His3	= 7;
const int HideShowBtn_His4	= 8;
const int HideShowBtn_Jbm	= 9;
const int HideShowBtn_Xxm	= 10;
const int HideShowBtn_WebInfo= 11;
const int HideShowBtn_PadSet = 12;
const int HideShowBtn_Homepage=13;

const int Reflect_ToZstime	 = 1;
const int Reflect_ZstSynch	 = 2;
const int Reflect_Cb_FxtStart= 3;
const int Reflect_Cb_FxtEnd	 = 4;
const int Reflect_Cb_Date	 = 5;
const int Reflect_Tv_Date	 = 6;
const int Reflect_Tv_Period	 = 7;

#define CLR_NOWTESTING		RGB(255,0,0)	//红色
#define CLR_TESTED			RGB(0,0,255)	//蓝色
#define CLR_INTERRUPT		RGB(132,0,0)	

//InfoDlg的风格
#define FX_INFODLG		0
#define QJ_INFODLG		1

#define CREATE_TMPDC	\
	CDC *pDC = m_pView->GetDC(); \
	CDC m_MyDC;	\
	CBitmap m_MyBitmap,*m_pOldBitmap; \
	m_MyDC.CreateCompatibleDC(pDC); \
	m_MyBitmap.CreateCompatibleBitmap(pDC,Width(),Height()); \
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);

#define DESTORY_TMPDC	\
	m_MyDC.SelectObject(m_pOldBitmap); \
	m_MyBitmap.DeleteObject(); \
	m_MyDC.DeleteDC(); \
	m_pView->ReleaseDC(pDC);

#define SHOW_NOMODAL_CORNER	\
	CSize sizeScreen(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)); \
	CRect rect; \
	GetWindowRect(&rect); \
	CMainFrame *tempMainFrame = (CMainFrame *)AfxGetMainWnd(); \
	BOOL bHasStatusBar = tempMainFrame->m_wndStatusBar.IsWindowVisible(); \
	int tempCy = bHasStatusBar?(sizeScreen.cy-(g_GH.bBigStatusBar?31:28)-rect.Height()):(sizeScreen.cy-rect.Height()); \
	if(g_bHasCaption) tempCy+=4; \
	SetWindowPos(NULL,sizeScreen.cx-rect.Width(),tempCy-20,0,0,SWP_NOSIZE); 

#define LOAD_CURSOR(_hCursor,_IDC_Cursor) \
		_hCursor = (HCURSOR)::LoadImage(m_hInstance,\
		MAKEINTRESOURCE(_IDC_Cursor),IMAGE_CURSOR,32,32,LR_VGACOLOR);

#define LOAD_ICON(_hIcon,_IDC_Icon) \
		_hIcon = (HICON)::LoadImage(m_hInstance,\
		MAKEINTRESOURCE(_IDC_Icon),IMAGE_ICON,32,32,LR_VGACOLOR);

enum	//特殊画线方式
{
	VAL_EQUVOL_LINE ,	//等量图
	VAL_EQUK_LINE,		//等量Ｋ线
	VAL_CAN_LINE,		//压缩图
	VAL_OX_LINE,		//圈叉图
	VAL_OXHL_LINE,		//高低圈叉图
	VAL_TPO_LINE,		//四度空间
	VAL_TBL_LINE,		//新三价线
	VAL_TOW_LINE		//宝塔线
};

////////////////////////////////////////////////////////////////////////
//TCPIP远程宏

#define SIMHQ_WANTNUM		250
#define GRIDHQ_MAXNUM		80
#define MULHQ_WANTNUM		50
#define MAXSTAT_MAXNUM		50

#define IMG_STAT_HQ			0
#define TCK_STAT_HQ			1

#define HQSTAT_SEC			12

#define	HQ_SUB_KEY			_T("Software\\Microsoft\\100NIAN")
#define HQ_VALUE_STRING		_T("100NIAN")
#define HQ_L2_VALUE_STRING	_T("100NIAN2")

#define ERR_NOTYPE		11
#define ERR_NOINMODE	12
#define ERR_NOUSER		13
#define ERR_ERRPASSWORD	14
#define ERR_INVALID		15
#define ERR_EXCEEDUSER	16
#define ERR_ERRNETCARD	17
#define ERR_ERRTIME		18
#define ERR_ERRSERIAL	19
#define ERR_ERRVER		20
#define ERR_OTHER		21

//服务类型
#define WT_SERTYPE		1
#define HQ_SERTYPE		2
#define CHAT_SERTYPE	3
#define TAPI_SERTYPE	4
#define UDP_SERTYPE		5

//拨入方式
#define OLDFY_INMODE	0
#define DOS_INMODE		1
#define TDXW_INMODE		2
#define WEB_INMODE		3
#define PDA_INMODE		4
#define WIDE_INMODE		5
#define PLAY_INMODE		6

//接收消息的窗口类型宏
#define MAX_RECWIN_NUM		16

#define MAIN_WIN			0
#define GRIDVIEW_WIN		1
#define COMVIEW_WIN			2
#define HISVIEW_WIN			3
#define TXTVIEW_WIN			4
#define NEWSVIEW_WIN		5
#define LOGO_WIN			6
#define COMVIEW2_WIN		8
#define ZHSORT_WIN			9
#define DATADIG_WIN			10
#define BKDATA_WIN			11
#define HISFX_WIN			12
#define WIDEDATA_WIN		13
#define DOWNLOAD_WIN		14
#define ZSTICKDATA_WIN		15
#define	SYNCDATA_WIN		16
#define DSCONNECT_WIN		18
#define HKSEATQUERY_WIN		19
#define	PREVIEWUNIT_WIN		20
#define	INFOUPLOAD_WIN		21

//缓冲区的大小
#define MAX_SENDBUF_NUM		128

//请求的几种类型
#define BUFFER_REQTYPE		0
#define HQ_REQTYPE			1
#define DIRECT_REQTYPE		2
#define RELAY_REQTYPE		3

//代码类型
#define CODE_NONE		-1
#define CODE_SZ			0
#define CODE_SH			1
#define CODE_SZSH		2

///////////////////////////////
// 证券类型
#define CODE_SZAG       0	//A股
#define CODE_SZQZ       1	//权证
#define CODE_SZGZ       2	//国债
#define CODE_SZZQ       3	//企债
#define CODE_SZKZHZQ    4	//转债
#define CODE_SZGZHG     5	//回购
#define CODE_SZJJ       6	//基金
#define CODE_SZBG       7	//B股
#define CODE_ZXB        8	//中小企业
#define CODE_CYB		9	//中小企业
#define CODE_SZOTHER    10	//其它

#define CODE_SHAG      11	//A股
#define CODE_SHQZ      12	//权证
#define CODE_SHGZ      13	//国债
#define CODE_SHZQ      14	//企债
#define CODE_SHKZHZQ   15	//转债
#define CODE_SHGZHG    16	//回购
#define CODE_SHJJ      17	//基金
#define CODE_SHBG      18	//B股
#define CODE_SHOTHER   19	//其它

#define CODE_KFJJ	   20	//开放式基金
#define CODE_SB		   21	//三板

#define CODE_SZSPEC    22
#define CODE_SHSPEC    23

///////////////////////////////

#define JBM_TDX			-1

#define MAX_F10CAPTION	40

//大字菜单版面宏
#define MENU_TOP        0
#define MENU_DPFX       1
#define MENU_BJFX       2
#define MENU_JSFX       3
#define MENU_PERIOD     4
#define MENU_INDEX      5
#define MENU_TBBD       6
#define MENU_MULGP      7
#define MENU_XXM        8
#define MENU_SPEC		9
#define MENU_OTHER		10
#define MENU_BJFX_LB	11
#define MENU_MULGP_LB	12
#define MENU_TBBD_LB	13
#define MENU_EXPSYS		14
#define MENU_CHOICESYS	15
#define MENU_WARNSYS	16
#define MENU_STATICSYS	17

#define MENU_LEAF			30
#define MENU_BJFX_SELBLOCK	31
#define MENU_MULGP_SELBLOCK	32
//
#define L_DPFX_BASE			100
#define L_BJFX_BASE			500
#define L_MULGP_BASE		1200
#define L_TBBD_BASE			1900
//即时分析
#define L_JSFX_ZST			3000
#define L_JSFX_TICK			3001
#define L_JSFX_FJB			3002
#define L_JSFX_FSB			3003
#define L_JSFX_MMP			3004
#define L_JSFX_LB			3005
#define L_JSFX_MMLD			3006
#define L_JSFX_TICKT		3007
//指标
#define L_INDEX_BASE		3100
//委托
#define L_WT				4000
#define L_QUIT				4001
#define L_WEBINFO			4002
//特色功能
#define L_QJTYPE1			4010
#define L_QJTYPE2			4011
#define L_QJTYPE3			4012
#define L_QJTYPE4			4013

#define L_RMREPORT			4101
#define L_QJREPORT			4102
#define L_DATADIG			4106

#define L_FORMULA			4107
#define L_TJTEST			4108
#define L_JYTEST			4109
#define L_TESTINDEX			4110

#define L_CONDITIONCHOICE	4113
#define L_SIM_CHOICE		4114
#define L_CLECHOICE			4115
#define L_PLUGIN			4116
#define L_SELGP				4117

#define L_ZBREPORT			4118
#define L_QRREPORT			4119

#define L_ALLWARNGP			4121
#define L_ALLWARNCFG		4122
#define L_TJWARNGP			4123
#define L_TJWARNCFG			4124

//系统工具
#define L_BLOCKOPTION		4200
#define L_OPTION			4201
#define L_INVEST			4202
#define L_PROFIT			4203
#define L_BLOCKCALC			4204
#define L_AUTOUP			4205
#define L_CONNECTCFG		4206
#define L_CONNECT			4207
#define L_DISCONNECT		4208
#define L_CONNECTINFO		4209
#define L_DISCONNECTINFO	4210
#define L_DOWNLOAD			4211
#define L_NODETOOL			4212
#define L_FORUM				4213
#define L_COOLINFO			4214
#define L_HELP_FINDER		4215

#define L_XXM_BASE			4300	//公告消息

#define VAL_BJFX_BLOCK			0
#define VAL_MULGP_BLOCK			1

//BlockWin与MenuWin的画面尺寸宏
#define MENUTITLE_TOP			18
#define FIRSTMENU_TOP			60
#define MENU_HEIGHT				35

#define BLOCKITEM_HEIGHT		34
#define STOCKITEM_HEIGHT		22
#define BLOCKITEM_WIDTH			110
#define STOCKITEM_WIDTH			63
#define BLOCKITEM_GAP			44
#define STOCKITEM_GAP			24
#define BLOCKITEM_STARTX		20
#define STOCKITEM_STARTX		18

#define MAX_ZST_MANYDAY			10	//最多支持10天的走势图

#define AUTOCONNECT_RESET		0
#define AUTOCONNECT_PENDING		1
#define AUTOCONNECT_RECONNECT	2

////////////////////////////////////////////////////

#define	MAX_GROUPMARK			7
#define MAX_INFO_TITLE			350

#define IGN_CJZX				0	//财经资讯
#define IGN_MINE				1	//信息地雷
#define IGN_FLZL				2	//分类F10资料
#define IGN_GGBW				3	//个股备忘
#define IGN_OTHER				4	//其它

#define GN_TREETYPE				0
#define GP_TREETYPE				1
#define ZB_TREETYPE				2
#define JB_TREETYPE				3
#define ZX_TREETYPE				4
#define WEB_TREETYPE			5
#define ZX2_TREETYPE			6
#define ZX3_TREETYPE			7

#define AREA_NORMAL_WEBPAGE		0	//一般的网页
#define AREA_UID_PWD_WEBPAGE	1	//自动填单的网页
#define AREA_CHATROOM			2	//聊天室
#define AREA_BBS				3	//投资论坛
#define AREA_MINE				4	//信息地雷
#define AREA_FLZL				5	//分类资料
#define AREA_GGBW				6	//个股备忘
#define AREA_KEXPLAIN			7	//智能解盘
#define AREA_USERINFO			8	//用户信息
#define AREA_WT					9	//委托模块

//WEB网页的输入类型
#define EDIT_INPUT_TYPE			0
#define SELECT_INPUT_TYPE		1
#define FORM_SUBMIT_TYPE		2
#define BTN_SUBMIT_TYPE			3

//盘中监控预警的类型
enum
{
	WARN_RADAR,
	WARN_TJ
};

#define MAX_FY_GROUP			100
#define MAX_FYNUM_IN_GROUP		18

////////////////////////////////////////////////////

//安全方式
#define TDX_SECURE			0		//通达信加密
#define AUTHPASS_SECURE		1		//认证口令
#define CA_SECURE			2		//安全证书

//自动叠加的方式
#define AUTO_OVERLAP_IDX	0x0001	//指数
#define AUTO_OVERLAP_MG		0x0002	//正股

#define MAX_HISCACHE		1000

///////////////////////////////////////////////////

const	MAX_MKT_NUM			=	64;
const	MAX_MKTGUI_NUM		=	16;
const	DS_FROM				=	30000;
const	MAX_DS_STKNUM		=	20000;
const	MAX_DS_TIMERNUM		=	200;

//键盘精灵相关
const	KEYGUY_FUND			=	100000;
const	KEYGUY_DSCODE		=	65000;
const	KEYGUY_SEPCZB		=	62000;
const	KEYGUY_BLOCK		=	60000;
const	KEYGUY_MAINLINE		=	59800;
const	KEYGUY_DSTYPE		=	59000;
const	KEYGUY_ZB			=	51000;
const	KEYGUY_OPER			=	50000;
const	KEYGUY_OLDNAME		=	130000;

//个人版类型
const	PERSONAL_QD			=   1;
const	PERSONAL_BL			=   2;
const	PERSONAL_YG			=	3;	//西南股王
const	PERSONAL_JJTZ		=	4;	//晋境投资
const	PERSONAL_JJTZEX		=	5;	//晋境投资增强
const	PERSONAL_JJDX		=	6;	//晋境投资短线精灵版
const	PERSONAL_JJVIP		=	7;	//晋境荣赢尊贵VIP版
//通达信加密INI文件
const  COMTE_VIP_HOST		=	1;
const  COMTE_L2_HOST		=	2;
const  COMTE_L2_USERHOST	=	3;
const  COMTE_FHOST			=	4;
const  COMTE_TVHOST			=	5;
const  COMTE_DS_HOST		=	6;

#define COMTE_PASSWORD		"fortune@tend"

#define DATASYNC_NULL		0
#define DATASYNC_TEND		1
#define DATASYNC_JJ			2
#define DATASYNC_FILE		3
#define DATASYNC_ZIPFILE	4

//通告相关
#define URGENT_INIT			0	
#define URGENT_DURING		1
//
#define URGENT_NORMAL		0
#define URGENT_USER			1
#define URGENT_BG			2
#define URGENT_TDXFREE		3

//自动探测的秒数
#define DETECT_SECOND		20
#define DS_DETECTCODE_MIN	5

//主力监控类型
#define WARN_NONE			0
#define WARN_LINE			1				 	//拖拉机单(A,B,C:A+C=B+B,A-B=D*10^N,abs(D)<10)
#define WARN_ORDERSHOCK		2					//委托异动(大委大撤)
#define WARN_LARGEVOL		3					//主力大单
#define	WARN_QUICKRISE		4					//快速拉升
#define	WARN_QUICKFALL		5					//快速下跌
#define	WARN_REBOUND		6					//快速反弹
#define	WARN_QUICKDIVE		7					//高位跳水
#define	WARN_PRICEUPSHOCK	8					//价格上冲(已成交15分钟,最近3分钟涨幅达5%,之前振幅不超过2%)
#define	WARN_PRICEDNSHOCK	9					//价格下破(已成交15分钟,最近3分钟跌幅达5%,之前振幅不超过2%)
#define	WARN_TICKSHOCK		10					//单笔异动(价格异动)
#define	WARN_VOLUMESHOCK	11					//成交异动(已成交15分钟,最近5分钟分钟均量为全天的均量的2-3倍)

//拖拉机单预警
#define ORDERSHOCK_BORDER	0x00
#define ORDERSHOCK_BCANCEL	0x01
#define ORDERSHOCK_SORDER	0x02
#define ORDERSHOCK_SCANCEL	0x03

#define SH_DPCODE			"999999"
#define SH_ADPCODE			"999998"
#define SH_BDPCODE			"999997"

#define SZ_DPCODE			"399001"
#define SZ_ADPCODE			"399002"
#define SZ_BDPCODE			"399003"

#define HS300_SZZS			"399300"
#define HS300_SHZS			"000300"

#define ZXB_DPCODE			"399005"
#define CYB_DPCODE			"399006"

#define HK_DPCODE			"HSI"
#define HK_DPSETCODE		27

#define LCIDX_FIX			"TOP"
#define LCPT_FIX			"394"
#define LCEXT_FIX			"397"

#define HY_STRING_LEN		21

#define	MAX_PLUGINCLIENT	10
#define TDX_WEBPAGE			"bainian.yhglpt.com/qjjk/1.asp"
#define TDXBBS_WEBPAGE		""
#define TDXHTTP_WEBPAGE		""
#define TDXVIP_WEBPAGE		""
#define USERCENTER_WEBPAGE	""
#define BUYDS_WEBPAGE		""
#define GUBA_WEBPAGE		""
#define REPORT_WEBPAGE		""
#define HK_GP_F10_WEBPAGE	"http://company.caihuanet.com/stock_info.php?code=xxxxxx"
#define HK_QZ_F10_WEBPAGE	"http://company.caihuanet.com/warrant.php?code=xxxxxx"
#define HK_JYTIME_WEBPAGE	"http://www.hkex.com.hk/PrintFriendly/PrintFriendly.asp?url=http://www.hkex.com.hk/tradinfo/tradcal/caln#ShortYear#_c.htm"
#define	HEXUN_WEBPAGE		"http://stockdata.stock.hexun.com/f10/gonggaozhaiyao.aspx?stockid=xxxxxx"

#define CLOSE_HANDLE(hHandle) {if(hHandle!=NULL){CloseHandle(hHandle);hHandle=NULL;}} 

#define   MWT_MIN_FACTOR   (0) 
#define   MWT_MAX_FACTOR   (0xFF) 

#define	  MAX_DXBLOCK_PER	10
#define	  MAX_PZCALC		10

//登录错误码
#define	ERR_CONNECT	-1
#define	ERR_COMM	-2
#define	ERR_LOGIN	-3

enum
{
	CALC_TJBLOCK,
	CALC_ZSBLOCK,
	CALC_DXINDEX1,
	CALC_DXINDEX2,
};

#define		CALC_THREAD_NUM		2

#endif
