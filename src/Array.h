//----------------------------------------------------------------------------//
//                             ARRAY MANAGMENT                                //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 1.02                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  1.02 - sharing data functionality                           // 09.10.2009 //
//  1.01 - removed meaningless operators                        // 02.10.2009 //
//  1.00 - operation on array of given type, class provides                   //
//         compability with GMArray                             // 01.10.2009 //
//----------------------------------------------------------------------------//

#ifndef ARRAY_H
#define ARRAY_H


//----------------------------------------------------------------------------//
//! Arrays of T type with normal memory usage
/*! Class provides transparent interface for operating on T type array similar 
to that frm GMArray class and exists for compability purposes. */
template <typename T> class Array
{
  public:

	/*! Creates object with size memory allocated in CPU memory. */	
	Array( size_t size);

	/*! Shared array constructor. Sharing must be managed from higher level.
	Array memory allocation won't be released in created object. */
	Array( T* array, size_t size);
	
	/*! Creates exact copy of given array. */
	Array( const Array& array);

	/*! Release allocated space in memory. */
	~Array();

	/*! \return address of CPU memory array. */
	T* operator& ();
	
	/*! \return const pointer with address of CPU memory array. */
	const T* operator& () const;

	/*! \return reference to array element at given index. */
	T& operator[] (int index);
	
	/*! \return const reference to array element at given index. */
	const T& operator[] (int index) const;

	/*! Compares addresses of arrays alocated in CPU memory. */
	bool operator == (const Array& a);
	
	/*! Compares addresses of arrays alocated in CPU memory. */
	bool operator != (const Array& a);

  protected: /* Class is not designed for inheriting */

  private:

	/*! Simple, contiguos array of T type stored in CPU memory. */
	T* array_;

	/*! Number of array elements */
	size_t size_;
	
	/*! Flags for shared objects, no memory dealocation will be performed */
	bool sharedMemory_;
};
//----------------------------------------------------------------------------//


//Template class - all delarations must be in header file
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
template <typename T>
Array<T>:: Array( size_t size)
{
	size_ = size;
	array_ = new T[size_];
	sharedMemory_ = false;
}
//----------------------------------------------------------------------------//
template <typename T>
Array<T>:: Array( T* array, size_t size)
{
	size_ = size;
	array_ = array;
	sharedMemory_ = true;
}
//----------------------------------------------------------------------------//
template <typename T>
Array<T>:: Array( const Array& array)
{
	size_ = array.size_;
	array_ = new T[size_];
	for (register size_t i = 0; i < size_; i++)
		array_[i] = array.array_[i];
	sharedMemory_ = false;
}
//----------------------------------------------------------------------------//
template <typename T>
Array<T>:: ~Array()
{
	if (array_ && !sharedMemory_) delete[] array_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline T* Array<T>:: operator& ()
{
	return array_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline const T* Array<T>:: operator& () const
{
	return array_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline T& Array<T>:: operator[] (int index)
{
	return array_[index];
}
//----------------------------------------------------------------------------//
template <typename T>
inline const T& Array<T>:: operator[] (int index) const
{
	return array_[index];
}
//----------------------------------------------------------------------------//
template <typename T>
inline bool Array<T>:: operator == (const Array& a)
{
	return array_ == a.array_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline bool Array<T>:: operator != (const Array& a)
{
	return array_ != a.array_;
}
//----------------------------------------------------------------------------//

#endif //GMARRAY_H
