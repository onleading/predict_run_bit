#include "StdAfx.h"
#include "PoloTrade.h"
#include "algo_hmac.h"
#include "..\json/json.h"
#include <fstream>
#include "OneCalc.h"

char *publicUrl = "https://poloniex.com/public?";
char *tradeUrl = "https://poloniex.com/tradingApi";
char *Api_Key = "HJODWCGZ-BDVVUJ7Z-LZJC04PR-J5JE2J5W";
char *Secret = "56ce8d00427a1607d9e49c601e584a1bf59c577c2ec4a7c4342ebc5978ee09cfc3f377966d2401a91b902578e83579e9c91093a8c10081216f44341838016f9b";
int g_nonce = 0;

void	HMacSign(char *key, char *data, char *sign_mac)
{
	unsigned char * mac = NULL;
	unsigned int mac_length = 0;
	int ret = HmacEncode("sha512", Secret, strlen(Secret), data, strlen(data), mac, mac_length);
	for(int i = 0; i < mac_length; i++)
		sprintf(&sign_mac[i*2],"%02x", (unsigned int)mac[i]);
}

int		GetNonce()
{
	int newnonce = time(NULL);
	if(newnonce<=g_nonce)
		g_nonce++;
	else 
		g_nonce = newnonce;
	return g_nonce;
}

void	CancelAllOrders()
{
//	return;

	char content[1024]={0}, header_ex[1024]={0}, sign_mac[256]={0};
	
	g_PoloReqSec.Lock();
	
	sprintf(content, "command=returnOpenOrders&currencyPair=all&nonce=%d", GetNonce());
	HMacSign(Secret, content, sign_mac);
	sprintf(header_ex, "Sign: %s\r\nKey: %s", sign_mac, Api_Key);
	WebPostDownload(tradeUrl, content, "", "Orders.txt", header_ex);
	
	//read
	Json::Reader reader; 
	Json::Value root;
	
	ifstream is;  
	is.open ("Orders.txt");    
	if (reader.parse(is, root, FALSE)) 
	{  
		Json::Value::Members mm = root.getMemberNames();
		for(int k=0;k<mm.size();k++)
		{
			char *currencyPair = (char*)mm[k].c_str();			
			int nOrderCnt = root[currencyPair].size();
			if(nOrderCnt>0)
			{
				for(int j=0;j<nOrderCnt;j++)
				{
					const char *OrderNumber = root[currencyPair][j]["orderNumber"].asCString();
					
					sprintf(content, "command=cancelOrder&orderNumber=%s&nonce=%d",OrderNumber,GetNonce());
					HMacSign(Secret, content, sign_mac);
					sprintf(header_ex, "Sign: %s\r\nKey: %s", sign_mac, Api_Key);
					WebPostDownload(tradeUrl, content, "", "CancelOrder.txt", header_ex);
				}
			}
		}
	}
	is.close();
	
	g_PoloReqSec.Unlock();
}

void	GetBalances(vector<BalanceUnit> &vBalance, double &dwAvailMoney, BOOL &bHasAllOrders)
{
	vBalance.clear();
	dwAvailMoney = 0.0;

	g_PoloReqSec.Lock();
	char content[1024]={0}, header_ex[1024]={0}, sign_mac[256]={0};

	//Balance
	sprintf(content, "command=returnAvailableAccountBalances&nonce=%d", GetNonce());
	HMacSign(Secret, content, sign_mac);
	sprintf(header_ex, "Sign: %s\r\nKey: %s", sign_mac, Api_Key);
	WebPostDownload(tradeUrl, content, "", "Balances.txt", header_ex);
	
	Json::Reader reader2; 
	Json::Value root2;
	ifstream is2; 
    is2.open ("Balances.txt");
    if (reader2.parse(is2, root2, FALSE)) 
    {   
		Json::Value::Members mm = root2["exchange"].getMemberNames();
        for(int k=0;k<mm.size();k++)
		{
			char *currency = (char*)mm[k].c_str();	
			const char *Vol = root2["exchange"][currency].asCString();
			double dwVol = atof(Vol);
			if(strcmp(currency, "USDT")==0)
				dwAvailMoney += dwVol;
			else
			{
				BalanceUnit bu={0};
				strncpy(bu.currency, currency, 19);
				bu.dwInVol += dwVol;
				vBalance.push_back(bu);
			}
		}
	}
	is2.close();

	//orders
	sprintf(content, "command=returnOpenOrders&currencyPair=all&nonce=%d", GetNonce());
	HMacSign(Secret, content, sign_mac);
	sprintf(header_ex, "Sign: %s\r\nKey: %s", sign_mac, Api_Key);
	WebPostDownload(tradeUrl, content, "", "Orders.txt", header_ex);
	
	//read
	Json::Reader reader1; 
	Json::Value root1;	
	ifstream is1;
	is1.open ("Orders.txt");    
	if (reader1.parse(is1, root1, FALSE)) 
	{  
		Json::Value::Members mm = root1.getMemberNames();
		for(int k=0;k<mm.size();k++)
		{
			char *currencyPair = (char*)mm[k].c_str();			
			char *currency = NULL;
			int nBalXh = -1;
			if(strncmp(currencyPair, "USDT_", 5)==0)
			{
				currency = &currencyPair[5];
				for(int s=0;s<vBalance.size();s++)
				{
					if(strcmp(vBalance[s].currency, currency)==0)
					{
						nBalXh = s;
						break;
					}
				}
			}

			if(nBalXh>=0)
			{
				int nOrderCnt = root1[currencyPair].size();
				if(nOrderCnt<2*EGG_COUNT) bHasAllOrders = FALSE;
				for(int j=0;j<nOrderCnt;j++)
				{
					const char *OrderType = root1[currencyPair][j]["type"].asCString();
					if(strcmp(OrderType,"buy")==0)
					{
						const char *OrderTol = root1[currencyPair][j]["total"].asCString();
						dwAvailMoney += atof(OrderTol);
					}
					else if(strcmp(OrderType,"sell")==0)
					{
						const char *OrderAmo = root1[currencyPair][j]["amount"].asCString();
						vBalance[nBalXh].dwInVol += atof(OrderAmo);
					}
				}
			}
		}
	}
	is1.close();

	g_PoloReqSec.Unlock();
}

void	GetNowPrice(char *currency, BOOL bBuy, char *ActionPrice)
{	
	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);

	g_PoloReqSec.Lock();
	char ReqUrl[1024]={0};
	sprintf(ReqUrl, "%scommand=returnOrderBook&currencyPair=%s&depth=1", publicUrl,currencyPair);
	WebContentDownload(ReqUrl, "OrderBook.txt");
	
	//read
	Json::Reader reader; 
    Json::Value root;
	
    ifstream is;  
    is.open ("OrderBook.txt"); 
    if (reader.parse(is, root, FALSE)) 
	{
		Json::Value BSNode;
		if(bBuy)
			BSNode = root["asks"];
		else 
			BSNode = root["bids"];
		Json::Value::iterator iter = BSNode.begin();
		for(;iter!=BSNode.end();iter++)
		{
			Json::Value NowOrder = (*iter);
			Json::Value::iterator iter1 = NowOrder.begin();
			Json::Value OrderPrice = (*iter1);
			const char *Price = OrderPrice.asCString();
			if(strlen(Price)>0)
				strncpy(ActionPrice, Price, 19);
			break;
		}
	}
	is.close();
	g_PoloReqSec.Unlock();
}

void	GetOrderPrice(char *currency, int nOrderXh, char *OrderBuy, char *OrderSell)
{	
	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);
	
	g_PoloReqSec.Lock();
	char ReqUrl[1024]={0};
	sprintf(ReqUrl, "%scommand=returnOrderBook&currencyPair=%s&depth=%d", publicUrl,currencyPair,nOrderXh);
	WebContentDownload(ReqUrl, "OrderBook.txt");
	
	//read
	Json::Reader reader; 
    Json::Value root;
	
    ifstream is;  
    is.open ("OrderBook.txt"); 
    if (reader.parse(is, root, FALSE)) 
	{
		Json::Value Node_Buy, Node_Sell;
		Node_Buy = root["asks"];
		Node_Sell = root["bids"];

		Json::Value::iterator iterBuy = Node_Buy.end();
		Json::Value::iterator iterSell = Node_Sell.end();
		iterBuy--;
		iterSell--;

		Json::Value OrderPos = (*iterBuy);
		Json::Value::iterator iterPrice = OrderPos.begin();
		Json::Value OrderBuyPrice = (*iterPrice);
		const char *BuyPrice = OrderBuyPrice.asCString();
		if(strlen(BuyPrice)>0)
			strncpy(OrderBuy, BuyPrice, 19);

		OrderPos = (*iterSell);
		iterPrice = OrderPos.begin();
		Json::Value OrderSellPrice = (*iterPrice);
		const char *SellPrice = OrderSellPrice.asCString();
		if(strlen(SellPrice)>0)
			strncpy(OrderSell, SellPrice, 19);
	}
	is.close();
	g_PoloReqSec.Unlock();
}

void	GetMatchPrice(char *currency, BOOL bBuy, double amount, char *ActionPrice)
{	
	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);
	
	g_PoloReqSec.Lock();
	char ReqUrl[1024]={0};
	sprintf(ReqUrl, "%scommand=returnOrderBook&currencyPair=%s&depth=10", publicUrl,currencyPair);
	WebContentDownload(ReqUrl, "OrderBook.txt");
	
	//read
	Json::Reader reader; 
    Json::Value root;
	
    ifstream is;  
    is.open ("OrderBook.txt"); 
    if (reader.parse(is, root, FALSE)) 
	{
		Json::Value BSNode;
		if(bBuy)
			BSNode = root["asks"];
		else 
			BSNode = root["bids"];
		Json::Value::iterator iter = BSNode.begin();
		
		double dwAmountAcum=0.0;
		for(;iter!=BSNode.end();iter++)
		{
			Json::Value NowOrder = (*iter);
			Json::Value::iterator iter1 = NowOrder.begin();
			Json::Value OrderPrice = (*iter1);
			const char *Price = OrderPrice.asCString();
			if(strlen(Price)>0)
				strncpy(ActionPrice, Price, 19);

			iter1++;
			Json::Value OrderAmount = (*iter1);
			dwAmountAcum += OrderAmount.asDouble();
			if(dwAmountAcum>amount)
				break;
		}
	}
	is.close();
	g_PoloReqSec.Unlock();
}

void	TradeAction(char *currency, double amount, BOOL bBuy)
{
//	return;

	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);
	
	char ActionPrice[20]={0};
	GetMatchPrice(currency, bBuy, amount, ActionPrice);

	if(strlen(ActionPrice)>0)
	{
		char content[1024]={0}, header_ex[1024]={0}, sign_mac[256]={0};

		g_PoloReqSec.Lock();
		sprintf(content, "command=%s&currencyPair=%s&rate=%s&amount=%9.8f&%s&nonce=%d",
			bBuy?"buy":"sell",currencyPair,ActionPrice,amount,"immediateOrCancel=1",GetNonce());
		HMacSign(Secret, content, sign_mac);
		sprintf(header_ex, "Sign: %s\r\nKey: %s", sign_mac, Api_Key);
		WebPostDownload(tradeUrl, content, "", "TakeAction.txt", header_ex);
		g_PoloReqSec.Unlock();
	}
}

void	EggAction(char *currency, double amount, double price, BOOL bBuy)
{
//	return;

	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);

	char ActionPrice[20]={0};
	sprintf(ActionPrice, "%9.8f", price);

	char content[1024]={0}, header_ex[1024]={0}, sign_mac[256]={0};
	
	g_PoloReqSec.Lock();
	sprintf(content, "command=%s&currencyPair=%s&rate=%s&amount=%9.8f&%s&nonce=%d",
		bBuy?"buy":"sell",currencyPair,ActionPrice,amount,"postOnly=1",GetNonce());
	HMacSign(Secret, content, sign_mac);
	sprintf(header_ex, "Sign: %s\r\nKey: %s", sign_mac, Api_Key);
	WebPostDownload(tradeUrl, content, "", "TakeAction.txt", header_ex);
	g_PoloReqSec.Unlock();
}

void	GetAllMarketValue(vector<BalanceUnit> &MyBalances, double &AllValue, double &dwAvailMoney, BOOL &bHasAllOrders)
{
	AllValue = 0.0;
	GetBalances(MyBalances, dwAvailMoney, bHasAllOrders);
	AllValue += dwAvailMoney;
	for(int i=0;i<MyBalances.size();i++)
	{
		char Price[20]={0};
		GetNowPrice(MyBalances[i].currency, FALSE, Price);
		if(strlen(Price)>0)
		{
			MyBalances[i].dwValue += atof(Price)*MyBalances[i].dwInVol;
			AllValue += MyBalances[i].dwValue;
		}
	}
}

double GetLastTradePrice(char *currency)
{
	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);
	char content[1024]={0}, header_ex[1024]={0}, sign_mac[256]={0};
	
	g_PoloReqSec.Lock();
	sprintf(content, "command=returnTradeHistory&currencyPair=%s&limit=6&nonce=%d", currencyPair, GetNonce());
	HMacSign(Secret, content, sign_mac);
	sprintf(header_ex, "Sign: %s\r\nKey: %s", sign_mac, Api_Key);
	WebPostDownload(tradeUrl, content, "", "Trades.txt", header_ex);
	//parse
	//read
	Json::Reader reader; 
    Json::Value root;
	char *strTPrice = NULL;
    ifstream is;  
    is.open ("Trades.txt");
    if (reader.parse(is, root, FALSE)) 
	{
		for (Json::Value::ArrayIndex i = 0; i != root.size(); i++)
		{
			if(root[i].isMember("rate")&&strTPrice==NULL)
				strTPrice = (char*)root[i]["rate"].asCString();
		}
	}
	is.close();
	g_PoloReqSec.Unlock();

	if(strTPrice)
		return atof(strTPrice);
	return 0.0;
}

double GetLastMarketPrice(char *currency)
{
	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);

	WebContentDownload("https://poloniex.com/public?command=returnTicker", "MarketTicker.txt");
	Json::Reader reader; 
    Json::Value root;
	char *strTPrice = NULL;
    ifstream is;  
    is.open ("MarketTicker.txt");
    if (reader.parse(is, root, FALSE)) 
	{
		if(root.isMember(currencyPair)&&root[currencyPair].isMember("last"))
		{
			strTPrice = (char*)root[currencyPair]["last"].asCString();
			return atof(strTPrice);
		}
	}
	return 0.0;
}