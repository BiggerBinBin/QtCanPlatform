#pragma once

#include <QStyledItemDelegate>

class CustomTableWidgetDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	CustomTableWidgetDelegate(QObject *parent = nullptr);
	CustomTableWidgetDelegate(uint start,uint end,QObject *parent=nullptr);
	~CustomTableWidgetDelegate();
	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override;
private:
	uint m_start_;
	uint m_end_;
};
