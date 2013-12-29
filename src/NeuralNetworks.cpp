//----------------------------------------------------------------------------//
#include "NeuralNetworks.h"
#include <cmath>

//NeuronsLayer
//----------------------------------------------------------------------------//
NeuronsLayer:: NeuronsLayer()
{
	minWeight_ = -1;
	maxWeight_ = 1;
	learningSpeed_ = 1; //neutral value
}
//----------------------------------------------------------------------------//
NeuronsLayer:: NeuronsLayer(int in, int out)
{
	minWeight_ = -1;
	maxWeight_ = 1;
	learningSpeed_ = 1;
	weights_.resize(in, out, minWeight_, maxWeight_);	
	output_.resize(1, out);
}
//----------------------------------------------------------------------------//
NeuronsLayer:: ~NeuronsLayer()
{
	//nothing to do
}
//----------------------------------------------------------------------------//
const Matrix& NeuronsLayer:: compute(const Matrix& input)
{
	Matrix:: m_is_a_x_b( output_, input, weights_);
	return output_;
}
//----------------------------------------------------------------------------//
void NeuronsLayer:: resize(int in, int out)
{
	weights_.resize(in, out, minWeight_, maxWeight_);
}
//----------------------------------------------------------------------------//
void NeuronsLayer:: reset()
{
	weights_.setRandom( minWeight_, maxWeight_);
}
//----------------------------------------------------------------------------//
void NeuronsLayer:: load( fstream& file)
{
	file >> weights_;
}
//----------------------------------------------------------------------------//
void NeuronsLayer:: save( fstream& file)
{
	file << weights_;
}
//----------------------------------------------------------------------------//

//BLayer
//----------------------------------------------------------------------------//
BLayer:: BLayer()
{
	//nothing to do
}
//----------------------------------------------------------------------------//
BLayer:: BLayer(int in, int out) : NeuronsLayer( in, out)
{
	biases_.resize(1, out, (DataType)0.0);
}
//----------------------------------------------------------------------------//
BLayer:: ~BLayer()
{
	//Nothing to do
}
//----------------------------------------------------------------------------//
const Matrix& BLayer:: compute(const Matrix& input)
{
	Matrix:: m_is_a_x_b( output_, input, weights_) -= biases_;
	return output_;
}
//----------------------------------------------------------------------------//
void BLayer:: resize(int in, int out)
{
	weights_.resize(in, out, minWeight_, maxWeight_);
	biases_.resize(1, out, (DataType)0.0);
}
//----------------------------------------------------------------------------//
void BLayer:: reset()
{
	weights_.setRandom( minWeight_, maxWeight_);
	biases_.set(0);
}
//----------------------------------------------------------------------------//
void BLayer:: load( fstream& file)
{
	file >> weights_;
	file >> biases_;
}
//----------------------------------------------------------------------------//
void BLayer:: save( fstream& file)
{
	file << weights_;
	file << biases_;
}
//----------------------------------------------------------------------------//

//NLayer
//----------------------------------------------------------------------------//
NLayer:: NLayer()
{
	outFunction_ = &Matrix::passValue;
	derivFunction_ = &Matrix::constant;
}
//----------------------------------------------------------------------------//
NLayer:: NLayer(int in, int out) : NeuronsLayer( in, out)
{
	outFunction_ = &Matrix::passValue;
	derivFunction_ = &Matrix::constant;
	impulses_.resize(1, out);
}
//----------------------------------------------------------------------------//
NLayer:: ~NLayer()
{
	//Nothing to do
}
//----------------------------------------------------------------------------//
const Matrix& NLayer:: compute(const Matrix& input)
{
	Matrix:: m_is_a_x_b( impulses_, input, weights_);
	output_ = outFunction_( impulses_);
	return output_;
}
//----------------------------------------------------------------------------//
void NLayer:: resize(int in, int out)
{
	weights_.resize(in, out, minWeight_, maxWeight_);
	impulses_.resize(1, out);
}
//----------------------------------------------------------------------------//
void NLayer:: load( fstream& file)
{
	file >> weights_;
	impulses_.resize(1, weights_.cols());
}
//----------------------------------------------------------------------------//

//NBLayer
//----------------------------------------------------------------------------//
NBLayer:: NBLayer()
{
	outFunction_ = &Matrix::passValue;
	derivFunction_ = &Matrix::constant;
}
//----------------------------------------------------------------------------//
NBLayer:: NBLayer(int in, int out)  : BLayer( in, out)
{
	outFunction_ = &Matrix::passValue;
	derivFunction_ = &Matrix::constant;
	impulses_.resize(1, out);
}
//----------------------------------------------------------------------------//
NBLayer:: ~NBLayer()
{
	//Nothing to do	
}
//----------------------------------------------------------------------------//
const Matrix& NBLayer:: compute(const Matrix& input)
{
	Matrix:: m_is_a_x_b( impulses_, input, weights_);
	impulses_ -= biases_;
	output_ = outFunction_( impulses_);
	return output_;
}
//----------------------------------------------------------------------------//
void NBLayer:: resize(int in, int out)
{
	weights_.resize(in, out, minWeight_, maxWeight_);
	biases_.resize(1, out, (DataType)0.0);
	impulses_.resize(1, out);
}
//----------------------------------------------------------------------------//
void NBLayer:: load( fstream& file)
{
	file >> weights_;
	file >> biases_;
	impulses_.resize(1, weights_.cols());
}
//----------------------------------------------------------------------------//

//BPLayer
//----------------------------------------------------------------------------//
BPLayer:: BPLayer()
{
	//Nothing to do
}
//----------------------------------------------------------------------------//
BPLayer:: BPLayer(int in, int out) : NBLayer( in, out)
{
	deltas_.resize(1, out);
}
//----------------------------------------------------------------------------//
BPLayer:: ~BPLayer()
{
	//Nothing to do
}
//----------------------------------------------------------------------------//
void BPLayer:: propagateError( const BPLayer& higherLayer)
{
	Matrix:: m_is_a_x_bT( deltas_, higherLayer.deltas_, higherLayer.weights_); 
}
//----------------------------------------------------------------------------//
void BPLayer:: learn(const Matrix& input)
{
	//dW = - speed * dOut(impuls) * delta * In - standard equation for neuron
	//dW = - speed * Input' * (dOut(impuls) .* delta) - matrix version
	//for biases input == 1 
	//because we use nonlinear functions, which f'(x) = g(f(x))
	//we calulate derivative of outputs instead of neurons impulses
	//but this is not universal version
	const_cast<Matrix&>(input).T(); //this operation want change input
	weights_ += learningSpeed_ * input * 
	            Matrix:: m_is_a_mul_b( deltas_, deltas_, derivFunction_( output_));
	const_cast<Matrix&>(input).T(); //back to former shape
	biases_ += learningSpeed_ * 
	           Matrix:: m_is_a_mul_b( deltas_, deltas_, derivFunction_( output_));	
}
//----------------------------------------------------------------------------//
void BPLayer:: resize(int in, int out)
{
	weights_.resize(in, out, minWeight_, maxWeight_);
	biases_.resize(1, out, (DataType)0.0);
	impulses_.resize(1, out);
	deltas_.resize(1, out);
}
//----------------------------------------------------------------------------//
void BPLayer:: load( fstream& file)
{
	file >> weights_;
	file >> biases_;
	impulses_.resize(1, weights_.cols());
	deltas_.resize(1, weights_.cols());
}
//----------------------------------------------------------------------------//

//WTALayer
//----------------------------------------------------------------------------//
WTALayer:: WTALayer()
{
	winner_ = 0;
	minimalSpeed_ = 0.05; 
	speedDecrease_ = 0.005;
	outFunction_ = &Matrix::winnerTakesAll;
	output_.set(0);
}
//----------------------------------------------------------------------------//
WTALayer:: WTALayer(int in, int out) : NLayer( in, out)
{
	winner_ = 0;
	minimalSpeed_ = 0.05; 
	speedDecrease_ = 0.005;
	outFunction_ = &Matrix::winnerTakesAll;
	speeds_.resize( 1, out, (DataType)1.0);
	output_.set(0);
}
//----------------------------------------------------------------------------//
WTALayer:: ~WTALayer()
{
	//Nothing to do
}
//----------------------------------------------------------------------------//
void WTALayer:: reset()
{
	weights_.setRandom( minWeight_, maxWeight_);
	speeds_.set( learningSpeed_);
}
//----------------------------------------------------------------------------//
const Matrix& WTALayer:: compute( const Matrix& input)
{
	Matrix:: m_is_a_x_b( impulses_, input, weights_);
	output_[winner_] = 0;
	winner_ = impulses_.maxIndex();
	output_[winner_] = 1;
	return output_;
}
//----------------------------------------------------------------------------//
void WTALayer:: learn( const Matrix& input)
{
	//Learning method - for single neuron
	//weigths_.col(neuron) += speed_[neuron] * (input - weigths_.col(neuron));

	//single neuron modification
	Matrix neuron = weights_.col( winner_); //copy of weights column
	neuron.T();// fake transpose, doesnt touch data array
	neuron += speeds_[winner_] * ( input - neuron); //modyfies weights_ column
	weights_.setCol( winner_, neuron);
	//decreasing competition speeed
	if (speeds_[winner_] > minimalSpeed_)
		speeds_[winner_] -= speedDecrease_;
}
//----------------------------------------------------------------------------//

//CPN 2nd layer
//----------------------------------------------------------------------------//
Cpn2ndLayer:: Cpn2ndLayer()
{
	//nothing
}
//----------------------------------------------------------------------------//
Cpn2ndLayer:: Cpn2ndLayer( int in, int out)
	: NLayer( in, out)
{
	setOutFunction( &Matrix::step);
	weights_.set(0);
}
//----------------------------------------------------------------------------//
Cpn2ndLayer:: ~Cpn2ndLayer()
{
	//nothing
}
//----------------------------------------------------------------------------//
void Cpn2ndLayer:: learn( int winner, const Matrix& pattern)
{
	Matrix weights = weights_.row( winner);
	weights += learningSpeed_ * ( pattern - output_);
	weights_.setRow( winner, weights);
}
//----------------------------------------------------------------------------//


//NeuralNetwork
//----------------------------------------------------------------------------//
NeuralNetwork:: NeuralNetwork()
{
	layersNumber_ = 0;
	layers_       = 0;
}
//----------------------------------------------------------------------------//
NeuralNetwork:: NeuralNetwork(const Matrix& dimensions)
{	
	resize( dimensions);
}
//----------------------------------------------------------------------------//
NeuralNetwork:: ~NeuralNetwork()
{
	if (layers_)
	{
		for (int i = 0; i < layersNumber_; i++)
			delete layers_[i];
		delete[] layers_;
	}
}
//----------------------------------------------------------------------------//
void NeuralNetwork:: setLearningSpeed( DataType speed, int index)
{
	if (index == -1)
	{
		for (int i = 0; i < layersNumber_; i++)
			layers_[i]->setLearningSpeed( speed);
	}
	else layers_[index]->setLearningSpeed( speed);
}
//----------------------------------------------------------------------------//
void NeuralNetwork:: initializeLayers( const Matrix& dimensions)
{
	//new dimenions
	layers_ = new NeuronsLayer *[layersNumber_];
	for (int i = 0; i < layersNumber_; i++)
		layers_[i] = new NeuronsLayer(dimensions[i], dimensions[i+1]);
}
//----------------------------------------------------------------------------//
void NeuralNetwork:: resize (const Matrix& dimensions)
{
	//removing old network
	if (layers_)
	{
		for (int i = 0; i < layersNumber_; i++)
			delete layers_[i];
		delete[] layers_;
	}
	//input resize
	layersNumber_ = dimensions.length() - 1; //dimensions[0] is for input layer
	inputLayer_.resize(1, dimensions[0], (DataType)0.0);
	//Layers resize	
	initializeLayers( dimensions); //protected virtual 
}
//----------------------------------------------------------------------------//
void NeuralNetwork:: reset()
{
	//Layers reset
	for (int i = 0; i < layersNumber_; i++)
		layers_[i]->reset();
}
//----------------------------------------------------------------------------//
void NeuralNetwork:: assignInput(const Matrix& input)
{
	if (input.rows() != inputLayer_.rows() || 
		input.cols() != inputLayer_.cols()) 
		throw BadInput( inputLayer_, input);	
	inputLayer_ = input;
}
//----------------------------------------------------------------------------//
const Matrix& NeuralNetwork:: run()
{
	//Layers computation - this must be done for each network 
	//(speed optimalization - no virtual functions allowed)
	layers_[0]->compute( inputLayer_);
	for (int i = 1; i < layersNumber_; i++)
		layers_[i]->compute( layers_[i-1]->output()); //this executes NeuronsLayer::compute
	//Return last layer output
	return layers_[layersNumber_ - 1]->output(); 
}
//----------------------------------------------------------------------------//
void NeuralNetwork:: loadFromFile(const char* filePath)
{
	fstream f;
	f.open( filePath, ios::in | ios::binary);
	if (f.fail()) throw FileNotOpen( filePath);
	char header[] = "xxx\0";
	f.read( header, 3 * sizeof(char));
	if (string("NNW").compare(header) != 0) throw FileBadHeader( filePath);
	int tmp;
	f.read((char*)&tmp, sizeof(int));
	inputLayer_.resize(1, tmp, (DataType)0.0);
	f.read((char*)&layersNumber_, sizeof(int));
	for (int i = 0; i < layersNumber_; i++)
	{
		//Layers loading
		layers_[i]->load( f);
	}
	f.close();
}
//----------------------------------------------------------------------------//
void NeuralNetwork:: saveToFile(const char* filePath)
{
	fstream f;
	f.open( filePath, ios::out | ios::trunc | ios::binary);
	if (f.fail()) throw FileNotOpen( filePath);
	char header[] = "NNW";
	f.write( header, 3 * sizeof(char));
	int tmp = inputLayer_.cols();
	f.write((char*)&tmp, sizeof(int));
	f.write((char*)&layersNumber_, sizeof(int));
	for (int i = 0; i < layersNumber_; i++)
	{
		//Layers save
		layers_[i]->save( f);
	}
	f.close();
}
//----------------------------------------------------------------------------//

//BPNetwork
//----------------------------------------------------------------------------//
BPNetwork:: BPNetwork()
{
	//Nothing to do
}
//----------------------------------------------------------------------------//
BPNetwork:: BPNetwork(const Matrix& dimensions)
{
	resize( dimensions);
}
//----------------------------------------------------------------------------//
BPNetwork:: ~BPNetwork()
{
	//Nothing to do
}
//----------------------------------------------------------------------------//
void BPNetwork:: initializeLayers( const Matrix& dimensions)
{
	//new dimenions
	layers_ = new NeuronsLayer *[layersNumber_];
	for (int i = 0; i < layersNumber_; i++)
		layers_[i] = new BPLayer(dimensions[i], dimensions[i+1]);
}
//----------------------------------------------------------------------------//
const Matrix& BPNetwork:: run()
{
	//Layers computation - this must be done for each network 
	//(speed optimalization - no virtual functions allowed)
	((BPLayer*) layers_[0])->compute( inputLayer_);
	for (int i = 1; i < layersNumber_; i++)
		((BPLayer*) layers_[i])->compute( layers_[i-1]->output());
	//Return last layer output
	return layers_[layersNumber_ - 1]->output();
}
//----------------------------------------------------------------------------//
void BPNetwork:: computeDeltas(const Matrix& expectedOutput)
{	
	//the highest layer
	((BPLayer*) layers_[layersNumber_ - 1])->deltas() = expectedOutput - 
	                                         layers_[layersNumber_ - 1]->output();
	//back-propagation
	for (int i = layersNumber_ - 1; i > 0; i--)
	{
		((BPLayer*) layers_[i - 1])->propagateError( (const BPLayer&) *layers_[i]);
	}
}
//----------------------------------------------------------------------------//
void BPNetwork:: correctWeights()
{
	((BPLayer*)layers_[0])->learn( inputLayer_);
	for (int i = 1; i < layersNumber_; i++)
	{
		((BPLayer*)layers_[i])->learn( layers_[i-1]->output());
	}
}
//----------------------------------------------------------------------------//


//CPNetwork
//----------------------------------------------------------------------------//
CPNetwork:: CPNetwork()
{
	winner_ = -1;
}
//----------------------------------------------------------------------------//
CPNetwork:: CPNetwork( int inputSize, int competitiveSize, int outstarSize)
{
	inputLayer_.resize(1, inputSize, (DataType)0.0);
	layersNumber_ = 2;
	layers_ = new NeuronsLayer *[2];
	layers_[0] = new WTALayer( inputSize, competitiveSize);	
	layers_[1] = new Cpn2ndLayer( competitiveSize, outstarSize);		
	//vital part of CPN learning concept is neurons normalization 
	//which are columns of weights matrix (only 1st layer need to be normalized)
	layers_[0]->normalizeWeights(); 
	winner_ = -1; 
}
//----------------------------------------------------------------------------//
CPNetwork:: ~CPNetwork() 
{
	// nothing here 
}
//----------------------------------------------------------------------------//
void CPNetwork:: initializeLayers( const Matrix& dimensions)
{
	//new dimenions
	layers_ = new NeuronsLayer *[2];
	layers_[0] = new WTALayer(dimensions[0], dimensions[1]);
	layers_[1] = new Cpn2ndLayer(dimensions[1], dimensions[2]);
}
//----------------------------------------------------------------------------//
int CPNetwork:: findWinner()
{
	((WTALayer*)layers_[0])->compute( inputLayer_);
	winner_ = ((WTALayer*)layers_[0])->winner();
	return winner_;
}
//----------------------------------------------------------------------------//


//exceptions
//----------------------------------------------------------------------------//
NeuralNetwork:: BadInput:: BadInput(const Matrix& i, const Matrix& m)
{
	stringStream << "Bad input for this neural network. Should be matrix " 
	             << i.rows() << "x" << i.cols() << " but is matrix " 
	             << m.rows() << "x" << m.cols() << ".";
}
//----------------------------------------------------------------------------//
NeuralNetwork:: FileNotOpen:: FileNotOpen(const char* f)
{
	stringStream << "Neural Network load/save: couldnt open file: " << f;
}
//----------------------------------------------------------------------------//
NeuralNetwork:: FileBadHeader:: FileBadHeader(const char* f)
{
	stringStream << "Cant load Neural Network from " << f 
	             << ". File has wrong format (header mismatched).";
}
//----------------------------------------------------------------------------//
