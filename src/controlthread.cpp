#include "controlthread.h"
#include "UIFaceFinderWidget.h"
//----------------------------------------------------------------------------//
ControlThread:: ControlThread( UIFaceFinderWidget *ffw,
                               VideoInput *vi, VideoScanner *vs,
                               FaceRecognizer *fr, FaceDatabase *fdb, 
                               QObject *parent)
	: QThread( parent)
{
	faceFinderWidget = ffw;
	videoInput = vi; 
	videoScanner = vs;
	faceRecognizer = fr;
	faceDatabase = fdb;
	cout << "[ControlThread] built.\n";
}
//----------------------------------------------------------------------------//
ControlThread:: ~ControlThread()
{
	delete computationThread;
	delete videoThread;
	emit exit();
	wait();
}
//----------------------------------------------------------------------------//
void ControlThread:: enableBackgroundThreads( bool enable)
{
	videoThread->enableComputations( enable);
	computationThread->enableComputations( enable);
}
//----------------------------------------------------------------------------//
void ControlThread:: run()
{	
	cout << "[ControlThread] run() on thread " 
	     << hex << QThread::currentThread() << dec << endl;
	     
	videoThread = new VideoThread( videoInput, 
	                               faceDatabase);
	computationThread = new ComputationThread( videoScanner, 
	                                           faceRecognizer);
	
		//threads data transfer 
	connect (computationThread, SIGNAL( requestNewData()),
	         videoThread,       SLOT(   askedForData  ()));
	connect (videoThread,       SIGNAL( rawFrame( const Matrix&)),
	         computationThread, SLOT(   loadData( const Matrix&)));
	connect( computationThread,   SIGNAL( scanRect(int, int, int, int)), 
	         videoThread,         SLOT(   updateScanArea(int, int, int, int)));
	connect( computationThread, SIGNAL( faceClassified( int)),
	         videoThread,       SLOT(   faceClassified( int)));

	//register type to send QImage between threads
	qRegisterMetaType<QImage>("QImage");
	
	/* Processed frame connection
	Sends picture frame currently processed by computation thread to 
	visualize it in video thread, action has maintance menaning, 
	turned off in release version
	connect( computationThread, SIGNAL( processedFrame( const QImage&)),
	         videoThread,       SLOT(   processedFrame( const QImage&)));
	*/
	
	//visualization data
	connect( videoThread, 
	          SIGNAL( newVideoFrame(  const QImage&)), 
	         faceFinderWidget->videoWidget, 
	          SLOT(  reloadVideoFrame( const QImage&)));
	connect( faceFinderWidget->actionFpsShow, 
	          SIGNAL( toggled( bool)),
	         videoThread,
	          SLOT(   showFps( bool)));
	connect( faceFinderWidget->actionScannerFrameShow, 
	          SIGNAL( toggled(   bool)),
	         videoThread,
	          SLOT(   showFrame( bool)));
	         
	//scanning control
	connect( faceFinderWidget,  SIGNAL( scannerLeft()),
	         computationThread, SLOT(   scannerLeft())); 
	connect( faceFinderWidget,  SIGNAL( scannerRigth()),
	         computationThread, SLOT(   scannerRigth())); 
	connect( faceFinderWidget,  SIGNAL( scannerUp()),
	         computationThread, SLOT(   scannerUp()));
	connect( faceFinderWidget,  SIGNAL( scannerDown()),
	         computationThread, SLOT(   scannerDown()));
	         
	videoThread->start();
	computationThread->start();
	exec();
}
//----------------------------------------------------------------------------//
