// ----------------------------------------------------------------------------
// glrand.h : a random number generator specifically for what the openGL program
//				might want to refer as the point primitive; 
//
//			  since different procedure needed, here provided are disparate
//			  functions for generating integer and floating numbers
//				
//			  mostly, the routines in this module are agglomerated into a vector
//			  before they return to the caller
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#if !defined(_GLRAND_H_)
#define _GLRAND_H_

#include <stdint.h>
#include <time.h>
#include <string.h>

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <sstream>
#include <string>

#include <QtOpenGL/QtOpenGL>

#ifdef CORONA
#include <corona.h>
#endif

#define ARRAY_SIZE(a)		( NULL==a?0 : (int)(sizeof a / sizeof a[0]) )
#define ABS(a)				( a >= 1e-6 ? (a) : -(a) )

using std::cerr;
using std::ostringstream;
using std::cout;

#ifndef SETCURSOR
#ifdef USE_GLUTCURSOR
#define SETCURSOR(a) glutSetCursor(a)
#else
#define SETCURSOR(a) ;
#endif
#endif

#define LOCAL_MAXDOUBLE	0x00ffffff

namespace glrand {
	/*
	 * @brief randomly spawn a single integer, as GL points, say.
	 * @parm floor giving the lower boundary of the generating range
	 * @parm ceiling giving the upper boundary of the generating range
	 * @return a single integer in the designated range
	 * @see genIntvector 
	 */
	template <typename _T> // _T can be all variations of integer types
	_T genInt (const _T floor, const _T ceiling)
	{
		if (ceiling < floor ||
			ceiling > RAND_MAX ) {
			cerr << "range not supported.\n";
			return;
		}

		static bool bseeded = false;

		if ( !bseeded ) {
			srand( (unsigned int)time(NULL) );
			bseeded = true;
		}

		if ( floor == ceiling ) { 
			return floor;
		}

		return random () % ( ceiling - floor + 1 ) + floor;
	}

	/*
	 * @brief randomly spawn a sequence of integers, as GL points, say.
	 * @param _vector a pointer to a sequential and subscript-indexable
	 *        container used for receiving the data produced, 
	 *        note it should have a larger size than $n
	 * @param n an integer giving the number of points to produce
	 * @parm floor giving the lower boundary of the generating range
	 * @parm ceiling giving the upper boundary of the generating range
	 * @return none
	 * @see genInt
	 */
	template <typename _T> // _T can be all variations of integer types
	void genIntvector (_T *_vector, GLsizei n, const _T floor, const _T ceiling) 
	{
		if (ceiling < floor ||
			ceiling > RAND_MAX ) {
			cerr << "range not supported.\n";
			return;
		}

		GLsizei idx;
		_T curv;
		static bool bseeded = false;

		if ( !bseeded ) {
			srand( (unsigned int)time(NULL) );
			bseeded = true;
		}

		// just ascertain that the incoming _vector storage has actually a 
		// capacity of n slots 
		assert ( _vector && ( abs( _vector[ n - 1 ] ) + 1 ) );
		
		for( idx = 0 ; idx < n ; ++idx ) {
			if ( floor == ceiling ) { 
				curv = floor;
				continue;
			}
			else {
				curv = random () % ( ceiling - floor + 1 ) + floor;
			}
			_vector[ idx ] = curv;
		}
	}

	/*
	 * @brief randomly spawn a single floating point number, as GL points, say.
	 * @param precision an integer giving the precision of the floating point
	 *        numbers to make
	 * @parm floor giving the lower boundary of the generating range
	 * @parm ceiling giving the upper boundary of the generating range
	 * @return a single floating point number in the designated range
	 * @see genFloatvector, genDouble
	 */
	template <typename _T> // _T can be any variation of floating point types
	_T genFloat (GLsizei precision, long int floor, long int ceiling) 
	{
		if (ceiling < floor ||
			ceiling > RAND_MAX ) {
			cerr << "range not supported.\n";
			return;
		}

		static bool bseeded = false;
		long int inpart = 0;

		if ( !bseeded ) {
			srand( (unsigned int)time(NULL) );
			bseeded = true;
		}

		if ( ceiling - floor < 1e-6 ) { 
			return floor;
		}

		inpart = random() % (( ceiling - floor ) * 
			 static_cast<long int> (pow (10, precision)) );
		return inpart * 1.0 / pow (10, precision) + floor;
	}

	/*
	 * @brief randomly spawn a sequence of floating point numbers, as GL points,
	 * say.
	 * @param precision an integer giving the precision of the floating point
	 *        numbers to make
	 * @param _vector a pointer to a sequential and subscript-indexable
	 *        container used for receiving the data produced, 
	 *        note it should have a larger size than $n
	 * @param n an integer giving the number of points to produce
	 * @parm floor giving the lower boundary of the randomization range
	 * @parm ceiling giving the upper boundary of the randomization range
	 * @return none
	 * @see genFloat
	 */
	template <typename _T> // _T can be any variation of floating point types
	void genFloatvector (GLsizei precision, _T *_vector, GLsizei n, 
						long int floor, long int ceiling) 
	{
		if (ceiling < floor ||
			ceiling > RAND_MAX ||
			precision > int( log(RAND_MAX)/log(10) ) ) {
			cerr << "range or precision not supported.\n";
			return;
		}

		GLsizei idx;
		_T curv;
		static bool bseeded = false;
		long int inpart = 0;

		if ( !bseeded ) {
			srand( (unsigned int)time(NULL) );
			bseeded = true;
		}
		
		// just ascertain that the incoming _vector storage has actually 
		// a capacity of n slots 
		assert ( _vector && ( ABS( _vector[ n - 1 ] ) + 1 ) );

		for( idx = 0 ; idx < n ; ++idx ) {
			if ( ceiling - floor < 1e-6 ) { 
				curv = floor;
				continue;
			}
			else {
				inpart = random() % (( ceiling - floor ) * 
					 static_cast<long int> (pow (10, precision)) );
				curv = inpart * 1.0 / pow (10, precision) + floor;
			}
			_vector[ idx ] = curv;
		}
	}

	/*
	 * @brief randomly spawn a single double precision floating point number,
	 *  as GL points, say.
	 * @parm floor giving the lower boundary of the generating range
	 * @parm ceiling giving the upper boundary of the generating range
	 * @return a single double-precision floating point number in the 
	 *  designated range
	 * @see genFloat, genDoublevector
	 */
	template <typename _T> // _T can be any variation of floating point types
	_T genDouble (long int floor, long int ceiling) 
	{
		if (ceiling < floor ||
			ceiling > RAND_MAX ) {
			cerr << "range not supported.\n";
			return;
		}

		static bool bseeded = false;
		long int inpart = 0;
		long int lfloor = static_cast<long>(floor);
		long int lceiling = static_cast<long>(ceiling);
		_T curv;

		if ( !bseeded ) {
			srand( (unsigned int)time(NULL) );
			bseeded = true;
		}

		if ( ceiling - floor < 1e-6 ) { 
			return floor;
		}

		// randomly generate the integer part of the target
		if ( lceiling - lfloor > 0 ) {
			inpart = lrand48() % ( lceiling - lfloor + 1 );
		}
		else {
			inpart = lrand48() % 
				( static_cast<long int>(ceiling - floor) + 1 );
		}

		curv = static_cast<_T> ( inpart + drand48() + floor );
		if (curv > ceiling ) {
			curv = ceiling;
		}

		return curv;
	}

	/*
	 * @brief randomly spawn a sequence of floating point numbers, as GL points,
	 *   say. the precision needed will be postulated according to the range,
	 *   adopting the higher precision presented in either the floor or ceiling
	 * @note this routine could be viewed as an alternative to genFloatvector
	 * but different in its capability of deriving precision automatically
	 *
	 * @param _vector a pointer to a sequential and subscript-indexable
	 *        container used for receiving the data produced, 
	 *        note it should have a larger size than $n
	 * @param n an integer giving the number of points to produce
	 * @parm floor giving the lower boundary of the randomization range
	 * @parm ceiling giving the upper boundary of the randomization range
	 * @return none
	 * @see genFloatvector,genDouble
	 */
	template <typename _T> // _T can be any variation of floating point types
	void genDoublevector(_T *_vector, GLsizei n, 
						const _T floor, const _T ceiling) 
	{
		if (ceiling < floor) {
			cerr << "range or precision not supported.\n";
			return;
		}

		GLsizei idx;
		_T curv;
		static bool bseeded = false;
		long int lfloor = static_cast<long>(floor);
		long int lceiling = static_cast<long>(ceiling);
		long int inpart = 0;

		if ( !bseeded ) {
			srand48( (long int)time(NULL) );
			bseeded = true;
		}

		// just ascertain that the incoming _vector storage has actually 
		// a capacity of n slots 
		assert ( _vector && ( ABS( _vector[ n - 1 ] ) + 1 ) );

		for( idx = 0 ; idx < n ; ++idx ) {
			if ( ceiling - floor < 1e-6 ) { 
				curv = floor;
				continue;
			}

			// randomly generate the integer part of the target
			if ( lceiling - lfloor > 0 ) {
				inpart = lrand48() % ( lceiling - lfloor + 1 );
			}
			else {
				inpart = lrand48() % 
					( static_cast<long int>(ceiling - floor) + 1 );
			}

			curv = static_cast<_T> ( inpart + drand48() + floor );
			if (curv > ceiling ) {
				curv = ceiling;
			}

			_vector[ idx ] = curv;
		}
	}

	/*
	 * @brief randomly spawn one value out of the enumerated two
	 * @param v1 the first value enumerated
	 * @param v2 the second value enumerated
	 * @param p a double floating point number indicating the probability that
	 * the first value enumerated would be selected out as the return
	 * @return valued selected
	 * @see 
	 */
	template <typename _T> // _T can be any variation of numeric types available
	_T genInPool(const _T& v1, const _T& v2, double p = 0.5)
	{
		static bool bseeded = false;

		if ( !bseeded ) {
			srand48( (long int)time(NULL) );
			bseeded = true;
		}

		return (drand48() > p)? v1:v2;
	}

	/*
	 * @brief randomly spawn one value out of the enumerated two
	 * @param _vector a pointer to a sequential and subscript-indexable
	 *        container used for receiving the data produced, 
	 *        note it should have a larger size than $n
	 * @param n an integer giving the number of points to produce
	 * @param v1 the first value enumerated
	 * @param v2 the second value enumerated
	 * @param p a double floating point number indicating the probability that
	 * the first value enumerated would be selected out as the return
	 * @return valued selected
	 * @see 
	 */
	template <typename _T> // _T can be any variation of numeric types available
	void genVectorInPool(_T* _vector, GLsizei n, 
			const _T& v1, const _T& v2, double p = 0.5)
	{
		static bool bseeded = false;

		if ( !bseeded ) {
			srand48( (long int)time(NULL) );
			bseeded = true;
		}

		// just ascertain that the incoming _vector storage has actually 
		// a capacity of n slots 
		assert ( _vector && ( abs( _vector[ n - 1 ] ) + 1 ) );

		for(int idx = 0 ; idx < n ; ++idx ) {
			_vector[ idx ] = (drand48() > p)? v1:v2;
		}
	}


}; // end of namespace glrand

/* 
 * certain common functions used in GL programs
 */
template <typename _T>
inline _T magnitude(const _T& x, const _T& y, const _T& z)
{
	return sqrt(x*x + y*y + z*z);
}

template <typename _T>
inline void normalize(_T& x, _T& y, _T& z)
{
	_T mag = magnitude(x,y,z);
	if (0 == mag) {
		mag = 1;
	}

	x /= mag;
	y /= mag;
	z /= mag;
}

template <typename _T>
inline void crossproduct(_T& x, _T& y, _T& z,
					const _T& x1, const _T& y1, const _T& z1,
					const _T& x2, const _T& y2, const _T& z2)
{
	x = y1*z2 - z1*y2;
	y = z1*x2 - x1*z2;
	z = x1*y2 - y1*x2;
}

template <typename _T>
inline _T dotproduct(const _T& x1, const _T& y1, const _T& z1,
					const _T& x2, const _T& y2, const _T& z2)
{
	return x1*x2 + y1*y2 + z1*x2;
}

template <typename _T>
inline int transpoint(const _T mat[16], 
		_T& x, _T& y, _T& z, _T& w)
{
	// mat should point to an array that has 16 elements
	_T res[4] = {.0, .0, .0, .0};
	_T vec[4] = {x, y, z, w};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res[i] += mat[i*4+j]*vec[j];
		}
	}
	x = res[0], y = res[1], z = res[2], w = res[3];
	return 0;
}

// for debug use only
template <typename _T>
void dumpMatrix(const _T m[], int n=16)
{
	cout << "---------------------------------------------------------------\n";
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << m[i*4+j] << "\t";
		}
		cout << "\n";
	}
	cout << "---------------------------------------------------------------\n";
}

/*
   Write the current view to a file
   The multiple fputc()s can be replaced with
      fwrite(image,width*height*3,1,fptr);
   If the memory pixel order is the same as the destination file format.
*/
inline int glDumpImagePPM(int x, int y, int width, int height, bool stereo)
{
   int i,j;
   FILE *fptr;
   static int counter = 0; /* This supports animation sequences */
   char fname[32];
   unsigned char *image;

   /* Allocate our buffer for the image */
   if ((image = (unsigned char*)malloc(3*width*height*sizeof(char))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for image\n");
	  return -1;
   }

   glPixelStorei(GL_PACK_ALIGNMENT,1);

   /* Open the file */
   if (stereo)
      sprintf(fname,"L_%04d.ppm",counter);
   else
      sprintf(fname,"C_%04d.ppm",counter);
   if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"Failed to open file for window dump\n");
	  return -1;
   }

   /* Copy the image into our buffer */
   glReadBuffer(GL_BACK_LEFT);
   glReadPixels(x,y,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

   /* Write the PPM file */
   fprintf(fptr,"P6\n%d %d\n255\n",width,height); /* for ppm */
   for (j=height-1;j>=0;j--) {
      for (i=0;i<width;i++) {
         fputc(image[3*j*width+3*i+0],fptr);
         fputc(image[3*j*width+3*i+1],fptr);
         fputc(image[3*j*width+3*i+2],fptr);
      }
   }
   fclose(fptr);

   if (stereo) {
      /* Open the file */
      sprintf(fname,"R_%04d.ppm",counter);
      if ((fptr = fopen(fname,"w")) == NULL) {
         fprintf(stderr,"Failed to open file for window dump\n");
		  return -1;
      }

      /* Copy the image into our buffer */
      glReadBuffer(GL_BACK_RIGHT);
      glReadPixels(x,y,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

      /* Write the PPM file */
      fprintf(fptr,"P6\n%d %d\n255\n",width,height); /* for ppm */
      for (j=height-1;j>=0;j--) {
         for (i=0;i<width;i++) {
            fputc(image[3*j*width+3*i+0],fptr);
            fputc(image[3*j*width+3*i+1],fptr);
            fputc(image[3*j*width+3*i+2],fptr);
         }
      }
      fclose(fptr);
   }

   /* Clean up */
   counter++;
   free(image);
	return -1;
}

#ifdef CORONA
inline int glDumpImage(int x, int y, int width, int height)
{
   char fname[32];
   unsigned char *image;

   /* Allocate our buffer for the image */
   if ((image = (unsigned char*)malloc(4*width*height*sizeof(char))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for image\n");
	  return -1;
   }

   glPixelStorei(GL_PACK_ALIGNMENT,1);

   /* Open the file */
   sprintf(fname,"/tmp/cur.png");

   /* Copy the image into our buffer */
   glReadBuffer(GL_BACK_LEFT);
   glReadPixels(x,y,width,height,GL_RGBA,GL_UNSIGNED_BYTE,image);

   corona::Image *pimg = corona::CreateImage(width, height, corona::PF_R8G8B8A8,image);
   corona::FlipImage( pimg, corona::CA_X );
   corona::SaveImage( fname, corona::FF_PNG, pimg );

   /* Clean up */
   delete pimg;
   free(image);

   return 0;
}
#endif

#endif // _GLRAND_H_

/*vim: set ts=4 sts=4 sw=4 tw=80 */

