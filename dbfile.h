#ifndef	_DATA_BASE_FILE_
#define	_DATA_BASE_FILE_
/////////////////////////////////////////////////////////////////////////////
typedef struct	tagDbfHead	//	DBF ���ݿ��ļ�ͷ
{
	BYTE	DbfFlag;		//	��־��Ϣ
	BYTE	Year;			//	��
	BYTE	Month;			//	��
	BYTE	Day;			//	��
	DWORD	dwCount;		//	�ܼ�¼��Ŀ
	WORD	wHdSize;		//	�ļ�ͷ����
	WORD	wRdSize;		//	ÿ����¼����
	char	Reserved[20];	//	������Ϣ
}DBF_HEAD,*PDBF_HEAD;

typedef	struct	tagDbfField	//	DBF ���ݿ��ֶ���Ϣ
{
	char	FieldName[11];	//	�ֶ���
	BYTE	FieldType;		//	����
	WORD	FieldPos;		//	ƫ�Ƶ�ַ
	WORD	FieldPosLen;	//	���ݳ���
	BYTE	FieldLength;	//	��ֵ����
	BYTE	FieldDec;		//	С����λ��
	char	Reserved[14];	//	������Ϣ
}DBF_FIELD,*PDBF_FIELD;
/////////////////////////////////////////////////////////////////////////////
class	CDBFile
{
public:
	BOOL	Open(LPCSTR FilePath);	//	��
	void	Close();			//	�ر�
	BOOL	Create(LPCSTR FilePath,WORD wFieldNum,PDBF_FIELD pField);	//	����

	BOOL	Goto(DWORD dwRecord);	//	ƫ��,1��ʼ
	BOOL	Skip();				//	��ת��һ��

	WORD	GetFieldNum();		//	�ֶ���
	DWORD	GetRecordCount();	//	��¼��[ʵʱ����]

	PDBF_FIELD	GetFieldInfo(WORD wField);	//	�ֶ���Ϣ
	WORD	GetFieldID(LPCSTR FieldName);	//	�ֶ�ID

	LPSTR	ReadString(LPCSTR FieldName,LPSTR lpString,WORD wMaxRead=0);	//	��
	LPSTR	ReadString(WORD wFieldID,LPSTR lpString,WORD wMaxRead=0);

	BOOL	WriteString(LPCSTR FieldName,LPCSTR lpString,WORD wMaxWrite);	//	д
	BOOL	WriteString(WORD wFieldID,LPCSTR lpString,WORD wMaxWrite);
	BOOL	InsertRecord();	//	׷��
	BOOL	UpdateRecord();	//	д��
public:
	CDBFile();
	~CDBFile();
private:
	HANDLE		z_hFile;
	DBF_HEAD	z_iHd;
	PDBF_FIELD	z_pField;
	WORD		z_nField;
	LPSTR		z_pData;
	DWORD		z_dwRecord;
};
/////////////////////////////////////////////////////////////////////////////
#endif
