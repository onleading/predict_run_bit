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


#define USERBLOCK_NUM	400		//�Զ�������400��,������Ϊ100��
#define ZJHHYBLOCK_NUM	200		//֤�����ҵ�����
#define HKHYBLOCK_NUM	200		//�۹���ҵ�����

#define DY_START 		400
#define HY_START 		500
#define CON_START		600
#define ZHB_START		700
#define ZJHHY_START		800
#define HKHY_START		1000

#define	USER_SIZE		400		//���������ϰ���й�Ʊ�������Ŀ

//���DoMain����
#define DOMAIN_TYPE			0	//����
#define DOMAIN_DY			1	//����
#define DOMAIN_HY			2	//��ҵ
#define DOMAIN_CON			3	//����
#define DOMAIN_ZHB			4	//���
#define DOMAIN_USERBLOCK	5	//�������

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

#define KAI_FONT		"����_GB2312"
#ifndef BIG5
	#define SONG_FONT "����"
#else
	#define SONG_FONT "�ө���"
#endif

#define STD_NEWJY		0	//��׼
#define ZSZQ_NEWJY		1	//����
#define ZXZQ_NEWJY		2	//����
#define ZXJT_NEWJY		3	//���Ž�Ͷ
#define DGZQ_NEWJY		4	//��ݸ֤ȯ
#define GXZQ_NEWJY		5	//����֤ȯ
#define CJZQ_NEWJY		6	//����֤ȯ
#define YHZQ_NEWJY		7	//����֤ȯ
#define GTJA_NEWJY		8	//��̩����
#define SHZQ_NEWJY		9	//�Ϻ�֤ȯ
#define HYZQ_NEWJY		10	//��Դ֤ȯ
#define XYZQ_NEWJY		11	//��ҵ֤ȯ
#define GDZQ_NEWJY		12	//���֤ȯ
#define HLZQ_NEWJY		13	//����֤ȯ
#define MAX_DOMAINNUM	100

//���������DrawGrid
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

//ϵͳ֧�ֵĻ���������
const short TEXTXX	=	117;	//���ı��ļ�
const short YCQSXX	=	124;	//Զ��ȯ����Ϣ

//����ԭ��
#define HASBIGBUY				0           //�д���
#define HASBIGSELL				1           //�д�����
#define VOLRATIOBREAK			2           //����ͻ��
#define PRANGEBREAK				3			//�۷�ͻ��
#define PRANGEDROP				4			//�۷�����
#define HUGEVOL					5			//����
#define HUGEAMOUNT				6			//���
#define TICKDIFFHIGH			7           //���ʼ۲��
#define TICKDIFFLOW				8           //���ʼ۲��
#define LOWHIGH					9           //�Ϳ�����
#define HIGHLOW					10          //�߿�����
#define NEWMAX					11			//���¸�
#define NEWMIN					12          //���µ�
#define ABSHIGHWARN				13			//��������
#define ABSLOWWARN				14			//��������
#define ABSHANDCHG				15			//������ͻ��(����Ԥ��)
#define	ABSAPRATE				16			//�������(����Ԥ��)
#define UPCLOSE					17			//��ͣ��
#define DOWNCLOSE				18			//��ͣ��
#define UPOPEN					19          //����ͣ
#define DOWNOPEN				20			//�򿪵�ͣ
#define UPDOWN					21			//��ͣ����ͣ
#define DOWNUP					22			//��ͣ����ͣ
#define TICKHUGEAMO				23			//�������
#define ABSHIGHWARN2			24			//��ЧԤ��
#define ABSLOWWARN2				25			//��ЧԤ��
#define ABSHANDCHG2				26			//��ЧԤ��
#define ABSAPRATE2				27			//��ЧԤ��

#define TJWARNBASE				100			//����Ԥ���Ļ���

#define MAX_ABSWARNMAXNUM		2000		//���Լ�λ�����Ĺ�Ʊ��Ϊ2000��
#define MAX_TJWARN				20			//����Ԥ���������Ϊ20	
#define	WARN_BUFFER_SIZE		2000		//Ԥ���������Ĵ�СΪ2000

#define MAX_CUSWEB_NUM			20
#define MAX_TJCOMB_NUM			30

#define TOTAL_DY_NUM			32
#define TOTAL_HY_NUM			52

#define WIDEDATA_HQ			0		//��ȫ��������
#define WIDEDATA_SORT		1		//��ȫ��������(������)
#define WIDEDATA_IMG_STAT	2		//��ȫͳ����������
#define WIDEDATA_TCK_STAT	3		//��ȫͳ����������
#define WIDEDATA_CWVIEW		4		//����͸������
#define WIDEDATA_ZNJP		5		//���ܽ�������
#define WIDEDATA_IWDATA_GP	6		//��Ѷ��������
#define WIDEDATA_IWDATA_HY	7		//��Ѷ��������
#define WIDEDATA_IWDATA_OB	8		//��Ѷ��������
#define WIDEDATA_IWDATA_FL	9		//��Ѷ��������
#define WIDEDATA_TA0		10		//TA�ֱ�ͳ��(����)
#define WIDEDATA_TA4		11		//TA�ֱ�ͳ��(4��ͳ��)
#define WIDEDATA_TA04		12		//ͬʱ�������4��ͳ�Ƶ�
//////////////////////////////////////////////////////////////////
//�Զ�����Ϣ��
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

#define UM_AUTOCONNECT				WM_USER+0x1410	//�Զ���������Ϣ
#define UM_READANDPAINT				WM_USER+0x1411	//��ComView������������
#define UM_MOUSELEAVE				WM_USER+0x1412
#define UM_AUTOCONNECTDS			WM_USER+0x1413	//�Զ�������չ������վ

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
#define UM_SELECTMANY				WM_USER+102		//ר��ָʾ
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
#define UM_RESTORE					WM_USER+116		//֪ͨFormView�ָ�
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
#define UM_NOTIFYRESULT				WM_USER+3027	//֪ͨѡ�ɽ������
#define UM_LOGOENBALE				WM_USER+3029	//֪ͨLogoDlg Enable
#define UM_BLOCKMODIFIED			WM_USER+3030	//�����б仯ʱ,֪ͨGridView
#define UM_SHOWGUIDE				WM_USER+3031	//��ʾGuide�Ի���
#define UM_SWITCHNEXTHOST			WM_USER+3032	//ͨ��LogoDlg �л���վ
#define UM_RESETFYSWITCHBAR			WM_USER+3033	//��λ����ͼ�е��л���
#define UM_WIDEDATA_SORT			WM_USER+3034	//ˢ����������
#define UM_WIDEDATA_HQSTAT			WM_USER+3035	//ˢ��ͳ������

#define UM_POPMSG					WM_USER+3040

#define UM_BK_START					WM_USER+3050	//��̨����->��ʼ
#define UM_BK_HQ_OK					WM_USER+3051	//��̨�������
#define UM_BK_SORTHQ_OK				WM_USER+3052	//��̨�������(�յ���������)
#define UM_BK_RMHQ_OK				WM_USER+3053	//���Ű������еĺ�̨�������(�յ���������)
#define UM_BK_IMGSTATHQ_OK			WM_USER+3054	//ͳ���������(�յ���������)
#define UM_BK_TCKSTATHQ_OK			WM_USER+3055	//ͳ���������(�յ���������)
#define UM_BK_SWICTH_OK				WM_USER+3056	//��̨����(�ӿڲ���)
#define UM_BK_GLHQ_OK				WM_USER+3057	//��������
#define UM_BK_INFODATA_OK			WM_USER+3058	//��̨��Ϣ������
#define UM_BK_WTBKDATA_OK			WM_USER+3059	//��̨��Ϣ������(ί��)
#define UM_BK_END					WM_USER+3100	//��̨����->����

#define UM_SHOW_PAD					WM_USER+5010	//��ʾPAD���ֶ������벻����������
#define UM_HIDE_PAD					WM_USER+5011	//��ȥPAD
#define UM_SWAP_MENU				WM_USER+5012	//�ڲ˵�״̬�»س�ʱ�˵����ڵ�ID�����仯
#define UM_CHANGE_MENU_COLOR		WM_USER+5013	//�ı�˵�����ɫ
#define UM_MENU_SETBLOCK			WM_USER+5014	//���ð��
#define UM_MENU_INITPOS				WM_USER+5015	//�ص���ʼ�Ĵ��ֲ˵�
#define UM_SEARCH_BLOCK				WM_USER+5016	//����ĳֻ��Ʊ���������
#define UM_RESETGRIDTAB				WM_USER+5017	//��������������ǩ
#define UM_SEARCH_GPGXD				WM_USER+5019	//����ĳָֻ���Ĺ��׶�
#define UM_SEARCH_HKSEAT			WM_USER+5020	//�����۹�ϯλ

#define UM_SPECIALZX_MSG			WM_USER+5030	//��Ѷ�����ر����Ѷ���ܵĴ���
#define UM_RELOAD_MSG				WM_USER+5031	//����װ�ص�֪ͨ��Ϣ
#define UM_TIMERMORE_REQ			WM_USER+5032

#define UM_MDITABSELCHANGED			WM_USER+5040
#define	UM_RSSDATETIMESORT			WM_USER+5041
//////////////////////////////////////////////////////////////////

#define ELAPSE_SECOND		5			//��ʱ������ˢ��
#define TICK2_SECOND		1			//��ʳɽ�������ˢ��
#define QUEUE_SECOND		1			//�������е�����ˢ��
#define MARKETINFO_SECOND	12			//�г���Ϣˢ��
#define DDEHQ_SECOND		40			//A+H��ͳ������ˢ��ʱ��
#define STATHQ_SECOND		20			//DDE��ͳ������ˢ��ʱ��
#define RMREPORT_SECOND		30			//���Ű�����ˢ��ʱ��

#define FOR_UNIT(i) for( i=0;i<m_Units.Num && m_Units.pUnit[i];i++)

#define MAX_FMLPACK_NUM		50
#define MAX_BANDSTOCK_NUM	200

#define	MAX		max
#define MIN		min

#define NOXS	0		//û��С��
#define ZS		1		//ָ��
#define XS2		2		//2λС��
#define	XS3		3		//3λС��
#define XS4		4		//4λС��
#define XS5		5		//5λС��

#define FZNUM 360
#define equfz(x,y) (fabs(x-y) < 0.00001)

#define  CLEAR_FXTOOL      100    //  �������
#define  DRAW_FXTOOL       101    //  ��ʾ����
#define  SAVE_FXTOOL       102    //  ���滭��
#define  CLEARONE_FXTOOL   103    //  �������
#define  LDXD_FXTOOL        0     //  �����߶�
#define  LDLX_FXTOOL        1     //  ��������
#define  PXGP_FXTOOL        2     //  ƽ�й�Ʊ��
#define  GOLD_FXTOOL        3     //  �ƽ�ָ�
#define  DFBX_FXTOOL        4     //  �ٷֱ���
#define  BDX_FXTOOL         5     //  ������
#define  ZQX_FXTOOL         6     //  ������
#define  FBLQ_FXTOOL        7     //  �Ѳ�����
#define  SSGS_FXTOOL        8     //  ��������
#define  XJGS_FXTOOL        9     //  �½�����
#define  ZSX_FXTOOL        10     //  ������
#define  XXHG_FXTOOL       11     //  ���λع���
#define  XXHGD_FXTOOL      12     //  ���λع��
#define  XXHGC_FXTOOL      13     //  ���λع��ӳ�

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

#define CLR_NOWTESTING		RGB(255,0,0)	//��ɫ
#define CLR_TESTED			RGB(0,0,255)	//��ɫ
#define CLR_INTERRUPT		RGB(132,0,0)	

//InfoDlg�ķ��
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

enum	//���⻭�߷�ʽ
{
	VAL_EQUVOL_LINE ,	//����ͼ
	VAL_EQUK_LINE,		//��������
	VAL_CAN_LINE,		//ѹ��ͼ
	VAL_OX_LINE,		//Ȧ��ͼ
	VAL_OXHL_LINE,		//�ߵ�Ȧ��ͼ
	VAL_TPO_LINE,		//�Ķȿռ�
	VAL_TBL_LINE,		//��������
	VAL_TOW_LINE		//������
};

////////////////////////////////////////////////////////////////////////
//TCPIPԶ�̺�

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

//��������
#define WT_SERTYPE		1
#define HQ_SERTYPE		2
#define CHAT_SERTYPE	3
#define TAPI_SERTYPE	4
#define UDP_SERTYPE		5

//���뷽ʽ
#define OLDFY_INMODE	0
#define DOS_INMODE		1
#define TDXW_INMODE		2
#define WEB_INMODE		3
#define PDA_INMODE		4
#define WIDE_INMODE		5
#define PLAY_INMODE		6

//������Ϣ�Ĵ������ͺ�
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

//�������Ĵ�С
#define MAX_SENDBUF_NUM		128

//����ļ�������
#define BUFFER_REQTYPE		0
#define HQ_REQTYPE			1
#define DIRECT_REQTYPE		2
#define RELAY_REQTYPE		3

//��������
#define CODE_NONE		-1
#define CODE_SZ			0
#define CODE_SH			1
#define CODE_SZSH		2

///////////////////////////////
// ֤ȯ����
#define CODE_SZAG       0	//A��
#define CODE_SZQZ       1	//Ȩ֤
#define CODE_SZGZ       2	//��ծ
#define CODE_SZZQ       3	//��ծ
#define CODE_SZKZHZQ    4	//תծ
#define CODE_SZGZHG     5	//�ع�
#define CODE_SZJJ       6	//����
#define CODE_SZBG       7	//B��
#define CODE_ZXB        8	//��С��ҵ
#define CODE_CYB		9	//��С��ҵ
#define CODE_SZOTHER    10	//����

#define CODE_SHAG      11	//A��
#define CODE_SHQZ      12	//Ȩ֤
#define CODE_SHGZ      13	//��ծ
#define CODE_SHZQ      14	//��ծ
#define CODE_SHKZHZQ   15	//תծ
#define CODE_SHGZHG    16	//�ع�
#define CODE_SHJJ      17	//����
#define CODE_SHBG      18	//B��
#define CODE_SHOTHER   19	//����

#define CODE_KFJJ	   20	//����ʽ����
#define CODE_SB		   21	//����

#define CODE_SZSPEC    22
#define CODE_SHSPEC    23

///////////////////////////////

#define JBM_TDX			-1

#define MAX_F10CAPTION	40

//���ֲ˵������
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
//��ʱ����
#define L_JSFX_ZST			3000
#define L_JSFX_TICK			3001
#define L_JSFX_FJB			3002
#define L_JSFX_FSB			3003
#define L_JSFX_MMP			3004
#define L_JSFX_LB			3005
#define L_JSFX_MMLD			3006
#define L_JSFX_TICKT		3007
//ָ��
#define L_INDEX_BASE		3100
//ί��
#define L_WT				4000
#define L_QUIT				4001
#define L_WEBINFO			4002
//��ɫ����
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

//ϵͳ����
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

#define L_XXM_BASE			4300	//������Ϣ

#define VAL_BJFX_BLOCK			0
#define VAL_MULGP_BLOCK			1

//BlockWin��MenuWin�Ļ���ߴ��
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

#define MAX_ZST_MANYDAY			10	//���֧��10�������ͼ

#define AUTOCONNECT_RESET		0
#define AUTOCONNECT_PENDING		1
#define AUTOCONNECT_RECONNECT	2

////////////////////////////////////////////////////

#define	MAX_GROUPMARK			7
#define MAX_INFO_TITLE			350

#define IGN_CJZX				0	//�ƾ���Ѷ
#define IGN_MINE				1	//��Ϣ����
#define IGN_FLZL				2	//����F10����
#define IGN_GGBW				3	//���ɱ���
#define IGN_OTHER				4	//����

#define GN_TREETYPE				0
#define GP_TREETYPE				1
#define ZB_TREETYPE				2
#define JB_TREETYPE				3
#define ZX_TREETYPE				4
#define WEB_TREETYPE			5
#define ZX2_TREETYPE			6
#define ZX3_TREETYPE			7

#define AREA_NORMAL_WEBPAGE		0	//һ�����ҳ
#define AREA_UID_PWD_WEBPAGE	1	//�Զ������ҳ
#define AREA_CHATROOM			2	//������
#define AREA_BBS				3	//Ͷ����̳
#define AREA_MINE				4	//��Ϣ����
#define AREA_FLZL				5	//��������
#define AREA_GGBW				6	//���ɱ���
#define AREA_KEXPLAIN			7	//���ܽ���
#define AREA_USERINFO			8	//�û���Ϣ
#define AREA_WT					9	//ί��ģ��

//WEB��ҳ����������
#define EDIT_INPUT_TYPE			0
#define SELECT_INPUT_TYPE		1
#define FORM_SUBMIT_TYPE		2
#define BTN_SUBMIT_TYPE			3

//���м��Ԥ��������
enum
{
	WARN_RADAR,
	WARN_TJ
};

#define MAX_FY_GROUP			100
#define MAX_FYNUM_IN_GROUP		18

////////////////////////////////////////////////////

//��ȫ��ʽ
#define TDX_SECURE			0		//ͨ���ż���
#define AUTHPASS_SECURE		1		//��֤����
#define CA_SECURE			2		//��ȫ֤��

//�Զ����ӵķ�ʽ
#define AUTO_OVERLAP_IDX	0x0001	//ָ��
#define AUTO_OVERLAP_MG		0x0002	//����

#define MAX_HISCACHE		1000

///////////////////////////////////////////////////

const	MAX_MKT_NUM			=	64;
const	MAX_MKTGUI_NUM		=	16;
const	DS_FROM				=	30000;
const	MAX_DS_STKNUM		=	20000;
const	MAX_DS_TIMERNUM		=	200;

//���̾������
const	KEYGUY_FUND			=	100000;
const	KEYGUY_DSCODE		=	65000;
const	KEYGUY_SEPCZB		=	62000;
const	KEYGUY_BLOCK		=	60000;
const	KEYGUY_MAINLINE		=	59800;
const	KEYGUY_DSTYPE		=	59000;
const	KEYGUY_ZB			=	51000;
const	KEYGUY_OPER			=	50000;
const	KEYGUY_OLDNAME		=	130000;

//���˰�����
const	PERSONAL_QD			=   1;
const	PERSONAL_BL			=   2;
const	PERSONAL_YG			=	3;	//���Ϲ���
const	PERSONAL_JJTZ		=	4;	//����Ͷ��
const	PERSONAL_JJTZEX		=	5;	//����Ͷ����ǿ
const	PERSONAL_JJDX		=	6;	//����Ͷ�ʶ��߾����
const	PERSONAL_JJVIP		=	7;	//������Ӯ���VIP��
//ͨ���ż���INI�ļ�
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

//ͨ�����
#define URGENT_INIT			0	
#define URGENT_DURING		1
//
#define URGENT_NORMAL		0
#define URGENT_USER			1
#define URGENT_BG			2
#define URGENT_TDXFREE		3

//�Զ�̽�������
#define DETECT_SECOND		20
#define DS_DETECTCODE_MIN	5

//�����������
#define WARN_NONE			0
#define WARN_LINE			1				 	//��������(A,B,C:A+C=B+B,A-B=D*10^N,abs(D)<10)
#define WARN_ORDERSHOCK		2					//ί���춯(��ί��)
#define WARN_LARGEVOL		3					//������
#define	WARN_QUICKRISE		4					//��������
#define	WARN_QUICKFALL		5					//�����µ�
#define	WARN_REBOUND		6					//���ٷ���
#define	WARN_QUICKDIVE		7					//��λ��ˮ
#define	WARN_PRICEUPSHOCK	8					//�۸��ϳ�(�ѳɽ�15����,���3�����Ƿ���5%,֮ǰ���������2%)
#define	WARN_PRICEDNSHOCK	9					//�۸�����(�ѳɽ�15����,���3���ӵ�����5%,֮ǰ���������2%)
#define	WARN_TICKSHOCK		10					//�����춯(�۸��춯)
#define	WARN_VOLUMESHOCK	11					//�ɽ��춯(�ѳɽ�15����,���5���ӷ��Ӿ���Ϊȫ��ľ�����2-3��)

//��������Ԥ��
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

//��¼������
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
