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
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  // consider de Morgan's law, !(a & b) = (!a | !b)
  // also x^y = (x & ~y) | (~x & y)
  return ~(~(x&~y)&~(~x&y));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  // T_min = 10000.... , with 31 0s, so the answer is obvious
  return 1<<31;

}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  // consider (x << 1) + 1 = -1 only if x = -1 or T_max, also !!(x+1) is 0 when x is -1
  return !!(!!(x+1)&!(x+2+x));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  // 0xAA is odd bit, so we just & all x's bit pattern and check against 0xAA using mask
  int test = 0xAA;
  test &= x;
  x >>= 8;
  test &= x;
  x >>= 8;
  test &= x;
  x >>= 8;
  test &= x;
  
  test &= 0xAA;
  return !(test ^ 0xAA);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) { 
  // x + ~x = -1 from bit pattern, so
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int offset = ~48 + 1; // = -48
  int range = ~10 + 1; // = -10
  /*
  // mysterious dlc error
  x += offset; // x -= 48
  int test1 = x >> 31; // get x's sign bit, it should = 0
  x += range;
  int test2 = x >> 31; // it should be 1
  return !!(!test1 & test2);
  */
  return !!((!((x+offset)>>31)) & (x+offset+range)>>31);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int mask = ~(!x) + 1; // !x give 1 if x==0, 0 if x != 0. Then we negate it to give all 1 or all 0 bit mask. Now mask = -1 if x==0
  return (y&~mask) | (z&mask);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  // check sign bit of y-x, if 1, then x > y, negate this 
  int x_sign = x >> 31;
  int y_sign = y >> 31;
  int diff_sign = (y + (~x + 1)) >> 31; // -1 if same sign and x > y
  int neg_nn = x_sign & ~y_sign; // if -1, then x is neg and y is non-neg, x < y
  int nn_neg = ~x_sign & y_sign; // if -1, then x is non-neg and y is neg, x > y
  return !!((neg_nn | ~diff_sign)& ~nn_neg); 
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  int sx = (x = ~x) >> 16;
  //x = ~x;
  
  x &= sx;
  sx = x >> 8;
  x &= sx;
  sx = x >> 4;
  x &= sx;
  sx = x >> 2;
  x &= sx;
  sx = x >> 1;
  x &= sx;
  return x & 0x1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  // convert to positive first
  int sign = x >> 31;
  int temp = (sign&(~x)) + ((~sign)&x); // so x will be positive with same bits
  int ans = 0;
  int mask = (0xFF<<8) + 0xFF;
  int left = temp >> 16;
  int right = temp & mask;
  int decision = ~(!!left) + 1;
  ans += decision & 16;
  x = temp;
  
  x = (decision&left) + ((~decision)&right);
  mask >>= 8;
  left = x >> 8;
  right = x& mask;
  decision = ~(!!left) + 1;
  ans += decision & 8;

  x = (decision&left) + ((~decision)&right);
  mask >>= 4;
  left = x >> 4;
  right = x& mask;
  decision = ~(!!left) + 1;
  ans += decision & 4;

  x = (decision&left) + ((~decision)&right);
  mask >>= 2;
  left = x >> 2;
  right = x& mask;
  decision = ~(!!left) + 1;
  ans += decision & 2;

  x = (decision&left) + ((~decision)&right);
  mask >>= 1;
  left = x >> 1;
  right = x& mask;
  decision = ~(!!left) + 1;
  ans += decision & 1;

  ans += !!x; // for base case
    
  return ans + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned e = (uf >> 23) & 0xFF;
  unsigned frac = uf & 0x007FFFFF;
  unsigned left = frac >> 22;

  if (!(e^0xFF)) { // if uf represents NaN or Infinity, just return
    return uf;
  }

  if (e) { // if uf is normalized, then we just add 1 to exp, which means *2
    e += 1;
  } else { // if uf is Denormalized, we bit level times two first, then check origional leftmost bits
    frac <<= 1;
    if (left) {
      e += 1;
    }
  }

  uf &= 0x80000000; // just keep sign
  uf |= (e&0xFF) << 23; // put back exp
  uf |= frac& 0x007FFFFF; // put back frac
  return uf;
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
int floatFloat2Int(unsigned uf) {
  // true for uf > 0
  unsigned exp = (uf >> 23) & 0xFF;
  unsigned frac = uf & 0x007FFFFF;
  unsigned s = uf >> 31;
  if (exp > 30+127) { // bigger than int max = 2^31 - 1
    return 0x80000000u;
  }
  if (exp < 127) { // exp < 127, so E < 0, so s = 0.xxxx
    return 0;
  }
  // as uf is normalized, so M = 1.frac, we just do the suitable shift
  frac |= 0x1 << 23;
  if (exp > 150) {
    frac <<= (exp-150);
  } else {
    frac >>= (150-exp);
  }
  // frac now is unsigned
  if (s) {
    return ~frac + 1;
  }
  return frac;
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
unsigned floatPower2(int x) {
  int frac = 0;
  int exp = 1;
  if (x > 127) { // float max power of 2 = 2^127
    return 0x7f800000;
  }
  if (x < -126) { // float max power of 2 = 2^-126
    return 0;
  }
  if (x > -127) { // normalized
    exp = x + 127;
    return exp << 23;
  }
  frac <<= (22-x+127); // denormalized
  return frac;
}
