//----------------------------------------------------------------------------//
//                             VIDEO INPUT CLASS                              //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 4.00                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  4.00 - scanning routines moved to other class               // xx.10.2009 //
//  3.42 - data copying optimalization                          // 30.09.2009 //
//  3.10 - webcam support through opencv                        // 12.05.2009 //
//	3.00 - video scanning routines                              // 11.05.2009 // 
//  2.05 - QT support function                                  // 01.03.2009 //
//  2.00 - matrix driven version                                // 21.11.2008 //
//  1.00 - template version, with buffer for image              // 07.08.2008 //
//----------------------------------------------------------------------------//

#ifndef VIDEO_INPUT_H
#define VIDEO_INPUT_H

#define ON 1
#define OFF 0

//Qt support
//----------------------------------------------------------------------------//
//If you want to use qt-related functions for this class (e.g. copy from QImage) 
//set this macro to 1, otherwise it should be 0
//This need QT to be installed and libraries added to project.  
#define QT_SUPPORT ON

//OpenCV support
//----------------------------------------------------------------------------//
//If you have OpenCV installed you can turn this on (set to 1) to get 
//extra functionality e.g. capturing from camera
//Important! You must add opencv libraries to your project
//(see OpenCV wiki site for details)  
#define OPENCV_SUPPORT ON

//Behaviors
//----------------------------------------------------------------------------//
//determine if video input can be dynamicly resized
#define CAN_RESIZE ON

//Includes
//----------------------------------------------------------------------------//
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;
#include "matrix.h"
#if QT_SUPPORT
	#include <QImage>
	#include <QVector>
#endif
#if OPENCV_SUPPORT
	#include "cv.h"
	#include "highgui.h"
#endif

//Video resolution constraints
//----------------------------------------------------------------------------//
#define MIN_VIDEO_WIDTH 32
#define MAX_VIDEO_WIDTH 800
#define MIN_VIDEO_HEIGHT 32
#define MAX_VIDEO_HEIGHT 600


//! Video Input class
/*! Class provides uniform interface for several video data sources such as camera 
(trough OpenCV) or graphic files on disc. It can transform data to Matrix format 
so they can be further used by e.g NeuralNetwork based class. */
//----------------------------------------------------------------------------//
class VideoInput
{
	public:
	
		/*! Defines which data source should be use for video image aquisition.
		Possible values are:\n
		VSUndefined\t\t video source hasn't been defined properly\n
		VSNone\t\t use no input at all\n
		VSWebCamera\t\t get image from camera\n
		VSPictureSetRandom\t\t get image from picture containing set of smaller pictures. One small picture is picked randomly each time.\n
		VSPictureSetSequence\t\tget image from picture containing set of smaller pictures. One small picture is picked each time from first to last.\n
		VSListRandom\t\tget image from pgm file randomly picked from list\n
		VSListSequence\t\tget image from pgm file picked from list from first to last.\n
		For source depending on graphic files proper list must be set by user.
		*/
		enum VideoSource { VSUndefined, VSNone, VSWebCamera, VSPictureSetRandom, 
		                   VSPictureSetSequence, VSListRandom, VSListSequence};
		/*! Converts given std::string to VideoSource value */
		static VideoSource stringToVideoSource( const string&);
		/*! Convert given VideoSource value to std::string */
		static string videoSourcetoString( VideoSource);
		VideoInput();
		VideoInput( const VideoInput&);
		/*! Creates VideoInput object of given width and height. */
		VideoInput(int width, int height);
		~VideoInput();
		
		//properties
		int width() const;
		int height() const;		
		VideoSource videoSource() const;
		void setVideoSource( VideoSource);
		/*! Sets list of files containg pictures set. Syntax is obsolete.*/
		void setPictureSets( const vector<string>&, const vector<string>&);
		/*! When video source is set to VSList[Random|Sequence] list of files must be provide as vector of strings containings path to each picture. */
		void setFilesList( const vector<string>&);
		/*! When file list is used returns actually picked picture index in list. otherwise returned value is meaningless. */
		int fileIndex() const;
		
		//actions
		/*! Resize input to given width and height. Macro CAN_RESIZE must be set to ON before compilation (this is default setting). */
		void resize(int width, int height);
		/*! Clears current input image by filling it with 0 value (black image). */
		void clear();
		/*! Aquire new image from choosed video source. */
		void update();
		
		// fast access functions		
		/*! Read value of pixel at given position. Values has Matrix DataType in range [0,1]. (Obsolete function)*/
		DataType get(int x, int y);	
		/*! Sets one pixel in image (Obsolete)*/
		void     set(int x, int y, DataType value);

		// image functions
		/*! Fills image with random values N(0,1) - "white noise".*/
		void makeNoise(); 
		/*! Loads pgm file from given file path. */
		void loadPgmFile( const char* filePath);
		/*! Captures frame from camera. */
		void captureFromWebcam();
		/*! Picks image from pictures set, randomly.*/
		void fromSetsRandomly();
		/*! Picks image from pictures set, from first to last.*/
		void fromSetsSequential();
		/*! Picks image from pictures list, randomly.*/
		void fromListRandom();
		/*! Picks image from pictures list, from first to last.*/
		void fromListSequential();
		/*! Return basic description of camera properties (fram size at the moment).*/
		string cameraProperties() const;

		#if QT_SUPPORT
			/*! Converts current input image to QImage format. */
			const QImage& toQImage();
			/*! Color table required to present grayscale image in QImage Index8 format except for rgb format. */
			QVector<QRgb>& colorTable();
			/*! Loads image form given QImage object. */
			void loadFromQImage( const QImage&);
		#endif
		
		#if OPENCV_SUPPORT
			double cameraWidth() const;
			double cameraHeight() const;
			/*! When use OpenCV image is not automaticly stored in matrix object. This function must be first called. */
			const Matrix& toMatrix();
		#endif
		
	protected:
	
		Matrix videoFrame_;
		int width_, height_;
		VideoSource videoSource_;
		/*! Indicates if VideoInput matrix object is up-to-date. 
		Video can be aquired by OpenCV structues and updating is needed. */
		bool matrixUpdated_;
		std::vector<string> positiveSet_, negativeSet_, fileList_;
		int fileIndex_;
		
		#if QT_SUPPORT
			QImage qtImage_;
			QVector<QRgb> qtColorTable_;
		#endif
		
		#if OPENCV_SUPPORT
			CvCapture* cvCapture_;
			IplImage* cvRawFrame_; // Frame grabbed from capture device
			IplImage* cvColorFrame_; // Image in 24bit rgb format
			IplImage* cvGrayFrame_; // 8bit grayscale format
		#endif
	
	private:
		//! VideoInput exception
		/*! This exception is thrown when try to set VideoInput size 
		too high or too low.*/
		class VideoConstraints : public Exception 
			{ public: VideoConstraints(int, int); };
		//! VideoInput exception
		/*! This exception is thrown when try to load file but couldn't open it. */
		class FileNotFound : public Exception 
			{ public: FileNotFound(const char*); };
		//! VideoInput exception
		/*! This exception is thrown when encounter bad header in pgm file. */
		class PgmFormat : public Exception { public: PgmFormat(const char*); };
		//! VideoInput exception
		/*! This exception is thrown when size of image from current source does not match size of VideoInput object, and CAN_RESIZE was set to OFF. */
		class SourceSize : public Exception 
			{ public: SourceSize(int, int, int, int); };
		//! VideoInput exception
		/*! This exception is thrown when call function that requires Qt libraries but macro QT_SUPPORT was set to OFF. */
		class NoQt : public Exception { public: NoQt(); };
		//! VideoInput exception
		/*! This exception is thrown when call function that use OpenCV library but OPENCV_SUPPORT was set to OFF.*/
		class NoOpencv : public Exception {	public: NoOpencv(); };
		//! VideoInput exception
		/*! This exception is thrown when VideoSource was set to unknown value. */
		class InvalidSource : public Exception 
			{ public: InvalidSource( VideoSource); }; 

		static int str2int ( string s);
		static int pgm2( fstream& f);
		static int pgm5( fstream& f);
		static string readFromHeader(fstream& f);

};
//----------------------------------------------------------------------------//

//VideoInput inlines
//----------------------------------------------------------------------------//
inline int VideoInput:: width() const
{
	return width_; 
}
//----------------------------------------------------------------------------//
inline int VideoInput:: height() const
{
	return height_;
}
//----------------------------------------------------------------------------//
inline VideoInput::VideoSource VideoInput:: videoSource() const
{
	return videoSource_;
}
//----------------------------------------------------------------------------//
inline void VideoInput:: setPictureSets( const vector<string>& positive, 
                                  const vector<string>& negative)
{
	positiveSet_ = positive;
	negativeSet_ = negative;
	fileIndex_ = negativeSet_.size();
}
//----------------------------------------------------------------------------//
inline void VideoInput:: setFilesList( const vector<string>& files)
{
	fileList_ = files;
	fileIndex_ = 0;
}
//----------------------------------------------------------------------------//
inline int VideoInput:: fileIndex() const
{
	return fileIndex_;
}
//----------------------------------------------------------------------------//
inline DataType VideoInput:: get(int x, int y)
{
	return videoFrame_.cell(y, x);
}
//----------------------------------------------------------------------------//
inline void VideoInput:: set(int x, int y, DataType value)
{
	videoFrame_.cell( y, x) = value;
}
//----------------------------------------------------------------------------//
inline void VideoInput:: clear()
{
	videoFrame_.set(0);
}
//----------------------------------------------------------------------------//
inline void VideoInput:: makeNoise()
{
	videoFrame_.setRandom(0, 1);
}
//----------------------------------------------------------------------------//
inline double VideoInput:: cameraWidth() const
{
	return cvGetCaptureProperty( cvCapture_, CV_CAP_PROP_FRAME_WIDTH);
}
//----------------------------------------------------------------------------//
inline double VideoInput:: cameraHeight() const
{
	return cvGetCaptureProperty( cvCapture_, CV_CAP_PROP_FRAME_HEIGHT);
}
//----------------------------------------------------------------------------//
			
//Following functions are inline because they 
//should be use only in one place in project
//----------------------------------------------------------------------------//
inline void VideoInput:: captureFromWebcam()
{
	#if OPENCV_SUPPORT
		cvRawFrame_ = cvQueryFrame( cvCapture_);
		cvCvtColor( cvRawFrame_, cvGrayFrame_, CV_RGB2GRAY);
		cvCvtColor( cvRawFrame_, cvColorFrame_, CV_RGB2BGR);
		matrixUpdated_ = false;
	#else
		throw NoOpencv();
	#endif
}
//----------------------------------------------------------------------------//
#if QT_SUPPORT
inline QVector<QRgb>& VideoInput:: colorTable()
{
	return qtColorTable_;
}
#endif
//----------------------------------------------------------------------------//
inline void VideoInput:: fromSetsRandomly()
{
	if (rand() > RAND_MAX / 2) //positive set
	{
		 fileIndex_ = (float)rand() / RAND_MAX * (positiveSet_.size());
		 loadPgmFile( positiveSet_[fileIndex_].c_str());
	}
	else //negative set
	{
		fileIndex_ = (float)rand() / RAND_MAX * (negativeSet_.size());
		loadPgmFile( negativeSet_[fileIndex_].c_str());
	} 
}
//----------------------------------------------------------------------------//
inline void VideoInput:: fromSetsSequential()
{	
	if (++fileIndex_ >= (signed) positiveSet_.size()) 
		fileIndex_ = -negativeSet_.size();
	if (fileIndex_ >= 0)
		loadPgmFile( positiveSet_[fileIndex_].c_str());
	else
		loadPgmFile( negativeSet_[-fileIndex_].c_str());	
}
//----------------------------------------------------------------------------//
inline void VideoInput:: fromListRandom()
{
	fileIndex_ = (float)rand() / RAND_MAX * (fileList_.size());
	loadPgmFile( fileList_[fileIndex_].c_str());
}
//----------------------------------------------------------------------------//
inline void VideoInput:: fromListSequential()
{
	if (++fileIndex_ >= (signed) fileList_.size()) fileIndex_ = 0;
	loadPgmFile( fileList_[fileIndex_].c_str());
}
//----------------------------------------------------------------------------//

#endif //VIDEO_INPUT_H
