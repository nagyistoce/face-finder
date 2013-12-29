#include "VideoScanner.h"
#include <sstream>
//----------------------------------------------------------------------------//
VideoScanner:: VideoScanner()
{
	x1_ = 0;
	y1_ = 0;
	x2_ = 0;
	y2_ = 0;
	minX_ = VIDEO_RECT_MIN_X;
	maxX_ = VIDEO_RECT_MAX_X;
	minY_ = VIDEO_RECT_MIN_Y;
	maxY_ = VIDEO_RECT_MAX_Y;
	defaultX_ = 0;
	defaultY_ = 0;
	width_ = 0;
	height_ = 0;
	minWidth_ = 0;
	minHeight_ = 0;
	maxWidth_ = 0;
	maxHeight_ = 0;
	step_ = 0;
	ratio_ = 0;
	scanned_ = true; //won't scan until imageArea loading occurs
	scanningMode_ = SMNone;
}
//----------------------------------------------------------------------------//
VideoScanner:: VideoScanner( const VideoScanner& videoScanner)
{
	x1_ = videoScanner.x1_;
	y1_ = videoScanner.y1_;
	x2_ = videoScanner.x2_;
	y2_ = videoScanner.y2_;
	minX_ = videoScanner.minX_;
	maxX_ = videoScanner.maxX_;
	minY_ = videoScanner.minY_;
	maxY_ = videoScanner.maxY_;
	defaultX_ = videoScanner.defaultX_;
	defaultY_ = videoScanner.defaultY_;
	width_ = videoScanner.width_;
	height_ = videoScanner.height_;
	minWidth_ = videoScanner.minWidth_;
	minHeight_ = videoScanner.minHeight_;
	maxWidth_ = videoScanner.maxWidth_;
	maxHeight_ = videoScanner.maxHeight_;
	step_ = videoScanner.step_;
	ratio_ = videoScanner.ratio_;
	scanned_ = true; //won't scan until imageArea loading occurs
	scanningMode_ = videoScanner.scanningMode_;
}
//----------------------------------------------------------------------------//
VideoScanner:: VideoScanner( int x1, int y1, int x2, int y2)
{
	minX_ = VIDEO_RECT_MIN_X;
	maxX_ = VIDEO_RECT_MAX_X;
	minY_ = VIDEO_RECT_MIN_Y;
	maxY_ = VIDEO_RECT_MAX_Y;
	defaultX_ = 0;
	defaultY_ = 0;
	if( x1 >= minX_ && x2 <= maxX_ && x1 < x2 &&
	    y1 >= minY_ && y2 <= maxY_ && y1 < y2)
	{
		x1_ = x1;
		y1_ = y1;
		x2_ = x2;
		y2_ = y2;
		width_ = x2 - x1;
		height_ = y2 - y1;
	}
	else throw BadCoords( x1, y1, x2, y2);
	minWidth_ = 0;
	minHeight_ = 0;
	maxWidth_ = 0;
	maxHeight_ = 0;
	step_ = 0;
	ratio_ = 0;
	scanned_ = true; //won't scan until imageArea loading occurs
	scanningMode_ = SMNone;	
}
//----------------------------------------------------------------------------//
VideoScanner:: ~VideoScanner()
{
	//nothing to do
}
//----------------------------------------------------------------------------//
string VideoScanner:: status() const
{
	stringstream ss;
	ss << "[VideoScanner] scan size from [" << minWidth_ << ","
	   << minHeight_ << "] to [" << maxWidth_ << "," << maxHeight_
	   << "] with step/ratio [" << step_ << "," <<	ratio_
	   << "] in " << scanningModeToString( scanningMode_) << " mode.";
	return ss.str();
}
//----------------------------------------------------------------------------//
void VideoScanner:: stepLeft()
{
	x1_ = x1_ - step_ >= minX_ ? x1_ - step_ : minX_;
	x2_ = x1_ + width_;
}
//----------------------------------------------------------------------------//
void VideoScanner:: stepRight()
{
	x2_ = x2_ + step_ <= maxX_ ? x2_ + step_ : maxX_;
	x1_ = x2_ - width_;
}
//----------------------------------------------------------------------------//
void VideoScanner:: stepUp()
{
	y1_ = y1_ - step_ >= minY_ ? y1_ - step_ : minY_;
	y2_ = y1_ + height_;
}
//----------------------------------------------------------------------------//
void VideoScanner:: stepDown()
{
	y2_ = y2_ + step_ <= maxY_ ? y2_ + step_ : maxY_;
	y1_ = y2_ - height_;
}
//----------------------------------------------------------------------------//
void VideoScanner:: loadImage(const Matrix& image)
{
	//image object can be modified later
	imageArea_ = image; 	
	scanned_ = scanningMode_ & SMPosition ? false : true;
	scannedArea_ = imageArea_.subMatrix(y1_, y2_ - 1, x1_,  x2_ - 1);
}
//----------------------------------------------------------------------------//
void VideoScanner:: random( int width, int height)
{
	width_  = width;
	height_ = height;
	x1_ = ((float) rand() / RAND_MAX * (maxX_ / width_  - 1)) * width_;
	y1_ = ((float) rand() / RAND_MAX * (maxY_ / height_ - 1)) * height_;
	x2_ = x1_ + width_;
	y2_ = y1_ + height_;
}
//----------------------------------------------------------------------------//
bool VideoScanner::  scan()
{
	if (!scanned_)
	{
		scannedArea_ = imageArea_.subMatrix(y1_, y2_ - 1, x1_, x2_ - 1);
		//scanning current area
		if (scanningMode_ & SMPosition) move();	
		//changing scale of scanning (smaller area - larger distance of objects)
		if (scanningMode_ & SMDistance && scanned_)	scale();			
	}
	return scanned_;
}
//----------------------------------------------------------------------------//
void VideoScanner:: setScanningMode( ScanningMode scanningMode)
{
	cout << "[VideoScanner] setting mode " << scanningMode << endl;
	scanningMode_ = scanningMode;
	maxArea(); //ensure area is max area
	if (scanningMode_ == SMFixed) center();	
	if (scanningMode_ & SMPosition) reset();
	if (scanningMode == SMFullImage) setCoords( 0, 0, imageWidth_, imageHeight_);
}
//----------------------------------------------------------------------------//

//Exception for VideoScanner
//----------------------------------------------------------------------------//
VideoScanner:: BadCoords:: BadCoords(int x1, int y1, int x2, int y2)
{
	stringStream << "Attepmt to set VideoScanner coordinates to ("
	             << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ").";
}
//----------------------------------------------------------------------------//
VideoScanner::ScanningMode VideoScanner:: stringToScanningMode( const string& s)
{
	 if (s == "SMFixed")
	 	return SMFixed; 
	if ( s == "SMPosition")
		return SMPosition;
	if (s == "SMDistance")
		return SMDistance;
	if (s == "SMFull")
		return SMFull;
	if (s == "SMRandom")
		return SMRandom;
	if (s == "SMFullImage")
		return SMFullImage;
	return SMNone;
}
//----------------------------------------------------------------------------//
string VideoScanner:: scanningModeToString( ScanningMode sm)
{
	switch (sm)
	{
		case SMFixed:
	 		return "SMFixed"; 
		case SMPosition:
			return "SMPosition";
		case SMDistance:
			return "SMDistance";
		case SMFull:
			return "SMFull";
		case SMRandom:
			return "SMRandom";
		case SMFullImage:
			return "SMFullImage";
		case SMNone:
		default:
			return "SMNone";
	}
}
//----------------------------------------------------------------------------//