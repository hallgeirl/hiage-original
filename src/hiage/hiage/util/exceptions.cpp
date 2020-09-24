/*!
	\file com_exceptions.cpp

	\author Hallgeir Lien

	Contains implementation of Exception, IOException and FatalException classes.
*/

#include "exceptions.h"

using namespace hiage;
using namespace std;


Exception::Exception(string msg, bool print, ostream &stream) : runtime_error(msg), outstream(stream)
{
	//print to console, and to the specified stream
	if (print)
	{
		outstream << what() << endl;
		cout << what() << endl;
	}
	message = msg;
}

Exception::~Exception() throw()
{
}


/*!
	Prints the stored message to the stream passed in the constructor, or to another stream if specified.
*/
void Exception::print()
{
	outstream << what() << endl;
}

IOException::IOException(string msg, bool print, ostream &stream) : Exception(msg, print, stream)
{

}

FatalException::FatalException(string msg, bool print, ostream &stream) : Exception(msg, print, stream)
{
}
