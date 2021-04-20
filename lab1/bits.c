/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
//1
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y){	
	return (~(~x | ~y));
}
/*
 * bitMatch - Create mask indicating which bits in x match those in y
 *            using only ~ and &
 *   Example: bitMatch(0x7, 0xE) = 0x6
 *   Legal ops: ~ & |
 *   Max ops: 14
 *   Rating: 1
 */
int bitMatch(int x, int y){	
	return ((~x & ~y) | ((x & y)));
}
/*
 * bitNor - ~(x|y) using only ~ and &
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y){
	return (~x & ~y);
}
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y){
	return (~(x & y) & ~(~x & ~y));
}
//2
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x){

  int mask = 0x55; // mascara con 1 en posiciones pares.
	// creo una mascara de 8 bytes (lo permitido) y voy operando hasta obtener una mascara de 32 bytes (lo que necesito para trabajar). 
 	mask = mask | (mask << 8);
  	mask = mask | (mask << 16);
  	
 	return !(~(x | mask)); // al hacer el ~ del resultado obtengo si x tiene en todas sus posiones pares un 1.
}
/*
 * anyEvenBit - return 1 if any even-numbered bit in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples anyEvenBit(0xA) = 0, anyEvenBit(0xE) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyEvenBit(int x) {
	int mask_8 = 0X55;
	int mask_16 = mask_8 | (mask_8  << 8);
	int mask_32 = mask_16 | (mask_16 << 16); 

	return !!(x & mask_32); // al hacer & con la mascara me quedo con los bits pares.
}
/*
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m){
  int corrimiento_n = n << 3; // obtengo la cantidad de bits a correr para ubircarme en el bit n.
  int corrimiento_m = m << 3; // obtengo la cantidad de bits a correr para ubircarme en el bit m.
  int cleaner = 0;
  int reassign = 0;
  int allHigh = 0xFF;
  int cleanByteN = ~(allHigh << corrimiento_n); // mascara que tiene el byte n igual a 00 y el resto igual a ff.
  int cleanByteM = ~(allHigh << corrimiento_m); // mascara que tiene el byte m igual a 00 y el resto igual a ff.
  int byteAUsarN = ((allHigh << corrimiento_n) & x) >> corrimiento_n & allHigh; // se copia el byte n y se lo corre hasta el byte de la posisicon 0.
  int byteAUsarM = ((allHigh << corrimiento_m) & x) >> corrimiento_m & allHigh; // se copia el byte m y se lo corre hasta el byte de la posisicon 0.

  byteAUsarN = byteAUsarN << corrimiento_m; // se coloca el byte n en la posicion del byte m.
  byteAUsarM = byteAUsarM << corrimiento_n; // se coloca el byte m en la posicion del byte n.
  cleaner = cleanByteM & cleanByteN; // se forma la mascara con ff en los bytes distintos de n y m.
  reassign = byteAUsarN | byteAUsarM; // se forma la mascara con los bytes n y m swapeados y el resto de bytes 00.

  return ((x & cleaner) | reassign); // se borra de x los bytes n y m, se reasignan los bytes swapeados.
}
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n){
	int shift = 32 + (~n + 1); // diferencia entre 32 (maximo numero de n) y n ---> 32 + (~n + 1) = 32 - n ---> 32 bits - n bits.

	return !(x ^ ((x << shift) >> shift)); 
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x){
	return (~x + 1);
}
/*
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Rating: 2
 */
int sign(int x){
	return (x >> 31 | (!!x)); // si el numero es negativo, al hacer el corrimiento hacia derecha se agregan 1. !!1 = 1 
							  //---> si es positivo, al hacer | devuelve 1. Devuelve -1 en caso contrario.
}
//3
/*
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y){
  	// overflow cuando: el signo de x e y es el mismo
  	//					el signo de x o el de y es distinto al signo del resultado
  	int suma = x + y;
  	
  	return !(((x ^ suma) & (y ^ suma)) >> 31); // veo si el signo de x o el signo de y es distinto al de la suma.
}
/*
 * bitMask - Generate a mask consisting of all 1's
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit){
 	int mask = ~0;
 	int maskLow = mask << lowbit; // se genera la mascara con todos los bits en alto menos en el intervalo de lowbit hasta el bit menos significativo.
 	int maskHigh = mask << (highbit); // se genera la mascara con todos los bits en alto menos en el intervalo de highbit hasta el bit menos significativo.

  	maskHigh = maskHigh << 1; // se realiza un corrimiento mas, ya que el bit en la posicion highbit tiene que incluirse.

 	return (~maskHigh & (maskHigh ^ maskLow)); // obtengo el intervalo de lowbit a highbit. 
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z){
	int mask = ~((!x << 31) >> 31); // si mask = 0 ---> x = 0

 	return ((mask & y) | ((~mask) & z)); // si mask = 0 ---> devuelvo z. Devuelvo y en caso contrario.
}
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x){
	int bitSigno = 0x1 << 31;
	int limiteInf = 0x30;
	int limiteSup = 0x39;
	int infVsX = x + (~limiteInf + 1); // se resta x - limite inferior de digitos ascii.
	int supVsX = limiteSup + (~x + 1); // se resta limite superior de digitos - x.

	return (!(infVsX & bitSigno) & !(supVsX & bitSigno)); // si es un digito ascii debe estar en el rango, entonces ninguno de los signos debe ser negativo.
}
/*
 * isGreater - if x > y  then return 1, else return 0
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y){
 	int resta = (x + (~y + 1)) >> 31; // se obtiene el signo de la resta.
	int signoX = x >> 31; 
 	int signoY = y >> 31; 
 	int addOK = (signoX ^ !signoY) | !(signoX ^ resta); // se comprueba si hubo overflow.

 	return (((!signoX) & signoY) | ((!resta) & addOK)) & (!!(x ^ y));
 	// si x es positivo e y es negativo ---> x > y. Si la resta es positiva y ambos no son 0 ---> devuelve 1.
}
/*
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c){
	int shift_n = n << 3;
	int cleanByte = ~(0xFF << shift_n); // mascara para borrar el byte anterior.
	int newByte = c << shift_n; // mascara para ingresar el nuevo byte.

	return ((x & cleanByte) | newByte); // se borra el byte anterior y se ingresa el nuevo.
}
//4
/*
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x){
	int mask = x >> 31; // me fijo el signo de x.

  	return (x ^ mask) + (~mask + 1); 
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x){
	int noEsCero = (( x | (~x + 1)) >> 31) & 0x1; // da 0 si x = 0.
	int esCero = noEsCero ^ 0x1; // da 1 si x = 0.

	return esCero;
}
/*
 * isNonZero - Check whether x is nonzero using
 *              the legal operators except !
 *   Examples: isNonZero(3) = 1, isNonZero(0) = 0
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int isNonZero(int x){
	int mask = 0x1;

  	return ((x | (~x +1)) >> 31) & mask; 
}
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x){
	int mask = 0x1;
  	
  	return (((x | (~x +1)) >> 31) ^ 1) & mask;
}
//float
/*
 * floatAbsVal - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned floatAbsVal(unsigned uf){
	unsigned exp = (uf & 0x7f800000) ^ 0x7f800000; // me quedo con su exponente y lo convierto a 0.
	unsigned fracc = uf & 0x007fffff; // me quedo con la fraccion.

	if(!!fracc && !exp) // si el exponente es 0 y la fraccion es distinta de 0 ---> NaN.
		return uf;						 
	else
		return uf & 0x7fffffff; // devuelvo el valor absoluto
}
/*
 * floatIsEqual - Compute f == g for floating point arguments f and g.
 *   Both the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   If either argument is NaN, return 0.
 *   +0 and -0 are considered equal.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 25
 *   Rating: 2
 */
int floatIsEqual(unsigned uf, unsigned ug){
	unsigned exp_f = (uf & 0x7f800000) ^ 0x7f800000;
	unsigned fracc_f = uf & 0x007fffff; 
	unsigned exp_g = (ug & 0x7f800000) ^ 0x7f800000;
	unsigned fracc_g = ug & 0x007fffff;  

	if((!!fracc_f && !exp_f) && (!!fracc_g && !exp_g)) // me fijo si alguno es NaN.
		return 0;
  	else if(!(uf & 0x7fffffff) && !(ug & 0x7fffffff)) // me fijo si son +0 o -0.
   		return 1;
  	else
   		return uf == ug;
}
/*
 * floatNegate - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned floatNegate(unsigned uf){
	unsigned exp = (uf & 0x7f800000) ^ 0x7f800000;
	unsigned fracc = uf & 0x007fffff;   

	if(!!fracc && !exp)
		return uf;
 	else
 		return (uf ^ 0x80000000); // agrego un 1 al principio (bit de signo). 
}
/*
 * floatIsLess - Compute f < g for floating point arguments f and g.
 *   Both the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   If either argument is NaN, return 0.
 *   +0 and -0 are considered equal.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 3
 */
int floatIsLess(unsigned uf, unsigned ug){
 	unsigned exp_f = (uf & 0x7f800000) ^ 0x7f800000;
  	unsigned fracc_f = uf & 0x007fffff;
	unsigned exp_g = (ug & 0x7f800000) ^ 0x7f800000;
	unsigned fracc_g = ug & 0x007fffff;
	unsigned sign_uf = (uf >> 31); 
	unsigned sign_ug = (ug >> 31);
		   	
	if((fracc_f && !exp_f) || (fracc_g && !exp_g))
		return 0;
	else if(!(uf & 0x7fffffff) && !(ug & 0x7fffffff))
    	return 0;
 	else if(sign_uf > sign_ug || (!!sign_ug && (ug < uf)) || (!sign_ug && (uf < ug))) 
 		//   uf neg y ug pos  ||      ambos negativos     ||     ambos positivos.
    	return 1;
   
  return 0;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf){
	int mask_range = 0xff;
 	int bias = 127;
 	int exp = (uf >> 23) & 0xFF;
 	int fracc = uf & 0x7FFFFF;
 	int sign = (uf >> 31) & 1;  
	int E = exp - bias;   	
	int resultado = 0;
 
	if(exp == mask_range || E > 31) // NaN, INF o mas grande que los valores que se pueden representar en 32 bits.
  		return 0x80000000u;  

  	if(!exp || E < 0) // menor que los valores que se pueden representar en 32 bits.
  		return 0; 	

  	fracc = fracc | 0x800000; // agrega un 1 a la izquerda. 

	if(E > 23) 
		resultado = fracc << (E - 23);
 	else 
 		resultado = fracc >> (23 - E);

 	if(sign == 1) 
 		return ~resultado + 1; // si el signo de uf es negativo ---> devuelvo el resultado negativo.
 	 else 
  		return resultado;	
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x){
	int exp = x + 127;
	
	if(exp <= 0) // resultado muy pequenio para ser representado
		return 0;
	else if(exp >= 255) // resultado muy largo para ser representado, devuelvo +INF (todos 1 en el exponente y todos 0 en la fraccion).
		return 0xff << 23;
	else
		return exp << 23;
}
