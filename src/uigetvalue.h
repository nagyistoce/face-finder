#ifndef UIGETVALUE_H
#define UIGETVALUE_H
//
#include <QDialog>
#include "ui_get_value.h"
//
class UIGetValue : public QDialog, public Ui::gvDialog
{
Q_OBJECT
public:
	UIGetValue( QWidget * parent = 0, Qt::WFlags f = 0 );
	QString askForString( QString title, QString question, QString value);
	int     askForInt   ( QString title, QString question, int     value, 
	                      int min = 0, int max = 1000, int step = 1, 
	                      QString suffix = "");
	double  askForDouble( QString title, QString question, double  value,
	                      double min = -99.99, double max = 99.99,
	                      double step = 1.0, int decimals = 2,
	                      QString suffix = "");
};
#endif





