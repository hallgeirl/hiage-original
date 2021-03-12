#pragma once

#include <iostream>

namespace hiage
{
	template<typename T>
	struct BoundingBox
	{
		T left, right, top, bottom;
		BoundingBox(T left, T bottom, T right, T top)
		{
			this->left = left;
			this->right = right;
			this->top = top;
			this->bottom = bottom;
		}
		BoundingBox()
		{
			left = right = top = bottom = 0;
		}

		// From int->double/float/whatever is fine. Make this an explicitly allowed conversion.
		BoundingBox(const BoundingBox<int>& other)
		{
			left = other.left;
			right = other.right;
			top = other.top;
			bottom = other.bottom;
		}

        inline bool intersects(const BoundingBox<T>& other) const
        {
            return !(left > other.right || right < other.left || bottom > other.top || top < other.bottom);
        }

		inline T width() const
		{
			return right - left;
		}

		inline T height() const
		{
			return top - bottom;
		}


	};

	inline std::ostream & operator <<(std::ostream &stream, const BoundingBox<double> &r)
	{
		stream << "Left: " << r.left << " Top: " << r.top << " Right: " << r.right << " Bottom: " << r.bottom;
		return stream;
	}
}