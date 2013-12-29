//engine
#include "VideoInput.h"
#include "VideoScanner.h"
#include "FaceRecognizer.h"
#include "FaceDatabase.h"
//working threads
#include "videothread.h"
#include "computationthread.h"
#include "controlthread.h"
//qt gui
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QSettings>
#include "UIFaceFinderWidget.h"
#include "uiabout.h"
#include "uigetvalue.h"
#include "uivinputsettings.h"
#include "uiscannersettings.h"
#include "uirecognitionsettings.h"
#include "uirecognizerlearning.h"
#include "uirecognizertest.h"
#include "qpicturelisteditor.h"
#include "uidatabasesettings.h"
#include "uiaddindividual.h"


//if camera doesn't work try run with:
//LD_PRELOAD=/usr/lib/libv4l/v4l2convert.so

int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	cout << "[FaceFinder2] main() thread = " 
	     << hex << QThread::currentThread() << dec << endl;
	srand( time(0));
	QPixmap pixmap(":/images/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	QSettings settings("face-finder.conf", QSettings::IniFormat);	
	//face regonition objects initialization
	splash.showMessage("Video Input ...", Qt::AlignBottom);
	app.processEvents();
	
	VideoInput videoInput( settings.value("VideoInput/width", 0).toInt(),
	                       settings.value("VideoInput/height", 0).toInt());
	videoInput.setVideoSource( VideoInput::stringToVideoSource(
		                       string( settings.value("VideoInput/source", "").toString().toLocal8Bit())));
	VideoScanner videoScanner;	
	videoScanner.setDefaultX( settings.value("VideoScanner/defaultX", 0).toInt() );
	videoScanner.setDefaultY( settings.value("VideoScanner/defaultY", 0).toInt() );
	videoScanner.setMinWidth( settings.value("VideoScanner/minWidth", 0).toInt() );
	videoScanner.setMinHeight( settings.value("VideoScanner/minHeight", 0).toInt() );
	videoScanner.setMaxWidth( settings.value("VideoScanner/maxWidth", 0).toInt() );
	videoScanner.setMaxHeight( settings.value("VideoScanner/maxHeight", 0).toInt() );
	videoScanner.setStep( settings.value("VideoScanner/step", 0).toInt() );
	videoScanner.setRatio( settings.value("VideoScanner/ratio", 0).toInt() );
	videoScanner.setImageSize( videoInput.width(), videoInput.height() );
	videoScanner.setScanningMode( VideoScanner::stringToScanningMode(
	                              string(settings.value("VideoScanner/mode", "").toString().toLocal8Bit())));
	cout << videoScanner.status() << endl;	
	splash.showMessage("Face Recognizer ...", Qt::AlignBottom);
	app.processEvents();	
	FaceRecognizer faceRecognizer( settings.value("FaceRecognizer/individuals", 0).toInt(),
	                               settings.value("FaceRecognizer/neuronsPerIndividual", 0).toInt(),
	                               settings.value("FaceRecognizer/eigenfaces_number", 0).toInt(),
	                               settings.value("FaceRecognizer/eigenfaces_width", 0).toInt(),
	                               settings.value("FaceRecognizer/eigenfaces_height", 0).toInt(), 
	                               settings.value("FaceRecognizer/eigenfaces_file", "").toString().toLocal8Bit(),
	                               settings.value("FaceRecognizer/faceAvg_file", "").toString().toLocal8Bit(),
	                               settings.value("FaceRecognizer/faceStd_file", "").toString().toLocal8Bit() );
	splash.showMessage("Face Database ...", Qt::AlignBottom);
	app.processEvents();
	FaceDatabase faceDatabase;
	try 
	{
		faceDatabase.load( settings.value("FaceDatabase/file", "").toString().toLocal8Bit());
		faceRecognizer.load( settings.value("FaceRecognizer/weights_file", "").toString().toLocal8Bit());		
	} 
	catch( Exception &e ) 
	{
		cout << "(WW) " << e.what() << endl;
	}
	splash.showMessage("Initialize GUI ...", Qt::AlignBottom);
	app.processEvents();
	UIAbout uiAbout;
	UIGetValue uiGetValue;
	UIVInputSettings uiVInputSettings (&videoInput);
	UIAddIndividual uiAddIndividual;
	QPictureListEditor pictureListEditor( &uiGetValue);
	pictureListEditor.setMaxId( pow( 2, faceRecognizer.layerGrossberg()) );
	UIRecognizerLearning uiRecognizerLearning( &videoInput,
	                                           &videoScanner,
	                                           &faceRecognizer,
	                                           &pictureListEditor);
	UIRecognizerTest uiRecognizerTest( &videoInput,
	                                   &videoScanner,
	                                   &faceRecognizer,
	                                   &pictureListEditor);
	UIRecognitionSettings uiRecognitionSettings( &faceRecognizer, 
	                                             &videoInput,
	                                             &uiGetValue,
	                                             &uiRecognizerLearning,
	                                             &uiRecognizerTest);
	app.connect( &uiRecognitionSettings, SIGNAL(recognizerOutputSize( int )),
	             &pictureListEditor,     SLOT(              setMaxId( int )));
	UIScannerSettings uiScannerSettings( &videoScanner);
	UIDatabaseSettings uiDatabaseSettings( &faceDatabase,
	                                       &uiGetValue,
	                                       &uiAddIndividual);
	UIFaceFinderWidget mainWindow( &uiAbout, 
	                               &uiVInputSettings, 
	                               &uiScannerSettings, 
	                               &uiRecognitionSettings,
	                               &uiDatabaseSettings
	                              );
	//set videoWidget initial size
	mainWindow.videoWidget->setMaximumWidth( videoInput.width());
	mainWindow.videoWidget->setMaximumHeight( videoInput.height());
	//connect video settings dialog with main window (videoWidget size updating)  
	uiVInputSettings.setFaceFinderWidget( &mainWindow);
	cout << "GUI created.\n";
	splash.showMessage("Background Threads ...", Qt::AlignBottom);
	app.processEvents();
	ControlThread controlThread( &mainWindow,
	                             &videoInput,
	                             &videoScanner,
	                             &faceRecognizer,
	                             &faceDatabase
	                            );
	mainWindow.setControlThread( &controlThread);
	//start threads
	controlThread.start();
	mainWindow.show();
	splash.finish( &mainWindow);
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	cout << "Initialization succeded.\n";
	int exitValue(0);
	try
	{
		exitValue = app.exec(); //starts event loop
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		exitValue = EXIT_FAILURE;
	}
	return exitValue;
}
