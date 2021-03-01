#ifndef H_PROTOCOL
#define H_PROTOCOL

#define NOW_VERSION		12		//ͨ����.netԶ�̰�汾Ϊ8,֧�������ݽӿ���Ϊ10,֧�ֳɽ����Ϊ11,������V4.0ʹ��12
#define SEPARATOR_NUM	7654321
#define REQBUFFER_LEN	16*1024
#define ANSBUFFER_LEN	4*8196	//32K

#define  SH_CODE_LEN   6
#define  NAME_LEN      8

#define  GP_MAXGBBQNUM	200

typedef struct tagReqHeader
{
	struct
	{
		unsigned char Version:4;
		unsigned char Compressed:1;
		unsigned char Encrypted:1;
		unsigned char Reserved:2;
	}     Info;
	short MainID;
	short AssisID;
	char  Priority;
	unsigned short PacketLen;
	unsigned short RawLen;
} REQHEADER;

typedef struct tagAnsHeader
{
	long	Separator;
	struct
	{
		unsigned char Version:4;
		unsigned char Compressed:1;
		unsigned char Encrypted:1;
		unsigned char Reserved:2;
	}     Info;
	short MainID;
	short AssisID;
	char  Priority;
	short req;
	unsigned short PacketLen;
	unsigned short RawLen;
} ANSHEADER;

typedef struct 
{
	REQHEADER ReqHeader;
	char      ReqBuffer[REQBUFFER_LEN];
} REQBUFFER;

const short TIME_REQ	= 1;
const short XXMCFG_REQ  = 2;
const short JBZLDATE_REQ= 3;
const short ALIVE_REQ	= 4;
const short URGENT_REQ	= 5;
const short HOSTINF_REQ = 6;
/****/
const short PRETRANS_REQ	= 7;
const short PUBKEY_REQ		= 8;
const short SESSIONKEY_REQ	= 9;
/****/
const short USER_URGENT_REQ	= 10;		//�û�����ص�ͨ��

const short INMODE_EREQ=	11;
const short INMODE2_EREQ=	12;
const short HOSTMORE_EREQ=	13;
const short AUTOBASE_EREQ=	14;
const short AUTOGBBQ_EREQ=	15;
const short AUTOBASE2_EREQ=	16;

const short OTHERINFO_REQ = 21;	//�ڲ�������Ϣ

const short NEED_AUTHP_REQ = 30;//�Ƿ���Ҫ��֤,������վ�Լ�����Ϣ,guest�û�������,ȯ���û�������
const short CODE_REQ=     101;
const short GPT_REQ=      102;
const short JYMM_REQ=     105;
const short JYMM_REQ2=    106;			  //����ҪУ��������Ϣ
const short CHMM_REQ=	  107;			  //��������

const short CJ_REQ		  =401;       //�ܳɽ�

const short XXMITEM_REQ = 702;            //������Ϣ
const short FILE_REQ    = 704;
const short XXMWZ_LONG_REQ   = 706;       //������Ϣ
const short JBMWZ_LONG_REQ   = 707;       //������Ϣ
const short MAILITEM_REQ     = 708;
const short FILELEN_REQ = 709;            //�ļ�����
const short XXM_LEN_REQ = 710;            //��Ϣ���ļ�����
const short JBM_LEN_REQ = 711;            //���������ļ�����
const short JBMWZ_ALL_REQ   = 713;       //����������Ϣ
const short F10CFG_REQ		= 714;       //����๦��F10��������Ϣ
const short JBMWZ_LONG2_REQ = 715;		 //�����������������,ÿ������10K

const short NEW_F10CFG_REQ	= 719;       //����๦��F10��������Ϣ
const short NEW_F10TXT_REQ = 720;		 //����๦��F10��������Ϣ

const short CODE_NREQ=     1101;
const short STOCKNUM_NREQ= 1102;
const short CODE2_NREQ=	   1103;
const short CODE3_NREQ=	   1104;
const short IHQ_NREQ=      1107;
const short CODELS_NREQ=   1108;
const short IHQLS_NREQ =   1109;

const short EXTHQ_NREQ=    1201;          // ����������ʾ
const short ZDHQ_NREQ=     1202;          // ����������ʾ
const short MULTIHQ_NREQ=  1203;          // ֧��������Ŀ���е�������ʾ
const short COMBHQ_NREQ=   1204;          // ��������Ʊ��������ʾ(��ѡ��,����,ָ��)
const short ZHSORT_NREQ=   1205;
const short FST_NREQ=      1301;          // ��ʱ����ͼ
const short GGHQ_NREQ=     1302;          // ��������
const short ZSHQ_NREQ=     1303;          // ָ��
const short HQB_NREQ=      1304;          // �����
const short FBB_NREQ=      1305;          // ÿ�ʳɽ���ϸ
const short FJB_NREQ=      1306;          // �ּ۱�
const short MMLD_NREQ=     1307;          // buyv,salev ��������/�ǵ�����
const short CCL_NREQ=      1308;          //�ֲ���
const short COMBFST_NREQ=  1309;          // ��Ϸ�ʱ����ͼ(�ɰ��������/��ÿ�ʳɽ���ϸ)
const short COMBHQB_NREQ=  1310;          // ��������(�����ÿ�ʳɽ���ϸ)

const short GGHQ_5MMP_NREQ=		1311;		  // ֧��5�������̵ĸ�������Э��
const short MULTIHQ_5MMP_NREQ=	1312;		  // ֧��5��������
const short COMBHQ_5MMP_NREQ=	1313;		  // ֧��5��������
const short MULTIHQ2_5MMP_NREQ=	1314;		  // ֧��5��������
const short COMBHQ2_5MMP_NREQ=	1315;		  // ֧��5��������
const short MULTIHQ3_5MMP_NREQ=	1316;		  // ֧��5��������,֧��������A����003��609
const short G2_COMBHQ_NREQ=		1318;		  // �ڶ�������Э��
const short L2_COMBQUEUE_NREQ=	1319;		  // ֧��Level2Э�������Э��

//Ϊ��Խ���ӵ���Э��
const short MULTIHQ2_NREQ= 1320;
const short COMBHQ2_NREQ = 1321;
const short CDP_NREQ	 = 1322;
const short CODEFX_NREQ	 = 1323;			//����Դ�����������������
const short COMBFX3_NREQ = 1324;
//����ETF�ȵĲ���Э��
const short COMBFX4_NREQ		= 1325;	// 
const short COMBFST2_NREQ		= 1326;	// ��Ϸ�ʱ����ͼ(�ɰ��������/��ÿ�ʳɽ���ϸ)
const short FBB2_NREQ			= 1327; // ÿ�ʳɽ���ϸ
const short GGHQ2_5MMP_NREQ		= 1328;	// ֧��5�������̵ĸ�������Э��
const short COMBHQ_2_5MMP_NREQ	= 1329;	// ֧��5��������

const short MULTIHQ_NEW_NREQ	= 1341;	//֧�ַ������ٵķ����������ù���
const short COMBHQ_NEW_NREQ		= 1342;	//֧�ַ������ٵķ����������ù���
const short ZHSORT_NEW_NREQ		= 1343; //֧�ַ������ٵķ����������ù���

const short G2_COMBHQ2_NREQ		= 1349;	// �ڶ�������Э�� ������� ����

const short L2_TICNUM_NREQ		= 1360;	// Level2�е���ʳɽ�����Ŀ
const short L2_REVTIC_NREQ		= 1361;	// Level2�е���ʳɽ�(����)
const short L2_TIC_NREQ			= 1362;	// Level2�е���ʳɽ�(����)

const short L2_WT_RANK_NREQ				= 1370;	// ��ί������
const short L2_SINGLECANCEL_RANK_NREQ	= 1371;	// ��ʱ��������
const short L2_TOTALCANCEL_RANK_NREQ	= 1372;	// �ܳ�������
const short L2_HY_RANK_NREQ				= 1373;	// ��ҵ�ɽ�����

const short G2_DEEPWARN_NREQ			= 1379;	// ����Ԥ��
const short L2_ZMZM_NREQ				= 1380;	// ��������������¼
const short L2_JGJC_NREQ				= 1381;	// ���Ӿ��ҵ�������
const short L2_AUCTION_NREQ				= 1385;	// ���Ͼ���

const short L2_IMGSTAT_NREQ				= 1390;	// ͳ�����ݵ���������(Img)

const short L2_NEWCJBS_NREQ				= 1392;	// �³ɽ�����
const short L2_NEWVOLUME_NREQ			= 1393;	// �´�ͳ��
const short L2_NEWTICKSTAT_NREQ			= 1394;	// ����������
const short L2_NEWTCKSTAT_NREQ			= 1395;	// ͳ�����ݵ���������(Tck)

const short FX_NREQ=       1501;          // ��������
const short COMBFX_NREQ=   1502;          // ��ϼ�������(�ɰ��������/��ÿ�ʳɽ���ϸ)
const short AMOUNT_NREQ=   1503;          // ��ʷ���ݳɽ���
const short UPDOWN_NREQ=   1504;          // ��ʷ�����ǵ�����(ָ��)

const short PUTFILE_REQ   =1712;
const short PATHITEM_REQ  =1713;
const short GETCWD_REQ    =1716;
const short PATHFILE_REQ		=1721;

//////////////////	4000 ���ϵ���Э��(רΪͨ����.netԶ�̰����) /////
const short COMTRY_NREQ			= 4010;	 //��ϰ�����̽�԰�
const short REVBSP1_NREQ		= 4013;	 //��С����������,����ȡ����
const short REVFBB_NREQ			= 4015;	 
const short COMBFST_HIS_NREQ	= 4020;	//��ʷ��������
const short REVFBB_HIS_NREQ		= 4021;	//��ʷTick����
const short REVBSP1_HIS_NREQ	= 4023;	//��ʷBsp����,����ȡ����
const short FJB_HIS_NREQ		= 4024;	//��ʷ�ּ۱�
//���Э��
const short SIMHQ_NREQ			= 4029;	//�ܶྫ������,һ��Ϊ200ֻ��Ʊ
const short SIMFX_NREQ			= 4030;	//�ܶ�����ķ�������,һ��Ϊ10ֻ��Ʊ
const short PREDOWNLOAD_NREQ	= 4031;	//��ʼ����֮ǰ
const short SIMFX2_NREQ			= 4032;	//�ܶ�����ķ�������,�����ɽ����

//����ETF�ȵĲ���Э��
const short COMBFST2_HIS_NREQ	= 4036;	//��ʷ��������
const short REVFBB2_NREQ		= 4037;	//����Tick����
const short REVFBB2_HIS_NREQ	= 4038;	//��ʷTick����

const short FXFILE_NREQ			= 4040;	//���������ļ�����

const short EXDAYFILE_NREQ		= 4043;	//��ʱ�����ļ�����

const short DATADIG_NREQ		= 4050;	//�����ھ�����
const short DATADIG_EXT_NREQ	= 4051;	//���������ھ�����

const short BIGVOLTICK_NREQ		= 4055;	//����������

const short GET_PASSPORT_NREQ	= 4059; //�õ�ͨ��֤
const short GET_QSID_NREQ		= 4060; //ȡȯ�̱�ʶ
const short PUT_URGENT_NREQ		= 4061; //�ύ����ͨ����Ϣ
const short GET_URLURGENT_NREQ	= 4062; //ȡURL����ͨ��
const short RESTART_NREQ		= 4063;	//������

const short GET_IW_TITLE		= 4080;	//�õ���Ѷ����������Ϣ

const short ACTIVEINFO_NREQ		= 4104;	//����
const short INFOUPLOAD_NREQ		= 4105;	//���淢��

struct remote_cod_info
{ 
	char           code[SH_CODE_LEN+1];   //֤ȯ����
	short          unit;                  //���׵�λ
	long           ltgb;                  //��ͨ�ɱ�
	char           name[NAME_LEN+1];      //֤ȯ����
	short          lbbase;
	char           precise;
};

struct remote_hq_struc
{
	char   code[6];
	WORD   itemnum;               //��������
	WORD   close;                 //ǰ���̼�,ָ��
	WORD   open;                  //���̼�,ָ��
	WORD   max;                   //��߼�,ָ��
	WORD   min;                   //��ͼ�,ָ��
	WORD   now;                   //�ּ�,���ָ��
	WORD   buy;                   //��߽����
	WORD   sale;                  //��ͽ�����
	long   cjl;                   //�ܳɽ���
	WORD   now_cjl;               //����
	float  cjje;                  //�ܳɽ����
	long   inside;                //����
	long   outside;               //����
	WORD   sd2;                   //���ǵ�
	long   ccl;                   //��ӯ��1,�ֲ���
	WORD   buyp[3];               //���������
	WORD   buyv[3];               //��Ӧ��������۵���������
	WORD   salep[3];              //����������
	WORD   salev[3];              //��Ӧ���������۵���������
};

struct hq_struc                 // ��.IHQ�ļ��ж�������ݽṹ
{
  short          itemnum;               // ��������
  float          close;                 // ǰ���̼�,ָ��
  float          open;                  // ���̼�,ָ��
  float          max;                   // ��߼�,ָ��
  float          min;                   // ��ͼ�,ָ��
  float          now;                   // �ּ�,���ָ��
  float          buy;                   // ��߽����
  float          sale;                  // ��ͽ�����,����ָ��(ָ��)
  long           cjl;                   // ����
  WORD           now_cjl;               // ����(���ֲ�)
  float          cjje;                  // �ܳɽ����
  long           inside;                // ����
  long           outside;               // ����
  float          sd2;                   // ������(��λ��)
  union
  {
                 float syl;             // ��ӯ��1,������Ȩ��ָ��
                 long ccl;              // �ֲ���
  }              syl_ccl;
  float          buyp[3];               // ���������
                                        // ����ɽ����:buyp[0]
                                        // ծȯ�ɽ����:buyp[1]
                                        // ��ծ�ɽ����:buyp[2]
  WORD           buyv[3];               // ��Ӧ��������۵���������
                                        // ���Ǽ���:buyv[0]
  float          salep[3];              // ����������
					// Ȩ֤�ɽ����:salep[0]
					// �ڻ���ֲ���:salep[1]
					// ָ�������ɽ����:salep[1]
					// �ڻ�������:salep[2]
                                        // ָ���򲻺���Ȩָ��:salep[2]
  WORD           salev[3];              // ��Ӧ���������۵���������
                                        // �µ�����:salev[0]
  struct
  {
        WORD           inoutflag:2;     // �����̱�־
        short          sdunit:4;        // ��������λ(���ڼ�������ָ��)
        WORD           unused:10;       // δ�ñ�־
  }              flag;                  // ��־λ�ṹ
};

struct remote_ls_struc
{ 
	WORD            minute;
	WORD            now;                   //�ּۻ����ָ��
	WORD            now_cjl;               //����
	short           ccl_dif;               //�ֲ�������
	short           bs_flag;               //������־
};

struct remote_fz_data
{ 
	WORD            now;
	WORD            average;
	WORD            now_cjl;
};

struct remote_mmld_struc
{ 
	WORD            buyv;
	WORD            salev;
};

struct remote_ls_data
{ 
	union long_short time;
	WORD open;
	WORD high;
	WORD low;
	WORD close;
	long volume;
};

struct remote_TmpStkInfo
{ 
	char           Code[SH_CODE_LEN];     // ֤ȯ����
	short          Unit;                  // ���׵�λ
	float          ActiveCapital;         // ��ͨ�ɱ�
	char           Name[NAME_LEN];        // ֤ȯ����
	long           VolBase;               // ���ȵĻ���
	char           precise;
};

struct remote_TmpStkInfo2
{
	char           Code[SH_CODE_LEN];     // ֤ȯ����
	short          Unit;                  // ���׵�λ
	char           Name[NAME_LEN];        // ֤ȯ����
	long           VolBase;               // ���ȵĻ���
	char           precise;
	float		   Close;				  //����
};

struct remote_TmpStkInfo3
{
	char           Code[SH_CODE_LEN];     // ֤ȯ����
	short          Unit;                  // ���׵�λ
	char           Name[NAME_LEN];        // ֤ȯ����
	long           VolBase;               // ���ȵĻ���
	char           precise;
	float		   Close;				  //����

	short		   BaseFreshCount;		  //�������ϵĸ��´���
	short		   GbbqFreshCount;		  //�ɱ���Ǩ�ĸ��´���
};

struct remote_BaseInfo2	//Protocol Change
{
	char		setcode;			//�г�����
	char        Code[SH_CODE_LEN];  // ֤ȯ����
	float       ActiveCapital;      //��ͨ�ɱ�
	short       J_addr;				//����ʡ��
	short       J_hy;				//������ҵ
    long        J_gxrq;				//��������
	long		J_start;			//��������
    float       J_zgb;				//�ܹɱ�
    float       J_zjhhy;			//֤�����ҵ
    float       J_oldjly;			//������ھ�����
    float       J_oldzysy;			//���������Ӫ����
    float       J_bg;				//B��
    float       J_hg;				//H��
    float       J_mgsy2;			//����ÿ������ (�Ʊ����ṩ��ÿ������,������Ĳ���)
    float       J_zzc;				//���ʲ�(ǧԪ)
    float       J_ldzc;				//�����ʲ�
    float       J_gdzc;				//�̶��ʲ�
    float       J_wxzc;				//�����ʲ�
    float       J_cqtz;				//����Ͷ��
    float       J_ldfz;				//������ծ
    float       J_cqfz;				//���ڸ�ծ
    float       J_zbgjj;			//�ʱ�������
    float       J_jzc;				//�ɶ�Ȩ��(���Ǿ��ʲ�)
    float       J_zysy;				//��Ӫ����
    float       J_zyly;				//��Ӫ����
    float       J_yszk;				//Ӧ���ʿ�
    float       J_yyly;				//Ӫҵ����
    float       J_tzsy;				//Ͷ������
    float       J_jyxjl;			//��Ӫ�ֽ�����
    float       J_zxjl;				//���ֽ�����
    float       J_ch;				//���
    float       J_lyze;				//�����ܶ�
    float       J_shly;				//˰������
    float       J_jly;				//������
    float       J_wfply;			//δ��������
    float       J_tzmgjz;			//����ÿ�ɾ��ʲ� ��������:  ���ʲ�/��������ܹɱ�
	float		J_HalfYearFlag;		//ȫ������Ϊ����Ϊ��λ
};

struct remote_cdpInfo
{
	float       cdp;                   // �����м�
	float       nh;                    // ����
	float       nl;                    // ���
	float       ah;                    // �߼�ͻ�Ƶ�
	float       al;                    // �ͼ�ͻ�Ƶ�
 	char		dkflag;                // �ʺ����������
	float       tbp;                   // ���ƽ���
	float       stoplost;              // ֹ���
	float       leave;                 // �˽��
};

//��������ṹ
struct simhq_Info
{
  float          Open;                  // ���̼�,ָ��
  float          Max;                   // ��߼�,ָ��
  float          Min;                   // ��ͼ�,ָ��
  float          Now;                   // �ּ�,���ָ��
  DWORD          Volume;                // ����
  float          Amount;                // �ܳɽ����  
};

struct inmode_req
{
	short req;
	char  type;
	char  inmode;
	char  username[13];
	char  password[13];
	char  bnetcard;
	char  netcard[13];
	char  castring[13];
};

struct Protocol_IpInfo
{
	char ipaddress[26];
	unsigned short port;
	char description[50];
};

struct inmode_ans
{
	char  ch;	//�ɹ���־,0:�ɹ�,�ͻ��˶Ͽ�����,������IP 1:��ɹ�,�ͻ��˼����������� 2:��ɹ�,�ͻ��˼����������ӣ���һ������뷢��һӪҵ����š�Ϊ����:�����
	short minute;
	char  right;
	char  ipnum;
	struct Protocol_IpInfo Info[1];
};

struct inmode2_req
{
	short req;
	char  type;
	char  inmode;
	char  username[13];
	char  nickname[21];
};

struct inmode2_ans
{ 
	char  ch;		//�ɹ���־,0(����'0'):�ɹ� ����:�����
};

struct sendyybno_req
{
	short req;
	short whichyyb;
};

struct sendyybno_ans
{
	char ch;		//�ɹ���־,0(����'0'):�ɹ� ����:�����
};

struct autobase_req //������BASE��GBBQǰ��Ӧ�ȶ���INF
{
	short req;
	short stknum;
	char  buf[1];	//��Ʊ���뻺����,�����autobase2�����������setcode
};

struct autobase2_ans	//Protocol Change
{
	short stknum;
	struct remote_BaseInfo2 basep[1];
};

struct autogbbq_req
{
	short req;
	short stknum;
	char  buf[1];
};

struct autogbbq_ans
{
	short stknum;
	char  buf[1]; //��Ź�Ʊ�г�����,��Ǩ�����;����Ǩ�Ļ�����
};

struct cod_req
{ 
	short req;
	short setcode;
	short startxh;
};

struct stknum_req
{
	short req;
	short setcode;
	long  rq;
};

struct gpt_req
{ 
	short req;
};

struct mm_req
{ 
	short req;
	char  username[13];
	char  password[13];
};

struct mm_req2
{ 
	short req;
	char  username[13];
	char  password[13];
	char  netcard[13];
};

struct chmm_req
{
	short req;
	char  oldpassword[13];
	char  newpassword[13];
};

struct chmm_ans
{
	short chret;//-1:�����벻��ȷ��0:����ԭ�����޸�,1:�ɹ�
};

struct ihq_req
{ 
	short req;
	short setcode;
	short startxh;
};
struct codls_req
{ 
	short req;
	short setcode;
	short startxh;
	long  rq;
};

struct ihqls_req
{ short req;
  short setcode;
  short startxh;
  long  rq;
};

struct hq_req
{ short req;
  short setcode;
  short htype;
  short ftype;
  short startxh;
  char  choicestr[4];
};
struct exthq_req
{ short req;
  short setcode;
  short htype;
  short ftype;
  short startxh;
  short wantnum;
  char  choicestr[4];
};

struct  zdhq_req
{ short req;
  short setcode;
  char  code[19*SH_CODE_LEN];
};

struct multihq_req
{ short req;
  short setDomain;
  short coltype;
  short startxh;
  short wantnum;
  short sorttype;
};

struct combhq_req
{ short req;
  short num;
  char  codehead[1];
};

struct multihq_new_req
{ short req;
  short setDomain;
  short coltype;
  short startxh;
  short wantnum;
  short sorttype;
  short zfmin;
  short unused[3];
};

struct combhq_new_req
{ short req;
  short zfmin;
  short unused[3];
  short num;
  char  codehead[1];
};

struct zst_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct gghq_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct hqb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct fbb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

struct fbb_his_req
{ short req;
  long  ldate;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

struct fjb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct fjb_his_req
{ short req;
  long  ldate;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct mmld_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct ccl_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct combzst_req
{ short req;
  short setcode;			//����setcode
  char  code[SH_CODE_LEN];
  short hqflag;
  short ticknum;
};

struct combzst_his_req
{ short req;
  long ldate;
  char setcode;
  char code[SH_CODE_LEN];
};

struct combhqb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  short hqflag;
  short ticknum;
};

struct cj_req
{ 
	short req;
	short flag;
	short setcode;
	short startxh;
	short recnum;
};

//��������ǰ������
struct pretrans_ans
{
	unsigned char  SecureRank;
	unsigned char  HQSecure;
	unsigned char  RsaBits;
	unsigned char  WhichSession;
	unsigned char  WhichHash;
	unsigned char  KeyRank;
	unsigned char  AllInit;
	unsigned char  Reserved;
};

//////////

struct fx_req
{ short req;
  short offset;
  short setcode;
  short linetype;
  char  code[SH_CODE_LEN];
};

struct codefx_req
{ 
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
	short offset;
	short itemnum;
};

struct combfx2_req
{ 
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
	short linetype;
	short mulnum;
	short offset;
	short itemnum;
};

struct amount_req
{ 
	short req;
	short offset;
	short linetype;
	char  code[SH_CODE_LEN];
	short itemnum;
};

struct updown_req
{ 
	short req;
	short offset;
	short linetype;
	char  code[SH_CODE_LEN];
	short itemnum;
};

struct wz_req
{ 
	short req;
	short htype;
	short startxh;
};

struct file_req
{  
	short req;
	DWORD pos;
	char  fname[40];
};


struct filelen_req
{  
	short req;
	char  fname[40];
};

struct filelen_ans
{
   DWORD filelen;
   char  usemdstr;
   char	 mdstr[33];
};

struct mailitem_req
{  short req;
   char  code[7];
   short startxh;
};

struct xxmwz_long_req
{ short req;
  short xxmxh;
  short xh;
  DWORD pos;
};

struct xxm_len_req
{ short req;
  short xxmxh;
  short xh;
};
struct jbmwz_long_req
{ short req;
  DWORD pos;
  short setcode;
  char code[SH_CODE_LEN];
};

struct jbmwz_all_req
{ short req;
  short setcode;
  char code[SH_CODE_LEN];
};

struct jbm_len_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct pathfile_req
{
	short	req;
	long	pos;
	long	len;
	char	pathfile[100];
};

struct pathfile_ans
{
	long	len;
	char	buf[1];
};

struct secret_req
{ short req;
  DWORD bios;
  DWORD biosppp;
};

struct openfile_req
{ short req;
  char  fname[40];
};

struct putfile_req
{ short req;
  DWORD pos;
  WORD  len;
  char  fname[40];
  char  buf[1];
};

struct pathitem_req
{ short req;
  WORD  startxh;
  char  path[80];
};

struct gpt_ans
{ 
	short sz[4];
	short sh[4];
};

struct mm_ans
{ 
	char  ch;
	short minute;
	char  right;
};

struct hq_ans
{ short  totalnum;
  short  dispnum;
  struct remote_hq_struc hqp[19];
};

struct cj_ans
{ 
	short startxh;
	short recnum;
	char  disp_string[1];
};

struct wz_ans
{ 
	short num;
	char  buf[1];
};

struct time_ans
{ 
	struct bc_date date;
	struct bc_time time;
};

struct otherinfo_ans
{
	long ClientNum;		//��ǰ��������
	long MaxConnectNum;
	long PackageNum;
	char HasStatus;
	char HasLog;
	char bHQ;
	char bWT;
	char starttime[25];

	char HostVer[30];
	char ProtocolVer;
	long TotalClientNum;//��ǰ����������
	long UsedClientNum;	//�ӿ�����վ�������ж����˵�½��
	char bAutoBase;
	char HomePath[255];
	char NetCardStr[20];
	long InfDate;
	long InfHms;
	char HostType;		//���������� �������ӣ���
	char ProcType;		//0:����ҵ��ʽ 1:���̷߳�ʽ
	char ZlibType;		//ѹ����ʽ 0:�Զ��Ż� 1:��ȫ��ѹ�� 2:����޶�ѹ��
	char bDayToMem;
	char bWeekToMem;
	char bMonToMem;
	char bMinToMem;
	char bKExplain;			//�ǲ��ǳɹ���������ѡ�ɵȷ���
	char bAlarm;
	char bHqStat;
	char bCanOldFy;		//�ǲ����������в���
	char bCanDos;
	char bCanTdxW;
	char bCanWeb;
	char bCanPda;
	char bCanWide;
	char bCanPlay;
	char bCanJdh;

	char bTapi;
	char bUdp;
	char bChat;
	short RecSocket;
	short UdpSocket;

	short ProcessNum;	//������
	short ThreadNum;	//�߳���
	short unused1;

	char  TicFlag;		//TIC��־
	char  MtcFlag;		//MTC��־
	char  unused2[30];
};

struct jbzldate_ans
{
	int date;
	int time;
};

struct xxmwz_ans
{ 
	short num;
	char  buf[1];
};

struct cod_new_ans
{ 
	short  stknum;
	struct remote_TmpStkInfo headp[1];
};

struct cod_new_ans2
{ 
	short  stknum;
	struct remote_TmpStkInfo2 headp[1];
};

struct cod_new_ans3
{ 
	short  stknum;
	struct remote_TmpStkInfo3 headp[1];
};

struct ihq_new_ans
{ 
	short  stknum;
	struct CurrStockData hqp[1];
};

struct secret_ans
{ 
	short seconds;
	short trustee;
};

struct urgent_req
{
	short req;
};

struct user_urgent_req
{
	short req;
	char  username[50];
	char  reserved[4];
};

struct urgent_ans
{
	char valid;
	long ldate;
	short advertlen;
	short sourcelen;
	short contentlen;
	char buf[1];
};

struct hostinf_ans
{
	char webpage;
	char hostname[21];
	char url[100];
};

struct hostmore_req
{
	short req;
	char  verflag;	
};

struct hostmore_ans
{
	char   errflag;	//errflagΪ0��ʾ���ã��������ʾ�д�
	struct bc_date date;
	struct bc_time time;
	short sz[8];
	short sh[8];
	char  byesterday;
	long  szinfdate;
	long  szinfhms;
	long  shinfdate;
	long  shinfhms;
	char  bserv1;
	char  bserv2;
	char  bserv3;
	char  nUrgentNum;
	char  linuxcheck;
	char  bbigjbm;
	char  jbmnum;
	char  bcanuserurgent;
	char  hasBakHostFile;
	char  webpage;
	char  hostname[21];
	char  url[70];	
	char  Jbmname[3][10];
};

struct zhsort_req
{
	short req;
	char  domain;
	char  num;
};

struct zhsort_new_req
{
	short req;
	char  domain;
	short zfmin;
	short zhtype;	//0:���ڵķ�ʽ,ί�б� 1:�µķ�ʽ,��ʾ���������ʺ�����������
	short unused[2];
	char  num;
};

struct zhsort_ans
{
	char num;//ÿ�������Ĺ�Ʊ��Ŀ
	char buf[1];
};

struct cdp_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];  
};

struct bsp1_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

struct bsp1_his_req
{ short req;
  long  ldate;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

//���Э��
struct simhq_req //�ܶྫ������
{ short req;
  short num;	
  char  buf[1];	//SetCode�ʹ��뻺����
};

struct simhq_ans
{
  short num;	//����ֻ��Ʊ
  char buf[1];	//����ľ������黺����  
};

struct simfx_req //�ܶ�����ķ�������
{ short req;
  short recentnum;	//�����Ʊ��
  char	type;		//����
  short mulnum;		//�����ڵ���Ŀ,һ����1
  short num;
  char  buf[1];		//���뻺����
};

struct simfx_ans
{ short num;
  char  type;
  char buf[1];		//����ķ������ݻ�����(����ÿֻ��Ʊ�ķ������ݸ���)
};

struct fxfile_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  long  startdate;
  long  enddate;  
  short type;
};

struct fxfile_ans
{ 
  short type;
  long filelen;
  char buf[1];
};

#define	EXDAYSZ_FILE	0
#define	EXDAYSH_FILE	1

struct dayfile_req
{
  short req;
  short type;
  long  ldate;
  long	lpos;
  long	reqlen;
};

struct dayfile_ans
{ 
  short type;
  long filelen;
  long sendlen;
  char buf[1];
};

struct HostList
{
	char  ch;	//�ɹ���־,0:�ɹ�,�ͻ��˶Ͽ�����,������IP 1:��ɹ�,�ͻ��˼����������� 2:��ɹ�,�ͻ��˼����������ӣ���һ������뷢��һӪҵ����š�Ϊ����:�����
	char  ipnum;
	struct Protocol_IpInfo Info[100];	//���֧��100��ί����վ��Ӫҵ��
};

struct pre_download_req
{
	short req;
};

struct pre_download_ans
{
	char sucflag;		//�ɹ���־ 0:�������� 1:������
	char errmsg[100];	//������Ϣ��
};

struct datadig_req
{
	short req;
	short reserved;
};

struct datadig_setcode_code
{
	char setcode;
	char stockcode[SH_CODE_LEN];
	unsigned long rate;	//��ע�Ȼ�����
};

struct datadig_ans
{
	long clientnum;
	long usedclientnum;
	short attention_num;
	short resonate_num;
	struct datadig_setcode_code attention[50];
	struct datadig_setcode_code resonate[50];
	long contentlen;
	char content[1];
};

struct datadig_ext_ans
{
	long clientnum;
	long usedclientnum;
	long seconds; //
	char HostName[21];
	short attention_num;
	short no_attention_num;
	short resonate_num;
	short no_resonate_num;
	struct datadig_setcode_code attention[80];
	struct datadig_setcode_code no_attention[80];
	struct datadig_setcode_code resonate[80];
	struct datadig_setcode_code no_resonate[80];
	long contentlen;
	char content[1];
};

struct BigVolTick
{
	char			SetCode;
	char			Code[SH_CODE_LEN];
	short			Minute;                // ��������ķ�����
	float			Now;                   // �ּ�
	DWORD			NowVol;                // ����
	char			InOutFlag;             // ������־
}; //�ṹΪ18���ֽ�,��32K��Ӧ��,���Դ�1800����

struct bigvoltick_req
{
	short	req;
	long	getpos;  //���getposΪ-1,�����������500��
	long    avail_vol;	//��������ֵ,ȱʡΪ500,�������400
	long	reserved;
};

struct bigvoltick_ans
{
	long	lastpos;
	long	ticknum;
	struct BigVolTick list[1];
};

struct get_urlurgent_req
{
	short req;
};

struct get_urlurgent_ans
{
	long  URLEndTime;
	char  URLTitle[81];
	char  URL[81];
	char  reserved[21];
};

struct f10cfg_req
{ short req;
  short setcode;
  char code[SH_CODE_LEN];
};

struct f10cfg_title
{
	char title[16];	
	long pos;
	long len;
};

struct f10cfg_ans
{
  short titlenum;
  struct f10cfg_title list[1];
};

///////////////////////

struct new_f10cfg_req
{
	short req;
	short setcode;
	char code[SH_CODE_LEN];
	short whichjbm;
	short reserved;
};

struct new_f10cfg_title
{
	char sTitle[64];
	char sFilePath[80];
	long nOffset;
	long nLength;
};

struct new_f10cfg_ans
{
  short titlenum;
  struct new_f10cfg_title list[1];
};

struct new_f10txt_req
{
	short req;
	short setcode;
	char code[SH_CODE_LEN];
	short which;
	char sFilePath[80];
	long nOffset;
	long nLength;
	short whichjbm;
	short reserved;
};

struct new_f10txt_ans
{
	short setcode;
	char code[SH_CODE_LEN];
	short which;
	short num;
	char  buf[1];
};

///////////////////////

struct need_auth_req
{
  short req;
  char  username[13];
  char  password[13]; 
  char  needcheckip;
  char  reserved[3];
};

struct IpInfo
{
	char ipaddress[26];
};

struct need_auth_ans
{
  char  sucflag;	//�û��Ƿ�ɹ�
  char  errmsg[50];
  char  need_auth;	//�Ƿ���Ҫ��֤
  short ipnum;
  struct IpInfo Info[1];
};

struct get_qsid_req
{
	short req;
};

struct get_qsid_ans
{
	char  qs_id[11];
	char  reserved[21];
};

struct get_passport_req
{
	short req;
	char QsId[11];
	float clientver;
	char  AutoupOrder;	//�Զ��������
	char  ProductIDchar;//��ƷID����Char
	char  magicchar2;	//���յĵڶ���MagicChar
	char  ProductID[11];//��ƷID
	char  valid_level;	//��Ч�ȼ�,������0,��Ҫ�Ƿ�ֹ�ͻ��˵������ƽ���ʹ��
};

struct get_passport_ans
{
	char Sucflag;			//0:ʧ�� 1:�ɹ�
	char bAutoupFromHQHost;	//��������վ�Ͻ�������
	char bVerLimit;			//0:�汾û�������� 1:�汾������
	char errmsg[50];		//������Ϣ
	char bNeedAutoup;		//�Ƿ���Ҫ���� 0:�������� 1:������ 2:��ǿ������
	float ThisVer;			//�ͻ��˰汾
	char  AutoupURL[120];	//��������ַĿ¼
	char  Descripton[1];	//����˵��
};

//�������
enum
{
	SYSAD_TYPE,
	NEWS_TYPE,
	URGENT_TYPE,
	WEBURL_TYPE
};

typedef	struct	tagReqInfoNull
{
	short	ReqID;
	time_t	TimeTag;
}REQINFONULL,*PREQINFONULL;

typedef	struct	tagReqInfoTimer
{
	short	ReqID;
	long	LogTimer;
	BYTE	MoreInfo;		//0��������Ϣ 1��������
	long	TimeTag;		//ʱ��
	long	InfoLen;
	char	Info[1];
}REQINFOTIMER,*PREQINFOTIMER;

typedef	struct tagUrgentInfo
{
	BYTE	Type;
	time_t	TimeTag;
	short	TitleLen;
	short	ContentLen;
	char	*Info;
	DWORD	Reserved[4];
}URGENTINFO, *PURGENTINFO;

typedef	struct	tagReqInfoUpload
{
	short	ReqID;
	short	ReqUpLoadLen;	//������Ϣ�Ĵ�С
	BYTE	Type;			//0��������Ϣ 1��������
	time_t	TimeTag;		//ʱ��
	long	InfoLen;
	char	Info[1];
}REQINFOUPLOAD,*PREQINFOUPLOAD;
//////////////////////////////////////////////////////////////////////////
struct iw_title_req
{
	short	req;
	char	search_type;			//0:���ݹ�Ʊ���� 1:������ҵ���� 2:���ݷ���ID 3:ȡ�۲�Ա����
	long	from_order;				//���Ǹ�˳��ʼ���ȡ����
	short	wantnum;				//���80��
	
	short	setcode;				//֤ȯ�г�
	char	code[11];				//����
	long	hy_id;					//��ҵID
	long	type_id;				//����ID
	
	short	magicshort2;			//�û�������������ص���֤��
	char	LoginName[20];			//�û���
	
	char	ununsed[8];				//����
};

struct iw_title_ans
{
	short	totalnum;
	short	titlenum;
	struct	iw_client_title list[1];
};
////////////////////////////////////////////////////////////////////////////////////
//Level2���Э��
//�����
struct l2ticnum_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  DWORD localSeq;
  short reserved;
};

struct l2ticnum_ans
{
  short setcode;
  char  code[SH_CODE_LEN];
  DWORD MaxSeq;
};

//�������
struct l2tic_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  DWORD startxh;
  short num;
};

//ί������
struct l2_wt_rank_req
{
	short req;
	short setcode;
	short fltype;
	DWORD reserved;
};

struct l2_WTRankings
{
	char	Code[SH_CODE_LEN];	//֤ȯ����
	float	TotalOrderQty;		//ί����
	float	TotalVolumeTraded;	//�ɽ���
};

struct l2_wt_rank_ans
{
	struct l2_WTRankings list[20];
};

//��ʱ��������
struct l2_singlecancel_rank_req
{
	short req;
	short setcode;
	short fltype;
	DWORD reserved;
};

struct l2_SingleCancelRankings
{
	char	Code[SH_CODE_LEN];	//֤ȯ����
	WORD	OrderSecond;		//ί��ʱ��(6�����Ժ����)
	float	Quantity;			//ί����
	float	Price;				//�۸�
};

struct l2_singlecancel_rank_ans
{
	struct l2_SingleCancelRankings list[20];
};

//�ܳ�������
struct l2_totalcancel_rank_req
{
	short req;
	short setcode;
	short fltype;
	DWORD reserved;
};

struct l2_TotalCancelRankings
{
	char	Code[SH_CODE_LEN];	//֤ȯ����
	float	TotalWithdrawnQty;	//�ܳ�����
};

struct l2_totalcancel_rank_ans
{
	struct l2_TotalCancelRankings list[20];
};

//��ҵ�ɽ�����
struct l2_hy_rank_req
{
	short req;
	short setcode;
	DWORD reserved;
};

struct l2_HYRankings
{
	char	IndustryCode[8];			//��ҵ����
	float	IndustryTotalValueTraded;	//��ҵ�ܳɽ���
};

struct l2_HYGPRankings
{
	char	Code[SH_CODE_LEN];			//֤ȯ����
	float	TotalValueTraded;			//�ɽ���
};

struct l2_hy_rank_ans
{
	float  MarketTotalValueTraded;
	struct l2_HYRankings HYlist[10];
	struct l2_HYGPRankings HYGPlist[10*5];
};

struct Server_MarketInfo
{
	struct l2_WTRankings WTRankings[20];
	struct l2_SingleCancelRankings SingleCancelRankings[20];
	struct l2_TotalCancelRankings TotalCancelRankings[20];
	float  MarketTotalValueTraded;
	struct l2_HYRankings	HYList[10];
	struct l2_HYGPRankings	HYGPList[10*5];
};	

//��ʱͼ���
struct l2_zmzm_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_jgjc_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_newcjbs_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_newvolume_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_newtickstat_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

#define MAX_AUCTIONNUM	500
struct l2_auction_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
	long  ItemPos;	//ϣ���õ�����ϢID,��0��,�Ϸ�ItemPos<ItemNum
	long  ItemNum;	//���Խ��յ��ĸ���
};

#define MAX_DEEPWARN		100
struct deepwarn_req
{
	short req;
	short setcode;
	long  ItemPos;	//ϣ���õ�����ϢID,��0��,�Ϸ�ItemPos<ItemNum
	long  ItemNum;	//���Խ��յ��ĸ���
};

////////////////////////////////////////////////////////////////////////////////////

//������������(10)
#define HQ_ASSISID			10
//ֱ�ӷ���(40-99)
#define HQALIVE_ASSISID		40
#define JUSTCURRHQ			41
#define STATUSZSHQ			42
#define WARNGPHQ_ASSISID	43
#define REFRESHHQ_ASSISID	44
#define SZ_DWARN_ASSISID	45
#define SH_DWARN_ASSISID	46
#define OLD_BIGVOL_ASSISID	47
#define IMGSTATHQ_ASSISID	48
#define TCKSTATHQ_ASSISID	49
#define MOREINFO			50

//ǰһ������Ӧ���ŷ���(100-255)
#define JBM_ASSISID			100
#define JBMLEN_ASSISID		101
#define XXM_ASSISID			102
#define XXMLEN_ASSISID		103
#define XXMTITLE_ASSISID	104
#define JGDATALEN_ASSISID	105
#define JGDATA_ASSISID		106
#define SZSTOCKNUM_ASSISID	107
#define SHSTOCKNUM_ASSISID	108
#define SZSTOCKINF_ASSISID	109
#define SHSTOCKINF_ASSISID	110
#define URLURGENT_ASSISID	111

#define INMODE2_ASSISID		116
#define AUTOBASE_ASSISID	117
#define AUTOGBBQ_ASSISID	118
#define BLOCKDATLEN_ASSISID	119
#define BLOCKDAT_ASSISID	120
#define ZHBZIPLEN_ASSISID	121
#define ZHBZIP_ASSISID		122
#define NEEDAUTH_ASSISID	123

#define OTHERFILELEN_ASSISID	124
#define OTHERFILE_ASSISID		125
#define	SYNCDATALEN_ASSISID		126
#define	SYNCDATA_ASSISID		127
#define	IWTITLE_ASSISID			128

#define SIMHQ_ASSISID			133
#define WIDEHQ_ASSISID			134
#define SIMFX_ASSISID			135
#define BKDATA_ASSISID			136
#define DOWN_MIN_ASSISID		137
#define DOWN_JBMLEN_ASSISID		138
#define DOWN_JBM_ASSISID		139
#define DATADIG_ASSISID			140
#define DOWNBAKHOST_ASSISID		141
#define DOWN_FXMIN_ASSISID		142
#define	DOWN_EXDAYFILE_ASSISID	143

#define URGENT_ASSISID		145
#define AD_ASSISID			146
#define HOSTMORE_ASSISID	147
#define JYMM_ASSISID		148
#define RXXMCFG_ASSISID		149
#define ZHSORT_ASSISID		150  //ӦС��255
#define JBMCFG_ASSISID		151  //ӦС��255
#define	HKSEATQ_ASSISD		152

#define GETPASSPORT_ASSISID	153  //�õ�ͨ��֤

#define NEW_JBMCFG_ASSISID	155
#define NEW_JBMTXT_ASSISID	156
#define DOWN_NEW_JBMCFG_ASSISID	157
#define DOWN_NEW_JBMTXT_ASSISID	158
#define TICKRANGEHIS_ASSIST	163
#define TICKRANGECUR_ASSIST	164

#define	XXGTITLE_ASSISID	166
#define	XXGTXT_ASSISID		167

#define BIWARNQ_ASSISID		171

#define	INFOUPLOAD_ASSISID	180
//���������(255����)

#endif
