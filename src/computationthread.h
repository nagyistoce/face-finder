//----------------------------------------------------------------------------//
//                      COMPUTATION THREAD CLASS                              //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 3.0                                                              //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  3.0 - video aquisition removed                                            //
//  2.0 - simplified and more stable version                                  //
//  1.0 - implementation of main alogritm of face recognition                 //
//----------------------------------------------------------------------------//

#ifndef COMPUTATION_THREAD_H
#define COMPUTATION_THREAD_H

//includes
//----------------------------------------------------------------------------//
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QRect>
#include <QString>
//#include <QImage> needed by processedFrame

class Matrix;
class VideoScanner;
class FaceRecognizer;

//Computation Thread class based on default QT thread
//----------------------------------------------------------------------------//
class ComputationThread : public QThread
{
Q_OBJECT
public:
	
	ComputationThread( VideoScanner *vs, FaceRecognizer *fr,
	                   QObject *parent = 0);
	~ComputationThread();
	
	void enableComputations( bool);
	
signals:
	void requestNewData();
	/* DISABLED: needed by processedFrame
	void processedFrame( const QImage&);
	*/
	void scanRect( int, int, int, int);
	void faceClassified( int);
	
public slots:
	void scannerLeft();
	void scannerRigth();
	void scannerUp();
	void scannerDown();
	void loadData( const Matrix&);
	
protected:
	void run();
	
private:
	QMutex mutex;
	QWaitCondition condition;
	bool abort; //abort execution of thread
	//computation flow control flags
	bool stopComputation; 
	//computation objects	
	VideoScanner *videoScanner;
	FaceRecognizer * faceRecognizer;
	//QImage frame_; needed by processedFrame
};
//----------------------------------------------------------------------------//

#endif
