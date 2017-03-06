#include "TCoord.h"


/** Implementierung der Klasse TCoord<S> **/

/* Elementzugriff */
template <typename S>
S&		TCoord<S>::operator[](std::size_t i)
{
    return m_x[i];
}

/* konstanter Elementzugriff */
template <typename S>
const S&	TCoord<S>::operator[](std::size_t i) const
{
    return m_x[i];
}

/* unäres Minus */
template <typename S>
TCoord<S>	TCoord<S>::operator-() const
{
    return {-m_x[0], -m_x[1], -m_x[2]};
}

/* Betrag des Vektors */
template <typename S>
comma		TCoord<S>::abs() const
{
    return std::sqrt(operator*(*this));
}


/* Addition eines Gleitkomma-Vektors */
template <typename S>
TCoord<comma>	TCoord<S>::operator+(TCoord<comma> const &b) const
{
    return {m_x[0] + b[0], m_x[1] + b[1], m_x[2] + b[2]};
}

/* Addition eines Ganzzahl-Vektors */
template <typename S>
TCoord<S>	TCoord<S>::operator+(TCoord<int> const &b) const
{
    return {m_x[0] + b[0], m_x[1] + b[1], m_x[2] + b[2]};
}

/* Subtraktion eines Gleitkomma-Vektors */
template <typename S>
TCoord<comma>	TCoord<S>::operator-(TCoord<comma> const &b) const
{
    return {m_x[0] - b[0], m_x[1] - b[1], m_x[2] - b[2]};
}

/* Subtraktion eines Ganzzahl-Vektors */
template <typename S>
TCoord<S>	TCoord<S>::operator-(TCoord<int> const &b) const
{
    return {m_x[0] - b[0], m_x[1] - b[1], m_x[2] - b[2]};
}

/* Skalarprodukt mit einem Gleitkomma-Vektor */
template <typename S>
comma		TCoord<S>::operator*(TCoord<comma> const &b) const
{
    return (m_x[0] * b[0] + m_x[1] * b[1] + m_x[2] * b[2]);
}

/* Skalarprodukt mit einem Ganzzahl-Vektor */
template <typename S>
S		TCoord<S>::operator*(TCoord<int> const &b) const
{
    return (m_x[0] * b[0] + m_x[1] * b[1] + m_x[2] * b[2]);
}


/* Umwandlung in anderen Vektor-Typ */
template <typename S>
template <typename T>
/*TCoord<T>*/	TCoord<S>::operator TCoord<T>() const
{
    return {(T)m_x[0], (T)m_x[1], (T)m_x[2]};
}

/* Zuweisungsaddition eines Vektors */
template <typename S>
template <typename T>
TCoord<S>&	TCoord<S>::operator+=(TCoord<T> const &b)
{
    for (int i = 0; i < 3; i++)
        m_x[i] += b[i];
    return *this;
}

/* Zuweisungssubtraktion eines Vektors */
template <typename S>
template <typename T>
TCoord<S>&	TCoord<S>::operator-=(TCoord<T> const &b)
{
    for (int i = 0; i < 3; i++)
        m_x[i] -= b[i];
    return *this;
}

/* Gleichheit: Gleichheit aller Komponenten */
template <typename S>
template <typename T>
bool		TCoord<S>::operator==(TCoord<T> const &b) const
{
    return ((m_x[0] == b[0]) && (m_x[1] == b[1]) && (m_x[2] == b[2]));
}

/* Ungleichheit: keine Gleichheit */
template <typename S>
template <typename T>
bool		TCoord<S>::operator!=(TCoord<T> const &b) const
{
    return !(*this == b);
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool		TCoord<S>::operator<=(TCoord<T> const &b) const
{
    return ((m_x[0] <= b[0]) && (m_x[1] <= b[1]) && (m_x[2] <= b[2]));
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool		TCoord<S>::operator<(TCoord<T> const &b) const
{
    return ((m_x[0] < b[0]) && (m_x[1] < b[1]) && (m_x[2] < b[2]));
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool		TCoord<S>::operator>=(TCoord<T> const &b) const
{
    return ((m_x[0] >= b[0]) && (m_x[1] >= b[1]) && (m_x[2] >= b[2]));
}

/* Vergleich für alle (!) Komponenten */
template <typename S>
template <typename T>
bool		TCoord<S>::operator>(TCoord<T> const &b) const
{
    return ((m_x[0] > b[0]) && (m_x[1] > b[1]) && (m_x[2] > b[2]));
}

/** Ende der TCoord<S>-Implementierung **/


/* Multiplikation (Skalar * Vektor) für eine Gleitkommazahl) */
template <typename T>
TCoord<comma>	operator*(comma a, TCoord<T> const &b)
{
    return {a * b[0], a * b[1], a * b[2]};
}


/* Multiplikation (Skalar * Vektor) für eine Ganzzahl */
template <typename T>
TCoord<T>	operator*(int a, TCoord<T> const &b)
{
    return {a * b[0], a * b[1], a * b[2]};
}
