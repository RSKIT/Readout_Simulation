/*
    ROME (ReadOut Modelling Environment)
    Copyright © 2014  Stefan Backens,
                      Rudolf Schimassek (rudolf.schimassek@kit.edu),
                      Karlsruhe Institute of Technology (KIT)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as 
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of the ROME simulation framework.
    It originates from a drift chamber simulation written by a group of
    students in the Lecture "Collaborative Software Design for Physicists" 
    in Winter Semester 14/15.
    This class was implemented by Stefan Backens.
*/

#ifndef TCOORD_H
#define TCOORD_H

#include <cstddef>
#include <cmath>
#include <iostream>


typedef double commaType;

/** Class for geometric vectors
  * foreseen for integers <int>
  * and floating point numbers <commaType> */
template <typename S>
class TCoord
{
    public:
        S m_x[3];

        S&          operator[](std::size_t i);          /* Elementzugriff */
        const S&    operator[](std::size_t i) const;    /* konstanter Elementzugriff */
        TCoord<S>   operator-() const;                  /* unäres Minus */
        commaType   abs() const;                        /* Betrag des Vektors */
        bool        isZero() const;                     /* Prüfung auf Nullvektor */

        /* Addition eines Gleitkomma-Vektors */
        TCoord<commaType>   operator+(TCoord<commaType> const &b) const;
        /* Addition eines Ganzzahl-Vektors */
        TCoord<S>           operator+(TCoord<int> const &b) const;
        /* Subtraktion eines Gleitkomma-Vektors */
        TCoord<commaType>   operator-(TCoord<commaType> const &b) const;
        /* Subtraktion eines Ganzzahl-Vektors */
        TCoord<S>           operator-(TCoord<int> const &b) const;
        /* Skalarprodukt mit einem Gleitkomma-Vektor */
        commaType           operator*(TCoord<commaType> const &b) const;
        /* Skalarprodukt mit einem Ganzzahl-Vektor */
        S                   operator*(TCoord<int> const &b) const;
        /* Kreuzprodukt mit einem Ganzzahl-Vektor */
        TCoord<S>           cross(TCoord<int> const &b) const;
        /* Kreuzprodukt mit einem Gleitkomma-Vektor */
        TCoord<commaType>   cross(TCoord<commaType> const &b) const;

        /* Umwandlung in anderen Vektor-Typ */
        template <typename T>
        /*TCoord<T>*/ operator TCoord<T>() const;

        /* Zuweisungsaddition eines Vektors */
        template <typename T>
        TCoord<S>& operator+=(TCoord<T> const &b);

        /* Zuweisungssubtraktion eines Vektors */
        template <typename T>
        TCoord<S> &operator-=(TCoord<T> const &b);

        /* Gleichheit: Gleichheit aller Komponenten */
        template <typename T>
        bool operator==(TCoord<T> const &b) const;

        /* Ungleichheit: keine Gleichheit */
        template <typename T>
        bool operator!=(TCoord<T> const &b) const;

        /* Vergleich für alle (!) Komponenten untereinander (größer/kleiner[gleich])*/
        template <typename T>
        bool operator<=(TCoord<T> const &b) const;
        template <typename T>
        bool operator<(TCoord<T> const &b) const;
        template <typename T>
        bool operator>=(TCoord<T> const &b) const;
        template <typename T>
        bool operator>(TCoord<T> const &b) const;

        /* Nullvektor */
        static const TCoord<S> Null;
        
        /* Prüfung ob ein Punkt in einem Volumen liegt */
        template <typename T, typename U>
        bool inside(TCoord<T> const &start, TCoord<U> const &end);

        /* berechnet das Volumen eines Quaders mit diesem Vektor als Raumdiagonale */
        S volume();
};

/* Multiplikation (Skalar * Vektor) für eine Gleitkommazahl) */
template <typename T>
TCoord<commaType> operator*(commaType a, TCoord<T> const &b);

/* Multiplikation (Skalar * Vektor) für eine Ganzzahl */
template <typename T>
TCoord<T> operator*(int a, TCoord<T> const &b);

/* Ausgabe eines Vektors direkt in einen Ausgabestream (spart viel Tipparbeit) */
template <typename T>
std::ostream& operator<<(std::ostream &out, TCoord<T> const &b);

template <typename S, typename T, typename U, typename V>
TCoord<commaType> OverlapVolume(TCoord<S> startone, TCoord<T> endone, 
                                TCoord<U> starttwo, TCoord<V> endtwo);

#include "TCoord.cpp"
#endif // TCOORD_H
