#include "FaceDatabase.h"
#include <iostream>

//Individual
//----------------------------------------------------------------------------//
Individual:: Individual()
{
	id_ = -1;
	image_ = 0;
	imageLength_ = 0;
}
//----------------------------------------------------------------------------//
Individual:: Individual( const Individual& individual)
{
	id_         = individual.id_;	
	name_       = individual.name_;
	description_ = individual.description_;
	imageLength_ = individual.imageLength_;
	image_ = new char[ imageLength_];
	for (int i = 0; i < imageLength_; i++)
		image_[i] = individual.image_[i];
}
//----------------------------------------------------------------------------//
Individual:: Individual( int id, const char* image, int imageLength, 
		                 const string& name, const string& description)
{
	id_          = id;
	name_       = name;		
	description_ = description;
	imageLength_ = imageLength;
	image_ = new char[ imageLength_];
	for (int i = 0; i < imageLength_; i++)
		image_[i] = image[i];
}
//----------------------------------------------------------------------------//
Individual:: ~Individual()
{
	if (image_) 
		delete[] image_;
}
//----------------------------------------------------------------------------//
void Individual:: setImage( const char* imagePath)
{
	fstream file( imagePath, ios::in | ios::binary);
	if (!file) throw FileNotOpen( imagePath);
	char data[INDIVIDUAL_IMAGE_MAX_SIZE];
	file.read( data, INDIVIDUAL_IMAGE_MAX_SIZE);
	if (!file.eof()) throw ImageTooBig();
	imageLength_ = file.gcount();
	if (image_) delete[] image_;
	image_ = new char[ imageLength_];
	for (int i = 0; i < imageLength_; i++)
		image_[i] = data[i];
}
//----------------------------------------------------------------------------//
void Individual:: load( fstream& file)
{
	int s;
	if (image_) delete[] image_; //delete old image data
	file.read( (char*)&id_, sizeof(int)); //id
	file.read( (char*)&s, sizeof(int));  //title
	image_ = new char[s]; //memory allocate temporalily for name
	file.read( image_, s * sizeof(char));
	name_ = string(image_, s);
	delete[] image_;
	file.read( (char*)&s, sizeof(int)); //description
	image_ = new char[s]; //memory allocate temporarily for description
	file.read( image_, s * sizeof(char));
	description_ = string(image_, s);
	delete[] image_;
	file.read( (char*)&imageLength_, sizeof(int)); //image
	image_ = new char[imageLength_]; //new image data memory space
	file.read( image_, imageLength_ * sizeof(char));
}
//----------------------------------------------------------------------------//
void Individual:: save( fstream& file)
{
	int s;
	file.write( (char*)&id_, sizeof(int));           //id	
	s = name_.length();                             //title
	file.write( (char*)&s, sizeof(int));
	file.write( name_.c_str(), s * sizeof(char));
	s = description_.length();                       //description
	file.write( (char*)&s, sizeof(int));
	file.write( description_.c_str(), s * sizeof(char));		
	file.write( (char*)&imageLength_, sizeof(int));  //image
	file.write( image_, imageLength_ * sizeof(char));
}
//----------------------------------------------------------------------------//
Individual& Individual:: operator = (const Individual& individual)
{
	id_          = individual.id_;	
	name_        = individual.name_;
	description_ = individual.description_;
	imageLength_ = individual.imageLength_;
	if (image_) delete[] image_;
	image_ = new char[ imageLength_];
	for (int i = 0; i < imageLength_; i++)
		image_[i] = individual.image_[i];
	return *this;
}
//----------------------------------------------------------------------------//
bool operator < (const Individual& a, const Individual& b)
{
	return a.id_ < b.id_;
}
//----------------------------------------------------------------------------//
bool operator == (const Individual& a, const Individual& b)
{
	return a.id_ == b.id_;
}
//----------------------------------------------------------------------------//
Individual:: FileNotOpen:: FileNotOpen(const char* f)
{
	stringStream << "Individual image load: couldnt open file: " << f;
}
//----------------------------------------------------------------------------//
Individual:: ImageTooBig:: ImageTooBig()
{
	stringStream << "Maximum image size is " <<	INDIVIDUAL_IMAGE_MAX_SIZE
	             << " bytes (" << INDIVIDUAL_IMAGE_MAX_SIZE / 1024
	             << " kB).";
}
//----------------------------------------------------------------------------//

//face Database
//----------------------------------------------------------------------------//
FaceDatabase:: FaceDatabase()
{
	index_ = database_.begin();
}
//----------------------------------------------------------------------------//
FaceDatabase:: FaceDatabase( const FaceDatabase& d)
{
	database_ = d.database_;
	index_ = database_.begin();
}
//----------------------------------------------------------------------------//
FaceDatabase::~FaceDatabase()
{
	database_.clear();
}
//----------------------------------------------------------------------------//
Individual& FaceDatabase:: next()
{
	if (database_.empty()) return empty_;
	if (++index_ == database_.end()) index_--; 
	return *index_;
}
//----------------------------------------------------------------------------//
Individual& FaceDatabase:: prev()
{
	if (database_.empty()) return empty_;
	if (index_ != database_.begin()) index_--;
	return *index_;
}
//----------------------------------------------------------------------------//
void FaceDatabase:: editCurrent( const Individual& individual)
{
	if (database_.empty()) return;
	*index_ = individual;
}
//----------------------------------------------------------------------------//
void FaceDatabase:: removeCurrent()
{
	if (database_.empty()) return;
	database_.remove( *index_);
	index_ = database_.begin();
}
//----------------------------------------------------------------------------//
Individual& FaceDatabase:: findId(int id)
{
	if (database_.empty()) return empty_;
	list<Individual>::iterator i;
	for( i = database_.begin(); i != database_.end(); i++)
	{
		if ( ((Individual)*i).id() == id) 
		{
			index_ = i;
			return *i;	
		}
	}
	throw IdNotFound( id);
}
//----------------------------------------------------------------------------//
Individual& FaceDatabase:: findName( const std::string& name)
{
	if (database_.empty()) return empty_;
	list<Individual>::iterator i;
	for( i = database_.begin(); i != database_.end(); i++)
	{
		if ( ((Individual)*i).name() == name) 
		{
			index_ = i;
			return *i;	
		}
	}
	throw NameNotFound( name.c_str());
} 
//----------------------------------------------------------------------------//
void FaceDatabase:: addIndividual( const Individual& individual)
{
	database_.push_back( individual);
}
//----------------------------------------------------------------------------//
void FaceDatabase:: clear()
{
	database_.clear();
}
//----------------------------------------------------------------------------//
void FaceDatabase:: load( const char* filePath)
{
	fstream f;
	f.open( filePath, ios::in | ios::binary);
	if (!f) throw FileOpenRead( filePath);
	char header[] = "xxx\0";
	f.read( header, sizeof(char) * 3);
	if (string(header).compare("FDB") != 0) throw FileWrongHeader( filePath);
	database_.clear();
	int size;
	Individual individual;
	f.read( (char*)&size, sizeof(int));
	for(int i = 0; i < size; i++)
	{
		individual.load( f);
		database_.push_back( individual);
	} 		
	f.close();
	index_ = database_.begin();
	std::cout << "[FaceDatabase] loaded from: " << filePath << std::endl;
}
//----------------------------------------------------------------------------//
void FaceDatabase:: save( const char* filePath)
{
	if (database_.empty()) return;
	fstream f;
	f.open( filePath, ios::out | ios::binary);
	if (!f) throw FileOpenWrite( filePath); //cant open for writing
	char header[] = "FDB"; //Face Data Base file header
	f.write( header, sizeof(char) * 3);
	int size = database_.size();
	f.write( (char*)&size, sizeof(int));
	list<Individual>::iterator i;
	Individual individual;
 	for (i = database_.begin(); i != database_.end(); i++)
 	{
 		individual = *i;
 		individual.save( f);
	}
	f.close();
}
//----------------------------------------------------------------------------//


//FaceDatabase exceptions
//----------------------------------------------------------------------------//
FaceDatabase:: FileOpenWrite:: FileOpenWrite( const char* f)
{
	stringStream << "Face Database Error. File: " << f 
	             << "couldn't be open in write-binary mode.";
}
//----------------------------------------------------------------------------//
FaceDatabase:: FileOpenRead:: FileOpenRead( const char* f)
{
	stringStream << "Face Database Error. File: "<< f
	             << "couldn't be open in read-binary mode.";
}
//----------------------------------------------------------------------------//
FaceDatabase:: FileWrongHeader:: FileWrongHeader( const char* f)
{
	stringStream << "Face Database Error. File: "<< f 
	             << " is not face-database file (has wrong header).";
}
//----------------------------------------------------------------------------//
FaceDatabase:: IdNotFound:: IdNotFound( int id)
{
	stringStream << "Individual with id " << id << " is not in face database.";
}
//----------------------------------------------------------------------------//
FaceDatabase:: NameNotFound:: NameNotFound( const char* name)
{
	stringStream << "Indivudual named \"" << name 
	             << "\" does not exist in database";
}
//----------------------------------------------------------------------------//
