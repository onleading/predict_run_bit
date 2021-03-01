#ifndef JY_H_
#define JY_H_


enum LOGIN_HQWTTYPE
{
	LOGIN_HQWT		= 0,	//����+���׵�¼
	LOGIN_ONLYHQ	= 1,	//��������
	LOGIN_ONLYWT	= 2,	//��������
};

enum JYSTATUS
{
	JY_LOGINED	=  0x00000001,//�Ƿ��ǵ�¼״̬
	JY_LOCKING	=  0x00000002,//�Ƿ�������״̬
	JY_RECVING	=  0x00000004,//�Ƿ�������״̬(�շ����ݹ�����)
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
#define ZBZH		18		// �����ʻ�
#define SBZH		19		// �����ʻ�
#define QHZH		20		// �ڻ��ʺ�
#define ZZQH		21		//
#define SHQH		22
#define DLQH		23
#define KHKH		24		// �ͻ�����
#define YHWT		25		// ���з�ʽ��¼	
#define UNUSED1_WT  26		// 
#define UNUSED2_WT  27		// 
#define ZCYH		28		// ע���û�

struct	wtdef_new_info
{	
	short	scnum;					//�����г�����
	short	yh_scnum;				//�����г�����
	short	sc[100];				//�г����
	char	scname[100][32];		//�г�����
	char	bankcode[100][8];		//���д���
	char	bankyyb[100][8];		//����Ӫҵ��
	char	bankgdtype[100][8];		//���йɶ�����г�
	char	bShowSSL;				//�Ƿ���ʾSSL��ʽ
	char	bAuthPass;				//�Ƿ�֧����֤����
	char	bTDXCA;					//�Ƿ�֧��CA֤��
	short	yybnum;					//Ӫҵ���б���
	char	yyb_tdxid[300][6];		//Ӫҵ��ͨ����ID
	char	yyb_desc[300][35];		//Ӫҵ������
	short	yyb_bhfs[300];			//Ӫҵ�����ŷ�ʽ
	char	nyybType[300];			//Ӫҵ������ 0 ���ڻ�; 1 �ڻ�Ӫҵ�� 
	int		nyyb_clitype[300];		//Ӫҵ���ͻ�������(ClientType)
	char	yyb_noprefix[300];		//��ZJZHMaxLen��ֵ��������Ƿ�Ӫҵ��ǰ׺����ǰ�棬ȱʡΪ0
	char	bSwapCombo;				//�Ƿ����Combo��λ��
	char	nColorYzm;				//�Ƿ��ǲ�ɫ����֤��
	int		ZJZHMaxLen;				//�ʽ��ʺŵ���󳤶�,��������˳���,��ǰ�油Ӫҵ�����
	char	bMustDigitPwd;			//��¼�����ϵĽ����������������
	int		nMustAccountLen;		//�ʽ��ʺű����Ƕ��,Ϊ0��ʾ������
	int		nMaxAccountLen;			//�ʽ��ʺ�������е���󳤶�
	char	AqfsCombo;				//��û�а�ȫ��ʽ��Combo,���û�в���ֻ��һ��ͨ����SSL,������
	char	UrlApplyAuthPass[255];	//ȯ����ҳ�����������URL
	char	UrlModifyAuthPass[255];	//ȯ����ҳ�޸Ŀ�������URL
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

//����cookie (TQCookie.dat�ļ�����)
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

//������ʹ�õĽ��׽�����Ϣ
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
	long	m_lYBGPnum;			// �б�����ÿ����Բ�ѯ��֧��Ʊ
	long	m_lYBZXnum;			// �б�����һ֧��Ʊ���ؼ�����¼
	long	m_lCCGPnum;			// �ֱֲ���ÿ����Բ�ѯ��֧��Ʊ
	long	m_lCCZXnum;			// �ֱֲ���һ֧��Ʊ���ؼ�����¼
	long	m_lXXSDGPnum;		// ��Ϣ�ٵ�ÿ����Բ�ѯ��֧��Ʊ
	long	m_lXXSDZXnum;		// ��Ϣ�ٵ�һ֧��Ʊ���ؼ�����¼
};

struct CcCache_Info
{
	char	m_szZZQDM[SH_CODE_LEN+1];	// ֤ȯ���볤��Ϊ7�ֽ�
	long	m_lZQSC;					// ֤ȯ�г�
	double	m_lZQSL;					// ֤ȯ����
	double	m_lKMSL;					// ��������
	float	m_CBJ;						// ��ǰ�ɱ�
	char	m_szUnUsed[65];				// �����ֶ�
};

struct OtherCache_Info					//������8�ı���
{
	char	LastRightStr[255];			// �ϴε�Ȩ�޴�
	char	m_szUnUsed[545];			// �����ֶ�
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

#define SCLV_SSL		1				// SSL�ʻ���֤��ʽ
#define SCLV_AUTHPASS	2				// �ʻ���֤+��֤���ʽ
#define SCLV_CA			4				// �ʻ���֤+��֤֤�鷽ʽ

#endif
