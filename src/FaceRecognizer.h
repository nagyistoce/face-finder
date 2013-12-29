//----------------------------------------------------------------------------//
//                             FACE RECOGNIZER CLASS                          //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 0.4                                                              //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  0.5 - disconnect from VideoInput                            // 05.10.2009 //
//  0.4 - verify function for simple face detection             // 26.09.2009 //
//	0.3 - increased ability of modelling initial nueurons                     //
//        weights to gain similarity with eigenfaces                          //
//        statistical face projection                           // 05.08.2009 //
//  0.2 - assign with VideoInput class                          // 19.05.2009 //
//  0.1 - basic class functionality                             // 23.04.2009 //
//----------------------------------------------------------------------------//

#ifndef FACE_RECOGNIZER_H
#define FACE_RECOGNIZER_H

//includes
//----------------------------------------------------------------------------//
#include "NeuralNetworks.h"
#include "VideoScanner.h"

//defines
//----------------------------------------------------------------------------//
//If input is not normalized ( ||input|| != 1) set this ON, 
//otherwise can be OFF (it will be sligthly faster). If unsure set ON.
#define FORCE_INPUT_NORMALIZATION ON

//Save neurons answers 
//If this is on, results of neron competition is saved to history matrix
#define COMPETITION_HISTORY OFF

//! Engine for face recognition
/*! Recognition is based of eigenfaces method. Face projections on eigenfaces
are computed and learned to counter propagation neural network. Network is 
teached to answer specific id number for each learned face. For compiling options see header file. */ 
//----------------------------------------------------------------------------//
class FaceRecognizer : private CPNetwork
{
	public:
		/*! Creates fully operational object, ready for face recognition.
		\param input pointer to VideoInput object which is used to obtain
		image source for recognition
		\param individuals number of diffrent persons which should be 
		later recognized 
		\param neuronsPerIndividual number of neurons in first layer for each 
		face class
		\param numberOfEigenfaces number of eigenfaces used to represent faces,
		what determines network input size
		\param eigenfacesFile path to CSV file with matrix of eigenfaces. It 
		should be matrix which each column is one of eigenfaces.
		\param eigenWidth width of images eigenfaces were made from
		\param eigenHeight height of images eigenfaces were made from
		\param avgFile path to file with vector matrix representing average
		face projection on eigenfaces matrix
		\param stdFile path to file with vector matrix representing standard
		devation of average projection coefficients. */
		FaceRecognizer( int individuals, int neuronsPerIndividual, 
		                int numberOfEigenfaces, int eigenWidth, int eigenHeight, 
		                const char* eigenfacesFile,
		                const char* avgFile,
		                const char* stdFile );
		~FaceRecognizer();
		
		/*! \return Number of eigenfaces*/
		int numberOfEigenfaces() const;
		
		/*! \return pointer to char buffer containing graphical representation
		eigenface specified by given index. */
		unsigned char* eigenface(int index);
		
		/*! Image width accepted by recognizer. */
		int imageWidth() const;
		
		/*! Image height accepted by recognizer. */
		int imageHeight() const;
		
		/*! Returns currently processed image. */
		const Matrix& currentImage() const;
		
		/*! Number of individuals current recognizer was designed for. */
		int individuals() const;
		
		/*! Number of neurons reserved for each face class. */
		int neuronsPerIndividual() const;
		
		/*! Size of first layer of CPNetwork. */
		int layerKohonen() const;
		
		/*! Size of second layer of CPNetwork. */
		int layerGrossberg() const;
		
		/* Value determines how quick neurons should fit their weights to 
		input. Speed must less than 1, high values may cause algorithm unstable
		or even incorrect. */
		DataType initialSpeed() const;
		void setInitialSpeed( DataType speed);
		
		/*! Minimal value of input vector similarity to recognize. */
		DataType faceThreshold();
		void setFaceThreshold( DataType threshold);
		
		/*! Loads new eigenfaces matrix what is equal to build completely new 
		object. \param imageWidth eigenfaces width
		\param imageHeight eigenfaces height
		\param file path to file with matrix of eigenfaces. Each column is
		eigenface vector of length = imageWidth * imageHeight */
		void loadEigenfaces( int imageWidth, int imageHeight, 
		                     const char * file);
		 
		/*! Loads vector with average values of face projection on 
		eigenfaces space
		\sa loadStdProjection*/
		void setProjectionAvg(const Matrix& projectionAvg);
		
		/*! Loads vector with standard devation values of face projection on 
		eigenfaces space
		\sa loadAvgProjection*/
		void setProjectionStd(const Matrix& projectionStd);
		                     
		/*! Modifies size of first layer of neural network. Causes data loss
		of all previous learning process. */
		void resizeRecognizer( int individuals, int neuronsPerIndividual,
		                       int numberOfEigenfaces = 0);

		/*! Clears all weights of neurons to their initial values. */
		void reset();
		
		/*! Projection on eigenfaces of image from current video input
		taken from scanner object. */
		void setInput( VideoScanner *input);
		
		/*! Get network classification of given image 
		\return if image is recognizable face returns its class id,
		if not (face not recognized or no face on image) returns 0 */
		unsigned int classify(); 	
		
		/*! Remember current projection in network, and set class number for 
		it. \param classNumber number to return for current projection */
		void setClassification( int classNumber);

		/*! Remember current projection in network, do not learn output. */
		int remember();
		
		/*! Set class number to return for current projection assuming its 
		already remebered by netwrk first layer. */
		void setClassId(int id);//2nd learning stage
		
		/*! Loads network weights from given file
		\param filePath path of NNW file to load from */
		void load( const char* filePath);
		
		/*! Saves network weights to given file
		\param filePath path of NNW file to save to */
		void save( const char* filePath);
		
	protected: /*class not designed for inheriting*/
	   
	private:
			
		int individuals_;
		int neuronsPerIndividual_;
		int numberOfEigenfaces_,
			eigenWidth_, eigenHeight_;
		Matrix eigenfaces_, eigenface_, projectionAvg_, projectionStd_, image_;
		DataType initialSpeed_;
		static DataType faceThreshold_;
#if COMPETITION_HISTORY
		Matrix history_;
		int historyIndex_;
		static const int historySize_;
#endif
		class EigenfacesSize : public Exception 
			{ public: EigenfacesSize(int, int, int); };
		class EigenfacesNumber : public Exception 
			{ public: EigenfacesNumber(int, int); };

};
//----------------------------------------------------------------------------//
inline int FaceRecognizer:: numberOfEigenfaces() const
{
	return numberOfEigenfaces_;
}
//----------------------------------------------------------------------------//
inline int FaceRecognizer:: imageWidth() const
{
	return eigenWidth_;
}
//----------------------------------------------------------------------------//
inline int FaceRecognizer:: imageHeight() const
{
	return eigenHeight_;
}
//----------------------------------------------------------------------------//
inline const Matrix& FaceRecognizer:: currentImage() const
{
	return image_;
}
//----------------------------------------------------------------------------//
inline int FaceRecognizer:: individuals() const
{
	return individuals_;
}
//----------------------------------------------------------------------------//
inline int FaceRecognizer:: neuronsPerIndividual() const
{
	return neuronsPerIndividual_;
}
//----------------------------------------------------------------------------//
inline int FaceRecognizer:: layerKohonen() const
{
	return competitiveSize();
}
//----------------------------------------------------------------------------//
inline int FaceRecognizer:: layerGrossberg() const
{
	return outstarSize();
}
//----------------------------------------------------------------------------//
inline DataType FaceRecognizer:: initialSpeed() const
{
	return initialSpeed_;
}
//----------------------------------------------------------------------------//
inline void FaceRecognizer:: setInitialSpeed( DataType speed)
{
	initialSpeed_ = speed;
}
//----------------------------------------------------------------------------//
inline DataType FaceRecognizer:: faceThreshold()
{
	return faceThreshold_;
}
//----------------------------------------------------------------------------//
inline void FaceRecognizer:: setFaceThreshold( DataType threshold)
{
	faceThreshold_ = threshold;
}
//----------------------------------------------------------------------------//
inline unsigned char* FaceRecognizer:: eigenface(int index)
{
	eigenface_ = eigenfaces_.col( index);
	return eigenface_.toUchar( eigenWidth_, eigenHeight_);
}
//----------------------------------------------------------------------------//
inline void FaceRecognizer:: setProjectionAvg(const Matrix& projectionAvg)
{
	projectionAvg_ = projectionAvg;
}
//----------------------------------------------------------------------------//
inline void FaceRecognizer:: setProjectionStd(const Matrix& projectionStd)
{
	projectionStd_ = projectionStd;
}
//----------------------------------------------------------------------------//
inline void FaceRecognizer:: load( const char* filePath)
{
	loadFromFile( filePath);
}
//----------------------------------------------------------------------------//
inline void FaceRecognizer:: save( const char* filePath)
{
	saveToFile( filePath);
}
//----------------------------------------------------------------------------//

#endif
