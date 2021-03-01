#ifndef __INC_H__
#define __INC_H__

#include <AfxWin.h>
#include <afxcmn.h>
#include <vector>


/////////////////////////////////////////////////////////////////////////////////////
//���ýṹ

union VARDATA
{
	int		intdata;
	double	doubledata;
};

enum  SG_HEAD_DATA_TYPE//��������
{
	SG_TYPE_STRING = 0,			//�ַ�������
	SG_TYPE_DOUBLE,				//��������
	SG_TYPE_INT,				//��������
};

enum SG_HEAD_CALC_TYPE//��������
{
	SG_CALC_STATIC=0,			//���뾲̬
	SG_CALC_DYNA,				//ʵʱ�仯
	SG_CALC_COM,				//�ۺϼ���
};

enum SG_ALIGN_TYPE//��������
{
	SG_ALIGN_LEFT=0,			//�����
	SG_ALIGN_RIGHT,				//�Ҷ���
	SG_ALIGN_CENTER				//�м����
};

enum SG_FORMAT_TYPE//��ʾ��ʽ����
{
	SG_FORMAT_NORMAL=0,			//�����κδ���
	SG_FORMAT_COMMA_SEP,		//ǧ��λ���ŷָ�

	SG_PERCENT_BANJIAO,			//��ǰٷֺ�
	SG_PERCENT_QUANJIAO,		//ȫ�ǰٷֺ�
	SG_ONE_THOUSAND,			//ǧ�ֺ�

	SG_FORMAT_WAN,				//��ʾΪXXX.XX��
	SG_FORMAT_YI,				//��ʾΪXXX.XX��
	SG_FORMAT_COMMA_SEP_WAN,	//XXX,XXX,XXX,XXX.XX��
    SG_FORMAT_COMMA_SEP_YI,		//XXX,XXX,XXX,XXX.XX��
	SG_FORMAT_WAN_YI,			//�Զ����ڱ�ʾ��
	SG_FORMAT_COMMA_SEP_WAN_YI,	//��ǧ��λ���Զ����ڱ�ʾ��

	SG_DATE_SLASH,				//б�ָܷ�ʱ��yyyy/mm/dd
	SG_DATE_SLASH2,				//б�ָܷ�ʱ��mm/dd/yyyy
	SG_DATE_SHORTLINE,			//�̺��߷ָ�ʱ��yyyy-mm-dd
	SG_DATE_NORMAL,				//YYYYMMDD
	SG_DATE_SLASH_SHORT,		//YY/MM/DD
	SG_DATE_SLASH2_SHORT,		//MM/DD/YY
	SG_DATE_SHORTLINE_SHORT,	//YY-MM-DD
	SG_DATE_NORMAL_SHORT,		//YYMMDD
	
	SG_DATE_REPORT,				//�걨��ʽ
	SG_DATE_REPORT_SHORT,		//���걨��ʽ

	SG_FORMAT_FIXED_2,			//�̶������ʾ,��ʾ2λ								
	SG_FORMAT_FIXED_16 = (SG_FORMAT_FIXED_2+14)
								//�̶������ʾ,��ʾ16λ,���ڹ�Ʊ,�ɱ�ʾλSG_FORMAT_FIXED_2+4;
};

enum SG_FORMAT_ZERO//�����ʾ0
{
	SG_FORMAT_ZERO_NORMAL = 0,   //���ر�Ҫ��
	SG_FORMAT_ZERO_BLANK,	     //��ʾΪ�ո�
	SG_FORMAT_ZERO_SHORT_LINE,	 //��ʾΪ�̺���
};

enum SG_FORMAT_INVALID//�����ʾinvalid
{
	SG_FORMAT_INVALID_BLANK = 0, //��ʾΪ�ո�
	SG_FORMAT_INVALID_SHORT_LINE,//��ʾΪ�̺���
};

enum SG_DATACOLOR_MODE//������ɫ
{
	SG_DATACOLOR_MODE_V=0,		//�ɽ�����ɫ;
	SG_DATACOLOR_MODE_W,		//����������ʾ���ͻ��˵��б��ı���ɫ;
	SG_DATACOLOR_MODE_P,		//�۸���ʾ�������������̼۱Ƚϣ��ֱ�ȡ��ɫ���ü��ϵ��ǵ�ƽ��ɫ��ʾ;
	SG_DATACOLOR_MODE_B,		//������ʾ��������OutName_BJָʾ�ֶ����Ƚϣ��ֱ�ȡ��ɫ���ü��ϵ��ǵ�ƽ��ɫ��ʾ;
	SG_DATACOLOR_MODE_Z,		//������ʾ������0.0�Ƚϸߵͣ��ֱ�ȡ��ɫ���ü��ϵ��ǵ�ƽ��ɫ��ʾ
	SG_DATACOLOR_MODE_F			//�����е�state����ȷ��ȡm_OtherInfo.StateColor[]�е�������ɫ
};

enum SG_SUMARY//���ܸ�ʽ
{
	SG_SUMARY_BLANK=0,
	SG_SUMARY_SHORT_LINE,
	SG_SUMARY_STRING,
	SG_SUMARY_SUM,
	SG_SUMARY_COUNT,
	SG_SUMARY_MIN,
	SG_SUMARY_MAX,
};

struct sg_head//��ͷ�ĳ���
{
   	char				title[101];		//��ʾ�ı���
	char				name[101];		//Ψһ��ʶĳһ����ͷ
	short				width;			//��ͷ���,������Ϊ��λ,Ϊʵ����ʾ�Ŀ��
	int					decimaldigits;	//С��λ��
    SG_ALIGN_TYPE		align;			//�������ݶ�������
	SG_ALIGN_TYPE		headalign;		//��ͷ���ݶ�������
	SG_HEAD_DATA_TYPE	datatype;		//��������
	SG_FORMAT_TYPE		formattype;		//��ʾ��ʽ����
	SG_FORMAT_ZERO		formatzero;		//�����ʾ0
	SG_FORMAT_INVALID	formatinvalid;	//�����ʾinvalid
	SG_DATACOLOR_MODE	datacolormode;	//������ɫ
	char				outname_bj[101];	//�Ƚ���
	UINT				nFlag;			//��־λ�������Ժ���չ;
};

struct sg_cell//����е�Ԫ�����ݵĳ���
{
	VARDATA				data;			//���������,˫������;
	CString				stringdata;		//������ַ�����
	COLORREF			txtcolor;		//�ı���ɫ
	int					iImage;			//�ڵ�һ����ʾ��ͼƬ,��Ϊ-1,��ʾ����ʾ;�������ImageList
	int					iSelectedImage;	//��ѡ��ʱ��ʾ��ͼƬ
};


struct SuperList_ColorFont//������ɫ������Ľṹ
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

struct SuperList_OtherInfo//������ɫ,����֮�����Ϣ���趨
{
	int		HeadHeight;			//��ͷ��λ�и� 
	int		LineHeight;			//����ÿ�и߶� 
	char	strSumary[20];		//ȱʡΪ�ϼ� 
	float	InvalidFloat;		//��Ч������,ȱʡΪ7654321.00,֧���в�����
	int		InvalidInt;			//��Ч����,ȱʡΪ87654321,֧���в�����
	UINT	SelChangeMsg;		//��ѡ��仯ʱ���͵���Ϣ;ȱʡֵΪ0,��������Ϣ; 
	UINT	ColSortMsg;			//��������
	UINT	LButtonDblClkMsg;	//˫��ĳ��ʱ���͵���Ϣ;ȱʡֵΪ0,��������Ϣ; 
	UINT	ReturnKeyMsg;		//ѡ��ĳ�а��س���ʱ���͵���Ϣ;ȱʡֵΪ0,��������Ϣ;
	char	Unused[100];		//����
}; 
 
/////////////////////////////////////////////////////////////////////////////////////
//���ú�

#define TDX_LISTCTRLEX_IMAGELIST_LARGE  0 
#define TDX_LISTCTRLEX_IMAGELIST_NORMAL 1 

//����4��������AppendMessage(...,������Ҽ�������ͷ��ĳ��ʱ�������Ҽ��˵�����Ӳ˵���
#define TDX_LISTCTRLEX_RBTN_LISTITEM	0X00000001
#define TDX_LISTCTRLEX_RBTN_LISTHEADER	0X00000002
#define TDX_LISTCTRLEX_SEPARATOR_MASK	0XF0000000 
#define TDX_LISTCTRLEX_ITEM_MASK		0X0FFFFFFF 

//���ڶ����б�ؼ�����ʽ
#define TLVS_AutoArrange		0X00000002	//����Ӧ�п� 
#define TLVS_ColumnHeader		0X00000020	//��ʾ��ͷ 
#define TLVS_SortColumn			0X00000040	//�п����� 
#define TLVS_HeaderOnTop		0X00004000	//��ͷ���ϣ�����ģʽ 
#define TLVS_HasListGrid		0X00010000	//�б�������� 
#define TLVS_ChangeMode			0X00020000	//�����л���������ģʽ 
#define TLVS_SaveOut			0X00040000	//���Ա����б����ݵ��ļ�������� 
#define TLVS_CanFilter			0X00080000	//�Ƿ���ʾ���˲˵� 
#define TLVS_Summary			0X00200000	//������ģʽ

#ifdef   _DEBUG
  #pragma   comment(lib,"TDXGridD.lib")
  #pragma   message("���ӵ����԰� ------> TDXGridD.lib")
#else
  #pragma   comment(lib,"TDXGrid.lib")
  #pragma   message("���ӵ����а� ------> TDXGrid.lib")
#endif

#endif//__INC_H__