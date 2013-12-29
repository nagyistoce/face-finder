#include "VideoInput.h"
//----------------------------------------------------------------------------//

//Video input
//----------------------------------------------------------------------------//
VideoInput:: VideoInput()
{
	width_ = 0;
	height_ = 0;
	#if OPENCV_SUPPORT
		cvCapture_ = 0;
		cvColorFrame_ = 0;
		cvGrayFrame_ = 0;
	#endif
	#if QT_SUPPORT
		for (int v, i = 0; i < 256; i++)
		{
			v = i | (i << 0x8) | (i << 0x10) | (0xFF << 0x18);
			qtColorTable_.push_back( v);
		}
	#endif
	matrixUpdated_ = false;
}
//----------------------------------------------------------------------------//
VideoInput:: VideoInput( const VideoInput& videoInput)
{
	width_ = videoInput.width_;
	height_ = videoInput.height_;
	#if OPENCV_SUPPORT
		cvCapture_ = 0;
		cvColorFrame_ = cvCreateImage( cvSize(width_, height_), 
		                               IPL_DEPTH_8U, 3);
		cvGrayFrame_ = cvCreateImage( cvSize(width_, height_), 
		                              IPL_DEPTH_8U, 1);
	#endif
	#if QT_SUPPORT
		for (int v, i = 0; i < 256; i++)
		{
			v = i | (i << 0x8) | (i << 0x10) | (0xFF << 0x18);
			qtColorTable_.push_back( v);
		}
	#endif
	matrixUpdated_ = false;
}
//----------------------------------------------------------------------------//
VideoInput:: VideoInput(int width, int height)
	: videoFrame_( height, width)
{
	width_ = width;
	height_ = height;
	#if OPENCV_SUPPORT
		cvCapture_ = 0;
		cvColorFrame_ = cvCreateImage( cvSize(width_, height_), 
		                               IPL_DEPTH_8U, 3);
		cvGrayFrame_ = cvCreateImage( cvSize(width_, height_), 
		                              IPL_DEPTH_8U, 1);
	#endif
	#if QT_SUPPORT
		for (int v, i = 0; i < 256; i++)
		{
			v = i | (i << 0x8) | (i << 0x10) | 0xFF000000 ;
			qtColorTable_.push_back( v);
		}
	#endif
	cout << "VideoInput " << width_ << "x" << height_ << " created.\n";
	matrixUpdated_ = false;
}
//----------------------------------------------------------------------------//
VideoInput:: ~VideoInput()
{
	#if OPENCV_SUPPORT
		if (cvColorFrame_)
			cvReleaseImage( &cvColorFrame_);
		if (cvGrayFrame_) 
			cvReleaseImage( &cvGrayFrame_);
		if (cvCapture_)
			cvReleaseCapture( &cvCapture_);
	#endif
}
//----------------------------------------------------------------------------//
void VideoInput:: setVideoSource( VideoSource vs)
{
	int w, h;
	
	switch (vs)	
	{
		case VSWebCamera:
			#if OPENCV_SUPPORT
				if (!cvCapture_)
					cvCapture_ = cvCaptureFromCAM( CV_CAP_ANY );
				w = cameraWidth();
				h = cameraHeight();
				if (w != width_ || h != height_)
				{
					#if CAN_RESIZE
						resize( w, h);
					#else
						throw SourceSize( w, h, width_, height_);
					#endif
				}
				break;
			#else
				throw NoOpencv();
			#endif
		case VSPictureSetRandom:
		case VSListRandom:
			fileIndex_ = -1;
			break;
		case VSPictureSetSequence:
		case VSListSequence:
			fileIndex_ = 0;
			break;
		case VSUndefined:
			vs = videoSource_;
		case VSNone:
			break;
		default: 
			throw InvalidSource( vs);
	}
	videoSource_ = vs;
}
//----------------------------------------------------------------------------//
void VideoInput:: resize(int width, int height)
{
	if(width_ == width && height_ == height) return;
	if( width  >= MIN_VIDEO_WIDTH && 
	    width  <= MAX_VIDEO_WIDTH &&
	    height >= MIN_VIDEO_HEIGHT &&
	    height <= MAX_VIDEO_HEIGHT )
	{
		width_ = width;
		height_ = height;
		videoFrame_.resize(height_, width_, 0, true);
		#if OPENCV_SUPPORT
		if (cvColorFrame_) 
			cvReleaseImage( &cvColorFrame_);
		cvColorFrame_ = cvCreateImage( cvSize(width_, height_), 
		                               IPL_DEPTH_8U, 3);
		if (cvGrayFrame_) 
			cvReleaseImage( &cvGrayFrame_);
		cvGrayFrame_ = cvCreateImage( cvSize(width_, height_), 
		                              IPL_DEPTH_8U, 1);
		#endif
	}
	else throw VideoConstraints( width, height);
}
//----------------------------------------------------------------------------//
void VideoInput:: update()
{
	switch (videoSource_)
		{
			case VSWebCamera:
				captureFromWebcam(); //this will be inlined
				break;
			case VSPictureSetRandom:
				fromSetsRandomly(); //this will be inlined
				break;
			case VSPictureSetSequence:
				fromSetsSequential(); //this will be inlined
				break;
			case VSListRandom:
				fromListRandom(); //this will be inlined
				break;
			case VSListSequence:
				fromListSequential(); //this will be inlined
				break;
			case VSNone:
			case VSUndefined:
			default:
				matrixUpdated_ = true;
		}
}
//----------------------------------------------------------------------------//
const Matrix& VideoInput:: toMatrix()
{
	if (matrixUpdated_ == false) //by now only camera capturing can cause this
	{
		/*no better option than full copy
		Matrix uses float precision, cvGrayFrame is 8bit precision and cannot 
		be created in the other way cause cvCvtColor requires same depth for
		source and destinanion, where source is retrieved directly from
		capture device. Besides Images are stored in row order and most of
		computation libraries preffered or even required column order, so data 
		copying can't be avoided. */
		videoFrame_.copyBuffer( cvGrayFrame_->imageData);
	}
	return videoFrame_;
}
//----------------------------------------------------------------------------//
string VideoInput:: cameraProperties() const
{
	stringstream msg;
	msg << "Currently active camera properties:" << endl;
	try
	{
		#if OPENCV_SUPPORT
			msg << "Video stream width:  "
			    << cameraWidth() << endl
			    << "Video stream height: "
			    << cameraHeight();
		#else
			throw NoOpencv();
		#endif
	}
	catch (exception &e)
	{
		msg << "Error: " << e.what();
	}
	return msg.str();
}
//----------------------------------------------------------------------------//

//QT relateted functions
#if QT_SUPPORT
//----------------------------------------------------------------------------//
const QImage& VideoInput:: toQImage()
{
#if OPENCV_SUPPORT
	if (videoSource_ == VSWebCamera)
		qtImage_ = QImage( (const uchar*) cvColorFrame_->imageData, 
		                    width_, height_, 
		                    cvColorFrame_->widthStep, QImage::Format_RGB888);
	else
#endif
	{
		qtImage_ = QImage( (const uchar*) videoFrame_.toUchar(), 
		                   width_, height_, 
		                   width_, QImage::Format_Indexed8);
		qtImage_.setColorTable( qtColorTable_);
	}
	return qtImage_;
}
//----------------------------------------------------------------------------//
void VideoInput:: loadFromQImage( const QImage& image)
{
	if( image.width() != width_ || image.height() != height_)
	{
		#if CAN_RESIZE
			resize( image.width(), image.height());
		#else
			throw SourceSize( image.width(), image.height(), width_, height_);
		#endif
	}
	qtImage_ = image;
	matrixUpdated_ = false;
}
//----------------------------------------------------------------------------//
#endif //QT_SPUPPORT

//Functions for PGM files 
//----------------------------------------------------------------------------//
void VideoInput:: loadPgmFile( const char* filePath)
{
	fstream f;
	string line;
	int w, h, val, max, i, j;
	int pgmFormat = 0 ; //0 - none/unknown, 1 - pm5,  2 - pm2
	f.open( filePath, ios::in | ios::binary);	
	if (!f) throw FileNotFound( filePath);
	//read header - check format
	line = readFromHeader( f);
	if (!pgmFormat && line.compare("P2") == 0)
		pgmFormat = 2;
	if (!pgmFormat && line.compare("P5") == 0)
		pgmFormat = 1;
	if (!pgmFormat)	throw PgmFormat( filePath);
	//picture dimension
	line = readFromHeader( f);
	w = str2int( line);
	line = readFromHeader( f);
	h = str2int( line);
	if (w != width_ || h != height_)
	{
		#if CAN_RESIZE
			resize( w, h);
		#else
			throw PgmSize( w, h, width_, height_);
		#endif
	}
	//maximal density
	line = readFromHeader( f);
	max = str2int( line);
	val = i = j = 0; //matrix indexes
	while (!f.eof()) 
	{
		switch (pgmFormat)
		{	
			case 1:
			val = pgm5(f);
			break;
			case 2:
			val = pgm2(f);
		}		
		if(val == -1) break;
		try 
		{
			videoFrame_.cell( i, j++) = (DataType)val / max;
		}
		catch( Exception &e)
		{
			cout << e.what() << endl;
		}		
		if(j == w) 
		{
			j = 0;
			i++;
			if (i == h) break;
		}
	}
	f.close(); 
	matrixUpdated_ = true;
}
//----------------------------------------------------------------------------//
int VideoInput:: str2int ( string s) //convert string to integer
{
	int i = 0;
	stringstream ss;
	ss << s; ss >> i;
	return i;
}
//----------------------------------------------------------------------------//
int VideoInput:: pgm2( fstream& f) //read one value from P2 formt (pgm)
{
	string s;
	if (f >> s) return str2int(s);
	else        return -1;
}
//----------------------------------------------------------------------------//
int VideoInput:: pgm5( fstream& f) //read one value from P5 format (ppm)
{
	if (f.eof()) return -1;
	else         return f.get();
}
//----------------------------------------------------------------------------//
string VideoInput:: readFromHeader(fstream& f) //reads values in header section
{
	string s;
	f >> s;
	while (s[0] == '#') //comment line
	{
		getline( f, s); //read the rest of comment including '\n'
		f >> s; //take next string after comment line
	}
	return s;
}
//----------------------------------------------------------------------------//
VideoInput:: VideoConstraints:: VideoConstraints( int w, int h)
{
	stringStream << "VideoInput:: Video size constraints volation. Minimal size"
	             << " is " << MIN_VIDEO_WIDTH << "x" << MIN_VIDEO_HEIGHT
	             << " and maximal is " << MAX_VIDEO_WIDTH << "x" 
	             << MAX_VIDEO_HEIGHT << " (Attempt to create " << w << "x" 
	             << h << ").";
}
//----------------------------------------------------------------------------//
VideoInput:: FileNotFound:: FileNotFound( const char* f)
{
	stringStream << "VideoInput:: loadPgmFile:: can't open " << f;
}
//----------------------------------------------------------------------------//
VideoInput:: PgmFormat:: PgmFormat( const char* f)
{
	stringStream << "VideoInput:: loadPgmFile:: unknown format, can't read from " << f;
}
//----------------------------------------------------------------------------//
VideoInput:: SourceSize:: SourceSize(int px, int py, int vx, int vy)
{
	stringStream << "VideoInput:: SourceSize:: source = [" << px << "x" << py
	             << "], videoInput = [" << vx << "x" << vy << "].";
}
//----------------------------------------------------------------------------//
VideoInput:: NoQt:: NoQt()
{
	stringStream << "VideoInput has been compiled without QT support." << endl
	             << "To add this functionality please set QT_SUPPORT macro in"
	             << "VideoInput.h header file to 1 and rebuild your project.";
}
//----------------------------------------------------------------------------//
VideoInput:: NoOpencv:: NoOpencv()
{
	stringStream << "VideoInput has been compiled without OpenCV support." 
	             << endl << "To add this functionality please set " 
	             << "OPENCV_SUPPORT macro in VideoInput.h header file to 1 "
	             << "and rebuild your project.";
}
//----------------------------------------------------------------------------//
VideoInput:: InvalidSource:: InvalidSource( VideoSource vs)
{
	stringStream << "VideoInput:: Attempt to set video source property to "
	             << "unknown value (" << (int) vs << ").";
}
//----------------------------------------------------------------------------//
VideoInput::VideoSource VideoInput:: stringToVideoSource( const string& s)
{
		if (s == "VSNone")
			return VSNone;
		if (s == "VSWebCamera") 
			return VSWebCamera; 
		if (s == "VSPictureSetRandom")
			return VSPictureSetRandom;
		if (s == "VSPictureSetSequence") 
			return VSPictureSetSequence;
		if (s == "VSListRandom") 
			return VSListRandom;
		if (s == "VSListSequence")
			return VSListSequence;
	return VSUndefined;
}
//----------------------------------------------------------------------------//
string VideoInput:: videoSourcetoString( VideoSource vs)
{
	switch (vs)
	{
		case VSNone : 
			return "VSNone";
		case VSWebCamera : 
			return "VSWebCamera"; 
		case VSPictureSetRandom : 
			return "VSPictureSetRandom";
		case VSPictureSetSequence : 
			return "VSPictureSetSequence";
		case VSListRandom : 
			return "VSListRandom";
		case VSListSequence : 
			return "VSListSequence";
		case VSUndefined:
		default:
			return "VSUndefined";
	}	
}
//----------------------------------------------------------------------------//
