#include "MsgParser.h"
#include "QsLog.h"
MsgParser::MsgParser(QObject *parent)
	: QObject(parent)
{}

MsgParser::~MsgParser()
{

}
float MsgParser::intel_Parser(const protoItem& pItem, const QByteArray& data, bool isSend)
{
        QStringList binaryStr = QByteToBinary(data);
        int startByte = pItem.startByte;
        int startBit = pItem.startBit;
        int startLenght = pItem.bitLeng;
        float precision = pItem.precision;
        int offset = pItem.offset;
        bool octHex = pItem.octhex;
        parseData pd;
        float temp = 0;
        if (startByte > binaryStr.size() - 1)
        {
            QLOG_WARN() << "�����ֽڳ�����Χ";
            return temp;
        }
        QString datafrom = pItem.dataFrom;
        //�ж��Ƿ���ֽڣ���ֹλģ8���ó��ǵ�ǰ�ֽڵ���ֹλ���ټӸ�����
        if (datafrom != "-1")
        {
            return -1;
        }
        else if (datafrom.contains("XOR"))
        {
            return -2;
        }
        else
        {
            //�ж��Ƿ���ֽڣ���ֹλģ8���ó��ǵ�ǰ�ֽڵ���ֹλ���ټӸ�����
            int len = startBit % 8 + startLenght;
            if (len <= 8)
            {   //�����ֽڣ�����ͱȽϼ���
                //15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
                //^��λ��ǰ����λ�ں�
                if (octHex)
                {
                    int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
                    QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                    temp = ss.toInt() * precision + offset;
                }
                else
                {
                    temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
                }
            }
            else if (len <= 16)
            {
                if (octHex)
                {
                    int nn = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2);
                    QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                    temp = ss.toInt() * precision + offset;
                }
                else
                {
                    temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;;
                }
            }
            else if (len <= 24)
            {
                if (octHex)
                {
                    int nn = (binaryStr[startByte + 2].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte + 1] + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2);
                    QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                    temp = ss.toInt() * precision + offset;
                }
                else
                {
                    temp = (binaryStr[startByte + 2].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8))+ binaryStr[startByte + 1] + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;;
                }
            }
            
        }
        return temp;
}

float MsgParser::moto_Msb_Parser(const protoItem& pItem, const QByteArray& data, bool isSend)
{
    QStringList binaryStr = QByteToBinary(data);
    int startByte = pItem.startByte;
    int startBit = pItem.startBit;
    int startLenght = pItem.bitLeng;
    float precision = pItem.precision;
    int offset = pItem.offset;
    QString datafrom = pItem.dataFrom;
    bool octHex = pItem.octhex;
    parseData pd;
    float temp = 0;
    if (startByte > binaryStr.size() - 1)
    {
        QLOG_WARN() << "�����ֽڳ�����Χ";
        return temp;
    }
    //�ж��Ƿ���ֽڣ���ֹλģ8���ó��ǵ�ǰ�ֽڵ���ֹλ���ټӸ�����
    //if (datafrom != "-1")
    if (datafrom.contains("*") || datafrom.contains("/"))
    {
                
    }
    else if (datafrom.contains("XOR"))
    {
                
    }
    else
    {
        int len = startBit % 8 + startLenght;
        if (len <= 8)
        {   //�����ֽڣ�����ͱȽϼ���
            //15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
            //^��λ��ǰ����λ�ں�

            if (octHex)
            {
                int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
                QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                temp = ss.toInt() * precision + offset;
            }
            else
            {
                
                temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) * precision + offset;
                //2023 12.21�����������
                //temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
            }

        }
        else if (len <= 16)
        {

            if (octHex)
            {
                int nn = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2);
                QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                temp = ss.toInt() * precision + offset;
            }
            else
            {
                temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) * precision + offset;
            }

        }
        else if (len <= 24)
        {

            if (octHex)
            {
                int nn = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1] + binaryStr[startByte + 2].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2);
                QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                temp = ss.toInt() * precision + offset;
            }
            else
            {
                temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte+1] +binaryStr[startByte + 2].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) * precision + offset;
            }

        }
    }
    return temp;
    
}

float MsgParser::moto_Lsb_Parser(const protoItem& pItem, const QByteArray& data, bool isSend)
{

    QStringList binaryStr = QByteToBinary(data);
    int startByte = pItem.startByte;
    int startBit = pItem.startBit;
    int startLenght = pItem.bitLeng;
    float precision = pItem.precision;
    int offset = pItem.offset;
    bool octHex = pItem.octhex;
    parseData pd;
    float temp = 0;
    if (startByte > binaryStr.size() - 1)
    {
        QLOG_WARN() << "�����ֽڳ�����Χ";
        return temp;
    }
    QString datafrom = pItem.dataFrom;
    //�ж��Ƿ���ֽڣ���ֹλģ8���ó��ǵ�ǰ�ֽڵ���ֹλ���ټӸ�����
    if (datafrom != "-1")
    {
        return -1;
    }
    else if (datafrom.contains("XOR"))
    {
        return -2;
    }
    else
    {
        //�ж��Ƿ���ֽڣ���ֹλģ8���ó��ǵ�ǰ�ֽڵ���ֹλ���ټӸ�����
        int len = startBit % 8 + startLenght;
        if (len <= 8)
        {   //�����ֽڣ�����ͱȽϼ���
            //15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
            //^��λ��ǰ����λ�ں�
            if (octHex)
            {
                int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
                QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                temp = ss.toInt() * precision + offset;
            }
            else
            {
                temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
            }
        }
        else if (len <= 16)
        {
            if (octHex)
            {
                int nn = (binaryStr[startByte - 1].mid(16 + (startBit % 8)- startLenght, startLenght - 8 + (startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2);
                QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                temp = ss.toInt() * precision + offset;
            }
            else
            {
                temp = (binaryStr[startByte - 1].mid(16 + (startBit % 8) - startLenght, startLenght - 8 + (startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;
            }
        }else if(len <= 24)
        {
            //�������ֽڵģ�Ӧ��û��.��ƨ����߹�ľ���
            if (octHex)
            {
                int nn = (binaryStr[startByte - 2].mid(16 + (startBit % 8) - startLenght, startLenght - 8 + (startBit % 8)) + binaryStr[startByte - 1] + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2);
                QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                temp = ss.toInt() * precision + offset;
            }
            else
            {
                temp = (binaryStr[startByte - 2].mid(16 + (startBit % 8) - startLenght, startLenght - 8 + (startBit % 8)) + binaryStr[startByte-1] + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;
                
            }
        }
    }
    return temp;
}

QStringList MsgParser::QByteToBinary(const QByteArray& data)
{
    QStringList binaryStr;
    QString hex;
    for (int k = 0; k < data.size(); ++k)
    {
        QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
        binaryStr.append(str);

    }
    return binaryStr;
}
