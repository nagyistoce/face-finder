//----------------------------------------------------------------------------//
//                         USE-COUNTED HANDLE CLASS                           //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 1.57                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  1.57 - system for keeping objects  data unique              // 09.10.2009 //
//	1.16 - removed functions which were not genric              // 02.11.2009 //
//  1.04 - address operator, relation operators                 // 20.08.2009 //
//  1.03 - extend functionality for handling constant objects   // 19.08.2009 //
//  1.00 - generic fuctionality                                 // 15.08.2009 //
//----------------------------------------------------------------------------//

#ifndef COUNTED_HANDLE
#define COUNTED_HANDLE


//Perfomance switches
//----------------------------------------------------------------------------//
//ON (safer)
#define ON 1 
//OFF (faster)
#define OFF 0

//BOUND_CHECK
//Checks if handle_ is bounded with an object, almost certainly can be turn off
#define BOUND_CHECK OFF

//AUTO_SEPARATION
/* Ensure that object won't change data if they are shared by oher objects by
making its own copy of data. */
#define AUTO_SEPARATION ON

//Includes
//----------------------------------------------------------------------------//
#include "Exception.h"


//! General use-counted handle class
//----------------------------------------------------------------------------//
/*! Class manages pointers to type T objects in order to avoid unnecessary 
    data copying. For compiling options see header file. */
template <typename T> class CountedHandle 
{
	public:

		/*! Creates empty handle for later use. Must be assigned to something. */
		CountedHandle(); 

		/*! Build CountedHandle object pointing to given handle
		    \param handle pointer to desired object 
		    \param users number of other objects which already use that handle.
		                 This parameter can be omited most of time, but in order to
                     avoid object deletion in CountedHandle destructor set it to
                     no less than 1.*/
		CountedHandle( T* handle, int users = 0);

		/*!Copy constructor - dont copy countedHandle data, share them instead. */
		CountedHandle( const CountedHandle& countedHandle);

		/*! Destroys handled data if no other object use them. */
		~CountedHandle();

		/*! Assign operator - dont copy countedHandle, just shares data with it. */
		CountedHandle& operator = (const CountedHandle& countedHandle);
		
		/*! Returning object which handle is pointing to.*/
		T& operator * ();

		/*! Returning object which handle is pointing to.*/
		const T& operator * () const;
		
		/*! If class handles pointer to other class this would give access to 
    its members. Shouldn't be use otherwise or would cause memory 
    runtime errors.*/
		T* operator -> ();

		/*! If class handles pointer to other class this would give access to 
    its members. Shouldn't be use otherwise or would cause memory 
    runtime errors.*/
		const T* operator -> () const;

		/*! \Return true if both CountedHandle objects points to the same 
		space in memory. */
		bool operator == (const CountedHandle& h) const;

		/*! Returns the oposite of equality operator. */
		bool operator != (const CountedHandle& h) const;

	protected:

#if AUTO_SEPARATION
		/*! Ensures that objects use unique data. */
		void makeDataUnique();

		/*! Makes full copy of given handle to object*/
		T* copy( T* handle);
#endif

	private:
		T* handle_;
		size_t* useCounter_;
		void clean_(); //safely cleans memory
		class UnboundHandle : public Exception { public: UnboundHandle(); };
};
//----------------------------------------------------------------------------//
template <typename T> 
CountedHandle<T>:: CountedHandle() 
	: handle_(0), useCounter_(new size_t(1))
{
	//nothing to do
};
//----------------------------------------------------------------------------//
template <typename T>
CountedHandle<T>:: CountedHandle( T* handle, int users) 
	: handle_( handle), useCounter_(new size_t( users + 1)) 
{
	//nothing to do
};
//----------------------------------------------------------------------------//
template <typename T>
CountedHandle<T>:: CountedHandle( const CountedHandle& countedHandle)
	: handle_( countedHandle.handle_), useCounter_( countedHandle.useCounter_)
{
	++*useCounter_; //dereference and increment
}
//----------------------------------------------------------------------------//
template <typename T>
CountedHandle<T>:: ~CountedHandle()
{
	clean_();
}
//----------------------------------------------------------------------------//
template <typename T>
CountedHandle<T>& 
CountedHandle<T>:: operator = (const CountedHandle& countedHandle)
{
	++*countedHandle.useCounter_; //self-assignment prone
	clean_();
	handle_ = countedHandle.handle_;
	useCounter_ = countedHandle.useCounter_;
	return *this;
}
//----------------------------------------------------------------------------//

template <typename T> 
inline T& CountedHandle<T>:: operator * ()
{
#if BOUND_CHECK
	if (!handle_)
		throw UnboundHandle();
	else
#endif
#if AUTO_SEPARATION
	makeDataUnique();
#endif
	return *handle_;
}
//----------------------------------------------------------------------------//
template <typename T> 
inline const T& CountedHandle<T>:: operator * () const
{
#if BOUND_CHECK
	if (!handle_)
		throw UnboundHandle();
	else
#endif
	return *handle_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline T* CountedHandle<T>:: operator -> ()
{
#if BOUND_CHECK
	if (!handle_)
		throw UnboundHandle();
	else
#endif
#if AUTO_SEPARATION
	makeDataUnique();
#endif
	return handle_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline const T* CountedHandle<T>:: operator -> () const
{
#if BOUND_CHECK
	if (!handle_)
		throw UnboundHandle();
	else
#endif
	return handle_;
}

//----------------------------------------------------------------------------//
template <typename T>
inline bool CountedHandle<T>:: operator == (const CountedHandle<T>& h) const
{
	return handle_ == h.handle_;
}
//----------------------------------------------------------------------------//
template <typename T>
inline bool CountedHandle<T>:: operator != (const CountedHandle<T>& h) const
{
	return handle_ != h.handle_;
}
//----------------------------------------------------------------------------//


#if AUTO_SEPARATION
//----------------------------------------------------------------------------//
template <typename T>
void CountedHandle<T>:: makeDataUnique()
{
	if (*useCounter_ > 1) //handle is shared
	{
		//cout << "CountedHandle::makeDataUnique(): copying data.\n";
		//object wont use that handle anymore
		--*useCounter_; 
		useCounter_ = new size_t(1);
    	handle_ = handle_ ? copy( handle_) : 0;	
	}
}
//----------------------------------------------------------------------------//
template <typename T>
inline T* CountedHandle<T>:: copy( T* handle)
{
	//assuming that T object has proper copy constructor
	return new T( *handle);
}
//----------------------------------------------------------------------------//
#endif


//----------------------------------------------------------------------------//
template <typename T>
inline void CountedHandle<T>:: clean_()
{
	if (--*useCounter_ == 0)
	{
		delete useCounter_;
		delete handle_;
	}
}
//----------------------------------------------------------------------------//
template <typename T>
CountedHandle<T>:: UnboundHandle:: UnboundHandle()
{
	stringStream << "CountedHandle:: No object bound to handle.";
}
//----------------------------------------------------------------------------//

#endif //COUNTED_HANDLE
