#include <itpp/itbase.h>


/** Convert an array of complex short into an array of complex double
 * @pre:
 *    - array: the pointer to the array of complex short
 *    - nElem: the number of elem in array
 *    - out: a pointer to an complex double array of size nElem
 *
 * @post:
 *    - out is now filled with complex double
 */
void compShortToCompDouble(complex<short> *array, int nElem, complex<double>* out)


/** Convert an array of complex double into an array of complex short
 * @pre:
 *    - array: the pointer to the array of complex double
 *    - nElem: the number of elem in array
 *    - out: a pointer to an complex short array of size nElem
 *                                               --------------
 * @post:
 *    - out is now filled with complex short
 */
void compDoubleToCompShort(complex<double> *array, int nElem, complex<short>* out)
