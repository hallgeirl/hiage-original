/*!
	\file com_exceptions.h

	\author Hallgeir Lien

	Contains declaration of Exception, IOException and FatalException classes.
*/

#pragma once

#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>

//classes for handling exceptions
namespace hiage
{
	/*!
		Base exception class. For generic exceptions.
	*/
	class Exception : public std::runtime_error
	{
	private:
		std::ostream &outstream;
		std::string message;

	public:
		/*!
			Constructor for Exception class.
			\param msg The message to print/store.
			\param print Set to true if you want to print the message to stream. Default is true.
			\param stream The stream to print msg to. Default is std::cerr.
		*/
		Exception(std::string msg, bool print = true, std::ostream &stream=std::cerr);
		virtual ~Exception() throw();

		void print();
	};

	/*!
		Exception class for IO exceptions, for instance errors while opening files, or write/read errors.
	*/
	class IOException : public Exception
	{
	public:
		/*!
			Constructor for IOException class.
			\param msg The message to print/store.
			\param print Set to true if you want to print the message to stream. Default is true.
			\param stream The stream to print msg to. Default is std::cerr.
		*/
		IOException(std::string msg, bool print = true, std::ostream &stream=std::cerr);
	};

	/*!
		Exception class for exceptions the program probably shouldn't attempt to recover from.
		Examples are failure to initialize the specified video mode, initialization errors, memory allocation errors and so on.
	*/
	class FatalException : public Exception
	{
	public:
		/*!
			Constructor for FatalException class.
			\param msg The message to print/store.
			\param print Set to true if you want to print the message to stream. Default is true.
			\param stream The stream to print msg to. Default is std::cerr.
		*/
		FatalException(std::string msg, bool print = true, std::ostream &stream=std::cerr);
	};

	/*!
		Overloaded operator+, to ease exception throwing mainly. Appends an integer to a string.
		\param str The string to append the integer to.
		\param n The integer to append to the string.
		\return The resulting string.
	*/
	inline std::string operator+(std::string str, int n)
	{
		std::stringstream ss;
		ss << str << n;

		return ss.str();
	}

	/*!
		Overloaded operator+, to ease exception throwing mainly. Appends an unsigned integer to a string.
		\param str The string to append the integer to.
		\param n The integer to append to the string.
		\return The resulting string.
	*/
	inline std::string operator+(std::string str, unsigned int n)
	{
		std::stringstream ss;
		ss << str << n;

		return ss.str();
	}

	/*!
		Overloaded operator+, to ease exception throwing mainly. Appends a floating point number to a string.
		\param str The string to append the number to.
		\param n The double precision floating point number to append to the string.
		\return The resulting string.
	*/
	inline std::string operator+(std::string str, double n)
	{
		std::stringstream ss;
		ss << str << n;

		return ss.str();
	}


}
