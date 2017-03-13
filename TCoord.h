<<<<<<< HEAD
#ifndef TCOORD_H
#define TCOORD_H

#include <cstddef>
#include <cmath>
#include <iostream>


typedef double commaType;    //auf diese Weise kann man einfacher auf double umstellen

/** Klasse für Vektoren
  * vorgesehen für Ganzzahlen <int>
  * oder für Gleitkommazahlen <commaType> */
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



#endif // TCOORD_H
=======
#ifndef TCOORD_H
#define TCOORD_H

#include <cstddef>
#include <cmath>
#include <iostream>


typedef double commaType;    //auf diese Weise kann man einfacher auf double umstellen

/** Klasse für Vektoren
  * vorgesehen für Ganzzahlen <int>
  * oder für Gleitkommazahlen <commaType> */
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


#include "TCoord.cpp"

#endif // TCOORD_H
>>>>>>> 9cfbd860e2aef52c692b90c6e8335ae31226232f
