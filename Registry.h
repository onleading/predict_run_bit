#ifndef __REGISTRY__H__
#define __REGISTRY__H__

typedef struct
{	char	strBlockName[9];
	short	nStockNum;
	short   nContent[USER_SIZE];
}MEM_BLOCK;

typedef struct
{	char    strBlockName[9];
	short   nStockNum;
	short	BlockAttr;
	char    strContent[USER_SIZE][7];
}DSK_BLOCK;

//////////////////////////////////////////

#define FREE			-1
#define USED			0
#define EMPTY			-1

#define RT_KEY			0
#define RT_VALUE		64
#define RT_LIMIT		128
#define RT_ANY          256

#define RT_CHAR         1
#define RT_CARRAY		2
#define RT_CHARSTRING	3
#define RT_SHORT		4
#define RT_INT			5
#define RT_LONG			6
#define RT_USHORT		7
#define RT_UINT			8
#define RT_ULONG		9
#define RT_FLOAT		10
#define RT_DOUBLE		11
#define RT_LDOUBLE      12
#define RT_PTR          13
#define RT_USRDEF		14

#define MAXNAMELEN      64

typedef struct	tagRegHead
{	char	strVersion[MAXNAMELEN];     //注册表的版本序号
	long    lIndexOffset;				//索引表的偏移		(字节偏移)
	long    lDataOffset;        		//数据区的偏移		(字节偏移)
	long    lIndexSize;         		//索引区的大小		(字节数)
	long    lDataSize;					//数据区的大小		(字节数)
	long    lIndexRecNum;				//索引的条数
}REGHEAD;


//键名,值名索引表
//      名称			键名
//						值名
//      类型			键名 RT_KEY  +数据类型(若为RT_KEY    则值为空)
//						值名 RT_VALUE+数据类型(若为RT_VALUSE 则值为空)
//      数据            RT_KEY...  键的数据
//						RT_VALUE...值的数据
//      长度			RT_KEY...  键的数据的长度
//						RT_VALUE...值的数据的长度
//      偏移			RT_KEY...  键的数据的长度(相对数据区)
//						RT_VALUE...值的数据的长度(相对数据区)
//		前键或值索引	上值或上键的索引号//若为根或为首节点则为EMPTY
//		次键或值索引	次值或次键的索引号//若为根或为尾节点则为EMPTY
//      父键索引		键或值从属的键(若为根则为EMPTY)
//		首子键或值索引	首值或首键的索引号

typedef struct	tagRegIndex
{	char    strName[MAXNAMELEN];
	long    lType;
	char*   pData;
	long    lLength;
	long    lOffset;
	long    lPrevIndex;
	long    lNextIndex;
	long    lParentIndex;
	long    lChildIndex;
	long    bUsed;
}REGINDEX;

class CRegistry
{
public:
	CRegistry(char * strVersion);
	virtual	~CRegistry();
	long	OpenReg(char* lpzRegDBPath,short bCreate=FALSE);
	long    SaveReg(char* lpzRegDBPath);
	long    RegWrite(char*lpzBranchPath,long lType=RT_KEY,void*pData=NULL,long lSize=-1);
	long    RegRead(char*lpzBranchPath,void* pData,long nMaxBytes);
	LPSTR	Peekstr(char*lpzBranchPath);
private:
	REGHEAD		m_RegHead;
	REGINDEX	*m_pRegIndex;
	short       m_bRegSafe;
	char        m_strVersion[MAXNAMELEN];
private:
	long    FindBranch(char*lpzBranchPath);
	long    LocateChild(char*lpzChildName,long lParentIndex,long lType);
	long    NewIndex(char*lpzName,long lType,void*pData,long lLength,long lParentIndex);
	long	SetIndexData(char*lpzName,long lType,void*pData,long lLength,long lTargetIndex);
};
#endif