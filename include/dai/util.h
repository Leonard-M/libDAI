/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2006-2009  Joris Mooij  [joris dot mooij at libdai dot org]
 *  Copyright (C) 2006-2007  Radboud University Nijmegen, The Netherlands
 */


/// \file
/// \brief Defines general utility functions and adds an abstraction layer for platform-dependent functionality
/// \todo Improve documentation


#ifndef __defined_libdai_util_h
#define __defined_libdai_util_h


#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>
#include <algorithm>


#if defined(WINDOWS)
    #include <map>
#elif defined(CYGWIN)
    #include <boost/tr1/unordered_map.hpp>
#else
    #include <tr1/unordered_map>
#endif


/// An alias to the BOOST_FOREACH macro from the boost::foreach library
#define foreach BOOST_FOREACH

#ifdef DAI_DEBUG
/// \brief "Print variable". Prints the text of an expression, followed by its value (only if DAI_DEBUG is defined)
/**
 *  Useful debugging macro to see what your code is doing.
 *  Example: \code DAI_PV(3+4) \endcode
 *  Output: \code 3+4= 7 \endcode
 */
#define DAI_PV(x) do {std::cerr << #x "= " << (x) << std::endl;} while(0)
/// "Debugging message": Prints a message (only if DAI_DEBUG is defined)
#define DAI_DMSG(str) do {std::cerr << str << std::endl;} while(0)
#else
#define DAI_PV(x) do {} while(0)
#define DAI_DMSG(str) do {} while(0)
#endif

/// Produces accessor and mutator methods according to the common pattern.
/** Example:
 *  \code DAI_ACCMUT(size_t& maxIter(), { return props.maxiter; }); \endcode
 *  \todo At the moment, only the mutator appears in doxygen documentation.
 */
#define DAI_ACCMUT(x,y)                     \
      x y;                                  \
      const x const y;

/// Macro to give error message \a stmt if props.verbose>=\a n
#define DAI_IFVERB(n, stmt) if(props.verbose>=n) { cerr << stmt; }


#ifdef WINDOWS
    /// Returns true if argument is NAN (Not A Number)
    bool isnan( double x );

    /// Returns inverse hyperbolic tangent of argument
    double atanh( double x );

    /// Returns log(1+x)
    double log1p( double x );

    /// Define INFINITY
    #define INFINITY (std::numeric_limits<Real>::infinity())
#endif


namespace dai {


/// Real number (alias for double, which could be changed to long double if necessary)
typedef double Real;

/// Returns logarithm of \a x
inline Real log( Real x ) {
    return std::log(x);
}

/// Returns exponent of \a x
inline Real exp( Real x ) {
    return std::exp(x);
}


#ifdef WINDOWS
    /// hash_map is an alias for std::map.
    /** Since there is no TR1 unordered_map implementation available yet, we fall back on std::map.
     */
    template <typename T, typename U, typename H = boost::hash<T> >
        class hash_map : public std::map<T,U> {};
#else
    /// hash_map is an alias for std::tr1::unordered_map.
    /** We use the (experimental) TR1 unordered_map implementation included in modern GCC distributions.
     */
    template <typename T, typename U, typename H = boost::hash<T> >
        class hash_map : public std::tr1::unordered_map<T,U,H> {};
#endif


/// Returns wall clock time in seconds
double toc();


/// Returns absolute value of \a t
template<class T>
inline T abs( const T &t ) {
    return (t < 0) ? (-t) : t;
}


/// Sets the random seed
void rnd_seed( size_t seed );

/// Returns a real number, distributed uniformly on [0,1)
Real rnd_uniform();

/// Returns a real number from a standard-normal distribution
Real rnd_stdnormal();

/// Returns a random integer in interval [min, max]
int rnd_int( int min, int max );

/// Returns a random integer in the half-open interval \f$[0,n)\f$
inline int rnd( int n) {
    return rnd_int( 0, n-1 );
}


/// Writes a std::vector to a std::ostream
template<class T>
std::ostream& operator << (std::ostream& os, const std::vector<T> & x) {
    os << "(";
    for( typename std::vector<T>::const_iterator it = x.begin(); it != x.end(); it++ )
        os << (it != x.begin() ? ", " : "") << *it;
    os << ")";
    return os;
}

/// Writes a std::set to a std::ostream
template<class T>
std::ostream& operator << (std::ostream& os, const std::set<T> & x) {
    os << "{";
    for( typename std::set<T>::const_iterator it = x.begin(); it != x.end(); it++ )
        os << (it != x.begin() ? ", " : "") << *it;
    os << "}";
    return os;
}

/// Writes a std::map to a std::ostream
template<class T1, class T2>
std::ostream& operator << (std::ostream& os, const std::map<T1,T2> & x) {
    os << "{";
    for( typename std::map<T1,T2>::const_iterator it = x.begin(); it != x.end(); it++ )
        os << (it != x.begin() ? ", " : "") << it->first << "->" << it->second;
    os << "}";
    return os;
}

/// Writes a std::pair to a std::ostream
template<class T1, class T2>
std::ostream& operator << (std::ostream& os, const std::pair<T1,T2> & x) {
    os << "(" << x.first << ", " << x.second << ")";
    return os;
}

/// Concatenate two vectors
template<class T>
std::vector<T> concat( const std::vector<T>& u, const std::vector<T>& v ) {
    std::vector<T> w;
    w.reserve( u.size() + v.size() );
    for( size_t i = 0; i < u.size(); i++ )
        w.push_back( u[i] );
    for( size_t i = 0; i < v.size(); i++ )
        w.push_back( v[i] );
    return w;
}

/// Split a string into tokens
void tokenizeString( const std::string& s, std::vector<std::string>& outTokens, const std::string& delim="\t\n" );

/// Used to keep track of the progress made by iterative algorithms
class Diffs : public std::vector<Real> {
    private:
        size_t _maxsize;
        Real _def;
        std::vector<Real>::iterator _pos;
        std::vector<Real>::iterator _maxpos;
    public:
        /// Constructor
        Diffs(long maxsize, Real def) : std::vector<Real>(), _maxsize(maxsize), _def(def) {
            this->reserve(_maxsize);
            _pos = begin();
            _maxpos = begin();
        }
        /// Returns maximum difference encountered
        Real maxDiff() {
            if( size() < _maxsize )
                return _def;
            else
                return( *_maxpos );
        }
        /// Register new difference x
        void push(Real x) {
            if( size() < _maxsize ) {
                push_back(x);
                _pos = end();
                if( size() > 1 ) {
                    if( *_maxpos < back() ) {
                        _maxpos = end();
                        _maxpos--;
                    }
                } else {
                    _maxpos = begin();
                }
            } else {
                if( _pos == end() )
                    _pos = begin();
                if( _maxpos == _pos ) {
                    *_pos++ = x;
                    _maxpos = max_element(begin(),end());
                } else {
                    if( x > *_maxpos )
                        _maxpos = _pos;
                    *_pos++ = x;
                }
            }
        }
        /// Return maximum number of differences stored
        size_t maxSize() { return _maxsize; }
};


} // end of namespace dai


#endif
