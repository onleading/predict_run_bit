#ifndef JY_H_
#define JY_H_


enum LOGIN_HQWTTYPE
{
	LOGIN_HQWT		= 0,	//行情+交易登录
	LOGIN_ONLYHQ	= 1,	//独立行情
	LOGIN_ONLYWT	= 2,	//独立交易
};

enum JYSTATUS
{
	JY_LOGINED	=  0x00000001,//是否是登录状态
	JY_LOCKING	=  0x00000002,//是否是锁定状态
	JY_RECVING	=  0x00000004,//是否是运行状态(收发数据过程中)
};

#define MAX_WTTYPE	29

#define SZAG		0
#define SHAG		1
#define SZBG		2
#define SHBG		3
#define SZGZ		4
#define SHGZ		5
#define DFGT		6
#define QHWT		7
#define SHHY		8
#define KHWT		9
#define SHWT		10
#define CYWT		11
#define SAWT		12
#define SBWT		13
#define SBGB		14
#define KFJJ		15
#define SZHG		16
#define SHHG		17
#define ZBZH		18		// 主板帐户
#define SBZH		19		// 三板帐户
#define QHZH		20		// 期货帐号
#define ZZQH		21		//
#define SHQH		22
#define DLQH		23
#define KHKH		24		// 客户卡号
#define YHWT		25		// 银行方式登录	
#define UNUSED1_WT  26		// 
#define UNUSED2_WT  27		// 
#define ZCYH		28		// 注册用户

struct	wtdef_new_info
{	
	short	scnum;					//所有市场个数
	short	yh_scnum;				//银行市场个数
	short	sc[100];				//市场序号
	char	scname[100][32];		//市场名称
	char	bankcode[100][8];		//银行代码
	char	bankyyb[100][8];		//银行营业部
	char	bankgdtype[100][8];		//银行股东类别－市场
	char	bShowSSL;				//是否显示SSL方式
	char	bAuthPass;				//是否支持认证口令
	char	bTDXCA;					//是否支持CA证书
	short	yybnum;					//营业部列表数
	char	yyb_tdxid[300][6];		//营业部通达信ID
	char	yyb_desc[300][35];		//营业部描述
	short	yyb_bhfs[300];			//营业部补号方式
	char	nyybType[300];			//营业部类型 0 非期货; 1 期货营业部 
	int		nyyb_clitype[300];		//营业部客户端类型(ClientType)
	char	yyb_noprefix[300];		//在ZJZHMaxLen有值的情况，是否将营业部前缀补在前面，缺省为0
	char	bSwapCombo;				//是否调换Combo的位置
	char	nColorYzm;				//是否是彩色的验证码
	int		ZJZHMaxLen;				//资金帐号的最大长度,如果不够此长度,在前面补营业部编号
	char	bMustDigitPwd;			//登录界面上的交易密码必须是数字
	int		nMustAccountLen;		//资金帐号必须是多大,为0表示不限制
	int		nMaxAccountLen;			//资金帐号输入框中的最大长度
	char	AqfsCombo;				//有没有安全方式的Combo,如果没有并且只有一个通达信SSL,则隐藏
	char	UrlApplyAuthPass[255];	//券商网页申请口令连接URL
	char	UrlModifyAuthPass[255];	//券商网页修改口令连接URL
};

typedef struct tag_InputZhInfo
{
	short nZhType;
	char  szBankCode[8];
	char  szBankYyb[8];
	char  szBankGdType[8];
	char  ZhInfo[31];
	char  Password[31];
	char  SecureType;
	char  SecurePassword[31];
	char  yyb_tdxid[6];
}InputZhInfo; 

//本地cookie (TQCookie.dat文件内容)
struct Zjzh_Cookie
{
	char	qsid[11];
	char	yybinfo[21];	
	char	zjzh[41];
	long	exploredate;
	char	LocalPath[91];
	char	authchar1;
	char	authchar2;
	char	unused1[4];
	long	writedate;
	short	khqx;
	char	vipuserflag;
	char	canqslevel2;
	char	canadvjy;
	char	canjgb;
};

//本程序使用的交易交换信息
struct JyCookie_Info
{
	char	qsid[11];
	char	zjzh[41];
	WORD	yybid;
	char	khqx;
	char	vipuserflag;
	char	canqslevel2;
	char	canadvjy;
	char	canjgb;
};

struct ZxCache_Info
{
	long	productid;
	char	flag;
	char	unnused[11];
};

struct VSatZx_Info
{
	long	m_lYBGPnum;			// 研报检索每天可以查询几支股票
	long	m_lYBZXnum;			// 研报检索一支股票返回几条记录
	long	m_lCCGPnum;			// 持仓报告每天可以查询几支股票
	long	m_lCCZXnum;			// 持仓报告一支股票返回几条记录
	long	m_lXXSDGPnum;		// 信息速递每天可以查询几支股票
	long	m_lXXSDZXnum;		// 信息速递一支股票返回几条记录
};

struct CcCache_Info
{
	char	m_szZZQDM[SH_CODE_LEN+1];	// 证券代码长度为7字节
	long	m_lZQSC;					// 证券市场
	double	m_lZQSL;					// 证券数量
	double	m_lKMSL;					// 可卖数量
	float	m_CBJ;						// 当前成本
	char	m_szUnUsed[65];				// 保留字段
};

struct OtherCache_Info					//必须是8的倍数
{
	char	LastRightStr[255];			// 上次的权限串
	char	m_szUnUsed[545];			// 保留字段
};

#define	MAX_SAVE_ZHNUM			20

#define MAX_ZXCOOKIE_NUM		1000
#define MAX_CCCOOKIE_NUM		100

extern  int						g_nEmbedTabSel;
extern  BOOL					g_bSaveEmbedTabSel;
extern  int						g_nLastZhType;
extern  int						g_nLastAqfs;
extern  int						g_nLastLoginType;
extern  char					g_strLastZhInfo[31];
extern  char					g_strSaveZhInfo[MAX_SAVE_ZHNUM][31];
extern  int						g_nSaveZhNum;
extern  int						g_nLastYYBSel;
extern  char					g_strLastYYBID[6];

extern  char					wtbt[MAX_WTTYPE][15];
extern  struct	wtdef_new_info	wtdef;

extern  void GetWTDef();
extern  BOOL ReadTCCookie(struct JyCookie_Info *pJyInfo);
extern  int  ReadZXCookie(struct ZxCache_Info *pZxInfo,int nMaxNum,BYTE *pZXSecurePackage);
extern  int  ReadCCCookie(struct CcCache_Info *pCcInfo,int nMaxNum);

extern  BOOL LoadOtherCookie(struct OtherCache_Info *pInfo);
extern  BOOL MakeOtherCookie(struct OtherCache_Info *pInfo);

#define SCLV_SSL		1				// SSL帐户验证方式
#define SCLV_AUTHPASS	2				// 帐户验证+认证口令方式
#define SCLV_CA			4				// 帐户验证+认证证书方式

#endif
