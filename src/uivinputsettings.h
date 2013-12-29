#ifndef UIVINPUTSETTINGS_H
#define UIVINPUTSETTINGS_H
//
#include <QDialog>
#include <QStringListModel>
#include <QSize>
#include "ui_vinput_settings.h"
#include "VideoInput.h"

class UIFaceFinderWidget;

class UIVInputSettings : public QDialog, public Ui::visDialog
{
Q_OBJECT
public:
	UIVInputSettings( VideoInput *vi, 
	                  QWidget * parent = 0, Qt::WFlags f = 0 );
	void setFaceFinderWidget( UIFaceFinderWidget * ffw);
	void updateSettings();
private slots:
	void on_buttonAddPictures_clicked();
	void on_buttonClearList_clicked();
	void on_buttonFaceSet_clicked();
	void on_buttonNonfaceSet_clicked();
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
private:	
	UIFaceFinderWidget *faceFinderWidget;
	VideoInput *videoInput;	
	VideoInput::VideoSource videoSource;
	QStringListModel listFaces, listNonfaces;
	QSize setsSize;
	QString defaultFaceSetDir, defaultNonfaceSetDir;
	static QString vs2str( VideoInput::VideoSource); //convertion to string
};

inline void UIVInputSettings:: setFaceFinderWidget( UIFaceFinderWidget * ffw)
{
	faceFinderWidget = ffw;
}
#endif





