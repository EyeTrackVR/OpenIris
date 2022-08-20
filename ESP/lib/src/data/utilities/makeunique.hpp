#pragma once
#ifndef MAKE_UNIQUE_HPP
#define MAKE_UNIQUE_HPP
#include <memory>
#include <cstddef>
#include <type_traits>
#include <utility>

/**
 * @brief override the STD namespace to add make_unique function
 *
 * @tparam T
 * @tparam Args
 * @return std::unique_ptr<T>
 */
namespace std
{
    template <class T>
    struct _Unique_if
    {
        typedef unique_ptr<T> _Single_object;
    };

    template <class T>
    struct _Unique_if<T[]>
    {
        typedef unique_ptr<T[]> _Unknown_bound;
    };

    template <class T, size_t N>
    struct _Unique_if<T[N]>
    {
        typedef void _Known_bound;
    };

    template <class T, class... Args>
    typename _Unique_if<T>::_Single_object
    make_unique(Args &&...args)
    {
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <class T>
    typename _Unique_if<T>::_Unknown_bound
    make_unique(size_t n)
    {
        typedef typename remove_extent<T>::type U;
        return unique_ptr<T>(new U[n]());
    }

    template <class T, class... Args>
    typename _Unique_if<T>::_Known_bound
    make_unique(Args &&...) = delete;
}

#endif // !MAKE_UNIQUE_HPP