struct BalanceUnit
{
	char	currency[20];
	double	dwInVol;
	double	dwValue;
};
extern int g_nonce;

void	HMacSign(char *key, char *data, char *sign_mac);
int		GetNonce();

void	CancelAllOrders();
void	GetBalances(vector<BalanceUnit> &vBalance, double &dwAvailMoney, BOOL &bHasAllOrders);
void	GetNowPrice(char *currency, BOOL bBuy, char *ActionPrice);
void	GetOrderPrice(char *currency, int nOrderXh, char *OrderBuy, char *OrderSell);
void	GetMatchPrice(char *currency, BOOL bBuy, double amount, char *ActionPrice);
void	TradeAction(char *currency, double amount, BOOL bBuy);
void	EggAction(char *currency, double amount, double price, BOOL bBuy);
void	GetAllMarketValue(vector<BalanceUnit> &MyBalances, double &AllValue, double &dwAvailMoney, BOOL &bHasAllOrders);
double	GetLastTradePrice(char *currency);
double	GetLastMarketPrice(char *currency);