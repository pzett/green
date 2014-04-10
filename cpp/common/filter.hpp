
/** Filter the real array x by the filter h(q)=b(q)/a(q)
 *
 *                             b[0]+b[1]*q^(-1)+...+b[nElemB-1]*q^(nElemB-1)
 *                       h(q)=----------------------------------------------
 *                              1+a[1]*q^(-1)+...+a[nElemA-1]*q^(nElemA-1)
 * @pre:
 *    - b: pointer to array of the numerator of h(q) b[n]
 *    - nElemB: size of b[]
 *    - a: pointer to array of the numerator of h(q) a[n]
 *             !!!  a[0]=1 !!!
 *    - nElemA: size of a[]
 *    - x: the array to filter
 *    - y: the array of filtered elements, size nElemIN
 *    - nElemIN: the size of x[] and y[]
 *
 * @post:
 *    - y is now filled
 *
 */
template <class T, class U> // To be removed for use !!!
void filter(U b[], int nElemB, U a[], int nElemA, T x[], T y[], int nElemIN)


/** Split a complex array into an array of real and an array of imag
 *
 * @pre:
 *    - in: pointer to complex array to split
 *    - real: pointer to array that will contain the real part
 *                            size of nElem/2
 *    - imag: pointer to array that will contain the real part
 *                            size of nElem/2
 *    - nElem: the size of in[]
 *
 * @post:
 *    - real is now filled
 *    - imag is now filled
 */
template <class T>
void splitComp(T in[], T real[], T imag[], int nElem)


/** Merge an array of real and an array of imag into an array of complex
 *
 * @pre:
 *    - in: pointer to complex array to split
 *    - real: pointer to array that contains the real part
 *                            size of nElem
 *    - imag: pointer to array that contains the real part
 *                            size of nElem
 *    - out: pointer that will contain the complex array
 *                            size of 2*nElem
 *    - nElem: the size of real[]
 *
 * @post:
 *    - out is now filled
 */
template <class T>
void compMerge(T real[], T imag[], T out[], int nElem)


/** Filter the complex array x by the filter h(q)=b(q)/a(q)
 *
 *                             b[0]+b[1]*q^(-1)+...+b[nElemB-1]*q^(nElemB-1)
 *                       h(q)=----------------------------------------------
 *                              1+a[1]*q^(-1)+...+a[nElemA-1]*q^(nElemA-1)
 * @pre:
 *    - b: pointer to array of the numerator of h(q) b[n]
 *    - nElemB: size of b[]
 *    - a: pointer to array of the numerator of h(q) a[n]
 *             !!!  a[0]=1 !!!
 *    - nElemA: size of a[]
 *    - x: the array to filter
 *    - y: the array of filtered elements, size nElemIN
 *    - nElemIN: the size of x[] and y[]
 *
 * @post:
 *    - y is now filled
 */
template <class T, class U>
void filterComp(U b[], int nElemB, U a[], int nElemA, T x[], T y[], int nElemIN)
