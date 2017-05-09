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

#include "TCoord.h"

/** Implementierung der Klasse TCoord<S> **/

/* Elementzugriff */
template <typename S>
S& TCoord<S>::operator[](std::size_t i)
{
    return m_x[i];
}

/* konstanter Elementzugriff */
template <typename S>
const S& TCoord<S>::operator[](std::size_t i) const
{
    return m_x[i];
}

/* unäres Minus */
template <typename S>
TCoord<S> TCoord<S>::operator-() const
{
    return {-m_x[0], -m_x[1], -m_x[2]};
}

/* Betrag des Vektors */
template <typename S>
commaType TCoord<S>::abs() const
{
    return std::sqrt(operator*(*this));
}

/* Prüfung auf Nullvektor */
template <typename S>
bool TCoord<S>::isZero() const
{
    return (*this == TCoord<S>::Null);
}


/* Addition eines Gleitkomma-Vektors */
template <typename S>
TCoord<commaType> TCoord<S>::operator+(TCoord<commaType> const &b) const
{
    return {m_x[0] + b[0], m_x[1] + b[1], m_x[2] + b[2]};
}

/* Addition eines Ganzzahl-Vektors */
template <typename S>
TCoord<S> TCoord<S>::operator+(TCoord<int> const &b) const
{
    return {m_x[0] + b[0], m_x[1] + b[1], m_x[2] + b[2]};
}

/* Subtraktion eines Gleitkomma-Vektors */
template <typename S>
TCoord<commaType> TCoord<S>::operator-(TCoord<commaType> const &b) const
{
    return {m_x[0] - b[0], m_x[1] - b[1], m_x[2] - b[2]};
}

/* Subtraktion eines Ganzzahl-Vektors */
template <typename S>
TCoord<S> TCoord<S>::operator-(TCoord<int> const &b) const
{
    return {m_x[0] - b[0], m_x[1] - b[1], m_x[2] - b[2]};
}

/* Skalarprodukt mit einem Gleitkomma-Vektor */
template <typename S>
commaType TCoord<S>::operator*(TCoord<commaType> const &b) const
{
    return (m_x[0] * b[0] + m_x[1] * b[1] + m_x[2] * b[2]);
}

/* Skalarprodukt mit einem Ganzzahl-Vektor */
template <typename S>
S TCoord<S>::operator*(TCoord<int> const &b) const
{
    return (m_x[0] * b[0] + m_x[1] * b[1] + m_x[2] * b[2]);
}

/* Kreuzprodukt mit einem Ganzzahl-Vektor */
template <typename S>
TCoord<S> TCoord<S>::cross(TCoord<int> const &b) const
{
return { m_x[1] * b[2] - m_x[2] * b[1], m_x[2] * b[0] - m_x[0] * b[2], m_x[0] * b[1] - m_x[1] * b[0]};
}

/* Kreuzprodukt mit einem Gleitkomma-Vektor */
template <typename S>
TCoord<commaType> TCoord<S>::cross(TCoord<commaType> const &b) const
{
return { m_x[1] * b[2] - m_x[2] * b[1], m_x[2] * b[0] - m_x[0] * b[2], m_x[0] * b[1] - m_x[1] * b[0]};
}

/* Umwandlung in anderen Vektor-Typ */
template <typename S>
template <typename T>
/*TCoord<T>*/ TCoord<S>::operator TCoord<T>() const
{
    return {(T)m_x[0], (T)m_x[1], (T)m_x[2]};
}

/* Zuweisungsaddition eines Vektors */
template <typename S>
template <typename T>
TCoord<S>& TCoord<S>::operator+=(TCoord<T> const &b)
{
    for (int i = 0; i < 3; i++)
        m_x[i] += b[i];
    return *this;
}

/* Zuweisungssubtraktion eines Vektors */
template <typename S>
template <typename T>
TCoord<S>& TCoord<S>::operator-=(TCoord<T> const &b)
{
    for (int i = 0; i < 3; i++)
        m_x[i] -= b[i];
    return *this;
}

/* Gleichheit: Gleichheit aller Komponenten */
template <typename S>
template <typename T>
bool TCoord<S>::operator==(TCoord<T> const &b) const
{
    return ((m_x[0] == b[0]) && (m_x[1] == b[1]) && (m_x[2] == b[2]));
}

/* Ungleichheit: keine Gleichheit */
template <typename S>
template <typename T>
bool TCoord<S>::operator!=(TCoord<T> const &b) const
{
    return !(*this == b);
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool TCoord<S>::operator<=(TCoord<T> const &b) const
{
    return ((m_x[0] <= b[0]) && (m_x[1] <= b[1]) && (m_x[2] <= b[2]));
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool TCoord<S>::operator<(TCoord<T> const &b) const
{
    return ((m_x[0] < b[0]) && (m_x[1] < b[1]) && (m_x[2] < b[2]));
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool TCoord<S>::operator>=(TCoord<T> const &b) const
{
    return ((m_x[0] >= b[0]) && (m_x[1] >= b[1]) && (m_x[2] >= b[2]));
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool TCoord<S>::operator>(TCoord<T> const &b) const
{
    return ((m_x[0] > b[0]) && (m_x[1] > b[1]) && (m_x[2] > b[2]));
}

/* Nullvektor */
template <typename S>
const TCoord<S> TCoord<S>::Null = TCoord<S>{0, 0, 0};

/** Ende der TCoord<S>-Implementierung **/


/* Multiplikation (Skalar * Vektor) für eine Gleitkommazahl) */
template <typename T>
TCoord<commaType> operator*(commaType a, TCoord<T> const &b)
{
    return {a * b[0], a * b[1], a * b[2]};
}


/* Multiplikation (Skalar * Vektor) für eine Ganzzahl */
template <typename T>
TCoord<T> operator*(int a, TCoord<T> const &b)
{
    return {a * b[0], a * b[1], a * b[2]};
}


/* Ausgabe eines Vektors direkt in einen Ausgabestream (spart viel Tipparbeit) */
template<typename T>
std::ostream& operator<<(std::ostream &out, TCoord<T> const &b)
{
    out << "{" << b[0] << ", " << b[1] << ", " << b[2] << "}";
    return out;
}