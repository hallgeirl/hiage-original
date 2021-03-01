/*!
	\file util_vector2.h

	\author Hallgeir Lien

	Contains declaration and implementatino of the Vector2 template class.
*/

#pragma once

#include <cmath>
#include <iostream>
#include "../core/typedefs.h"

namespace hiage
{
	/*!
		Template class for a vector class with two dimensions.
	*/
	template <typename T> class __IMPORTEXPORT Vector2
	{
	private:
		T x, y;
	public:
		/*!
			Initializes the vector to (0, 0).
		*/
		Vector2()
		{
			set(0, 0);
		}

		/*!
			Initializes the vector to (x, y).
			\param x x-component of vector.
			\param y y-component of vector.
		*/
		Vector2(T x, T y)
		{
			set(x, y);
		}

		/*!
			Returns the length of the vector (sqrt(x^2 + y^2)).
			\return The length of the vector.
		*/
		const double length() const
		{
			return sqrt((x * x) + (y * y));
		}

		/*!
			Sets the length of the vector to 1 - meaning it becomes a unit vector.
			\note Use this carefully when using vectors of integer type because rounding errors are as good as guaranteed.
			\return A copy of the normalized vector.
		*/
		Vector2<T> normalize()
		{
			double vlength = length();

			if (vlength <= 0)
			{
				x = y = 0;

			}
			else
			{
				x /= vlength;
				y /= vlength;
			}

			return *this;
		}

		Vector2<T> normalized() const
		{
			double vlength = length();

			return Vector2<T>(x / vlength, y / vlength);
		}



		/*!
			Scale the vector by a factor of f.
			\param f The factor to scale the vector by.
			\return A copy of the scaled vector.
		*/
		Vector2<T> scale(double f)
		{
			x *= f;
			y *= f;

			return *this;
		}

		/*!
			Perform vector addition, and store the result in this.
			\param v Vector to add to this.
			\return A copy of the results.
		*/
		Vector2<T> add(const Vector2<T>& v)
		{
			x += v.x;
			y += v.y;

			return *this;
		}

		/*!
			Perform vector subtraction, and store the result in this.
			\param v Vector to subtract from this.
			\return A copy of the results.
		*/
		Vector2<T> subtract(const Vector2<T>& v)
		{
			x -= v.x;
			y -= v.y;

			return *this;
		}

		/*!
			Get the dot product of this vector and v.
			\param v The second vector.
			\return A copy of the results.
		*/
		T dot(const Vector2<T>& v) const
		{
			return ((x * v.x) + (y * v.y));
		}

		/*!
			Return the x-component of this vector.
			\return The x-component of this.
		*/
		T getX() const
		{
			return x;
		}

		/*!
			Return the y-component of this vector.
			\return The y-component of this.
		*/
		T getY() const
		{
			return y;
		}

		/*!
			Set this vector to the specified value.
			\param x The new x-component.
			\param y The new y-component.
		*/
		void set(T x_, T y_)
		{
			this->x = x_;
			this->y = y_;
		}

		void set(const Vector2<T> &v)
		{
			this->x = v.x;
			this->y = v.y;
		}

		/*!
			Set the x-component of this vector individually.
			\param x The new x-component.
		*/
		void setX(T x_)
		{
			this->x = x_;
		}

		/*!
			Set the y-component of this vector individually.
			\param y The new y-component.
		*/
		void setY(T y_)
		{
			this->y = y_;
		}

		//operator overloads
		/*!
			Performs vector addition on two vectors and returns the results.
			\param v The second vector.
			\note This does not change this vector.
			\return The resulting vector.
		*/
		Vector2<T> operator +(const Vector2<T>& v) const
		{
			Vector2<T> v2;
			v2 = *this;
			v2.add(v);

			return v2;
		}

		/*!
			Performs vector subtraction on two vectors and returns the results.
			\param v The second vector.
			\note This does not change this vector.
			\return The resulting vector.
		*/
		Vector2<T> operator -(const Vector2<T>& v) const
		{
			Vector2<T> v2;
			v2 = *this;
			v2.subtract(v);

			return v2;
		}

		/*!
			Performs vector inversion / unary - and returns the results.
			\note This does not change this vector.
			\return The resulting vector.
		*/
		Vector2<T> operator -() const
		{
			Vector2<T> v2(-1 * this->getX(), -1 * this->getY());

			return v2;
		}

		/*!
			Returns a scaled vector, scaled by f.
			\param f The scale factor.
			\note This does not change this vector.
			\return The resulting vector.
		*/
		Vector2<T> operator *(double f) const
		{
			Vector2<T> v;
			v = *this;
			v.scale(f);

			return v;
		}

		/*!
			Returns a scaled vector, scaled by 1/f.
			\param f The scale factor.
			\note This does not change this vector.
			\return The resulting vector.
		*/
		Vector2<T> operator /(double f) const
		{
			Vector2<T> v;
			v = *this;
			v.scale(1/f);

			return v;
		}

		/*!
			Adds v to this vector.
			\param v The second vector.
			\return The resulting vector.
		*/
		Vector2<T> operator +=(const Vector2<T>& v)
		{
			add(v);

			return *this;
		}

		/*!
			Subtracts v from this vector.
			\param v The second vector.
			\return The resulting vector.
		*/
		Vector2<T> operator -=(const Vector2<T>& v)
		{
			subtract(v);

			return *this;
		}

		/*!
			Scales this vector by f.
			\param f The scale factor.
			\return The resulting vector.
		*/
		Vector2<T> operator *=(const double f)
		{
			scale(f);

			return *this;
		}

		/*!
			Scales this vector by 1/f.
			\param f The scale factor.
			\return The resulting vector.
		*/
		Vector2<T> operator /=(const double f)
		{
			scale(1/f);

			return *this;
		}

		/*!
			Sets this vector's components equal to v.
			\param v The second vector.
			\return The resulting vector.
		*/
		Vector2<T> operator =(const Vector2<T>& vec)
		{
			set(vec);

			return *this;
		}

		/*!
			Checks if this vector is equal to the second vector.
			\param v The second vector.
			\return The resulting vector.
		*/
		bool operator ==(const Vector2<T>& vec)
		{
			if (x == vec.x && y == vec.y)
			{
				return true;
			}

			return false;
		}

		/*!
			A shortcut function to check wether this vector is inside a rectangle - if we consider it a position vector (origin in (0,0), end-point at (x,y)).
			\param left Left side of the rectangle.
			\param top Top side of the rectangle.
			\param right Right side of the rectangle.
			\param bottom Bottom side of the rectangle.
			\return True if the position vector is considered inside the specified rectangle. False otherwise.
		*/
		bool isInRect(double left, double top, double right, double bottom)
		{
			if ((x < left) || (x > right) || (y < bottom) || (y > top))
				return false;

			return true;
		}
	};
	typedef Vector2<double> Vec2d;

	inline std::ostream & operator <<(std::ostream &stream, const Vector2<double> &v)
	{
		stream << "(" << v.getX() << ", " << v.getY() << ")";
		return stream;
	}
}
