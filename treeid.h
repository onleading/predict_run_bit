#ifndef TREEID_H_
#define TREEID_H_

#define CMD_MAIN	0		//������
#define CMD_HQ		1		//���鴰��
#define CMD_COM		2		//��������
#define CMD_JBM		3		//�������ϴ���
#define CMD_NEWS	4		//������Ϣ����
#define CMD_HIS		5		//��ʷ��������
#define CMD_ADVHQ	6		//�߼����鴰��
#define CMD_JBFX	7		//������������
#define CMD_JJ		8		//�����������

#define CMD_DIRECT	50		//ֱ�ӵ�Command
#define CMD_TCDLG	51		//���׶Ի���

struct TreeID_Map
{
	char	IDStr[20];
	DWORD	CommandType;
	DWORD	CommandID;
};

extern struct TreeID_Map g_pTreeIDMap[];

extern void TreeID_CarryOut(const char *pIDStr);
extern BOOL BeforeNavigate_Process(char *url);

#endif