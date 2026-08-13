#pragma once

#include <QObject>
#include <QTableWidget>
#include <QMap>
#include <QMutex>

#include <QGroupBox>
#include <QGridLayout>
#include "../ModelDataManage/Unities/dataStruct.h"
#include <QTimer>

namespace _request_mannage_
{
	typedef struct s_d8
	{
		short int r_len;
		uchar d[8];
		QTimer* t;
		s_d8()
		{
			t = nullptr;
			r_len = 8;
			memset(d, 0, sizeof(uchar) * 8);
		}
	}Data8;
	typedef struct
	{
		QWidget* qwg;
		int value;
	}Property;
	typedef struct
	{
		QMap<uint, Property>wProperty;
		uint32_t id;
		uchar start_bit;
		uchar bit_len;
		uchar type;						
		float factor;					//因子
		int offset;						//偏移
		uint32_t curValue;				//当前值
		QString dataFrom;				//数据类型
	}WgMap;
	typedef struct
	{
		QGroupBox* qgp;
		WgMap wg;
	}WgGroup;
}
class RequestTableManage  : public QObject
{
	Q_OBJECT

public:
	RequestTableManage(QObject *parent);
	~RequestTableManage();
	std::shared_ptr<QMap<uint32_t, _request_mannage_::Data8>> getReqArray();
	std::shared_ptr<QMap<uint32_t, _request_mannage_::Data8>> getReqArray_Pure() {
		return m_reqMsgArray;
	}
	void doSelfOperator(uint32_t id);
	/*{
		return m_reqMsgArray;
	}*/
	QWidget* getTableWidget()
	{
		return widget;
			
	}
private:

	QWidget* widget = nullptr;										//显示到主界面上的窗口
	QVector<_request_mannage_::WgGroup> m_operator;					//操作组件
	int m_currModelIndex;											//当前型号下标
	QVector<canIdData>m_vecData;									//当前型号数据
	QHBoxLayout* vlayout = nullptr;									//总体布局
	QVector<QGroupBox*>m_vecGroup;									//好像没什么用
	std::shared_ptr<QMap<uint32_t, _request_mannage_::Data8>> m_reqMsgArray;			//用户请求的数组
	QMap<uint32_t, _request_mannage_::Data8> m_disabledData;		//备份容器，将数据备份
	enum PROTOCOL
	{
		INTEL = 0,
		MOTOROLA_LSB = 1,
		MOTOROLA_MSB = 2
	};
	PROTOCOL protocol;
	ushort m_usRoll=0;
	std::map<uint32_t, quint8> v_usRoll;
	QMutex m_mutex;
	QMutex m_dataMutex;
private:
	void InitWidget();
	void DrawWidget();
	void GetModelData(const int index);
	bool setArrayValue(const uint id, const int start_bit, const int lenght, const int value, PROTOCOL protocol);
	bool intelProtocol(canIdData& cdata, uchar data[], unsigned int& fream_id);
	bool motoProtocol(canIdData& cdata, uchar data[], unsigned int& fream_id);
	void doSelfInput(const _request_mannage_::WgMap& itemp, uchar* array);
	unsigned char crc_high_first(const uchar data[], unsigned char len);	//BOSCH专用
	uchar SAEJ1850_CRC8(uchar* data, uchar length);							//Duola专用
	unsigned char crc_0X2F(const uchar data[], unsigned char len);			//创安
	unsigned char crc_KaiWo(const uchar data[], unsigned char len);
	unsigned char checksumXOR(const uchar data[]);
	//辅助函数
	int getArrayValue(uint32_t id, int start_bit,int bit_len, int protocol);
	void setArrayValueForData(_request_mannage_::Data8& data,int start_bit, int bit_len,int value, int protocol);
	void restoreUiState(uint32_t id);
	void saveUiState(uint32_t id);
	void calculateCRCWithTempZero(uchar* data, int crcBytePos, const QString& crcType, uchar& crcValue);
public Q_SLOTS:
	void on_currentModelIndex_Changed(int index);
private Q_SLOTS:
	void on_idGroupboxStatus_Changed(bool b);						//ID组框盒子变化响应槽
	void on_sigRadioButtonStatus_Changed(bool b);					//单选按钮变化槽函数
	void on_sigLineedit_textChanged();			//输入框变化槽函数

	
};
