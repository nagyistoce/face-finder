#include "UIFaceFinderWidget.h"
#include "uiabout.h"
#include "uiscannersettings.h"
#include "uivinputsettings.h"
#include "uirecognitionsettings.h"
#include "uidatabasesettings.h"
#include "controlthread.h"
#include "videothread.h"
#include <QKeyEvent>
//----------------------------------------------------------------------------//
UIFaceFinderWidget:: UIFaceFinderWidget( UIAbout *a, UIVInputSettings *vis, 
                                         UIScannerSettings *ss, 
                                         UIRecognitionSettings *rs,
                                         UIDatabaseSettings *ds,
                                         QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	//connection with engine and gui objects
	setupUi(this);
	uiAbout = a;
	uiVInputSettings = vis;
	uiScannerSettings = ss;
	uiRecognitionSettings = rs;
	uiDatabaseSettings = ds;
	
	//main menu states setting
	actionVideoFit->setChecked( videoWidget->fitVideo());	
}
//----------------------------------------------------------------------------//
void UIFaceFinderWidget:: setControlThread( ControlThread * ct)
{	
	controlThread = ct;
}
//----------------------------------------------------------------------------//
void UIFaceFinderWidget:: on_actionSetVideo_triggered()
{

	controlThread->enableBackgroundThreads( false);
	uiVInputSettings->updateSettings();
	uiVInputSettings->exec();
	controlThread->enableBackgroundThreads( true);
	videoWidget->resetCycles();
}
//----------------------------------------------------------------------------//
void UIFaceFinderWidget:: on_actionSetScanner_triggered()
{
	controlThread->enableBackgroundThreads( false);
	uiScannerSettings->updateSettings();
	uiScannerSettings->exec();
	controlThread->enableBackgroundThreads( true);
	videoWidget->resetCycles();
}
//----------------------------------------------------------------------------//
void UIFaceFinderWidget:: on_actionSetRecognizer_triggered()
{
	controlThread->enableBackgroundThreads( false);
	uiRecognitionSettings->updateSettings();
	uiRecognitionSettings->exec();
	controlThread->enableBackgroundThreads( true);
	videoWidget->resetCycles();
}
//----------------------------------------------------------------------------//
void UIFaceFinderWidget:: on_actionSetDatabase_triggered()
{
	controlThread->enableBackgroundThreads( false);
	uiDatabaseSettings->updateSettings();
	uiDatabaseSettings->exec();
	controlThread->enableBackgroundThreads( true);
	videoWidget->resetCycles();
}
//----------------------------------------------------------------------------//
void UIFaceFinderWidget:: keyPressEvent(QKeyEvent *event)
{	
	switch (event->key())
	{
		case Qt::Key_Up:
			emit scannerUp();
			break;
		case Qt::Key_Down:
			emit scannerDown();
			break;
		case Qt::Key_Left:
			emit scannerLeft();
			break;
		case Qt::Key_Right:
			emit scannerRigth();
			break;
		case Qt::Key_Escape:
			emit exit( EXIT_SUCCESS);
		default: /*Nothing*/;
	}
}
//----------------------------------------------------------------------------//
