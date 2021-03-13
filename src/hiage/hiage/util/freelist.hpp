#pragma once

#include <vector>

namespace hiage
{
    // Basically a vector, but allows reuse of elements instead of popping etc.
    template<typename T>
    class FreeList
    {
    private:
        std::vector<T> _data;
        size_t _size = 0;
    public:
        size_t insert(const T& element)
        {
            if (_size == _data.size())
                _data.push_back(element);
            else
                _data[_size] = element;

            _size++;

            return _size - 1;
        }
        
        size_t insert()
        {
            if (_size == _data.size())
                _data.push_back(T());

            _size++;

            return _size - 1;
        }

        void clear()
        {
            _size = 0;
        }

        size_t size()
        {
            return _size;
        }

        T& operator[](size_t i)
        {
            return _data[i];
        }

        T pop_back()
        {
            return _data[--_size];
        }
    };
}