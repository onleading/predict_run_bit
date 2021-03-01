#ifndef	_DATA_BASE_FILE_
#define	_DATA_BASE_FILE_
/////////////////////////////////////////////////////////////////////////////
typedef struct	tagDbfHead	//	DBF 数据库文件头
{
	BYTE	DbfFlag;		//	标志信息
	BYTE	Year;			//	年
	BYTE	Month;			//	月
	BYTE	Day;			//	日
	DWORD	dwCount;		//	总记录数目
	WORD	wHdSize;		//	文件头长度
	WORD	wRdSize;		//	每条记录长度
	char	Reserved[20];	//	保留信息
}DBF_HEAD,*PDBF_HEAD;

typedef	struct	tagDbfField	//	DBF 数据库字段信息
{
	char	FieldName[11];	//	字段名
	BYTE	FieldType;		//	类型
	WORD	FieldPos;		//	偏移地址
	WORD	FieldPosLen;	//	数据长度
	BYTE	FieldLength;	//	数值长度
	BYTE	FieldDec;		//	小数点位数
	char	Reserved[14];	//	保留信息
}DBF_FIELD,*PDBF_FIELD;
/////////////////////////////////////////////////////////////////////////////
class	CDBFile
{
public:
	BOOL	Open(LPCSTR FilePath);	//	打开
	void	Close();			//	关闭
	BOOL	Create(LPCSTR FilePath,WORD wFieldNum,PDBF_FIELD pField);	//	创建

	BOOL	Goto(DWORD dwRecord);	//	偏移,1开始
	BOOL	Skip();				//	跳转下一条

	WORD	GetFieldNum();		//	字段数
	DWORD	GetRecordCount();	//	记录数[实时更新]

	PDBF_FIELD	GetFieldInfo(WORD wField);	//	字段信息
	WORD	GetFieldID(LPCSTR FieldName);	//	字段ID

	LPSTR	ReadString(LPCSTR FieldName,LPSTR lpString,WORD wMaxRead=0);	//	读
	LPSTR	ReadString(WORD wFieldID,LPSTR lpString,WORD wMaxRead=0);

	BOOL	WriteString(LPCSTR FieldName,LPCSTR lpString,WORD wMaxWrite);	//	写
	BOOL	WriteString(WORD wFieldID,LPCSTR lpString,WORD wMaxWrite);
	BOOL	InsertRecord();	//	追加
	BOOL	UpdateRecord();	//	写入
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
