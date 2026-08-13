#include "RequestTableManage.h"
#include "../ModelDataManage/Unities/qGboleData.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QFile>
#include <qelapsedtimer.h>
#include "../DeviceIO/3rd/QsLog/include/QsLog.h"
RequestTableManage::RequestTableManage(QObject *parent)
	: QObject(parent), widget(new QWidget)
{
	vlayout = new QHBoxLayout();
	m_reqMsgArray = std::make_shared<QMap<uint32_t, _request_mannage_::Data8>>();
	InitWidget();
	v_usRoll.clear();
}

RequestTableManage::~RequestTableManage()
{}

//std::shared_ptr<QMap<uint32_t, _request_mannage_::Data8>> RequestTableManage::getReqArray()
//{
//	QMap<uint32_t, _request_mannage_::WgMap>_crc_;
//	for (auto v : m_operator)
//	{
//		if (v.wg.dataFrom == "CRC" || v.wg.dataFrom == "CRC8"|| v.wg.dataFrom == "CRC2F")
//		{
//			_crc_.insert(v.wg.id, v.wg);
//		}
//		else if (v.wg.dataFrom.contains("++"))
//		{
//			for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
//			{
//				if (m.key() == v.wg.id)
//				{
//					this->doSelfInput(v.wg, m->d);
//					break;
//				}
//			}
//			
//		}
//	}
//	for (auto vec : _crc_)
//	{
//		for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
//		{
//			if (m.key() == vec.id)
//			{
//				/*if (vec.dataFrom == "CRC")
//				{
//					m->d[vec.start_bit/8]=this->crc_high_first(m->d, 7);
//				}
//				else if (vec.dataFrom == "CRC8")
//				{
//					m->d[vec.start_bit / 8] = this->SAEJ1850_CRC8(m->d, 7);
//				}
//				else if (vec.dataFrom == "CRC2F")
//				{
//					m->d[vec.start_bit / 8] = this->crc_0X2F(m->d, 7);
//				}*/
//				int crcBytePos = vec.start_bit / 8;
//				uchar crcValue;
//
//				// 使用辅助函数计算CRC
//				calculateCRCWithTempZero(m->d, crcBytePos, vec.dataFrom, crcValue);
//
//				// 将计算出的CRC值放入正确的位置
//				int startbit = vec.start_bit % 8;
//				int length = vec.bit_len;
//
//				if (length <= 8)
//				{
//					// 清除原有数据
//					uchar mask = (0xFF << (startbit + length)) | (0xFF >> (8 - startbit));
//					m->d[crcBytePos] &= mask;
//
//					// 放入新的CRC值
//					m->d[crcBytePos] |= (crcValue << startbit) & ~mask;
//				}
//				else
//				{
//					// 对于超过8位的CRC，需要跨字节处理
//					// 这里简化处理，假设CRC不会超过8位
//					m->d[crcBytePos] = crcValue;
//				}
//			}
//		}
//		
//	}
//	return m_reqMsgArray;
//}

std::shared_ptr<QMap<uint32_t, _request_mannage_::Data8>> RequestTableManage::getReqArray()
{
	QMutexLocker locker(&m_dataMutex); // 添加互斥锁

	QMap<uint32_t, _request_mannage_::WgMap>_crc_;
	for (auto v : m_operator)
	{
		if (v.wg.dataFrom == "CRC" || v.wg.dataFrom == "CRC8" || v.wg.dataFrom == "CRC2F" || v.wg.dataFrom == "CRCKaiWo")
		{
			_crc_.insert(v.wg.id, v.wg);
		}
		else if (v.wg.dataFrom.contains("++"))
		{
			for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
			{
				if (m.key() == v.wg.id)
				{
					this->doSelfInput(v.wg, m->d);
					break;
				}
			}
		}
	}

	for (auto vec : _crc_)
	{
		// 检查ID是否在m_reqMsgArray中
		if (!m_reqMsgArray->contains(vec.id)) {
			continue; // 如果ID已被禁用，跳过CRC计算
		}

		for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
		{
			if (m.key() == vec.id)
			{
				int crcBytePos = vec.start_bit / 8;
				uchar crcValue;

				// 使用辅助函数计算CRC
				calculateCRCWithTempZero(m->d, crcBytePos, vec.dataFrom, crcValue);

				// 将计算出的CRC值放入正确的位置
				int startbit = vec.start_bit % 8;
				int length = vec.bit_len;

				if (length <= 8)
				{
					// 清除原有数据
					uchar mask = (0xFF << (startbit + length)) | (0xFF >> (8 - startbit));
					m->d[crcBytePos] &= mask;

					// 放入新的CRC值
					m->d[crcBytePos] |= (crcValue << startbit) & ~mask;
				}
				else
				{
					// 对于超过8位的CRC，需要跨字节处理
					// 这里简化处理，假设CRC不会超过8位
					m->d[crcBytePos] = crcValue;
				}
			}
		}
	}
	return m_reqMsgArray;
}

void RequestTableManage::doSelfOperator(uint32_t id)
{
	QMutexLocker locker(&m_dataMutex); // 添加互斥锁

	QMap<uint32_t, _request_mannage_::WgMap>_crc_;

	for (auto v : m_operator)
	{
		if (v.wg.id == id)
		{
			if (v.wg.dataFrom == "CRC" || v.wg.dataFrom == "CRC8" || v.wg.dataFrom == "CRC2F"|| v.wg.dataFrom == "CRCKaiWo")
			{
				_crc_.insert(v.wg.id, v.wg);
			}
			else if (v.wg.dataFrom.contains("++"))
			{
				for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
				{
					if (m.key() == v.wg.id)
					{
						this->doSelfInput(v.wg, m->d);
						break;
					}
				}
			}
		}
	}

	for (auto vec : _crc_)
	{
		// 检查ID是否在m_reqMsgArray中
		if (!m_reqMsgArray->contains(vec.id)) {
			continue; // 如果ID已被禁用，跳过CRC计算
		}

		for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
		{
			if (m.key() == vec.id)
			{
				int crcBytePos = vec.start_bit / 8;
				uchar crcValue;

				// 使用辅助函数计算CRC
				calculateCRCWithTempZero(m->d, crcBytePos, vec.dataFrom, crcValue);

				// 将计算出的CRC值放入正确的位置
				int startbit = vec.start_bit % 8;
				int length = vec.bit_len;

				if (length <= 8)
				{
					// 清除原有数据
					uchar mask = (0xFF << (startbit + length)) | (0xFF >> (8 - startbit));
					m->d[crcBytePos] &= mask;

					// 放入新的CRC值
					m->d[crcBytePos] |= (crcValue << startbit) & ~mask;
				}
				else
				{
					// 对于超过8位的CRC，需要跨字节处理
					// 这里简化处理，假设CRC不会超过8位
					m->d[crcBytePos] = crcValue;
				}
			}
		}
	}
}

//void RequestTableManage::doSelfOperator(uint32_t id)
//{
//	QMap<uint32_t, _request_mannage_::WgMap>_crc_;
//
//	for (auto v : m_operator)
//	{
//		if (v.wg.id == id)
//		{
//			if (v.wg.dataFrom == "CRC" || v.wg.dataFrom == "CRC8" || v.wg.dataFrom == "CRC2F")
//			{
//				_crc_.insert(v.wg.id, v.wg);
//			}
//
//			else if (v.wg.dataFrom.contains("++"))
//			{
//				for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
//				{
//					if (m.key() == v.wg.id)
//					{
//						this->doSelfInput(v.wg, m->d);
//						break;
//					}
//				}
//
//			}
//		}
//	}
//	for (auto vec : _crc_)
//	{
//		for (auto m = m_reqMsgArray->begin(); m != m_reqMsgArray->end(); m++)
//		{
//			if (m.key() == vec.id)
//			{
//				/*if (vec.dataFrom == "CRC")
//				{
//					m->d[vec.start_bit / 8] = this->crc_high_first(m->d, 7);
//				}
//				else if (vec.dataFrom == "CRC8")
//				{
//					m->d[vec.start_bit / 8] = this->SAEJ1850_CRC8(m->d, 7);
//				}
//				else if (vec.dataFrom == "CRC2F")
//				{
//					m->d[vec.start_bit / 8] = this->crc_0X2F(m->d, 7);
//				}*/
//				int crcBytePos = vec.start_bit / 8;
//				uchar crcValue;
//
//				// 使用辅助函数计算CRC
//				calculateCRCWithTempZero(m->d, crcBytePos, vec.dataFrom, crcValue);
//
//				// 将计算出的CRC值放入正确的位置
//				int startbit = vec.start_bit % 8;
//				int length = vec.bit_len;
//
//				if (length <= 8)
//				{
//					// 清除原有数据
//					uchar mask = (0xFF << (startbit + length)) | (0xFF >> (8 - startbit));
//					m->d[crcBytePos] &= mask;
//
//					// 放入新的CRC值
//					m->d[crcBytePos] |= (crcValue << startbit) & ~mask;
//				}
//				else
//				{
//					// 对于超过8位的CRC，需要跨字节处理
//					// 这里简化处理，假设CRC不会超过8位
//					m->d[crcBytePos] = crcValue;
//				}
//			}
//		}
//
//	}
//}

void RequestTableManage::InitWidget()
{
	m_usRoll = 0;
	GetModelData(0);
	DrawWidget();
	
}
void RequestTableManage::DrawWidget()
{
	this->m_reqMsgArray->clear();
	//释放之前组件
	for (auto n : m_operator)
	{
		delete n.qgp;
		n.qgp = nullptr;
	}
	//释放外面的组box
	for (auto v : m_vecGroup)
	{
		delete v;
		v = nullptr;
	}
	m_vecGroup.clear();
	m_operator.clear();
	//删除布局，这个才起作用
	if (vlayout) delete vlayout;
	vlayout = new QHBoxLayout();
	
	
	for (int i = 0; i < m_vecData.size(); i++)
	{
		//一个ID一个组框
		QGroupBox* idg = new QGroupBox(m_vecData.at(i).strCanId);
		//开启可选按钮，这个功能太牛逼了
		idg->setCheckable(true);
		m_vecGroup.append(idg);
		_request_mannage_::Data8 d8;
		memset(d8.d, 0, 8);
		if (m_vecData.at(i).isSend)
		{
			idg->setChecked(true);
			d8.r_len = m_vecData.at(i).len;
			m_reqMsgArray->insert(m_vecData.at(i).strCanId.toInt(nullptr, 16), d8);
		}
		else
			idg->setChecked(false);
		connect(idg, &QGroupBox::toggled, this, &RequestTableManage::on_idGroupboxStatus_Changed);
		QHBoxLayout* hlayout = new QHBoxLayout();
		for (int j = 0; j < m_vecData.at(i).pItem.size(); j++)
		{
			_request_mannage_::WgGroup wg;
			wg.qgp = new QGroupBox(m_vecData.at(i).pItem.at(j).bitName);

			//当前组件，信号的位置信息值等
			wg.wg.bit_len = m_vecData.at(i).pItem.at(j).bitLeng;
			wg.wg.factor = m_vecData.at(i).pItem.at(j).precision;
			wg.wg.start_bit = m_vecData.at(i).pItem.at(j).startBit;
			wg.wg.id = m_vecData.at(i).strCanId.toInt(nullptr, 16);
			wg.wg.offset = m_vecData.at(i).pItem.at(j).offset;
			wg.wg.dataFrom = m_vecData.at(i).pItem.at(j).dataFrom;
			if (m_vecData.at(i).pItem.at(j).stl_itemProperty.size() != 0)
			{

				//单个信号组内的布局
				QVBoxLayout* grounbox = new QVBoxLayout();

				//记录当前组件代表的值
				wg.wg.curValue = m_vecData.at(i).pItem.at(j).stl_itemProperty.at(0).value.toInt();
				setArrayValue(wg.wg.id, wg.wg.start_bit, wg.wg.bit_len, wg.wg.curValue, protocol);
				for (int m = 0; m < m_vecData.at(i).pItem.at(j).stl_itemProperty.size(); m++)
				{
					//组内的单选框
					QRadioButton* qrb = new QRadioButton(m_vecData.at(i).pItem.at(j).stl_itemProperty.at(m).toWord);

					//存储每个组件对应的值，后面点击了这个组件，就知道是什么人值了
					_request_mannage_::Property pg;
					pg.qwg = qrb;
					pg.value = m_vecData.at(i).pItem.at(j).stl_itemProperty.at(m).value.toInt();
					//给个默认状态
					if (m == 0)qrb->setChecked(true);

					//存储这个单选框
					wg.wg.wProperty.insert(m, pg);
					
					grounbox->addWidget(qrb);
					//单个信号操作
					wg.qgp->setLayout(grounbox);
					connect(qrb, &QRadioButton::toggled, this, &RequestTableManage::on_sigRadioButtonStatus_Changed);
				}
				grounbox->setSpacing(0);
			}
			else
			{
				QHBoxLayout* grounbox = new QHBoxLayout();
				//组内的单选框
				QLineEdit* qrb = new QLineEdit();
				
				qrb->setMaximumWidth(150);

				//存储每个组件对应的值，后面点击了这个组件，就知道是什么值了
				_request_mannage_::Property pg;
				pg.qwg = qrb;
				wg.wg.wProperty.insert(0, pg);
				
				grounbox->addWidget(qrb);
				//单个信号操作
				wg.qgp->setLayout(grounbox);
				wg.qgp->setMaximumWidth(150);
				wg.wg.curValue = 0;
				setArrayValue(wg.wg.id, wg.wg.start_bit, wg.wg.bit_len, wg.wg.curValue, protocol);
				connect(qrb, &QLineEdit::editingFinished, this, &RequestTableManage::on_sigLineedit_textChanged);
				if (wg.wg.dataFrom == "CRC" || wg.wg.dataFrom == "CRC8" || wg.wg.dataFrom == "CRC2F"|| wg.wg.dataFrom == "CRCKaiWo"||wg.wg.dataFrom.contains("++"))
				{
					qrb->setEnabled(false);
				}
			}
			wg.qgp->layout()->setSpacing(0);
			hlayout->addWidget(wg.qgp);
			m_operator.append(wg);
		}
		hlayout->setSpacing(0);
		idg->setLayout(hlayout);
		vlayout->addWidget(idg);
	}
	//不同ID信号的布局
	//vlayout->addLayout(hlayout);
	
	vlayout->addSpacerItem(new QSpacerItem(100, 100, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

	this->widget->setLayout(vlayout);
	this->widget->update();
}



void RequestTableManage::GetModelData(const int index)
{
	//获取数据类
	qGboleData* qgb = qGboleData::getInstance();
	if (!qgb)return;
	//若未初始化，需要初始化
	if (!qgb->getIsInit())
	{
		qgb->read();
	}
	//判断是否越界
	if (index<0 || index > qgb->pGboleData.size() - 1)return;

	m_vecData.clear();
	auto model = qgb->pGboleData.at(index);

	//获取发送的
	for (auto it : qgb->pGboleData.at(index).cItem)
	{
		if (1 == it.opt)
		{
			m_vecData.append(it);
		}
	}
	switch (model.agreement)
	{
		case 0:
			protocol = this->INTEL; break;
		case 1:
			protocol = this->MOTOROLA_MSB; break;
		case 2:
			protocol = this->MOTOROLA_LSB; break;
		default:
			protocol = this->INTEL; break;
	}
}
//void RequestTableManage::on_sigRadioButtonStatus_Changed(bool b)//25-08-15
//{
//	QRadioButton* qgb = dynamic_cast<QRadioButton*>(sender());
//	if (!qgb)return;
//	//一个按钮按下，意味着原来的按钮会释放，但是在数据上它们是互斥的，只需要处理按下的那个就可以了
//	if(b)
//	{
//		for (auto x : this->m_operator)
//		{
//			for (auto v : x.wg.wProperty)
//			{
//				//查找这个按钮
//				if (v.qwg == qgb)
//				{
//					//获取按钮对应的值
//					this->setArrayValue(x.wg.id, x.wg.start_bit, x.wg.bit_len, v.value, protocol);
//					return;
//				}
//			}
//
//		}
//	}
//}
//void RequestTableManage::on_sigLineedit_textChanged()
//{
//	QLineEdit*ledit = dynamic_cast<QLineEdit*>(sender());
//	if (!ledit)return;
//	QString str = ledit->text();
//	for (auto x : this->m_operator)
//	{
//		for (auto v : x.wg.wProperty)
//		{
//			//查找这个按钮
//		if (v.qwg == ledit)
//			{
//				//计算值
//				int t = str.toFloat()*x.wg.factor+x.wg.offset;
//				//设置值
//				setArrayValue(x.wg.id, x.wg.start_bit, x.wg.bit_len, t, protocol);
//				return;
//			}
//		}
//
//	}
//}
/*********************型号切换槽函数***************************
*@brief：当外面型号切换时，这个槽函数就要来响应他的，主要是更新
*发送的操作界面
*
*@parameters：
*			@ int index：型号的序号
*
***************************************************************/
void RequestTableManage::on_currentModelIndex_Changed(int index)
{
	m_reqMsgArray->clear();

	this->m_currModelIndex = index;

	GetModelData(this->m_currModelIndex);
	DrawWidget();
}
/*********************ID启用发送换槽函数***************************
*@brief：响应用户的启用和关闭某个ID的操作
*发送的操作界面
*
*@parameters：
*			@ bool b：启用与关闭某个ID
*
***************************************************************///25-08-15,修复再次勾选发送canID信号，CAN数据会清零的bug
//void RequestTableManage::on_idGroupboxStatus_Changed(bool b)
//{
//	//从组件头获取ID
//	QGroupBox* qgb = dynamic_cast<QGroupBox*>(sender());
//	if (!qgb)return;
//	uint32_t id = qgb->title().toUInt(nullptr,16);
//	
//
//	if (b)
//	{
//		//启用，插入一个数据
//		_request_mannage_::Data8 d8;
//		memset(d8.d, 0, sizeof(uchar) * 8);
//		for(auto v:m_vecData)
//		{
//			if (id == v.strCanId.toUInt(nullptr, 16))
//			{
//				d8.r_len = v.len;
//			}
//		}
//		m_reqMsgArray->insert(id, d8);
//		for (auto v : m_operator)
//		{
//			if (v.wg.id == id)
//			{
//				if (v.wg.wProperty.size() > 0)
//				{
//					this->setArrayValue(id, v.wg.start_bit, v.wg.bit_len, v.wg.wProperty.first().value, protocol);
//					//break;
//				}
//			}
//		}
//		
//	}
//	else
//	{
//		//关闭，删除它
//		auto it = m_reqMsgArray->find(id);
//		if (it != m_reqMsgArray->end())
//		{
//			m_reqMsgArray->erase(it);
//		}
//	}
//}
//计算掩码，用于置0之前的数据
uchar getMask(int leng,int start_bit)
{
	uchar mask;
	switch (leng)
	{
	case 8:
		mask = 0xFF; break;
	case 7:
		mask = 0x7F; break;
	case 6:
		mask = 0x3F; break;
	case 5:
		mask = 0x1F; break;
	case 4:
		mask = 0x0F; break;
	case 3:
		mask = 0x7; break;
	case 2:
		mask = 0x3; break;
	case 1:
		mask = 0x1; break;
	default:
		mask = 0xFF;
		break;
	}
	return mask << start_bit;
}
/*********************** setArrayValue ***********************
* @brief：把要发送的信号值按照通信协议上的格式塞到请求数组中
* @parameters：
*			@uint id：			信号值所在的ID
*			@int start_bit：	信号值的起始位
*			@const int lenght	信号值的长度
*			@const int value	信号值
*			@PROTOCOL protocol	CAN格式格式，intel或者motorola
* @return:	bool
*/
bool RequestTableManage::setArrayValue(const uint id, const int start_bit, const int lenght, const int value, PROTOCOL protocol)
{
	//版本请求的数据
	auto it = m_reqMsgArray->find(id);

	uchar* d = nullptr;
	//发送数据里面没有这个ID
	if (it == m_reqMsgArray->end())
	{
		return false;
	}
	else
	{//发送里面有这个，更新值就行
		d = it->d;
	}

	int bit_ = start_bit % 8;
	int byte = start_bit / 8;
	if (byte > 7) 
	{
		QLOG_WARN() << "start bit error: id=" << QString::number(id, 16).toUpper() << "start bit:" << start_bit;
		return false;
	}
	//计算有没有跨字节，大于8就是跨字节了
	int t_l = bit_ + lenght;
	uchar mask = getMask(lenght, bit_);
	if (t_l <= 8)
	{
		//清除
		d[byte] &= ~(mask);
		//赋值
		d[byte] |= 0xFF & (value << bit_);
	}
	else if (t_l <= 16)
	{
		switch (protocol)
		{
		case RequestTableManage::INTEL:
		{
			//获取掩码
			mask = getMask(8 - bit_, bit_);
			//清除
			d[byte] &= ~(mask);
			// 低位在低字节，起始位在低字节，往左数
			d[byte] |= 0xFF & (value << bit_);


			if (byte+1 > 7)
			{
				QLOG_WARN() << "lenght error: id=" << QString::number(id, 16).toUpper() << "start bit:" << start_bit;
				return false;
			}

			//获取掩码
			mask = getMask(lenght -(8 - bit_), 0);
			//清除之前的数值
			d[byte + 1] &= ~(mask);
			//高位在高字节
			d[byte + 1] |= 0xFF & (value >> (8 - bit_));
		}
		break;
		case RequestTableManage::MOTOROLA_LSB:
		{
			//获取掩码
			mask = getMask(8 - bit_, bit_);
			//清除
			d[byte] &= ~(mask);

			//低位放在高字节，起始字节在高位，往左数
			d[byte] += 0xFF & (value << bit_);


			if (byte - 1 < 0)
			{
				QLOG_WARN() << "lenght error: id=" << QString::number(id, 16).toUpper() << "start bit:" << start_bit;
				return false;
			}

			//获取掩码
			mask = getMask(lenght - (8 - bit_), 0);
			//清除之前的数值
			d[byte - 1] &= ~(mask);
			//高位放在低字节
			d[byte - 1] += 0xFF & (value >> (8 - bit_));
		}
		break;
		case RequestTableManage::MOTOROLA_MSB:
		{

			//放低字节的长度
			int H_l = bit_ + 1;
			//放高字节的长度
			int L_b = lenght - H_l;

			//获取清除低字节的掩码
			mask = getMask(H_l, 0);
			//清除
			d[byte] &= ~(mask);



			if (byte + 1 > 7)
			{
				QLOG_WARN() << "lenght error: id=" << QString::number(id, 16).toUpper() << "start bit:" << start_bit;
				return false;
			}

			//获取清除高字节的掩码
			mask = getMask(L_b, 8- L_b);
			d[byte + 1] &= ~(mask);

			//低位放高字节，记住，起始字节在低位，往右数
			d[byte] += 0xFF & (value >> H_l);
			d[byte + 1] += 0xFF & (value << (8 - L_b));
		}
		break;
		default:
			break;
		}
	}
	//插入发送数据的map去
	//transmit.insert(std::pair<uint32_t, uchar*>(id, d));

	return true;
}
bool RequestTableManage::intelProtocol(canIdData& cdata, uchar data[], unsigned int& fream_id)
{
	if (cdata.pItem.size() <= 0)
		return false;
	fream_id = cdata.strCanId.toUInt(NULL, 16);
	//memset(data, 0x0, 8);
	protoItem crcTemp;
	bool crc = false;
	bool hasCrc = false;//9-05
	QString crcType;

	int crcBytePos = -1;//9-05

	//for (int i = 0; i < cdata.pItem.size(); i++)
	//{
	//	const protoItem& itemp = cdata.pItem.at(i);
	//	int startbyte = itemp.startBit/8;
	//	int startbit = itemp.startBit;
	//	int lengght = itemp.bitLeng;
	//	int senddd = (int)(itemp.send * itemp.precision + itemp.offset);
	//	if (itemp.dataFrom == "CRC" || itemp.dataFrom == "CRC8"|| itemp.dataFrom=="CRC2F")
	//	{
	//		crcType = itemp.dataFrom;
	//		crcTemp = cdata.pItem.at(i);
	//		crc = true;
	//		continue;
	//	}
	//	//保证范围在0-7内
	//	//startbyte = YB::InRang(0, 7, startbyte);
	//	if (itemp.dataFrom.contains("++"))
	//	{
	//		int iTemp = itemp.dataFrom.mid(2).toInt();
	//		if (m_usRoll > iTemp)m_usRoll = 0;
	//		int pos = startbit % 8;
	//		data[startbyte] += (m_usRoll++) << pos;
	//		continue;
	//	}
	//	if (lengght <= 8)
	//	{
	//		int pos = startbit % 8;             //起止位，模8，1字节8位，uchar是1节长度的
	//		uchar m_send = senddd; //<< pos &0xff; //左移起止位，再&0xff，保证数据是不超过255
	//		//data[startbyte] += m_send;                  //加上去，有可能其它的数据也在这个字节里
	//		//*(data+ startbit) += m_send; 
	//		// 这里处理跨字节的情况                 
	//		if (startbit % 8 + lengght > 8)
	//		{
	//			int bit = 8 - (startbit % 8);
	//			switch (bit)
	//			{
	//			case 1:
	//				data[startbyte] += (senddd & 0x1) << 7;
	//				break;
	//			case 2:
	//				data[startbyte] += (senddd & 0x3) << 6;
	//				break;
	//			case 3:
	//				data[startbyte] += (senddd & 0x7) << 5;
	//				break;
	//			case 4:
	//				data[startbyte] += (senddd & 0xF) << 4;
	//				break;
	//			case 5:
	//				data[startbyte] += (senddd & 0x1F) << 3;
	//				break;
	//			case 6:
	//				data[startbyte] += (senddd & 0x3F) << 2;
	//				break;
	//			case 7:
	//				data[startbyte] += (senddd & 0x7F) << 1;
	//				break;
	//			case 8:
	//				data[startbyte] = (senddd & 0xFF);
	//				break;
	//			default:
	//				break;
	//			}
	//			if (startbyte < 7)
	//			{
	//				data[startbyte + 1] += senddd >> (8 - (startbit % 8));
	//			}
	//			else
	//			{
	//				//出错处理
	//			}
	//		}
	//		else
	//		{
	//			uchar m_send2 = senddd << pos & 0xff;
	//			data[startbyte] += m_send2;
	//		}
	//	}
	//	else if (lengght <= 16)
	//	{
	//		int pos = startbit % 8;
	//		uchar m_send = senddd << pos & 0xff; //低8位
	//		data[startbyte] += m_send;
	//		m_send = senddd >> 8 & 0xff;         //高8位
	//		if (startbyte < 7)
	//			data[startbyte + 1] += m_send;
	//	}
	//}
	////目前就只有博士的有CRC,现在多拉，科安创能也有，25-09-04
	//if (!crc)
	//	return true;
	//if (crcType == "CRC")
	//	data[crcTemp.startBit / 8] = crc_high_first(data, 7);
	//else if (crcType == "CRC8")
	//	data[crcTemp.startBit / 8] = SAEJ1850_CRC8(data, 7);
	//else if(crcType == "CRC2F")
	//	data[crcTemp.startBit / 8] = crc_0X2F(data, 7);
	//return true;
	// 第一遍：收集CRC信息
	for (int i = 0; i < cdata.pItem.size(); i++)
	{
		const protoItem& itemp = cdata.pItem.at(i);
		if (itemp.dataFrom == "CRC" || itemp.dataFrom == "CRC8" || itemp.dataFrom == "CRC2F"|| itemp.dataFrom == "CRCKaiWo")
		{
			crcType = itemp.dataFrom;
			crcTemp = itemp;
			crcBytePos = itemp.startBit / 8;
			hasCrc = true;
			break;
		}
	}

	// 第二遍：填充数据，跳过CRC字段
	for (int i = 0; i < cdata.pItem.size(); i++)
	{
		const protoItem& itemp = cdata.pItem.at(i);
		int startbyte = itemp.startBit / 8;
		int startbit = itemp.startBit;
		int lengght = itemp.bitLeng;
		int senddd = (int)(itemp.send * itemp.precision + itemp.offset);

		// 跳过CRC字段
		if (itemp.dataFrom == "CRC" || itemp.dataFrom == "CRC8" || itemp.dataFrom == "CRC2F" || itemp.dataFrom == "CRCKaiWo")
		{
			continue;
		}

		// 保证范围在0-7内
		if (itemp.dataFrom.contains("++"))
		{
			int iTemp = itemp.dataFrom.mid(2).toInt();
			if (m_usRoll > iTemp)m_usRoll = 0;
			int pos = startbit % 8;
			data[startbyte] += (m_usRoll++) << pos;
			continue;
		}

		if (lengght <= 8)
		{
			int pos = startbit % 8;
			uchar m_send2 = senddd << pos & 0xff;
			data[startbyte] += m_send2;
		}
		else if (lengght <= 16)
		{
			int pos = startbit % 8;
			uchar m_send = senddd << pos & 0xff; //低8位
			data[startbyte] += m_send;
			m_send = senddd >> 8 & 0xff;         //高8位
			if (startbyte < 7)
				data[startbyte + 1] += m_send;
		}
	}

	// 计算CRC
	if (hasCrc)
	{
		uchar crcValue;

		// 使用辅助函数计算CRC
		calculateCRCWithTempZero(data, crcBytePos, crcType, crcValue);

		// 将计算出的CRC值放入正确的位置
		int startbit = crcTemp.startBit % 8;
		int length = crcTemp.bitLeng;

		if (length <= 8)
		{
			// 清除原有数据
			uchar mask = (0xFF << (startbit + length)) | (0xFF >> (8 - startbit));
			data[crcBytePos] &= mask;

			// 放入新的CRC值
			data[crcBytePos] |= (crcValue << startbit) & ~mask;
		}
		else
		{
			// 对于超过8位的CRC，需要跨字节处理
			// 这里简化处理，假设CRC不会超过8位
			data[crcBytePos] = crcValue;
		}
	}

	return true;
}
bool RequestTableManage::motoProtocol(canIdData& cdata, uchar data[], unsigned int& fream_id)
{
	if (cdata.pItem.size() <= 0)
		return false;
	fream_id = cdata.strCanId.toUInt(NULL, 16);
	bool crc = false;
	QString crcType;
	protoItem crcTemp;
	for (int i = 0; i < cdata.pItem.size(); i++)
	{
		const protoItem& itemp = cdata.pItem.at(i);
		int startbyte = itemp.startBit/8;
		int startbit = itemp.startBit;
		int lengght = itemp.bitLeng;
		int senddd = itemp.send * itemp.precision + itemp.offset;
		
		if (itemp.dataFrom == "CRC" || itemp.dataFrom == "CRC8" ||itemp.dataFrom == "CRC2F"|| itemp.dataFrom =="CRCKaiWo")
		{
			crcType = itemp.dataFrom;
			crcTemp = cdata.pItem.at(i);
			crc = true;
			continue;
		}

		if (itemp.dataFrom.contains("++"))
		{
			int iTemp = itemp.dataFrom.mid(2).toInt();
			if (m_usRoll > iTemp)m_usRoll = 0;
			int pos = startbit % 8;
			data[startbyte] += (m_usRoll++) << pos;
			continue;
		}
		if (lengght <= 8)
		{
			int pos = startbit % 8;             //起止位，模8，1字节8位，uchar是1节长度的
			uchar m_send = senddd; //<< pos &0xff; //左移起止位，再&0xff，保证数据是不超过255
			//data[startbyte] += m_send;                  //加上去，有可能其它的数据也在这个字节里
			//*(data+ startbit) += m_send; 
			// 这里处理跨字节的情况                 
			if (startbit % 8 + lengght > 8)
			{
				int bit = 8 - (startbit % 8);
				switch (bit)
				{
				case 1:
					data[startbyte + 1] += (senddd & 0x1) << 7;
					break;
				case 2:
					data[startbyte + 1] += (senddd & 0x3) << 6;
					break;
				case 3:
					data[startbyte + 1] += (senddd & 0x7) << 5;
					break;
				case 4:
					data[startbyte + 1] += (senddd & 0xF) << 4;
					break;
				case 5:
					data[startbyte + 1] += (senddd & 0x1F) << 3;
					break;
				case 6:
					data[startbyte + 1] += (senddd & 0x3F) << 2;
					break;
				case 7:
					data[startbyte + 1] += (senddd & 0x7F) << 1;
					break;
				case 8:
					data[startbyte + 1] = (senddd & 0xFF);
					break;
				default:
					break;
				}
				if (startbyte < 7)
				{
					data[startbyte] += senddd >> (8 - (startbit % 8));
				}
				else
				{
					//出错处理
				}
			}
			else
			{
				uchar m_send2 = senddd << pos & 0xff;
				data[startbyte] += m_send2;
			}
		}
		else if (lengght <= 16)
		{
			int pos = startbit % 8;
			uchar m_send = senddd << pos & 0xff; //低8位
			data[startbyte + 1] += m_send;
			m_send = senddd >> 8 & 0xff;         //高8位
			data[startbyte] += m_send;
		}
		else if (lengght <= 24)
		{
			if (0 != startbit % 8)
			{

			}
			else
			{
				if (startbyte - 2 < 0)
				{
					QLOG_INFO() << "start byte error";
					return false;
				}
				data[startbyte - 2] += (senddd >> 16) & 0xFF;
				data[startbyte - 1] += (senddd >> 8) & 0xFF;
				data[startbyte] += (senddd & 0xFF);
			}


		}
	}
	if (crcType == "CRC")
		data[crcTemp.startBit / 8] = crc_high_first(data, 7);
	else if (crcType == "CRC8")
		data[crcTemp.startBit / 8] = SAEJ1850_CRC8(data, 7);
	else if (crcType == "CRC2F")
		data[crcTemp.startBit / 8] = crc_0X2F(data, 7);
	else if(crcType == "CRCKaiWo")
		data[crcTemp.startBit / 8] = crc_KaiWo(data, 8);
	return true;
}
void RequestTableManage::doSelfInput(const _request_mannage_::WgMap& itemp,uchar* array)
{
	QMutexLocker lock(&this->m_mutex);
	
	int iTemp = itemp.dataFrom.mid(2).toInt();

	if (v_usRoll.find(itemp.id) == v_usRoll.end())
	{
		v_usRoll.insert(std::pair(itemp.id, 0));
	}
	if (v_usRoll[itemp.id] > iTemp)v_usRoll[itemp.id] = 0;
	int pos = itemp.start_bit % 8;
	array[itemp.start_bit/8] = (v_usRoll[itemp.id]++) << pos;
	//qDebug() << "itemp.id: " << itemp.id<< ",v_usRoll[itemp.id]: " << v_usRoll[itemp.id];
}
unsigned char RequestTableManage::crc_high_first(const uchar data[], unsigned char len)
{
	//    unsigned char i;
	unsigned char crc = 0xFF; /* 计算的初始crc值 */
	const  uchar* ptr = &data[1];
	//    *ptr++;

	while (len--)
	{
		crc ^= *ptr++;  /* 每次先与需要计算的数据异或,计算完指向下一数据 */
		for (int i = 8; i > 0; --i)   /* 下面这段计算过程与计算一个字节crc一样 */
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ 0x2F;
			else
				crc = (crc << 1);

		}
	}
	crc ^= 0xFF;
	return (crc);
}
//uchar RequestTableManage::SAEJ1850_CRC8(uchar* data, uchar length)
//{
//	uchar t_crc;
//	uchar f, b;
//	//初值
//	t_crc = 0xFF;
//	for (f = 0; f < length; f++)
//	{
//		t_crc ^= data[f];
//		for (b = 0; b < 8; b++)
//		{
//			if ((t_crc & 0x80) != 0)
//			{
//				t_crc <<= 1;
//				t_crc ^= 0x1D;//多项值0x1D
//			}
//			else
//			{
//				t_crc <<= 1;
//			}
//		}
//	}
//	return ~t_crc;
//}

uchar RequestTableManage::SAEJ1850_CRC8(uchar* data, uchar length)
{
	uchar i, j;
	uchar crc = 0x00;  // SAE J1850 CRC-8 初始值为 0x00
	const uchar poly = 0x1D;  // SAE J1850 多项式：0x1D (x? + x? + x? + x? + 1)

	for (i = 0; i < length; i++)
	{
		crc ^= data[i];  // 逐字节异或

		for (j = 0; j < 8; j++)  // 每字节处理 8 位
		{
			if (crc & 0x80)  // 检查最高位是否为 1
			{
				crc = (crc << 1) ^ poly;  // 左移并异或多项式
			}
			else
			{
				crc <<= 1;  // 仅左移
			}
		}
	}

	return crc;  // 返回计算后的 CRC-8 值
}

unsigned char RequestTableManage::crc_0X2F(const uchar data[], unsigned char len)
{
	//for (int i = 0; i < 8; i++)
	//{
	//	qDebug() << data[i];
	//}
	//
	//unsigned char crc = 0xFF; // 初始值
	//unsigned char i;
	//while (len--) {
	//	crc ^= *data++; // 与当前数据字节异或
	//	for (i = 8; i > 0; --i) { // 处理每个字节的8位
	//		if (crc & 0x80) {
	//			crc = (crc << 1) ^ 0x2F; // 多项式 0x2F
	//		}
	//		else {
	//			crc = (crc << 1);
	//		}
	//	}
	//}
	//return crc ^ 0xFF; // 最终异或值

	data++;      // 指针移动到第二个字节
	len--;       // 长度减1

	unsigned char crc = 0xFF; // 初始值
	unsigned char i;
	while (len--) {
		crc ^= *data++; // 与当前数据字节异或
		for (i = 8; i > 0; --i) { // 处理每个字节的8位
			if (crc & 0x80) {
				crc = (crc << 1) ^ 0x2F; // 多项式 0x2F
			}
			else {
				crc = (crc << 1);
			}
		}
	}
	return crc ^ 0xFF; // 最终异或值
}

//unsigned char RequestTableManage::crc_KaiWo(const uchar data[], unsigned char len)
//{
//	// 计算Temp：对所有字节(除了RollingCounter)进行异或
//	unsigned char Temp = 0;
//
//	// 遍历所有数据字节（除了RollingCounter所在的低4位）
//	// 根据表格，RollingCounter在最后一个字节的低4位
//	qDebug() << data;
//	for (unsigned char i = 0; i < len - 1; i++)
//	{
//		Temp ^= data[i];
//	}
//
//	// 处理最后一个字节的高4位（CRC位不参与计算，只处理其他位）
//	// 最后一个字节：bit7-4是CRC，bit3-0是RollingCounter
//	// 在计算Temp时，我们只考虑除了RollingCounter之外的部分
//	// 所以将最后一个字节的高4位参与异或计算
//	//unsigned char lastByteHighNibble = data[len - 1] & 0xF0; // 取高4位
//	//Temp ^= lastByteHighNibble;
//
//	// 提取Temp的高4位和低4位
//	unsigned char highNibble = (Temp >> 4) & 0x0F;
//	unsigned char lowNibble = Temp & 0x0F;
//
//	// 提取RollingCounter（最后一个字节的低4位）
//	unsigned char rollingCounter = (data[len - 1])>>4 & 0x0F;
//
//	// 计算Checksum
//	unsigned char checksum = highNibble ^ lowNibble ^ rollingCounter;
//
//	return checksum;
//	//return data[0] + data[1] + data[2];
//}


unsigned char RequestTableManage::crc_KaiWo(const uchar data[], unsigned char len) {
	unsigned char temp = 0x00;

	// Step 1: XOR over all bytes except RollingCounter (byte 7)
	for (unsigned char i = 0; i < len-1 ; i++) {
		qDebug() << data[i];
		temp ^= data[i];
	}

	// Step 2: XOR over low 4 bits of last byte (RollingCounter)
	// Note: This is part of the "except RollingCounter" exclusion in the first XOR
	// RollingCounter is at data[len-1] low nibble
	unsigned char rolling_counter = data[len - 1] & 0x0F;

	// Step 3: Calculate high and low nibbles of temp
	unsigned char high_nibble = (temp >> 4) & 0x0F;
	unsigned char low_nibble = temp & 0x0F;

	// Step 4: Final checksum calculation
	unsigned char checksum = (high_nibble ^ low_nibble ^ rolling_counter) & 0x0F;

	return checksum;
}

unsigned char RequestTableManage::checksumXOR(const uchar data[])
{
	return (data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6] ^ data[7]);
}


//辅助备份函数
/*********************恢复UI状态***************************/
void RequestTableManage::restoreUiState(uint32_t id)
{
	for (auto& x : m_operator) {
		if (x.wg.id != id) continue;

		for (auto& prop : x.wg.wProperty) {
			if (QRadioButton* rb = dynamic_cast<QRadioButton*>(prop.qwg)) {
				// 恢复单选框状态
				rb->blockSignals(true);
				rb->setChecked(prop.value == getArrayValue(id, x.wg.start_bit,
					x.wg.bit_len, protocol));
				rb->blockSignals(false);
			}
			else if (QLineEdit* le = dynamic_cast<QLineEdit*>(prop.qwg)) {
				// 恢复文本框值
				int rawValue = getArrayValue(id, x.wg.start_bit,
					x.wg.bit_len, protocol);
				float displayValue = (rawValue - x.wg.offset) / x.wg.factor;

				le->blockSignals(true);
				le->setText(QString::number(displayValue));
				le->blockSignals(false);
			}
		}
	}
}

/*********************保存UI状态***************************/
void RequestTableManage::saveUiState(uint32_t id)
{
	// 不需要额外保存，因为数据已备份
	// UI状态会在恢复时从备份数据重建
}

/*********************设置数据值(直接操作数据结构)***************************/
void RequestTableManage::setArrayValueForData(_request_mannage_::Data8& data,
	int start_bit, int bit_len,
	int value, int protocol)
{
	// 实现与setArrayValue相同的逻辑，但直接操作Data8结构
	// 示例伪代码：
	uint8_t* bytes = data.d;
	// 计算字节偏移和位偏移
	int byte_offset = start_bit / 8;
	int bit_offset = start_bit % 8;

	// 确保操作在有效范围内
	if (byte_offset >= 0 && byte_offset < 8) {
		// 提取原始值
		uint32_t current = 0;
		memcpy(&current, bytes + byte_offset, sizeof(uint32_t));

		// 创建掩码
		uint32_t mask = (1 << bit_len) - 1;

		// 清除目标位
		current &= ~(mask << bit_offset);

		// 设置新值
		current |= (value & mask) << bit_offset;

		// 写回数据
		memcpy(bytes + byte_offset, &current, sizeof(uint32_t));
	}
}

/*********************获取数据值***************************/
int RequestTableManage::getArrayValue(uint32_t id, int start_bit,
	int bit_len, int protocol)
{
	// 实现从数据结构中读取值的逻辑
	// 这里需要您根据实际的数据操作逻辑实现
	// 示例伪代码：
	if (m_reqMsgArray->contains(id)) {
		_request_mannage_::Data8 data = (*m_reqMsgArray)[id];
		uint8_t* bytes = data.d;

		int byte_offset = start_bit / 8;
		int bit_offset = start_bit % 8;

		if (byte_offset >= 0 && byte_offset < 8) {
			uint32_t value = 0;
			memcpy(&value, bytes + byte_offset, sizeof(uint32_t));

			// 创建掩码
			uint32_t mask = (1 << bit_len) - 1;

			// 提取值
			return (value >> bit_offset) & mask;
		}
	}
	return 0;
}

//------------------------------------------------------------
/*********************ID启用发送换槽函数***************************/
//void RequestTableManage::on_idGroupboxStatus_Changed(bool b)
//{
//	QGroupBox* qgb = dynamic_cast<QGroupBox*>(sender());
//	if (!qgb) return;
//
//	uint32_t id = qgb->title().toUInt(nullptr, 16);
//
//	if (b) // 启用
//	{
//		// 检查是否有备份数据
//		if (m_disabledData.contains(id))
//		{
//			// 恢复备份数据
//			m_reqMsgArray->insert(id, m_disabledData[id]);
//			m_disabledData.remove(id);
//
//			// 更新UI状态
//			restoreUiState(id);
//		}
//		else // 首次启用
//		{
//			// 初始化数据
//			_request_mannage_::Data8 d8;
//			memset(d8.d, 0, sizeof(uchar) * 8);
//
//			// 设置数据长度
//			for (auto& v : m_vecData) {
//				if (id == v.strCanId.toUInt(nullptr, 16)) {
//					d8.r_len = v.len;
//					break;
//				}
//			}
//			m_reqMsgArray->insert(id, d8);
//
//			// 初始化属性值
//			for (auto& v : m_operator) {
//				if (v.wg.id == id && v.wg.wProperty.size() > 0) {
//					this->setArrayValue(id, v.wg.start_bit, v.wg.bit_len,
//						v.wg.wProperty.first().value, protocol);
//					break;
//				}
//			}
//		}
//	}
//	else // 禁用
//	{
//		// 备份当前数据
//		auto it = m_reqMsgArray->find(id);
//		if (it != m_reqMsgArray->end()) {
//			m_disabledData[id] = it.value(); // 保存数据备份
//			m_reqMsgArray->erase(it);
//		}
//
//		// 保存UI状态
//		saveUiState(id);
//	}
//}
void RequestTableManage::on_idGroupboxStatus_Changed(bool b)
{
	QGroupBox* qgb = dynamic_cast<QGroupBox*>(sender());
	if (!qgb) return;

	uint32_t id = qgb->title().toUInt(nullptr, 16);

	// 添加互斥锁保护数据访问
	QMutexLocker locker(&m_dataMutex);

	if (b) // 启用
	{
		// 检查是否有备份数据
		if (m_disabledData.contains(id))
		{
			// 恢复备份数据
			m_reqMsgArray->insert(id, m_disabledData[id]);
			m_disabledData.remove(id);

			// 更新UI状态
			restoreUiState(id);
		}
		else // 首次启用
		{
			// 初始化数据
			_request_mannage_::Data8 d8;
			memset(d8.d, 0, sizeof(uchar) * 8);

			// 设置数据长度
			for (auto& v : m_vecData) {
				if (id == v.strCanId.toUInt(nullptr, 16)) {
					d8.r_len = v.len;
					break;
				}
			}
			m_reqMsgArray->insert(id, d8);

			// 初始化属性值
			for (auto& v : m_operator) {
				if (v.wg.id == id && v.wg.wProperty.size() > 0) {
					this->setArrayValue(id, v.wg.start_bit, v.wg.bit_len,
						v.wg.wProperty.first().value, protocol);
					break;
				}
			}
		}
	}
	else // 禁用
	{
		// 备份当前数据
		auto it = m_reqMsgArray->find(id);
		if (it != m_reqMsgArray->end()) {
			m_disabledData[id] = it.value(); // 保存数据备份
			m_reqMsgArray->erase(it);
		}

		// 保存UI状态
		saveUiState(id);
	}
}
/*********************单选框状态变更槽函数***************************/
void RequestTableManage::on_sigRadioButtonStatus_Changed(bool b)
{
	QRadioButton* qrb = dynamic_cast<QRadioButton*>(sender());
	if (!qrb || !b) return; // 只处理选中状态

	for (auto& x : this->m_operator) {
		for (auto& v : x.wg.wProperty) {
			if (v.qwg == qrb) {
				// 获取当前ID的启用状态
				bool isEnabled = m_reqMsgArray->contains(x.wg.id);

				if (isEnabled) {
					// 直接设置值
					this->setArrayValue(x.wg.id, x.wg.start_bit,
						x.wg.bit_len, v.value, protocol);
				}
				else {
					// 保存到备份数据
					if (m_disabledData.contains(x.wg.id)) {
						this->setArrayValueForData(m_disabledData[x.wg.id],
							x.wg.start_bit, x.wg.bit_len,
							v.value, protocol);
					}
				}
				return;
			}
		}
	}
}

/*********************文本框变更槽函数***************************/
void RequestTableManage::on_sigLineedit_textChanged()
{
	//QLineEdit* ledit = dynamic_cast<QLineEdit*>(sender());
	//if (!ledit) return;

	//QString str = ledit->text();
	//bool ok;
	//float value = str.toFloat(&ok);
	//if (!ok) return; // 转换失败时忽略

	//for (auto& x : this->m_operator) {
	//	for (auto& v : x.wg.wProperty) {
	//		if (v.qwg == ledit) {
	//			// 计算实际值
	//			int t = value * x.wg.factor + x.wg.offset;

	//			// 获取当前ID的启用状态
	//			bool isEnabled = m_reqMsgArray->contains(x.wg.id);

	//			if (isEnabled) {
	//				// 直接设置值
	//				this->setArrayValue(x.wg.id, x.wg.start_bit,
	//					x.wg.bit_len, t, protocol);
	//			}
	//			else {
	//				// 保存到备份数据
	//				if (m_disabledData.contains(x.wg.id)) {
	//					this->setArrayValueForData(m_disabledData[x.wg.id],
	//						x.wg.start_bit, x.wg.bit_len,
	//						t, protocol);
	//				}
	//			}
	//			return;
	//		}
	//	}
	//}
	QLineEdit* ledit = dynamic_cast<QLineEdit*>(sender());
	if (!ledit) return;

	QString str = ledit->text();
	bool ok = false;
	float value = 0.0f;

	// 处理空字符串的情况
	if (str.isEmpty()) {
		// 当内容为空时，我们将其视为0
		value = 0.0f;
		ok = true;
	}
	else {
		// 非空内容正常转换
		value = str.toFloat(&ok);
	}

	if (!ok) return; // 转换失败时忽略

	for (auto& x : this->m_operator) {
		for (auto& v : x.wg.wProperty) {
			if (v.qwg == ledit) {
				// 计算实际值
				int t = value * x.wg.factor + x.wg.offset;

				// 获取当前ID的启用状态
				bool isEnabled = m_reqMsgArray->contains(x.wg.id);

				if (isEnabled) {
					// 直接设置值
					this->setArrayValue(x.wg.id, x.wg.start_bit,
						x.wg.bit_len, t, protocol);
				}
				else {
					// 保存到备份数据
					if (m_disabledData.contains(x.wg.id)) {
						this->setArrayValueForData(m_disabledData[x.wg.id],
							x.wg.start_bit, x.wg.bit_len,
							t, protocol);
					}
				}
				return;
			}
		}
	}
}

void RequestTableManage::calculateCRCWithTempZero(uchar* data, int crcBytePos, const QString& crcType, uchar& crcValue)
{
	// 添加边界检查
	if (crcBytePos < 0 || crcBytePos >= 8) {
		crcValue = 0;
		return;
	}

	uchar tempCrcValue = data[crcBytePos];

	// 将CRC字段位置设置为0，以便计算CRC
	data[crcBytePos] = 0;

	// 计算CRC
	if (crcType == "CRC")
		crcValue = crc_high_first(data, 7);
	else if (crcType == "CRC8")
		crcValue = SAEJ1850_CRC8(data, 7);
	else if (crcType == "CRC2F")
		crcValue = crc_0X2F(data, 8);
	else if(crcType == "CRCKaiWo")
		crcValue = crc_KaiWo(data, 8);
	else
		crcValue = 0;

	// 恢复CRC字段位置的值
	data[crcBytePos] = tempCrcValue;
}