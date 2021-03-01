#ifndef REMOTE_H_
#define REMOTE_H_

//远程数据到本地数据的转换函数
char   GetPrecise ( char * code,short setcode);
void   MemStkInfo_rtn3 ( struct remote_TmpStkInfo3 * rp, struct MemStkInfo * np );
void   CurrStockData_rtn ( char * buf, struct CurrStockData * np, short &len ,BOOL b5MMP=FALSE,BOOL bAllInfo=FALSE);
void   CurrStockData_Code_rtn ( char * buf, struct CurrStockData_Code * np, short &len,BOOL b5MMP=FALSE,BOOL bAllInfo=FALSE);
void   CurrStockDataG2_Code_rtn ( char * buf, struct CurrStockData_Code * np, short &len);
void   CurrQueueL2_Code_rtn ( char * buf, struct CurrQueue_Code * np, short &len);
void   CurrStockDataEx_rtn ( char * buf, struct CurrStockDataEx * np, short &len ,BOOL b5MMP=FALSE);
void   MinuteData_rtn ( char * buf, struct MinuteData * np, char precise, char *code, short setcode,short num, short &len,BOOL bLead=FALSE);
void   MmldStruc_rtn ( char * buf, struct MinuteData * np, short num, short &len );
void   VolInStock_rtn ( char * buf, float * np, char * code, short setcode,short num, short &len );
void   simhq_rtn( char *buf,char &setcode,char *code,struct simhq_Info *np,short &len);
void   CdpStruc_rtn( char *buf,struct MemStkInfo *scode,struct remote_cdpInfo *np);
void   TickData_rtn ( char * buf, struct TickData * np, char precise, short num, short &len ,BOOL bVolInStock_dif,BOOL bZS);
void   TickDataL2_rtn ( char * buf, struct TickData_L2 * np, short num, short &len);
void   Bsp1Data_rtn ( char * buf, TICK_MMP * np, struct MemStkInfo *scode, short num, short &len );
void   PriceVolTable_rtn ( struct MemStkInfo * scode, char * buf, struct PriceVolTable * np, short num, short &len );
void   AnalyData_rtn3 (MemStkInfo *scode, char * buf, struct AnalyData * np, short num, short &len );
void   AnalyData_rtn4 (MemStkInfo *scode, char * buf, struct AnalyData * np, short num, short &len );
void   ZmzmStruc_rtn ( char * buf, LPNORMALSTAT np, short num, short &len );
void   JgjcStruc_rtn ( char * buf, LPNORMALSTAT np, short num, short &len );
//
void   CjbsStruc_rtn( char * buf, LPTICKSTAT np, short num, short &len );
void   CjtjStruc_rtn( char * buf, LPTICKSTAT np, short num, short &len );
void   TickStatStruc_rtn(char *buf,LPTICKSTAT np,short num,short &len);

#endif
