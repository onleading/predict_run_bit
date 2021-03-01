#ifndef CVCHART__H__H
#define CVCHART__H__H
#include <afxwin.h>
#include <vector>
#include <string>

//��̬��ͼ
class DynaChartItem
{
public:
	//��̬��ͼ�ĸ��ֿ���ֶ�
	std::vector<std::string> name;
	//��̬��ͼ�ĸ��ֿ������
	std::vector<std::string> vtitle;
	//��̬��ͼ�ĸ��ֿ����ɫ
	std::vector<COLORREF> color;
	//��ͼ�ı���
	std::string title;
};


//��̬ͼ��
//for StaticYItemType, nType
#define TCSS_YITEM_LINE	0X00000000//��y����ʾΪ����,ͬһ����̬ͼ�и���y�������ʾΪ���߻���,ͬһ��y��Ҳ������ͬһ��ͼ��ͬʱ��ʾΪ���ߺ���
#define TCSS_YITEM_POLE	0X00000001//��y����ʾΪ��װ,ͬһ����̬ͼ�и���y�������ʾΪ���߻���,ͬһ��y��Ҳ������ͬһ��ͼ��ͬʱ��ʾΪ���ߺ���
#define TCSS_YITEM_MASK	0X00000001//����
//for StaticYItemType, nType,������Ŀ̶ȵ�λ���ܲ�ͬ
#define TCSS_AXIS_LEFT	0X00000000//��y��ʹ����������̶�ֵ,���һ����̬ͼ������y�ᶼʹ������,�����᲻�ᱻ����
#define TCSS_AXIS_RIGHT 0X00000010//��y��ʹ����������̶�ֵ,���һ����̬ͼ������y�ᶼʹ������,�����᲻�ᱻ����
#define TCSS_AXIS_MASK	0X00000010//����

//��̬ͼ��Ķ���,һ��x��,���y��
class StaticChartItem
{
public:
	class StaticXType
	{
	public:
		std::string name;
		std::string title;
	};
	class StaticYItemType
	{
	public:
		StaticYItemType(){}		
		StaticYItemType(const std::string& _name, int _nType,
						const COLORREF& _color, const std::string& _caketitle, 
						const std::string& _poletitle, const std::string& _title)
		{
			name	  = _name;
			nType	  = _nType;
			color	  = _color;
			caketitle = _caketitle;
			poletitle = _poletitle;
			title	  = _title; 

		}
		StaticYItemType(const std::string& _name, int _nType,
						const COLORREF& _color, const std::string& _caketitle,
						const std::string& _poletitle)
		{
			name	  = _name;
			nType	  = _nType;
			color	  = _color; 
			caketitle = _caketitle;
			poletitle = _poletitle;
		}		
		UINT		nType;		//����ѡ��,����������
		COLORREF    color;
		std::string name;
		std::string title;		//���Ϊ���߻�����y�����
		std::string caketitle;	//���Ϊ��,���ı���
		std::string poletitle;	//���Ϊ�����������ı���
	};

	UINT							nFlag;
	StaticXType						xname;
	std::vector<StaticYItemType>	ynames;
};


//�ǿ�ͼ,����3ά;x,y��(StarColumnType)��Ϊ��ֵ;Ȼ��ÿһ������һ������(StarMainType)
class StarChartItem
{
public:
	class StarColumnType
	{
	public:
		std::string name;
		std::string title;
		COLORREF	color;
	};
	class StarMainType
	{
	public:
		std::string name;
		std::string title;
		COLORREF	color;
	};

	int				type;
	StarColumnType	xname;   //x��
	StarColumnType	yname;	//y��
	StarMainType	mainname;//z��
};

#define TDX_DLLCHART_CLR_NUM	20	//ͼ����ɫ

struct ChartColorFont
{
	COLORREF BkColor;				//ϵͳ������ɫ
	COLORREF TitleTextColor;		//ϵͳ������ɫ
	COLORREF ChartBkColor;			//ͼ������ɫ
	COLORREF ChartTxtColor;			//ͼ���ı���ɫ
	COLORREF AxisTxtColor;			//���ı���ɫ
	COLORREF LightColor;			//ͼ����������ɫ
	COLORREF AxisColor;				//���ߵ���ɫ
	COLORREF BrushColor[TDX_DLLCHART_CLR_NUM];		//ͼ������ɫ

	LOGFONT	 TitleFont;				//��������
	LOGFONT	 DescFont;				//ͼ��ʹ�õ�����
	LOGFONT	 ChartFont;				//��������õ�����
};

//֧�ֵ�������ͼ��
#define TCS_STATIC					0X00000001	//ͼ��Ϊ��̬��
#define TCS_DYNA					0X00000002	//��̬��
#define TCS_STAR					0X00000004	//�ǿ�ͼ
#define TCS_STYLE					0X00000007	//�����������

//��̬�������С��
#define TCS_CAKE					0X00000000	//��ͼ
#define TCS_PARA					0X00000010	//����������,��������ϵ
#define TCS_ALLINONE				0X00000020	//�ۺ���,������ͬһ����ϵ

#define TCS_STATIC_STYLE			0X00000030	//��С������룬ChangeStyle()ʱ�����õ�
#define TCS_STATIC_STYLE_CHANGEABLE	0X00000040	//����Ǿ�̬����ָ���ɷ��Ϊ���л��ۺϣ����Ϊ���л��ۺϣ�ָ���Ƿ�ɱ�Ϊ��ͼ

#define TCS_VERT					0X00000080	//����PARA��ALLINONE���Ƿ�ߵ�xy����

//ͼ����
#define TCS_HGRID					0X00000100	//�Ƿ���ʾ����������
#define TCS_VGRID					0X00000200	//�Ƿ���ʾ����������
#define TCS_3D						0X00000400	//ͼ���Ƿ�֧��3dЧ��
#define TCS_XFLOATINGBOX			0X00001000	//�Ƿ�����ʾx���α�
#define TCS_YFLOATINGBOX			0X00002000	//�Ƿ�����ʾy���α�
#define TCS_FLOATINGBOX				(TCS_XFLOATINGBOX|TCS_YFLOATINGBOX)

#define TCS_TOOLTIP_AUTOWANYI		0X00004000	//ToolTip�Ƿ��Զ������ڱ�ʾ��

#define TCS_SYSCLR_DYNA				0X00010000	//��̬ͼ��ʹ��ȱʡ����ɫ
#define TCS_SYSCLR_STATIC			0X00020000	//��̬ͼ��ʹ��ȱʡ����ɫ

//���ھ�̬ͼ�ۺ�ͼ��ͼ��
//1:ͼ��λ��
#define TCS_ALLINONE_EXAMPLE_TOP		0X04000000	//������ͼ�Ͽ��ƶ�
#define TCS_ALLINONE_EXAMPLE_RIGHT		0X08000000	//�̶�������ͼ֮��
#define TCS_ALLINONE_EXAMPLE_POS		0X0C000000	//����λ�õ�����
//2:ͼ����ʾģʽ
#define TCS_ALLINONE_EXAMPLE_1Row		0X00000000	//��ʾΪһ��
#define TCS_ALLINONE_EXAMPLE_1Col		0X10000000	//��ʾΪһ��
#define TCS_ALLINONE_EXAMPLE_2Col		0X20000000	//��ʾΪ����
#define TCS_ALLINONE_EXAMPLE_2Row		0X30000000	//��ʾΪ����
#define TCS_ALLINONE_EXAMPLE_ROW_COL	0X30000000	//��ʾģʽ����

//��ͼ��ؼ�����Ӹ�ͼ��Ĳ���:1:���ͼ��;2:��ͼ�����������
class VChart: public CWnd
{
public:
	//�ӿ�
	virtual DWORD GetStyle(DWORD dwMask = 0XFFFFFFFF) = 0;
	virtual BOOL  ChangeStyle(DWORD dwRevmove, DWORD dwAdd = 0, UINT nFlag = 0) = 0;

	//��������ʱ�������ˢ��ʹ�ò���bRefresh=TRUE������Ҫʹ��Invalidate();
	//���õ�index(0 based)�ǿ�ͼ�����������ᣬx�����ᣬy������
	virtual BOOL SetStarMainData(int index, const std::vector<std::string>& maindata, BOOL bRefresh = FALSE) = 0;
	virtual BOOL SetStarXData(int index, const std::vector<double>& pData, BOOL bRefresh = FALSE) = 0;
	virtual BOOL SetStarYData(int index, const std::vector<double>& pData, BOOL bRefresh = FALSE) = 0;

	//���þ�̬ͼ��x������͵�index��y����������
	virtual BOOL SetStaticXData(const std::vector<std::string>& xdata, BOOL bRefresh = FALSE) = 0;
	virtual BOOL SetStaticYData(int index, const std::vector<double>& pData, BOOL bRefresh = FALSE) = 0;

	//���ö�̬��������
	virtual BOOL SetDynaChartData(int index, const std::vector<double>& data, BOOL bRefresh= FALSE)= 0;

	//���ɾ���޸��ǿ�ͼ
	virtual int  AddStarChart(const StarChartItem& item) = 0;
	virtual BOOL DeleteStarChart(int index = -1) = 0;
	virtual BOOL ModifyStarChart(int index, const StarChartItem& item) = 0;

	//���ɾ���޸ľ�̬ͼ����
	virtual int  AddStaticYChart(const StaticChartItem::StaticYItemType& item) = 0;
	virtual BOOL DeleteStaticYChart(int index = -1) = 0;
	virtual BOOL ModifyStaticYChart(int index, StaticChartItem::StaticYItemType& item)= 0;
	virtual BOOL ModifyStaticXChart(StaticChartItem::StaticXType& xname) = 0;

	//���ɾ���޸Ķ�̬ͼ����
	virtual int  AddDynaChart(const DynaChartItem& item) = 0;
	virtual BOOL DeleteDynaChart(int index = -1) = 0;
	virtual BOOL ModifyDynaChart(int index, const DynaChartItem& item) = 0;
	
	//����ø�ͼ��,ֱ������ؼ�
	virtual void  SetStarChart(const std::vector<StarChartItem>& StarChart) = 0;
	virtual const std::vector<StarChartItem>& GetStarChart() const = 0;
	virtual void  SetStaticChart(const StaticChartItem& StaticChart) = 0;
	virtual const StaticChartItem& GetStaticChart() const = 0;
	virtual void  SetDynaChart(const std::vector<DynaChartItem>& DynaCharts) = 0;
	virtual const std::vector<DynaChartItem>& GetDynaChart() const = 0;

	//������ɫ��
	virtual void  SetChartColorFont(const ChartColorFont& mgt) = 0;
	virtual const ChartColorFont& GetChartColorFont() const = 0;

	//��̬ͼ����ã�����ѡ������
	virtual void  SetStaticSel(int index,int nSel = -1,COLORREF rgb=RGB(128,128,128)) = 0;

	//ͼ�����仯ʱ(������)����RefreshChart()Ȼ����Invalidate();
	virtual void RefreshChart() = 0;
};

#ifdef  IMPL_TDX_CCHARTD
	#define TDX_CCHARTD_DLL __declspec(dllexport)
#else
	#define TDX_CCHARTD_DLL __declspec(dllimport)
#endif

//ͼ��ؼ��Ĵ�����ɾ��ʹ��������������,������ʹ��new delete
TDX_CCHARTD_DLL VChart* CreateNewTChart(DWORD dwStyle, HWND hWnd, CRect rect, UINT nID);
TDX_CCHARTD_DLL void DeleteTChart(VChart** ppVChart);

#endif
