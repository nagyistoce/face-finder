#include "computationthread.h"
#include "VideoScanner.h"
#include "FaceRecognizer.h"
#include <QStringList>
//----------------------------------------------------------------------------//
ComputationThread::ComputationThread( VideoScanner *vs, FaceRecognizer *fr,
	                                  QObject *parent) 
	: QThread(parent)
{
	//getting external objects addresses
	videoScanner = vs;
	faceRecognizer = fr;
	//settings flags
	abort = false;
	stopComputation = false;
	std::cout << "[ComputationThread] built.\n";
}
//----------------------------------------------------------------------------//
ComputationThread::~ComputationThread()
{
     mutex.lock();
     abort = true;
     condition.wakeOne();
     mutex.unlock();
     wait();	
}
//----------------------------------------------------------------------------//
void ComputationThread:: enableComputations( bool enable)
{
	QMutexLocker locker( &mutex);
	stopComputation = !enable;
	if (stopComputation == false) //resume thread execution
	{
		if (videoScanner->scanningMode() == VideoScanner::SMFixed)
			emit scanRect( videoScanner->x1(), videoScanner->y1(), 
			               videoScanner->x2(), videoScanner->y2());
		else
			emit scanRect(0,0,0,0);
		condition.wakeOne();
	}
}
//----------------------------------------------------------------------------//
void ComputationThread:: scannerLeft()
{
	QMutexLocker locker( &mutex);
	if (videoScanner->scanningMode() != VideoScanner::SMFixed) return;	
	videoScanner->stepLeft();
	emit scanRect( videoScanner->x1(), videoScanner->y1(), 
	               videoScanner->x2(), videoScanner->y2());
}
//----------------------------------------------------------------------------//
void ComputationThread:: scannerRigth()
{
	QMutexLocker locker( &mutex);
	if (videoScanner->scanningMode() != VideoScanner::SMFixed) return;
	videoScanner->stepRight();
	emit scanRect( videoScanner->x1(), videoScanner->y1(), 
	               videoScanner->x2(), videoScanner->y2());
}
//----------------------------------------------------------------------------//
void ComputationThread:: scannerUp()
{
	QMutexLocker locker( &mutex);
	if (videoScanner->scanningMode() != VideoScanner::SMFixed) return;
	videoScanner->stepUp();
	emit scanRect( videoScanner->x1(), videoScanner->y1(), 
	               videoScanner->x2(), videoScanner->y2());
}
//----------------------------------------------------------------------------//
void ComputationThread:: scannerDown()
{
	QMutexLocker locker( &mutex);
	if (videoScanner->scanningMode() != VideoScanner::SMFixed) return;
	videoScanner->stepDown();
	emit scanRect( videoScanner->x1(), videoScanner->y1(), 
	               videoScanner->x2(), videoScanner->y2());
}
//----------------------------------------------------------------------------//
void ComputationThread:: loadData( const Matrix& data)
{
	QMutexLocker locker( &mutex);
	videoScanner->loadImage( data);
	//thread suppose to be waiting, wake it up
	condition.wakeOne();
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// RUN FUNCTION IS EXECUTED IN SEPERATE THREAD                                //
//----------------------------------------------------------------------------//
void ComputationThread::run()
{
	bool abort, stopComputation, scanned;
	int faceId, lastId = 0;	
	
	cout << "[ComputationThread] run() on thread " 
	     << hex << QThread::currentThread() << dec << endl;
	     
	//initial frame rect position
	emit scanRect( videoScanner->x1(),
	               videoScanner->y1(),
	               videoScanner->x2(),
	               videoScanner->y2() );
	//need to wait for first frame to operate on
	emit requestNewData();
	mutex.lock();
	condition.wait(&mutex);
	mutex.unlock();
	
	while (1) //forever 
    //exit this loop only when abort flag is set on by thread destructor
	{
		mutex.lock(); //lock to prevent asynchronous access to flags
		abort              = this->abort;
		stopComputation    = this->stopComputation;
		
//------THREAD-CONTROL--------------------------------------------------------//
		if (abort) return; //thread ending
		if (stopComputation) //thread temporarily stopping
			condition.wait(&mutex);

//----------------------------TRY-CATCH-SECTION-------------------------------//
		try //start exception vulnerable section
		{

//-------COMPUTATIONS---------------------------------------------------------//
		scanned = false;
		while (!scanned)
		{				
			faceRecognizer->setInput( videoScanner);
			faceId = faceRecognizer->classify();
			// 0 = unrecognized, otherwise send signal only once
			if (faceId && faceId != lastId) 
			{
				emit faceClassified( faceId);
				emit scanRect( videoScanner->x1(), videoScanner->y1(), 
				               videoScanner->x2(), videoScanner->y2());
			}
			lastId = faceId; //avoid signal multi-sending
			
			/* emiting of processed frame is turned off in release version
			frame_ = QImage( (const uchar*)
			                 faceRecognizer->currentImage().toUchar( 
			                                     faceRecognizer->imageWidth(), 
			                                     faceRecognizer->imageHeight(),
			                                     0, 1), 
			                 faceRecognizer->imageWidth(), 
			                 faceRecognizer->imageHeight(),
			                 faceRecognizer->imageWidth(),
			                 QImage::Format_Indexed8 );
			emit processedFrame( frame_);
			*/
								
			scanned = videoScanner->scan();			  
		}

		//ask VideoThread for new data to operate on
		emit requestNewData();
		condition.wait(&mutex); //wait until new data will arrive		
		} //try
		catch (exception &e)
		{
			//report to console
			cout << "[ComputationThread] " << e.what() << endl;
		}
		mutex.unlock();
//---------------------END-OF-TRY-CATCH-SECTION-------------------------------//
	} //while (1)
}
//----------------------------------------------------------------------------//
// END OF RUN FUNCTION                                                        //
//----------------------------------------------------------------------------//
