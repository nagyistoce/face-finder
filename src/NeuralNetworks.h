//----------------------------------------------------------------------------//
//                  NEURAL NETWORKS CLASSES AND DEFINITIONS                   //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 0.70                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
// 	0.78 - removed modified_ property, performance upgrade      // 26.09.2009 //
//  0.70 - change architecture tu layer independent             // 30.08.2009 //
//	0.30 - Add matrix holding nerons impulses                                 //
//         (weighted sum of input)                              // 20.05.2009 //
//  0.29 - Counter-propagation network implementtion            // 19.03.2009 //
//	0.22 - resize function                                      // 15.01.2009 //
//  0.20 - back-propagation learning algorithm                  // 09.10.2008 //
//  0.16 - exception handling                                   // 04.10.2008 //
//  0.15 - implement bias and non-linear output,                              //
//         weights load/save                                    // 29.09.2008 //
//  0.12 - display functions, layers loading                    // 26.09.2008 //
//  0.10 - basic interface for NN (e.g. creating, running)      // 23.09.2008 //
//----------------------------------------------------------------------------//

#ifndef NEURAL_NETWORKS_H
#define NEURAL_NETWORKS_H

//includes
//----------------------------------------------------------------------------//
#include "matrix.h"
#include <fstream>

//! Linear neurons layer
/*! Basic definitions of neurons layer for use as a base for more 
sophisticated layers. Output of this layer is simply multiplication of input and weights matrices. */
//----------------------------------------------------------------------------//
class NeuronsLayer
{
	public:
		NeuronsLayer();
		/*! Creates layers with given number of inputs and outputs.
		\sa resize */
		NeuronsLayer(int in, int out);
		virtual ~NeuronsLayer();
		/*! \return Size of layer input */
		int inputSize() const;
		/*! \return Size of layer (number of neurons) */
		int size() const;
		/*! Provides read and wrtie access to layers neurons weights. */
		Matrix& weights();
		/*! Initialize neurons weights with normal randoms numbers of given parameters.
		\param avg vector specifying average (mean) value of each neuron weight seperately (that is for each neurons input) 
		\param std vector like avg, providing stard devation of weights value */
		void shapeWeights(const Matrix& avg, const Matrix& std);
		/*! Performs neurons normalization. After that neurons weights norm is equal to 1. */
		void normalizeWeights();
		/*! Learning speed is value used in most of neural networks learning algorithms,
		\return layer learning speed value
		\sa setLearningSpeed*/
		DataType learningSpeed() const;
		/*! Sets learning speed to given speed. */
		virtual void setLearningSpeed( DataType speed);		

		/*! Allows read access to layer output vector */
		const Matrix& output() const;

		/*! Performs computation of layer output for current input. This function 
		isnt virtual to get better performance, so it must be called 
        explicitly using scope operator to ensure execution of proper code.*/
		const Matrix& compute(const Matrix& input);

		/*! Resizes layer to given size values
		\param in number of inputs for each neuron in layer
		\param out number of neurons (and layer outputs as well) */
		virtual void resize(int in, int out);
		/*! Set all weights to random values N(0,2)*/
		virtual void reset();
		/*! Reads weights from opended and se file stream */
		virtual void load( fstream& file);
		/*! Writes weights to opended and se file stream */
		virtual void save( fstream& file);
	protected:
		Matrix weights_; //layer weights
		Matrix output_; //1xN vector
		DataType minWeight_, maxWeight_; //weights constraints
		DataType learningSpeed_;
	private:
};
//----------------------------------------------------------------------------//
inline int NeuronsLayer:: inputSize() const
{
	return weights_.rows();
}
//----------------------------------------------------------------------------//
inline int NeuronsLayer:: size() const
{
	return weights_.cols();
}
//----------------------------------------------------------------------------//
inline Matrix& NeuronsLayer:: weights()
{
	return weights_;
}
//----------------------------------------------------------------------------//
inline void NeuronsLayer:: shapeWeights(const Matrix& avg, const Matrix& std)
{
	weights_.randomVectors( avg, std); //set each column (neuron)	
}
//----------------------------------------------------------------------------//
inline DataType NeuronsLayer:: learningSpeed() const
{
	return learningSpeed_;
}
//----------------------------------------------------------------------------//
inline void NeuronsLayer:: setLearningSpeed( DataType speed)
{
	learningSpeed_ = speed;
}
//----------------------------------------------------------------------------//
inline const Matrix& NeuronsLayer:: output() const
{
	return output_;
}
//----------------------------------------------------------------------------//
inline void NeuronsLayer:: normalizeWeights()
{
	weights_.normalize(); //normalize columns (each neuron seperately)
}
//----------------------------------------------------------------------------//

//! Biased neurons layer
/*! This is linear neurons layers with bias value added for each neuron. */
//----------------------------------------------------------------------------//
class BLayer : public NeuronsLayer
{
	public:
		BLayer();
		BLayer(int in, int out);
		~BLayer();
		const Matrix& compute(const Matrix& input);
		/*! Vector containing biases for each neuron in layer. */
		Matrix& biases();
		//virtual functions
		void resize(int in, int out);
		void reset();
		void load( fstream& file);
		void save( fstream& file);
	protected:
		Matrix biases_; //bias of each neuron
	private:
};
//----------------------------------------------------------------------------//
inline Matrix& BLayer:: biases()
{
	return biases_;
}
//----------------------------------------------------------------------------//

//! Nonlinear neurons layer
/*! In this layer multiplication of input and weights matrices is called 
impulses, Output is value of nonlinear function result fo impulses. */
//----------------------------------------------------------------------------//
class NLayer : public NeuronsLayer
{
	public:
		NLayer();
		NLayer(int in, int out);
		~NLayer();
		Matrix& impulses();
		/*! Sets output function used in layer
		\param outFunction address of function to use in layer */
		void setOutFunction( Matrix (*outFunction)(const Matrix&));
		/*! Sets derivative of output function used in layer
		\param derivFunction address of function to use in layer */
		void setDerivFunction( Matrix (*derivFunction)(const Matrix&));
		const Matrix& compute(const Matrix& input);
		//virtual functions
		void resize(int in, int out);
		void load( fstream& file);
	protected:
		Matrix impulses_; 
		Matrix (*outFunction_)(const Matrix&);
		Matrix (*derivFunction_)(const Matrix&); 
	private:
};
//----------------------------------------------------------------------------//
inline Matrix& NLayer:: impulses()
{
	return impulses_;
}
//----------------------------------------------------------------------------//
inline void NLayer:: setOutFunction( Matrix (*outFunction)(const Matrix&))
{
	outFunction_ = outFunction;
}
//----------------------------------------------------------------------------//
inline void NLayer:: setDerivFunction( Matrix (*derivFunction)(const Matrix&))
{
	derivFunction_ = derivFunction;
}
//----------------------------------------------------------------------------//

//! Nonlinear biased neurons layer
/*! Layer of neurons that have bias and nonlinear output function */
//----------------------------------------------------------------------------//
class NBLayer : public BLayer
/* Class could deriv from BLayer and NLayer but gives abiguosity and unsued 
extra objects form NeuronsLayer, its easier to rewrite code from NLayer 
and deriv only BLayer cause of more similarities */
{
	public:
		NBLayer();
		NBLayer(int in, int out);
		~NBLayer();
		const Matrix& compute(const Matrix& input);
		Matrix& impulses();
		void setOutFunction( Matrix (*outFunction)(const Matrix&));
		void setDerivFunction( Matrix (*derivFunction)(const Matrix&));
		//virtual functions
		void resize(int in, int out);
		void load( fstream& file);
	protected:
		Matrix impulses_; 
		Matrix (*outFunction_)(const Matrix&);
		Matrix (*derivFunction_)(const Matrix&); 
	private:
};
//----------------------------------------------------------------------------//
inline Matrix& NBLayer:: impulses()
{
	return impulses_;
}
//----------------------------------------------------------------------------//
inline void NBLayer:: setOutFunction( Matrix (*outFunction)(const Matrix&))
{
	outFunction_ = outFunction;
}
//----------------------------------------------------------------------------//
inline void NBLayer:: setDerivFunction( Matrix (*derivFunction)(const Matrix&))
{
	derivFunction_ = derivFunction;
}
//----------------------------------------------------------------------------//

//! Neurons layer for back-propagation network
/*! Class has NBLayer interface and provides methods for back-propagation
learning alogrithm */
//----------------------------------------------------------------------------//
class BPLayer : public NBLayer
{
	public:
		BPLayer();
		BPLayer(int in, int out);
		~BPLayer();
		/*! Access to deltas matrix which contain delta (output error) for
		each neuron in layer. */
		Matrix& deltas();
		/*! Performs delta back-propagation from given layer to yhis layer 
		\param higherLayer reference to layer from which neurons deltas 
        should be propagated */
		void propagateError( const BPLayer& higherLayer);
		/*! Performs Back-Propagation learning algorithm.
		Function computes weights modification based on current values in 
		deltas vector and given input */
		void learn(const Matrix& input);
		//virtual functions
		void resize(int in, int out);
		void load( fstream& file);
	protected:
		Matrix deltas_;
	private:
};
//----------------------------------------------------------------------------//
inline Matrix& BPLayer:: deltas()
{
	return deltas_;
}
//----------------------------------------------------------------------------//

//! Winner Takes All Layer
/*! Layer with non-linear output function: neuron with highest impulse get output equal to 1, the rest neurons outputs are set to 0. */
//----------------------------------------------------------------------------//
class WTALayer : public NLayer 
{
	public:
		WTALayer();
		WTALayer(int in, int out);
		~WTALayer();
		/*! Sets initial speed of learning algorithm. Each winning decreases neuron learning speed. */
		void setLearningSpeed( DataType speed);
		
		/*! Index of neuron that is the best match to input in whole layer. */
		int winner() const;
		void reset();
		const Matrix& compute( const Matrix& input);

		/*! This function learns layer on WTA basis: only neurons with highest 
		    impulse is learned. */
		void learn( const Matrix& input); 

	protected:
		Matrix speeds_; //learning speed of each neuron
		int winner_; //winning neuron index
		DataType minimalSpeed_, speedDecrease_;
	private:
};
//----------------------------------------------------------------------------//
inline void WTALayer:: setLearningSpeed( DataType speed)
{
	speeds_.set( learningSpeed_ = speed);
}
//----------------------------------------------------------------------------//
inline int WTALayer:: winner() const
{
	return winner_;
}
//----------------------------------------------------------------------------//


//! Counter Propagation 2nd layer
/*! Non-linear layer designed for output layer in CPNN. */
//----------------------------------------------------------------------------//
class Cpn2ndLayer : public NLayer
{
	public:
		Cpn2ndLayer();
		Cpn2ndLayer( int in, int out);
		~Cpn2ndLayer();
		const Matrix& compute( const Matrix& input);
		/*! Learning with Grossberg algorithm (outstar)*/
		void learn( int winner, const Matrix& pattern);
		void reset();
	protected:
	private:
};
//----------------------------------------------------------------------------//
inline const Matrix& Cpn2ndLayer:: compute( const Matrix& input)
{
	return output_ = outFunction_( Matrix:: 
	                               m_is_a_x_b( impulses_, input, weights_));
}
//----------------------------------------------------------------------------//
inline void Cpn2ndLayer:: reset()
{
	weights_.set(0);
}
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//! Interface for neural networks
/*! NeuralNetwork is base class for all types neural networks classes. Provides
basic interface for neural network, but performs no learning and  makes no 
sence to is it directly. */
class NeuralNetwork
{
	public:
		
		/*! You must call resize after creating object by default costructor.
		\sa resize */
		NeuralNetwork();
		
		/*! Creates network with given geometry (number and sizes of network 
		layers). \param dimensions - matrix of size 1xN, where N-1 is number of
		 layers in NeuralNetwork. For i = 1 : N, dimensions[i] is number of 
		 neurons in i-th layer, dimension[0] is "input layer" size. */
		NeuralNetwork(const Matrix& dimensions);
		
		~NeuralNetwork();
		
		int layersNumber() const;
		
		/*! Most of neural network learning alogrithms uses value called 
		learning speed for weights modification. By default speed is set to 1, 
		so it won't affect anything.
		\param speed desired learning speed to set
		\param index 0-based index of network layer to modify. If index is -1 
		(default) speed will be set in all layers. */ 
		void setLearningSpeed( DataType speed, int index = -1);
		
		/*! Assigns input matrix to external matrix. Thanks to CountedHandle 
		system of matrix data managing this must be	done only once before 
		network running, and each time you want connect	to other source. After 
		assigning network shares the same memory, so all changes done to 
		external input affects also network input. 
		\param input matrix of size 1xIN, where IN is number of network 
		inputs (size of input layer). */
		void assignInput(const Matrix& input); 
		
		/*! Computes network answer for current input. This function should be 
		overloded in every inherited network class to perform computations 
		specific to exact network type. Function is not	virtual in order to 
		maximally increase network computations speed. 
		\return output matrix of size 1xOUT, where OUT is outputs number. */ 
		const Matrix& run(); 
				
		/*! Changes geometry of network - number and sizes of network layers.
		\param dimensions - matrix of size 1xN, where N-1 is number of
		 layers in NeuralNetwork. For i = 1 : N, dimensions[i] is number of 
		 neurons in i-th layer, dimension[0] is "input layer" size. */
		void resize (const Matrix& dimension);
		
		/*! Reset all weigts in network. This will have effect similar to
		creating completely new network. All learn progress is lost. */
		void reset();
		
		/*! Load network weights from file of given path. It is advised to keep
		naming convesion as *.nnw (Neural Network Weights), but system depends 
		on header format neither than file extension. */
		void loadFromFile(const char* filePath);
		
		/*! Save network weigts to file of given path. It is advised to keep
		naming convesion as *.nnw (Neural Network Weights), but system depends 
		on header format neither than file extension. */
		void saveToFile(const char* filePath);
			
	protected:
	
		virtual void initializeLayers( const Matrix& dimensions);
		
		/*! Number of layers in neural network */
		int layersNumber_; 
		
		/*! Input layer which serves as input for neural network. */
		Matrix  inputLayer_;
		
		/*! Dynamic array of pointers to layers of neurons. Operating on 
		pointers netheir than objects gives advantage of having the same 
		interface for all types of networks trough c++ inheritance system. */ 
		NeuronsLayer **layers_; 
		
	private:
	  
		class BadInput : public Exception 
			{ public: BadInput(const Matrix&, const Matrix&); };
		
		class FileNotOpen : public Exception 
			{ public: FileNotOpen(const char*); };
		
		class FileBadHeader : public Exception 
			{ public: FileBadHeader(const char*); };
	
};
//----------------------------------------------------------------------------//
inline int NeuralNetwork:: layersNumber() const
{
	return layersNumber_;
}
//----------------------------------------------------------------------------//

//!Back-Propagation Neural Network
/*! Network with error back-propagation learning algorithm. */
//----------------------------------------------------------------------------//
class BPNetwork : public NeuralNetwork
{
	public:
		BPNetwork();
		
		/*! Creates network with given geometry (number and sizes of network 
		layers). \param dimensions - matrix of size 1xN, where N-1 is number of
		 layers in NeuralNetwork. For i = 1 : N, dimensions[i] is number of 
		 neurons in i-th layer, dimension[0] is "input layer" size. */
		BPNetwork(const Matrix& dimensions);
		
		~BPNetwork();
		
		/*! Neurons in Back Propagation Network are nonlinear, and their output 
		function can be specified for all layers.
		\param function address of function to use, must be one from Matrix 
		static functions with one argument const Matrix&, and returning Matrix 
		object by value. sa setDerivFunction */		
		void setOutFunction(   Matrix (*function)( const Matrix&));
		
		/*! Learning alogrithm requires computation of derivative of output
		function, which can be selected in the same way as output function 
		itself. \sa setOutFunction */
		void setDerivFunction( Matrix (*function)( const Matrix&));
		
		/*! Computes and returns network output. */
		const Matrix& run();
		
		/*! Performs error (delta) back propagaation in network. Last layer 
		error is computed from diffrents between current and expected output,
		then pushed back to lower layers based on back propagation alogrithm 
		rules. */
		void computeDeltas(const Matrix& expectedOutput);
		
		/*! Correct neurons weigts in whole network on the basis of theirs 
		errors, each neuron input and output. */
		void correctWeights();
		
	protected:
	
		/*! Initializes network layers as BPLayers. */
		virtual void initializeLayers( const Matrix& dimensions);
		
	private:
};
//----------------------------------------------------------------------------//
inline void BPNetwork:: setOutFunction( Matrix (*function)( const Matrix&))
{
	for (int i = 0; i < layersNumber_; i++)
	{
		((BPLayer*) layers_[i])->setOutFunction( function);
	}
}
//----------------------------------------------------------------------------//
inline void BPNetwork:: setDerivFunction( Matrix (*function)( const Matrix&))
{
	for (int i = 0; i < layersNumber_; i++)
	{
		((BPLayer*) layers_[i])->setDerivFunction( function);
	}
}
//----------------------------------------------------------------------------//

//!Counter Propagation Neural Network
/*! Network consists of two neurons layers. First is competitive layer, where
neurons teach themselves to get the best fit of iput layer. Second is used to 
obtain desired output for learned input. */
//----------------------------------------------------------------------------//
class CPNetwork : public NeuralNetwork
{
	public:
		CPNetwork()
		;
		/*! Creates network of give sizes of input and both layers. */
		CPNetwork( int inputSize, int competitiveSize, int outstarSize);
		~CPNetwork();
		
		/*! \return size of first layer */
		int competitiveSize() const;
		
		/*! \return size of second layer */
		int outstarSize() const;
		
		/*! Performs normalization on input, so ||input|| = 1 */
		void normalizeInput();
		
		/*! Find neuron from first layer that fits the best to input. 
		\return index of winning neuron. */
		int findWinner();
		
		/*! Corrects winning neurons weights to get even better fit to input. */
		void learnWinner();

		/*! Learn second layer to answer with given pattern for current 
		input. \param pattern desired output of network */
		void learnPattern(const Matrix& pattern);
		
		/*! Computes all network neurons outputs for current input. */
		const Matrix& run();
		
	protected:
		virtual void initializeLayers( const Matrix& dimensions);
		int winner_; //winning neuron index
	private:		
};
//----------------------------------------------------------------------------//
inline int CPNetwork:: competitiveSize() const
{
	return layers_[0]->size();
}
//----------------------------------------------------------------------------//
inline int CPNetwork:: outstarSize() const
{
	return layers_[1]->size();
}
//----------------------------------------------------------------------------//
inline void CPNetwork:: normalizeInput()
{
	inputLayer_.normalize( 2 ); //normalize row vector
}
//----------------------------------------------------------------------------//
inline void CPNetwork:: learnWinner()
{
	((WTALayer*)layers_[0])->learn( inputLayer_);
}
//----------------------------------------------------------------------------//
inline void CPNetwork:: learnPattern(const Matrix& pattern)
{
	((Cpn2ndLayer*)layers_[1])->learn( winner_, pattern);
}
//----------------------------------------------------------------------------//
inline const Matrix& CPNetwork:: run()
{
	((WTALayer*)layers_[0])->compute( inputLayer_);
	winner_ = ((WTALayer*)layers_[0])->winner();
	return ((Cpn2ndLayer*)layers_[1])->compute( layers_[0]->output());
}
//----------------------------------------------------------------------------//

#endif //NEURAL_NETWORKS_H
