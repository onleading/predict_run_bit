/**************************************************/
/*				Dbf动态连接库导出头文件           */
/**************************************************/
#ifndef _DBF_H_
#define _DBF_H_

#ifdef DBF_EXPORTS
#define DBF_API __declspec(dllexport)
#else
#define DBF_API __declspec(dllimport)
#endif

#define         TRUE                     1
#define         FALSE                    0

#define		LOCK_F_OFFSET	0x40000000l
#define		LOCK_C_OFFSET   1000000000l
#define		LOCK_FILE_LEN	0x3fffffffl

#define		PATH_LENGTH		80
#define 	MAX_STATION		8

#define		LAYER			6
#define		MAXRETNO		200
#define		ROOT			1
#define		BRANCH			0
#define		IDXOVERFLOW		-2
#define		READ_IDX_ERR		-3
#define		SEEK_IDX_ERR		-4
#define		KEYWORD_ERR		-5
#define         MAX_FLD_NUM             20
#define         CH_SPACE              0x20

#define		SUCCESS				0
#define		FAIL                -1

#define		NO_CONFIG_FILE		-2
#define		NOT_ENOUPH_KEYWORD	-3
#define		TOO_MANY_KEYWORD	-4
#define		PATH_TOO_LONG		-5
#define		FILE_NOT_FOUND		-6
#define		TOO_MANY_STATION	-7

#define		APPEND_DBF   	        -1l

#define		SEEK_DBF_ERR		-1l
#define		READ_DBF_ERR		-2l
#define		WRITE_DBF_ERR		-3l
#define		REC_NUMBER_ERR		-4l
#define     OPEN_DBF_ERR            -5l
#define     ALLOC_DBF_ERR           -6l
#define     DBF_DESTROYED           -7l
#define     DBF_INCOMPATIBLE        -8l
#define     INIT_DBF_ERR            -9l

#define     SUCCEED                  0
#define     OPEN_ERROR              -1
#define     ALLOC_ERROR             -2
#define     DBF_ERROR               -3
#define     READ_ERROR              -4
#define     WRITE_ERROR             -5
#define     LOCK_FAILED             -6
#define		WAIT_TIME		10     /* seconds */
#define     VALID                    1
#define     INVALID                  0
#define     ENDOFFILE             0x1a
#define     MAX_NDX_LEVEL           10
#define     MAX_FLD_NUM             20

#define      BLOCK_SIZE             0x200

typedef struct TAGHEADER
{  long           root ;            /* -1 means unknown */
   long           free_list ;       /* start of the free list (-1 if none) */
   unsigned long  version ;         /* used multi-user only */
   short          key_len ;         /* Key Length */
   char           type_code;        /* 0x01 Uniq; 0x08 For Clause; 0x32 Compact; 0x80 Compound */
   char           signature ;       /* unused */

/* char           dummy2[482] ;        unused */
   char           dummy3[4] ;
   short          descending   ;    /* 1 = descending, 0 = ascending */
   short          filter_pos ;      /* not used, == to expr_len */
   short          filter_len ;      /* length of filter clause */
   short          expr_pos ;        /* not used, == to 0  */
   short          expr_len ;        /* length of expression */
/* char           expr_pool[512] ;  expression and filter pool */
}  TagHeader;

typedef struct KEYDATA
{  long  num ;
   char  value[1] ;  /* The key size is variable */
} KeyData;

/* the following structure is used only on the leaf nodes of the tree structure */
typedef struct NODEHEADER
{  short            free_space ;        /* # bytes available in node */
   unsigned long    rec_num_mask ;      /* record number mask */
   char             dup_byte_cnt ;      /* duplicate byte mask count */
   char             trail_byte_cnt ;    /* Trailing byte mask count */
   char             rec_num_len ;       /* # bits used for record number */
   char             dup_cnt_len ;       /* # bits used for duplicate count */
   char             trail_cnt_len ;     /* # bits used for trail count */
   char             info_len ;          /* # bytes for holding record number, */
} NodeHeader;

typedef struct BLOCKHEADER
{  short      node_attribute ;    /* 0=index, 1=root, 2=leaf */
   short      n_keys ;            /* Block Image starts here */
   long       left_node ;         /* -1 if not present */
   long       right_node ;        /* -1 if not present */
} BlockHeader;

typedef struct BLOCK
{  TagHeader *tag;
   short key_on ;         /* The current key within the block */
   short cur_trail_cnt ;  /* current value used for seeking */
   short cur_dup_cnt ;    /* current value used for seeking */
   short dup_pos ;        /* bit offset into the info for the duplicate data */
   short trail_pos ;      /* bit offset into the info for the trail data */
   short rec_pos ;        /* bit offset into the info for the record # data */
   char  *cur_pos ;       /* current position into the data (starts at end) */
   short built_on ;       /* the 'current' key value (i.e. key really 'on') */
   char  *built_pos ;     /* position where built on */
   KeyData *built_key ;
   BlockHeader header ;
   NodeHeader node_hdr ;  /* only if the block is a leaf */
   char  data[1] ;        /* the remaining data */
} Block;

typedef struct FLDINFO {
      char          name[10];        /* node type                      */
      char          null1;           /* fixed to 0x00                  */
      char          type;            /* field type                     */
      short         offset;          /* field offset in a record       */
      char          null2[2];        /* null chars                     */
      BYTE          wid;             /* field length                   */
      char          dec;             /* point number length            */
      char          null3[14];       /* used by system                 */
      } FieldStruct;

typedef struct DBFINFO {
      char id;   //.DBF标识    /* the ID.for a FoxBase+ DBF File */
      char          year,month,day;  //文件最后修改日期
      long          RecNum;  //总记录数
      short         HeadLen; //.DBF头文件长度    /* length of DBF head             */
      short         RecLen;  //记录长度          /* length of each record          */
      char	  * RecBuf;  //记录Buffer指针
      FieldStruct * Field;   //字段结构
      short	    FieldNum;//字段数
      short	    fpt;     //DBF句柄
      long	    NowRec;  //当前记录号???
      char          null[4]; /* null char in the info. unit    */
      short	    fptfpt;  //.FPT句柄
      short	    fptflag; //.FPT标志
      short	    fptreclen;  //.FPT中记录长度
      short         fptcdx;    //.CDX句柄
      TagHeader     *tag_index;
      Block         *block;
      }  DBH_Head;

typedef struct IDXHEADER {
      long          root;
      long          ff;
      long          new_node;
      short         key_len;
      } IdxHeader;

typedef struct IDXNODEHEADER {
      char          flag;
      char          zero1;
      char          item_num;
      char          zero2;
      long          left_node;
      long          right_node;
      } IdxNodeHeader;

typedef struct IDXINFO {
		      short         fpt;
		      IdxHeader     head;
		      char          *buf;
		      short         cur;
		      } IDX_Head;

typedef struct IDXHEAD{
	       short sign;
	       short version;
	       long root;
	       long eof;
	       short item_len;
	       short key_len;
	      } 	    IdxHead;

typedef struct NDXHEAD{
		   char             raddr[4];         /* the relative address of root node  */
		   char             hl[4];            /* label of boot node,normally 0xFFFFFFFF */
		   char             total[4];         /* total number of nodes */
		   char             keylen[2];        /* length of index key word */
		   char             keytype;          /* 0 if key word is char.type,else 1 */
		   char             exp[100];         /* index expresiion */
		  }          NdxHd;


typedef struct NDXNODE{
		   char    type;              /* node type */
		   char    c1;                /* normally 0 */
		   char    ndxnum;            /* valide number of index items */
		   char    c2;                /* normally 0 */
		   unsigned long    preptr;         /* pre_pointer of the node,0xFFFFFFFF if no pre_node */
		   unsigned long    postptr;        /* post_pointer of the node,0xFFFFFFFF if no post_node */
		   char    item[500];         /* index item */
			}           NdxNd;

typedef struct NDLEVEL {
		   char             id;                /* id this level is valide or invaid */
		   short            lastbv;
		   short            endbv;
		   short            bv;                /* boundary value for this level */
		   short            ioffs;             /* index item offs in this block node */
/*		   struct NDXNODE   lnd;  */             /* hold node for this level */
		   }                NdLevel;

typedef struct NDXREC {
		   char             id;                /* if valid,ptr is record no indexed out,else
							  it's pointer to node */
		   short            SeqNo;
		   char             kw[20];               /* pointer to key word needed to index */
		   long             ptr;               /* pointer to next block node or record no */
		  }                 NdxRec;

#define  BaseDataTop(DBH)  ReadDbf(DBH,1l)
#define  BaseDataBottom(DBH)     ReadDbf(DBH,GetDbfRecNum(DBH))


DBF_API	short  OpenBase( DBH_Head *DBHptr,char * filename,short access);
DBF_API	short  CloseBase(DBH_Head *DBHptr);
DBF_API	short  CheckBase(DBH_Head * DBHptr);
DBF_API	short  LockDbf(DBH_Head * DBHptr, long rec);
DBF_API	short  UnlockDbf(DBH_Head * DBHptr, long rec);
DBF_API	short  LockBase(DBH_Head * DBHptr);
DBF_API	short  UnlockBase(DBH_Head * DBHptr);
DBF_API	short  LockIndex(short idx_fpt);
DBF_API	short  UnlockIndex(short idx_fpt);
DBF_API	long GetDbfRecNum(DBH_Head * DBHptr);
DBF_API	long ReadDbfNow(DBH_Head * DBHptr);
DBF_API	long ReadDbf(DBH_Head * DBHptr,long rec);
DBF_API	long ReadDbfN(DBH_Head * DBHptr, char *str, long rec, long rec_num);
DBF_API	long WriteDbfNow(DBH_Head * DBHptr);
DBF_API	long WriteDbf(DBH_Head * DBHptr, long rec);
DBF_API	short  WriteIndex(short idx_fpt,char *keyword,long rec);
DBF_API	short  GetFieldSeq(DBH_Head * DBHptr,char * fname);
DBF_API	short  GetFieldStr(DBH_Head * DBHptr,char * fname,char * vari);
DBF_API	short  GetFieldSeq2(DBH_Head * DBHptr,char * fname);
DBF_API	short  GetFieldStr2(DBH_Head * DBHptr,char * fname,char * vari);
DBF_API	long Skip(long n,DBH_Head *DBH);
DBF_API	void formatstr(char *buf,char *vari,short type,short width,short dec);
DBF_API	short  PrevFieldStr(DBH_Head * DBHptr,char *fname,char * vari);
DBF_API	short  PrevFieldStrLeft(DBH_Head * DBHptr,char *fname,char * vari);
DBF_API	short  GetMemoStr(DBH_Head * DBHptr,char *fname,char * buffer);
DBF_API	short reverse_short(short val);
DBF_API	long reverse_long(long val);
DBF_API	long CdxTop(DBH_Head *DBHptr,char *tag_name);
DBF_API	long CdxBottom(DBH_Head *DBHptr,char *tag_name);
DBF_API	long CdxSeek(DBH_Head *DBHptr,char *tag_name,char *key,short descending);
DBF_API	long CdxSeekDouble(DBH_Head *DBHptr,char *tag_name,double key,short descending);
DBF_API	long CdxContinue(DBH_Head *DBHptr,char *key,short descending);
DBF_API	long CdxContinueDouble(DBH_Head *DBHptr,double key,short descending);
DBF_API	short  IdxSeek_c(short fpt,char *keyword,long *rec,short maxcount);
DBF_API	short  IdxSeek_f(short fpt,char *keyword,long *rec,short maxcount);
DBF_API	short  IdxSeek_double(short fpt,double keyword,long *rec,short maxcount);
DBF_API	void set_secret(void);
DBF_API	void clr_secret(void);
DBF_API	char * AllocRecbuf(DBH_Head *DBHptr);
DBF_API	long ReadDbfNowToBuf(DBH_Head * DBHptr,char * RecBuf);
DBF_API	short  GetFieldStrFromBuf(DBH_Head * DBHptr,char * fname,char * vari,char * RecBuf);
DBF_API	short  OpenIdx(IDX_Head *IDXptr,char *filename,short access);
DBF_API	short  CloseIdx(IDX_Head *IDXptr);
DBF_API	long IdxSeek(IDX_Head *IDXptr,char *keyword);
DBF_API	long IdxContinue(IDX_Head *IDXptr,char *keyword);
DBF_API	long IdxSeekDouble(IDX_Head *IDXptr,double keyword);
DBF_API	long IdxContinueDouble(IDX_Head *IDXptr,double keyword);
DBF_API	long MemGetDbfRecNum( DBH_Head * DBHptr, char * buf );
DBF_API	long MemReadDbf( DBH_Head * DBHptr, long rec, char * buf );
DBF_API	long MemReadDbfNow( DBH_Head * DBHptr, char * buf );
DBF_API	long MemSkip( long step, DBH_Head * DBHptr, char * buf );
#endif