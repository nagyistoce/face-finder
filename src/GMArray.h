//----------------------------------------------------------------------------//
//                             GPU MEMORY ARRAY                               //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 1.7                                                              //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  1.7 - CUBLAS exceptions                                     // 03.10.2009 //
//  1.4 - add system of array synchronization to avoid                        //
//        unnecessery data copying between CPU and GPU memories // 01.10.2009 //
//  1.0 - Class for alloacating  arrays in both CPU memory                    //
//        and GPU memory neede for GPGPU operations             // 30.09.2009 // 
//----------------------------------------------------------------------------//

#ifndef GMARRAY_H
#define GMARRAY_H

//OPTIONS
#define ON 1
#define OFF 0
//----------------------------------------------------------------------------//
// AUTO_SYNCHRONIZATION
/* If auto synchronization is turn on it adds some extra code in functions
that can modify array content to assure that good version of data is in use. */
#define AUTO_SYNCHRONIZATION ON
//----------------------------------------------------------------------------//
// ALWAYS_USE_GPU
/* Defines default behavior of using Gpu memory space. If ON all created 
objects will have gpu memory allocated (useGpuMemory_  == true). If this macro
is set to OFF value using of GPU memory must be enabled manually with
function useGpuMemory( bool ). Note that when object wont allocate GPU memory
either automatical nor manual there is no machanism of checking this in 
functions that operates on GPU data. */
#define ALWAYS_USE_GPU ON
//----------------------------------------------------------------------------//


//Includes
//----------------------------------------------------------------------------//
#include <cublas.h> //CUBLAS interface for using GPU memory
#include "Exception.h" 


//----------------------------------------------------------------------------//
//! Arrays of T type with dual memory usage
/*! Class provides transparent interface for operating on T type array which 
are intended to be used in GPGPU operations what requires allocation on 
graphical memory in addition to memory allocated in CPU memory. For better 
performance there is automatic synchronization between these two memory
areas. When its active remeber that calling subscript operator in loops is not
efficient anymore, because of synchronization condition checking. But you can 
turn auto synchronization off and use functions setGpuMemory 
and getGpuMemory manually in your code instead. For compilation options see header file. */
template <typename T> class GMArray
{
  public:

	/*! Creates object with size memory allocated in CPU and GPU memories. */	
	GMArray( size_t size);

	/*! Shared array constructor. Sharing must be handeled from higher level. 
	Array memory allocation won't be released in created object. Shared memory 
	concerns only CPU memory space, GPU memory is always unique. */
	GMArray( T* array, size_t size);

	/*! Creates exact copy of given array. */
	GMArray( const GMArray& gmarray);

	/*! Release allocated space in both types of memory. */
	~GMArray();

	/*!User can decide that no GPGPU operations will be excuted on
	current object and graphic memory can be released. 
	\param flag boolean value, true if object should to use GPU memory, 
	false if not */
	void useGpuMemory( bool flag);
	bool usingGpuMemory() const; 
	
	/*! Copy values from CPU memory array to array in GPU memory. 
	Required before GPGPU operations. */
	void setGpuMemory() const;

	/*! Copy values from array in graphic memory to array in CPU memory.
	Required after GPGPU operations. */
	void getGpuMemory() const;

	/*! \return pointer to array in GPU memory. */
	T* gpuMemoryArray();

	/*! \return const pointer to array in GPU memory. */
	const T* gpuMemoryArray() const;

	/*! \return address of CPU memory array. */
	T* operator& ();
	
	/*! \return const pointer with address of CPU memory array. */
	const T* operator& () const;

	/*! \return reference to array element at given index. */
	T& operator[] (int index);
	
	/*! \return const reference to array element at given index. */
	const T& operator[] (int index) const;

	/*! Compares addresses of arrays alocated in CPU memory memory. */
	bool operator == (const GMArray& gmArray);
	
	/*! Compares addresses of arrays alocated in CPU memory memory. */
	bool operator != (const GMArray& gmArray);

  protected: /* Class is not designed for inheriting */

  private:
	
	void gpuAlloc();
	void gpuFree();

	/*! Simple, contiguos array of T type stored in CPU memory. */
	T* array_;

	/*! Pointer to memory allocated on GPU memory. */
	T* gpuMemArray_;

	/*! Number of array elements */
	size_t size_;
	
	/*! Indicates if objects uses graphic memory. */
	bool useGpuMemory_;

#if AUTO_SYNCHRONIZATION
	/*! Arrays auto-synchronization flag indicates that array in cpu memory
	is up-to-date. */
	mutable bool cpuMemUpdated_;

	/*! Arrays auto-synchronization flag indicates that array in gpu memory
	is up-to-date. */
	mutable bool gpuMemUpdated_;
#endif

	/*! Flags for shared objects, no memory dealocation will be performed */
	bool sharedMemory_;

	/*! Number of objects using memory from graphic card. */
	static int objectsInGpuMemory_;
	
	mutable cublasStatus gpuStatus_;
	class CublasError : public Exception { public: CublasError(cublasStatus s); };

};
//----------------------------------------------------------------------------//

//Definition of objectsInGpuMemory_
//----------------------------------------------------------------------------//
template <typename T>
int GMArray<T>:: objectsInGpuMemory_ = 0;

//Template class - all delarations must be in header file
//----------------------------------------------------------------------------//
template <typename T>
GMArray<T>:: GMArray( size_t size)
{
	size_ = size;
	array_ = new T[size_];
	sharedMemory_ = false;	
#if AUTO_SYNCHRONIZATION
	cpuMemUpdated_ = true;
	gpuMemUpdated_ = true;
#endif
#if ALWAYS_USE_GPU
	useGpuMemory_  = true;
	gpuAlloc();
#else
	useGpuMemory_  = false;
#endif
}
//----------------------------------------------------------------------------//
template <typename T>
GMArray<T>:: GMArray( T* array, size_t size)
{
	size_ = size;
	array_ = array; 
	sharedMemory_ = true;
#if AUTO_SYNCHRONIZATION
	cpuMemUpdated_ = true;
	gpuMemUpdated_ = false;
#endif	
#if ALWAYS_USE_GPU
	useGpuMemory_  = true;
	gpuAlloc();
#else
	useGpuMemory_  = false;
#endif
}
//----------------------------------------------------------------------------//
template <typename T>
GMArray<T>:: GMArray( const GMArray& gmarray)
{
	size_ = gmarray.size_;
	array_ = new T[size_];
	for (unsigned int i = 0; i < size_; i++)
		array_[i] = gmarray.array_[i];
	sharedMemory_ = false;
#if AUTO_SYNCHRONIZATION
	cpuMemUpdated_ = true;
	gpuMemUpdated_ = false;
#endif
#if ALWAYS_USE_GPU
	useGpuMemory_  = true;
	gpuAlloc();
#else
	useGpuMemory_  = false;
#endif
}
//----------------------------------------------------------------------------//
template <typename T>
GMArray<T>:: ~GMArray()
{
	if (array_ && !sharedMemory_) 
		delete[] array_;	
	if (useGpuMemory_)
		gpuFree();
}
//----------------------------------------------------------------------------//
template <typename T>
void GMArray<T>:: useGpuMemory( bool flag)
{
	if (useGpuMemory_ && !flag) //turn off
	{
#if AUTO_SYNCHRONIZATION
		if (!cpuMemUpdated_) getGpuMemory();
#endif
		gpuFree();
	}
	if (!useGpuMemory_ && flag) //turn on
	{
		gpuAlloc();
#if AUTO_SYNCHRONIZATION
		gpuMemUpdated_ = false;
#endif
	}
	useGpuMemory_ = flag;
}
//----------------------------------------------------------------------------//
template <typename T>
inline bool GMArray<T>:: usingGpuMemory() const
{
	return useGpuMemory_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline void GMArray<T>:: setGpuMemory() const
{
#if AUTO_SYNCHRONIZATION
	gpuMemUpdated_ = true;	
#endif		
	cublasSetVector( size_, sizeof(T), array_, 1, gpuMemArray_, 1);
	gpuStatus_ = cublasGetError();
	if (gpuStatus_ != CUBLAS_STATUS_SUCCESS) throw CublasError( gpuStatus_);
}
//----------------------------------------------------------------------------//
template <typename T>
inline void GMArray<T>:: getGpuMemory() const
{
#if AUTO_SYNCHRONIZATION
	cpuMemUpdated_ = true;
#endif	
	cublasGetVector( size_, sizeof(T), gpuMemArray_, 1, array_, 1);
	gpuStatus_ = cublasGetError();
	if (gpuStatus_ != CUBLAS_STATUS_SUCCESS) throw CublasError( gpuStatus_);
}
//----------------------------------------------------------------------------//
template <typename T>
inline T* GMArray<T>:: gpuMemoryArray()
{
#if AUTO_SYNCHRONIZATION
	if (!gpuMemUpdated_) 
		setGpuMemory();
	cpuMemUpdated_ = false;
#endif
	return gpuMemArray_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline const T* GMArray<T>:: gpuMemoryArray() const
{
#if AUTO_SYNCHRONIZATION
	if (!gpuMemUpdated_) 
		setGpuMemory();
#endif
	return gpuMemArray_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline T* GMArray<T>:: operator& ()
{
#if AUTO_SYNCHRONIZATION
	if (!cpuMemUpdated_) 
		getGpuMemory();
	gpuMemUpdated_ = !useGpuMemory_;
#endif
	return array_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline const T* GMArray<T>:: operator& () const
{
#if AUTO_SYNCHRONIZATION
	if (!cpuMemUpdated_) 
		getGpuMemory();
#endif
	return array_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline T& GMArray<T>:: operator[] (int index)
{
#if AUTO_SYNCHRONIZATION
	if (!cpuMemUpdated_) 
		getGpuMemory();
	gpuMemUpdated_ = !useGpuMemory_;
#endif
	return array_[index];
}
//----------------------------------------------------------------------------//
template <typename T>
inline const T& GMArray<T>:: operator[] (int index) const
{
#if AUTO_SYNCHRONIZATION
	if (!cpuMemUpdated_) 
		getGpuMemory();
#endif
	return array_[index];
}
//----------------------------------------------------------------------------//
template <typename T>
inline bool GMArray<T>:: operator == (const GMArray& gmArray)
{
	return array_ == gmArray.array_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline bool GMArray<T>:: operator != (const GMArray& gmArray)
{
	return array_ != gmArray.array_;
}
//----------------------------------------------------------------------------//
template <typename T>
void GMArray<T>:: gpuAlloc()
{
	//if this is first object must initialize GPU
	if (objectsInGpuMemory_++ == 0)
	{	
		cout << "[GMArray] CUDA initialization: "; cout.flush();
		gpuStatus_ = cublasInit();
		if (gpuStatus_ != CUBLAS_STATUS_SUCCESS) throw CublasError( gpuStatus_);
		cout << "done.\n";
	}
	gpuStatus_ = cublasAlloc( size_, sizeof(T), 
	                          reinterpret_cast<void**>( &gpuMemArray_));
	if (gpuStatus_ != CUBLAS_STATUS_SUCCESS) throw CublasError( gpuStatus_);
}
//----------------------------------------------------------------------------//
template <typename T>
void GMArray<T>:: gpuFree()
{
	if (!gpuMemArray_) return;
	gpuStatus_ = cublasFree( gpuMemArray_);
	if (gpuStatus_ != CUBLAS_STATUS_SUCCESS) throw CublasError( gpuStatus_);
	//if this is the last object close GPU connection
	if (--objectsInGpuMemory_ == 0)
	{
		cout << "[GMArray]  CUDA shuting down: "; cout.flush();
		gpuStatus_ = cublasShutdown();
		if (gpuStatus_ != CUBLAS_STATUS_SUCCESS) throw CublasError( gpuStatus_);
		cout << "done.\n";
	}
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
template <typename T>
GMArray<T>:: CublasError:: CublasError(cublasStatus s)
{
	stringStream << "[CUBLAS::";
	switch (s)
	{
		case CUBLAS_STATUS_NOT_INITIALIZED:
			stringStream << "CUBLAS_STATUS_NOT_INITIALIZED] "
			             << "Library is not initialized";
			break;
		case CUBLAS_STATUS_ALLOC_FAILED:
			stringStream << "CUBLAS_STATUS_ALLOC_FAILED] "
			             << "Resource allocation failed";
			break;
		case CUBLAS_STATUS_INVALID_VALUE:
			stringStream << "CUBLAS_STATUS_INVALID_VALUE] "
			             << "Unsupported numerical value "
			             << "was passed to function.";
			break;
		case CUBLAS_STATUS_ARCH_MISMATCH:
			stringStream << "CUBLAS_STATUS_ARCH_MISMATCH] "
			             << "Function requires architectural feature absent from "
			             << "architecture of the device.";
			break;
		case CUBLAS_STATUS_MAPPING_ERROR:
			stringStream << "CUBLAS_STATUS_MAPPING_ERROR] "
			             << "Access to GPU memory space failed.";
			break; 
		case CUBLAS_STATUS_EXECUTION_FAILED:
			stringStream << "CUBLAS_STATUS_EXECUTION_FAILED] "
			             << "GPU program failed to execute.";
			break;
		case CUBLAS_STATUS_INTERNAL_ERROR:
			stringStream << "CUBLAS_STATUS_INTERNAL_ERROR] "
			             << "An internal CUBLAS operation failed.";
			break;
		default:
			stringStream << "] Unknown error code: " << (int)s << ".";
			break;
	}
}
//----------------------------------------------------------------------------//

#endif //GMARRAY_H
