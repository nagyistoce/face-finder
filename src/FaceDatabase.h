//----------------------------------------------------------------------------//
//         										FaceDetector Data Base                          //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 0.1                                                              //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  0.1 - basic version                                                       //
//----------------------------------------------------------------------------//

#ifndef FACE_DATABASE_H
#define FACE_DATABASE_H

//Includes
//----------------------------------------------------------------------------//
#include <string>
#include <list>
#include <fstream>
using namespace std;
#include "Exception.h"

//Defines
//----------------------------------------------------------------------------//
#define INDIVIDUAL_IMAGE_MAX_SIZE 20 * 1024
	// set maximal image size to 20 kB
	
//! Class for storing info of human idividuals.
/*! Class stores id, image, name and descrition of one person. Intende to 
use with FaceDatabase container. */
//----------------------------------------------------------------------------//
class Individual
{
	public:
		Individual();
		Individual( const Individual&);
		/*! Creates object containing given info. */
		Individual(int id, const char* image, int imageLength, 
		           const string& name, const string& description);
		~Individual();
		int id() const;
		/*! Returns adress of image buffer for use in graphic class. */
		const char* imageAddress() const;
		/*! Number of bytes stored in image data buffer. */
		const int& imageLength() const;
		const string& name() const;
		const string& description() const;
		void setId( int id);
		void setImage( const char* imagePath);
		void setName( const string& name);
		void setDescription( const string& description);
		/*! Reads data from opened and set file stream. */
		void load( fstream& file);
		/*! Writes data to opened and set file stream. */
		void save( fstream& file);
		Individual& operator = (const Individual&);
		friend bool operator < (const Individual&, const Individual&);
		friend bool operator == (const Individual&, const Individual&);
	
	protected:

	private:
		 
		class FileNotOpen : public Exception 
			{ public: FileNotOpen(const char*); };
		class ImageTooBig : public Exception { public: ImageTooBig(); };
	
		int    id_;          //id number for face recognizer
		char*  image_;       //face image binary data
		int    imageLength_; //length of image buffer
		string name_,       //name of individual
		       description_; //additional info
};


//! Simple set of Individuals 
/*! Container class based on std::list<T> and providing basic interface for 
individuals management. */
//----------------------------------------------------------------------------//
class FaceDatabase
{
	public:
		FaceDatabase();
		FaceDatabase( const FaceDatabase&);
		~FaceDatabase();
		/*! Returns number of individuals in database. */
		int count() const;
		/*! Returns next individual in database. */
		Individual& next();
		/*! Returns previous individual in database. */
		Individual& prev();
		/*! Returns firstindividual in database with given id. User should ensure that all id's are unique.*/
		Individual& findId(int id);
		/*! Returns firstindividual in database with given name.*/
		Individual& findName( const std::string&);
		/*! Replaces idnividual which is currently selected by database pointer with given individual */
		void editCurrent( const Individual&);
		/*! Removes idnividual which is currently selected by database pointer. */
		void removeCurrent();
		/*! Add new individual to list.  */
		void addIndividual( const Individual&);
		/*! REmoves all individuals from list. */
		void clear();
		/*! Loads database from given file location. */
		void load( const char* filePath);
		/*! Saves database to given file location. */
		void save( const char* filePath); 		 
	
	private:
		class FileOpenWrite : public Exception 
				{ public: FileOpenWrite(const char*); };
		class FileOpenRead : public Exception 
				{ public: FileOpenRead(const char*); };
		class FileWrongHeader : public Exception 
				{ public: FileWrongHeader(const char*); };
		class IdNotFound : public Exception	{ public: IdNotFound( int); };
		class NameNotFound : public Exception	
			{ public: NameNotFound( const char*); }; 
			
    	list<Individual> database_;
    	list<Individual>::iterator index_;
    	Individual empty_; 
};

//Inlines
//----------------------------------------------------------------------------//
inline int Individual:: id() const
{
	return id_;
}
//----------------------------------------------------------------------------//
inline const string& Individual:: name() const
{
	return name_;
}
//----------------------------------------------------------------------------//
inline const string& Individual:: description() const
{
	return description_;
}
//----------------------------------------------------------------------------//
inline void Individual:: setId( int id)
{
	id_ = id;
}
//----------------------------------------------------------------------------//
inline const char* Individual:: imageAddress() const
{
	return image_;
}
//----------------------------------------------------------------------------//
inline const int& Individual:: imageLength() const
{
	return imageLength_;
}
//----------------------------------------------------------------------------//
inline void Individual:: setName( const string& name)
{
	name_ = name;
}
//----------------------------------------------------------------------------//
inline void Individual:: setDescription( const string& description)
{
	description_ = description;
}
//----------------------------------------------------------------------------//
inline int FaceDatabase:: count() const
{ 
	return database_.size(); 
}
//----------------------------------------------------------------------------//

#endif //FACE_DATABASE_H
