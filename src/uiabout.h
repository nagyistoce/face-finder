#ifndef UIABOUT_H
#define UIABOUT_H
//
#include <QDialog>
#include "ui_about.h"
//
class UIAbout : public QDialog, public Ui::aboutDialog
{
Q_OBJECT
public:
	UIAbout( QWidget * parent = 0, Qt::WFlags f = 0 );
private slots:
};
#endif





