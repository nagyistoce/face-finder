#include "videothread.h"
#include "VideoInput.h"
#include <QTextCodec>
//----------------------------------------------------------------------------//
VideoThread::VideoThread( VideoInput* vi, FaceDatabase *fdb, QObject *parent) 
	: QThread( parent),
	  textEffects(this)
{
	videoInput = vi;
	faceDatabase = fdb;
	abort = false;
	stopComputation = false;
	needNewMatrixData_ = true;
	//rendering options
	faceRect_.setRect(10, 358, 92, 112);
	nameRect_.setRect(112, 434, 300, 36);
	pen_.setColor( QColor(213,28,9));
	pen_.setWidth(2);	
	font_.setFamily("Courier New");
	font_.setPointSize(9);
	dataPen_.setColor( QColor(14,56,140));
	dataPen_.setWidth(3);
	dataFont_.setFamily("Comic Sans");
	dataFont_.setPointSize(14);
	dataFont_.setBold(true);
	dataBrush_.setStyle(Qt::Dense4Pattern);
	dataBrush_.setColor(QColor(51,123,218));
	textEffects.setPainter( &painter_);
	textEffects.setRect( &nameRect_);
	textEffects.setShowTime( 2000);
	showFps_ = true;
	showFrame_ = true;
	showIndividual_ = false;
	//fps counting
	microCounter_.start();	
	std::cout << "[VideoThread] built.\n";
}
//----------------------------------------------------------------------------//
VideoThread:: ~VideoThread()
{
	mutex.lock();
	abort = true;
	condition.wakeOne();
	mutex.unlock();
	wait();	
}
//----------------------------------------------------------------------------//
void VideoThread:: enableComputations( bool enable)
{
	QMutexLocker locker( &mutex);
	stopComputation = !enable;
	if (stopComputation == false) //resume thread execution
		condition.wakeOne();
}
//----------------------------------------------------------------------------//
void VideoThread:: faceClassified( int id)
{	
	try
	{
		individual = faceDatabase->findId( id);
		showIndividual_ = true; 
		textEffects.loadText( QString( QTextCodec::codecForLocale()
				                       ->toUnicode( individual.name().c_str() )
				                      ) + " (id " +QString::number( id) + ")"
				            );
	}
	catch (exception& e) 
	{
		cout << "[VideoThread] (WW) " << e.what() << endl;
	}
}
//----------------------------------------------------------------------------//
void VideoThread:: askedForData()
{
	//Stop frame processing time
	microCounter_.stop();
	
	//VideoThread must process first data before this slot is called
	//which means it must be runed before threads that needs his data
	//the rest is handled by QMutexLocker synchronization
	QMutexLocker locker( &mutex);	
	
	//ensure that data will get to the destination unchanged
	//matrixData_ will be updated in next run() loop and we want to keep 
	//current frame unchanged (it will be implicitly shared by VideoScanner)
	dataToSend_.copy( matrixData_); 

	//send data 
	emit rawFrame( dataToSend_);

	//order new matrix data
	needNewMatrixData_ = true;
	
	//start new time period 
	microCounter_.start();	
}
//----------------------------------------------------------------------------//
void VideoThread:: run()
{
	bool abort, stopComputation;
	
	cout << "[VideoThread] run() on thread " 
	     << hex << QThread::currentThread() << dec << endl;
	     
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
//------VIDEO-UPDATING--------------------------------------------------------//
		try
		{
			videoInput->update();
			videoFrame_ = videoInput->toQImage();
			if (needNewMatrixData_)
			{
				matrixData_ = videoInput->toMatrix();
				needNewMatrixData_ = false;		
			}			
		}
		catch (exception &e)
		{
			//report to console
			cout << "[VideoThread] " << e.what() << endl;
		}
		mutex.unlock();
//-------RENDERING------------------------------------------------------------//		
		painter_.begin( &videoFrame_); 
		painter_.setRenderHint(QPainter::Antialiasing);
		painter_.setPen( pen_);
		painter_.setFont( font_);
		/* Processed frame visualization is turned off in release version
		mutex.lock();
		painter_.drawImage( faceRect_, processedFrame_);
		mutex.unlock();
		*/	
		if (showFrame_) painter_.drawRect( scanRect_);
		if (showFps_)
		{
			painter_.drawText( videoFrame_.rect(), 
			                   Qt::AlignTop + Qt::AlignRight, 
			                   "fps: " + QString::number( 
			                   microCounter_.eventFrequency(), 'f', 1));
		}
		if (showIndividual_)
		{
			painter_.setPen( dataPen_);
			painter_.setFont( dataFont_);
			painter_.drawImage( faceRect_, 
			                    QImage::fromData(
			                           (const uchar*)individual.imageAddress(), 
			                           individual.imageLength()));
			painter_.drawRect( faceRect_);
			painter_.fillRect( nameRect_, dataBrush_);
			showIndividual_ = textEffects.stepInAndOut();
		}	  
		painter_.end();
		emit newVideoFrame( videoFrame_);
		
	} //while (1)
	
}
//----------------------------------------------------------------------------//