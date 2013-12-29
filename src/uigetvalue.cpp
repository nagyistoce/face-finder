#include "uigetvalue.h"
//
UIGetValue::UIGetValue( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
}
//

QString UIGetValue:: askForString( QString title, QString question, 
                                   QString value)
{
	setWindowTitle( title);
	labelQuestion->setText( question);
	stackedWidget->setCurrentWidget( stringPage);
	lineEdit->setText( value);
	if (exec())
		return lineEdit->text();
	else
		return value;
}

int UIGetValue:: askForInt( QString title, QString question, int value,
                            int min, int max, int step, QString suffix)
{
	setWindowTitle( title);
	labelQuestion->setText( question);
	stackedWidget->setCurrentWidget( intPage);
	spinBox->setValue( value);
	spinBox->setMinimum( min);
	spinBox->setMaximum( max);
	spinBox->setSingleStep( step);
	spinBox->setSuffix( suffix);
	if (exec())
		return spinBox->value();
	else
		return value;
}
double UIGetValue:: askForDouble( QString title, QString question, 
                                  double value, double min, double max,
                                  double step, int decimals, QString suffix)
{
	setWindowTitle( title);
	labelQuestion->setText( question);
	stackedWidget->setCurrentWidget( doublePage);
	doubleSpinBox->setValue( value);
	doubleSpinBox->setMinimum( min);
	doubleSpinBox->setMaximum( max);
	doubleSpinBox->setSingleStep( step);
	doubleSpinBox->setDecimals( decimals);
	doubleSpinBox->setSuffix( suffix);
	if (exec())
		return doubleSpinBox->value();
	else
		return value;
}
