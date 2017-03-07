#ifndef TCOORD_H
#define TCOORD_H

#include <cstddef>
#include <cmath>


typedef double comma;    //auf diese Weise kann man einfacher auf double umstellen

/** Klasse für Vektoren
  * vorgesehen für Ganzzahlen <int>
  * oder für Gleitkommazahlen <comma> */
template <typename S>
class TCoord
{
    public:
        S m_x[3];

        S&		operator[](std::size_t i);
        const S&	operator[](std::size_t i) const;
        TCoord<S>	operator-() const;
        comma		abs() const;

        TCoord<comma>	operator+(TCoord<comma> const &b) const;
        TCoord<S>	operator+(TCoord<int> const &b) const;
        TCoord<comma>	operator-(TCoord<comma> const &b) const;
        TCoord<S>	operator-(TCoord<int> const &b) const;
        comma		operator*(TCoord<comma> const &b) const;
        S		operator*(TCoord<int> const &b) const;

        template <typename T>
        /*TCoord<T>*/	operator TCoord<T>() const;
        template <typename T>
        TCoord<S>&	operator+=(TCoord<T> const &b);
        template <typename T>
        TCoord<S>	&operator-=(TCoord<T> const &b);
        template <typename T>
        bool		operator==(TCoord<T> const &b) const;
        template <typename T>
        bool		operator!=(TCoord<T> const &b) const;
        template <typename T>
        bool 		operator<=(TCoord<T> const &b) const;
        template <typename T>
        bool 		operator<(TCoord<T> const &b) const;
        template <typename T>
        bool		operator>=(TCoord<T> const &b) const;
        template <typename T>
        bool		operator>(TCoord<T> const &b) const;
};


template <typename T>
TCoord<comma>		operator*(comma a, TCoord<T> const &b);


template <typename T>
TCoord<T>		operator*(int a, TCoord<T> const &b);


#endif // TCOORD_H
