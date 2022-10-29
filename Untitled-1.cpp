void qtexcel::writeExcelFast(QString fileName, QList<QList<QVariant>> & x_y)
{
	workbooks->dynamicCall("Add");//新建一个工作表。 新工作表将成为活动工作表
	workbook = excel->querySubObject("ActiveWorkBook");// 获取活动工作簿 
	worksheet = workbook->querySubObject("Sheets(int)", 1); //获取第一个工作表，最后参数填1
	
	int row = x_y.size();//行数
	int col = x_y.at(0).size();//列数
	/*将列数转换成EXCEL中列的字母形式*/
	QString rangStr;
	convert2ColName(col, rangStr);
	rangStr += QString::number(row);
	rangStr = "A1:" + rangStr;
	usedrange_Write = worksheet->querySubObject("Range(const QString&)", rangStr);

	QVariant var;
	castListListVariant2Variant(x_y, var);
	usedrange_Write->setProperty("Value", var);

	workbook->dynamicCall("SaveCopyAs(QString)", QDir::toNativeSeparators(fileName));
	workbook->dynamicCall("Close(bool)", false);  //关闭文件
	excel->dynamicCall("Quit()");//关闭excel
}

void qtexcel::castListListVariant2Variant(QList<QList<QVariant>>& cells, QVariant & res)
{
	QVariantList vars;
	const int rowCount = cells.size();
	for (int i = 0; i < rowCount; ++i)
	{
		vars.append(QVariant(cells[i]));
	}
	res = QVariant(vars);
}

// brief 把列数转换为excel的字母列号
// param data 大于0的数
// return 字母列号，如1->A 26->Z 27 AA
void qtexcel::convert2ColName(int data, QString &res)
{
	Q_ASSERT(data > 0 && data < 65535);
	int tempData = data / 26;
	if (tempData > 0)
	{
		int mode = data % 26;
		convert2ColName(mode, res);
		convert2ColName(tempData, res);
	}
	else
	{
		res = (to26AlphabetString(data) + res);
	}
}

// brief 数字转换为26字母
// 1->A 26->Z
QString qtexcel::to26AlphabetString(int data)
{
	QChar ch = data + 0x40;//A对应0x41
	return QString(ch);
}
