#ifndef UISCANNERSETTINGS_H
#define UISCANNERSETTINGS_H
//
#include <QDialog>
#include "ui_scanner_settings.h"
//
class VideoScanner;

class UIScannerSettings : public QDialog, public Ui::scannerDialog
{
Q_OBJECT
public:
	UIScannerSettings( VideoScanner *vi,
	                   QWidget * parent = 0, Qt::WFlags f = 0 );
	void updateSettings();
private slots:
	void accept();
	void reject();
private:
	VideoScanner * videoScanner;
};
#endif





