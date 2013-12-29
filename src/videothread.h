//----------------------------------------------------------------------------//
//                          VIDEO THREAD CLASS                                //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 1.0                                                              //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  1.0 - frame aquisition an rendering                                       //
//----------------------------------------------------------------------------//

#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

//includes
//----------------------------------------------------------------------------//
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QImage>
#include <QPainter>
#include <QRect>
#include <QPen>
#include <QFont>
#include <QBrush>
#include "MicroCounter.h"
#include "FaceDatabase.h"
#include "VideoInput.h" //cant be forwarde cause of inlines
#include "texteffects.h"

class Matrix;

// video input reading and main window videoWidget frame rendering
//----------------------------------------------------------------------------//
class VideoThread : public QThread
{
Q_OBJECT
public:
	VideoThread (VideoInput* vi, FaceDatabase *fdb, QObject *parent = 0);
	~VideoThread();
	bool showFps() const;
	bool showFrame() const;
	void enableComputations( bool enable);
signals:
	void rawFrame(const Matrix&);
	void newVideoFrame(const QImage&);	
public slots:
	void askedForData();
	void showFps(bool on);
	void showFrame(bool on);
	/* Processed frame visualization is turned off in release version
	void processedFrame( const QImage&);
	*/
	void updateScanArea( int, int, int, int);
	void faceClassified( int);
protected:
	void run();
private:	
	//thread control
	QMutex mutex;
	QWaitCondition condition;
	bool abort, stopComputation;
	//video aquisition
	VideoInput *videoInput; 
	Matrix matrixData_, dataToSend_;
	bool needNewMatrixData_;
	//rendering objects
	bool showFps_, showFrame_, showIndividual_;
	MicroCounter microCounter_;
	FaceDatabase * faceDatabase;
	Individual individual;
	QImage videoFrame_; //, processedFrame_;
	QPainter painter_;
	QPen pen_, dataPen_;
	QFont font_, dataFont_;
	QBrush dataBrush_;
	QRect scanRect_, faceRect_, nameRect_;
	TextEffects textEffects;
};
//----------------------------------------------------------------------------//
/* Processed frame visualization is turned off in release version
inline void VideoThread:: processedFrame( const QImage& frame)
{
	QMutexLocker locker( &mutex);
	processedFrame_ = frame;	
	processedFrame_.setColorTable( videoInput->colorTable());
}*/
//----------------------------------------------------------------------------//
inline void VideoThread:: updateScanArea(int x1, int y1, int x2, int y2)
{
	QMutexLocker locker( &mutex);
	scanRect_.setCoords( x1, y1, x2, y2);	
}
//----------------------------------------------------------------------------//
inline bool VideoThread:: showFps() const
{
	return showFps_;
}
//----------------------------------------------------------------------------//
inline bool VideoThread:: showFrame() const
{
	return showFrame_;
}
//----------------------------------------------------------------------------//
inline void VideoThread:: showFps(bool on)
{
	QMutexLocker locker( &mutex);
	showFps_ = on;
}
//----------------------------------------------------------------------------//
inline void VideoThread:: showFrame(bool on)
{
	QMutexLocker locker( &mutex);
	showFrame_ = on;
}
//----------------------------------------------------------------------------//

#endif
