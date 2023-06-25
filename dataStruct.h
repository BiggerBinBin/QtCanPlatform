#pragma once
#include <QString>
#include <iostream>
#include<vector>
#include <QColor>
#include <QTimer>
struct AutoTestStruct
{
	int m_iEnableInLine;		//0 ʹ�ܰ�ť������
	int m_iEnOp;				//1 ʹ��ʱ������ѡ�ĸ����ڻ�ͨ��2��������1����
	int m_iPowerInLine;			//2 ���������������
	int m_iLowVoltage;			//3 ��ѹǷѹ��ֵ
	int m_iLowVoltageRe;		//4 Ƿѹ�ָ�
	int m_iOverVoltage;			//5 ��ѹ��ѹ��ֵ
	int m_iOverVoltageRe;		//6 ��ѹ�ָ�
	int m_iOverTemperature;		//7 ���±���
	int m_iOverTempRe;			//8 ���»ָ�
	int m_iOverTempTolerance;	//9 ���»ָ�/�������
	int m_iRatedVolt;			//10 ���ѹ
	float m_fRequirePW;			//11 ����ʱ����Ĺ��ʣ��е���ռ�ձȣ��е��ǹ��ʣ�������Ҫ�������
	float m_fRatedPW;			//12 �����
	QString m_strTolerance;		//13 �������
	QString m_strVer;			//14 ����汾��
	QString m_strVerRecID;		//15 �汾�Ž�����ID
	ushort m_usIDBytelen;		//16 �汾�ɼ����ֶ����
	ushort m_usIDByteStartbit;	//17 �汾��������ֹλ
	int m_usRatedPWTemp;		//18 ����ʵ��¶�
	int m_usRatedPWFlow;		//19 ����ʵ�����
	int m_usHeatTemp;			//20 ������ʼ�¶�
	int m_usCoolTemp;			//21 �����¶�
	int m_bTurnOffFlow;			//22 ���Թ��±���ʱ�Ƿ�ر�����
	int m_bTurnOffCool;			//22 ���Թ��±���ʱ�Ƿ�ر�����
	int m_iOutTempStop;			//23 ��ˮ�ڹ����¶ȣ������ֹͣʹ��
	ushort m_usOutOrInTemp;		//24 ʹ�ó�ˮ���¶Ȼ�����ˮ���¶ȣ�0����1�룩
	//bool isIndepandSend;

};
struct RollStruct
{
	float value;
	QString name;
};
struct YBRGB
{
	uchar r;
	uchar g;
	uchar b;
};
struct parseData
{
	QString name;
	float value;
	QString toWord;
	struct YBRGB color;
};
struct showTableData
{
	QString IdName;
	std::vector<parseData> Pdata;
};
//ֵ�Ժ���
struct cellProperty
{
	uint16_t r;									//��ɫ����
	uint16_t g;									//��ɫ����
	uint16_t b;									//��ɫ����
	QString toWord;								//ֵ�ĺ���
	QString value;								//ֵ
	bool isStand;								//�Ƿ�����ֵ�������ֵΪ�����쳣��ֵ��
	//QColor  color;
	
};
//�ֶ�����
struct protoItem {
	bool octhex;								//16���Ʒ�
	bool isRoll;								//�Ƿ��������ʾ������һ�����
	//int send;									//���͵�����
	float send;									//���͵�����
	int startByte;								//��ֹ�ֽ�
	int startBit;								//��ֹλ
	int bitLeng;								//����
	float precision;							//����
	int offset;									//ƫ��
	QString bitName;							//�ֶ�����
	QString dataFrom;							//��Դ���籨��ֻ�е�����ѹ��Ҫ��ʾ���ʣ����õ�����ѹ���
	std::vector<cellProperty>stl_itemProperty;	//��ʾ������
	//std::map<QString, cellProperty>itemProperty;
};
//CAN-ID���ݽṹ��
struct canIdData {
	bool isSend;							//�Ƿ��������ID
	uint8_t opt;							//���ձ��Ļ��Ƿ��ͱ���
	uint8_t len;							//���ݳ��ȣ�LINͨ����
	int circle;						//��Ϊ����IDʱ����ID�ķ�������
	QTimer* timeAdd;							//��ʱ����ַ
	QString strCanId;						//CAN-ID
	std::vector<struct protoItem>pItem;		//����
};
//�ͺ����ݽṹ��
struct protoData {
	
	uint8_t agreement;				//Э��,0:intel,1:motolora
	int bundRate;					//������
	int circle;						//��������
	QString modelName;				//����
	std::vector<canIdData>cItem;	//����
	bool bStandardId;				//�Ƿ�Ϊ��׼֡
	QString sPlatform;				//ƽ̨:3kW,5kW,7kW,10kW,15kW
	struct AutoTestStruct ats;		//�Զ����Բ���
};
struct autoTestData {
	int m_iLowVolt;				//Ƿѹ�𲽵�ѹ
	int m_iHeightVolt;			//��ѹ�𲽵�ѹ
	int m_iVoltStep;			//��ѹ����
	int m_iVoltError;			//��ѹ���
	int m_iHeatTempture;		//��ʼ���ȵ��¶�
	int m_iPowerTempture;		//����ʵ��¶�
	int m_iOverTime;			//��ʱ
	QString m_sInWebAddr;		//�ӿڵ�ַ
	QString m_sOutWebAddr;		//�ӿڵ�ַ
	int m_iShowType;			//���Ͻ���ʾ�ķ�ʽ��0����ʾ���Ӳ�����1����ʾ�Զ������Բ���
	int m_iRecOnNoSend;			//�Ƿ���豸�ͽ������ݣ�
	int m_iSaveListNum;			//���������ݱ���һ�Σ�
	bool m_bShowAutoTest;
};
struct messageData
{
	uint Id;
	double TimeStemp;
	uchar Data[8];
};
struct messageMange
{
	std::vector<messageData>vData;
	uint iLen;
	uint iIndex;
};
