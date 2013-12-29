//----------------------------------------------------------------------------//
//                           VIDEO SCANNER CLASS                              //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 1.00                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  1.00 - scanning operations transfered from VideoInput       // xx.10.2009 //
//----------------------------------------------------------------------------//

#ifndef VIDEO_SCANNER_H
#define VIDEO_SCANNER_H

//Includes
//----------------------------------------------------------------------------//
#include "matrix.h"

//Video scanning constraints
//----------------------------------------------------------------------------//
#define VIDEO_RECT_MIN_X 0
#define VIDEO_RECT_MAX_X 640
#define VIDEO_RECT_MIN_Y 0
#define VIDEO_RECT_MAX_Y 480

//! frame scanning definitions
/*! Class provides interfaces for scanning image partially. User can choose which are exactly should be return for processing. Area can be variable in size and position. */
//----------------------------------------------------------------------------//
class VideoScanner
{
	public:
		/*! Determine the way that image is scanned,
		Availble modes:\n
		SMNone\t\t\nSMFixed\t\tReturn area of fixed size and position in image to further processing.\n 
		SMPosition\t\t Returned area changes its positon to scan whole image. Each time moving by step value.\n
		SMDistance\t\t Returned area has variable size to better fits scanning to objects at diffrent distances. (This is fully experimental mode)\n
		SMFull\t\t This mode is simple connection of SMPosition and SMDistance\n
		SMRandom\t\t Assuming image consist of grid of smaller images each time randomly chosed small image area is returned.\n
		SMFullImage\t\tReturn all available area of image to process.\n
		*/
		enum ScanningMode { SMNone = 0, SMFixed = 1, 
		                    SMPosition = 2, SMDistance = 4, SMFull = 6,
		                    SMRandom = 8, SMFullImage = 16};
		/*! Converts string to ScanningMode value. */
	    static ScanningMode stringToScanningMode( const string&);
		/*! Converts ScanningMode value to string. */
	    static string scanningModeToString( ScanningMode);
	    
		VideoScanner();
		VideoScanner( const VideoScanner&);
		
		/*! Creates scanner object with active area set to given coordinates of image. */
		VideoScanner( int x1, int y1, int x2, int y2);
		~VideoScanner();
		
		/*! Return short string description of scanner operating mode. */
		string status() const;
		
		/*! Scanned area coordinate: left border position of scanning rectangular*/
		int x1() const;
		
		/*! Scanned area coordinate: right border position of scanning rectangular*/
		int x2() const;
		
		/*! Scanned area coordinate: top border position of scanning rectangular*/
		int y1() const;
		
		/*! Scanned area coordinate: bottom border position of scanning rectangular*/
		int y2() const;
		
		/*!\sa minX_*/
		int minX() const;
		
		/*!\sa maxX_*/
		int maxX() const;
		
		/*!\sa minY_*/
		int minY() const;		
		
		/*!\sa maxY_*/
		int maxY() const;
		
		/*!\sa defaultX_*/
		int defaultX() const;
		
		/*!\sa defaultY_*/
		int defaultY() const;
		
		/*! Scanned area current width. */
		int width() const;
		
		/*! Scanned area current heigth. */
		int height() const;		
		
		/*!\sa minWidth_*/
		int minWidth() const;
		
		/*!\sa maxWidth_*/
		int maxWidth() const;
		
		/*!\sa minHeight_*/
		int minHeight() const;
		
		/*!\sa maxHeight_*/
		int maxHeight() const;
		
		/*!\sa step_*/
		int step() const;
		
		/*!\sa ratio_*/
		int ratio() const;
		
		/*! Flag indicating if current image has been fully scanned. */
		bool scanned() const;
		
		/*!\sa scanningMode_*/
		ScanningMode scanningMode() const;
		
		/*!\sa minX_*/
		void setMinX( int x);
		
		/*!\sa maxX_*/
		void setMaxX( int x);
		
		/*!\sa minY_*/
		void setMinY( int y);
		
		/*!\sa maxY_*/
		void setMaxY( int y);
		
		/*!\sa defaultX_*/
		void setDefaultX( int x);
		
		/*!\sa defaultY_*/
		void setDefaultY( int y);
		
		/*!\sa minWidth_*/
		void setMinWidth(int minWidth);
		
		/*!\sa maxWidth_*/
		void setMaxWidth(int maxWidth);
		
		/*!\sa minHeight_*/
		void setMinHeight(int minHeight);
		
		/*!\sa maxHeight_*/
		void setMaxHeight(int maxHeight);
		
		/*!\sa step_*/
		void setStep(int step);
		
		/*!\sa ratio_*/
		void setRatio(int ratio);
		
		/*! Sets input image size to given width and height. */
		void setImageSize( int width, int height);
		
		/*!\sa scanningMode_*/
		void setScanningMode( ScanningMode);
		
		/*! Moves scanning area one step left. */
		void stepLeft();
		
		/*! Moves scanning area one step right. */
		void stepRight();
		
		/*! Moves scanning area one step up. */
		void stepUp();
		
		/*! Moves scanning area one step down. */
		void stepDown();
		
		/*! Loads image from givem Matrix to perform scanning on it. */
		void loadImage(const Matrix& image);
		
		/*! Set scanning rect at random position. */
		void random( int width, int height);
			
		/*! Sets scanning area to new position or size and return state of scanned flag. */
		bool scan(); //return true if image is fully scanned				
		/*! Transforms scanned area to given matrix shape.*/
		Matrix& scanAreaToMatrix(Matrix& m);
		
	protected:
	
		void move();
		void reset(); //moves to top-left corner
		void center(); //center at given point (defultX_, defaultY_)
		void scale(); //scales with ratio
		void maxArea(); //set to max area (maxWidth and maxHeight)
		void setCoords( int x1, int y1, int x2, int y2);
		
		/*! VideoScanner object current scanning mode value. 
		\sa scanningMode, setScanningMode */
		ScanningMode scanningMode_;	
		
		Matrix imageArea_,
		       scannedArea_;
		
		int x1_, y1_, x2_, y2_,
		    width_, height_,
			
			/*! Scanned area minimum position in width dimension.
			\sa minX, setMinX */
		    minX_,
			
			/*! Scanned area minimum position in height dimension.
			\sa minY, setMinY */
			minY_,
			
			/*! Scanned area maximum position in width dimension.
			\sa maxX, setMaxX */
			maxX_,
			
			/*! Scanned area maximum position in height dimension.
			\sa maxY, setMaxY */
			maxY_,
			
			/*! Default center of scanned area in width dimension. 
			\sa defaultX, setDefaultX */
		    defaultX_,
			
			/*! Default center of scanned area in height dimension.
			\sa defaultY, setDefaultY */
			defaultY_,
			
			/*! Scanned area minimum width. 
			\sa minWidth, setMinWidth */
		    minWidth_, 
			
			/*! Scanned area minimum heigth. 
			\sa minHeight, setMinHeight */
			minHeight_,
			
			/*! Scanned area maximum width. 
			\sa maxWidth, setMaxWidth */
		    maxWidth_,
			
			/*! Scanned area maximum heigth. 
			\sa maxHeight, setMaxHeight */
			maxHeight_,
			
		    imageWidth_, 
			imageHeight_,
			
			/*! Scanned area single movement step value. 
			\sa step, setStep*/
		    step_, 
			
			/*! Scanned area resizing ratio.
			\sa ratio, setRatio*/
			ratio_;
		
		bool scanned_; //true when whole area is scaned with current scale
		
	private:
	
		class BadCoords : public Exception 
			{ public: BadCoords(int, int, int, int); };		
				
};
//----------------------------------------------------------------------------//

//VideoScanner inlines
//----------------------------------------------------------------------------//
inline int VideoScanner:: x1() const 
{ 
	return x1_; 
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: x2() const
{
	return x2_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: y1() const
{
	return y1_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: y2() const
{
	return y2_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: minX() const
{
	return minX_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: maxX() const
{
	return maxX_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: minY() const
{
	return minY_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: maxY() const
{
	return maxY_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: defaultX() const
{
	return defaultX_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: defaultY() const
{
	return defaultY_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: minWidth() const
{
	return minWidth_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: maxWidth() const
{
	return maxWidth_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: minHeight() const
{
	return minHeight_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: maxHeight() const
{
	return maxHeight_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: step() const
{
	return step_;
}
//----------------------------------------------------------------------------//
inline int VideoScanner:: ratio() const
{
	return ratio_;
}
//----------------------------------------------------------------------------//
inline bool VideoScanner:: scanned() const
{
	return scanned_;
}
//----------------------------------------------------------------------------//
inline VideoScanner::ScanningMode VideoScanner:: scanningMode() const
{
	return scanningMode_;	
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMinX( int x)
{
	minX_ = x;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMaxX( int x)
{
	maxX_ = x;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMinY( int y)
{
	minY_ = y;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMaxY( int y)
{
	maxY_ = y;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMinWidth(int minWidth)
{
	minWidth_ = minWidth;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMaxWidth(int maxWidth)
{
	maxWidth_ = maxWidth;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMinHeight(int minHeight)
{
	minHeight_ = minHeight;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setMaxHeight(int maxHeight)
{
	maxHeight_ = maxHeight;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setStep(int step)
{
	step_ = step;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setRatio(int ratio)
{
	ratio_  = ratio;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setDefaultX( int x)
{
	defaultX_ = x;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setDefaultY( int y)
{
	defaultY_ = y;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setImageSize( int width, int height)
{
	imageWidth_ = width;
	imageHeight_ = height;
}
//----------------------------------------------------------------------------//
inline Matrix& VideoScanner:: scanAreaToMatrix( Matrix& m)
{
	return m.scaleFrom(scannedArea_, m.rows(), m.cols());
}
//----------------------------------------------------------------------------//

// functions below are protected and inlined for convience
//----------------------------------------------------------------------------//
inline void VideoScanner:: reset()
{
	x1_ = minX_;
	y1_ = minY_;
	x2_ = x1_ + width_;
	y2_ = y1_ + height_;
	scanned_ = false;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: center()
{
	x1_ = defaultX_ - width_ / 2;
	y1_ = defaultY_ - height_ / 2;
	x2_ = x1_ + width_;
	y2_ = y1_ + height_;
	scanned_ = true;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: scale()
{		
	if (width_ > minWidth_) //scale image by ratio
	{
		width_  = width_  / ratio_ >= minWidth_  ? 
		          width_  / ratio_ : minWidth_;
		height_ = height_ / ratio_ >= minHeight_ ? 
		          height_ / ratio_ : minHeight_;
		x2_ = x1_ + width_;
		y2_ = y1_ + height_;
		scanned_ = false;
	}
	else maxArea();
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: maxArea()
{
	width_ = maxWidth_;
	height_ = maxHeight_;
	x2_ = x1_ + width_;
	y2_ = y1_ + height_;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: setCoords(  int x1, int y1, int x2, int y2)
{
	x1_ = x1;
	x2_ = x2;
	y1_ = y1;
	y2_ = y2;
	width_  = x2_ - x1_; 
	height_ = y2_ - y1_;
	scanned_ = true;
}
//----------------------------------------------------------------------------//
inline void VideoScanner:: move()
{
	if(x2_ + step_ <= maxX_)
	{
		x1_ += step_;
		x2_ += step_;
	}
	else
	{
		x1_ = minX_;
		x2_ = x1_ + width_;
		if (y2_ + step_ <= maxY_)
		{
			y1_ += step_;	
			y2_ += step_;
		}
		else //whole area scaned, return to (minX_, minY_)
		{
			y1_ = minY_;
			y2_ = y1_ + height_;
			scanned_ = true;
		}
	}
}
//----------------------------------------------------------------------------//

#endif //VIDEO_SCANNER_H
