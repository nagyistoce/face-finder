//----------------------------------------------------------------------------//
//                           FACE DETECTOR CLASS                              //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 3.00                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  3.00 - cpnn implementation                                                //
//  2.00 - version for matrix-based neural network // 01.09.2008 - 26.04.2009 //
//  1.00 - basic version                           // 12.03.2008 - 27.08.2008 //
//----------------------------------------------------------------------------//

#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include "NeuralNetworks.h"

class VideoScanner;

#define FACE_DETECTOR_LAYER_MIN 1024
#define FACE_DETECTOR_LAYER_MAX 16384

//----------------------------------------------------------------------------//
class FaceDetector : private CPNetwork
{
	public: 
		FaceDetector( int inputSize, int size);
		~FaceDetector();
		void loadImage( VideoScanner*);
		bool decide();
		DataType learn( bool isFace);
		void reset();
		void load( const char* filePath);		
		void save( const char* filePath);
	protected:
	private:
		size_;
		class LayerConstraints : public Exception 
			{ public: LayerConstraints( int); };
};
//----------------------------------------------------------------------------//
inline void FaceDetector:: reset()
{
	NeuralNetwork:: reset();
}
//----------------------------------------------------------------------------//
inline void FaceDetector:: load( const char* filePath)
{
	loadFromFile( filePath);
}
//----------------------------------------------------------------------------//
inline void FaceDetector:: save( const char* filePath)
{
	saveToFile( filePath);
}
//----------------------------------------------------------------------------//

#endif
