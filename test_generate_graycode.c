#include <stdio.h>

int binary2gray ( int ival )
{
  return ival ^ ( ival >> 1) ;
}

void toBString ( int n, char* buf, int len )
{
  for (int i = len-1; i >= 0; i--) {
        buf[i] = (n & 1)? '1': '0';
        n >>= 1;
  }
}
/*
 * Convert altitude to Gillham code for Altitude Encoder. Gillham code 
 * consists of 12 bits in groups of 3, in this order:
 *                 D1,D2,D4  A1,A2,A4  B1,B2,B4  C1,C2,C4
 *                 
 * Bits D1 and D2 are not used in most General Aviation aircaft.
 * Bits D4 through B4 are simply the Gray code representation of the number N
 * of 500 foot increments from -1000 feet, such that the mesured altitude is 
 * within +/- 200 feet.
 *  
 * To find N, we need to solve the inequality, such that:
 * 
 * -1000 + N*500 -200 <= altitude <= -1000 + N*500 + 200
 * 
 * Adding 1200 to each term and dividing by 500, we get:
 * 
 *      N <= (altitude + 1200)/500 <= N + 400/500
 *      
 * But, since N is a whole number and cannot be a fraction, 400/500 is discarded.
 * 
 * From which we get:    N = (altitude + 1200) / 500
 * 
 * For a measured altitude at +900 feet, N =4,
 * because (900 + 1200)/500 = 4, ( keeping only whole numbers, and discarding fractions)
 * As a check, we note that:
 * 
 *  -1000 + 4*500 - 200 <= 900 <= -1000 + 4*500 + 200
 *  Or,
 *             800 <= 900 <= 1200, a true statement.
 *             
 * For a measured altitude of -700 feet, N =1,
 * because (-700 + 1200)/500 = 1, and 
 * -1000 + 1*500 - 200 <= -700 <= -1000 + 1*500 + 200
 * Or,
 *              -700 <= -700 <= -300,  a true statement
 * 
 * The C1,C2,C4 bits are the Grey code representation of the number of 100' 
 * increments F from the datum.  The datum is defined to be:
 * 
 *            datum = -1300 + N*500
 *             
 *  We solve for F, where:
 *  
 *         F = ( altitude - datum )/ 100 = (altitude +1300 -N*500)/100
 * 
 * Before converting F to Gray code, two special rules are applied about half 
 * the time to transform the vlaue(to be explained below).
 * 
 * For example,
 *       If the measured altitude is -200', we get:
 * 
 *       N = ( -200 + 1200 )/500 = 2
 *       datum = -1300 + 2*500   = -300
 * and       
 *       F = ( -200 -(-300) )/100 = 1
 * 
 *  The D4 - B4 bits would contain the Gray code for 2 (N):
 *          0 000 011
 *  and bits C1 - C4 would contain the Gray code for 1 (F):
 *            001
 * giving the complete Gray code, D4 - C4:  0 000 011 001
 *
 * In general the rules are as follows:
 * 
 *   N = ( Alt + 1200 ) / 500
 *   Datum = -1300 + N*500
 *   F = ( Alt - Datum ) / 100 = ( Alt +1300 -N*500 ) / 100
 *
 * However, there are two additional special rules that are used to modify the
 * value of F before it is converted to Gray code:
 *
 * (1)  If N is Odd, then F = 6 - F  (this reverses the Gray code)
 *
 * (2)  If F = 5, then F = 7 (this uses the Gray code for 7 in place of 5)
 *
 * Otherwise, F is simply converted straight to ordinary Gray code.
 * 
*/
void ShowGrayCode_Out ( int ialt )
{
  int N500, F100, ival, gray, irem;
  char txtgil[11]={ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 0};
  char outgil[13] = {0};
  char buf[11];
  ival = ialt + 1200;
  // if the altitude is within 50 feet of the next 100,
  // output next 100 Gray code
  irem = (ialt %100);
  if ( irem >= 50 ) ival = ival + (100 - irem);
  N500 = ival / 500 ;
  F100 = ( ival + 100 -N500*500 ) / 100;
  if ( N500 %2 )   F100 = 6 - F100 ;
  if ( F100 == 5 ) F100 = 7;

  gray = binary2gray( N500 )<<3;
  gray = gray + binary2gray( F100 );
  toBString( gray, buf, 10 );

  sprintf(outgil," %i\t  %.1s    %.1s    %.1s    %.1s    %.1s    %.1s    %.1s    %.1s    %.1s    0    0    %.1s", ialt, buf+1,
                  buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf);

  printf("%s \n", outgil);
}

void main()
{
 int altitude;
 //altitude = -700;
 //ShowGrayCode_Out ( altitude );

 for (int i = -1200; i< 62800; i+=100) {
   ShowGrayCode_Out ( i ) ;
 }
}
