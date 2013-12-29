#include "matrix.h"
#include <fstream>
#include <sstream>

//----------------------------------------------------------------------------//
const Matrix::ArrayOrder Matrix:: prefferedOrder_ = Matrix::byCols;
const int Matrix:: maxRows_ = 16384;
const int Matrix:: maxCols_ = 16384;
#if DATA_TYPE == FLOAT
	const DataType Matrix:: Scalar:: absMaxValue = FLT_MAX;
	const DataType Matrix:: Scalar:: absMinValue = FLT_MIN;
#elif DATA_TYPE == DOUBLE
	const DataType Matrix:: Scalar:: absMaxValue = DBL_MAX;
	const DataType Matrix:: Scalar:: absMinValue = DBL_MIN;
#else
	const DataType Matrix:: Scalar:: absMaxValue = FLT_MAX;
	const DataType Matrix:: Scalar:: absMinValue = FLT_MIN;
#endif
const DataType Matrix:: Scalar:: pi = 4 * std::atan(1);
int Matrix:: Scalar:: randnIsReady_ = 1;
DataType Matrix:: Scalar:: randnValue_;
//----------------------------------------------------------------------------//
Matrix:: Matrix()
{
	rows_ = 0;
	cols_ = 0;
	length_ = 0;
	order_ = prefferedOrder_;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	charData_ = 0;
	charLength_ = 0;
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( int rows, int cols)	
	: matrix_(new DataArray(rows * cols))
{
#if SIZE_PROTECTION
	if (rows < 1 || rows > maxRows_) throw RowConstraint( rows);
	if (cols < 1 || cols > maxCols_) throw ColConstraint( cols);
#endif
	rows_ = rows;
	cols_ = cols;
	order_ = prefferedOrder_;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	length_ = rows_ * cols_;	
	charData_ = 0;
	charLength_ = 0;
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( int rows, int cols, DataType value)
	: matrix_(new DataArray(rows * cols))
{
#if SIZE_PROTECTION
	if (rows < 1 || rows > maxRows_) throw RowConstraint( rows);
	if (cols < 1 || cols > maxCols_) throw ColConstraint( cols);
#endif
#if NAN_PROTECTION
	if (value != value) throw NanAssign( "Matrix( int, int, DataType)");
#endif
	rows_ = rows;
	cols_ = cols;
	order_ = prefferedOrder_;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	length_ = rows_ * cols_;
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		matrix[i] = value;
	charData_ = 0;
	charLength_ = 0;
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( const string& values)
{
	stringstream convertBuffer;
	convertBuffer << values;
	convertBuffer >> rows_;
	convertBuffer >> cols_;
	order_ = byRows;
	rs_ = 1;
	cs_ = cols_;
	length_ = rows_ * cols_;	
	matrix_ = CountedHandle<DataArray>( new DataArray(length_));	
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		convertBuffer >> matrix[i];
	charData_ = 0;
	charLength_ = 0;
#if UNIFORM_ORDERS
	reorder( prefferedOrder_);
#endif
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( int rows, int cols, DataType min, DataType max)
	: matrix_(new DataArray(rows * cols))
{
#if SIZE_PROTECTION
	if (rows < 1 || rows > maxRows_) throw RowConstraint( rows);
	if (cols < 1 || cols > maxCols_) throw ColConstraint( cols);
#endif
	rows_ = rows;
	cols_ = cols;
	order_ = prefferedOrder_;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	length_ = rows_ * cols_;	
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
			matrix[i] = Scalar::rand() * (max - min) + min;
	charData_ = 0;
	charLength_ = 0;
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( unsigned short int binary, unsigned char digits)
{
	rows_ = 1;
	if (digits) //fixed number of digits
		cols_ = digits;
	else //smallest number of digits for binary representation
		cols_ = ceil( log2( binary));
	order_ = prefferedOrder_;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	length_ = rows_ * cols_;
	matrix_ = CountedHandle<DataArray>(new DataArray(length_));		
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = length_ - 1; i >= 0; i--)
	{
		matrix[i] = binary % 2;
		binary /= 2; //flooring due to type of binary
	}
	charData_ = 0;
	charLength_ = 0;	
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( int rows, int cols, DataType * data, ArrayOrder order)
	: matrix_( new DataArray( data, rows * cols), 1)
{
#if SIZE_PROTECTION
	if (rows < 1 || rows > maxRows_) throw RowConstraint( rows);
	if (cols < 1 || cols > maxCols_) throw ColConstraint( cols);
#endif
	rows_ = rows;
	cols_ = cols;
	order_ = order;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	length_ = rows_ * cols_;
	charData_ = 0;
	charLength_ = 0;
#if UNIFORM_ORDERS
	reorder( prefferedOrder_);
#endif
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw NanAssign("Matrix( Matrix&, int, int, int, int)");
#endif
	rows_ = m.rows_;
	cols_ = m.cols_;
	order_ = m.order_;
	rs_ = m.rs_;
	cs_ = m.cs_;
	length_ = m.length_;
	matrix_ = m.matrix_; //data sharing system in CountedHandle<DataArray>
	charData_ = 0;
	charLength_ = 0;	
}
//----------------------------------------------------------------------------//
Matrix:: Matrix( const Matrix& m, int r1, int r2, int c1, int c2)
{
#if INDEX_PROTECTION
	if (r1 < 0 || r1 > r2 || r2 >= m.rows_ || c1 < 0 || c1 > c2 || c2 >= m.cols_) 
		throw BadSelection(r1, r2, m.rows_ ,c1 ,c2, m.cols_);
#endif
#if NAN_PROTECTION
	if (m.isNan()) throw NanAssign("Matrix( Matrix&, int, int, int, int)");
#endif
	rows_ = r2 - r1 + 1;
	cols_ = c2 - c1 + 1;
	length_ = rows_ * cols_;
	order_ = prefferedOrder_;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	matrix_ = CountedHandle<DataArray>( new DataArray(length_));
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	for (int c, r = 0; r < rows_; r++)
	{
		for (c = 0; c < cols_; c++)
			matrix[ r * cs_ + c * rs_ ] = m_matrix[ (r + r1) * m.cs_ + 
			                                        (c + c1) * m.rs_ ];
	}
	charData_ = 0;
	charLength_ = 0;		
}
//----------------------------------------------------------------------------//
Matrix:: ~Matrix()
{
	//matrix_ object is freed by CountedHandle<DataArray> destructor
	if( charData_)
		delete[] charData_;
}
//----------------------------------------------------------------------------//
void Matrix:: copy( const Matrix& source)
{
	resize( source.rows_, source.cols_, true); //allocates new memory
	if (order_ != source.order_)
	{
		order_ = source.order_;
		rs_ = order_ == byRows ? 1 : rows_;
		cs_ = order_ == byCols ? 1 : cols_;
	}
#if OPTIMIZATION == OPT_NONE
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	const DataType *s_matrix = &(*source.matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
		matrix[i] = s_matrix[i];
#elif OPTIMIZATION == OPT_BLAS
	blas_copy( length_, &(*source.matrix_), 1, &(*matrix_), 1);
#elif OPTIMIZATION == OPT_CUDA
	cublas_copy( length_, source.matrix_->gpuMemoryArray(), 1 , 
	                             matrix_->gpuMemoryArray(), 1);
#else
	throw UnrecognizedOptimization();
#endif
}
//----------------------------------------------------------------------------//
void Matrix:: copyBuffer( const char * buffer)
{
	//Important! buffer data are inteded to be stored byRows order
	DataType* matrix = &(*matrix_); //direct access to array object
	if (order_ == byRows)
	{
		for (int i = 0; i < length_; i++)
			matrix[i] = (DataType)buffer[i] / 255;
	}
	else
	{
		for (int i = 0; i < length_; i++)
			matrix[ i / cols_ + (i % cols_) * rows_] = (DataType)
			                                           buffer[i] / 255;
	}
}
//----------------------------------------------------------------------------//
bool Matrix:: isNan() const
{
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
			if (matrix[i] != matrix[i]) return true;
	return false;
}
//----------------------------------------------------------------------------//
bool Matrix:: equalTo( const Matrix& m) const
{
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
#if UNIFORM_ORDERS
	for (int i = 0; i < length_; i++)
		if (matrix[i] != m_matrix[i]) return false;
	return true;
#else
	if(order_ == m.order_)
	{
		for (int i = 0; i < length_; i++)
			if (matrix[i] != m_matrix[i]) return false;
	}
	else //diffrent orders, more complex loop
	{
		#define idx r * cs_ + c * rs_
		#define midx r * m.cs_ + c * m.rs_
		for (int c, r = 0; r < rows_; r++)
		{
			for (c = 0; c < cols_; c++)
			{
				if (matrix[idx] != m_matrix[midx])
					return false;
			}
		}
		#undef idx
		#undef midx
	}
	return true;
#endif
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: reorder(ArrayOrder order)
{
#if UNIFORM_ORDERS
	if (order != prefferedOrder_) 
		throw UniformOrders("reorder");
#endif
	if (order_ == order) return *this;
	int rs = order == byRows ? 1 : rows_;
	int cs = order == byCols ? 1 : cols_;
	//temp pointer to old array
	CountedHandle<DataArray> tempHandle( matrix_);
	//new data array
	matrix_ = CountedHandle<DataArray>( new DataArray(length_));
	//data coping loop from old order to new order
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	DataType *temp = &(*tempHandle); //temporary direct access to data array
	for (int c, r = 0; r < rows_; r++)
		for (c = 0; c < cols_; c++)
			matrix[r * cs + c * rs] = temp[r * cs_ + c * rs_];
	//properties update
	order_ = order;
	rs_ = rs;
	cs_ = cs;
	return *this;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: reshape(int rows, int cols, ArrayOrder order)
{
	if (rows == rows_ && cols == cols_) return *this;
	if ((rows * cols) != length_) throw CantReshape(rows_, cols_, rows, cols);
	int rs = order_ == byRows ? 1 : rows;
	int cs = order_ == byCols ? 1 : cols;
#if UNIFORM_ORDERS
	if (order == prefferedOrder_) //no phisical data movining is neeeded
	{
		//setting new dimensions
		rows_ = rows;
		cols_ = cols;
		rs_ = rs;
		cs_ = cs;
		return *this;
	}
#endif
	CountedHandle<DataArray> tempHandle( matrix_);
	matrix_ = CountedHandle<DataArray>( new DataArray(length_));
	int r_ = 0, c_ = 0; 
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	DataType *temp = &(*tempHandle); //temporary direct access to data array
	if (order == byCols) //reshaping order
	{
		for (int r, c = 0; c < cols; c++)
		{
			for (r = 0; r < rows; r++)
			{
				matrix[r * cs + c * rs] = temp[r_ * cs_ + c_ * rs_];
				if (++r_ == rows_) { r_ = 0; c_++; } //reshaping by old columns
			}
		}
	}
	else //byRows
	{
		for (int c, r = 0; r < rows; r++)
		{
			for (c = 0; c < cols; c++)
			{
				matrix[r * cs + c * rs] = temp[r_ * cs_ + c_ * rs_];
				if (++c_ == cols_) { c_ = 0; r_++; } //reshaping by old rows
			}
		}
	}
	//setting new dimensions
	rows_ = rows;
	cols_ = cols;
	rs_ = rs;
	cs_ = cs;
	return *this;
}
//----------------------------------------------------------------------------//
unsigned int Matrix:: binary() const
{
	unsigned int number = 0, pos = 0; // base = 1;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = length_ - 1; i >= 0; i--)
	{
		/*
		number += (*matrix_)[i] * base;
		base *= 2;
		*/
		number |= (matrix[i] ? 0x1 : 0x0) << pos++;
	}
	return number;
}
//----------------------------------------------------------------------------//
void Matrix:: set( DataType value)
{
#if NAN_PROTECTION
	if (value != value) throw NanAssign( "set( DataType)");
#endif
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
		matrix[i] = value;
}
//----------------------------------------------------------------------------//
void Matrix:: setRow( int r, const Matrix& v)
{
#if SIZE_PROTECTION
	if (v.rows_ > 1 && v.cols_ > 1) throw NotVector(" setRow");
	if (v.length_ != cols_) 
		throw SizeMismatch( *this, v, "setRow");
#endif
#if NAN_PROTECTION
	if (v.isNan()) throw NanAssign("setRow( int, Matrix&)");
#endif
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	const DataType *v_matrix = &(*v.matrix_); //temporary direct access to data array
	for (int i = 0; i < v.length(); i++)
		matrix[r * cs_ + i * rs_] = v_matrix[i];
}
//----------------------------------------------------------------------------//
void Matrix:: setCol( int c, const Matrix& v)
{
#if SIZE_PROTECTION
	if (v.rows_ > 1 && v.cols_ > 1) throw NotVector(" setCol");
	if (v.length_ != rows_) 
		throw SizeMismatch( *this, v, "setCol");
#endif
#if NAN_PROTECTION
	if (v.isNan()) throw NanAssign("setCol( int, Matrix&)");
#endif
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	const DataType *v_matrix = &(*v.matrix_); //temporary direct access to data array
	for (int i = 0; i < v.length(); i++)
		matrix[i * cs_ + c * rs_] = v_matrix[i];
}
//----------------------------------------------------------------------------//
void Matrix:: resize( int rows, int cols, bool forceResize)
{	
	//do nothing if matrix has already good size and resizing is not forced
	if (rows == rows_ && cols == cols_ && !forceResize) return; 
#if SIZE_PROTECTION
	if (rows < 1 || rows > maxRows_) throw RowConstraint( rows);
	if (cols < 1 || cols > maxCols_) throw ColConstraint( cols);
#endif
	rows_ = rows;
	cols_ = cols;
	length_ = rows_ * cols_;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	matrix_ = CountedHandle<DataArray>( new DataArray(length_));
}
//----------------------------------------------------------------------------//
void Matrix:: resize( int rows, int cols, DataType value, bool forceResize)
{	
	if (rows != rows_ || cols != cols_ || forceResize)
	{
#if SIZE_PROTECTION
		if (rows < 1 || rows > maxRows_) throw RowConstraint( rows);
		if (cols < 1 || cols > maxCols_) throw ColConstraint( cols);
#endif
#if NAN_PROTECTION
		if (value != value) throw NanAssign("resize (valued version)");
#endif
		rows_ = rows;
		cols_ = cols;
		length_ = rows_ * cols_;
		rs_ = order_ == byRows ? 1 : rows_;
		cs_ = order_ == byCols ? 1 : cols_;
		matrix_ = CountedHandle<DataArray>(new DataArray(length_));
	}
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
		matrix[i] = value;	
}
//----------------------------------------------------------------------------//
void Matrix:: resize( int rows, int cols, DataType min, DataType max, 
                      bool forceResize)
{
	if (rows != rows_ || cols != cols_ || forceResize) 
	{
#if SIZE_PROTECTION
		if (rows < 1 || rows > maxRows_) throw RowConstraint( rows);
		if (cols < 1 || cols > maxCols_) throw ColConstraint( cols);
#endif
#if NAN_PROTECTION
		if (min != min || max != max) throw NanOperand( "resize (random version)");
#endif
		rows_ = rows;
		cols_ = cols;
		length_ = rows_ * cols_;
		rs_ = order_ == byRows ? 1 : rows_;
		cs_ = order_ == byCols ? 1 : cols_;
		matrix_ = CountedHandle<DataArray>( new DataArray(length_));
	}
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
		matrix[i] = Scalar::rand() * (max - min) + min;	
} 
//----------------------------------------------------------------------------//
void Matrix:: expand( int rowIncrease, int colIncrease) 
{
#if INDEX_PROTECTION
	if ( (rowIncrease == 0 && colIncrease == 0) || rowIncrease < 0 ||
	     colIncrease < 0) 
		return;
#endif
	CountedHandle<DataArray> tempHandle( matrix_); 
	matrix_ = CountedHandle<DataArray>( new DataArray( length_ + 
	                                                   rowIncrease * colIncrease));
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	DataType *temp = &(*tempHandle); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		matrix[i] = temp[i];
	rows_ += rowIncrease;
	cols_ += colIncrease;
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	length_ = rows_ * cols_;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: scaleFrom( const Matrix src, int rows, int cols)
{
	int rRatio = src.rows_ / rows;
	int cRatio = src.cols_ / cols;
	if ( !rRatio || !cRatio ||
	     (src.length_ / rRatio / cRatio != rows * cols))
		throw CantScale( src.rows_, src.cols_, rows, cols);
	resize( rows, cols, (DataType)0.0);
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	const DataType *s_matrix = &(*src.matrix_); //temporary direct access to data array
	#define midx r / rRatio * cs_ + c / cRatio * rs_
	#define sidx r * src.cs_ + c * src.rs_
	for (int r, c = 0; c < src.cols_; c++)
	{
		for (r = 0; r < src.rows_; r++)
		{
			matrix[midx] += s_matrix[sidx] / rRatio / cRatio;
		}
	}
	#undef midx
	#undef sidx
	return *this;
}
//----------------------------------------------------------------------------//
void Matrix:: setRandom( DataType min, DataType max)
{	
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
			matrix[i] = Scalar::rand() * (max - min) + min;
}
//----------------------------------------------------------------------------//
void Matrix:: randomVectors( const Matrix& avg, const Matrix& std, 
                             int dimension)
{
	int r, c, *i;
	#if SIZE_PROTECTION
	if (avg.rows_ > 1 && avg.cols_ > 1) 
		throw NotVector( "randomVectors(...) - bad parameter avg - ");
	if (std.rows_ > 1 && std.cols_ > 1) 
		throw NotVector( "randomVectors(...) - bad parameter std - ");
	#endif
	switch (dimension)
	{
		case 1: //set columns
			#if SIZE_PROTECTION
			if ( avg.length_ != rows_) 
				throw SizeMismatch( *this, avg, 
				            "randomVectors(...) on matrix rows with given avg");
			if ( std.length_ != rows_) 
				throw SizeMismatch( *this, std, 
				            "randomVectors(...) on matrix rows with given std");
			#endif
			i = &r; //fixed column, variable row
			break;
		case 2: //set rows
			#if SIZE_PROTECTION
			if ( avg.length_ != cols_) 
				throw SizeMismatch( *this, avg, 
				         "randomVectors(...) on matrix columns with given avg");
			if ( std.length_ != cols_) 
				throw SizeMismatch( *this, std, 
				         "randomVectors(...) on matrix columns with given std");
			#endif
			i = &c; //fixed row, variable column
			break;
		default: throw WrongParameter( "dimension", "randomVectors(...)");
	}
	
	//temporary direct access to data arrays
	DataType *matrix = &(*matrix_); 
	const DataType *avg_matrix = &(*avg.matrix_); 
	const DataType *std_matrix = &(*std.matrix_);
	for (r = 0; r < rows_; r++)
	{
		for (c = 0; c < cols_; c++)
			matrix[r * cs_ + c * rs_] = Scalar::randn() * std_matrix[*i] 
			                            + avg_matrix[*i];
	}
}
//----------------------------------------------------------------------------//
void Matrix:: loadFromCsv( const char * csvFilePath)
{
	fstream file;
	file.open( csvFilePath, ios::in);
	if (!file) 
	{ 
		cout << "[Matrix] Can't load from " << csvFilePath << endl; 
		return; 
	} 
	//getting dimension of matrix - not smart version
	int r = 0, c = 1;
	char s = 0;
	while (file.good() && s != '\n') //obtaining number of cols_
	{
		file.read(&s, sizeof(char));
		if (s == ',') c++;
	}
	while (file.good()) //obtaining number of rows_
	{
		file.read(&s, sizeof(char));
		if (s == '\n') r++;
	}	
	file.close();
	file.clear();
	cout << "[Matrix] loading matrix " << r << "x" << c << " from file "
	     << csvFilePath << endl;
	file.open( csvFilePath, ios::in); //reopened as text file
	//size matching
	if (r != rows_ || c != cols_)
	{
		rows_ = r;
		cols_ = c;
		length_ = rows_ * cols_;
		order_ = prefferedOrder_;
		rs_ = order_ == byRows ? 1 : rows_;
		cs_ = order_ == byCols ? 1 : cols_;	
		matrix_ = CountedHandle<DataArray>( new DataArray( length_));
	}
	//loading data, changing meaning of r and c variables	
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (r = 0; r < rows_; r++)
	{
		for (c = 0; c < cols_; c++)
		{
			file >> matrix[r * cs_ + c * rs_]; 
			file.ignore(); //skip delimiter (which should be comma character)			
		}
	}
	file.close();
}
//----------------------------------------------------------------------------//
void Matrix:: saveToCsv( const char * csvFilePath) const
{
	fstream file;
	file.open( csvFilePath, ios::out); //opened as text file
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int c, r = 0; r < rows_; r++)
	{
		for (c = 0; c < cols_-1; c++) //saving contest of matrix_
			file << matrix[r * cs_ + c * rs_] << ",";
		file << matrix[r * cs_ + (cols_ -1) * rs_] << "\n"; 
		               // or without \n at file end: << (r < rows_ - 1 ? "\n" : "");
	}
	file.close();	
}
//----------------------------------------------------------------------------//
unsigned char* Matrix:: toUchar(int width, int height, 
		                        DataType min, DataType max) const
{
	if (!width) width = cols_;
	if (!height) height = rows_;
#if SIZE_PROTECTION
	if ((width * height) != length_) 
		throw CantReshape( rows_, cols_, height, width);
#endif
	if (!min && !max)
	{
		min = this->min();
		max = this->max();
	}
	//array strides for (width,height)
	int rs = order_ == byRows ? 1 : height;
	int cs = order_ == byCols ? 1 : width;	 
	if( charData_) delete[] charData_;
	charLength_ = length_;
	charData_ = new unsigned char[ charLength_];
	const DataType *matrix = &(*matrix_); //direct access to data array
	for (int c, r = 0; r < height; r++)
	{
		#define midx r * cs + c * rs
		#define cidx r * width + c
		for (c = 0; c < width; c++)
			charData_[cidx] = (matrix[midx] - min) / (max - min) * 255;
		#undef cidx
		#undef midx
	}		
	return charData_;
}
//----------------------------------------------------------------------------//
unsigned char* Matrix:: toPgmFormat( int width, int height, 
                                     DataType min, DataType max,
                                     ArrayOrder reshapingOrder) const
{
	if (!width)	 width  = cols_;
	if (!height) height = rows_;
#if SIZE_PROTECTION	
	if ((width * height) != length_) 
		throw CantReshape( rows_, cols_, height, width);
#endif
	if (!min && !max)
	{
		min = this->min();
		max = this->max();
	}
	int dataStart = 11 + (int)log10( width) + (int)log10( height);
	charLength_ = dataStart + length_;
	if (charData_) delete[] charData_;
	charData_ = new unsigned char[charLength_];
	sprintf((char*)charData_, "P5\n%d %d\n255\n", width, height);
	//save in rows order, reshape in reshapingOrder
	int r, c, r_, c_, i;
	#define cidx dataStart + r * width + c
	#define midx r_ * cs_ + c_ * rs_
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	if (reshapingOrder == byCols)
	{
		for (r = 0; r < height; r++)
		{
			for (c = 0; c < width; c++)
			{	
				i = c * height + r;
				r_ = i % rows_;
				c_ = i / rows_;
				charData_[cidx] = (unsigned char)((matrix[midx] - min) /
				                                  (max - min) * 255);
			}
		}
	}
	else //reshaping byRows
	{
		for (r = 0; r < height; r++)
		{
			for (c = 0; c < width; c++)
			{	
				i = r * width + c;
				r_ = i / cols_;
				c_ = i / cols_;
				charData_[cidx] = (unsigned char)((matrix[midx] - min) /
				                                  (max - min) * 255);
			}
		}
	}
	#undef cidx
	#undef midx
	return charData_;
}
//----------------------------------------------------------------------------//
DataType Matrix:: max() const
{
	DataType max = - Scalar::absMaxValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (matrix[i] > max) max = matrix[i];	
	return max;
}
//----------------------------------------------------------------------------//
int Matrix:: maxIndex() const
{
#if OPTIMIZATION == OPT_NONE
	int idx(-1);
	DataType max = - Scalar::absMaxValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (matrix[i] > max) 
		{
			max = matrix[i];
			idx = i;
		}
	return idx;
#elif OPTIMIZATION == OPT_BLAS
	return blas_imax( length_, &(*matrix_), 1);
#elif OPTIMIZATION == OPT_CUDA
	return cublas_imax( length_, matrix_->gpuMemoryArray(), 1);
#else
	throw UnrecognizedOptimization();
#endif
}
//----------------------------------------------------------------------------//
DataType Matrix:: min() const
{
	DataType min = Scalar::absMaxValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (matrix[i] < min) min = matrix[i];
	return min;
}
//----------------------------------------------------------------------------//
int Matrix:: minIndex() const
{
	int idx(-1);
	DataType min = Scalar::absMaxValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (matrix[i] < min) 
		{
			min = matrix[i];
			idx = i;
		}
	return idx;
}
//----------------------------------------------------------------------------//
DataType Matrix:: absMax() const
{
	DataType max = Scalar::absMinValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (abs(matrix[i]) > max) max = abs(matrix[i]);		
	return max;
}
//----------------------------------------------------------------------------//
int Matrix:: absMaxIndex() const
{
	int idx(-1);
	DataType max = Scalar::absMinValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (abs(matrix[i]) > max) 
		{
			max = abs(matrix[i]);
			idx = i;			
		}
	return idx;
}
//----------------------------------------------------------------------------//
DataType Matrix:: absMin() const
{
	DataType min = Scalar::absMaxValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (abs(matrix[i]) < min) min = abs(matrix[i]);		
	return min;
}
//----------------------------------------------------------------------------//
int Matrix:: absMinIndex() const
{
	int idx(-1);
	DataType min = Scalar::absMaxValue;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++) 
		if (abs(matrix[i]) < min) 
		{
			min = abs(matrix[i]);
			idx = i;
		}
	return idx;
}
//----------------------------------------------------------------------------//
DataType Matrix:: sum() const
{
	DataType sum = 0.0;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
		sum += matrix[i];
	return sum;
}
//----------------------------------------------------------------------------//
DataType Matrix:: var() const
{
	DataType sum = 0.0,
	         avg = this->avg();
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
		sum += (matrix[i] - avg) * (matrix[i] - avg);
	return sum / length_;
}
//----------------------------------------------------------------------------//
DataType Matrix:: norm() const
{
#if SIZE_PROTECTION
	if (rows_ > 1 && cols_ > 1) 
		throw NotVector( "Matrix::norm() ");
#endif
#if OPTIMIZATION == OPT_NONE
	DataType sum2= 0.0;
	const DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (int i = 0; i < length_; i++)
		sum2 += matrix[i] * matrix[i];
	return sqrt( sum2 );
#elif OPTIMIZATION == OPT_BLAS
	return blas_nrm2( length_, &(*matrix_), 1);
#elif OPTIMIZATION == OPT_CUDA
	return cublas_nrm2(length_, matrix_->gpuMemoryArray(), 1);
#else
	throw UnrecognizedOptimization();
#endif
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: normalize( int dimension)
{
	int *r, *c, i, imax, j, jmax;
	switch (dimension)
	{
		case 1: //normalize columns vectors
			c = &i; //for each column
			imax = cols_;
			r = &j; //go trough all rows
			jmax = rows_;
			break;
		case 2: //normalize rows vectors
			r = &i; //for each row
			imax = rows_;
			c = &j; //go trough all columns
			jmax = cols_;
			break;
		default: 
			throw WrongParameter( "dimension", "normalize( int dimension)");
	}
	DataType s2;
	DataType *matrix = &(*matrix_); //temporary direct access to data array
	for (i = 0; i < imax; i++)
	{
		s2 = 0.0;
		for (j = 0; j < jmax; j++)
			s2 += matrix[*r * cs_ + *c * rs_] * 
			      matrix[*r * cs_ + *c * rs_];
		s2 = 1 / sqrt( s2); //inverted norm
		for (j = 0; j < jmax; j++)
			matrix[*r * cs_ + *c * rs_] *= s2;
	}
	return *this;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: T()
{
#if UNIFORM_ORDERS
	if (rows_ != 1 && cols_ != 1) //not vector transpose
		throw UniformOrders("T()"); //cant change matrix data order
#else
	//switch order
	order_ = - order_;
#endif 
	//swap rows_ and cols_ using rs_ as temp
	rs_ = rows_;	
	rows_ = cols_;
	cols_ = rs_; 
	//set stride
	rs_ = order_ == byRows ? 1 : rows_;
	cs_ = order_ == byCols ? 1 : cols_;
	return *this;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: operator = (const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw NanAssign( "operator = (const Matrix&)");
#endif
	rows_ = m.rows_;
	cols_ = m.cols_;
	length_ = m.length_;
	order_ = m.order_;
	rs_ = m.rs_;
	cs_ = m.cs_;
	matrix_ = m.matrix_; //CountedHandle<DataArray> assign operator
	return *this;
}
//----------------------------------------------------------------------------//
ostream& operator << (ostream& os, const Matrix& m)
{
	const DataType *matrix = &(*m.matrix_); //temporary direct access to data array
	for (int c, r = 0; r < m.rows_; r++)
	{
		os <<"[ ";
		for (c = 0; c < m.cols_; c++) 
			os << matrix[r * m.cs_ + c * m.rs_] << " ";
		os << "]" << (r != m.rows_ - 1 ? "\n" : "");
	}
	return os;
}
//----------------------------------------------------------------------------//
fstream& operator << (fstream& f, const Matrix& m)
{
	f.write((char*)&m.rows_, sizeof(int));
	f.write((char*)&m.cols_, sizeof(int));
	f.write((char*)&m.order_, sizeof(m.order_));
	const DataType *matrix = &(*m.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		f.write((char*)&matrix[i], sizeof(DataType));
	return f;
}
//----------------------------------------------------------------------------//
Matrix&  operator >> (fstream& f, Matrix& m)
{
	int rows, cols;
	f.read((char*)&rows, sizeof(int));
	f.read((char*)&cols, sizeof(int));
	f.read((char*)&m.order_, sizeof(m.order_));
	m.resize( rows, cols);
	const DataType *matrix = &(*m.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		f.read((char*)&matrix[i], sizeof(DataType));
#if UNNIFORM_ORDERS
	reorder( prefferedOrder_);
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: transpose( const Matrix& m)
{
#if NAN_PROTECTION
	if(m.isNan()) throw Matrix::NanOperand("transpose");
#endif
	Matrix tr( m.cols_, m.rows_); //result matrix
	DataType *t_matrix = &(*tr.matrix_); //temporary direct access to data array
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	#define tidx r * tr.cs_ + c * tr.rs_
	#define midx c * m.cs_ + r * m.rs_
	for (int c, r = 0; r < tr.rows_; r++)
	{
		for (c = 0; c < tr.cols_; c++)
			t_matrix[tidx] = m_matrix[midx];
	}
	#undef tidx
	#undef midx
	return tr;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_plus_b( Matrix& m, const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if (a.isNan() || b.isNan()) 
		throw NanOperand("m_is_a_plus_b");
#endif
#if SIZE_PROTECTION
	if (a.rows_ != b.rows_ || a.cols_ != b.cols_) 
		throw Matrix:: SizeMismatch:: SizeMismatch( a, b, "m_is_a_plus_b");	
#endif
	m.resize( a.rows_, a.cols_);
#if OPTIMIZATION == OPT_NONE
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int c, r = 0; r < m.rows_; r++)
	{
		#define midx r * m.cs_ + c * m.rs_
		#define aidx r * a.cs_ + c * a.rs_
		#define bidx r * b.cs_ + c * b.rs_
		for (c = 0; c < m.cols_; c++)
		{
			m_matrix[midx] = a_matrix[aidx] + b_matrix[bidx];
		}
		#undef midx
		#undef aidx
		#undef bidx
	}
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		//m copy a
		if (m.matrix_ != a.matrix_) //copy only if m != a 
			blas_copy( m.length_, &(*a.matrix_), 1, &(*m.matrix_), 1);
		// m += b
		blas_axpy( m.length_, 1.0, &(*b.matrix_), 1, &(*m.matrix_), 1);
	#else
		throw UniformOrders("BLAS: xAXPY");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			//m copy a
			if (m.matrix_ != a.matrix_) //copy only if m != a 
				cublas_copy( m.length_, a.matrix_->gpuMemoryArray(), 1, 
				             m.matrix_->gpuMemoryArray(), 1);
			// m += b
			cublas_axpy( m.length_, 1.0, b.matrix_->gpuMemoryArray(), 1, 
			             m.matrix_->gpuMemoryArray(), 1);
		#else
			throw Exception("CUDA optimalization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xAXPY");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_plus_b( Matrix& m, const Matrix& a, const DataType& b)
{
#if NAN_PROTECTION
	if (a.isNan() || b != b) 
		throw NanOperand("m_is_a_plus_b");
#endif
	m.order_ = a.order_;
	m.resize( a.rows_, a.cols_);
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		m_matrix[i] = a_matrix[i] + b;
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_minus_b( Matrix& m, const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if (a.isNan() || b.isNan()) 
		throw NanOperand("m_is_a_minus_b");
#endif
#if SIZE_PROTECTION
	if (a.rows_ != b.rows_ || a.cols_ != b.cols_) 
		throw Matrix:: SizeMismatch:: SizeMismatch( a, b, "m_is_a_minus_b");	
#endif
	m.resize( a.rows_, a.cols_);
#if OPTIMIZATION == OPT_NONE
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int c, r = 0; r < m.rows_; r++)
	{
		#define midx r * m.cs_ + c * m.rs_
		#define aidx r * a.cs_ + c * a.rs_
		#define bidx r * b.cs_ + c * b.rs_
		for (c = 0; c < m.cols_; c++)
		{
			m_matrix[midx] = a_matrix[aidx] - b_matrix[bidx];
		}
		#undef midx
		#undef aidx
		#undef bidx
	}
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		//m copy a
		if (m.matrix_ != a.matrix_) //copy only if m != a 
			blas_copy( m.length_, &(*a.matrix_), 1, &(*m.matrix_), 1);
		// m += -b
		blas_axpy( m.length_, -1.0, &(*b.matrix_), 1, &(*m.matrix_), 1);
	#else
		throw UniformOrders("BLAS: xAXPY");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			//m copy a
			if (m.matrix_ != a.matrix_) //copy only if m != a 
				cublas_copy( m.length_, a.matrix_->gpuMemoryArray(), 1, 
				             m.matrix_->gpuMemoryArray(), 1);
			// m += -b
			cublas_axpy( m.length_, -1.0, b.matrix_->gpuMemoryArray(), 1, 
			             m.matrix_->gpuMemoryArray(), 1);
		#else
			throw Exception("CUDA optimalization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xAXPY");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_minus_b( Matrix& m, const Matrix& a, const DataType& b)
{
#if NAN_PROTECTION
	if (a.isNan() || b != b) 
		throw NanOperand("m_is_a_minus_b");
#endif
	m.order_ = a.order_;
	m.resize( a.rows_, a.cols_);
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array	
	for (int i = 0; i < m.length_; i++)
		m_matrix[i] = a_matrix[i] - b;
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_div_b( Matrix& m, const Matrix& a, const DataType& b)
{
#if NAN_PROTECTION
	if (a.isNan() || b != b) 
		throw NanOperand("m_is_a_div_b");
#endif
	m.order_ = a.order_;
	m.resize( a.rows_, a.cols_);
#if OPTIMIZATION == OPT_NONE
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	DataType inv_b = 1 / b;
	for (int i = 0; i < m.length_; i++)
		m_matrix[i] = a_matrix[i] * inv_b;
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		//m copy a
		if (m.matrix_ != a.matrix_) //copy only if m != a 
			blas_copy( m.length_, &(*a.matrix_), 1, &(*m.matrix_), 1);
		// m *= 1/b
		blas_scal( m.length_, 1/b, &(*m.matrix_), 1);
	#else
		throw UniformOrders("BLAS: xSCAL");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			//m copy a
			if (m.matrix_ != a.matrix_) //copy only if m != a 
				cublas_copy( m.length_, a.matrix_->gpuMemoryArray(), 1, 
				             m.matrix_->gpuMemoryArray(), 1);
			// m *= 1/b
			cublas_scal( m.length_, 1/b, m.matrix_->gpuMemoryArray(), 1);
		#else
			throw Exception("CUDA optimalization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xSCAL");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_div_b( Matrix& m, const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if(a.isNan() || b.isNan()) throw Matrix::NanOperand("element division");
#endif
#if SIZE_PROTECTION
	if (a.rows_ != b.rows_ || a.cols_ != b.cols_) 
		throw Matrix:: SizeMismatch( a, b, "element-wise division");
#endif
	m.resize( a.rows_, a.cols_);
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
 		m_matrix[i] = a_matrix[i] / b_matrix[i];
	return m;	
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_mul_b( Matrix& m, const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if(a.isNan() || b.isNan()) throw Matrix::NanOperand("element multiplication");
#endif
#if SIZE_PROTECTION
	if (a.rows_ != b.rows_ || a.cols_ != b.cols_) 
		throw Matrix:: SizeMismatch( a, b, "element-wise multiplication");
#endif
	m.resize( a.rows_, a.cols_);
	DataType *m_matrix = &(*m.matrix_);  //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
 		m_matrix[i] = a_matrix[i] * b_matrix[i];
	return m;	
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_x_b( Matrix& m, const Matrix& a, const DataType& b)
{
#if NAN_PROTECTION
	if (a.isNan() || b != b) 
		throw NanOperand("m_is_a_x_b");
#endif
	m.order_ = a.order_;
	m.resize( a.rows_, a.cols_);
#if OPTIMIZATION == OPT_NONE
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		m_matrix[i] = a_matrix[i] * b;
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		//m copy a
		if (m.matrix_ != a.matrix_) //copy only if m != a 
			blas_copy( m.length_, &(*a.matrix_), 1, &(*m.matrix_), 1);
		// m *= b
		blas_scal( m.length_, b, &(*m.matrix_), 1);
	#else
		throw UniformOrders("BLAS: xSCAL");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			//m copy a
			if (m.matrix_ != a.matrix_) //copy only if m != a 
				cublas_copy( m.length_, a.matrix_->gpuMemoryArray(), 1, 
				             m.matrix_->gpuMemoryArray(), 1);
			// m *= b
			cublas_scal( m.length_, b, m.matrix_->gpuMemoryArray(), 1);
		#else
			throw Exception("CUDA optimalization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xSCAL");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_x_b( Matrix& m, 
                             const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if(a.isNan() || b.isNan()) throw Matrix::NanOperand("m_is_a_x_b");
#endif
#if SIZE_PROTECTION
	if (a.cols_ != b.rows_) 
		throw Matrix:: SizeMismatch:: SizeMismatch( a, b, "m_is_a_x_b");
#endif
	m.resize( a.rows_, b.cols_, (DataType)0.0);
#if OPTIMIZATION == OPT_NONE
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int m_idx, i, c , r = 0; r < m.rows_; r++)
	{
		for (c = 0; c < m.cols_; c++)
		{
			#define aidx r * a.cs_ + i * a.rs_
			#define bidx i * b.cs_ + c * b.rs_
			m_idx = r * m.cs_ + c * m.rs_;
			for (i = 0; i < a.cols_; i++)
				m_matrix[m_idx] += a_matrix[aidx] * b_matrix[bidx];	
			#undef aidx
			#undef bidx
		}
	}
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			blas_gemm( CblasColMajor, CblasNoTrans, CblasNoTrans, 
			           a.rows_, b.cols_, a.cols_, 
			           1.0, &(*a.matrix_), a.rs_, &(*b.matrix_), b.rs_,
			           1.0, &(*m.matrix_), m.rs_);
		#else //prefferedOrder_ == byRows
			blas_gemm( CblasRowMajor, CblasNoTrans, CblasNoTrans, 
			           a.rows_, b.cols_, a.cols_, 
			           1.0, &(*a.matrix_), a.cs_, &(*b.matrix_), b.cs_, 
			           1.0, &(*m.matrix_), m.cs_);
		#endif
	#else
		throw UniformOrders("BLAS: xGEMM");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			cublas_gemm( 'N', 'N', a.rows_, b.cols_, a.cols_, 1.0, 
			             a.matrix_->gpuMemoryArray() , a.rs_, 
			             b.matrix_->gpuMemoryArray(), b.rs_, 1.0, 
			             m.matrix_->gpuMemoryArray(), m.rs_);
		#else
			throw Exception("CUDA optimization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xGEMM");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_aT_x_b( Matrix& m, 
                              const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if(a.isNan() || b.isNan()) throw Matrix::NanOperand("m_is_aT_x_b");
#endif
#if SIZE_PROTECTION
	if (a.rows_ != b.rows_) 
		throw Matrix:: SizeMismatch:: SizeMismatch( a, b, "m_is_aT_x_b");
#endif
	m.resize( a.cols_, b.cols_, (DataType)0.0);
#if OPTIMIZATION == OPT_NONE
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int m_idx, i, c , r = 0; r < m.rows_; r++)
	{
		for (c = 0; c < m.cols_; c++)
		{
			#define aidx i * a.cs_ + r * a.rs_
			#define bidx i * b.cs_ + c * b.rs_
			m_idx = r * m.cs_ + c * m.rs_;
			for (i = 0; i < a.rows_; i++)
				m_matrix[m_idx] += a_matrix[aidx] * b_matrix[bidx];	
			#undef aidx
			#undef bidx
		}
	}
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			blas_gemm( CblasColMajor, CblasTrans, CblasNoTrans,
		   	        a.cols_, b.cols_, a.rows_, 
	  	 	        1.0, &(*a.matrix_), a.rs_, &(*b.matrix_), b.rs_, 
			          1.0, &(*m.matrix_), m.rs_);
		#else //prefferedOrder_ == byRows
			blas_gemm( CblasRowMajor, CblasTrans, CblasNoTrans,
		    	       a.cols_, b.cols_, a.rows_, 
	  	  	       1.0, &(*a.matrix_), a.cs_, &(*b.matrix_), b.cs_, 
			           1.0, &(*m.matrix_), m.cs_);
		#endif
	#else
	throw UniformOrders("BLAS: xGEMM");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			cublas_gemm( 'T', 'N', a.cols_, b.cols_, a.rows_, 1.0, 
	  	             a.matrix_->gpuMemoryArray(), a.rs_, 
			             b.matrix_->gpuMemoryArray(), b.rs_, 1.0, 
			             m.matrix_->gpuMemoryArray(), m.rs_);
		#else
			throw Exception("CUDA optimalization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xGEMM");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_a_x_bT( Matrix& m, 
                              const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if(a.isNan() || b.isNan()) throw Matrix::NanOperand("m_is_a_x_bT");
#endif
#if SIZE_PROTECTION
	if (a.cols_ != b.cols_) 
		throw Matrix:: SizeMismatch:: SizeMismatch( a, b, "m_is_a_x_bT");
#endif
	m.resize( a.rows_, b.rows_, (DataType)0.0);
#if OPTIMIZATION == OPT_NONE	
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int m_idx, i, c , r = 0; r < m.rows_; r++)
	{
		for (c = 0; c < m.cols_; c++)
		{
			#define aidx r * a.cs_ + i * a.rs_
			#define bidx c * b.cs_ + i * b.rs_
			m_idx = r * m.cs_ + c * m.rs_;
			for (i = 0; i < a.cols_; i++)
				m_matrix[m_idx] += a_matrix[aidx] * b_matrix[bidx];	
			#undef aidx
			#undef bidx
		}
	}
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			blas_gemm( CblasColMajor, CblasNoTrans, CblasTrans, 
		  	         a.rows_, b.rows_, a.cols_,  
	  	  	       1.0, &(*a.matrix_), a.rs_, &(*b.matrix_), b.rs_, 
			           1.0, &(*m.matrix_), m.rs_);
		#else //prefferedOrder_ == byRows
			blas_gemm( CblasRowMajor, CblasNoTrans, CblasTrans,
		  	         a.rows_, b.rows_, a.cols_, 1.0, 
	  	  	       &a.matrix_, a.cs_, &b.matrix_, b.cs_, 1.0, &m.matrix_, m.cs_);
		#endif
	#else
	throw UniformOrders("BLAS: xGEMM");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			cublas_gemm( 'N', 'T', a.cols_, b.cols_, a.rows_, 1.0, 
	  	             a.matrix_->gpuMemoryArray(), a.rs_, 
			             b.matrix_->gpuMemoryArray(), b.rs_, 1.0, 
			             m.matrix_->gpuMemoryArray(), m.rs_);
		#else
			throw Exception("CUDA optimalization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xGEMM");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix& Matrix:: m_is_aT_x_bT( Matrix& m, 
                               const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if(a.isNan() || b.isNan()) throw Matrix::NanOperand("m_is_aT_x_bT");
#endif
#if SIZE_PROTECTION
	if (a.rows_ != b.cols_) 
		throw Matrix:: SizeMismatch:: SizeMismatch( a, b, "m_is_aT_x_bT");
#endif
	m.resize( a.cols_, b.rows_, (DataType)0.0);
#if OPTIMIZATION == OPT_NONE	
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int m_idx, i, c , r = 0; r < m.rows_; r++)
	{
		for (c = 0; c < m.cols_; c++)
		{
			#define aidx i * a.cs_ + r * a.rs_
			#define bidx c * b.cs_ + i * b.rs_
			m_idx = r * m.cs_ + c * m.rs_;
			for (i = 0; i < a.rows_; i++)
				m_matrix[m_idx] += a_matrix[aidx] * b_matrix[bidx];	
			#undef aidx
			#undef bidx
		}
	}
#elif OPTIMIZATION == OPT_BLAS
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			blas_gemm( CblasColMajor, CblasTrans, CblasTrans,
		  	         a.cols_, b.rows_, a.rows_, 
	  	  	       1.0, &(*a.matrix_), a.rs_, &(*b.matrix_), b.rs_, 
			           1.0, &(*m.matrix_), m.rs_);
		#else //prefferedOrder_ == byRows
			blas_gemm( CblasRowMajor, CblasTrans, CblasTrans,
			           a.cols_, b.rows_, a.rows_, 
		  	         1.0, &(*a.matrix_), a.cs_, &(*b.matrix_), b.cs_, 
			           1.0, &(*m.matrix_), m.cs_);
		#endif
	#else
	throw UniformOrders("BLAS: xGEMM");
	#endif
#elif OPTIMIZATION == OPT_CUDA
	#if UNIFORM_ORDERS
		#if prefferedOrder_ == byCols
			cublas_gemm( 'T', 'T', a.cols_, b.cols_, a.rows_, 1.0, 
	  	             a.matrix_->gpuMemoryArray(), a.rs_, 
			             b.matrix_->gpuMemoryArray(), b.rs_, 1.0, 
			             m.matrix_->gpuMemoryArray(), m.rs_);
		#else
			throw Exception("CUDA optimalization requires prefferedOrder == byCols");
		#endif
	#else
		throw UniformOrders("CUBLAS: xGEMM");
	#endif
#else
	throw UnrecognizedOptimization();
#endif
	return m;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: distance (const Matrix& a, const Matrix& b)
{
#if NAN_PROTECTION
	if(a.isNan() || b.isNan()) throw Matrix::NanOperand("distance");
#endif
#if SIZE_PROTECTION
	if (a.cols_ != b.rows_) 
		throw Matrix:: SizeMismatch:: SizeMismatch( a, b, "distance");
#endif
	DataType d; //vector difference
	Matrix m(a.rows_, b.cols_, 0.0);
	DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	const DataType *a_matrix = &(*a.matrix_); //temporary direct access to data array
	const DataType *b_matrix = &(*b.matrix_); //temporary direct access to data array
	for (int m_idx, i, c , r = 0; r < m.rows_; r++)
		for (c = 0; c < m.cols_; c++)
		{
			m_idx = r * m.cs_ + c * m.rs_; //array index of (r,c) element
			for (i = 0; i < a.cols_; i++)
			{				
				d = a_matrix[r * a.cs_ + i * a.rs_] - 
				    b_matrix[i * b.cs_ + c * b.rs_];
				m_matrix[m_idx] += d * d;
			}	                                  ;	
			m_matrix[m_idx] = sqrt(m_matrix[m_idx]);
		}
	return m;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: passValue(const Matrix& m)
{	
	return Matrix(m); 
}
//----------------------------------------------------------------------------//
Matrix Matrix:: constant(const Matrix& m)
{
	return Matrix( m.rows(), m.cols(), 1);
}
//----------------------------------------------------------------------------//
Matrix Matrix:: step(const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("step");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		r_matrix[i] = m_matrix[i] > 0 ? 1.0 : 0.0;
	return result;

}
//----------------------------------------------------------------------------//
Matrix Matrix:: perceptron(const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("perceptron");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		r_matrix[i] = m_matrix[i] > 0 ? m_matrix[i] : 0;
	return result;

}
//----------------------------------------------------------------------------//
Matrix Matrix:: sign(const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("sign");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		r_matrix[i] = m_matrix[i] < 0 ? -1 : (m_matrix[i] > 0 ?  1 : 0);
	return result;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: brainStateBox(const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("brainStateBox");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		r_matrix[i] = m_matrix[i] > 1 ? 1 : (m_matrix[i] < -1 ? -1 : m_matrix[i]);
	return result;
}
//----------------------------------------------------------------------------//
const DataType Matrix:: stepBias = 0.5;
//----------------------------------------------------------------------------//
Matrix Matrix:: biasedStep(const Matrix& m)
//replace values with either 0 or 1 
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("biasedStep");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		r_matrix[i] = m_matrix[i] > stepBias ? 1 : 0;
	return result;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: winnerTakesAll( const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("winnerTakesAll");
#endif
	Matrix result(m.rows_, m.cols_, 0.0);
	DataType winner = - Scalar::absMaxValue;
	int idx = -1;
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
	{
		if (m_matrix[i] > winner)
		 {
			winner = m_matrix[i];
			idx = i;
	 	}
 	}	
 	(*result.matrix_)[idx] = 1;
	return result;	
}
//----------------------------------------------------------------------------//
const DataType Matrix:: shapeRatio = 5.0;
//----------------------------------------------------------------------------//
Matrix Matrix:: sigmoid(const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("sigmoid");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		r_matrix[i] = Scalar:: sigmoid( Matrix::shapeRatio, m_matrix[i]);
	return result;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: dSigmoid(const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("dSigmoid");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		//if m.(*matrix_)[i] = Scalar::sigmoid( m.(*matrix_)[i])
		r_matrix[i] = Matrix::shapeRatio * m_matrix[i] * (1 - m_matrix[i]);
		//full formula
		//result.(*matrix_)[i] = Matrix::shapeRatio 
		//                    * Scalar::sigmoid( Matrix::shapeRatio, m.(*matrix_)[i])
		//               * (1 - Scalar::sigmoid( Matrix::shapeRatio, m.(*matrix_)[i]));
	return result;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: tanh( const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("tanh");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		r_matrix[i] = Scalar:: tanh( Matrix::shapeRatio, m_matrix[i]);
	return result;
}
//----------------------------------------------------------------------------//
Matrix Matrix:: dTanh( const Matrix& m)
{
#if NAN_PROTECTION
	if (m.isNan()) throw Matrix::NanOperand("dTanh");
#endif
	Matrix result(m.rows_, m.cols_);
	const DataType *m_matrix = &(*m.matrix_); //temporary direct access to data array
	DataType *r_matrix = &(*result.matrix_); //temporary direct access to data array
	for (int i = 0; i < m.length_; i++)
		//if m.(*matrix_)[i] = Scalar::tanh( m.(*matrix_)[i])
		r_matrix[i] = Matrix::shapeRatio * (1 + m_matrix[i]) * (1 - m_matrix[i]);
		//full formula
		//result.(*matrix_)[i] = Matrix::shapeRatio *
 		//                    (1 + Scalar::tanh( Matrix::shapeRatio, m.(*matrix_)[i]))
 		//                  * (1 - Scalar::tanh( Matrix::shapeRatio, m.(*matrix_)[i]));
	return result;
}
//----------------------------------------------------------------------------//
string Matrix:: arrayOrderToString( ArrayOrder order)
{
	string s;
	switch (order)
	{
		case byCols:
			s = "byCols";
			break;
		case byRows:
			s = "byRows";
			break;
		default:
			s = "undefined order";
	}
	return s;
}
//----------------------------------------------------------------------------//
DataType Matrix:: Scalar:: randn()
{
	/* normal random generator based on Box - Muller alogrithm 
	R1, R2 is (0,1)
	N1 = sqrt(-2 ln(R1) ) * cos( 2pi * R2 )
	N2 = sqrt(-2 ln(R1) ) * sin( 2pi * R2 )
	N1, N2 is then  N(0,1)
	*/
	randnIsReady_ ^= 1; //switch ready status
	if (randnIsReady_)
		return randnValue_;
	else
	{
		DataType R1, R2, value;
		R1 = rand();
		R2 = rand();
		value = -2 * std::log(R1);
		if (value > 0) 
			value = sqrt(value);
		else
			value = 0;
		randnValue_ = value * cos( 2 * pi * R2);
		return value * sin( 2 * pi * R2);	
	}
}
//----------------------------------------------------------------------------//

//Exception
//----------------------------------------------------------------------------//
Matrix:: IndexOut:: IndexOut(int i, int size)
{	
	stringStream << "[Matrix::IndexOut] Index is out of matrix range: "
	             << "try to access index [" << i 
	             << "], but it should be number between [0] and [" << size - 1 
	             << "].";
}
//----------------------------------------------------------------------------//
Matrix:: IndexOut:: IndexOut(int i, int j, int iSize, int jSize)
{
	stringStream << "[Matrix::IndexOut] Index is out of matrix range: "
	             << " try to access index [" << i  << "][" << j 
	             << "], but it should be between [0][0] and ["
	             << iSize - 1 << "][" << jSize - 1 << "].";
}
//----------------------------------------------------------------------------//
Matrix:: RowConstraint:: RowConstraint( int r)
{
	stringStream << "[Matrix::RowConstraint] Attempt to set number of rows to "
	             << r << " - must be integer between 0 and " << maxRows_ << ".";
}
//----------------------------------------------------------------------------//
Matrix:: ColConstraint:: ColConstraint( int c)
{
	stringStream << "[Matrix::ColConstraint] Attepmt to set number of cols to "
	             << c << " - must be integer between 0 and " << maxCols_ << ".";
}
//----------------------------------------------------------------------------//
Matrix:: NotVector:: NotVector(const char* calledFunction)
{
	stringStream << "[Matrix::NotVector] Function " << calledFunction 
	             << " can be used only with vector "
	             << "(matrix of size 1xN or Nx1).";
}
//----------------------------------------------------------------------------//
Matrix:: WrongParameter:: WrongParameter( const char* p, const char* f)
{
	stringStream << "[Matrix::WrongParameter] Used wrong parameter " << p 
	             << " in function " << f
	             << ". See documentation for parameter requirments.";
}
//----------------------------------------------------------------------------//
Matrix:: BadSelection:: BadSelection(int r1, int r2, int rSize, 
                                     int c1, int c2, int cSize)
{
	stringStream << "[Matrix::BadSelection] Selected submatrix is from [" << r1 
	             << "][" << c1 << "] to [" << r2 << "][" << c2 
	             << "], but it should be beetween [0][0] and [" << rSize - 1 
	             << "][" << cSize - 1 << "]"	
	             << " and first index cannot be higher than second!";
}
//----------------------------------------------------------------------------//
Matrix:: SizeMismatch:: SizeMismatch(const Matrix& a, const Matrix& b, 
                                     const char* calledFunction)
{
	stringStream << "[Matrix::SizeMismatch] " << calledFunction 
	             << " cannot be executed because first matrix is "
	             << a.rows() << "x" << a.cols() << " and second is " << b.rows()
	             << "x" << b.cols() << ".";
}
//----------------------------------------------------------------------------//
Matrix:: NanAssign:: NanAssign( const char* function)
{
	stringStream << "[Matrix::NanAssign] Attempt of assign NaN value to matrix "
	             << "in function: " << function << ".";
}
//----------------------------------------------------------------------------//
Matrix:: NanOperand:: NanOperand( const char* f)
{
	stringStream << "[Matrix::NanOperand] Function " << f 
	             << " would generated NaN value in matrix.";
}
//----------------------------------------------------------------------------//
Matrix:: CantReshape:: CantReshape(int r1, int c1, int r2, int c2)
{
	stringStream << "[Matrix::CantReshape] Cant reshape matrix [" << r1 << "x" 
	             << c1 << "] to [" << r2 << "x" << c2 << "]."; 
}
//----------------------------------------------------------------------------//
Matrix:: CantScale:: CantScale(int r1, int c1, int r2, int c2)
{
	stringStream << "[Matrix::CantScale] Cant scale from matrix " << r1 << "x" 
	             << c1 << " to matrix " << r2 << "x" << c2 << ".";
}
//----------------------------------------------------------------------------//
Matrix:: UnrecognizedOptimization:: UnrecognizedOptimization()
{
	stringStream << "[Matrix::UnrecognizedOptimization] OPTIMIZATION macro "
	             << "has unrecognized value. Check the \"matrix.h\" file to "
	             << "set it properly and rebuild your project.";
}
//----------------------------------------------------------------------------//
Matrix:: UniformOrders:: UniformOrders( const char* f)
{
	stringStream << "[Matrix::UniformOrders] Function " << f << "cannot be "
	             << "executed because UNIFORM_ORDERS option is set " 
	             << (UNIFORM_ORDERS ? "on":"off") << ". Check the \"matrix.h\""
	             << " file to set it properly and rebuild your project.";
}
//----------------------------------------------------------------------------//
