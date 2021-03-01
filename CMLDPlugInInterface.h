/********************************************************
*														*
*			�����״���ʾ�����						*
*														*
*			�Ϻ��������								*
*														*
*********************************************************/

#ifndef __CMLD_PLUGIN_INTERFACE__
#define __CMLD_PLUGIN_INTERFACE__

#pragma pack( push, 1 )
//	��������
typedef struct	tagLINEDAYITEM
{
	DWORD	dwYYYYMMDD;
	float	open;
	float	high;
	float	low;
	float	close;
	float	volume;
	float	amount;				//	�������ݻ���Ҫ����
}LINEDAYITEM;
#pragma pack(pop)

// ֤ȯ�г�
#define		SH_MARKET_EX			'HS'		// �Ϻ�
#define		SZ_MARKET_EX			'ZS'		// ����

typedef struct tagONE_STK_LABEL
{
	WORD	wMarket;			//	�г�����
	char	szLabel[10];		//	��Ʊ����
}ONE_STK_LABEL;

#define	MAX_PARAM	6
typedef struct tagTDX_CALCINFO
{
	const DWORD m_dwSize;		//�ṹ��С
	const DWORD m_dwVersion;	//��������汾(V2.10 : 0x210)
	const DWORD m_dwSerial;		//����������к�
	const ONE_STK_LABEL m_strStkLabel; //��Ʊ����
	const BOOL m_bIndex;		//����

	const int m_nNumData;		//��������(pData,pDataEx,pResultBuf��������)
	const LINEDAYITEM* m_pData; //��������,ע��:��m_nNumData==0ʱ����Ϊ NULL

	// ÿ��m_pfParam�������飬����Ϊm_nNumData����m_pResultBufһ��
	// ���������Ĺ�ʽ
	// A:=MA(CLOSE,10);
	// B:=MA(CLOSE,20);
	// C:PLUGFUNC_X(A,B);
	// A��B��������m_pfParam����
	const float* m_pfParam[MAX_PARAM];	//���ò���

	float* m_pResultBuf; //���������
}TDX_CALCINFO;

//////////////////////////////////////////////////////////////////////////
interface ICMLDPlugIn_Client;

//-------------------------------------------------------------------
//	ϵͳ�ṩ�ĺ��������ʹ�ã����Ӧ�ñ����ýӿ�ָ�룬�Ա�ʹ��
interface ICMLDPlugIn_Server
{	
	// ������������ȡ��������������������ʹ��
	virtual DWORD  GetCapability( int nIndex ) = 0;

	// ��ȡ�û��ʻ�
	virtual LPCSTR GetUserAccount() = 0;
	// ��ȡ�û���¼���룬Ҳ�����Ǵ������û����룬����Ψһ��ʾ����ԭ��������
	virtual LPCSTR GetUserPassword() = 0;
	
	//	�л�����������������ƶ���pLabelIDָ���Ĺ�Ʊ��
	virtual	void	SwitchToReportView( ONE_STK_LABEL * pLabelID, ONE_STK_LABEL * pOrderList, int nCount )=0; 
	//	�л���pLabelIDָ����Ʊ��K�ߴ��ڣ�
	//	pOrderList��nCountΪ��ѡ�������� pOrderList ��Ϊ NULL ʱ��Ҫ��������ʾ���ڰ���ָ����˳���л���Ʊ���簴 PageDown �·���Ʊʱ)
	virtual	void	SwitchToKlineView( ONE_STK_LABEL * pLabelID, ONE_STK_LABEL * pOrderList, int nCount)=0;
	//	�л���pLabelIDָ����Ʊ��F10����
	virtual	void	SwitchToF10View( ONE_STK_LABEL * pLabelID)=0;
	//	ִ�п�ݼ����硰03������04����
	virtual	void	OnKeyboardGeniusCmd( LPCSTR lpszCmd )=0;
	//	��ȡpLabelIDָ���Ĺ�Ʊ���ƣ�����NULL��ʾʧ��
	virtual	LPCSTR	GetStkName( ONE_STK_LABEL * pLabelID )=0;

	// ��ȡָ����Ʊ���߸�����
	// ���ڷ����ڴ�
	virtual int		GetLineDayDataSize( ONE_STK_LABEL * pLabelID ) = 0;
	//	��ȡ pLabelID ָ���Ĺ�Ʊ���� nStartDate��nEndDate ֮����������ݣ�
	//	pnOutDayCount ����������ݸ���
	//	bDoPowerSplit ָ������������Ƿ���Ҫ��Ȩ, TRUE ��ʾ��Ҫ��Ȩ
	//  ���� �����������飬���ñ�����ͨ�� FreeMemory �����ͷš�
	virtual LINEDAYITEM * GetLineDayData( ONE_STK_LABEL * pLabelID, int nStartDate, int nEndDate, int * pnOutDayCount, BOOL bDoPowerSplit ) = 0;

	// ��ȡ��Ʊ�б�
	// wMarketType			�г����ͣ�SH_MARKET_EX or SZ_MARKET_EX
	//						0 ��ʾȫ���г�
	// pnCount				�����Ʊ����
	// ���أ�
	//		��Ʊ�������飻�����ڴ潫ͨ�� FreeMemory �ͷš�
	virtual ONE_STK_LABEL * GetStockLabelList( WORD wMarketType, int * pnCount ) = 0;

	//	ע�����˵����˵�ID��ϵͳ�Զ����䡣
	//	pClientObj		�������
	//	lpszMenuItem	���˵����еĲ˵�����
	//	alpszSubMenu[0] ~ alpszSubMenu[nSubMenuCount-1]	Ϊ�Ӳ˵����� alpszSubMenu[i] = "-" ���ʾΪ�ָ���
	//	nSubMenuCount	Ϊ�˵������
	//	ahMenuBmp		Ϊ��˵����Ӧ��ͼ�Σ��� ahMenuBmp[i] = NULL����ʾ��Ӧ alpszMenu[i]û��ͼ��
	//					ϵͳ��Ҫ������Ӧ�� Bitmap�����ú������غ� ahMenuBmp �е� bitmap ���ܱ��ͷ�
	virtual BOOL	RegisterMenu( ICMLDPlugIn_Client * pClientObj, LPCSTR lpszMenuItem, LPCSTR alpszSubMenu[], int nSubMenuCount, HBITMAP ahMenuBmp[] ) = 0;

	// �ͷ��ɷ�����������ڴ�
	virtual void FreeMemory( void * pBuf ) = 0;	
	
	// ������������������ӿ�	
	virtual HRESULT QueryInterface( REFIID iid, void **ppvObject ) = 0;	
};

//--------------------------------------------------------
//	�ͻ��˽ӿڣ����������ò�����������ᱣ���ýӿ��Ա�ʹ��
interface ICMLDPlugIn_Client
{
	// ��ʼ�����
	virtual void Init( HWND hMainWnd, ICMLDPlugIn_Server * pServer ) = 0;
	// �������˳�ǰҪ���������Դ
	virtual void Exit() = 0;

	//	��Ӧ�˵��¼�
	virtual void OnMenuCommand( LPCSTR lpszMenuString ) = 0;
	//	�������ݣ����� TRUE,�û�����ȡ����ť��FALSE �û�û�а�ȡ����ť
	virtual BOOL OnDownloadData() = 0;
	
	// �ͷ��ɲ��������ڴ�
	virtual void FreeMemory( void * pBuf ) = 0;
	// ��ȡ�ӿ�
	virtual HRESULT QueryInterface( REFIID iid, void **ppvObject ) = 0;		// ��������������ӿ�	
};


//--------------------------------------------------------
//	�����ӿ�
extern "C" {

	//	��ȡ����ӿڣ��Ա�����������WINAPI	
	__declspec(dllexport) ICMLDPlugIn_Client * GetClientInterface();
};

//ָ�����ĺ�������
#define PLUGIN_CMLD __declspec(dllexport)

extern "C" PLUGIN_CMLD BOOL	PLUGINFUNC_A(TDX_CALCINFO*pCalcItem);


#endif // __CMLD_PLUGIN_INTERFACE__
