#include "CustomTableWidgetDelegate.h"
#include <QLineEdit>
#include <QIntValidator>
CustomTableWidgetDelegate::CustomTableWidgetDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	m_start_ = 0;
	m_end_ = 7;
}

CustomTableWidgetDelegate::CustomTableWidgetDelegate(uint start, uint end, QObject * parent):m_start_(start),m_end_(end), QStyledItemDelegate(parent)
{

}

CustomTableWidgetDelegate::~CustomTableWidgetDelegate()
{}

QWidget* CustomTableWidgetDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QLineEdit* editor = new QLineEdit(parent);
	QIntValidator* validator = new QIntValidator(m_start_, m_end_, editor);
	editor->setValidator(validator);
	return editor;
}
