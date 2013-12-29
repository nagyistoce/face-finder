//----------------------------------------------------------------------------//
//                      MATRIX ARITHMETIC DEFINITIONS                         //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 3.52                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  3.52 - Improvements in reshape and toUchar() functions                    //
//         and new m_is_a_div_b and m_is_a_mul_b                // 27.10.2009 //
//	3.50 - Optimazing array data access by replacing                          //
//         calls to array operator [], which is not always                    //
//         one operation function, with address based access    // 09.10.2009 //
//  3.00 - CUBLAS optimalization (BLAS on NVIDIA CUDA)          // 02.10.2009 //
//  2.45 - BLAS optimalization                                  // 24.09.2009 //
//	1.95 - Modification of submatrix function to support                      //
//         data contiguosity in memory                          // 09.09.2009 //
//  1.90 - Reimplementation of member functions to return                     //
//         matrix objects by reference instead of by value      // 20.08.2009 // 
//	1.87 - Implementation of use-counted handles for matrix                   //
//          array except for normal pointers                    // 19.08.2009 //
//  1.12 - added functionality of shared data system (obsolete) // 20.05.2009 //
//  0.78 - toUChar, toPgmFormat and scaleFrom                   // 10.05.2009 // 
//  0.77 - functions for load from and save to CSV file         // 23.04.2009 // 
//  0.76 - statistical functions                                // 07.04.2009 //
//  0.75 - replaced some friend function with static ones       // 22.03.2009 //
//  0.74 - distance function                                    // 21.03.2009 //
//	0.73 - preprocesor switches for protection routines                       //
//         what gives safer/faster version                      // 20.03.2009 //
//  0.70 - using array DataType* instead of DataType**          // 18.03.2009 //
//  0.33 - NaN result protection                                // 16.03.2009 //
//  0.30 - math functions: sigmoid, dSigmoid, constant          // 15.03.2009 //
//  0.29 - casting to DataType, statistic function max, min     // 02.01.2009 //
//  0.27 - whole matrix set, operators: +=, -=, *=,                           //
//         switched arguments extension,                                      //
//         element-wise multiplication                          // 08.10.2008 //
//  0.24 - multiplication speed up functions: a_x_bt, at_x_b,                 //
//         and at_x_bt (where t is transposition),                            //
//         transpose function                                   // 04.10.2008 //
//  0.22 - expand function, setRandom function, resize                        //
//         modifications, streaming to/from file, remake                      //
//         exception handling to derives from std:: exception                 //
//         class trough Exception class                         // 03.10.2008 //
//  0.17 - constructor from string, random values constructor,                //
//         subMatrix function                                   // 27.09.2008 //
//  0.15 - exceptions handling, operator =, lenght() function   // 23.09.2008 //
//  0.11 - resize function, operator []                         // 21.09.2008 //
//  0.10 - basic matrix definition                              // 19.09.2008 //
//----------------------------------------------------------------------------//


#ifndef MATRIX_H
#define MATRIX_H


//Optimalization methods
//----------------------------------------------------------------------------//
//OPT_NONE
/* Use no optimalization libraries, only code from matrix in version 1.xx
Linking demands only standard libraries */
#define OPT_NONE 0
//----------------------------------------------------------------------------//
//OPT_BLAS
/* Use architecture optimalized function from BLAS library in most coputational 
extensive functions. BLAS need to be installed in operating system and its path
added to linker (dynamiccly is -lblas)
!!! Important !!!
When using BLAS functions all matrices must have the same order, best is byCols.
Diffrent orders cause undefined functions behaviors.
Orders aren't check in function calls, so must be checked manually or set ON
performance switch UNIFORM_ORDERS below. 
To biuld project add dynamic library -lblas to linker. */
#define OPT_BLAS 1
//----------------------------------------------------------------------------//
// OPT_CUDA
/* Use functions from CUBLAS library. This library has interface similar to 
BLAS and use GPU for computations. You must have NVIDIA graphic card from
8xxx series or above, and CUDA Tools installed in system. 
To make it work UNIFORM_ORDERS must be set ON and preffered order set to byCols. 
Because CUBLAS functions use GPU memory, DataType will be changed from simple
float type to GMArray<float> type which supports using of GPU memory.
To build project add dynamic library -lcublas to linker. */
#define OPT_CUDA 2
//----------------------------------------------------------------------------//


//Matrix optimalization method: choose one from listed above
//----------------------------------------------------------------------------//
#define OPTIMIZATION OPT_CUDA
//----------------------------------------------------------------------------//



//Perfomance switches
//----------------------------------------------------------------------------//
//If you want to use any available optimalizations above it's adviced to
//turn off most of the switches below except for UNIFORM_ORDERS
//ON (safer)
	#define ON 1 
//OFF (faster)
	#define OFF 0 
//----------------------------------------------------------------------------//
//NAN_PROTECTION
//Preserve generating or assgning NaN value to matrix
#define NAN_PROTECTION OFF
//----------------------------------------------------------------------------//
//INDEX_PROTECTION
//Validation of array indexes
#define INDEX_PROTECTION OFF
//----------------------------------------------------------------------------//
//SIZE_PROTECTION
/* Protection against too big arrays creation and 
using of matrix with wrong size for certain functions */
#define SIZE_PROTECTION OFF
//----------------------------------------------------------------------------//
//UNIFORM_ORDERS
/* Ensure that all matrix objects have the same data ordering (byCols or byRows)
determined by const static matrix class member prefferedOrder_. */
#define UNIFORM_ORDERS ON
//----------------------------------------------------------------------------//

//Type of data used 
//----------------------------------------------------------------------------//
#define FLOAT 0
#define DOUBLE 1
//DATA_TYPE 
/* Determines type of data used in matrix object. New type can be added, either 
builtin or user defined. Currently available are: FLOAT, DOUBLE*/
#define DATA_TYPE FLOAT
 
//includes
//----------------------------------------------------------------------------//
#include <iostream> //operator >> and <<  (matrix to/from stream)
#include <cfloat> //DBL_MIN, DBL_MAX macros
#include <cmath> //sqrt() in inline function std()
#include <stdlib.h> //RAND_MAX macro
#include "Exception.h" //for inheritance of matrix-specific exceptions
#include "CountedHandle.h" //for shared memory managment
#if OPTIMIZATION == OPT_BLAS
	#include <cblas.h> //BLAS C interface 
#endif
#if OPTIMIZATION == OPT_CUDA
	#include <cublas.h> //CUBLAS interface
	#include "GMArray.h" //Special type of data array
#else
	#include "Array.h" //Simple array interface
#endif
//----------------------------------------------------------------------------//



//Defining type of data for use in matrices
//----------------------------------------------------------------------------//
//! Type of data Matrix class will operate on (default: float)
#if DATA_TYPE == FLOAT
	typedef float DataType;
#elif DATA_TYPE == DOUBLE
	typedef double DataType;
// #elif DATA_TYPE ==
#else
	typedef float DataType;
#endif
//! Type for array of DataType
#if OPTIMIZATION == OPT_CUDA //specific type of data required
	typedef GMArray<DataType> DataArray;
#else
	typedef Array<DataType> DataArray;
#endif
/* !Important!
When using BLAS optimalization functions changes prefixes depends on data type
You should set it properly:
DataType		blas_
float			cblas_s
double			cblas_d
complex			cblas_c (check BLAS documentation for complex type infos) */
#define blas_copy cblas_scopy
#define blas_axpy cblas_saxpy
#define blas_scal cblas_sscal
#define blas_gemm cblas_sgemm
#define blas_nrm2 cblas_snrm2
#define blas_imax	cblas_isamax
//The same operation is required for CUBLAS library too
#define cublas_copy cublasScopy
#define cublas_axpy cublasSaxpy
#define cublas_scal cublasSscal
#define cublas_gemm cublasSgemm
#define cublas_nrm2 cublasSnrm2
#define cublas_imax	cublasIsamax
//----------------------------------------------------------------------------//



//----------------------------------------------------------------------------//
//! Matrices implementation
/*! Interface for two dimensional matrix arithmetic. Matrix data are stored 
in one dimensional array (with special data ordering). Provided functions are 
prepared for project faceFinder but most of them have general use. For 
compiling options see header file. */
class Matrix
{
	public:
	
		/*! Determine the way data are stored in array.
		Example:\n
		Assume we have matrix A of size 2x3:\n
		A = \n[ 1 2 3 ]\n
		      [ 4 5 6 ]\n
		A will be stored in array either in row order, which is 
		[ 1 2 3 4 5 6 ] or in column order, which is [ 1 4 2 5 3 6 ]. */
		enum ArrayOrder 
		{ 
			byRows = -1, //<! Matrix is saved by rows
		 	byCols = 1   //<! Matrix is saved by columns (preffered)
		 };
		
		Matrix();
		
		/*! Creates Matrix of given size. 
		\param rows number of rows
		\param cols number of columns */
		Matrix( int rows, int cols);
		
		/*! Creates matrix of given size and every element set to value.
		\param rows number of rows
		\param cols number of columns
		\param value value assigned to each matrix cell */
		Matrix( int rows, int cols, DataType value);
		
		/*! Creates matrix of shape and values given in special 
		formatted string.
		\param values string with space separated values, where first two 
		determine size of matrix and following are values read in row 
		order. */
		Matrix( const string& values);
		
		/*! Creates matrix of given size and values distributed in given range.
		\param rows number of rows
		\param cols number of columns
		\param min minimum matrix value
		\param max maximum matrix value */
		Matrix( int rows, int cols, DataType min, DataType max);
		
		/*! Creates row vector consisting only of "0" and "1" values, which
		is binary representation of given number.
		\param binary positive number for binary reprasantion
		\param digits number of digits (bits) to represent given number. If
		not specified matrix will have the smallest, possible size. 
		If specified should be explicitly declared as unsigned char, 
		otherwise compiler will call Matrix( int, int) constructor! */
		Matrix( unsigned short int binary, unsigned char digits = 0);
		
		/*! Creates matrix of given size and data in external array. 
		Warning! Assigning matrix with external array may cause memory
		errors and therefore should be avoided whenever its possible
		\param rows number of rows
		\param cols number of columns
		\param data pointer to array with matrix data
		\param order order of data in array */
		Matrix( int rows, int cols, DataType * data, ArrayOrder order = byCols); 
		
		/*! Creates exact copy of given matrix with seperated memory space. */
		Matrix( const Matrix& m);
		
		/*! Creates matrix as copy of submatrix of given matrix.
		\param m matrix to copy data from
		\param r1 beginning row
		\param r2 ending row
		\param c1 beginning column
		\param c2 ending column */
		Matrix( const Matrix& m, int r1, int r2, int c1, int c2); 
		
		~Matrix();
		
		/*! If CUDA optimization is turn on this will tell the GMArray object 
		to use only CPU moemory, cause no GPU operations will be executed on 
		this matrix. If Matrix dont use CUDA optimization that function 
		does nothing.
		\param cpuOnly true for usage of only cpu memory, false to restore 
		default dual memory usage. */
		void useCpuOnly( bool cpuOnly ); 
		
		/*! Copy contest of source matrix. Unstead of simply sharing source data
		 array with CountedHandle mechanism it allocates new data space in 
		memory and copy all source values there. Function is usefull when we 
		want to modify copied matrix without altering original. */
		void copy( const Matrix& source);

		/*!Copies values from char data buffer. Proper scalling is implemented. 
		Currently reads only 1-byte data from buffer. Its low level operation!
		Data are supposed to be stored in byRows order and theirs size must 
		matched matrix size.*/ 
		void copyBuffer( const char * buffer);
		
		/*! Read access to rows_ \sa rows_ */
		int rows() const;
		
		/*! Read access to cols_ \sa cols_ */
		int cols() const;
		
		/*! Read access to length_ \sa length_ */
		int length() const;
		
		/*! Read access to order_ \sa order_ */
		int order() const;
		
		/*! String description of cuurent order_ \sa order_*/
		string orderDescription() const;
		
		/*! Matrix description of preffered order. \sa prefferedOrder_ */
		static ArrayOrder prefferedOrder();
		
		/*! String description of preffered array order \sa prefferedOrder_ */
		static string prefferedOrderDescription();
		
		/*! Returns addreess of first element of matrix. Can be used for calling
		functions operating on data pointers like e.g. BLAS library.
		Warning! This function allow direct access to data array what takes off 
		class member protection and may cause memory errors if used not 
		properly. Therefore shouldnt be used in most cases.
		\return pointer to matrix data array (phisical data begining) */		
		DataType * address();
		const DataType* address() const;
	
		/* Returns true if matrix is row or column vector*/
		bool isVector() const;
		
		/*! Function checks if any of matrix values is NaN (Not a Number).
		\return true if NaN occured, false otherwise */
		bool isNan() const;

		/*! Checks if all matrix values	are equal to values of given m matrix.
		\return true if all values matched, flase otherwise */
		bool equalTo( const Matrix& m) const;

		/*! Changes matrix order. If matrix doesnt have given order it would be 
		changed and data will be rewrite to new array with changed order.
		\return changed Matrix object (*this reference) */
		Matrix& reorder(ArrayOrder order);
		
		/*! Changes matrix shape. New data array is created according to given 
		rows and cols. Important! Data array length must be unchanged, which 
		means rows * cols must be equal to matrix current rows_ * cols_
		\param rows new number of rows
		\param cols new number of columns
		\param reshapingOrder detrmine the way matrix is reshaped, default 
		is byCols e.g. we have matrix 2x6:\n 
		11 12 13 14 15 16\n 21 22 23 24 25 26\n 
		and we reshape it to matrix 4x3, byCols is:\n
		11 13 15\n 21 23 25\n 12 14 16\n 22 24 26\n
		byRows is:\n 11 12 13\n 14 15 16\n 21 22 23\n 24 25 26\n
		\return changed Matrix object (*this reference) */
		Matrix& reshape(int rows, int cols, ArrayOrder reshapingOrder = byCols);
		
		/*! Binary represented value.
		\return binary value represented by vector which all cells are "0"
		or "1" */
		unsigned int binary() const; 

		/*! Elements access operator. Works only with vector matrices, throw an 
		expception otherwise
		\param index index of element to access
		\return reference to value at given index 
        \sa cell */
		DataType& operator [] (int index);
		const DataType& operator [] (int index) const;
		
		/*! Matrix elements access. Returns reference to matrix element at 
		given position, providing read and write access.
		\param r row index
		\param c column index
		\return specified matrix element.
		\sa [] */
		DataType& cell( int r, int c);
		
		/*! Overloaded function for const matrix objects read-only elements 
		access. */
		const DataType& cell( int r, int c) const;
		
		/*! Sets every element of matrix to given value.
		\param value value to set
		\sa setRow, setCol */
		void set( DataType value);
		
		/*! Copies values of given vector to specified row of matrix
		Sizes must match or an exception will be thrown.
		\param r row index
		\param v vector to copy from
		\sa set, setCol, setRandom */
		void setRow( int r, const Matrix& v);
		
		/*! Copies values of given vector to specified column of matrix
		Sizes must match or an exception will be thrown.
		\param c column index
		\param v vector to copy from
		\sa set, setRow, setRandom */
		void setCol( int c, const Matrix& v);
		
		/*! Changes size of matrix. All previous data are lost.
		\param rows new number of rows
		\param cols new number of columns
		\param forceResize resize even if size already matched and therefore 
		always allocate new memory space for matrix data (default is false).
		\sa expand */  
		void resize( int rows, int cols, bool forceResize = false);
		
		/*! Changes size of matrix and sets every elemnt to given value. 
		All previous data are lost.
		\param rows new number of rows
		\param cols new number of columns
		\param value value to set
		\param forceResize resize even if size already matched and therefore 
		always allocate new memory space for matrix data (default is false).
		\sa expand */ 
		void resize( int rows, int cols, DataType value, bool forceResize = false);
		
		/*! Changes size of matrix and sets elements to random values from
		specified range. All previous data are lost.
		\param rows new number of rows
		\param cols new number of columns
		\param min minimum matrix value
		\param max maximum matrix value
		\param forceResize resize even if size already matched and therefore 
		always allocate new memory space for matrix data (default is false).
		\sa expand */  
		void resize( int rows, int cols, DataType min, DataType max, 
		             bool forceResize = false);
		
		/*! Increases number of rows and/or columns and preserve all previous
		marix values.
		\param rowIncrease number of row to add to matrix
		\param colIncrease number of columns to add to matrix
		\sa resize */
		void expand( int rowIncrease, int colIncrease);
		 
		/*! Resize matrix and makes copy of given source matrix, which is the
		same size or larger by integral ratio. Effect similar to shrinking 
		image.
		\param src matrix to copy from
		\param rows new number of rows
		\param cols new number of columns */
		Matrix& scaleFrom( const Matrix src, int rows, int cols);
		
		/*! Sets matrix elements with random values from given range.
		\param min minimum matrix value
		\param max maximum matrix value
		\sa set, setRow, setCol */
		void setRandom( DataType min, DataType max);
		
		/*! Generates random vectors of given properties to fill rows or 
		columns	of matrix.
		\param avg vector representing average (mean) values for each vector
		element 
		\param std vector representing standard devation values for each vector 
		element
		\param dimension determines whether generated vectors are put into rows 
		or into columns of matrix. Posible values:\n
		1 - puts vectors into columns (default)\n 2 - puts vectors into rows
		\sa setRandom */
		void randomVectors( const Matrix& avg, const Matrix& std, 
		                    int dimension = 1);
		
		/*! Loads matrix data from given CSV file.
		\param csvFilePath path of file to load from */
		void loadFromCsv( const char * csvFilePath);
		
		/*! Stores matrix data into given file using CSV format.
		\param csvFilePath path of file to write to */
		void saveToCsv( const char * csvFilePath) const;
		
		/*! Converts matrix data to unsigned char format, which can be use 
		e.g. for graphic purposes in the fastest way. Image can be reshaped 
		in order of data represantation (e.g if data are stored byCols reshape 
		will be performed also byCols). Data in buffer are stored always 
		in byRows order.
		\param width if given, determine image width, otherwise will be number 
		of matrix columns 
		\param height if given, determine image height, otherwise will be number 
		of matrix rows
		\param min minimum value for contrast scaling, if none it will be 
		taken from matrix properties
		\param max maximum value for contrast scaling, if none it will be 
		taken from matrix properties
		\return pointer to buffer with unsigned char values which can be
		interpreted as raw image of data in grayscale 
		\sa toPgmFormat, reshape */ 
		unsigned char* toUchar(int width = 0 , int height = 0, 
		                       DataType min = 0, DataType max = 0) const;
		
		/*! Converts matrix data to PGM format (PGM header and unsigned char 
		data), making graphical representation of matrix. Dimension of 
		picture and contrast can be set automaticly or manually. Due to more 
		sophisticated way of matrix reshaping function is slower than toUchar().
		But it can be used to directly obtain data ready to write to file.
		\param width width of PGM picture, if none specified current matrix 
		dimension will be used
		\param height height of PGM picture, if none specified current 
		matrix dimension will be used
		\param min minimum value for contrast scaling, if none it will be 
		taken from matrix properties
		\param max maximum value for contrast scaling, if none it will be 
		taken from matrix properties
		\param reshapingOrder if data should be reshaped this will determine
		order of reshaping (more info in reshape function description)
		\return pointer to buffer with PGM data (and updates charLength_ 
		property which should be use for providing buffer length)
		\sa toUchar, reshape*/
		unsigned char* toPgmFormat( int width = 0 , int height = 0, 
		                            DataType min = 0, DataType max = 0,
		                            ArrayOrder reshapingOrder = byCols) const;
		
		/* \return current length of buffer of unsigned char values returnued 
		by functions: toUchar and toPgmFormat.
		\sa toUchar, toPgmFormat */			
		int charLength() const;
		
		/*! \return maximal value encountered in whole matrix
		\sa maxIndex, min, absMax, absMin, sum, avg, var, std */
		DataType max() const;
		
		/*! \return index of first occurance of maximal value in whole matrix
		\sa max, min, absMax, absMin, sum, avg, var, std */
		int maxIndex() const;
		
		/*! \return minimal value encountered in whole matrix
		\sa minIndex, max, absMax, absMin, sum, avg, var, std */
		DataType min() const;
		
		/*! \return index of first occurance of minimal value in whole matrix
		\sa max, min, absMax, absMin, sum, avg, var, std*/
		int minIndex() const;
		
		/*! \return maximal absolute value encountered in whole matrix 
		\sa max, min, absMaxIndex, absMin, sum, avg, var, std*/
		DataType absMax() const;
		
		/*! \return index of first occurance of maximum absolute value
		\sa max, min, absMax, absMin, sum, avg, var, std*/
		int absMaxIndex() const;
		
		/*! \return minimal absolute value encoutered in whole matrix
		\sa max, min, absMax, absMinIndex, sum, avg, var, std */
		DataType absMin() const;
		
		/*! \return index of first occurance of minimal absolute value
		\sa max, min, absMax, absMin, sum, avg, var, std */
		int absMinIndex() const; 
		
		/*! \return sum of all values, that matrix contains
		\sa max, min, absMax, absMin, avg, var, std */
		DataType sum() const;
		
		/*! \return arithmetic average, avg = sum(values) / length
		\sa max, min, absMax, absMin, sum, var, std */
		DataType avg() const;
		
		/*! \return variation = sum(values - avg))^2 / length
		\sa max, min, absMax, absMin, sum, avg, std */
		DataType var() const;
		
		/*! \return std = sqrt( var )
		\sa max, min, absMax, absMin, sum, avg, var */
		DataType std() const;
		
		/*! Computes norm of vector. Throw exception if call on non-vector.
		\return euclidian norm of vector:\n Euclidian norm is root from sum
		of all elements squars ||X|| = sqrt( x1^2 + x2^2 + ... + xn^2) */
		DataType norm() const;
		
		/*! Normalizes matrix values for each row or column as a seperate vector.
		\param dimension determines direction of noramlization. Possible 
		values: 1 - normalize columns (default)\n 2 - normalize rows
		\return normalized matrix
		\sa norm */
		Matrix& normalize( int dimension = 1);
		
		/*! Returns copy of specified submatrix of current matrix. 
		\param r1 row index of submatrix begining
		\param r2 row index of submatrix ending
		\param c1 column index of submatrix begining
		\param c2 column index of submatrix ending
		Rows and columns are indexed from 0.
		\return new matrix object with values taken from current matrix 
		starting from element (r1,c1) and ends on element (r2,c2). 
		\sa row, col, copy */
		Matrix subMatrix( int r1, int r2, int c1, int c2);

		/*! \param index 0-based index of matrix row
		\return copy of matrix row
		\sa col, subMatrix */
		Matrix row( int index);
		
		/*! \param index 0-based index of matrix column
		\return copy of matrix column
		\sa col, subMatrix */
		Matrix col( int index);
		
		/*! Quick transposition of current matrix by affecting array order 
		instead of data array. When UNIFORM_ORDERS is set on this can be
		performed only on vectors, becuase they don't need order switching.
		\sa transpose */
		Matrix& T();

		/*! Converts matrix 1x1 to DataType object. Doesn't checks matrix size,
		simply returning first element of matrix and therefore must be use with 
		caution to matrix size). */
		operator DataType() const;

		/*! Because counted handle system is used for data managment, assinging 
		cause no data copy, what makes this function time constant and efficient.
		One can't forget that assigned matrix shares memory with source matrix
		and assigning to non-temporary objects may be risky e.g.\n
		Matrix a, b, c;\n
		... // some initialization\n
		a = b + c; //thats ok, safe data copy from temporary object\n
		a = b; //a do not copy from b, its share the same memory */
		Matrix& operator  = (const Matrix&);		
		
		/*! Writes textual represantion of matrix to given stream. */
		friend ostream& operator << (ostream&, const Matrix&);
		
		/*! Saves matrix properties and values to an opened file stream. */
		friend fstream& operator << (fstream&, const Matrix&);
		
		/*! Loads matrix properties and values from an opened file stream. */
		friend Matrix&  operator >> (fstream&, Matrix&);

		Matrix& operator += (const Matrix& b);
		friend Matrix operator +  (const Matrix& a, const Matrix& b);
		Matrix&       operator += (const DataType& b);
		friend Matrix operator +  (const Matrix&   a, const DataType& b);
		friend Matrix operator +  (const DataType& a, const Matrix&   b);
		
		Matrix&       operator -= (const Matrix& b);
		friend Matrix operator -  (const Matrix& a, const Matrix& b);
		Matrix&       operator -= (const DataType& b);
		friend Matrix operator -  (const Matrix&   a, const DataType& b);
		friend Matrix operator -  (const DataType& a, const Matrix&   b);

		friend Matrix operator *  (const Matrix& a, const Matrix& b);
		Matrix&       operator *= (const DataType& b);
		friend Matrix operator *  (const Matrix&   a, const DataType& b);
		friend Matrix operator *  (const DataType& a, const Matrix&   b);
		
		Matrix&       operator /= (const DataType& b);
		friend Matrix operator /  (const Matrix&   a, const DataType& b);	

		friend bool operator == (const Matrix& a, const Matrix& b);
		friend bool operator != (const Matrix& a, const Matrix& b);
		
		/*! Makes matrix object which is transposition of given matrix.
		\param m matrix to transpose
		\return new matrix which is transposition of m matrix
		\sa T */
		static Matrix transpose( const Matrix& m);

		/*!	Performs the fastest possible addition of matrices a and b with
		result stored in matrix m. Use this function instead of matrix + 
		operator if execution speed is more important than code clarity. */
		static Matrix& m_is_a_plus_b( Matrix& m, 
		                              const Matrix& a, const Matrix& b);

		/*!	Performs the fastest possible addition of matrix a and scalar b with
		result stored in matrix m. Use this function instead of matrix + 
		operator if execution speed is more important than code clarity. */
		static Matrix& m_is_a_plus_b( Matrix& m, 
		                              const Matrix& a, const DataType& b);
		
		/*!	Performs the fastest possible subtraction of matrices a and b with
		result stored in matrix m. Use this function instead of matrix - 
		operator if execution speed is more important than code clarity. */
		static Matrix& m_is_a_minus_b( Matrix& m, 
		                               const Matrix& a, const Matrix& b);		 

		/*! Performs the fastest possible subtraction of matrix a and scalar 
		b with result stored in matrix m. Use this function instead of matrix - 
		operator if execution speed is more important than code clarity. */
		static Matrix& m_is_a_minus_b( Matrix& m, 
		                               const Matrix& a, const DataType& b);

		/*!	Performs the fastest possible division of matrix a by scalar b with
		result stored in matrix m. Use this function instead of matrix / 
		operator if execution speed is more important than code clarity. */
		static Matrix& m_is_a_div_b( Matrix& m, 
		                             const Matrix& a, const DataType& b);

		/*!	Performs the fastest possible element-wise division of matrix a by 
		matrix b with result stored in matrix m. */
		static Matrix& m_is_a_div_b( Matrix& m, 
		                             const Matrix& a, const Matrix& b);

		/*!	Performs the fastest possible element-wise multiplication of 
		matrix a by	matrix b with result stored in matrix m. */
		static Matrix& m_is_a_mul_b( Matrix& m, 
		                             const Matrix& a, const Matrix& b);
		                             
		/*!	Performs the fastest possible multiplication of matrix a and 
		scalar b with result stored in matrix m. Use this function instead of 
		matrix * operator if execution speed is more important than code 
		clarity. */
		static Matrix& m_is_a_x_b( Matrix& m, 
		                           const Matrix& a, const DataType& b);
		
		/*! Performs the fastest possible multiplication of matrices a and b 
		with result stored in matrix m. Use this function instead of matrix * 
		operator if execution speed is more important than code	clarity. */
		static Matrix& m_is_a_x_b  ( Matrix& m, 
		                             const Matrix& a, const Matrix& b);

		/*! Performs the fastest possible multiplication of matrices 
		a(transposed) and b with result stored in matrix m. Use this function 
		instead of matrix *	operator if execution speed is more important 
		than code clarity. */
		static Matrix& m_is_aT_x_b ( Matrix& m, 
		                             const Matrix& a, const Matrix& b);

		/*! Performs the fastest possible multiplication of matrices 
		a and b(transposed) with result stored in matrix m. Use this function 
		instead of matrix *	operator if execution speed is more important 
		than code clarity. */
		static Matrix& m_is_a_x_bT ( Matrix& m, 
		                             const Matrix& a, const Matrix& b);

		/*! Performs the fastest possible multiplication of matrices 
		a(transposed) and b(transposed) with result stored in matrix m. Use 
		this function instead of matrix * operator if execution speed is more 
		important than code clarity. */
		static Matrix& m_is_aT_x_bT( Matrix& m, 
		                             const Matrix& a, const Matrix& b);

		/*! Computes vectors distances in N-dimensional space.
		distance = sqrt ((a(0) - b(0)) ^ 2 + ... + (a(N-1) - b(N-1)) ^ 2)
		works on matrix like multipplication algorithm, so
		for each i,j  result(i,j) = distance(a.row(i), b(col(j))) */
		static Matrix distance (const Matrix& a, const Matrix& b);
		
		/*! Argument return. Functions acts as f(x)=x for each matrix element.
		Function was created for compabilty with rest of element-wise functions.
		\return unchanged matrix x (by value) 
		\sa constant, step, perceptron, sign, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix passValue(const Matrix& m);
		
		/*! Constant return. Function was created for compabilty with rest of 
		element-wise functions. 
		\return matrix of the same size as m and all values set to one.
		\sa passValue, step, perceptron, sign, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix constant(const Matrix& m);
		
		/*! Element-wise function. step(x) =\n 0 if x <= 0\n 1 if x > 0 
		\sa passValue, constant, perceptron, sign, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix step(const Matrix& m);
		
		/*! Element-wise function. perceptron(x) =\n x if x > 0\n 0 if x <= 0
		\sa passValue, constant, step, sign, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix perceptron(const Matrix& m);
		
		/*! Element-wise function. 
		sign(x) =\n -1 if x < 0\n 0 if x == 0\n 1 if x > 0
		\sa passValue, constant, step, perceptron, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix sign(const Matrix& m);
		
		/*! Element-wise function. 
		bsb(x) =\n -1 if x < -1\n x if -1 <= x <= 1\n 1 if x > 1
		\sa passValue, constant, step, perceptron, sign,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix brainStateBox(const Matrix& m);
		
		/*! Constant for step function bias. */
		static const DataType stepBias;
		
		/*! Element-wise function. 
		biasedStep(x) =\n 0 if x <= stepBias\n 1 if x > stepBias
		\sa passValue, constant, step, perceptron, sign, brainStateBox,
		winnerTakesAll, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix biasedStep(const Matrix& m);
		
		/*! Element-wise function. 
		wta(x) =\n 1 if x is maximal of all values\n 0 otherwise
		\sa passValue, constant, step, perceptron, sign, brainStateBox,
		biasedStep, sigmoid, dSigmoid, tanh, dTanh */
		static Matrix winnerTakesAll( const Matrix& m);
		
		/*! Coefficient for functions sigmoid and tanh */
		static const DataType shapeRatio; 
		
		/*! Element-wise function. 
		sigmoid(x) = 1 / (1 + exp( -b * x)), b = shapeRatio 
		\sa passValue, constant, step, perceptron, sign, brainStateBox,
		biasedStep, winnerTakesAll, dSigmoid, tanh, dTanh */
		static Matrix sigmoid( const Matrix& m);

		/*! Element-wise function. dSigmoid(x) = d/dx sigmoid(x)
		\sa passValue, constant, step, perceptron, sign, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, tanh, dTanh */		
		static Matrix dSigmoid( const Matrix& m); 
		
		/*! Element-wise function. 
		tanh(x) =\n (exp( b * x) - exp( -b * x)) /\n 
		(exp( b * x) + exp( -b * x)), b = shapeRatio
		\sa passValue, constant, step, perceptron, sign, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, dTanh */		
		static Matrix tanh( const Matrix& m);
		
		//! Element-wise derivative of tanh
		/*! Element-wise function. dTanh(x) = d/dx tanh(x)
		\sa passValue, constant, step, perceptron, sign, brainStateBox,
		biasedStep, winnerTakesAll, sigmoid, dSigmoid, tanh */
		static Matrix dTanh( const Matrix& m);

	protected:
		
		/*! Number of matrix rows. */
		int rows_; 
		
		/*! Number of matrix columns. */
		int cols_;
		
		/*! length of matrix data array. length = rows * cols. */
		int length_;
		
		/*! Matrix data array. Memory is managed by CountedHandle system. */
		CountedHandle<DataArray> matrix_;
		
		/*! Matrix data array order. \sa ArrayOrder*/
		int order_; 
		
		/*! Distance between rows in data array.
		rs_  = order == byRows ? 1 : rows_ */
		int rs_; 
		
		/*! Distance between columns in data array
		cs_ = order == byCols ? 1 : cols_ */
		int cs_;
		
		/*! If UNIFORM_ORDERS macro is 
		set on (perfomance switches section in matrix.h) all matrix object 
		would be force to use preffered order of 
		data storing in memory. */
		static const ArrayOrder prefferedOrder_;
  	
  		/*! Buffer for converting matrix to array of char (e.g. images). */
		mutable unsigned char* charData_;
		
		/*! Lenght of charData_ buffer.*/
		mutable int charLength_;
		
	private:
		
		/*! \return string description of givem ArrayOrder value. */
		static string arrayOrderToString( ArrayOrder order);

		//! Matrix exception
		/*! This exception is thrown when index is out of range. */
		class IndexOut : public Exception 
			{ public: IndexOut(int, int); 
			          IndexOut(int, int, int, int); };
		
		//! Matrix exception	          
		/*! This exception is thrown when number of rows exceeds acceptable value. */ 
		class RowConstraint : public Exception 
			{ public: RowConstraint( int); };
		
		//! Matrix exception
		/*! This exception is thrown when number of columns exceeds acceptable value. */
		class ColConstraint : public Exception 
			{ public: ColConstraint( int); };
		
		//! Matrix exception
		/*! This exception is thrown when current matrix object is not a vector. */
		class NotVector : public Exception 
			{ public: NotVector(const char*); };
		
		//! Matrix exception
		/*! This exception is thrown when bad parameter is given in function call. */
		class WrongParameter : public Exception 
			{ public: WrongParameter( const char*, const char*); };

		//! Matrix exception
		/*! This exception is thrown when invalid submatrix is selected for operation. */
		class BadSelection : public Exception 
			{ public: BadSelection(int, int, int, int, int, int); };

		//! Matrix exception
		/*! This exception is thrown when sizes of matrices don't match for the operation. */
		class SizeMismatch : public Exception 
			{ public: SizeMismatch(const Matrix&, const Matrix&, const char*); };

		//! Matrix exception
		/*! This exception is thrown when attempt to assign NaN value. */
		class NanAssign : public Exception 
			{ public : NanAssign( const char*); };

		//! Matrix exception
		/*! This exception is thrown when function operand is NaN value. */
		class NanOperand : public Exception 
			{ public : NanOperand( const char*); };

		//! Matrix exception
		/*! This exception is thrown when matrix can't be reshaped. */
		class CantReshape: public Exception 
			{ public: CantReshape(int, int, int, int); };

		//! Matrix exception
		/*! This exception is thrown when matrix can't be scaled. */
		class CantScale : public Exception 
			{ public: CantScale(int, int, int, int); };

		//! Matrix exception
		/*! This exception is thrown when set wrong value for OPTIMIZATION macro. */
		class UnrecognizedOptimization : public Exception 
			{ public: UnrecognizedOptimization(); };

		//! Matrix exception
		/*! This exception is thrown when set wrong value for UNIFORM_ORDERS macro. */	
		class UniformOrders : public Exception
			{ public: UniformOrders( const char*); };

		/*! Maximum number of rows that matrix can contain. */
		static const int maxRows_;

		/*! Maximum number of columns that matrix can contain. */
		static const int maxCols_;

		//! scalar functions used by matrix
		class Scalar
		{
			public:
				/*! Sigmoid function */
				static DataType sigmoid( DataType b, DataType x);
				/*! Hiperbolic tanges function */ 
				static DataType tanh   ( DataType b, DataType x);
				/*! Random value from [0,1] */
				static DataType rand();
				/*! Random value from [-1,1] */
				static DataType randn();
				static const DataType absMaxValue;
				static const DataType absMinValue;
				static const DataType pi;
			private:
				static int randnIsReady_;
				static DataType randnValue_;
		};

};
//----------------------------------------------------------------------------//


//Inlines
//----------------------------------------------------------------------------//
#if OPTIMIZATION == OPT_CUDA
inline void Matrix:: useCpuOnly( bool cpuOnly )
{
	matrix_->useGpuMemory( !cpuOnly);
}
#else
inline void Matrix:: useCpuOnly( bool /* cpuOnly */ )
{
	/* Class can use only CPU memory*/
}
#endif
//----------------------------------------------------------------------------//
inline int Matrix:: rows() const
{
	return rows_;
}
//----------------------------------------------------------------------------//
inline int Matrix:: cols() const
{
	return cols_;
}
//----------------------------------------------------------------------------//
inline int Matrix:: length() const
{
#if SIZE_PROTECTION
	if (rows_ > 1 && cols_ > 1) NotVector("length()");
#endif
	return length_;
}
//----------------------------------------------------------------------------//
inline DataType* Matrix:: address()
{
	return &(*matrix_);
}
//----------------------------------------------------------------------------//
inline const DataType* Matrix:: address() const
{
	return &(*matrix_);
}
//----------------------------------------------------------------------------//
inline bool Matrix:: isVector() const
{
	return rows_ == 1 || cols_ == 1;
}
//----------------------------------------------------------------------------//
inline int Matrix:: order() const
{
	return order_;
}
//----------------------------------------------------------------------------//
inline string Matrix:: orderDescription() const
{	
	return arrayOrderToString( (ArrayOrder)order_);
}
//----------------------------------------------------------------------------//
inline Matrix::ArrayOrder Matrix:: prefferedOrder()
{
	return prefferedOrder_;
}
//----------------------------------------------------------------------------//
inline string Matrix:: prefferedOrderDescription()
{
	return arrayOrderToString( prefferedOrder_);
}
//----------------------------------------------------------------------------//
inline DataType& Matrix:: operator [] (int index)
{
#if SIZE_PROTECTION
	if (rows_ > 1 && cols_ > 1) 
		throw NotVector("operator []");
#endif
#if INDEX_PROTECTION
	if (index < 0 || index >= length_) 
		throw IndexOut( index, length_);
#endif
	return (*matrix_)[index];
}
//----------------------------------------------------------------------------//
inline const DataType& Matrix:: operator [] (int index) const
{
#if SIZE_PROTECTION
	if (rows_ > 1 && cols_ > 1) 
		throw NotVector("operator []");
#endif
#if INDEX_PROTECTION
	if (index < 0 || index >= length_) 
		throw IndexOut( index, length_);
#endif
	return (*matrix_)[index];
}
//----------------------------------------------------------------------------//
inline DataType& Matrix:: cell( int r, int c)
{
#if INDEX_PROTECTION
	if (r < 0 || c < 0 || r >= rows_ || c >= cols_) 
		throw IndexOut(r, c, rows_, cols_);
#endif
	return (*matrix_)[r * cs_ + c * rs_];
}
//----------------------------------------------------------------------------//
inline const DataType& Matrix:: cell( int r, int c) const
{
#if INDEX_PROTECTION
	if (r < 0 || c < 0 || r >= rows_ || c >= cols_) 
		throw IndexOut(r, c, rows_, cols_);
#endif
	return (*matrix_)[r * cs_ + c * rs_];
}
//----------------------------------------------------------------------------//
inline Matrix Matrix:: subMatrix( int r1, int r2, int c1, int c2)
{
	return Matrix(*this, r1, r2, c1, c2);
}
//----------------------------------------------------------------------------//
inline Matrix Matrix:: row( int index)
{
	return subMatrix( index, index, 0, cols_ - 1);
}
//----------------------------------------------------------------------------//
inline Matrix Matrix:: col( int index)
{
	return subMatrix(0, rows_ - 1, index, index);
}
//----------------------------------------------------------------------------//
inline int Matrix:: charLength() const
{
	return charLength_;
}
//----------------------------------------------------------------------------//
inline DataType Matrix:: avg() const
{
	return sum() / length_;
}
//----------------------------------------------------------------------------//
inline DataType Matrix:: std() const
{
	return sqrt( var());
}
//----------------------------------------------------------------------------//
inline Matrix:: operator DataType() const
{
	//if matrix is other than 1x1 that conversion makes no sense
	return (*matrix_)[0]; //return first element (assuming the only one)
}
//----------------------------------------------------------------------------//
inline Matrix& Matrix:: operator += (const Matrix& b)
{
	return m_is_a_plus_b( *this, *this, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator + (const Matrix& a, const Matrix& b)
{
	Matrix m;
	return Matrix::m_is_a_plus_b( m, a, b);
}
//----------------------------------------------------------------------------//
inline Matrix& Matrix:: operator += (const DataType& b)
{ 
	return m_is_a_plus_b( *this, *this, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator + (const Matrix& a, const DataType& b)
{
	Matrix m;
	return Matrix::m_is_a_plus_b( m, a, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator + (const DataType& a, const Matrix& b)
{
	Matrix m;
	return Matrix::m_is_a_plus_b( m, b, a);
}
//----------------------------------------------------------------------------//
inline Matrix& Matrix:: operator -= (const Matrix& b)
{
	return m_is_a_minus_b( *this, *this, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator - (const Matrix& a, const Matrix& b)
{
	Matrix m;
	return Matrix::m_is_a_minus_b( m, a, b);
}
//----------------------------------------------------------------------------//
inline Matrix& Matrix:: operator -= (const DataType& b)
{
	return m_is_a_minus_b( *this, *this, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator - (const Matrix& a, const DataType& b)
{
	Matrix m;
	return Matrix::m_is_a_minus_b( m, a, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator - (const DataType& a, const Matrix& b)
{
	Matrix m;
	return Matrix::m_is_a_plus_b( m, Matrix::m_is_a_x_b( m, b, -1.0), a);
}
//----------------------------------------------------------------------------//
inline Matrix operator * (const Matrix& a, const Matrix& b)
{
	Matrix m;
	return Matrix::m_is_a_x_b( m, a, b);
}
//----------------------------------------------------------------------------//
inline Matrix& Matrix:: operator *= (const DataType& b)
{
	return m_is_a_x_b( *this, *this, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator * (const Matrix& a, const DataType& b)
{
	Matrix m;
	return Matrix::m_is_a_x_b( m, a, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator * (const DataType& a, const Matrix& b)
{
	Matrix m;
	return Matrix::m_is_a_x_b( m, b, a);
}
//----------------------------------------------------------------------------//
inline Matrix& Matrix:: operator /= (const DataType& b)
{
	return m_is_a_div_b( *this, *this, b);
}
//----------------------------------------------------------------------------//
inline Matrix operator / (const Matrix& a, const DataType& b)
{
	Matrix m;
	return Matrix::m_is_a_div_b( m, a, b);
}
//----------------------------------------------------------------------------//
inline bool operator == (const Matrix& a, const Matrix& b)
{
	// if points to the same memory assume a is equal to b
	// otherwise if dimensions matched perform full equality test
	return (a.matrix_ == b.matrix_) ||
	       (a.rows_ == b.rows_ && a.cols_ == b.cols_ && a.equalTo( b));
}
//----------------------------------------------------------------------------//
inline bool operator != (const Matrix& a, const Matrix& b)
{
	// !(a == b)
	return (a.matrix_ != b.matrix_) &&
	       (a.rows_ == b.rows_ || a.cols_ != b.cols_ || !a.equalTo( b));
}
//----------------------------------------------------------------------------//
inline DataType Matrix:: Scalar:: sigmoid( DataType b, DataType x)
{
 return 1 / (1 + exp( -b * x));
}
//----------------------------------------------------------------------------//
inline DataType Matrix:: Scalar:: tanh( DataType b, DataType x)
{
 return (exp( b * x) - exp( -b * x)) / (exp( b * x) + exp( -b * x));
}
//----------------------------------------------------------------------------//
inline DataType Matrix:: Scalar:: rand()
{
	return (DataType) ::rand() / RAND_MAX;
}
//----------------------------------------------------------------------------//
 
#endif //MATRIX_H
