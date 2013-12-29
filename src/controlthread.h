//----------------------------------------------------------------------------//
//                          CONTROL THREAD CLASS                              //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 1.0                                                              //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  1.0 - controls information sharing between other threads                  //
//----------------------------------------------------------------------------//

#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

//includes
//----------------------------------------------------------------------------//
#include <QThread>
#include "videothread.h"
#include "computationthread.h"

class UIFaceFinderWidget;
class VideoInput;
class VideoScanner;
class FaceRecognizer;
class FaceDatabase;


/* thread is having event loop for signal handling between computation, video, 
and main (gui) threads */
//----------------------------------------------------------------------------//
class ControlThread : public QThread
{
Q_OBJECT
public:
	ControlThread ( UIFaceFinderWidget *ffw,
	                VideoInput *vi, VideoScanner *vs,
	                FaceRecognizer *fr, FaceDatabase *fdb,
	                QObject *parent = 0);
	~ControlThread();
	void enableBackgroundThreads( bool enable);
	const VideoThread* readVideoThread() const;
	const ComputationThread* readComputationThread() const;
	
protected:
	void run();
	
private:
	UIFaceFinderWidget *faceFinderWidget;	
	VideoThread *videoThread;
	ComputationThread *computationThread;
	VideoInput *videoInput;
	VideoScanner *videoScanner;
	FaceRecognizer *faceRecognizer;
	FaceDatabase *faceDatabase;
};
//----------------------------------------------------------------------------//
inline const VideoThread* ControlThread:: readVideoThread() const
{
	return videoThread;
}
//----------------------------------------------------------------------------//
inline const ComputationThread* ControlThread:: readComputationThread() const
{
	return computationThread;
}
//----------------------------------------------------------------------------//

#endif
