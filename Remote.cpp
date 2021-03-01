#include "stdafx.h"
#include "Remote.h"
/************************************************************/
/*			远程数据到本地数据的转换函数					
/************************************************************/

char GetPrecise ( char * code,short setcode)
{
  struct MemStkInfo * scode;

  scode = g_pStockIO->Get_MemStkInfo ( code,setcode );
  if ( scode != NULL ) return scode->tp_flag;
  else                 return 2;
}

void CurrStockData_rtn ( char * buf, struct CurrStockData * np, short &len, BOOL b5MMP, BOOL bAllInfo)
{
  short  i, pos = 0;
  float  factor;
  char   code[SH_CODE_LEN + 1]={0},setcode,precise;
  double ftmp;

  memmove( &setcode, buf + pos, 1 );
  memmove( code, buf + pos + 1, SH_CODE_LEN );
  code[SH_CODE_LEN] = '\0';
  pos    += 1+SH_CODE_LEN;
  factor  = 100.0;
  precise = GetPrecise ( code,setcode );
  if ( precise > 2 )
  {
     for ( i = 0; i < precise - 2; i++ )
         factor *= 10.0;
  }  
  np->ItemNum = 0;
  memmove ( & ( np->ItemNum ) , buf + pos, sizeof ( WORD ) );
  pos += sizeof ( WORD );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Now = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Close = (float)( ftmp / factor + np->Now );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Open = (float)( ftmp / factor + np->Now );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Max = (float)( ftmp / factor + np->Now );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Min = (float)( ftmp / factor + np->Now );
  pos += BufToDouble ( ftmp, buf + pos );
//  np->RefreshNum = (long)( ftmp / factor + np->Now ); //不取此数据,在CurrStockDataG2_Code_rtn中使用
  pos += BufToDouble ( ftmp, buf + pos );
  if ( testzs ( code,setcode ) )
     np->Lead = (float)( ftmp / 100.0 ); // 领先指标
  else
     np->Lead = (float)( ftmp / factor + np->Now );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Volume = (DWORD)ftmp;
  pos += BufToDouble ( ftmp, buf + pos );
  np->NowVol = (DWORD)ftmp;
  memmove ( & ( np->Amount ) , buf + pos, sizeof ( float ) );
  pos += sizeof ( float );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Inside = (DWORD)ftmp;
  pos += BufToDouble ( ftmp, buf + pos );
  np->Outside = (DWORD)ftmp;
  pos += BufToDouble ( ftmp, buf + pos );
  np->TickDiff = (float)( ftmp / factor );
  if ( code[0] >= 'A' )
  {
     pos += BufToDouble ( ftmp, buf + pos );
     np->Yield_VolInStock.VolInStock = (DWORD)ftmp;
     pos += BufToDouble ( ftmp, buf + pos );
     np->Buyp[0] = (float)( ftmp / factor + np->Now );
     pos += BufToDouble ( ftmp, buf + pos );
     np->Sellp[0] = (float)( ftmp / factor + np->Now );
     pos += BufToDouble ( ftmp, buf + pos );
     np->Sellp[1] = (float)ftmp;
     pos += BufToDouble ( ftmp, buf + pos );
     np->Sellp[2] = (float)( ftmp / factor + np->Close );
     for ( i = 0; i<3; i++ )
     {
       pos += BufToDouble ( ftmp, buf + pos );
       np->Buyv[i] = (DWORD)ftmp;
       pos += BufToDouble ( ftmp, buf + pos );
       np->Sellv[i] = (DWORD)ftmp;
     }
  }
  else
  {
     pos += BufToDouble ( ftmp, buf + pos );
     if ( testzs ( code,setcode ) )
        np->Yield_VolInStock.Yield = (float)( ftmp / factor + np->Now );
     else
        np->Yield_VolInStock.Yield = (float)( ftmp / 100.0 );
     for ( i = 0; i<3; i++ )
     {
         if ( testzs ( code,setcode ) )
         {
            pos += BufToDouble ( ftmp, buf + pos );
            np->Buyp[i] = (float)( ftmp * 10000.0 );
            pos += BufToDouble ( ftmp, buf + pos );
            if ( i == 2 )
               np->Sellp[i] = (float)( ftmp / factor );
            else
               np->Sellp[i] = (float)( ftmp * 10000.0 );
         }
         else
         {
            pos += BufToDouble ( ftmp, buf + pos );
            np->Buyp[i] = (float)( ftmp / factor + np->Now );
            pos += BufToDouble ( ftmp, buf + pos );
            np->Sellp[i] = (float)( ftmp / factor + np->Now );
         }
         pos += BufToDouble ( ftmp, buf + pos );
         np->Buyv[i] = (DWORD)ftmp;
         pos += BufToDouble ( ftmp, buf + pos );
         np->Sellv[i] = (DWORD)ftmp;
     }
	 if(b5MMP)
	 {
		 for ( i = 3; i<5; i++ )
		 {
	         if ( testzs ( code,setcode ) )
			 {
				pos += BufToDouble ( ftmp, buf + pos );
				np->Buyp[i] = (float)( ftmp * 10000.0 );
				pos += BufToDouble ( ftmp, buf + pos );
				np->Sellp[i] = (float)( ftmp * 10000.0 );
			 }
			 else
			 {
				 pos += BufToDouble ( ftmp, buf + pos );
				 np->Buyp[i] = (float)( ftmp / factor + np->Now );
				 pos += BufToDouble ( ftmp, buf + pos );
				 np->Sellp[i] = (float)( ftmp / factor + np->Now );
			 }
			 pos += BufToDouble ( ftmp, buf + pos );
			 np->Buyv[i] = (DWORD)ftmp;
			 pos += BufToDouble ( ftmp, buf + pos );
			 np->Sellv[i] = (DWORD)ftmp;
		 }
	 }
	 else
	 {
		 for ( i = 3; i<5; i++ )
		 {
			 np->Buyp[i] = 0.000;
			 np->Sellp[i] = 0.000;
			 np->Buyv[i] = 0;
			 np->Sellv[i] = 0;
		 }
	 }

  }
  memmove ( & ( np->Flag ) , buf + pos, sizeof ( np->Flag ) );
  pos    += sizeof ( np->Flag );
  if(bAllInfo)
  {
	  memmove ( & ( np->CJBS ) , buf + pos, sizeof ( np->CJBS ) );
	  pos    += sizeof ( np->CJBS );
	  memmove ( & ( np->Jjjz ) , buf + pos, sizeof ( np->Jjjz ) );
	  pos    += sizeof ( np->Jjjz );
  }
  len += pos;
}

void CurrStockData_Code_rtn ( char * buf, struct CurrStockData_Code * np, short &len ,BOOL b5MMP,BOOL bAllInfo)
{
  memmove( &(np->SetCode) , buf , 1);
  memmove( np->Code, buf+1, SH_CODE_LEN );
  np->Code[SH_CODE_LEN] = '\0';
  CurrStockData_rtn(buf,&(np->hq),len,b5MMP,bAllInfo);
}

void CurrStockDataG2_Code_rtn ( char * buf, struct CurrStockData_Code * np, short &len)
{
	int  i, pos = 0;
	double ftmp;

	memset(np,0,sizeof(struct CurrStockData_Code));
	memmove( &(np->SetCode), buf + pos, 1 );
	memmove( np->Code, buf + pos + 1, SH_CODE_LEN );
	np->Code[SH_CODE_LEN] = 0;
	pos += 1+SH_CODE_LEN;

	float factor  = 100.0;
	char precise = GetPrecise ( np->Code,np->SetCode );
	if ( precise > 2 )
	{
		for ( i = 0; i < precise - 2; i++ )
			factor *= 10.0;
	}
	//防止黑客处理
	if(!g_AntiHack.bPassorted) 
		return;
	(np->hq).ItemNum = 0;
	memmove ( & ( (np->hq).ItemNum ) , buf + pos, sizeof ( WORD ) );
	pos += sizeof ( WORD );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Now = (float)( ftmp / factor );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Close = (float)( ftmp / factor + (np->hq).Now );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Open = (float)( ftmp / factor + (np->hq).Now );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Max = (float)( ftmp / factor + (np->hq).Now );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Min = (float)( ftmp / factor + (np->hq).Now );
	memmove ( & ( (np->hq).RefreshNum ) , buf + pos, sizeof ( DWORD ) );
	pos += sizeof ( DWORD );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Lead = (float)( ftmp / 1000.0 ); // 领先指标
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Volume = (DWORD)ftmp;
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).NowVol = (DWORD)ftmp;
	memmove ( & ( (np->hq).Amount ) , buf + pos, sizeof ( float ) );
	pos += sizeof ( float );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Inside = (DWORD)ftmp;
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Outside = (DWORD)ftmp;
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).TickDiff = (float)( ftmp / factor );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hq).Yield_VolInStock.Yield = (float)( ftmp / 1000.0 );
	for ( i = 0; i<5; i++ )
	{
		if ( testzs ( np->Code,np->SetCode ) )
		{
			pos += BufToDouble ( ftmp, buf + pos );
			(np->hq).Buyp[i] = (float)( ftmp * 10000.0 );
			pos += BufToDouble ( ftmp, buf + pos );
			if ( i == 2 )
				(np->hq).Sellp[i] = (float)( ftmp / factor );
			else
				(np->hq).Sellp[i] = (float)( ftmp * 10000.0 );
		}
		else
		{
			pos += BufToDouble ( ftmp, buf + pos );
			(np->hq).Buyp[i] = (float)( ftmp / factor + (np->hq).Now );
			pos += BufToDouble ( ftmp, buf + pos );
			(np->hq).Sellp[i] = (float)( ftmp / factor + (np->hq).Now );
		}
		pos += BufToDouble ( ftmp, buf + pos );
		(np->hq).Buyv[i] = (DWORD)ftmp;
		pos += BufToDouble ( ftmp, buf + pos );
		(np->hq).Sellv[i] = (DWORD)ftmp;
	}
	memmove ( & ( (np->hq).Flag ) , buf + pos, sizeof ( (np->hq).Flag ) );
	pos += sizeof ( (np->hq).Flag );
	memmove ( & ( (np->hq).CJBS ) , buf + pos, sizeof ( (np->hq).CJBS ) );
	pos += sizeof ( (np->hq).CJBS );
	memmove ( & ( (np->hq).Jjjz ) , buf + pos, sizeof ( (np->hq).Jjjz ) );
	pos += sizeof ( (np->hq).Jjjz );
	//
	for(i=0;i<5;i++)
	{
		pos += BufToDouble ( ftmp, buf + pos );
		(np->hql2).Buyp[i] = (float)( ftmp / factor + (np->hq).Now );
		pos += BufToDouble ( ftmp, buf + pos );
		(np->hql2).Sellp[i] = (float)( ftmp / factor + (np->hq).Now );
		pos += BufToDouble ( ftmp, buf + pos );
		(np->hql2).Buyv[i] = (DWORD)ftmp;
		pos += BufToDouble ( ftmp, buf + pos );
		(np->hql2).Sellv[i] = (DWORD)ftmp;
	}
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hql2).BuyAvp = (float)( ftmp / factor + (np->hq).Now );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hql2).SellAvp = (float)( ftmp / factor + (np->hq).Now );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hql2).TotalBuyv = (DWORD)ftmp;
	pos += BufToDouble ( ftmp, buf + pos );
	(np->hql2).TotalSellv = (DWORD)ftmp;

	len += pos;
}

void CurrQueueL2_Code_rtn ( char * buf, struct CurrQueue_Code * np, short &len)
{
	int  i, pos = 0;
	double ftmp;

	memset(np,0,sizeof(struct CurrQueue_Code));
	memmove( &(np->SetCode), buf + pos, 1 );
	memmove( np->Code, buf + pos + 1, SH_CODE_LEN );
	np->Code[SH_CODE_LEN] = 0;
	pos += 1+SH_CODE_LEN;

	float factor  = 100.0;
	char precise = GetPrecise ( np->Code,np->SetCode );
	if ( precise > 2 )
	{
		for ( i = 0; i < precise - 2; i++ )
			factor *= 10.0;
	}
	//防止黑客处理
	if(!g_AntiHack.bPassorted) 
		return;
	memmove ( & ( (np->queue).RefreshNum ) , buf + pos, sizeof ( DWORD ) );
	pos += sizeof ( DWORD );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->queue).Buyp = (float)( ftmp / factor );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->queue).Sellp = (float)( ftmp / factor );
	pos += BufToDouble ( ftmp, buf + pos );
	(np->queue).buy1num = (DWORD)ftmp;
	pos += BufToDouble ( ftmp, buf + pos );
	(np->queue).sell1num = (DWORD)ftmp;
	for(i=0;i<min(50,(np->queue).buy1num);i++)
	{
		pos += BufToDouble ( ftmp, buf + pos );
		(np->queue).buy1vol[i] = (DWORD)ftmp;
	}
	for(i=0;i<min(50,(np->queue).sell1num);i++)
	{
		pos += BufToDouble ( ftmp, buf + pos );
		(np->queue).sell1vol[i] = (DWORD)ftmp;
	}

	len += pos;
}

void CurrStockDataEx_rtn ( char * buf, struct CurrStockDataEx * np, short &len ,BOOL b5MMP)
{
  short  i, pos = 0;
  float  factor;
  char   precise;
  double ftmp;

  memmove( &(np->SetCode), buf + pos, 1 );
  memmove( np->Code, buf + pos + 1, SH_CODE_LEN );
  np->Code[SH_CODE_LEN] = '\0';
  factor  = 100.0;
  precise = GetPrecise ( np->Code,np->SetCode );
  if ( precise > 2 )
  {
     for ( i = 0; i < precise - 2; i++ )
         factor *= 10.0;
  }
  short tmplen = len;
  short stmp;
  CurrStockData_rtn(buf,&(np->hq),len,b5MMP);
  pos = len-tmplen;
  memcpy(&np->dkflag,buf+pos,sizeof(char));
  pos+=sizeof(char);
  pos+=BufToDouble(ftmp,buf+pos);
  np->tbp = (float)( ftmp / factor + (np->hq).Now );
  pos+=BufToDouble(ftmp,buf+pos);
  np->stoplost = (float)( ftmp / factor + (np->hq).Now );
  pos+=BufToDouble(ftmp,buf+pos);
  np->leave = (float)( ftmp / factor + (np->hq).Now );
  memcpy(&(stmp),buf+pos,sizeof(short));
  np->zangsu = stmp/100.0;
  pos+=sizeof(short);
  memcpy(&(np->nWarnCount),buf+pos,sizeof(unsigned short));
  pos+=sizeof(unsigned short);
  len = pos+tmplen;
}

void MinuteData_rtn ( char * buf, struct MinuteData * np, char precise, char *code, short setcode,short num, short &len ,BOOL bLead)
{
  float  factor, now = 0.0, fLead = 0.0,average=0.0;
  short  i, pos = 0;
  double ftmp;

  factor = 100.0;
  if ( precise > 2 )
     for ( i = 0; i < precise - 2; i++ ) factor *= 10.0;
  for ( i = 0; i < num; i++ )
  {
      pos += BufToDouble ( ftmp, buf + pos );
      now = ( np + i )->Now = (float)( ftmp / factor + now );
      if ( i == 0 ) average = now;
      pos += BufToDouble ( ftmp, buf + pos );
      average = ( np + i )->Average = (float)( ftmp / factor / 100.0 + average );
      if ( testzs ( code,setcode) )
      {
         pos += BufToDouble ( ftmp, buf + pos );
         ( np + i )->NowVol = (DWORD)( ftmp * 100 ); //好象有问题
      }
      else
      {
         pos += BufToDouble ( ftmp, buf + pos );
         ( np + i )->NowVol = (DWORD)ftmp;
      }
      if(bLead)
	  {
		  pos += BufToDouble ( ftmp, buf + pos );
		  fLead = ( np + i )->VolInStock_Lead.Lead = (float)( ftmp / factor / 100.0 + fLead );
	  }
  }
  len  += pos;
}

void MmldStruc_rtn ( char * buf, struct MinuteData * np, short num, short &len )
{
  short  i, pos = 0;
  double ftmp;

  for ( i = 0; i < num; i++ )
  {
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Buyv = (DWORD)ftmp;
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Sellv = (DWORD)ftmp;
  }
  len  += pos;
}

void VolInStock_rtn ( char * buf, float * np, char * code,short setcode,short num, short &len )
{
  short  i, pos = 0;
  float  lead = 0.0;
  double ftmp;

  for ( i = 0; i < num; i++ )
  {
      if ( testzs ( code,setcode ) )
      {
         pos += BufToDouble ( ftmp, buf + pos );
         lead = np[i] = (float)( ftmp / 10000.0 + lead );
      }
      else
      {
         pos += BufToDouble ( ftmp, buf + pos );
         np[i] = ftmp;
      }
  }
  len  += pos;
}

void simhq_rtn( char *buf,char &setcode,char *code,struct simhq_Info *np,short &len)
{
  short  i, pos = 0;
  float  factor;
  char   precise;
  double ftmp;

  memmove( &setcode, buf + pos, 1 );
  memmove( code, buf + pos + 1, SH_CODE_LEN );
  code[SH_CODE_LEN] = '\0';
  pos    += 1+SH_CODE_LEN;
  factor  = 100.0;
  precise = GetPrecise ( code,setcode );
  if ( precise > 2 )
  {
     for ( i = 0; i < precise - 2; i++ )
         factor *= 10.0;
  }
  pos += BufToDouble ( ftmp, buf + pos );
  np->Now = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Open = (float)( ftmp / factor + np->Now );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Max = (float)( ftmp / factor + np->Now );
  pos += BufToDouble ( ftmp, buf + pos );
  np->Min = (float)( ftmp / factor + np->Now );

  pos += BufToDouble ( ftmp, buf + pos );
  np->Volume = (DWORD)ftmp;
  memmove ( & ( np->Amount ) , buf + pos, sizeof ( float ) );
  pos += sizeof ( float );

  len  += pos;
}

void CdpStruc_rtn( char *buf,struct MemStkInfo *scode,struct remote_cdpInfo *np)
{
  short  pos = 0;
  double ftmp;
  float  factor  = 100.0;
  char   precise = GetPrecise ( scode->Code,scode->setcode );
  if ( precise > 2 )
  {
     for ( int i = 0; i < precise - 2; i++ )
         factor *= 10.0;
  }
  pos += BufToDouble ( ftmp, buf + pos );
  np->cdp = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->nh = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->nl = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->ah = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->al = (float)( ftmp / factor );
  memcpy(&np->dkflag,buf+pos,sizeof(char));
  pos+=sizeof(char);
  pos += BufToDouble ( ftmp, buf + pos );
  np->tbp = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->stoplost = (float)( ftmp / factor );
  pos += BufToDouble ( ftmp, buf + pos );
  np->leave = (float)( ftmp / factor );
}

void TickData_rtn ( char * buf, struct TickData * np, char precise, short num, short &len ,BOOL bVolInStock_dif,BOOL bZS)
{
  short  i, pos = 0;
  long   now = 0L;
  double ftmp;

  for ( i = 0; i < num; i++ )
  {
      memmove ( & ( ( np + i )->Minute ) , buf + pos, sizeof ( ( np + i )->Minute ) );
      pos += sizeof ( ( np + i )->Minute );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Now = (DWORD)( ftmp + now );
      now = ( np + i )->Now;
      if ( precise < 3 ) ( np + i )->Now *= 10;
      pos += BufToDouble ( ftmp, buf + pos );
	  if(bZS)
	  {
		  if((long)ftmp<COMPPREC || ftmp<(100.0*COMPPREC))
			  ( np + i )->NowVol = 0;
		  else
			  ( np + i )->NowVol = (DWORD)(ftmp/100.0);
	  }
	  else
		  ( np + i )->NowVol = (DWORD)ftmp;
	  if(bVolInStock_dif)
	  {
		  pos += BufToDouble ( ftmp, buf + pos );
		  ( np + i )->VolInStock_dif = (DWORD)ftmp;
	  }
	  else
		  ( np + i )->VolInStock_dif = 0;
      memmove ( & ( ( np + i )->InOutFlag ) , buf + pos, sizeof ( ( np + i )->InOutFlag ) );
      pos  += sizeof ( ( np + i )->InOutFlag );
  }
  len  += pos;
}

void TickDataL2_rtn ( char * buf, struct TickData_L2 * np, short num, short &len)
{
  short  i, pos = 0;
  long   now = 0L;
  double ftmp;

  for ( i = 0; i < num; i++ )
  {
      memmove ( & ( ( np + i )->nSecond ) , buf + pos, sizeof ( ( np + i )->nSecond ) );
      pos += sizeof ( ( np + i )->nSecond );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Now = (DWORD)( ftmp + now );
      now = ( np + i )->Now;
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->NowVol = (DWORD)ftmp;
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Rest = (DWORD)ftmp;
  }
  len  += pos;
}

void Bsp1Data_rtn ( char * buf, TICK_MMP * np, struct MemStkInfo *scode, short num, short &len )
{
	short  i, j,pos = 0;
	float factor = 100.0;
	char  precise = GetPrecise ( scode->Code,scode->setcode );
	if( precise > 2 )
		for(i = 0; i < precise - 2; i++) factor *= 10.0;
	double ftmp;
	for ( i = 0; i < num; i++ )
	{
		memmove ( & ( ( np + i )->Minute ) , buf + pos, sizeof ( ( np + i )->Minute ) );
		pos += sizeof ( ( np + i )->Minute );
		pos += BufToDouble ( ftmp, buf + pos );
		( np + i )->Now = (float)( ftmp / factor );
		pos += BufToDouble ( ftmp, buf + pos );
		( np + i )->NowVol = (DWORD)ftmp;
		for(j = 0;j < 5;j++)
		{
			pos += BufToDouble ( ftmp, buf + pos );
			( np + i )->BuyP[j] = ftmp / factor ;
			pos += BufToDouble ( ftmp, buf + pos );
			( np + i )->BuyV[j] = (DWORD)ftmp;
			pos += BufToDouble ( ftmp, buf + pos );
			( np + i )->SellP[j] = ftmp / factor ;
			pos += BufToDouble ( ftmp, buf + pos );
			( np + i )->SellV[j] = (DWORD)ftmp;
		}
		memmove ( & ( ( np + i )->InOutFlag ) , buf + pos, sizeof ( ( np + i )->InOutFlag ) );
		pos  += sizeof ( ( np + i )->InOutFlag );
	}
	len  += pos;
}

void PriceVolTable_rtn ( struct MemStkInfo * scode, char * buf, struct PriceVolTable * np, short num, short &len )
{
  if(!scode) return;
  char  precise = GetPrecise ( scode->Code,scode->setcode );
  short  i, pos = 0;
  long   price = 0L;
  double ftmp;

  for ( i = 0; i < num; i++ )
  {
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Price = (DWORD)( ftmp + price );
      price = ( np + i )->Price;
      if ( precise < 3 ) ( np + i )->Price *= 10;
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Volume = (DWORD)ftmp;
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Kcl = (DWORD)ftmp;
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Pcl = (DWORD)ftmp;
  }
  len  += pos;
}

void AnalyData_rtn3 ( MemStkInfo *scode,
							  char * buf, struct AnalyData * np, short num, short &len )
{
  short  i, pos = 0;
  float  close = 0.0, factor;
  double ftmp;

  factor = 1000.0;
  BOOL bZs = testzs(scode);
  for ( i = 0; i < num; i++ )
  {
      memmove ( & ( ( np + i )->Time ) , buf + pos, sizeof ( ( np + i )->Time ) );
      pos  += sizeof ( np->Time );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Open = (float)( ftmp / factor + close );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Close = (float)( ftmp / factor + ( np + i )->Open );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->High = (float)( ftmp / factor + ( np + i )->Open );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Low = (float)( ftmp / factor + ( np + i )->Open );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Volume = (DWORD)ftmp;
	  memcpy(&(( np + i )->Amount),buf+pos,sizeof(long));
	  pos+=sizeof(long);
	  if(bZs)
	  {
		memcpy(&(( (np + i )->zd).up),buf+pos,sizeof(WORD));
	    pos+=sizeof(WORD);
		memcpy(&(( (np + i )->zd).down),buf+pos,sizeof(WORD));
	    pos+=sizeof(WORD);
	  }
	  else
		( np + i )->YClose = 0.0;

      close = ( np + i )->Close;
  }
  len  += pos;
}

void AnalyData_rtn4 ( MemStkInfo *scode,
							  char * buf, struct AnalyData * np, short num, short &len )
{
  short  i, pos = 0;
  float  close = 0.0, factor;
  double ftmp;

  factor = 1000.0;
  BOOL bZs = testzs(scode);
  for ( i = 0; i < num; i++ )
  {
      memmove ( & ( ( np + i )->Time ) , buf + pos, sizeof ( ( np + i )->Time ) );
      pos  += sizeof ( np->Time );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Open = (float)( ftmp / factor + close );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Close = (float)( ftmp / factor + ( np + i )->Open );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->High = (float)( ftmp / factor + ( np + i )->Open );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Low = (float)( ftmp / factor + ( np + i )->Open );
      pos += BufToDouble ( ftmp, buf + pos );
      ( np + i )->Volume = (DWORD)ftmp;
	  memcpy(&(( np + i )->Amount),buf+pos,sizeof(long));
	  pos+=sizeof(long);
	  if(bZs)
	  {
		memcpy(&(( (np + i )->zd).up),buf+pos,sizeof(WORD));
	    pos+=sizeof(WORD);
		memcpy(&(( (np + i )->zd).down),buf+pos,sizeof(WORD));
	    pos+=sizeof(WORD);
	  }
	  else
	  {
		memcpy(&( (np + i )->YClose),buf+pos,sizeof(float));
		pos+=sizeof(float);
	  }

      close = ( np + i )->Close;
  }
  len  += pos;
}

void MemStkInfo_rtn3 ( struct remote_TmpStkInfo3 * rp, struct MemStkInfo * np )
{
  memset ( np, 0, sizeof ( struct MemStkInfo ) );
  memmove ( np->Code, rp->Code, SH_CODE_LEN );
  np->Code[SH_CODE_LEN] = 0;
  memmove ( np->Name, rp->Name, NAME_LEN );
  np->Name[NAME_LEN]    = 0;
  np->Unit              = rp->Unit;
  np->tp_flag           = rp->precise;
  np->VolBase           = rp->VolBase;
  np->Close			    = rp->Close;
  //第三版加入Base和Gbbq的更新次数
  np->BaseFreshCount = rp->BaseFreshCount;
  np->GbbqFreshCount = rp->GbbqFreshCount;
  //键盘精灵拼音
  if(np->SpellCode[0]=='\0')
	  strncpy(np->SpellCode,GetPYStr(np->Name,5),SPELLCODE_SIZE-1);
}

void  ZmzmStruc_rtn( char * buf, LPNORMALSTAT np, short num, short &len )
{
	short  i, pos = 0;
	double ftmp;

	for ( i = 0; i < num; i++ )
	{
		memcpy(&((np+i)->BuyAvp),buf+pos,sizeof(float));
		pos+=sizeof(float);
		pos += BufToDouble ( ftmp, buf + pos );
		( np + i )->TotalBuyv = (DWORD)(ftmp+0.5);
		memcpy(&((np+i)->SellAvp),buf+pos,sizeof(float));
		pos+=sizeof(float);
		pos += BufToDouble ( ftmp, buf + pos );
		( np + i )->TotalSellv = (DWORD)(ftmp+0.5);
	}
	len  += pos;
}

void  JgjcStruc_rtn( char * buf, LPNORMALSTAT np, short num, short &len )
{
  short  i, pos = 0;

  for ( i = 0; i < num; i++ )
  {
	 memcpy(&((np+i)->fBOrder),buf+pos,sizeof(float));
     pos+=sizeof(float);
	 memcpy(&((np+i)->fBCancel),buf+pos,sizeof(float));
     pos+=sizeof(float);
	 memcpy(&((np+i)->fSOrder),buf+pos,sizeof(float));
     pos+=sizeof(float);
	 memcpy(&((np+i)->fSCancel),buf+pos,sizeof(float));
     pos+=sizeof(float);
  }
  len  += pos;
}

void   CjbsStruc_rtn( char * buf, LPTICKSTAT np, short num, short &len )
{
	short i,j,k, pos = 0;
	double ftmp;

	for(i = 0;i < num;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<5;k++)
			{
				pos+=BufToDouble(ftmp,buf+pos);
				(np+i)->VolNum[j][k] = DWORD(ftmp+0.5);
			}
		}
	}
	len += pos;
}

void   CjtjStruc_rtn( char * buf, LPTICKSTAT np, short num, short &len )
{
	short i,j,k, pos = 0;

    for(i = 0;i < num;i++)
    {
		for(j=0;j<3;j++)
		{
			for(k=0;k<5;k++)
			{
				memcpy(&((np+i)->fVol[j][k]),buf+pos,sizeof(float));
				pos+=sizeof(float);
			}
		}
    }
	len += pos;
}

void   TickStatStruc_rtn(char *buf,LPTICKSTAT np,short num,short &len)
{
	short i,j,k, pos = 0;
	double ftmp;

    for(i = 0;i < num;i++)
    {
		for(j=0;j<3;j++)
		{
			for(k=0;k<5;k++)
			{
				memcpy(&((np+i)->fVol[j][k]),buf+pos,sizeof(float));
				pos+=sizeof(float);
				pos+=BufToDouble(ftmp,buf+pos);
				(np+i)->VolNum[j][k] = DWORD(ftmp+0.5);
			}
		}
    }
	len += pos;
}
