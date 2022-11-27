#include "chex.h"

const QString HexTypeTable[6] =
{
    "00","01","02","03","04","05",
};

CHex::CHex()
{

}

char CHex::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return (ch-0x30);
    else if((ch >= 'A') && (ch <= 'F'))
        return ((ch-'A')+10);
    else if((ch >= 'a') && (ch <= 'f'))
        return ((ch-'a')+10);
    else return (-1);
}

EHexErrorCode CHex::getHexLineData(QByteArray bydata,stHexLineData *p)
{
    quint8 i = 0;
    quint8 cs_temp = 0;
    QString str(bydata);
    char *pcdata = bydata.data();
    quint32 linelen = str.size();
    if((linelen < MIN_HEX_LINE_COUNT_LENGHT)) {return HEX_LENGHT_ERROR;}
    if(*pcdata != 0x3A) {return HEX_FORMAT_ERROR;}//必须以":"号开始
    //获取Type
    QString stype = str.mid(7,2);
    for(i = 0; i < RECORD_HEX_MAX; i++)
    {
        if(stype == HexTypeTable[i])
        {
            p->type = (emHexType)i;
            break;
        }
    }
    if(i == RECORD_HEX_MAX) {qDebug("HEX_FORMAT_ERROR");return HEX_FORMAT_ERROR;}
    cs_temp += (ConvertHexChar(*(pcdata + 7)) << 4) | ConvertHexChar(*(pcdata + 8));
    //获取count
    p->count = (ConvertHexChar(*(pcdata + 1)) << 4) | ConvertHexChar(*(pcdata + 2));
    cs_temp += p->count;
    if(p->count != (((linelen - 2) / 2) - 4)) {qDebug("HEX_FORMAT_ERROR");return HEX_FORMAT_ERROR;}
    //获取address
    p->address = (ConvertHexChar(*(pcdata + 3)) << 12) | (ConvertHexChar(*(pcdata + 4)) << 8) | (ConvertHexChar(*(pcdata + 5)) << 4) | ConvertHexChar(*(pcdata + 6));
    cs_temp += (p->address >> 8) & 0xFF;
    cs_temp += p->address & 0xFF;
    //获取data
    for(i = 0; i < p->count; i++)
    {
        p->data[i] = (ConvertHexChar(*(pcdata + 2*i + 9)) << 4) | ConvertHexChar(*(pcdata + 2*i + 10));

        cs_temp += p->data[i];
    }
    if(p->count % 4 != 0)
    {
        for(int j = 0; j < 4-(p->count%4); j++)
            p->data[p->count+j] = 0xff;
    }
    p->checksum = (ConvertHexChar(*(pcdata + 2*i + 9)) << 4) | ConvertHexChar(*(pcdata + 2*i + 10));
    if(p->checksum != ((0x100 - cs_temp) & 0xFF))
    {
        qDebug("HEX_VERIFY_ERROR");
        return HEX_VERIFY_ERROR;
    }
    p->datalen = p->count;

    return HEX_NO_ERROR;
}
