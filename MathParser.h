#ifndef __MATHPARSER_H__
#define __MATHPARSER_H__
//#include "psms.h"

#define  ParserStackSize  100
#define  MaxFuncNameLen  5
#define  ExpLimit  11356
#define  SQRLIMIT  1E246
#define  MaxExpLen  4
#define  TotalErrors  7
#define  ErrParserStack  1
#define  ErrBadRange  2
#define  ErrExpression  3
#define  ErrOperator  4
#define  ErrOpenParen  5
#define  ErrOpCloseParen  6
#define  ErrInvalidNum  7

//type  ErrorRange = 0..TotalErrors;

typedef enum 
{
	PLUS, MINUS, TIMES, DIVIDE, EXPO, OPAREN, CPAREN, NUM,
	FUNC, EOL, BAD, ERR, MODU
}TokenTypes;

typedef double Extended;

typedef struct
{
	  BYTE State;
	  Extended Value;
	  CString FuncName;//[MaxFuncNameLen];
}TokenRec;

typedef struct{
	Extended Value;	
	char VarName[30];
}VARSTRU;

class  CMathParser
{
 public:
	CMathParser(CString s);
	~CMathParser();
	void AddVar(Extended Value, CString  VarName);

  
  protected:
	  CString  FInput;
	  TokenRec  CurrToken;
      BOOL MathError;
      TokenRec Stack[ParserStackSize];
      int StackTop;// : 0..ParserStackSize;
      int TokenError;// : ErrorRange;
      WORD TokenLen;// : Word;
      TokenTypes TokenType;
	  CPtrList m_VarList;

      WORD GotoState(WORD Production);
      BOOL IsFunc(CString S);
      BOOL IsVar(Extended &Value);

	  TokenTypes NextToken();
      void Push(TokenRec Token);
      void Pop(TokenRec &Token);
      void Reduce(WORD Reduction);
      void Shift(WORD State);
	  int Round(double value);
	
	  BOOL GetVar(CString VarName, Extended &Value);

  public://    { Public declarations }
      WORD Position;
      BOOL ParseError;
      Extended ParseValue;
      void  Parse();
  
};
#endif
