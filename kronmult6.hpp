#ifndef KRONMULT6_HPP
#define  KRONMULT6_HPP 1

#include "kroncommon.hpp"

#include "kgemm_nt.hpp"
#include "kronmult5.hpp"

//  -------------------------------------------
//  device function to evaluate
//  Y = kron(A1,...,A6)*X as
//  W(:,k) = X(:,k) * transpose(A1), k=1:nvec
//  Y = kron(A2,..,A6) * W
//  -------------------------------------------
template<typename T>
DEVICE_FUNCTION
void kronmult6( int const n, 
                int const nvec,
                T   const A1_[],
                T   const A2_[],
                T   const A3_[],
                T   const A4_[],
                T   const A5_[],
                T   const A6_[],
                T   X_[],
                T   Y_[],
                T   W_[] )
// -----------------
// note A1 is n by n
//      X is (n^6 by nvec)
// -----------------
{

    int const n2 = n*n;
    int const n4 = n2*n2;
    int const n5 = n*n4;
    int const n6 = n2*n4;

    int const ldX = n6;
    int const ldW = n6;


#define X(i,j)  X_[ indx2f(i,j,ldX) ]
#define W(i,j)  W_[ indx2f(i,j,ldW) ]

    for(int i=1; i <= nvec; i++) {
            T *Xi_ = &( X(1, i) );
            T *Wi_ = &( W(1, i) );
            int const ldXi = n5;
            int const ldWi = n5;
            // ----------------------------
            // Xi viewed as (n^5 by n) array
            // Wi viewed as (n^5 by n) array
            // ----------------------------
#define Xi(i,j)  Xi_[ indx2f(i,j,ldXi) ]
#define Wi(i,j)  Wi_[ indx2f(i,j,ldWi) ]
            // --------------------------------------------------------
            // Wi(1:n^5, 1:n) = Xi(1:n^5, 1:n) * transpose(A1(1:n,1:n))
            // --------------------------------------------------------
            int const mm = n5;
            int const nn = n;
            int const kk = n;
            T const alpha = 1;
            T const beta = 0;

            T const * const  Ap = &(Xi(1,1));
            T const * const  Bp = A1_;
            T       * const  Cp = &(Wi(1,1));

            int const ld1 = ldXi;
            int const ld2 = n;
            int const ld3 = ldWi;

            kgemm_nt( mm,nn,kk, 
                      alpha, Ap, ld1,
                             Bp, ld2,
                      beta,  Cp, ld3 );
    };

    int const next_nvec = nvec * n;

    // --------------------------------
    // note now X_ is used as workspace
    // --------------------------------
    {
    kronmult5( n, next_nvec, 
               A2_, A3_, A4_, A5_, A6_,
               W_,  Y_,   X_ );
    }

}



#undef X
#undef W
#undef Xi
#undef Wi



#endif