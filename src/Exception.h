//----------------------------------------------------------------------------//
//                      STANDARD EXCEPTION CHILD CLASS                        //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 0.1                                                              //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  0.1 - inherit from std:: exception and add stringstream object for more   //
//        advanced text operations (01.10.2008)                               //
//----------------------------------------------------------------------------//

#ifndef EXCEPTION_H
#define EXCEPTION_H

//includes
//----------------------------------------------------------------------------//
#include <exception> //standard exception class
#include <sstream>   //standard string streams
using namespace std;

//! Base class for class specific exceptions
/*! Class derived from standard exception class. It operates on string streams
 *  making easy to build more complicated what() messages.
 */
//----------------------------------------------------------------------------//
class Exception : public exception
{
	public:

		Exception() {}
		/*! Creates object with given message. */
		Exception( const char* message) throw() 
			{ stringStream << message; }
		Exception( const Exception& e) 
			{ stringStream.str( e.stringStream.str()); }
		/*! Exception descritption. Class version of virtual function what() from std::exception */
		const char* what() const throw() 
			{ return stringStream.str().c_str(); }
		~Exception() throw() {}
	protected:
		//!  stringstream
		/*! Exception message - std::stringstream object containing message returning by what() function. There message can be built in easy way from various 
		types of data. 
		*/  
		stringstream stringStream;
	private:		
};
//----------------------------------------------------------------------------//

#endif //EXCEPTION_H