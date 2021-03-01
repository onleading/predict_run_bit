#include "stdafx.h"
#include "ViewManager.h"
#include "Znxg.h"
#include "interface.h"
#include "custompad.h"

HINSTANCE g_resModule=NULL;

//!!!
char	g_szReleaseVersion[10]="7.3";
char	g_szMainExeVersion[10]="7.3";
char	g_szDSExeVersion[10]="1.02";

UINT	UM_MINEMSG	= RegisterWindowMessage("SendMineMessage");
UINT	UM_GGBWMSG	= RegisterWindowMessage("ReReadGGBWMessage");
UINT	UM_FLZLMSG  = RegisterWindowMessage("ReqFLZLMessage");
UINT    UM_HQ_MINE_MSG	= RegisterWindowMessage("HqMineInfoMesssage");
UINT	UM_CJZXMSG	= RegisterWindowMessage("SendCjzxMessage");

const TCHAR szMainFrameClass[] = _T("TopView_MainFrame_Class");

class CHQComm	*g_pComm = NULL,*g_pComm3 = NULL;
class CGetData	*g_pGetData = NULL;

int		RECENTNUM	= 140;
int		g_RMSp		= 0;

int		g_nLinkage	= 0;			//0:������ 1:�������� 2:�������

BOOL	g_DDEDataNeedLoad = TRUE;	//�Ƿ���Ҫ��������Exday(��վ���ӳɹ�,�������ݺ���Ҫ��������)

BOOL	OnLine = FALSE;				//��ͨ����Internet���߱�־
BOOL	DSOnLine = FALSE;
BOOL	OffLineMode = FALSE;	//�Ƿ����ѻ�״̬

BOOL  g_bAllWarnSwitch = FALSE;	//Ԥ������
BOOL  g_bTjWarnSwitch = FALSE;	//����Ԥ������
BOOL  g_bNotifyDownload = TRUE;	//�˳�ϵͳ���Ƿ���ʾ��������
BOOL  g_bDay_WriteDisk = FALSE;	//�����Զ�д��

BOOL  g_bDrawBlue = TRUE;

//!!!
long	g_nDefUserLevel = 1;
CString g_strPreName = "";//��Ʒ����ǰ׺
CString g_strTitleName = "";//��Ʒ����
CString g_strAdName = "";
CString g_strMoving = "";

//���ݰ汾��Ӧ�������ѡ���ʵ���ֵ
BOOL  g_bInsideOpenPage = FALSE;//�ڳ����ڲ����������
BOOL  g_bExtern_Info = FALSE;	//�ǲ��������Ѷ
BOOL  g_bExtern_Invest = FALSE;	//�ǲ���������ϵͳ
short g_nSwitchMainID = 0;		//����Unit�õ�ComView��HisDlg����ID

char  g_strMovingZXG[MAXGPNUM][SH_CODE_LEN+10];
short g_nMovingZXG = 0;

BOOL  LayoutFlag = FALSE;
int	  g_nMinnNum = 10,g_nDaynNum = 45;
NTime g_NoTime;

struct AnalyData g_SyncAnalyData;
float	g_SyncClose = 0.0;

int	  g_nGzPerMin = 120;

int	  g_nWhichHisSwitch = 0;
BOOL  g_bVolTQ = FALSE;
BYTE   TMEANLESS_DATA[8]={0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8};

DTopViewCalc	* g_pCalc = NULL, * g_pExtCalc = NULL;

UINT  g_WTMsgID,g_WTMsgID2;
UINT  g_uRefreshAllHQ;
UINT  g_uCloseMsg;
BOOL  g_bSpecClose = FALSE;	//�ǲ������ⲿ���������
BOOL  g_bDirectQuit = TRUE;
BOOL  g_bReLoginStatus = FALSE;

long   g_lOpenRq = -1;

BOOL   g_bDonotClose = FALSE;
CZnxg *g_pZnxg = NULL;
CLESIGN g_pXGSign;

//�������Ű�������ESC�߼�
BOOL	g_bSepEsc = FALSE;

BOOL	g_bOldGridDomain=TRUE;
//��������
int		 g_nZFFzNum = 5;//ȫ��������������

//
//����������ַ
CString  g_NetCardAddress = "";
//���ڰ���
BlockInfo_Struct g_BlockInfo[USERBLOCK_NUM]={0};
int		g_nBlockNum = 0;
//���ڸ������
MEM_BLOCK g_ConceptBlock[100];
int		  g_ConceptBlockNum = 0;
//������ϰ���
ZHB_BlockInfo_Struct *g_ZHBBlock = NULL;
int					 g_ZHBBlockNum = 0;

//������ɫ
char		g_ColorBlockGp[8][MAXGPNUM][SH_CODE_LEN+10];
int			g_ColorBlockGpNum[8];
BLOCKCOLOR	g_BlockColor[8];

KeyWordSeach* g_pKeyWordSearch=NULL;

short	GPFZNUM;
short	QHFZNUM;
short	fztb[4]={1,3,6,12};

int		ScreenCx=800,ScreenCy=600;

char *DpGpCode[19]={"000010",SH_DPCODE/*��֤����*/,SH_ADPCODE,SH_BDPCODE,SH_DPCODE/*ADL*/,SH_DPCODE/*DK*/,"399004",
					SZ_DPCODE/*��������*/,"399106","399107","399108","399106","399106","000016","000011",CYB_DPCODE,"399101","000015",HS300_SZZS};

class StockDataIo	*g_pStockIO	= NULL;
class LcIdxMng		*g_pLcIdxMng= NULL;
class ExdayMng		*g_pExdayMng= NULL;
class ConstMng		*g_pConstMng= NULL;

CFGINFO   g_TdxCfg;
short	  g_WSXH=2;
CString	  g_WSXHStr;
CTime	  g_tNowTime;
long	  g_iToday=0;
int       g_bYesterdayFlag = 0;

short anFZRatio[10]={5,15,30,60,1,1,1,1,10,10}; // <- anFzRatio[7]={1,3,6,12,1,1,1}
short sz[8]={570,690,780,900,900,900,900,900};
short sh[8]={570,690,780,900,900,900,900,900};
char  lpszTimeStr[12][6] = {"SZQSK",  //����ǰ�п���ʱ��
			          "SZQSS",  //����ǰ������ʱ��
			          "SZHSK",  //���к��п���ʱ��
			          "SZHSS",  //���к�������ʱ��
					  "SZYSK",
					  "SZYSS",
			          "SHQSK",  //����ǰ�п���ʱ��
                      "SHQSS",  //����ǰ������ʱ��
                      "SHHSK",  //���к��п���ʱ��
                      "SHHSS",  //���к�������ʱ��
					  "SHYSK",
					  "SHYSS"};

char	g_DefCusPadName[64] = "";
vector<DynaCmdTable> g_vDynaCmdTab;
BOOL	g_bFirstCusPad = FALSE;

struct CmdTable g_pCmdTable[]=
{
	{"0       ���ܲ˵�",		"0",		CMD_MAIN,0},

	{".5      ��ʱ����ͼ",		".5",		CMD_COM,501},
	{".501    ��ʱ����ͼ",		".501",		CMD_COM,501},
	{"FST     ��ʱ����ͼ",		"FST",		CMD_COM,501},
	{"ZST     ��ʱ����ͼ",		"ZST",		CMD_COM,501},

	{".502    ��ʱ�ɽ���ϸ",	".502",		CMD_COM,502},
	{"01      ��ʱ�ɽ���ϸ",	"01",		CMD_COM,502},
	{"TICK    ��ʱ�ɽ���ϸ",	"TICK",		CMD_COM,502},

	{".503    �ּ۱�",			".503",		CMD_COM,503},
	{"02      �ּ۱�",			 "02",		CMD_COM,503},
	{"FJB     �ּ۱�",			"FJB",		CMD_COM,503},

	{".504    ��ʳɽ���ϸ",   ".504",		CMD_COM,504},

	{".506    ����(���)ָ��",  ".506",		CMD_COM,506},
	{"LBZB    ����(���)ָ��",  "LBZB",		CMD_COM,506},

	{".507    ��������(�ǵ���)",".507",		CMD_COM,507},
	{"MMLD    ��������(�ǵ���)","MMLD",		CMD_COM,507},

	{"08      �л�����",		"08",		CMD_COM,8},
	{"M1      1������",			"M1",		CMD_COM,801},
	{"M5      5������",			"M5",		CMD_COM,802},
	{"M15     15������",		"M15",		CMD_COM,803},
	{"M30     30������",		"M30",		CMD_COM,804},
	{"M60     60������",		"M60",		CMD_COM,805},
	{"MN      N������",			"MN",		CMD_COM,806},
	{"DAY     ����",			"DAY",		CMD_COM,807},
	{"WEEK    ����",			"WEEK",		CMD_COM,808},
	{"MONTH   ����",			"MONTH",	CMD_COM,809},
	{"DAYN    N����",			"DAYN",		CMD_COM,810},
	{"SEASON  ����",			"SEASON",	CMD_COM,811},
	{"YEAR    ����",			"YEAR",		CMD_COM,812},

	{".101    ��֤180����   ",	".101",		CMD_COM,101},
	{".102    ��֤��ָ���� ",	".102",		CMD_COM,102},
	{"03      ��֤����ָ�� ",	"03",		CMD_COM,102},
	{".103    ��֤A������  ",	".103",		CMD_COM,103},
	{".104    ��֤B������  ",	".104",		CMD_COM,104},
	{".105    ��֤ADLָ��  ",	".105",		CMD_COM,105},
	{".106    ��֤���ָ�� ",	".106",		CMD_COM,106},

	{".107    ��֤100���� ",	".107",		CMD_COM,107},
	{".108    ��֤�ɷ����� ",	".108",		CMD_COM,108},
	{"04      ��֤����ָ�� ",	"04",		CMD_COM,108},
	{".109    ��֤��ָ���� ",	".109",		CMD_COM,109},
	{".110    ��֤A������  ",	".110",		CMD_COM,110},
	{".111    ��֤B������  ",	".111",		CMD_COM,111},
	{".112    ��֤ADLָ��  ",	".112",		CMD_COM,112},
	{".113    ��֤���ָ�� ",	".113",		CMD_COM,113},

	{".114    ��֤50���� ",		".114",		CMD_COM,114},
	{".115    ��֤����ָ��  ",	".115",		CMD_COM,115},
	{".116    ��ҵ��ָ�� ",	".116",		CMD_COM,116},
	{".117    ��С��ҵָ��  ",	".117",		CMD_COM,117},
	{".118    ��֤����ָ��  ",	".118",		CMD_COM,118},
	{".119    ����300����  ",	".119",		CMD_COM,119},

	{".201    ����������ʾ ",	".201",		CMD_HQ,201},
	{"HQT     ����������ʾ ",	"HQT",		CMD_HQ,201},

	{".202    �������",		".202",		CMD_HQ,202},
	{".203    ����ǿ�ƹ�",		".203",		CMD_HQ,203},
	{".204    �������ƹ�",		".204",		CMD_HQ,204},
	{".205    �ɽ�������",		".205",		CMD_HQ,205},
	{".206    �ܽ������",		".206",		CMD_HQ,206},
	{".207    ��������",		".207",		CMD_HQ,207},
	{".208    ί������",		".208",		CMD_HQ,208},
	{".209    �ּ�����",		".209",		CMD_HQ,209},
	{".210    ���´������",	".210",		CMD_HQ,210},
	{".211    ��ӯ������",		".211",		CMD_HQ,211},
  	{".212    ����������",		".212",		CMD_HQ,212},
	{".213    ���ʲ�����",		".213",		CMD_HQ,213},
	{".214    ���ʲ�����",		".214",		CMD_HQ,214},
	{".215    ÿ����������",	".215",		CMD_HQ,215},
	{".216    ���ʲ�����������",".216",		CMD_HQ,216},
	{".217    ��ͨ��ֵ����",	".217",		CMD_HQ,217},
	{".218    ��ͨ�ɱ�����",	".218",		CMD_HQ,218},
	{".219    AB������ֵ����",  ".219",		CMD_HQ,219},
	{".220    �ܹɱ�����",		".220",		CMD_HQ,220},
	{".221    ÿ�ɹ���������",  ".221",		CMD_HQ,221},
	{".222    ��Ծ������",		".222",		CMD_HQ,222},
	{".223    ÿ�ʾ�������",	".223",		CMD_HQ,223},
	{".224    ÿ�ʻ�������",	".224",		CMD_HQ,224},
	
	{".225    ÿ�ɾ��ʲ�����",	".225",		CMD_HQ,225},
	{".226    �о�������",		".226",		CMD_HQ,226},

	{".300    ��������", 		".300",		CMD_COM,300},
	{"FXT     ��������",		"FXT",		CMD_COM,300},

	{"�ͣ��ã�����ָ��",		".301",		CMD_COM,301},
	{"�ģͣ�����ָ��",			".302",		CMD_COM,302},
	{"�ģͣ�����ָ��",			".303",		CMD_COM,303},
	{"�ţأУͣ�����ָ��",		".304",		CMD_COM,304},
	{"�ԣңɣ�����ָ��",		".305",		CMD_COM,305},
	{"�£ң�������ָ��",		".306",		CMD_COM,306},
	{"�ã�����ָ��",			".307",		CMD_COM,307},
	{"�֣�����ָ��",			".308",		CMD_COM,308},
	{"�ϣ£�����ָ��",			".309",		CMD_COM,309},
	{"���ӣ�����ָ��",			".310",		CMD_COM,310},
	{"�ţͣ�����ָ��",			".311",		CMD_COM,311},
	{"�ף֣�������ָ��",		".312",		CMD_COM,312},
	{"�ңӣ�ǿ��ָ��",			".313",		CMD_COM,313},
	{"�ף���ǿ��ָ��",			".314",		CMD_COM,314},
	{"�ӣ���ͣ��ָ��",			".315",		CMD_COM,315},
	{"�ˣģʳ�����",			".316",		CMD_COM,316},
	{"�ããɳ�����",			".317",		CMD_COM,317},
	{"�ңϣó�����",			".318",		CMD_COM,318},
	{"�ͣɣˣ�ѹ��֧��",		".319",		CMD_COM,319},
	{"�£ϣ̣�ѹ��֧��",		".320",		CMD_COM,320},
	{"�Уӣ�������",			".321",		CMD_COM,321},
	{"�ãģ����Ʋ���ϵͳ",		".322",		CMD_COM,322},

	{"05      �����л�",		"05",		CMD_MAIN,5},

	{"1       ��֤����",		"1",		CMD_HQ,1},
	{"2       ��֤�¹�",		"2",		CMD_HQ,2},
	{"3       ��֤����",		"3",		CMD_HQ,3},
	{"4       ��֤�¹�",		"4",		CMD_HQ,4},
	{"5       ��֤ծȯ",		"5",		CMD_HQ,5},
	{"6       ��֤ծȯ",		"6",		CMD_HQ,6},
	{"7       �����",		"7",		CMD_HQ,7},
	{"8       ��¹�",		"8",		CMD_HQ,8},
	{"9       ��С��ҵ",		"9",		CMD_HQ,13},
	{"11      �Ȩ֤",		"11",		CMD_HQ,14},
	{"12      ��ҵ��",			"12",		CMD_HQ,15},
	{"06      ��ѡ��",			"06",		CMD_HQ,16},
	
	{"10      ��������",		"10",		CMD_JBM,801},


	{"180     ��֤180����   ",	"180",		CMD_COM,101},
	{"100     ��֤100����   ",	"100",		CMD_COM,107},
	{"300     ����300����   ",	"300",		CMD_COM,119},

	{"61      �����Ƿ�����",	"61",		CMD_HQ,61},
	{"62      �����Ƿ�����",	"62",		CMD_HQ,62},
	{"63      ����Ƿ�����",	"63",		CMD_HQ,63},
	{"64      ����Ƿ�����",	"64",		CMD_HQ,64},
	{"65      ��ծ�Ƿ�����",	"65",		CMD_HQ,65},
	{"66      ��ծ�Ƿ�����",	"66",		CMD_HQ,66},
	{"67      ����Ƿ�����",	"67",		CMD_HQ,67},
	{"68      ����Ƿ�����",	"68",		CMD_HQ,68},
	{"69      ��С��ҵ�Ƿ�����","69",		CMD_HQ,69},
	{"611     �Ȩ֤�Ƿ�����","611",		CMD_HQ,611},
	{"612     ��ҵ���Ƿ�����",  "612",		CMD_HQ,612},

	{"613     �л��Ƿ�����",	"613",		CMD_HQ,613},

	{"81      �����ۺ�����",	"81",		CMD_MAIN,81},
	{"82      �����ۺ�����",	"82",		CMD_MAIN,82},
	{"83      ����ۺ�����",	"83",		CMD_MAIN,83},
	{"84      ����ۺ�����",	"84",		CMD_MAIN,84},
	{"85      ��ծ�ۺ�����",	"85",		CMD_MAIN,85},
	{"86      ��ծ�ۺ�����",	"86",		CMD_MAIN,86},
	{"87      ����ۺ�����",	"87",		CMD_MAIN,87},
	{"88      ����ۺ�����",	"88",		CMD_MAIN,88},
	{"89      ��С��ҵ�ۺ�����","89",		CMD_MAIN,89},
	{"811     �Ȩ֤�ۺ�����","811",		CMD_MAIN,811},
	{"812     ��ҵ���ۺ�����"  ,"812",		CMD_MAIN,812},

	{"9.      1������",			"9.",		CMD_COM,801},
	{"96      5������",			"96",		CMD_COM,802},
	{"97      15������",		"97",		CMD_COM,803},
	{"98      30������",		"98",		CMD_COM,804},
	{"99      60������",		"99",		CMD_COM,805},
	{"91      ����",			"91",		CMD_COM,807},
	{"92      ����",			"92",		CMD_COM,808},
	{"93      ����",			"93",		CMD_COM,809},
	{"90      N����",			"90",		CMD_COM,810},
	{"910     N������",			"910",		CMD_COM,806},
	{"94	  ����",			"94",		CMD_COM,811},
	{"95      ����",			"95",		CMD_COM,812},

	{".902    ����ѡ�ɲ���",	".902",		CMD_MAIN,902},
	{".903    ����ϵͳ����",	".903",		CMD_MAIN,903},
	{".904    ̽�����ָ��",	".904",		CMD_MAIN,904},

	{".905    ����ѡ��",		".905",		CMD_MAIN,905},
	{".906    ����ѡ��",		".906",		CMD_MAIN,906},
	{".909    �ۺ�ѡ��",		".909",		CMD_MAIN,909},

	{".910    �г��״�����",	".910",		CMD_MAIN,910},
	{".911    �г��״��б�",	".911",		CMD_MAIN,911},
	{".918    ����Ԥ������",	".918",		CMD_MAIN,918},
	{".919    ����Ԥ���б�",	".919",		CMD_MAIN,919},

	{".913    ��Ʊ��ϼ���",	".913",		CMD_MAIN,913},

	{".930    ϵͳ����",		".930",		CMD_MAIN,930},
	{".931    �����û����",	".931",		CMD_MAIN,931},
	{".933    �̺���������",	".933",		CMD_MAIN,933},
	{".936    ����˵����",		".936",		CMD_MAIN,936},

    {"",0,0,0}  //CodeΪ0��ʾ����
};

/*
struct CmdTable g_pCmdTable[]=
{
	{"0       ���ܲ˵�",		"0",		CMD_MAIN,0},

	{".8      ����ί��",		".8",		CMD_MAIN,1000},

	{".5      ��ʱ����ͼ",		".5",		CMD_COM,501},
	{".501    ��ʱ����ͼ",		".501",		CMD_COM,501},
	{"FST     ��ʱ����ͼ",		"FST",		CMD_COM,501},
	{"ZST     ��ʱ����ͼ",		"ZST",		CMD_COM,501},

	{".502    ��ʱ�ɽ���ϸ",	".502",		CMD_COM,502},
	{"01      ��ʱ�ɽ���ϸ",	"01",		CMD_COM,502},
	{"TICK    ��ʱ�ɽ���ϸ",	"TICK",		CMD_COM,502},

	{".503    �ּ۱�",			".503",		CMD_COM,503},
	{"02      �ּ۱�",			 "02",		CMD_COM,503},
	{"FJB     �ּ۱�",			"FJB",		CMD_COM,503},

	{".504    ��ʳɽ���ϸ",   ".504",		CMD_COM,504},

	{".505    ��ϸ������",		".505",		CMD_COM,505},
	{"MMP     ��ϸ������",		"MMP",		CMD_COM,505},

	{".506    ����(���)ָ��",  ".506",		CMD_COM,506},
	{"LBZB    ����(���)ָ��",  "LBZB",		CMD_COM,506},

	{".507    ��������(�ǵ���)",".507",		CMD_COM,507},
	{"MMLD    ��������(�ǵ���)","MMLD",		CMD_COM,507},
	{".508    ��������ͼ",		".508",		CMD_COM,508},

	{"08      �л�����",		"08",		CMD_COM,8},
	{"M1      1������",			"M1",		CMD_COM,801},
	{"M5      5������",			"M5",		CMD_COM,802},
	{"M15     15������",		"M15",		CMD_COM,803},
	{"M30     30������",		"M30",		CMD_COM,804},
	{"M60     60������",		"M60",		CMD_COM,805},
	{"MN      N������",			"MN",		CMD_COM,806},
	{"DAY     ����",			"DAY",		CMD_COM,807},
	{"WEEK    ����",			"WEEK",		CMD_COM,808},
	{"MONTH   ����",			"MONTH",	CMD_COM,809},
	{"DAYN    N����",			"DAYN",		CMD_COM,810},
	{"SEASON  ����",			"SEASON",	CMD_COM,811},
	{"YEAR    ����",			"YEAR",		CMD_COM,812},

	{".101    ��֤180����   ",	".101",		CMD_COM,101},
	{".102    ��֤��ָ���� ",	".102",		CMD_COM,102},
	{"03      ��֤����ָ�� ",	"03",		CMD_COM,102},
	{".103    ��֤A������  ",	".103",		CMD_COM,103},
	{".104    ��֤B������  ",	".104",		CMD_COM,104},
	{".105    ��֤ADLָ��  ",	".105",		CMD_COM,105},
	{".106    ��֤���ָ�� ",	".106",		CMD_COM,106},

	{".107    ��֤100���� ",	".107",		CMD_COM,107},
	{".108    ��֤�ɷ����� ",	".108",		CMD_COM,108},
	{"04      ��֤����ָ�� ",	"04",		CMD_COM,108},
	{".109    ��֤��ָ���� ",	".109",		CMD_COM,109},
	{".110    ��֤A������  ",	".110",		CMD_COM,110},
	{".111    ��֤B������  ",	".111",		CMD_COM,111},
	{".112    ��֤ADLָ��  ",	".112",		CMD_COM,112},
	{".113    ��֤���ָ�� ",	".113",		CMD_COM,113},

	{".114    ��֤50���� ",		".114",		CMD_COM,114},
	{".115    ��֤����ָ��  ",	".115",		CMD_COM,115},
	{".116    ��ҵ��ָ�� ",	".116",		CMD_COM,116},
	{".117    ��С��ҵָ��  ",	".117",		CMD_COM,117},
	{".118    ��֤����ָ��  ",	".118",		CMD_COM,118},
	{".119    ����300����  ",	".119",		CMD_COM,119},

	{".201    ����������ʾ ",	".201",		CMD_HQ,201},
	{"HQT     ����������ʾ ",	"HQT",		CMD_HQ,201},

	{".202    �������",		".202",		CMD_HQ,202},
	{".203    ����ǿ�ƹ�",		".203",		CMD_HQ,203},
	{".204    �������ƹ�",		".204",		CMD_HQ,204},
	{".205    �ɽ�������",		".205",		CMD_HQ,205},
	{".206    �ܽ������",		".206",		CMD_HQ,206},
	{".207    ��������",		".207",		CMD_HQ,207},
	{".208    ί������",		".208",		CMD_HQ,208},
	{".209    �ּ�����",		".209",		CMD_HQ,209},
	{".210    ���´������",	".210",		CMD_HQ,210},
	{".211    ��ӯ������",		".211",		CMD_HQ,211},
  	{".212    ����������",		".212",		CMD_HQ,212},
	{".213    ���ʲ�����",		".213",		CMD_HQ,213},
	{".214    ���ʲ�����",		".214",		CMD_HQ,214},
	{".215    ÿ����������",	".215",		CMD_HQ,215},
	{".216    ���ʲ�����������",".216",		CMD_HQ,216},
	{".217    ��ͨ��ֵ����",	".217",		CMD_HQ,217},
	{".218    ��ͨ�ɱ�����",	".218",		CMD_HQ,218},
	{".219    AB������ֵ����",  ".219",		CMD_HQ,219},
	{".220    �ܹɱ�����",		".220",		CMD_HQ,220},
	{".221    ÿ�ɹ���������",  ".221",		CMD_HQ,221},
	{".222    ��Ծ������",		".222",		CMD_HQ,222},
	{".223    ÿ�ʾ�������",	".223",		CMD_HQ,223},
	{".224    ÿ�ʻ�������",	".224",		CMD_HQ,224},
	
	{".225    ÿ�ɾ��ʲ�����",	".225",		CMD_HQ,225},
	{".226    �о�������",		".226",		CMD_HQ,226},

	{".400    ���Ű�����",	".400",		CMD_HIS,400},
	{".401    ��ʷ���鱨��",	".401",		CMD_HIS,401},
	{".402    ǿ����������",	".402",		CMD_HIS,402},
	{".403    �����ǵ�����",	".403",		CMD_HIS,403},
	{".404    ���任������",	".404",		CMD_HIS,404},
	{".405    �����������",	".405",		CMD_HIS,405},
	{".406    �����񵴷���",	".406",		CMD_HIS,406},

	{".300    ��������", 		".300",		CMD_COM,300},
	{"FXT     ��������",		"FXT",		CMD_COM,300},

	{"�ͣ��ã�����ָ��",		".301",		CMD_COM,301},
	{"�ģͣ�����ָ��",			".302",		CMD_COM,302},
	{"�ģͣ�����ָ��",			".303",		CMD_COM,303},
	{"�ţأУͣ�����ָ��",		".304",		CMD_COM,304},
	{"�ԣңɣ�����ָ��",		".305",		CMD_COM,305},
	{"�£ң�������ָ��",		".306",		CMD_COM,306},
	{"�ã�����ָ��",			".307",		CMD_COM,307},
	{"�֣�����ָ��",			".308",		CMD_COM,308},
	{"�ϣ£�����ָ��",			".309",		CMD_COM,309},
	{"���ӣ�����ָ��",			".310",		CMD_COM,310},
	{"�ţͣ�����ָ��",			".311",		CMD_COM,311},
	{"�ף֣�������ָ��",		".312",		CMD_COM,312},
	{"�ңӣ�ǿ��ָ��",			".313",		CMD_COM,313},
	{"�ף���ǿ��ָ��",			".314",		CMD_COM,314},
	{"�ӣ���ͣ��ָ��",			".315",		CMD_COM,315},
	{"�ˣģʳ�����",			".316",		CMD_COM,316},
	{"�ããɳ�����",			".317",		CMD_COM,317},
	{"�ңϣó�����",			".318",		CMD_COM,318},
	{"�ͣɣˣ�ѹ��֧��",		".319",		CMD_COM,319},
	{"�£ϣ̣�ѹ��֧��",		".320",		CMD_COM,320},
	{"�Уӣ�������",			".321",		CMD_COM,321},
	{"�ãģ����Ʋ���ϵͳ",		".322",		CMD_COM,322},

	{"05      �����л�",		"05",		CMD_MAIN,5},

	{"1       ��֤����",		"1",		CMD_HQ,1},
	{"2       ��֤�¹�",		"2",		CMD_HQ,2},
	{"3       ��֤����",		"3",		CMD_HQ,3},
	{"4       ��֤�¹�",		"4",		CMD_HQ,4},
	{"5       ��֤ծȯ",		"5",		CMD_HQ,5},
	{"6       ��֤ծȯ",		"6",		CMD_HQ,6},
	{"7       �����",		"7",		CMD_HQ,7},
	{"8       ��¹�",		"8",		CMD_HQ,8},
	{"9       ��С��ҵ",		"9",		CMD_HQ,13},
	{"11      �Ȩ֤",		"11",		CMD_HQ,14},
	{"12      ��ҵ��",			"12",		CMD_HQ,15},
	{"06      ��ѡ��",			"06",		CMD_HQ,15},
	
	{"10      ��������",		"10",		CMD_JBM,801},
	{"TDXJBZL ȨϢ����",		"TDXJBZL",	CMD_JBM,802},

	{"16      ��Ϣ����",		"16",		CMD_COM,16},
	{"17      Ͷ���ռ�",		"17",		CMD_COM,17},
	{"18      ������������",	"18",		CMD_MAIN,916},
#if defined(OEM_NEWJY) || defined(LEVEL2)
	{"19      �о�����ƽ̨",	"19",		CMD_MAIN,1080},
#endif
#ifdef OEM_NEWJY 
	{"20      ����ί��",		"20",		CMD_MAIN,1000},
	{"21      ��������",		"21",		CMD_MAIN,1001},
	{"22      ������ѯ",		"22",		CMD_MAIN,1002},	
	{"23      ��������",		"23",		CMD_MAIN,1003},
	{"25      ������",			"25",		CMD_MAIN,1005},
	{"221     ��ͨ����",		"221",		CMD_MAIN,1006},
	{"223     ��ͨ����",		"223",		CMD_MAIN,1007},

	{".+1     ��һ������",		".+1",		CMD_MAIN,1011},
	{".+2     ����������",		".+2",		CMD_MAIN,1012},
	{".+3     ����������",		".+3",		CMD_MAIN,1013},
	{".+4     ���ļ�����",		".+4",		CMD_MAIN,1014},
	{".+5     ���������",		".+5",		CMD_MAIN,1015},

	{".+01    ��һ������",		".+01",		CMD_MAIN,1016},
	{".+02    ���������",		".+02",		CMD_MAIN,1017},
	{".+03    ����������",		".+03",		CMD_MAIN,1018},
	{".+04    ���ļ�����",		".+04",		CMD_MAIN,1019},
	{".+05    ���������",		".+05",		CMD_MAIN,1020},

	{".-1     ��һ������",		".-1",		CMD_MAIN,1021},
	{".-2     ���������",		".-2",		CMD_MAIN,1022},
	{".-3     ����������",		".-3",		CMD_MAIN,1023},
	{".-4     ���ļ�����",		".-4",		CMD_MAIN,1024},
	{".-5     ���������",		".-5",		CMD_MAIN,1025},

	{".-01    ��һ������",		".-01",		CMD_MAIN,1026},
	{".-02    ����������",		".-02",		CMD_MAIN,1027},
	{".-03    ����������",		".-03",		CMD_MAIN,1028},
	{".-04    ���ļ�����",		".-04",		CMD_MAIN,1029},
	{".-05    ���������",		".-05",		CMD_MAIN,1030},
#endif
	{"30      �л�����Ʒ��",	"30",		CMD_COM,30},

	{"31      ��ƽ�����",		"31",		CMD_MAIN,912},
	{"32      �������",		"32",		CMD_MAIN,915},
	{"33      ��������",		"33",		CMD_MAIN,917},
	{"34      ���ݵ���",		"34",		CMD_MAIN,920},
	{"35      ��������",		"35",		CMD_MAIN,921},
	{"36      �زֳֹɻ���",	"36",		CMD_MAIN,922},
	{"37      �������ƽ̨",	"37",		CMD_MAIN,923},
	{"38      ���������",		"38",		CMD_MAIN,924},
	{"39      ���������л���",	"39",		CMD_MAIN,925},

	{"180     ��֤180����   ",	"180",		CMD_COM,101},
	{"100     ��֤100����   ",	"100",		CMD_COM,107},
	{"300     ����300����   ",	"300",		CMD_COM,119},

	{"61      �����Ƿ�����",	"61",		CMD_HQ,61},
	{"62      �����Ƿ�����",	"62",		CMD_HQ,62},
	{"63      ����Ƿ�����",	"63",		CMD_HQ,63},
	{"64      ����Ƿ�����",	"64",		CMD_HQ,64},
	{"65      ��ծ�Ƿ�����",	"65",		CMD_HQ,65},
	{"66      ��ծ�Ƿ�����",	"66",		CMD_HQ,66},
	{"67      ����Ƿ�����",	"67",		CMD_HQ,67},
	{"68      ����Ƿ�����",	"68",		CMD_HQ,68},
	{"69      ��С��ҵ�Ƿ�����","69",		CMD_HQ,69},
	{"611     �Ȩ֤�Ƿ�����","611",		CMD_HQ,611},
	{"612     ��ҵ���Ƿ�����",  "612",		CMD_HQ,612},

	{"613     �л��Ƿ�����",	"613",		CMD_HQ,613},

	{"81      �����ۺ�����",	"81",		CMD_MAIN,81},
	{"82      �����ۺ�����",	"82",		CMD_MAIN,82},
	{"83      ����ۺ�����",	"83",		CMD_MAIN,83},
	{"84      ����ۺ�����",	"84",		CMD_MAIN,84},
	{"85      ��ծ�ۺ�����",	"85",		CMD_MAIN,85},
	{"86      ��ծ�ۺ�����",	"86",		CMD_MAIN,86},
	{"87      ����ۺ�����",	"87",		CMD_MAIN,87},
	{"88      ����ۺ�����",	"88",		CMD_MAIN,88},
	{"89      ��С��ҵ�ۺ�����","89",		CMD_MAIN,89},
	{"811     �Ȩ֤�ۺ�����","811",		CMD_MAIN,811},
	{"812     ��ҵ���ۺ�����"  ,"812",		CMD_MAIN,812},

	{"91      1������",			"91",		CMD_COM,801},
	{"92      5������",			"92",		CMD_COM,802},
	{"93      15������",		"93",		CMD_COM,803},
	{"94      30������",		"94",		CMD_COM,804},
	{"95      60������",		"95",		CMD_COM,805},
	{"96      ����",			"96",		CMD_COM,807},
	{"97      ����",			"97",		CMD_COM,808},
	{"98      ����",			"98",		CMD_COM,809},
	{"99      N����",			"99",		CMD_COM,810},
	{"910     N������",			"910",		CMD_COM,806},
	{"911     ����",			"911",		CMD_COM,811},
	{"912     ����",			"912",		CMD_COM,812},

	{".7      �ƾ���Ѷ",		".7",		CMD_MAIN,1101},
	{"70      �ƾ���Ѷ",		"70",		CMD_MAIN,1101},
	{"07      �ƾ���Ѷ",		"07",		CMD_MAIN,1101},

	{".701",					".701",		CMD_NEWS,701},
	{"71",						"71",		CMD_NEWS,701},

	{".702 ",					".702",		CMD_NEWS,702},
	{"72",						"72",		CMD_NEWS,702},
	{".703 ",					".703",		CMD_NEWS,703},
	{"73",						"73",		CMD_NEWS,703},
	{".704 ",					".704",		CMD_NEWS,704},
	{"74",						"74",		CMD_NEWS,704},
	{".705 ",					".705",		CMD_NEWS,705},
	{"75",						"75",		CMD_NEWS,705},
	{".706 ",					".706",		CMD_NEWS,706},
	{"76",						"76",		CMD_NEWS,706},
	{".707 ",					".707",		CMD_NEWS,707},
	{"77",						"77",		CMD_NEWS,707},
	{".708 ",					".708",		CMD_NEWS,708},
	{"78",						"78",		CMD_NEWS,708},
	{".709 ",					".709",		CMD_NEWS,709},
	{"79",						"79",		CMD_NEWS,709},
	{".710 ",					".710",		CMD_NEWS,710},
	{".711 ",					".711",		CMD_NEWS,711},
	{".712 ",					".712",		CMD_NEWS,712},
	{".713 ",					".713",		CMD_NEWS,713},
	{".714 ",					".714",		CMD_NEWS,714},
	{".715 ",					".715",		CMD_NEWS,715},
	{".716 ",					".716",		CMD_NEWS,716},
	{".717 ",					".717",		CMD_NEWS,717},
	{".718 ",					".718",		CMD_NEWS,718},
	{".719 ",					".719",		CMD_NEWS,719},
	{".720 ",					".720",		CMD_NEWS,720},
	
	{"WT      ����ί��",		"WT",		CMD_MAIN,1000},

	{".901    ��ʽ������",		".901",		CMD_MAIN,901},
	{".902    ����ѡ�ɲ���",	".902",		CMD_MAIN,902},
	{".903    ����ϵͳ����",	".903",		CMD_MAIN,903},
	{".904    ̽�����ָ��",	".904",		CMD_MAIN,904},

	{".905    ����ѡ��",		".905",		CMD_MAIN,905},
	{".906    ����ѡ��",		".906",		CMD_MAIN,906},
	{".907    ����ѡ��",		".907",		CMD_MAIN,907},
	{".908    ���ѡ��",		".908",		CMD_MAIN,908},
	{".909    �ۺ�ѡ��",		".909",		CMD_MAIN,909},

	{".910    �г��״�����",	".910",		CMD_MAIN,910},
	{".911    �г��״��б�",	".911",		CMD_MAIN,911},
	{".918    ����Ԥ������",	".918",		CMD_MAIN,918},
	{".919    ����Ԥ���б�",	".919",		CMD_MAIN,919},

	{".912    ��ƽ�����",		".912",		CMD_MAIN,912},
	{".913    ��Ʊ��ϼ���",	".913",		CMD_MAIN,913},
	{".914    �����ھ�ο�",	".914",		CMD_MAIN,914},

	{".920    �ǿ�ͼ",			".920",		CMD_ADVHQ,0},
	{".921    ��������",		".921",		CMD_MAIN,921},

	{".930    ϵͳ����",		".930",		CMD_MAIN,930},
	{".931    �����û����",	".931",		CMD_MAIN,931},
	{".932    ͨѶ����",		".932",		CMD_MAIN,932},	
	{".933    �̺���������",	".933",		CMD_MAIN,933},
	{".934    ����ά������",	".934",		CMD_MAIN,934},
	{".935    �Զ�����",		".935",		CMD_MAIN,935},
	{".936    ����˵����",		".936",		CMD_MAIN,936},
	{".937    �û���̳",		".937",		CMD_MAIN,937},

    {"",0,0,0}  //CodeΪ0��ʾ����
};
*/

int g_nCmdTable = chDIMOF(g_pCmdTable);
WORD FuncMessageCode[][2] =	//������Item������Ϣ����
{ 
	CMD_COM,101, CMD_COM,102, CMD_COM,103, CMD_COM,104, CMD_COM,105,
	CMD_COM,106, CMD_COM,107, CMD_COM,108, CMD_COM,109, CMD_COM,110,
	CMD_COM,111, CMD_COM,112, CMD_COM,113, CMD_COM,114, CMD_COM,115, CMD_COM,116,CMD_COM,117,CMD_COM,118,CMD_COM,119,
	CMD_HQ, 201, CMD_HQ, 202, CMD_HQ, 203, CMD_HQ, 204, CMD_HQ, 205,
	CMD_HQ, 206, CMD_HQ, 207, CMD_HQ, 208, CMD_HQ, 209, CMD_HQ, 210,
	CMD_HQ, 211, CMD_HQ, 212, CMD_HQ, 213, CMD_HQ, 214, CMD_HQ, 215, CMD_HQ, 216,
	CMD_HQ, 217, CMD_HQ, 218, CMD_HQ, 219, CMD_HQ, 220, CMD_HQ, 221, CMD_HQ, 222,
	CMD_HQ, 223, CMD_HQ, 224, CMD_HQ, 225, CMD_HQ,226,
	CMD_COM,301, CMD_COM,302, CMD_COM,303, CMD_COM,304, CMD_COM,305,
	CMD_COM,306, CMD_COM,307, CMD_COM,308, CMD_COM,309, CMD_COM,310,
	CMD_COM,311, CMD_COM,312, CMD_COM,313, CMD_COM,314, CMD_COM,315, 
	CMD_COM,316, CMD_COM,317, CMD_COM,318, CMD_COM,319, CMD_COM,320, 
	CMD_COM,321, CMD_COM,322,
	CMD_HIS,400, CMD_HIS,401, CMD_HIS,402, CMD_HIS,403,CMD_HIS,404, CMD_HIS,405, CMD_HIS,406,
	CMD_COM,501, CMD_COM,502, CMD_COM,503, CMD_COM,504, CMD_COM,505,CMD_COM,506,CMD_COM,507,CMD_COM,508,
	CMD_MAIN,1100,		//��������
	CMD_MAIN,1101,		//��Ѷϵͳ
	CMD_MAIN,1102,		//�������
	CMD_MAIN,1000,		//����ί��
	CMD_MAIN,901,CMD_MAIN,902,CMD_MAIN,903,CMD_MAIN,904,
	CMD_MAIN,905,CMD_MAIN,906,CMD_MAIN,907,CMD_MAIN,908,CMD_MAIN,909,
	CMD_MAIN,910,CMD_MAIN,911,CMD_MAIN,918,CMD_MAIN,919,
	CMD_MAIN,912,CMD_MAIN,913,CMD_MAIN,914,
	CMD_ADVHQ,0, CMD_MAIN,921,
	CMD_MAIN,930,CMD_MAIN,931,CMD_MAIN,932,CMD_MAIN,933,CMD_MAIN,934,CMD_MAIN,935,CMD_MAIN,936,CMD_MAIN,937
};

struct FirstLevel FuncLevel1[] =
{	
	{"���̷���",19},{"���۷���",26},
	{"��������",22},{"�������",7},
	{"��ʱ����",8},
	{"��������",0},{"��Ѷϵͳ",0}, 
	{"�������",0},{"����ί��",0},
	{"ר��ϵͳ",4},
	{"ѡ����",5},
	{"Ԥ��ϵͳ",4},
	{"��������",3},
	{"�߼����鹦��",2},
	{"ϵͳ����",8},
	{"",0}
};

char *FuncLevel2[] = 
{
	".101 ��֤����������",
	".102 ��֤��ָ����",
	".103 ��֤��������",
	".104 ��֤�¹�����",
	".105 ��֤���ģ�ָ��",
	".106 ��֤���ָ��",
	".107 ��֤����������",
	".108 ��֤�ɷ�����",
	".109 ��֤��ָ����",
	".110 ��֤��������",
	".111 ��֤�¹�����",
	".112 ��֤���ģ�ָ��",
	".113 ��֤���ָ��",
	".114 ��֤��������",
	".115 ��֤����ָ��",
	".116 ��ҵ��ָ��",
	".117 ��С��ҵָ��",
	".118 ��֤����ָ��",
	".119 ����300����",

	".201 ����������ʾ",
	".202 �������",
	".203 ����ǿ�ƹ�",
	".204 �������ƹ�",
	".205 �ɽ�������",
	".206 �ܽ������",
	".207 ��������",
	".208 ί������",
	".209 �ּ�����",
	".210 ���´������",
	".211 ��ӯ������",
	".212 ����������",
	".213 ���ʲ�����",
	".214 ���ʲ�����",
	".215 ÿ����������",
	".216 ���ʲ�����������",
	".217 ��ͨ��ֵ����",
	".218 ��ͨ�ɱ�����",
	".219 AB������ֵ����",
	".220 �ܹɱ�����",
	".221 ÿ�ɹ���������",
	".222 ��Ծ������",
	".223 ÿ�ʾ�������",
	".224 ÿ�ʻ�������",
	".225 ÿ�ɾ��ʲ�����",
	".226 �о�������",	
	
	".301 �ͣ��ã�����ָ��",
	".302 �ģͣ�����ָ��",
	".303 �ģͣ�����ָ��",
	".304 �ţأУͣ�����ָ��",
	".305 �ԣңɣ�����ָ��",
	".306 �£ң�������ָ��",
	".307 �ã�����ָ��",
	".308 �֣�����ָ��",
	".309 �ϣ£�����ָ��",
	".310 ���ӣ�����ָ��",
	".311 �ţͣ�����ָ��",
	".312 �ף֣�������ָ��",
	".313 �ңӣ�ǿ��ָ��",
	".314 �ף���ǿ��ָ��",
	".315 �ӣ���ͣ��ָ��",
	".316 �ˣģʳ�����",
	".317 �ããɳ�����",
	".318 �ңϣó�����",
	".319 �ͣɣˣ�ѹ��֧��",
	".320 �£ϣ̣�ѹ��֧��",
	".321 �Уӣ�������",
	".322 �ãģ����Ʋ���",

	".400 ���Ű�����",
	".401 ��ʷ���鱨��",
	".402 ǿ����������",
	".403 �����ǵ�����",
	".404 ���任������",
	".405 �����������",
	".406 �����񵴷���",

	".501 ��ʱ����ͼ",
	".502 ��ʱ�ɽ���ϸ",
	".503 �ּ۱�",
	".504 ��ʳɽ���ϸ",
	".505 ��ϸ������",
	".506 ����/���ָ��",
	".507 ��������/�ǵ���",
	".508 ��������ͼ",

	".901 ��ʽ������",
	".902 ����ѡ�ɲ���",
	".903 ����ϵͳ����",
	".904 ̽�����ָ��",

	".905 ����ѡ��",
	".906 ����ѡ��",
	".907 ����ѡ��",
	".908 ���ѡ��",
	".909 �ۺ�ѡ��",

	".910 �г��״�����",
	".911 �г��״��б�",
	".918 ����Ԥ������",
	".919 ����Ԥ���б�",

	".912 ��ƽ�����",
	".913 ��Ʊ��ϼ���",
	".914 �����ھ�ο�",

	".920 �ǿ�ͼ",
	".921 ��������",

	".930 ϵͳ����",
	".931 �����û����",
	".932 ͨѶ����",
	".933 �̺���������",
	".934 ����ά������",
	".935 �Զ�����",
	".936 ����˵����",
	".937 �û���̳"
};

int   lpnSysDomainType[]={SH,SH,SZ,SZ,SH,SZ,SZ_SH,SZ_SH,SZ_SH,SZ_SH,SZ_SH,SZ_SH,SZ,SZ_SH,SZ};  //�Ϻ��������,������С��ҵ��ҵ��
short lpsSysClassType_new []={0x01,0x02,0x481,0x02,0x058,0x058,0x481,0x02,0x058,0x04,0x00,0x99,0x80,0x20,0x400};   //��Ʊ���:A/B/C

char  *GPType[]=
{
	"��֤����",
	"��֤�¹�",   
	"��֤����",   
	"��֤�¹�",   
	"��֤ծȯ",   
	"��֤ծȯ",   
	"�����",   
	"��¹�",   
	"�ծȯ",
	"�����",
	"������Ʒ",
	"����ָ��",
	"��С��ҵ",
	"�Ȩ֤",
	"��ҵ��",
	"��ѡ��",
	"��Ʊ��",
	"����֤ȯ",
	"���ָ��",
	"A+H��",
	"��ҵָ��",
};
//////////////////////////////////////////////////////////////////////////
char lpszRMItemName_All[][20] = 
{	"�������",
	"��Ȩ��",	//0
	"Ȩ�Ƿ�%%",	//1
	"�ɽ���",		//5
	"�ܽ��",		//6
	"�г���%%",		//7
	"������%%",		//8
	"��ӯ(��)",		//9
	"���ǹ�Ʊ",
	"�ǹɱ�",

	"������",
	"������",
	"�е�����",
	"�е�����",
	"С������",
	"С������",
};
float lpnRMItemWidth_All[]={10,9,9,10,10,9,9,9,10,8,9,9,9,9,9,9};
float lpnRMItemWidth_Long_All[]={30,9,9,10,10,9,9,9,10,8,9,9,9,9,9,9};

int nColToXh_Hq[]={0,1,2,3,4,5,6,7,8,9};
int nColToXh_Zj[]={0,2,3,4,10,11,12,13,14,15};
//////////////////////////////////////////////////////////////////////////
//��ʷ�������Ŀ����
char  lpszZBItemName[][20] =
{	"����   ",			//0
	"����",			//1
	" �Ƿ�%%",			//6
	" ����",			//2
	" ���",			//3
	" ���",			//4
	" ����",			//5
	"�ɽ���",			//7
	"�ܽ��",			//8
	"������%%",          //9
	"�����1",
	"�����2",
	"�����3",
	"�����4",
	"�����5",
	"�����6",

	"�����1",
	"�����2",
	"�����3",
	"�����4",
	"�����5",
	"�����6",

	"�����1",
	"�����2",
	"�����3",
	"�����4",
	"�����5",
	"�����6",

	"�����1",
	"�����2",
	"�����3",
	"�����4",
	"�����5",
	"�����6"
};
float lpnZBItemWidth[]={7,9,7,7,7,7,7,8,9,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9};

//ǿ����������Ŀ����
char  lpszQRItemName[][20] =
{	"����   ",				//0
	"����",			//1
	"����ǿ",			//2
	"        ",			//3
	"        ",			//4
	"        ",			//5
	"        ",			//6
	"        ",			//7
	"��ǿ��",			//8
	"����",				//9
	"���",				//10
	"���",				//11
	"����",				//12	
	"�ܽ��"			//13
};
float lpnQRItemWidth[] = {7,9,7,7,7,7,7,7,7,7,7,7,7,9};
//�����������Ŀ
char lpszQJItemName[][20] =
{	"����   ",		//0
	"����",			//1
	"ǰ����",		//2
	"���",			//3
	"���",			//4
	"����",			//5
	"�ǵ�����",		//6
	"�񵴷���",		//7
	"�ɽ���",       //8
	"�ܽ��",       //9
	"�г���%%",		//10				//�ܽ��/9999�ܽ��
	"������%%",		//11              	//�ܽ��/�ܽ��
	"5������%%"   	//12				//��5���������
};

float lpnQJItemWidth[]={7,9,7,7,7,7,9,9,9,9,8,8,10};

int naPriceExtent []={0,1,6,2,3,4,5,7,8,9,10,11,12};	//�ǵ�����
int naPopPoint    []={0,1,11,2,3,4,5,6,7,8,9,10,12};	//����������
int naVolumeExtent[]={0,1,12,2,3,4,5,6,7,8,9,10,11};	//�������
int naDiffExtent  []={0,1,7,2,3,4,5,6,8,9,10,11,12};	//�񵴷���

char *QJTypeStr[] = {"�ǵ�����","����������","�������","�񵴷���"};

char *AmiComboStr[8] = {"���������","ƽ���������","�۳����Ӯ�����������","ƽ��Ӯ�����","ƽ��������С��","�������Ӯ�����","������������С��","ӯ��ϵ�����"};

BOOL	bStartTips = TRUE;
int		nTipFilePos = 0;

HWND			FirstWindow,BigMenuWindow;
CViewManager	theViewManager;

struct GRIDCOLOR	GridColor;
struct VIPCOLOR		VipColor;
struct TXTCOLOR		TxtColor;
struct TREECOLOR	TreeColor;
struct SYSTEMCOLOR  SystemColor;

struct MENUCOLOR	MenuColor;

int					CurrentScheme;

CString HomePath="";

int AutoPageTime=8;
int HQRefreshTime = 4;
int StatusRefreshTime=5;

BOOL g_bAutoConnectInfo = FALSE;	//�������Զ���¼��Ѷ
BOOL g_bAutoLogo = FALSE;			//�������Զ���¼����
BOOL g_bAutoDS = FALSE;				//�������Զ���¼��չ����
BOOL g_bUseAutoConnect = TRUE;	//�Ƿ�ʹ���Զ�����
int	 g_nAutoConnectFlag = AUTOCONNECT_RESET;	//AUTOCONNECT_RESET,AUTOCONNECT_PENDING,AUTOCONNECT_RECONNECT
int	 g_nDSAutoConnectFlag = AUTOCONNECT_RESET;
int	 g_nAutoConnectNum = 0;		//�Զ������Ĵ���
BOOL g_bHasTdxProxy = FALSE;	//�Ƿ���ISA����

BOOL bTreeShow=FALSE;
BOOL bRestShow=FALSE;
int	 nLastTreeSel=0;
int	 nLastRestSel=0;
BOOL bToolBarShow=TRUE;
BOOL bStatusBarShow=TRUE;
BOOL g_bRightShow=TRUE;		//�Ƿ������ұߵ�����
BOOL g_nRollInfoHeight=24;	//������Ѷ�ĸ߶�

CString FirstBlockName;
int FirstGPType;
int FirstBlockFlag;
CString FirstHisBlockName;
int FirstHisGPType;
int FirstHisBlockFlag;

int g_CurrentJBM;	//��ǰ�û���ϲ���Ļ�������

//�����ڻ�������ģ���д��ݹ�Ʊ����
CString TxtCurGPCode;
short	TxtCurSetCode = -1;	//-1:�����������,SZ:�������в���,SH:���Ϻ��в���
//�����ڷ���ģ���д��ݹ�Ʊ����
CString ComCurGPCode = "600839";
short	ComCurSetCode = SH;	//-1:�����������,SZ:�������в���,SH:���Ϻ��в���
//���ڷ�����Ѷ��
CString WebInfoCurGPCode = "600839";
short	WebInfoCurSetCode = SH;	//-1:�����������,SZ:�������в���,SH:���Ϻ��в���
CString WebURL;
//���ڲƾ���Ѷ
CString CjzxCurGPCode = "600839";
short	CjzxCurSetCode = SH;	//-1:�����������,SZ:�������в���,SH:���Ϻ��в���
//���ڻ�������
CString JbfxCurGPCode = "600839";
short	JbfxCurSetCode = SH;	//-1:�����������,SZ:�������в���,SH:���Ϻ��в���

//��������ʷ�����д��ݵ�ǰ�û����ȴ򿪵���ʷ��������
int		HisCurType = 0;

int XXMNum = 0;
int XXMType[20];
CString XXMTopic[20];

struct SYSTEMCOLOR Scheme_Hold; //ϵͳ��������
struct SYSTEMCOLOR Scheme[20];
CString SchemeStr[20];
int SchemeNum;

struct SchemeItemContent theItemContent[28] = 
{
	"����ɫ",0,
	"���߼�������",1,
	"����",2,
	"�µ���",3,
	"ƽ��",4,
	"������",5,
	"������,��",6,
	"�����",7,
	"����������ɫ",8,
	"�ɽ�����ɫ",9,
	"ͼ������",10,
	"��������ɫ",11,
	"���߹��߻�����ɫ",12,
	"ָ�����1��ɫ",13,
	"ָ�����2��ɫ",14,
	"ָ�����3��ɫ",15,
	"ָ�����4��ɫ",16,
	"ָ�����5��ɫ",17,
	"ָ�����6��ɫ",18,
	"��������(�뱳����ͬ����ȥ)",19,
	"�����ѡ����",20,
	"�����̶������ı�",21,
	"�����̶����������ͷ",22,
	"������Ʊ���������",23,
	"�����������ɫ/��ѡ����ɫ",24,
	"���Ϻ���Ѷ�ı�",25,
	"�������ı�",26,
	"���ϡ���Ѷ�͹���������",27
};

//��������ѡ��
char *g_CalcPeriodStr[] = {"1����","5����","15����","30����","60����","����","����","����","�����","������","����","����",NULL};

int		IndexSortNum;	//����ָ�����,���֧��4��ָ��
int		CurIndexSort;	//��ǰ���ڴ��������ָ��
struct	IndexSortInfo_Struct IndexSortInfo[4];

//���ڴ��ǿ��������������
int QRParam[5];

WARN_STRU		g_WarnBuffer[WARN_BUFFER_SIZE];
int				g_nWarnNum = 0 ;
WARN_STRU		g_TJWarnBuffer[WARN_BUFFER_SIZE];
int				g_nTJWarnNum = 0 ;
int				g_nWarnRangeMinute = 5;
deque<WARN_STRU> g_TimeRangeWarnBuffer;
int				g_nWarnHQPos = 0;

WARNGLOBALPARAM		g_WarnParam;
WARNGLOBALPARAM2	g_WarnParam2;
TJ_WARN				g_aWarnTjEx[MAX_TJWARN];
short				g_aWarnGpSet[MAXGPNUM]={0};
int					g_nWarnGpNum = 0;
PWARNINGCND			*g_pAbsCondition=NULL;
int					g_pAbsConditionNum = 0;
long				g_nUnitRadarFlag = 0;
int					g_nWarnRefresh=5;
long				g_nUnitDeepWarnFlag = 0;

//����ʵ���ϷŵĹ��
HCURSOR		m_hCanPressB,m_hCanPressS,m_hCanPress,m_hCanDrop,m_hCanDrop_Overlap,m_hCannotDrop,m_hZoomCur,m_hZoomCur2,m_hZoomCur3,m_hVerSplit,m_hHorSplit,m_hMove,m_hCursor2,m_hDraw,m_hDrawMove,m_hDrawTurn,m_hFxtMove,m_hToBig,m_hToSmall;
HICON		m_hExpUpShow,m_hExpDownShow,m_hKLineShow,m_hExpUpShow_Sel,m_hExpDownShow_Sel,m_hKLineShow_Sel,m_hGBBQShow,m_hGGBWShow,m_hOneThunder,m_hMoreThunder,m_hMyThunder[6],m_hIcon_Pu[8],m_hFCIcon,m_hZstFlushIcon;
HICON		m_hRedUP,m_hRedDown,m_hGreenUP,m_hGreenDown,m_hIconUp,m_hIconDown,m_hIconPBuy[3],m_hIconPSell[3];

char g_cwAddrTopic[][10]=
{ 
	"������","�½����","���ְ��","������","�������","�ຣ���","�������","�������",
	"�����" ,"�������","�ӱ����","�㶫���","���ϰ��","���İ��","ɽ�����","�Ϻ����",
	"ɽ�����" ,"���ڰ��","�������","�������","���ϰ��","�������","�Ĵ����","���հ��",
	"������" ,"���հ��","���ϰ��","�㽭���","���ݰ��","���ϰ��","���ذ��","���ɰ��"
};
char g_pDyBlockPy[TOTAL_DY_NUM][5]=
{
  "HLJ" ,"XJBK","JLBK","GSBK","LLBK","QHBK","BJBK","SXBK",
  "TJBK","GXBK","HBBK","GDBK","HNBK","LXBK","SDBK","SHBK",
  "SXBK","SZBK","HBBK","FJBK","HNBK","JXBK","SCBK","AHBK",
  "CQBK","JSBK","YNBK","ZJBK","GZBK","HNBK","XZBK","NMBK"
};
char g_cwHYstr[][9]=
{
"������ҵ","������һ","������ҵ","�Ҿ���ҵ","ʯ����ҵ","��·����",
"��������","��ͨ����","ҽ����е","�Ƶ�����","���ز�ҵ","��ҵ�ٻ�",
"������ó","ʳƷ��ҵ","��֯��ҵ","������ҵ","ũ������","��ý����",
"������ҵ","ú̿��ҵ","��������","ˮ����ҵ","�ҵ���ҵ","������Ϣ",
"�ۺ���ҵ","��е��ҵ","������ҵ","ũҩ����","������ҵ","Ħ�г�",
"������", "���г�","��������","������ҩ","��������","��ɫ����",
"�����ҵ","��ֽ��ҵ","������ҵ","�մ���ҵ","��װЬ��","��ˮ����",
"�����豸","�������","��֯��е","ӡˢ��װ","������Ʒ","������ҵ",
"�ɻ�����","��������","�����Ǳ�","��������"
};
char   g_pHyBlockPy[TOTAL_HY_NUM][5]=
{ 
	"JRHY","SSJG","GTHY","JJHY","SYHY","GLQL",
	"QCZZ","JTYS","YLQX","JDLY","FDC","SYBH",
	"WZWM","SPGY","FZHY","DLHY","NLMY","CMYL",
	"HGHY","MTHY","JZJC","SNHY","JDHY","DZXX",
	"ZHHY","JXHY","HXHY","NYHF","DQHY","MTC",
	"KFQ" ,"ZXC" ,"CBZZ","SWZY","DZQJ","YSJS",
	"NJHY","ZZHY","HBHY","TCHY","FZXL","GSGQ",
	"FDSB","ZBHY","FZJX","YSBZ","SLZP","BLHY",
	"FJZZ","GGCL","YQYB","QTHY"
};

CLE_CONTABLE g_pCleKLine1[]=
{
	{"�͵�������������",    KLINE_1001   ,KLINETL_LOW         ,CLETYPE_0},
	{"�͵�������������",    KLINE_1002   ,KLINETL_LOW         ,CLETYPE_0},
	{"�͵������н�����",    KLINE_1003   ,KLINETL_LOWDULL     ,CLETYPE_0},
	{"�͵������й�����",    KLINE_1004   ,KLINETL_LOWDULL     ,CLETYPE_0},
	{"�͵���������������",  KLINE_1005   ,KLINETL_LOWDULL     ,CLETYPE_0},
	{"�͵���������������",  KLINE_1006   ,KLINETL_LOWDULL     ,CLETYPE_0},

	{"�͵�����ĩ�ڹ�����",  KLINE_1007   ,KLINETL_LOWDULL3    ,CLETYPE_1},
	{"�͵����̸��ڽ�����",  KLINE_1008   ,KLINETL_LOWDULL3    ,CLETYPE_1},
	{"�͵�����ĩ����ͻ��",  KLINE_1009   ,KLINETL_LOWDULL3    ,CLETYPE_1},
	{"С��������������",  KLINE_1010   ,KLINETL_LOWSLOWDOWN ,CLETYPE_1},
	{"�͵�����ĩ�ں�����",  KLINE_1011   ,KLINETL_LOWDULL     ,CLETYPE_1},

	{"�͵�����������",      KLINE_1012   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"��������",            KLINE_1013   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵���������ʮ����",  KLINE_1014   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵���������������",  KLINE_1015   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵���������������",  KLINE_1016   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵�������ͷ����",  KLINE_1017   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵������������",    KLINE_1018   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵������������",    KLINE_1019   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵�������������",    KLINE_1020   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵��������������",  KLINE_1021   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵�������������",    KLINE_1022   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵�������������",    KLINE_1023   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵���������ʮ����",  KLINE_1024   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵��������й�����",  KLINE_1025   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵��������н�����",  KLINE_1026   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵�������������",    KLINE_1027   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵�������������",    KLINE_1028   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵�������������",    KLINE_1029   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵���������������",  KLINE_1030   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"�͵����ξѻ�������",  KLINE_1031   ,KLINETL_LOW         ,CLETYPE_2},
	{""  ,0,0,0}
};

CLE_CONTABLE g_pCleKLine2[]=
{
	{"�ϵ����������ͻ��",  KLINE_1032   ,KLINETL_HIGH        ,CLETYPE_3},
	{"����һ���ߺ󲻲�ȱ",  KLINE_1033   ,KLINETL_HIGH        ,CLETYPE_3},
	{"���θ߿�����",        KLINE_1034   ,KLINETL_HIGH        ,CLETYPE_3},
	{"������Ԯ��",          KLINE_1035   ,KLINETL_HIGH        ,CLETYPE_3},
	{"����������",          KLINE_1036   ,KLINETL_UP          ,CLETYPE_3},
	{"����������ʮ����",    KLINE_1037   ,KLINETL_UP          ,CLETYPE_3},
	{"���������߽�����",    KLINE_1038   ,KLINETL_UP          ,CLETYPE_3},
	{"���������߹�����",    KLINE_1039   ,KLINETL_UP          ,CLETYPE_3},
	{"������Сѹ����",      KLINE_1040   ,KLINETL_UP          ,CLETYPE_3},
	{"������α����",        KLINE_1041   ,KLINETL_UP          ,CLETYPE_3},
	{"������������",        KLINE_1042   ,KLINETL_UP          ,CLETYPE_3},
	{"�����й�˳��",        KLINE_1043   ,KLINETL_UP          ,CLETYPE_3},
	{"������������",        KLINE_1044   ,KLINETL_UP          ,CLETYPE_3},
	{"������������",        KLINE_1045   ,KLINETL_UP          ,CLETYPE_3},

	{"������������",        KLINE_1046   ,KLINETL_UP          ,CLETYPE_4},
	{"������������",        KLINE_1047   ,KLINETL_UP          ,CLETYPE_4},
	{"�����в�����",        KLINE_1048   ,KLINETL_UP          ,CLETYPE_4},
	{"�����л�����",        KLINE_1049   ,KLINETL_UP          ,CLETYPE_4},
	{"����������",          KLINE_1050   ,KLINETL_UP          ,CLETYPE_4},
	{"��������",            KLINE_1051   ,KLINETL_UP          ,CLETYPE_4},
	{"������������",        KLINE_1052   ,KLINETL_UP          ,CLETYPE_4},
	{"��Խ����ѹ����",      KLINE_1053   ,KLINETL_UP          ,CLETYPE_4},
	{"������;���������",  KLINE_1054   ,KLINETL_UP          ,CLETYPE_4},
	{"������;˼��������",  KLINE_1055   ,KLINETL_UP          ,CLETYPE_4},
	{"������;������",      KLINE_1056   ,KLINETL_UP          ,CLETYPE_4},
	{"������;������",      KLINE_1057   ,KLINETL_UP          ,CLETYPE_4},
	{"������;α����",      KLINE_1058   ,KLINETL_UP          ,CLETYPE_4},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleKLine3[]=
{
	{"�ߵ�������������",    KLINE_1059   ,KLINETL_HIGH        ,CLETYPE_5},
	{"�ߵ������������",    KLINE_1060   ,KLINETL_HIGH        ,CLETYPE_5},
	{"����������",          KLINE_1061   ,KLINETL_HIGH        ,CLETYPE_5},
	{"�ߵ���������������",  KLINE_1062   ,KLINETL_HIGH        ,CLETYPE_5},
	{"�ߵ������й�����",    KLINE_1063   ,KLINETL_HIGH        ,CLETYPE_5},
	{"�ߵ������н�����",    KLINE_1064   ,KLINETL_HIGH        ,CLETYPE_5},
	{"�����߽ӻ�����",      KLINE_1065   ,KLINETL_HIGH        ,CLETYPE_5},
	{"������ѻ",            KLINE_1066   ,KLINETL_HIGH        ,CLETYPE_5},

	{"�ߵ�����ĩ������",    KLINE_1067   ,KLINETL_HIGHDULL3   ,CLETYPE_6},
	{"�ߵ�����ĩ����",     	KLINE_1068   ,KLINETL_HIGHDULL3   ,CLETYPE_6},
	{"�ߵ�����ĩ������",    KLINE_1069   ,KLINETL_HIGHDULL3   ,CLETYPE_6},
	{"С�����������",      KLINE_1070   ,KLINETL_HIGH        ,CLETYPE_6},
	{"������ѻ",            KLINE_1071   ,KLINETL_HIGH        ,CLETYPE_6},
	{"��¥��",              KLINE_1072   ,KLINETL_HIGH        ,CLETYPE_6},

	{"�ߵ�����������",      KLINE_1073   ,KLINETL_HIGHFASTUP  ,CLETYPE_7},
	{"��������",            KLINE_1074   ,KLINETL_HIGHFASTUP  ,CLETYPE_7},
	{"�ߵ���ʮ����",        KLINE_1075   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ���������",        KLINE_1076   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ���������",        KLINE_1077   ,KLINETL_HIGHFASTUP  ,CLETYPE_7},
	{"�ߵ���ͷ����",        KLINE_1078   ,KLINETL_HIGH        ,CLETYPE_7},
	{"����������",          KLINE_1079   ,KLINETL_HIGH        ,CLETYPE_7},
	{"����������",          KLINE_1080   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ����ջ�����",      KLINE_1081   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�������",          KLINE_1082   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�����������",    KLINE_1083   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�������",          KLINE_1084   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�����������",      KLINE_1085   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�������",          KLINE_1086   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�������",          KLINE_1087   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�����ʮ����",      KLINE_1088   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ����е�����",      KLINE_1089   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�������",          KLINE_1090   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�������",          KLINE_1091   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ�������",          KLINE_1092   ,KLINETL_HIGH        ,CLETYPE_7},
	{"�ߵ����ξѻ�����",    KLINE_1093   ,KLINETL_HIGH        ,CLETYPE_7},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleKLine4[]=
{
	{"�µ���������ͻ��",    KLINE_1094   ,KLINETL_LOWDULL     ,CLETYPE_8},
	{"��ˮһ���ߺ󲻲�ȱ",  KLINE_1095   ,KLINETL_LOWDULL     ,CLETYPE_8},
	{"������������",        KLINE_1096   ,KLINETL_DOWN        ,CLETYPE_8},
	{"���ջ�����",          KLINE_1097   ,KLINETL_DOWN        ,CLETYPE_8},
	{"����������",          KLINE_1098   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ�����ˮʮ����",    KLINE_1099   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ�����ˮ������",    KLINE_1100   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ�����ˮ������",    KLINE_1101   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ���С�����",      KLINE_1102   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ���α����",        KLINE_1103   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ���������",        KLINE_1104   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ��з�����",        KLINE_1105   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ���������",        KLINE_1106   ,KLINETL_DOWN        ,CLETYPE_8},
	{"�µ���������",        KLINE_1107   ,KLINETL_DOWN        ,CLETYPE_8},

	{"�µ��и�����",        KLINE_1108   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ���������",        KLINE_1109   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ��в�����",        KLINE_1110   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ��в�����",        KLINE_1111   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ�����Ԯ��",        KLINE_1112   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�½�������",          KLINE_1113   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�½�����",            KLINE_1114   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�½���������",        KLINE_1115   ,KLINETL_DOWN        ,CLETYPE_9},
	{"��Խ��������",      KLINE_1116   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ������������",    KLINE_1117   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ���˼��������",    KLINE_1118   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ���������",        KLINE_1119   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ���������",        KLINE_1120   ,KLINETL_DOWN        ,CLETYPE_9},
	{"�µ���α����",        KLINE_1121   ,KLINETL_DOWN        ,CLETYPE_9},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleJBM[] =
{
	{"С�̴��¹�",		JBMXG_1001,0,CLETYPE_10},
	{"���̴��¹�",		JBMXG_1002,0,CLETYPE_10},
	{"���̴��¹�",		JBMXG_1003,0,CLETYPE_10},
	{"С�̼��Ź�",		JBMXG_1004,0,CLETYPE_10},
	{"С�̼�ƽ��",		JBMXG_1005,0,CLETYPE_10},
	{"С�̼����",		JBMXG_1006,0,CLETYPE_10},
	{"���̼��Ź�",		JBMXG_1007,0,CLETYPE_10},
	{"���̼�ƽ��",		JBMXG_1008,0,CLETYPE_10},
	{"���̼����",		JBMXG_1009,0,CLETYPE_10},
	{"���̼��Ź�",		JBMXG_1010,0,CLETYPE_10},
	{"���̼�ƽ��",		JBMXG_1011,0,CLETYPE_10},
	{"���̼����",		JBMXG_1012,0,CLETYPE_10},
	{"�����",			JBMXG_1013,0,CLETYPE_10},
	{"�߹ɱ�����������",JBMXG_1014,0,CLETYPE_10},
	{"�߸�����������"  ,JBMXG_1015,0,CLETYPE_10},
	{"�߷����������",JBMXG_1016,0,CLETYPE_10},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleJSM[] =
{
	{"BIAS����ָʾ",	JSMXG_1001,0,CLETYPE_11}, 
	{"SAR����ָʾ",		JSMXG_1002,0,CLETYPE_11}, 
	{"KDJ����ָʾ",		JSMXG_1003,0,CLETYPE_11}, 
	{"RSI����ָʾ",		JSMXG_1004,0,CLETYPE_11}, 
	{"MACD����ָʾ",	JSMXG_1005,0,CLETYPE_11}, 
	{"MTM����ָʾ",		JSMXG_1006,0,CLETYPE_11}, 
	{"",0,0,0}
};

//K��������̬�����
short TrendCodeTable[] =
{
	KLINETL_HIGH                    ,
	KLINETL_LOW                     ,
	KLINETL_TOSS                    ,
	KLINETL_DULL                    ,
	KLINETL_UP                      ,
	KLINETL_DOWN                    ,
	KLINETL_HIGHTOSS                ,
	KLINETL_LOWTOSS                 ,
	KLINETL_HIGHDULL                ,
	KLINETL_LOWDULL                 ,
	KLINETL_FASTUP                  ,
	KLINETL_FASTDOWN                ,
	KLINETL_SLOWUP                  ,
	KLINETL_SLOWDOWN                ,
	KLINETL_HIGHTOSS1               ,
	KLINETL_LOWTOSS1                ,
	KLINETL_HIGHTOSS2               ,
	KLINETL_LOWTOSS2                ,
	KLINETL_HIGHTOSS3               ,
	KLINETL_LOWTOSS3                ,
	KLINETL_HIGHDULL1               ,
	KLINETL_LOWDULL1                ,
	KLINETL_HIGHDULL2               ,
	KLINETL_LOWDULL2                ,
	KLINETL_HIGHDULL3               ,
	KLINETL_LOWDULL3                ,
	KLINETL_HIGHFASTUP              ,
	KLINETL_HIGHFASTDOWN            ,
	KLINETL_HIGHSLOWUP              ,
	KLINETL_HIGHSLOWDOWN            ,
	KLINETL_LOWFASTUP               ,
	KLINETL_LOWFASTDOWN             ,
	KLINETL_LOWSLOWUP               ,
	KLINETL_LOWSLOWDOWN             
};

CUSTXG_CONTABLE g_nCustCon[] = 
{
	{"�ܹɱ�",C_ZGB,C_CUSTJBM,C_VT_FBJ,"���"},
	{"��ͨ�ɱ�",C_LTB,C_CUSTJBM,C_VT_FBJ,"���"},

	{"B��/A��",C_BG,C_CUSTJBM,C_VT_FBJ,"���"},
	{"H��",C_HG,C_CUSTJBM,C_VT_FBJ,"���"},

	{"AB������ֵ",C_ZSZ,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"��ֵͨ",C_LTSZ,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},

	{"���ʲ�",C_ZZC,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"�����ʲ�",C_LDZC,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},

	{"�����ʲ�",C_wxzc,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"����Ͷ��",C_cqtz,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"������ծ",C_ldfz,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"���ڸ�ծ",C_cqfz,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"�ʱ�������",C_zbgjj,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"Ӧ���ʿ�",C_yszk,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"Ӫҵ����",C_yyly,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"Ͷ������",C_tzsy,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"��Ӫ�ֽ�����",C_jyxjl,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"���ֽ�����",C_zxjl,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"���",C_ch,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"������",C_jly,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"δ��������",C_wfply,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},

	{"���ʲ�(�ɶ�Ȩ��)",C_JZC,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},

	{"ÿ�ɾ��ʲ�",C_MGJZC,C_CUSTJBM,C_VT_FBJ,"Ԫ"},
	{"ÿ�ɹ�����",C_MGGJJ,C_CUSTJBM,C_VT_FBJ,"Ԫ"},
	{"ÿ��δ����",C_MGWFP,C_CUSTJBM,C_VT_FBJ,"Ԫ"},
	{"ÿ������",C_MGSY,C_CUSTJBM,C_VT_FBJ,"Ԫ"},

	{"�ɶ�Ȩ���",C_GDQYB,C_CUSTJBM,C_VT_FBJ,"%"},
	{"�ʱ�������",C_ZBGJJ,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"��Ӫ����",C_ZYSY,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"�����ܶ�",C_LYZE,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"˰������",C_SHLY,C_CUSTJBM,C_VT_FBJ,"��Ԫ"},
	{"���ʲ�������",C_JYL,C_CUSTJBM,C_VT_FBJ,"%"},

	{"�ּ�",C_NOW,C_CUSTHQ,C_VT_FBJ,"Ԫ"},
	{"���",C_MAX,C_CUSTHQ,C_VT_FBJ,"Ԫ"},
	{"���",C_MIN,C_CUSTHQ,C_VT_FBJ,"Ԫ"},
	{"��",C_OPEN,C_CUSTHQ,C_VT_FBJ,"Ԫ"},
	{"����",C_CLOSE,C_CUSTHQ,C_VT_FBJ,"Ԫ"},
	{"����",C_VOLUME,C_CUSTHQ,C_VT_FBJ,"��"},
	{"�ܽ��",C_AMOUNT,C_CUSTHQ,C_VT_FBJ,"Ԫ"},

	{"�Ƿ�",C_ZAF,C_CUSTHQ,C_VT_FBJ,"%"},
	{"���",C_ZEF,C_CUSTHQ,C_VT_FBJ,"%"},
	{"��ӯ(��)",C_SYL,C_CUSTHQ,C_VT_FBJ,"��"},
	{"������",C_HSL,C_CUSTHQ,C_VT_FBJ,"%"},
	{"����",C_LIANGB,C_CUSTHQ,C_VT_FBJ,"��"},

	{"��������",C_SSNUM,C_CUSTOTHER,C_VT_NBJ,"��"},
	{"KDJ�е�Jֵ",C_KDJ_J,C_CUSTOTHER,C_VT_NBJ,""},
	{"RSIָ��ֵ",C_RSI,C_CUSTOTHER,C_VT_NBJ,""},
	{"DMI�е�ADXֵ",C_DMI_ADX,C_CUSTOTHER,C_VT_NBJ,""},
	{"WRָ��",C_WR,C_CUSTOTHER,C_VT_NBJ,""},
	{"VRָ��ֵ",C_VR,C_CUSTOTHER,C_VT_NBJ,""},
	{"",0,0,0,""}
};

char *BjCmpStr[] = {">","=","<"};

float g_my_rate=7.800f,g_gy_rate=1.000f;

int		ComCurMessageCode = -1;

char WeekInfo[][10]=
{
	"������","����һ","���ڶ�","������","������","������","������"
};

char * strWeekInfo[8] = {"��","һ","��","��","��","��","��",};

int	g_InfoDlgPosx=600,g_InfoDlgPosy=500;
BOOL g_bInfoMinimize=FALSE;

/////////////////////////////////////////////////////////////////////
DDrawLineTool	g_DrawLineTool;
int			g_nTxtFontSize	= 8;
COLORREF	g_nTxtColor = 0x00ffffff;

CustomFunc	g_CF;
FMLPACK		g_FmlPack[MAX_FMLPACK_NUM];
int			g_nFmlPack = 0;
FML2STK		g_Fml2Stk[MAX_BANDSTOCK_NUM];
int			g_nFml2Stk = 0;

Gdi			g_d;

int			g_nMulNum = 9,g_nNewMulNum=9;
BOOL		g_bDynaZoom = TRUE;
BOOL		g_bZBudFlag = TRUE;
BOOL		g_bTPAxis = FALSE;
BOOL		g_bLockRight = FALSE;
INFODLG		g_DlgInfo;
CBINFO		g_CbInfo;

UINT		g_uBigToolBarBtn[100]={0};
int			g_nBigToolBarBtn = 0;

UINT		g_uBigToolBarBtn_I[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_NOTSUPPORT,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_NOTSUPPORT,ID_SEPARATOR,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_DEFTOOLGN17,IDC_DEFTOOLGN18,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_NOTSUPPORT,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_NOTSUPPORT};
int			g_nBigToolBarBtn_I = chDIMOF(g_uBigToolBarBtn_I);

UINT		g_uBigToolBarBtn_II[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_DEFTOOLGN10,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_DEFTOOLGN13,IDC_DEFTOOLGN27,IDC_DEFTOOLGN28,ID_SEPARATOR,IDC_DEFTOOLGN15,IDC_DEFTOOLGN16,IDC_DEFTOOLGN17,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_DEFTOOLGN19,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_DEFTOOLGN24};
int			g_nBigToolBarBtn_II = chDIMOF(g_uBigToolBarBtn_II);

UINT		g_uBigToolBarBtn_III[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_DEFTOOLGN10,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_DEFTOOLGN13,IDC_DEFTOOLGN27,IDC_DEFTOOLGN28,ID_SEPARATOR,IDC_DEFTOOLGN15,IDC_DEFTOOLGN16,IDC_DEFTOOLGN17,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_DEFTOOLGN19,IDC_DEFTOOLGN20,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_DEFTOOLGN24};
int			g_nBigToolBarBtn_III = chDIMOF(g_uBigToolBarBtn_III);

UINT		g_uBigToolBarBtn_IIII[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_DEFTOOLGN10,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_DEFTOOLGN13,IDC_DEFTOOLGN14,IDC_DEFTOOLGN27,ID_SEPARATOR,IDC_DEFTOOLGN15,IDC_DEFTOOLGN16,IDC_DEFTOOLGN17,IDC_DEFTOOLGN18,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_DEFTOOLGN19,IDC_DEFTOOLGN20,IDC_DEFTOOLGN21,IDC_DEFTOOLGN22,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_DEFTOOLGN24};
int			g_nBigToolBarBtn_IIII = chDIMOF(g_uBigToolBarBtn_IIII);


#if defined(OEM_STAR) || defined(OEM_STAR_HM)
	UINT g_uFixedBtn[]={ID_SHOWHQ,ID_SHOWZST,ID_SHOWVIP,ID_SHOWHISTORY,IDC_GGZL,IDC_ZXTREE,IDC_BROKER,ID_FUNC_WT,ID_FORMULA,ID_TDXCHOICEGP,ID_ETF};
#else
	UINT g_uFixedBtn[]={ID_SYSPAD,ID_SHOWHQ,ID_SHOWZST,ID_SHOWVIP,ID_SHOWHISTORY,IDC_GGZL,IDC_ZXTREE,IDC_BROKER,ID_FUNC_WT,ID_FORMULA,ID_TDXCHOICEGP,ID_ETF,ID_SEPARATOR};
#endif

int			g_nFixedBtn = chDIMOF(g_uFixedBtn);

UINT		g_uTotalDynBtn[]={ID_MULUNIT,ID_TOOLS,ID_SWITCHLBMMLD,ID_HIDERIGHT,ID_TPAXIS,ID_PAGEUP,ID_PAGEDOWN,ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_RIGHT_ALLWINDOW,
							  ID_OTHERJBM,ID_TDX_JBZL,ID_SELECTDATE,ID_LAST_DATE,ID_NEXT_DATE,ID_SELECTQJ,ID_INDEXRANK,ID_QR_OPENBASE,
							  ID_GPFL,ID_GPSORT,ID_GPZHSORT,ID_RIGHT_ADDBLOCK,ID_TXTFIND,ID_FINDRESULT,
							  ID_MANYDAYZST,ID_HXZBDLG,ID_FXT_MOVE,ID_SHOWPERIOD,ID_SHOWTQ,ID_HISZS,ID_SHOWWINNUM,
							  ID_WEB_BACK,ID_WEB_FORWARD,ID_WEB_STOP,ID_WEB_REFRESH,ID_PADLINKAGE,ID_BEGINSPLIT,ID_SYSPAD
							 };
int			g_nTotalDynBtn = chDIMOF(g_uTotalDynBtn);

UINT		g_uDynBtn[14][30] = 
{
	{ID_GPFL,ID_GPSORT,ID_GPZHSORT,ID_RIGHT_ADDBLOCK},
	{ID_MULUNIT,ID_SWITCHLBMMLD,ID_MANYDAYZST,ID_TPAXIS,ID_HIDERIGHT,ID_TOOLS,ID_PAGEUP,ID_PAGEDOWN},
	{ID_MULUNIT,ID_HXZBDLG,ID_SHOWPERIOD,ID_SHOWWINNUM,ID_SHOWTQ,ID_HISZS,ID_HIDERIGHT,ID_TOOLS,ID_FXT_MOVE,ID_PAGEUP,ID_PAGEDOWN},
	{ID_MULUNIT,ID_TPAXIS,ID_PAGEUP,ID_PAGEDOWN},
	{ID_PAGEUP,ID_PAGEDOWN},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_SEPARATOR,ID_GPFL,ID_SELECTDATE,ID_LAST_DATE,ID_NEXT_DATE,ID_INDEXRANK},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_SEPARATOR,ID_GPFL,ID_QR_OPENBASE},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_SEPARATOR,ID_GPFL,ID_SELECTQJ},
	{ID_OTHERJBM,ID_TDX_JBZL,ID_TXTFIND,ID_FINDRESULT},
	{ID_RIGHT_ALLWINDOW,ID_TXTFIND,ID_FINDRESULT},
	{ID_TDX_JBZL},
	{ID_PADLINKAGE,ID_BEGINSPLIT},
	{ID_SEPARATOR}
};
int			g_nDynBtn[14]={4,8,11,4,2,4,10,7,7,4,3,1,2,1};

CString		g_TxtFindStr[MAX_ANDSTR];
int			g_nTxtFindNum=0;
CString		g_NewsFindStr[MAX_ANDSTR];
int			g_nNewsFindNum=0;
int			g_nTxtFindNo = -1;
int			g_nNewsFindNo = -1;
BOOL		g_nFindRadio = 0;

int			g_nFixedWidth = 198;
int			g_nFixedWidth2 = 214;

float		g_fProfitFee = 6.5;

//22�ֳ���ָ��
char		g_strOftenUseZb[][ZBCODE_LEN] = {"������ת","���̸���","�����ź�","��������","��ʦ����","����ׯ��","����׷��","�ʽ���","�ײ�����","��������","��ӥ̤ׯ","�ײ�����","׷�ٺ���","˳�ƶ�Ϊ","��ӥ��ׯ","��ͣ׷��","��������","��������","׽ׯɢ��","�ʽ���"};
//char		g_strOftenUseZb[][ZBCODE_LEN] = {"MACD","DMI","DMA","FSL","TRIX","BRAR","CR","VR","OBV","ASI","EMV",
//								   "VOL","RSI","WR","SAR","KDJ","CCI","ROC","MTM","BOLL","PSY","MCST"};
int			g_nOftenUseZb=sizeof(g_strOftenUseZb)/sizeof(char[ZBCODE_LEN]);
//11����ͼָ��
char		g_strFavUseZb[][ZBCODE_LEN] = {"VOL","MACD","KDJ","DMI","DMA","FSL","TRIX","BRAR","CR","VR","OBV","ASI"};
char		g_strZbMain[ZBCODE_LEN] = "��Ӯ����";
char		g_strLastExp[ZBCODE_LEN] = "";
char		g_strLastKLine[ZBCODE_LEN] = "";
char		g_strLastZBSort[ZBCODE_LEN] = "";

int			g_nMainZbFlag=0;	//0:K�� 1:BAR��
BOOL		g_bHasXHidden=FALSE,g_bHasYHidden=TRUE;

long		g_nHligtNowVol = 1000; //��

char		g_strFontName[11][40] = {"Tahoma","Tahoma",KAI_FONT,"Tahoma",SONG_FONT,SONG_FONT,"Tahoma",KAI_FONT,KAI_FONT,"Tahoma","Tahoma"};
int			g_nFontSize[11] = {16,13,16,16,13,13,16,21,19,18,19};
int			g_nFontWeigth[11] = {FW_NORMAL,FW_NORMAL,FW_SEMIBOLD,FW_MEDIUM,FW_NORMAL,FW_NORMAL,FW_NORMAL,FW_BOLD,FW_BOLD,FW_BOLD,FW_BOLD};
BOOL		g_bBigFontGrid = FALSE;
CPoint		g_ZbDragPoint = CPoint(0,0);

TINDEXINFO	g_SpeclIndex[]=
{
	{	0 , SEPCLINE_TYPE  , "EQUVOL" ,  "����ͼ" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.����ͼ��ѹ��ͼ�Ļ���һ�������ǣ���ʴ����խ��K �߲���\
�����ص���\r\n\
2.����ͼû�������ķ֡�\r\n\
3.����ͼ��Ҫ�ǹ۲������н��У�������ɽ���ʱ�ɼ۵ı仯��",	1
	} , 
	{	1 , SEPCLINE_TYPE  , "EQUK" ,  "��������" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.����K ����ѹ��ͼ�Ļ���һ�������ǣ���ʴ����խ��K �߲�\
�ụ���ص���\r\n\
2.����K �߿������������ߡ�\r\n\
3.����K ����Ҫ�ǹ۲������н��У�������ɽ���ʱ�ɼ۵ı仯",	1
	} , 
	{	2 , SEPCLINE_TYPE  , "CAN" ,  "ѹ��ͼ" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.��Ϊ��ϳɽ�����K �߻������ɽ���Խ����K ��Խ���ɽ�\
��ԽС����K ��Խխ��\r\n\
2.ͼ���Ϻ����ȸ�ֵ�ľ��벻�䣬���ԣ���K ����Ϊ��ɽ���\
����÷ʴ�ʱ���Ḳ�ǵ��Ա���խ��K �ߣ�Ҳ����˵���ɽ���\
С��K �߻ᱻ���ԣ�ֻ�нϿ��K �߲ž������塣",	1
	} , 
	{	3 , SEPCLINE_TYPE  , "OX" ,  "Ȧ��ͼ" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.ͼ���ɡ��𡹱�ɡ�����ʱ����ʾ�����\r\n\
2.ͼ���ɡ�������ɡ���ʱ����ʾ������\r\n\
3.OXͼ�Ͽ��Ի������ߣ�����������ͻ��������ʱ����Ϊ����źš�\r\n\
4.OXͼ�Ͽ��Ի������ߣ����������µ���������ʱ����Ϊ�����źš�\r\n\
5.OXͼ����Ԥ��δ���ǵ�����",	1
	} , 
	{	4 , SEPCLINE_TYPE  , "OXHL" ,  "�ߵ�Ȧ��ͼ" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.ͼ���ɡ��𡹱�ɡ�����ʱ����ʾ�����\r\n\
2.ͼ���ɡ�������ɡ���ʱ����ʾ������\r\n\
3.OXͼ�Ͽ��Ի������ߣ�����������ͻ��������ʱ����Ϊ����źš�\r\n\
4.OXͼ�Ͽ��Ի������ߣ����������µ���������ʱ����Ϊ�����źš�\r\n\
5.OXͼ����Ԥ��δ���ǵ�����",	1
	} , 
	{	5 , SEPCLINE_TYPE  , "TBL" ,  "��������" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.���������ɺ�ɫ��ɺ�ɫʱ����Ϊ����źš�\r\n\
2.���������ɺ�ɫ��ɺ�ɫʱ����Ϊ�����źš���������������������ǣ�\
�����ԭ���������������죬��ˣ��������߲�����K ����Գƣ���֮��Ȼ����һ�����ر�ע�⣡",	1
	} , 
	{	6 , SEPCLINE_TYPE  , "TOW" ,  "������" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.�������ɺ�ɫ��ɺ�ɫʱ����Ϊ����źš�\r\n\
2.�������ɺ�ɫ��ɺ�ɫʱ����Ϊ�����źš�������������������ǣ�\
�����ԭ�������������죬��ˣ������߲�����K ����Գƣ���֮��Ȼ����һ�����ر�ע�⣡",	1
	} , 
};

int	g_nSpeclIndexNum = chDIMOF(g_SpeclIndex);
int	g_nZBWinNum = 3;

////////////////////////////////////////////////////////////////////////
//					TCPIPԶ�̱���
////////////////////////////////////////////////////////////////////////

struct ManyHostInfo g_ManyHostInfo;
struct	CurrStockData_Code g_ZSHQData[10];

BOOL		g_bRiskPost = TRUE;		//�Ƿ��з��ս�ʾ
BOOL		g_bRestRightBtn = FALSE;	//�����������ұ�
BOOL		g_bEveryFirst = FALSE;	//ÿ���ؽ�ϵͳѡ���һ����վ
BOOL		g_bIsGuest = TRUE;
char		g_strLoginName[50];
char		g_strPassword[50];
char		g_L2UserName[50];
char		g_L2Password[50];
int			g_nL2Type = 0;
BOOL		g_bSaveUser = TRUE, g_bSavePass = TRUE;
TDXWHOST	g_HQHost;
TDXWHOST	g_VipHQHost,g_L2HQHost;
TDXWHOST	g_WTHost;
TDXWHOST	g_DSHost,g_SSDSHost;
PROXYINFO	g_ProxyInfo;

CString		g_HQIPStr="",g_HQNameStr="";
unsigned short g_HQPort=7709;

CString		g_DSHQIPStr="",g_DSHQNameStr="";
unsigned short g_DSHQPort=7721;

CString		g_HQTryOutIP="";
unsigned short g_HQTryOutPort=7709;

short	g_nMainID = 0;
short	g_nAssisID = 0;
char	g_nPriority = 1;
char	g_pReceive[ANSBUFFER_LEN+20],g_pReceive3[ANSBUFFER_LEN+20];
ANSHEADER g_AnsHeader,g_AnsHeader3;

short	g_nWideDataMainID = 0;	//���ڿ�����ݶ����ȫ��MainID,ÿ�ε���һ��MainID��1

CWnd *  g_pConnectWnd = NULL;
CSplashWnd * g_pSplashWnd = NULL;
CWnd *  g_pMainWin = NULL;
CWnd *  g_pHisDlg = NULL;
CWnd *  g_pWideDataDlg = NULL;
CWnd *	g_pDownLoadDlg = NULL;
CWnd *	g_pTickZstjDlg = NULL;
CWnd *  g_pDataSyncDlg = NULL;
CWnd *	g_pDSConnectWnd = NULL;
long	g_nLastQuerySeatID = 0;

class CBkData * g_pBkData = NULL;

int		g_nDataSyncType = DATASYNC_NULL;
CString	g_strDataSyncFile = "";

BOOL   g_bFirstStep=TRUE;
char   g_nUrgentNum = 1;


struct HostList g_HqHostList;

LOGININFO g_LoginInfo;

long  g_lRefreshHQSeconds = 0;		//�ϴ�ˢ�������ʱ��
long  g_lRefreshStatHQSeconds = 0;	//�ϴ�ˢ��ͳ�������ʱ��
BOOL  g_bDowning = FALSE;			//�Ƿ���������
BOOL  g_bSelftPHing = FALSE;		//�Ƿ������̺���
BOOL  g_bZnxg_Down = FALSE;			//�Ƿ����ع�������ǣ�Ҫ��������ѡ��

long	g_lQJBgnDate = 0,g_lQJEndDate = 0;
long	g_nCurQJType = 0;
BOOL	g_bHisViewTQ = TRUE;
int		g_nToolBarPos = 0;
short   *g_Simhq2StkIndex = NULL;	//���ڽ���(������ȫ�������Ż�����)
short	g_nSimhq2Stk = 0;
BOOL	g_bBlockModified = FALSE;	//�����Ƿ��޸Ĺ�
short	g_nMineNum[MAXGPNUM];		//�׵���Ŀ������Ʊ��Ŵ��

BOOL	g_bShowHqMine = TRUE;		//�Ƿ������������ʾ��Ϣ��

BOOL	g_bLongHighLine = TRUE;	//�Ƿ��ǳ��ĸ�����

int		g_nNetCondition = 1;//��������

//////////////////////////��ͳ��///////////////////////////
BOOL	g_bHasCaption = FALSE;
int		g_nMenuPeriod = PER_DAY;
int		g_nMenuSortType = 0;
int		g_nMul_WhichGPType = 6;
int		g_nMul_nBlockFlag = -1;
int		g_nMul_CurGP = -1;


int			g_nMenuStack[20];
int			g_nMenuStackTop = 0;

short g_awMenuStartNum[] =
{	0,11,
	11,19, 
	30,6, 
	36,8,
	44,12,
	56,22,
	78,26,
	104,6,
	110,20,
	130,14,
	144,13,
	157,14,
	171,14,
	185,9,
	194,4,
	198,5,
	203,4
};

char g_cwMenuTopic[][80] = 
{
 "���̷���",
 "���۷���",
 "��ʱ����",
 "��������",
 "��������",
 "�ر𱨵�",
 "ѡ�����",
 "��Ѷϵͳ",
 "��ɫ����",
 "ϵͳ����",
 "ѡ�����",
 "ѡ�����",
 "ѡ�����",
 "ר��ϵͳ",
 "ѡ����",
 "Ԥ��ϵͳ"
};

MENUITEMTDX  g_pMenu[] = {
//MENU_TOP
{"1�����̷���",          MENU_DPFX    ,-1},
{"2�����۷���",          MENU_BJFX    ,-1},
{"3����ʱ����",			 MENU_JSFX    ,-1},
{"4����������",          MENU_PERIOD  ,-1},
{"5�����ͬ��",          MENU_MULGP	 ,-1},
{"6���ر𱨵�",          MENU_TBBD    ,-1},
{"7����Ѷϵͳ",          MENU_XXM     ,-1},
{"8������ί��",          MENU_LEAF	 ,L_WT},
{"9����ɫ����",          MENU_SPEC	 ,-1},
{"10.ϵͳ����",          MENU_OTHER   ,-1},
{"11.�˳�ϵͳ",          MENU_LEAF    ,L_QUIT},
//MENU_DPFX
{"1����֤����������"      ,MENU_LEAF    ,L_DPFX_BASE+101},
{"2����֤��ָ����(F3)"   ,MENU_LEAF    ,L_DPFX_BASE+102},
{"3����֤��������"      ,MENU_LEAF    ,L_DPFX_BASE+103},
{"4����֤�¹�����"      ,MENU_LEAF    ,L_DPFX_BASE+104},
{"5����֤���ģ�"        ,MENU_LEAF    ,L_DPFX_BASE+105},
{"6����֤���ָ��"      ,MENU_LEAF    ,L_DPFX_BASE+106},
{"7����֤��������"      ,MENU_LEAF    ,L_DPFX_BASE+114},
{"8����֤����ָ��"		,MENU_LEAF	  ,L_DPFX_BASE+115},
{"9����֤����ָ��"    ,MENU_LEAF	 ,L_DPFX_BASE+118},
{"10.��֤����������"	   ,MENU_LEAF    ,L_DPFX_BASE+107},
{"11.��֤�ɷ�����(F4)"   ,MENU_LEAF    ,L_DPFX_BASE+108},
{"12.��֤��ָ����"      ,MENU_LEAF    ,L_DPFX_BASE+109},
{"13.��֤��������"      ,MENU_LEAF    ,L_DPFX_BASE+110},
{"14.��֤�¹�����"      ,MENU_LEAF    ,L_DPFX_BASE+111},
{"15.��֤���ģ�"        ,MENU_LEAF    ,L_DPFX_BASE+112},
{"16.��֤���ָ��"      ,MENU_LEAF    ,L_DPFX_BASE+113},
{"17.��ҵ��ָ��"      ,MENU_LEAF    ,L_DPFX_BASE+116},
{"18.��С��ҵָ��"    ,MENU_LEAF	 ,L_DPFX_BASE+117},
{"19.�����������"    ,MENU_LEAF	 ,L_DPFX_BASE+119},
//MENU_BJFX
{"1����ѡ��  "      ,   MENU_LEAF         ,L_BJFX_BASE+14},
{"2�������  "      ,   MENU_BJFX_LB      ,-1},
{"3������֤ȯ"      ,   MENU_LEAF         ,L_BJFX_BASE+16},
{"4�����ָ��"      ,   MENU_LEAF         ,L_BJFX_BASE+17},
{"5����Ʊ��  "      ,   MENU_LEAF         ,L_BJFX_BASE+15},
{"6������  "      ,   MENU_BJFX_SELBLOCK,-1},
//MENU_JSFX
{"1����ʱ����ͼ          .501" ,MENU_LEAF,L_JSFX_ZST},
{"2����ʱ�ɽ���ϸ       .502" ,MENU_LEAF,L_JSFX_TICK},
{"3���ּ۱�             .503" ,MENU_LEAF,L_JSFX_FJB},
{"4����ʳɽ���ϸ       .504" ,MENU_LEAF,L_JSFX_FSB},
{"5����ϸ������         .505" ,MENU_LEAF,L_JSFX_MMP},
{"6������/���ָ��      .506" ,MENU_LEAF,L_JSFX_LB},
{"7����������/�ǵ���    .507" ,MENU_LEAF,L_JSFX_MMLD},
{"8����������ͼ         .508" ,MENU_LEAF,L_JSFX_TICKT},

//MENU_PERIOD
{"1����    ��"            ,MENU_INDEX   ,PER_DAY},
{"2����    ��"            ,MENU_INDEX   ,PER_WEEK},
{"3����    ��"            ,MENU_INDEX   ,PER_MONTH},
{"4���� �� ��"            ,MENU_INDEX   ,PER_DAYN},
{"5���� �� ��"            ,MENU_INDEX   ,PER_MIN1},
{"6���� �� ��"            ,MENU_INDEX   ,PER_MIN5},
{"7����������"            ,MENU_INDEX   ,PER_MIN15},
{"8����������"            ,MENU_INDEX   ,PER_MIN30},
{"9����������"            ,MENU_INDEX   ,PER_HOUR},
{"10.�� �� ��"            ,MENU_INDEX   ,PER_MINN},
{"11.��    ��"            ,MENU_INDEX   ,PER_SEASON},
{"12.��    ��"            ,MENU_INDEX   ,PER_YEAR},
//MENU_INDEX
{"1���ͣ��ã�����ָ��"  ,MENU_LEAF  ,L_INDEX_BASE+301 },
{"2���ģͣ�����ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+302 },
{"3���ģͣ�����ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+303 },
{"4���ţأУͣ�����ָ��",MENU_LEAF  ,L_INDEX_BASE+304 },
{"5���ԣңɣ�����ָ��"  ,MENU_LEAF  ,L_INDEX_BASE+305 },
{"6���£ң�������ָ��"  ,MENU_LEAF  ,L_INDEX_BASE+306 },
{"7���ã�����ָ��"      ,MENU_LEAF  ,L_INDEX_BASE+307 },
{"8���֣�����ָ��"      ,MENU_LEAF  ,L_INDEX_BASE+308 },
{"9���ϣ£�����ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+309 },
{"10.���ӣ�����ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+310 },
{"11.�ţͣ�����ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+311 },
{"12.�ף֣�������ָ��"  ,MENU_LEAF  ,L_INDEX_BASE+312 },
{"13.�ңӣ�ǿ��ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+313 },
{"14.�ף���ǿ��ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+314 },
{"15.�ӣ���ͣ��ָ��"    ,MENU_LEAF  ,L_INDEX_BASE+315 },
{"16.�ˣģʳ�����"    ,MENU_LEAF  ,L_INDEX_BASE+316 },
{"17.�ããɳ�����"    ,MENU_LEAF  ,L_INDEX_BASE+317 },
{"18.�ңϣó�����"    ,MENU_LEAF  ,L_INDEX_BASE+318 },
{"19.�ͣɣˣ�ѹ��֧��"  ,MENU_LEAF  ,L_INDEX_BASE+319 },
{"20.�£ϣ̣�ѹ��֧��"  ,MENU_LEAF  ,L_INDEX_BASE+320 },
{"21.�Уӣ�������"      ,MENU_LEAF  ,L_INDEX_BASE+321 },
{"22.�ãģ����Ʋ���"	   ,MENU_LEAF  ,L_INDEX_BASE+322 },
//MENU_TBBD
//{"1������������ʾ"      ,MENU_TBBD_LB     ,0},
{"1���������"          ,MENU_TBBD_LB     ,1},
{"2������ǿ�ƹ�"		   ,MENU_TBBD_LB     ,2},
{"3���������ƹ�"        ,MENU_TBBD_LB     ,3},
{"4���ɽ�������"        ,MENU_TBBD_LB     ,4},
{"5���ܽ������"      ,MENU_TBBD_LB     ,5},
{"6����������"          ,MENU_TBBD_LB     ,6},
{"7��ί������"          ,MENU_TBBD_LB     ,7},
{"8���ּ�����"          ,MENU_TBBD_LB     ,8},
{"9�����´������"      ,MENU_TBBD_LB     ,9},
{"10.��ӯ������"        ,MENU_TBBD_LB     ,10},
{"11.����������"        ,MENU_TBBD_LB     ,11},
{"12.���ʲ�����"        ,MENU_TBBD_LB     ,12},
{"13.���ʲ�����"        ,MENU_TBBD_LB     ,13},	
{"14.ÿ����������"      ,MENU_TBBD_LB     ,14},
{"15.���ʲ�����������"  ,MENU_TBBD_LB     ,15},
{"16.��ͨ��ֵ����"      ,MENU_TBBD_LB     ,16},
{"17.��ͨ�ɱ�����"      ,MENU_TBBD_LB     ,17},
{"18.AB������ֵ����"    ,MENU_TBBD_LB     ,18},
{"19.�ܹɱ�����"        ,MENU_TBBD_LB     ,19},
{"20.ÿ�ɹ���������"    ,MENU_TBBD_LB     ,20},
{"21.��Ծ������"        ,MENU_TBBD_LB     ,21},
{"22.ÿ�ʾ�������"      ,MENU_TBBD_LB     ,22},
{"23.ÿ�ʻ�������"      ,MENU_TBBD_LB     ,23},
{"24.ÿ�ɾ��ʲ�����"	,MENU_TBBD_LB     ,24},
{"25.�о�������"		,MENU_TBBD_LB     ,25},
{"26.ȫ������"          ,MENU_TBBD_LB     ,27},
//MENU_MULGP
{"1����ѡ��  "      ,   MENU_LEAF         ,L_MULGP_BASE+14},
{"2�������  "      ,   MENU_MULGP_LB     ,-1},
{"3������֤ȯ"      ,   MENU_LEAF         ,L_MULGP_BASE+16},
{"4�����ָ��"      ,   MENU_LEAF         ,L_MULGP_BASE+17},
{"5����Ʊ��  "      ,   MENU_LEAF         ,L_MULGP_BASE+15},
{"6������  "      ,   MENU_MULGP_SELBLOCK     ,-1},
//MENU_XXM
{"1��Զ����Ϣ       .701", MENU_LEAF  ,L_XXM_BASE+0},
{"2��Զ����Ϣ       .702", MENU_LEAF  ,L_XXM_BASE+1},
{"3��Զ����Ϣ       .703", MENU_LEAF  ,L_XXM_BASE+2},
{"4��Զ����Ϣ       .704", MENU_LEAF  ,L_XXM_BASE+3},
{"5��Զ����Ϣ	   .705", MENU_LEAF  ,L_XXM_BASE+4},
{"6��Զ����Ϣ       .706", MENU_LEAF  ,L_XXM_BASE+5},
{"7��Զ����Ϣ	   .707", MENU_LEAF  ,L_XXM_BASE+6},
{"8��Զ����Ϣ	   .708", MENU_LEAF  ,L_XXM_BASE+7},
{"9��Զ����Ϣ	   .709", MENU_LEAF  ,L_XXM_BASE+8},
{"10.Զ����Ϣ	   .710", MENU_LEAF  ,L_XXM_BASE+9},
{"11.Զ����Ϣ	   .711", MENU_LEAF  ,L_XXM_BASE+10},
{"12.Զ����Ϣ	   .712", MENU_LEAF  ,L_XXM_BASE+11},
{"13.Զ����Ϣ       .713", MENU_LEAF  ,L_XXM_BASE+12},
{"14.Զ����Ϣ       .714", MENU_LEAF  ,L_XXM_BASE+13},
{"15.Զ����Ϣ	   .715", MENU_LEAF  ,L_XXM_BASE+14},
{"16.Զ����Ϣ       .716", MENU_LEAF  ,L_XXM_BASE+15},
{"17.Զ����Ϣ	   .717", MENU_LEAF  ,L_XXM_BASE+16},
{"18.Զ����Ϣ	   .718", MENU_LEAF  ,L_XXM_BASE+17},
{"19.Զ����Ϣ	   .719", MENU_LEAF  ,L_XXM_BASE+18},
{"20.Զ����Ϣ	   .720", MENU_LEAF  ,L_XXM_BASE+19},
//MENU_SPEC
{"1�����Ű�����",	MENU_LEAF  ,L_RMREPORT},
{"2����ʷ���鱨��",	 MENU_LEAF,L_ZBREPORT},
{"3��ǿ����������",   MENU_LEAF,L_QRREPORT},
{"4�������ǵ�����",   MENU_LEAF,L_QJTYPE1},
{"5�����任������",   MENU_LEAF,L_QJTYPE2},
{"6�������������",   MENU_LEAF,L_QJTYPE3},
{"7�������񵴷���",   MENU_LEAF,L_QJTYPE4},
{"8��ר��ϵͳ",		MENU_EXPSYS  ,-1},
{"9��ѡ����",		MENU_CHOICESYS  ,-1},
{"10.Ԥ��ϵͳ",		MENU_WARNSYS  ,-1},
{"11.��ƽ�����",	MENU_LEAF  ,L_PROFIT},
{"12.��Ʊ��ϼ���",	MENU_LEAF  ,L_BLOCKCALC},
{"13.�����ھ�ο�",	MENU_LEAF  ,L_DATADIG},
{"14.�������",		MENU_LEAF  ,L_INVEST},
//MENU_OTHER
{"1��ϵͳ����",		MENU_LEAF  ,L_OPTION},
{"2�������û����",	MENU_LEAF  ,L_BLOCKOPTION},
{"3��ͨѶ����",		MENU_LEAF  ,L_CONNECTCFG},
{"4���̺���������",	MENU_LEAF  ,L_DOWNLOAD},
{"5������ά������",	MENU_LEAF  ,L_NODETOOL},
{"6������������վ",	MENU_LEAF  ,L_CONNECT},
{"7���Ͽ�������վ",	MENU_LEAF  ,L_DISCONNECT},
{"8��������Ѷ��վ",	MENU_LEAF  ,L_CONNECTINFO},
{"9���Ͽ���Ѷ��վ",	MENU_LEAF  ,L_DISCONNECTINFO},
{"10.ϵͳ����¼",	MENU_LEAF  ,L_COOLINFO},
{"11.�Զ�����",		MENU_LEAF  ,L_AUTOUP},
{"12.����˵����",	MENU_LEAF  ,L_HELP_FINDER},
{"13.�û���̳",		MENU_LEAF  ,L_FORUM},
//MENU_BJFX_LB
{"1����֤����      ",   MENU_LEAF,L_BJFX_BASE+0},
{"2����֤�¹�      ",   MENU_LEAF,L_BJFX_BASE+1},
{"3����֤����      ",   MENU_LEAF,L_BJFX_BASE+2},
{"4����֤�¹�      ",   MENU_LEAF,L_BJFX_BASE+3},
{"5����֤ծȯ      ",   MENU_LEAF,L_BJFX_BASE+4},
{"6����֤ծȯ      ",   MENU_LEAF,L_BJFX_BASE+5},
{"7�������      ",   MENU_LEAF,L_BJFX_BASE+6},
{"8����¹�      ",   MENU_LEAF,L_BJFX_BASE+7},
{"9���ծȯ      ",   MENU_LEAF,L_BJFX_BASE+8},
{"10.�����      ",   MENU_LEAF,L_BJFX_BASE+9},
{"11.������Ʒ      ",   MENU_LEAF,L_BJFX_BASE+10},
{"12.����ָ��      ",   MENU_LEAF,L_BJFX_BASE+11},
{"13.��С��ҵ      ",   MENU_LEAF,L_BJFX_BASE+12},
{"14.�Ȩ֤      ",   MENU_LEAF,L_BJFX_BASE+13},
{"15.��ҵ��        ",   MENU_LEAF,L_BJFX_BASE+14},
//MENU_MULGP_LB
{"1����֤����      ",   MENU_LEAF,L_MULGP_BASE+0},
{"2����֤�¹�      ",   MENU_LEAF,L_MULGP_BASE+1},
{"3����֤����      ",   MENU_LEAF,L_MULGP_BASE+2},
{"4����֤�¹�      ",   MENU_LEAF,L_MULGP_BASE+3},
{"5����֤ծȯ      ",   MENU_LEAF,L_MULGP_BASE+4},
{"6����֤ծȯ      ",   MENU_LEAF,L_MULGP_BASE+5},
{"7�������      ",   MENU_LEAF,L_MULGP_BASE+6},
{"8����¹�      ",   MENU_LEAF,L_MULGP_BASE+7},
{"9���ծȯ      ",   MENU_LEAF,L_MULGP_BASE+8},
{"10.�����      ",   MENU_LEAF,L_MULGP_BASE+9},
{"11.������Ʒ      ",   MENU_LEAF,L_MULGP_BASE+10},
{"12.����ָ��      ",   MENU_LEAF,L_MULGP_BASE+11},
{"13.��С��ҵ      ",   MENU_LEAF,L_MULGP_BASE+12},
{"14.�Ȩ֤      ",   MENU_LEAF,L_MULGP_BASE+13},
{"15.��ҵ��        ",   MENU_LEAF,L_MULGP_BASE+14},
//MENU_TBBD_LB
{"1����֤����      ",   MENU_LEAF,L_TBBD_BASE+0},
{"2����֤�¹�      ",   MENU_LEAF,L_TBBD_BASE+1},
{"3����֤����      ",   MENU_LEAF,L_TBBD_BASE+2},
{"4����֤�¹�      ",   MENU_LEAF,L_TBBD_BASE+3},
{"5����֤ծȯ      ",   MENU_LEAF,L_TBBD_BASE+4},
{"6����֤ծȯ      ",   MENU_LEAF,L_TBBD_BASE+5},
{"7�������      ",   MENU_LEAF,L_TBBD_BASE+6},
{"8����¹�      ",   MENU_LEAF,L_TBBD_BASE+7},
{"9���ծȯ      ",   MENU_LEAF,L_TBBD_BASE+8},
//MENU_EXPSYS
{"1����ʽ������",	 MENU_LEAF,L_FORMULA},
{"2������ѡ�ɲ���",   MENU_LEAF,L_TJTEST},
{"3��ר��ϵͳ����",   MENU_LEAF,L_JYTEST},
{"4��̽�����ָ��",   MENU_LEAF,L_TESTINDEX},
//MENU_CHOICESYS
{"1������ѡ��",	 MENU_LEAF,L_CONDITIONCHOICE},
{"2������ѡ��",   MENU_LEAF,L_SIM_CHOICE},
{"3������ѡ��",   MENU_LEAF,L_CLECHOICE},
{"4�����ѡ��",   MENU_LEAF,L_PLUGIN},
{"5���ۺ�ѡ��",   MENU_LEAF,L_SELGP},
//MENU_WARNSYS
{"1���г��״�����",   MENU_LEAF,L_ALLWARNCFG},
{"2���г��״��б�",	 MENU_LEAF,L_ALLWARNGP},
{"3������Ԥ������",   MENU_LEAF,L_TJWARNCFG},
{"4������Ԥ���б�",	 MENU_LEAF,L_TJWARNGP},

{"",0,0}
};

int	 g_nOverlapNum = 0;								//����ͼ���ӵĸ���
MemStkInfo * g_pOverlapStkInfo[3];					//������������ֻ��Ʊ
BOOL g_bManyDayZST = FALSE,g_bHisManyDayZST = FALSE;
int	 g_nZSTManyDay = 1,g_nHisZSTManyDay = 1;		//��������ͼ������,�������MAX_ZST_MANYDAY��,�����������

BOOL g_nTmpDirectConnectHq = FALSE;

BOOL g_bShowVersoin = TRUE;
char g_strLogoAdvert[200];
BOOL g_bDiffVolMode = 0;			//0:��ɫģʽ1 1:��ɫģʽ2

int g_nTQFlag = 1;	//��Ȩ����	0--�޸�Ȩ 1--ǰ��Ȩ 2--��Ȩ 3--���㸴Ȩ
long g_nPartTq = 0;
long g_nTqStart = 20060101;
int	g_nMainAxis = 0; //��ͼ������
//��ͼ��ʷ����ģʽ
const MAINLINE g_MainLineName[] = {{"K","K��(��������)"},{"KR","K��(ʵ������)"},{"BAR","������"},{"CLOSE","������"},
	{"SCLOSE","����վ��"},{"MTOW","������"}};
const int g_MainLineNum = 6;

WORD    g_virtkey;
WORD    g_comkey;
BOOL    g_hideinfo=FALSE;
BOOL	g_bBossKey=FALSE;

int		g_nMenuMode = 0; //0:��ͳ��� 1:XP���
BOOL	g_bExtendXPStyle = 0; //�Ƿ�����ǿ�͵�XP��� 
BOOL	g_bEnableClassXP = FALSE;
BOOL	g_bClassXP = FALSE;	//

BOOL	g_bMin3Tray = FALSE;

long  g_URLEndTime = 0;		//URL����ʱ��
char  g_URLTitle[81];			//URL���ı���
char  g_URL[81];				//URL

float g_fJiangEnStep = 0.1f;
int   g_nJiangEnMode = 0;		//0:����λ 1:֧��λ
BOOL  g_nCBSwitch = 1;			//�ƶ��������������

unsigned long  g_LoginServerSeconds=0,g_LoginLocalSeconds=0,g_LoginServerDate=0;
long  g_LoginDSServerSubLocal=0;

int	  g_nWebPageNum = 0;	//�����ʾ40��
char  g_strWebPageStr[40][50];
char  g_strWebPageURL[40][150];
char  g_strMainWebTitle[100];

int	  g_nWebPageNum2 = 0;	//�����ʾ20��
char  g_strWebPageStr2[20][50];
char  g_strWebPageURL2[20][150];
char  g_strMainWebTitle2[100];

struct tag_VitalBtn g_VitalBtn;

//�ɷ������˿��ƵĹ���
BOOL  g_bNo003InAG = TRUE;
BOOL  g_bSupportETF = TRUE;
BOOL  g_bSupportLevel2 = FALSE;
BOOL  g_bSupportDownMin5 = TRUE;
BOOL  g_bG2DT = FALSE;
BOOL  g_bUseSpecVolumeUnit = FALSE;	//�Ƿ�ʹ���ر��������
//OEM���ƵĹ���
BOOL	g_bHasInfo = FALSE;
BOOL	g_bHasUserHost = TRUE;
CString g_strCheckUserURL = "";

BOOL	g_bHasDS = FALSE;
BOOL	g_bZBEditable = FALSE;

long	g_nDSAutoStopMinute = 0;
long	g_nDSAutoStopTimer = 0;
long	g_nDSCheckHostMinute = 0;

BOOL  g_bLongDSName = FALSE;
BOOL  g_bHasWebInfo = FALSE;
BOOL  g_bHasEmbWT = FALSE;
BOOL  g_bUserHostEverConnect = FALSE;
BOOL  g_bHasAdvHQVersion = FALSE;
int	  g_nPersonalVersion=0;
BOOL  g_bJRSConnected=FALSE;	//�Ƿ����û�������(ǰ����VIP)��������
BOOL  g_bHasTICQ = FALSE;
BOOL  g_bAutoConnectTICQ = FALSE;
BOOL  g_bTQAsBroker = FALSE;

char *SwitchUnit_ZstZS[3] = {"�ɽ���","���ָ��","�ǵ���"};
char *SwitchUnit_ZstGP[4] = {"�ɽ���","����","��������","����ͼ"};
char *SwitchUnit_ZstL2[11] = {"�ɽ���","����","��������","����ͼ","��������","���Ҿ���","�����ۼ�","��������","�󵥶���","�ǵ�����","�󵥲��"};

char *SwitchUnitStr[4] = {"��","��","��","��"};
char *SwitchUnitStr_Tips[4] = {"�ʽ���","��ʱ�ɽ���ϸ","�����춯","�ּ۱�"};
int	  SwitchUnitNum = 4;
char *SwitchUnitStr_Mid[7] = {"����","�ֲ�","����","����","�ܳ�","��ί","��ҵ"};
char *SwitchUnitStr_Mid_Tips[7] = {"���п��ܳɽ���ǰ50��ί�е�������Ϣ\r\n˫���������Ŵ�,�ٴ�˫������","��ǰ������ί�зֲ�","�����ʳɽ��ĸ���ɽ�ͳ����Ϣ","��ǰ������������10����/������\r\n˫���������Ŵ�,�ٴ�˫������","��������/����ί���ۼ���������ǰ10ֻ��Ʊ\r\n˫���������Ŵ�,�ٴ�˫������","ί������/������������ǰ10ֻ֤ȯ\r\n˫���������Ŵ�,�ٴ�˫������","��ҵ�ɶ�����,��ҵ�ڳɽ����ǰ5λ����\r\n˫���������Ŵ�,�ٴ�˫������"};
int	  SwitchUnitNum_Mid = 7;

int   RsaBitsInt[5] = {128,256,512,768,1024};
int   KeyLenInt[5] =  {8,16,32,64,128};

//////////////////////��ҵ����ƽ̨�ı���/////////////////////////////////

BOOL  g_bHasF10Cfg = TRUE;

BOOL    g_bConnectInfoStatus = FALSE;
struct f10cfg_title g_F10Caption[MAX_F10CAPTION];
struct new_f10cfg_title g_NewF10Caption[MAX_F10CAPTION];
char	g_F10CaptionNum = 0;
BOOL	g_bNLB = TRUE;
BOOL	g_bNLBOnLogo = TRUE;
struct NLBInfo g_NLBInfo[150];
BOOL	g_bWtUserVerifyStyle = 0;
BOOL	g_bHexinWtOnlyTXMM = FALSE;
short	g_WtVerifyIPNum = 0;
struct IpInfo *g_WTVerifyIP = NULL;
struct VipVerify_Info g_VipVerify_Info;
BOOL g_bAuthSuccess = FALSE;
BOOL g_bWTEverLogin = FALSE;	//�����Ƿ�������¼��

BOOL g_bHintSwitch = TRUE;
BOOL g_bRandSwitch = FALSE;
BOOL g_bNetErrorReLogin = TRUE;

struct ETInfo			g_ET;
struct Global_Habit		g_GH;
struct Global_Cfg		g_GC;
struct Global_EmbOem	g_OEM;
struct Global_Url		g_Url;
struct Global_SysInfo	g_SysInfo;
struct Global_DSCfg		g_DS;
struct JyCookie_Info	g_JyInfo;
struct AntiHack_Cfg		g_AntiHack;
struct NetParam_Cfg		g_NetParam;

BOOL g_bForbidOtherHost=FALSE;
BOOL g_bUseKList=FALSE;	//��F1�Ƿ�������߱���
CString g_PersonalPath;
////////////////////����ƽ̨////////////////////////////////

CHATAREA g_ChatArea;
	
long	g_nTreePos[20];
long	g_nRestPos[20];

char	g_UserHostIP[50]={0},g_BakUserHostIP[50]={0},g_BakUserHostIP2[50]={0};
unsigned short g_nUserHostPort = 7715,g_nBakUserHostPort = 7715,g_nBakUserHostPort2 = 7715;

char g_FHostIP[50]={0};
unsigned short g_nFHostPort = 7719;

char	g_UserHostQSID[11]={0};		//	�ǳ������û������������ȯ��ID
CString	g_strTdxVerifyFileName="winwt.exe";

char	g_MngRight = 0;				//	����Ȩ��,0������ 1����ͨ�û� 2��VIP�û� 3�������û� 4���о�Ա 5�������û�/�ܱ�
char	g_UserType = 0;				//	�û�����,0=�ͻ� 1�������� 2�����ǿͻ����Ǿ�����
char	g_strUserType[9] = {0};		//  �û����ʹ�
char	g_strUserCharacter[9] = {0};//  �û����ʴ�
char	g_strUserLimits[20] = {0};	//  �û�Ȩ�޴�

DWORD	g_CategoryInfo = 0;			//	��������Ϣ����,Ϊһλ�νṹ
char	g_RightStr[255] = {0};		//	�û�Ȩ�޴�
long	g_ExpireDate = 0;
long 	g_UrgentNum = 0;

CString SwitchCurGPCode = "600839";
short	SwitchCurSetCode = SH;

BOOL	g_bActTogether = TRUE;
BOOL	g_bFxtShowInfoDlg = FALSE;
BOOL	g_bFxtShowTab = FALSE;
BOOL	g_bTranparentTab = TRUE;

int l_YYBNum=0;
int l_PrimaryYYB=0;
struct tag_YYBInfo *l_pYYBInfo = NULL;

BOOL g_b2000XPSys=TRUE;
BOOL g_bUseBigJBM=FALSE;
BOOL g_bCanUserUrgent = FALSE;
BOOL g_bHasBakHostFile = FALSE;
BOOL g_bHasZXQY=TRUE;	//���������Ƿ���С��ҵ��
BOOL g_bCanDown = TRUE;
BOOL g_bLastRqDown = FALSE;
BOOL g_nReConnectNum=0; //������
int  g_nLogoTimeOut=8;	//��¼�����еĳ�ʱ����
int  g_nCusWebPageNum=0;
CUSWEBPAGE g_CusWebPage[MAX_CUSWEB_NUM];
CUSWEBPAGE g_F11WebPage;
float	l_aRatio[RATIONNUM]={0.0};//��Ų�ͬͼ�ı�����ϵ

/****************************************************
//	�������г���ȫ�ֱ���
****************************************************/
char			g_pNbbs_DS[10]={0};
SRVINFOANS		g_iDsSrvInfo={"",0,0,0,0,0};	//	��ȡ����վ��Ϣ
BOOL			g_bDSNeedIdx = FALSE;
BOOL			g_bDSNeedTimer = FALSE;
int				g_GuiMarketNum=0;
int				g_LocalGuiMarketNum=0;
GUIMARKET		g_GuiMarketSet[MAX_MKTGUI_NUM];
short			g_nDSMarketNum = 0;
MKTINFO			g_pDSMarket[MAX_MKT_NUM]={0};
  
////////////////////////////////////////////////////////////////////////
CGNPlugin	g_GNPlug;

GRIDCATITEM	g_SysDefGridCat[12]=
{
	{ABDEF_ITEM, "��ѡ��", WM_COMMAND, ID_RIGHT_TYPE15, 0, FALSE, 15, 0},
	{ABDEF_ITEM, "�������", WM_COMMAND, ID_RIGHT_TYPE6, 0, FALSE, 6, 0},
	{ABDEF_ITEM, "��С��ҵ", WM_COMMAND, ID_RIGHT_TYPE12, 0, FALSE, 12, 0},
	{ABDEF_ITEM, "��ҵ��", WM_COMMAND, ID_RIGHT_TYPE14, 0, FALSE, 14, 0},
	{ABDEF_ITEM, "����Ȩ֤", WM_COMMAND, ID_RIGHT_TYPE13, 0, FALSE, 13, 0},
	{ABDEF_ITEM, "�������", WM_COMMAND, ID_RIGHT_TYPE9, 0, FALSE, 9, 0},
	{ABDEF_ITEM, "����ծȯ", WM_COMMAND, ID_RIGHT_TYPE8, 0, FALSE, 8, 0},
	{ABDEF_ITEM, "����¹�", WM_COMMAND, ID_RIGHT_TYPE7, 0, FALSE, 7, 0},
	{ABDEF_ITEM, "����ָ��", WM_COMMAND, ID_RIGHT_TYPE11, 0, FALSE, 11, 0},
//	{ABDEF_ITEM, "���ָ��", WM_COMMAND, ID_RIGHT_TYPE20, 0, FALSE, TYPE_LC, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, IDR_LONELY_MENU, 16, TRUE, -1, 0},
	{ABDEF_ITEM, "��ҵ", WM_COMMAND, IDR_LONELY_MENU, -5, TRUE, -1, 0},
	{ABDEF_ITEM, "����ѡ��", WM_COMMAND, IDR_LONELY_MENU, -2, TRUE, -1, 0},
};

GRIDCATITEM	g_SysDefGridCat_Com2[12]=
{
	{ABDEF_ITEM, "��ѡ", WM_COMMAND, ID_RIGHT_TYPE15, 0, FALSE, 15, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, ID_RIGHT_TYPE6, 0, FALSE, 6, 0},
	{ABDEF_ITEM, "��С", WM_COMMAND, ID_RIGHT_TYPE12, 0, FALSE, 12, 0},
	{ABDEF_ITEM, "��ҵ", WM_COMMAND, ID_RIGHT_TYPE14, 0, FALSE, 14, 0},
	{ABDEF_ITEM, "Ȩ֤", WM_COMMAND, ID_RIGHT_TYPE13, 0, FALSE, 13, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, ID_RIGHT_TYPE9, 0, FALSE, 9, 0},
	{ABDEF_ITEM, "ծȯ", WM_COMMAND, ID_RIGHT_TYPE8, 0, FALSE, 8, 0},
	{ABDEF_ITEM, "�¹�", WM_COMMAND, ID_RIGHT_TYPE7, 0, FALSE, 7, 0},
	{ABDEF_ITEM, "ָ��", WM_COMMAND, ID_RIGHT_TYPE11, 0, FALSE, 11, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, IDR_LONELY_MENU, 16, TRUE, -1, 0},
	{ABDEF_ITEM, "��ҵ", WM_COMMAND, IDR_LONELY_MENU, -5, TRUE, -1, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, IDR_LONELY_MENU, -2, TRUE, -1, 0},
};

GRIDCATITEM	g_SysDefGridCat_All[16]=
{
	{ABDEF_ITEM, "����", WM_COMMAND, ID_RIGHT_TYPE6, 0, FALSE, 6, 0},
	{ABDEF_ITEM, "��С", WM_COMMAND, ID_RIGHT_TYPE12, 0, FALSE, 12, 0},
	{ABDEF_ITEM, "��ҵ", WM_COMMAND, ID_RIGHT_TYPE14, 0, FALSE, 14, 0},
	{ABDEF_ITEM, "�¹�", WM_COMMAND, ID_RIGHT_TYPE7, 0, FALSE, 7, 0},
	{ABDEF_ITEM, "Ȩ֤", WM_COMMAND, ID_RIGHT_TYPE13, 0, FALSE, 13, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, ID_RIGHT_TYPE9, 0, FALSE, 9, 0},
	{ABDEF_ITEM, "ծȯ", WM_COMMAND, ID_RIGHT_TYPE8, 0, FALSE, 8, 0},
	{ABDEF_ITEM, "��ָ", WM_COMMAND, ID_RIGHT_TYPE17, 0, FALSE, 17, 0},	
	{ABDEF_ITEM, "����", WM_COMMAND, ID_RIGHT_TYPE20, 0, FALSE, 20, 0},	
	{ABDEF_ITEM, "��ѡ", WM_COMMAND, ID_RIGHT_TYPE15, 0, FALSE, 15, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, IDR_LONELY_MENU, 16, TRUE, -1, 0},
	{ABDEF_ITEM, "��ҵ", WM_COMMAND, IDR_LONELY_MENU, 17, TRUE, -1, 0},
	{ABDEF_ITEM, "֤�����ҵ", WM_COMMAND, IDR_LONELY_MENU, -5, TRUE, -1, 0},
	{ABDEF_ITEM, "����", WM_COMMAND, IDR_LONELY_MENU, -1, TRUE, -1, 0},
	{ABDEF_ITEM, "���", WM_COMMAND, IDR_LONELY_MENU, -2, TRUE, -1, 0},
	{ABDEF_ITEM, "�Զ�", WM_COMMAND, IDR_LONELY_MENU, -3, TRUE, -1, 0}
};

long g_nSysDefGridCatNum=12,g_nSysDefGridCat_All=15;

vector<GRIDCATITEM>	g_CustomGridCat;

SPECMENUCOLOR	g_SpecMenuColor[] = 
{
	{ID_KQS,"",RGB(255,0,0)},
	{IDC_QZSDS,"",RGB(255,0,0)},
	{ID_GPGRADE_DLG,"",RGB(255,0,0)},
	{IDC_F11WEBPAGE,"",RGB(255,0,0)},
	{IDC_HKEYPROGRAM,"",RGB(255,0,0)},	
	{ID_HKSEATQUERY,"",RGB(255,0,0)},	
	{ID_MARK_ADD7,"",RGB(255,0,0)},	
	{ID_508,"",RGB(255,0,0)},
	{ID_RIGHT_ADDZXG,"",RGB(255,0,0)},

	{IDM_BUYDS,"",RGB(192,0,0)},

	{0,"���ư���",RGB(255,0,0)},
	{0,"",RGB(255,0,0)}
};

int g_nSpecMenuColor = chDIMOF(g_SpecMenuColor);

struct ETF_Info g_ETFInfo[]=
{
	{SH, "510050","��֤50  ETF","510051","000016"},
	{SH, "510180","��֤180 ETF","510181","000010"},
	{SH, "510880","��֤����ETF","510881","000015"},
	{SZ, "159901","��֤100 ETF","159901","399004"},
	{SZ, "159902","��С��  ETF","159902",ZXB_DPCODE}
};
int g_nETFInfoNum = chDIMOF(g_ETFInfo);

Status_GP	g_StatusGP[3];

long		g_nNetInCount = 0;
long		g_nNetOutCount = 0;

vector<PWARNINGCND> g_aSelfWarnGp;

//�������Ȩ������
short		g_nHZQZFlag = 0;		//0-ƽ�� 1-��ͨ�� 2-�ܹɱ�

int			g_nZHStyle = 1;
int			g_nZBReportSortIndex = 2;

BOOL		g_bAutoHideBar = FALSE;
CMyToolBar	*g_pMyToolBar = NULL;
BOOL	g_bShowBigToolBar = TRUE;

char		*g_strGroup[MAX_GROUPMARK] = {"��","��","��","��","��","��","T"};


vector<FY_GROUP_INFO> g_vfyGroup;
BOOL g_bFYDataAlwaysSync=FALSE;
struct FY_LOCATION g_FyLocation;

BOOL g_bTopTabWorkSpace=FALSE;
BOOL g_bEmbWTCreated=FALSE;
struct ADVERT_INFO g_AdvertInfo;
struct NewJy_CFG g_NewJyCfg;

struct Server_MarketInfo g_Mtl;

CString	g_strTendName="tend_std";
CString g_strTendCfg="tend_std.cfg";
CString g_strNeedAdName="needad.dat";
CString g_strTdxInfoIniName="kninfo.ini";

BOOL	g_bSaveZH=TRUE,g_bProtectZH=FALSE;
HWND	g_hSoftKeyWnd=NULL;	//����̵Ĵ��ھ��
HWND	g_hHXInfoWnd=NULL;	//������Ѷ��֤���ڵľ��
OverlapInfo	g_Overlap[2];

BOOL	g_bDebugMode=FALSE;
BOOL	g_bChangeDebugCode=FALSE;

HisTickCacheHead	g_HisTickCacheHdr[MAX_HISCACHE];
int					g_nNextHTCHPos = 0;

CToolTipMark		*g_pToolTipMark = NULL;
BOOL				g_bFixedXAxis=FALSE;
short				g_nFixedXAxis=20;
int					g_nDefaultZxWidth=214;
int					g_nDefaultRestHeight=250;

KeyGuyCNInfo g_KeyGuyCnInfo={0};
//BI SERVER����
char	g_BIHostIP[50],g_BakBIHostIP[50];
unsigned short g_nBIHostPort = 7717,g_nBakBIHostPort = 7717;

int					g_nFlushRealInfo=0;
int					g_nCurLevel2Tab=0;
BOOL				g_bNewJyUseAdvHQHost=FALSE;
//�۹���ҵ
vector<FL_CodeName>	g_SectorCN;

//////////////////////////////////////////////////////////////////////////
// �������ƽ̨
char lpszJJItemName[][20] = 
{
		"����",
		"��������",
		"��������",
		"��ֵ",
		"���ھ�ֵ",
		"�ۼƾ�ֵ",
		"�ܷݶ�(��)",
		"��ֵ�ܶ�(��)",
		"��ֵ����(��)",
		"��Ʊ��",
		"������ϯ��",
		"��ϯ��ֵ(��)",
		"����˾����",
		"Ͷ������",
		"Ͷ������"
};
float lpnOJJItemWidth[] = {7,17,9,7,9,9,11,12,12,7,12,13,20,15,15};
float lpnCJJItemWidth[] = {7,10,9,7,9,9,11,12,12,7,12,13,20,15,15};

char	lpszhbItemName[][20] = 
{
	"����",
	"��������",
	"��������",
	"�������",
	"�����������",
	"7����������",
	"�ܷݶ�(���)",
	"��ֵ�ܶ�(��)",
	"��ֵ����(��)",
	"����˾����",
	"Ͷ������"
};
float	lpnhbItemWidth[] = {7,15,9,9,13,12,12,12,12,19,12};

char lpszJJStockName[][20] = 
{
		"����",
		"��Ʊ����",
		"�زֻ������",
		"�ϼƳֹ�(��)",
		"�ϼƼ���(��)",
		"ռ���й�˾",
		"�ϼ���ֵ(��)"
};
float lpnJJStockWidth[] = {8,9,14,12,12,12,12};
pFUNDBASE		g_pFundBase = NULL;
int				g_nFundNum = 0;
pFUNDHOLDSTOCK	g_pFundHoldStock = NULL;
int				g_nHoldNum = 0;
BOOL			g_bFundDataAlwaysSync = FALSE;
//////////////////////////////////////////////////////////////////////////

struct WriteUrgentInfo *g_pWriteUrgent=NULL;
//////////////////////////////////////////////////////////////////////////
DWARNINF g_DWarnInf;
//////////////////////////////////////////////////////////////////////////
//��Comview2��������
char **		g_ppHqStyleStr;
char*		nullPadName = "tmp";
time_t		tmp_SetGPtime;

map<char*, int, cmpstr>			g_mUnitType;
map<char*, int, cmpstr>			g_mUnitStyle;
map<UINT, int, greater<UINT> >	g_mUnitMenu;

char		g_LayerPath[MAX_PATH]={0}, g_ContentPath[MAX_PATH]={0};
//�Ƿ�֧�ָ����Level2����
BOOL		g_bSupportDDE = FALSE;
CImageList	g_BtnImage,g_GridImage;

//TopViwe��ع���
BOOL			g_bHasTopView = FALSE;
BOOL			g_bTopViewStatus = FALSE;

char			g_TVHostIP[50] = {0};
unsigned short	g_nTVHostPort = 4010;
short			g_nMyProvineID = 0;
char			*g_pProvinceStr[35]={"����","����","�Ϻ�","���","����","����","����","����","�㶫","����",
									 "����","����","�ӱ�","����","������","����","����","����","����","����",
									 "����","���ɹ�","����","�ຣ","ɽ��","ɽ��","����","�Ĵ�","�½�","����",
									 "����","�㽭","̨��","���","����"};

//�۹ɴ�����
map<long, Seat_Code2Com, greater<long> > g_mSeat2Com;
map<long, Seat_Com2Name, greater<long> > g_mSeatCode2Name;

BOOL			g_bTickTStatus = FALSE;
BOOL			g_bShowCustomPadSwitch = TRUE;
BOOL			g_bHasHomePage = FALSE;	//����������������ҳģʽ
CString			g_InitDefHomePage = "";

int				g_nTopDockTop = 24;

map<CString, ShortCut_Cfg, cmpstr > g_mShortCut;

char			g_strYxhjCfg[MAX_PATH] = "100nian.dat";
char			g_strUserCfg[MAX_PATH] = "VipUser.dat";
char			g_strBlockNew[MAX_PATH] = "selfbk\\";

//��ʱ�������Ϣ
time_t	g_LastTimeTag = 0;
vector<URGENTINFO>	g_aUrgentInfo;

long					g_nAutoCalcLock = 0;
BOOL					g_bShow3DKLine = TRUE;

short					g_nUseDNSUrl = 0;
//!!!
char					*g_strDNSUrl[2] = {"", ""};
char					*g_strTwinserverUrl = "60.169.75.45";
//char					*g_strTwinserverUrl = "127.0.0.1";

CPreviewUnitDlg *g_pPreviewUnitDlg = NULL;
CHqSrcMgn *g_pHqSrc=NULL;
vector<MemStkInfo>			head[2];

map<long, long, greater<long> > g_DBIndexMap;
CDBFile *g_pHQDBFile = NULL;

long	g_nCountCalcBlockTJ = 1;
long	g_nCalcBlockIndex = 0;

long	g_nLastValidDate = 0;
short	g_nLastValidValue = 1;
long	g_nPopInfoLastTime = 0;

Scheme_King		g_Scheme_King;

char	*g_strCalc1 = 
"A1:=MAX(MA(C,60),MA(C,120));\r\n\
B1:=MIN(MA(C,60),MA(C,120));\r\n\
N1:=MA(C,60);\r\n\
M1:=MA(C,120);\r\n\
N2:=MA(C,60);\r\n\
M2:=MA(C,120);\r\n\
������:CLOSE<=M1 AND CLOSE>=N1;\r\n\
ţ����:C>A1;\r\n\
������:CLOSE<=N2 AND CLOSE>=M2;\r\n\
A:=MAX(MA(C,60),MA(C,120));\r\n\
B:=MIN(MA(C,60),MA(C,120));\r\n\
�ܹ���:C<=B;\r\n\
AA:=EMA(CLOSE,10)>REF(EMA(CLOSE,10),1);\r\n\
BB:=EMA(CLOSE,10)<REF(EMA(CLOSE,10),1);\r\n\
����:IF(AA,1,IF(BB,2,0));\r\n\
AAA:=EMA(CLOSE,30)>REF(EMA(CLOSE,30),1);\r\n\
BBB:=EMA(CLOSE,30)<REF(EMA(CLOSE,30),1);\r\n\
����:IF(AAA,1,IF(BBB,2,0));";

char	*g_strCalc2 = 
"A:=MA(C,5);\r\n\
A1:=MA(C,10);\r\n\
A2:=MA(C,20);\r\n\
A3:=MA(C,60);\r\n\
A4:=MA(C,120);\r\n\
A5:=REF(C,1)<REF(A,1) OR REF(C,1)<REF(A1,1) OR REF(C,1)<REF(A2,1) OR REF(C,1)<REF(A3,1) OR REF(C,1)<REF(A4,1);\r\n\
A6:=C>A AND C>A1 AND C>A2 AND C>A3 AND C>A4;\r\n\
B1:=(VOL/CAPITAL)>0.04 AND (VOL/CAPITAL)<0.11;\r\n\
B2:=CLOSE/REF(CLOSE,1)>1.02;\r\n\
��������:=A5 AND A6 AND B1 AND B2;\r\n\
ZJ:=(O+H+L+C)/4;\r\n\
YZ:=IF(BARSCOUNT(C)>60,VOL/SUM(VOL,60),VOL/SUM(VOL,BARSCOUNT(C)));\r\n\
CYC12:=DMA(ZJ,YZ/0.125);\r\n\
CYC25:=DMA(ZJ,YZ/0.25);\r\n\
CYC50:=DMA(ZJ,YZ/0.5);\r\n\
CYC0:=DMA(ZJ,YZ);\r\n\
ϴ�̽���:=FILTER(COUNT(VOL/CAPITAL<0.015,10)>=1 \r\n\
AND (MA(VOL/CAPITAL,10)<0.025 OR COUNT(VOL<MA(VOL,10),10)>=7) \r\n\
AND CLOSE>=HHV(CLOSE,10) \r\n\
AND COUNT(CLOSE<CYC0,20)<=5 \r\n\
AND CLOSE/CYC12<1.1 \r\n\
AND CLOSE/CYC0<1.3 \r\n\
AND LAST(CYC50>CYC0,30,0),10);\r\n\
��ͣ�ȷ�:=CLOSE/REF(CLOSE,1)>=1.099 \r\n\
AND AMOUNT>50000000 \r\n\
AND  VOL/CAPITAL>=0.04 \r\n\
AND 3*VOL/SUM(V,3)>1.5 \r\n\
AND CLOSE/REF(CLOSE,20)<1.6 \r\n\
AND CAPITAL<5000000;\r\n\
������̧:=C>=CYC12 \r\n\
AND CYC12>CYC25 AND CYC25>CYC50 \r\n\
AND INDEXC/REF(INDEXC,1)<0.975 \r\n\
AND C/REF(C,1)>1.03 \r\n\
AND VOL/CAPITAL>0.05;\r\n\
VAR3:=(MA(AMOUNT,5)/MA(VOL,5)/100-MA(AMOUNT,125)/MA(VOL,125)/100)/(MA(AMOUNT,5)/MA(VOL,5)/100);\r\n\
VAR4:=MA((LLV(LOW,45)-CLOSE)/(HHV(HIGH,45)-LLV(LOW,45))*100,3);\r\n\
���ط���:=CROSS(-5,VAR4) AND VAR3<-0.4;\r\n\
��ɫ:�������� AND ϴ�̽��� AND ��ͣ�ȷ�;\r\n\
��ɫ:�������� AND ϴ�̽��� AND ��ͣ�ȷ�;\r\n\
��ɫ:��ͣ�ȷ�;\r\n\
��ɫ:���ط���;\r\n\
����:������̧;\r\n\
��������:CROSS((C+REF(L,1))/2,EMA((H+O+L+C)/4,10));\r\n\
��������:CROSS((C+REF(L,1))/2,EMA((H+O+L+C)/4,30));";

char	*g_strCalc3 = 
"N:=MA(C,60);\r\n\
M:=MA(C,120);\r\n\
��:=CLOSE>N AND CLOSE>M;\r\n\
��:=CLOSE<M AND CLOSE>N;\r\n\
��:=CLOSE>M AND CLOSE<N;\r\n\
��:=CLOSE<N AND CLOSE<M;\r\n\
IF(��,1,IF(��,2,IF(��,3,IF(��,4,0))))";
//
CCriticalSection	g_DXIndexCalc;
CCriticalSection	g_HisDataReader;
BOOL	g_bAllLocalData = FALSE;
BOOL	g_bTickStatReaded = FALSE;

CCriticalSection	g_CalcIniLock, g_PoloReqSec;
map<long, float, greater<long> > g_mTolStat;
