//----------------------------------------------------------------------------//
//                            FACE FINDER WIDGET                              //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 1.00                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  1.00 - remake of faceFinder main window                     // 29.09 2009 // 
//----------------------------------------------------------------------------//

#ifndef UIFACEFINDERWIDGET_H
#define UIFACEFINDERWIDGET_H

//Includes
//----------------------------------------------------------------------------//
#include <QMainWindow>
#include "visualizationwidget.h"
#include "ui_FaceFinder.h"
#include "uivinputsettings.h" //needed by inline functions
#include "uiabout.h" //needed by inline functions

//Forward declarations
//----------------------------------------------------------------------------//
class UIScannerSettings;
class UIRecognitionSettings;
class UIDatabaseSettings;
class ControlThread;

//Main window
//----------------------------------------------------------------------------//
class UIFaceFinderWidget : public QMainWindow, public Ui::FaceFinderWidget
{
Q_OBJECT
public:
	UIFaceFinderWidget( UIAbout *a, UIVInputSettings *vis, 
	                    UIScannerSettings *ss, UIRecognitionSettings *rs,
	                    UIDatabaseSettings *ds, 
	                    QWidget * parent = 0, Qt::WFlags f = 0 );
	void setControlThread( ControlThread * ct);
signals:
	void scannerLeft();
	void scannerRigth();
	void scannerUp();
	void scannerDown();

protected:
	void keyPressEvent(QKeyEvent *event);
		
private slots:
	void on_actionExit_triggered();
	void on_actionVideoFit_triggered();
	void on_actionSetVideo_triggered();
	void on_actionSetScanner_triggered();
	void on_actionSetRecognizer_triggered();
	void on_actionSetDatabase_triggered();
	void on_actionAbout_triggered();
	
private:
	UIAbout *uiAbout;
	UIVInputSettings *uiVInputSettings;
	UIScannerSettings *uiScannerSettings;
	UIRecognitionSettings *uiRecognitionSettings;
 	UIDatabaseSettings *uiDatabaseSettings;
 	ControlThread *controlThread;
};
//----------------------------------------------------------------------------//

//Inlines
//----------------------------------------------------------------------------//
inline void UIFaceFinderWidget:: on_actionExit_triggered()
{
	close();
}
//----------------------------------------------------------------------------//
inline void UIFaceFinderWidget:: on_actionVideoFit_triggered()
{
	videoWidget->fitVideo( actionVideoFit->isChecked());
}
//----------------------------------------------------------------------------//
inline void UIFaceFinderWidget:: on_actionAbout_triggered()
{
	uiAbout->exec();
}
//----------------------------------------------------------------------------//

#endif
