#include "FaceDetector.h"
//----------------------------------------------------------------------------//
FaceDetector:: FaceDetector( VideoInput* input, int layer2) 
{
	resizeDetector( layer2, input);
	setOutFunction( &Matrix::sigmoid);
	setDerivFunction( &Matrix::dSigmoid);
	setLearningSpeed( 0.001);
	faceValue_ = 1;
	nonfaceValue_ = 0;
	maxError_ = 0.01;
}
//----------------------------------------------------------------------------//
FaceDetector:: ~FaceDetector()
{
	//nothing to do
}
//----------------------------------------------------------------------------//
DataType FaceDetector:: run()
{
	netOutput_ = (DataType) BPNetwork::run();
	return netOutput_;
}
//----------------------------------------------------------------------------//
bool FaceDetector:: decide()
{	
	netOutput_ = (DataType) BPNetwork::run();	
	error_ = faceValue_ - netOutput_;
	if (-maxError_ <= error_ && error_ <= maxError_) return true;
	error_ = nonfaceValue_ - netOutput_;
	if (-maxError_ <= error_ && error_ <= maxError_) return false;	
	throw UnspecifiedAnswer( netOutput_);
}
//----------------------------------------------------------------------------//
DataType FaceDetector:: learn( bool isFace)
{
	DataType goodAnswer = isFace ? faceValue_ : nonfaceValue_;
	//running net
	netOutput_ = (DataType) NeuralNetwork::run();
	//compute and correct error
	error_ = goodAnswer - netOutput_;
	if (-maxError_ > error_ || error_ > maxError_)
	{
		computeDeltas( Matrix(1, 1, goodAnswer));
		correctWeights();
	}
	return error_;
}
//----------------------------------------------------------------------------//
void FaceDetector:: resizeDetector( int layerSize, VideoInput* input)
{
	if (layerSize < FACE_DETECTOR_LAYER_MIN ||
	    layerSize > FACE_DETECTOR_LAYER_MAX)
	    throw LayerConstraints( layerSize);
	if (input) //updating video input
		input_ = input;
	layer1_ = input_->detectorWidth() * input_->detectorHeight();
	layer2_ = layerSize; //hidden layer	
	//matrix of info for resizing net	
	Matrix dimension( 1, 4);
	dimension[0] = layer1_; //input layer
	dimension[1] = layer1_; //first layer fully connected with input
	dimension[2] = layer2_; //2nd layer (hidden and scalable by user)
 	//3 layers can learn any given pattern
	dimension[3] = 1; //only one neuron in last layer for decission
	resize( dimension);	//neural network resize
	assignInput( input_->detectorInput());
}
//----------------------------------------------------------------------------//

//exceptions
//----------------------------------------------------------------------------//
FaceDetector:: LayerConstraints:: LayerConstraints( int s)
{
	stringStream << "Cannot set hidden layer of face detector to size of" << s
	             << ". Size must be between " << FACE_DETECTOR_LAYER_MIN
	             << " and " << FACE_DETECTOR_LAYER_MAX << ".";	
}
//----------------------------------------------------------------------------//
FaceDetector:: UnspecifiedAnswer:: UnspecifiedAnswer( DataType answer)
{
	stringStream << "Face Detector gave answer " << answer 
	             << " for current input, which is unspecified."; 
}
//----------------------------------------------------------------------------//

