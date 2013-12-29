#include "FaceRecognizer.h"
//----------------------------------------------------------------------------//
#if COMPETITION_HISTORY
const int FaceRecognizer:: historySize_ = 1000;
#endif
DataType FaceRecognizer::faceThreshold_ = 0.75; 
//----------------------------------------------------------------------------//
FaceRecognizer:: FaceRecognizer( int individuals, int neuronsPerIndividual, 
                                 int numberOfEigenfaces,
                                 int eigenWidth, int eigenHeight,
                                 const char* eigenfacesFile,
                                 const char* avgFile,
                                 const char* stdFile )
: CPNetwork( numberOfEigenfaces, 
             individuals * neuronsPerIndividual, 
             log2( individuals) + 1),
  individuals_( individuals),
  neuronsPerIndividual_( neuronsPerIndividual),
  numberOfEigenfaces_(numberOfEigenfaces),
  eigenWidth_( eigenWidth),
  eigenHeight_(eigenHeight),
  image_( 1, eigenWidth * eigenHeight)
{
	eigenfaces_.loadFromCsv( eigenfacesFile);
	if (eigenfaces_.rows() != eigenWidth_ * eigenHeight_)
		throw EigenfacesSize( eigenWidth_, eigenHeight_, eigenfaces_.rows());
	if (numberOfEigenfaces_ != eigenfaces_.cols())
		throw EigenfacesNumber( eigenfaces_.cols(), numberOfEigenfaces_);
	//modifying initial weights to better fit statistical average eigenface
	projectionAvg_.loadFromCsv( avgFile);	
	projectionStd_.loadFromCsv( stdFile);
	layers_[0]->shapeWeights( projectionAvg_, projectionStd_);
	//because we altered weights apart from neurons learning algorithm
	//normalization must be applied on 1st layer
	layers_[0]->normalizeWeights();
	//default learning speeds for network
	initialSpeed_ = 0.7;
	setLearningSpeed( initialSpeed_, 0); //competition layer
	setLearningSpeed( 0.1, 1); //outstar layer	
#if COMPETITION_HISTORY
	history_.resize( historySize_, layers_[0]->inputSize());
	historyIndex_ = 0;
#endif
	cout << "[FaceRecognizer] input size is " << numberOfEigenfaces 
	<< ", using "<< individuals << " face classes, " <<  neuronsPerIndividual
	<<" neurons per each.\n";
}
//----------------------------------------------------------------------------//
FaceRecognizer:: ~FaceRecognizer()
{
#if COMPETITION_HISTORY
	history_.saveToCsv("RecognizerHistory.csv");
#endif
}
//----------------------------------------------------------------------------//
void FaceRecognizer:: resizeRecognizer( int individuals, 
                                        int neuronsPerIndividual,
                                        int numberOfEigenfaces)
{
	if (numberOfEigenfaces) 
	{
		numberOfEigenfaces_ = numberOfEigenfaces;
		//STANDARD INITIALIZATION, MUST PROVIDE NEW SHAPE VECTORS
		projectionAvg_.resize(1, numberOfEigenfaces_, (DataType)0.0);
		projectionStd_.resize(1, numberOfEigenfaces_, (DataType)1.0);
	}
	individuals_          = individuals;
	neuronsPerIndividual_ = neuronsPerIndividual;
	Matrix sizes(1,3);
	sizes[0] = numberOfEigenfaces_;
	sizes[1] = individuals_ * neuronsPerIndividual_;
	sizes[2] = ceil( log2( individuals_));
	NeuralNetwork::resize( sizes);
	//pre-shaping
	layers_[0]->shapeWeights( projectionAvg_, projectionStd_);
	layers_[0]->normalizeWeights();
#if COMPETITION_HISTORY
	history_.resize( historySize_, layers_[0]->size());
	historyIndex_ = 0;
#endif
	cout << "[FaceRecognizer] resized to " << sizes[0] << "x" << sizes[1]
	     << "x" << sizes[2] << endl;
}
//----------------------------------------------------------------------------//
void FaceRecognizer:: loadEigenfaces( int imageWidth, int imageHeight,
                                      const char * file)
{
	eigenWidth_ = imageWidth;
	eigenHeight_ = imageHeight;
	eigenfaces_.loadFromCsv( file);
	if (eigenfaces_.rows() != imageWidth * imageHeight)
		throw EigenfacesSize( imageWidth, imageHeight, eigenfaces_.rows());
	if (numberOfEigenfaces_ != eigenfaces_.cols())
		resizeRecognizer( individuals_, neuronsPerIndividual_, 
		                  eigenfaces_.cols());
}
//----------------------------------------------------------------------------//
void FaceRecognizer:: reset()
{
	//set new random weightrs with givemn params
	layers_[0]->shapeWeights( projectionAvg_, projectionStd_);
	//cout << layers_[0]->weights().col(649) << endl;
	//make neurons normalized
	layers_[0]->normalizeWeights();
	// reset neurons to learn with initial speed
	setLearningSpeed( initialSpeed_, 0);
	//reset second layer (set all weights to 0)
	layers_[1]->reset();
}
//----------------------------------------------------------------------------//
void FaceRecognizer:: setInput( VideoScanner *input)
{
	image_.reshape( eigenHeight_, eigenWidth_);
	input->scanAreaToMatrix( image_);
	image_.reshape( 1, eigenWidth_ * eigenHeight_);
	Matrix::m_is_a_x_b( inputLayer_, image_, eigenfaces_);		
#if FORCE_INPUT_NORMALIZATION
	normalizeInput();
#endif
}
//----------------------------------------------------------------------------//
unsigned int FaceRecognizer:: classify()
{
	//run the the network	
	run();
	//check winning impulse value 
	if (((NLayer*)layers_[0])->impulses()[winner_] < faceThreshold_) 
		return 0; //perform no calassification
	//if passed the threshold return output vector as number
	return layers_[1]->output().binary();
}
//----------------------------------------------------------------------------//
void FaceRecognizer:: setClassification( int classNumber)
{	
	findWinner();
	learnWinner();
	learnPattern( Matrix( (unsigned short) classNumber, 
	                      (unsigned char)  outstarSize() ) );
}
//----------------------------------------------------------------------------//
int FaceRecognizer:: remember()
{
	findWinner();
#if COMPETITION_HISTORY
	/* Turned off
 	history_.setRow( historyIndex_, ((WTALayer*)layers_[0])->impulses());
	*/
#endif
	learnWinner();
	return winner_;
}
//----------------------------------------------------------------------------//
void FaceRecognizer:: setClassId(int id)
{
	run();
#if COMPETITION_HISTORY
	history_.setRow( historyIndex_++, layers_[0]->weights().col(winner_));
#endif
	learnPattern( Matrix( (unsigned short) id, 
	                      (unsigned char)  layers_[1]->size()));
}
//----------------------------------------------------------------------------//

//Exceptions
//----------------------------------------------------------------------------//
FaceRecognizer:: EigenfacesSize:: EigenfacesSize( int w, int h, int l)
{
	stringStream << "Eigenfaces load: Cant create " << w << "x" << h
	             << " eigenfaces images because there is " << l
	             << " pixels per image in loaded matrix.";	
}
//----------------------------------------------------------------------------//
FaceRecognizer:: EigenfacesNumber:: EigenfacesNumber(int l, int e)
{
	stringStream << l << " eigenfaces were loaded, but " << e
	             << " were exptected by recognizer input.";
}
//----------------------------------------------------------------------------//
