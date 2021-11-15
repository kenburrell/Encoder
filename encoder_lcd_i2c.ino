/*
 * Simple code to input the parallel (Gillham code) and serial data from an 
 * Altitude Encoder, and/or output a selected altitude in parallel (Gillham)
 * and serial format.  Serial format is user selectable via a Menu for either
 * input or output, and altitude is user selectable for output.
 * 
 * Code is for a 5V Arduino Nano interfacing 14V Altitude Encoders.
 * 
 * Copyright 01 September 2020.  Ken Burrell.
 * 
 * Code freely available under the terms of the Creative Commons License.
 *
*/

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

#include <Wire.h>
#include <I2CKeyPad.h>
const uint8_t KEYPAD_ADDRESS = 0x20;
I2CKeyPad keyPad;
uint32_t lastKeyPressed = 0;

int iSel, SerialLen, ibeg, iend;
char SerialEnd ;
String SerialSpeed;
String SerialName[] ={"IIMorrw","UPS AT","Magelln","NorStar","Icarus","Shadin","ARNAV","SERIAL OFF"} ;

int mFlag = 0; 
int16_t ialto = -9980 ;
char Alt [7] = {0};
char alto[7] = {0};
char galt0[7] = {0};
char galt[7] = {0} ;

void myMenu ()
{  
  uint8_t idx = -1 ;

  for ( int i=0; i<7; i++){
    galt0[i] = 0;
  }
  
  lcd.clear();
  
  lcd.setCursor( 0,0); lcd.print("0-"); lcd.setCursor( 2,0); lcd.print(SerialName[0]) ;
  lcd.setCursor(10,0); lcd.print("1-"); lcd.setCursor(12,0); lcd.print(SerialName[1]) ;
  lcd.setCursor( 0,1); lcd.print("2-"); lcd.setCursor( 2,1); lcd.print(SerialName[2]) ;
  lcd.setCursor(10,1); lcd.print("3-"); lcd.setCursor(12,1); lcd.print(SerialName[3]) ;
  lcd.setCursor( 0,2); lcd.print("4-"); lcd.setCursor( 2,2); lcd.print(SerialName[4]) ;
  lcd.setCursor(10,2); lcd.print("5-"); lcd.setCursor(12,2); lcd.print(SerialName[5]) ;
  lcd.setCursor( 0,3); lcd.print("6-"); lcd.setCursor( 2,3); lcd.print(SerialName[6]) ;
  lcd.setCursor( 8,3); lcd.print("7-"); lcd.setCursor(10,3); lcd.print(SerialName[7]) ;

  do {
    if ( keyPad.isPressed() )
    {
     idx = keyPad.getKey();
     switch ( idx ) {
      case 13:
        iSel = 0;
        break;
      case 0:
        iSel = 1;
        break;
      case 1:
        iSel = 2;
        break;
      case 2:
        iSel = 3;
        break;
      case 4:
        iSel = 4;
        break;
      case 5:
        iSel = 5;
        break;
      case 6:
        iSel = 6;
        break;
      case 8:
        iSel = 7;
        break;
     }
    }
    delay(250);
    //lcd.setCursor(12,3); lcd.print(idx) ;
  } while( idx != 12) ;

  lcd.clear();
  
  Serial.flush();
  
  switch (iSel) {
    case 0:
      Serial.begin(1200, SERIAL_7O1);
      SerialSpeed = "1200.7O1";
      SerialLen = 16 ;
      ibeg = 4; iend = ibeg + 6;
      SerialEnd = '\r' ;
      break;
    case 1:
      Serial.begin(1200, SERIAL_8N1);
      SerialSpeed = "1200.8N1";
      SerialLen = 16 ;
      ibeg = 4; iend = ibeg + 6;
      SerialEnd = '\r' ;
      break;
    case 2:
      Serial.begin(1200, SERIAL_7E1);
      SerialSpeed = "1200.7E1";
      SerialLen = 16 ;
      ibeg = 4; iend = ibeg + 6;
      SerialEnd = '\r' ;
      break;
    case 3:
      Serial.begin(2400, SERIAL_8N1);
      SerialSpeed = "2400.8N1";
      SerialLen = 9 ;
      ibeg = 4; iend = ibeg + 5;
      SerialEnd = '\r' ;
      break;
    case 4:
      Serial.begin(9600, SERIAL_8N1);
      SerialSpeed = "9600.8N1";
      SerialLen = 9 ;
      ibeg = 4; iend = ibeg + 5;
      SerialEnd = '\r' ;
      break;
    case 5:
      Serial.begin(9600, SERIAL_8N1);
      SerialSpeed = "9600.8N1";
      SerialLen = 16 ;
      ibeg = 4; iend = ibeg + 6;
      SerialEnd = '\r' ;
      break;
    case 6:
      Serial.begin(9600, SERIAL_8N1);
      SerialSpeed = "9600.8N1";
      SerialLen = 21 ;
      ibeg = 11; iend = ibeg + 6;
      SerialEnd = '\n' ;
      break;
    case 7:
      SerialSpeed = "        ";
      SerialLen = 0 ;
      ibeg = 0; iend = 0;
      break;
  }
  delay(500);
  if ( iSel == 7 )
  {
    lcd.setCursor( 0,0); lcd.print("Serial is disabled.");
  } else {
    lcd.setCursor( 0,0); lcd.print(SerialName[iSel]) ; lcd.setCursor(10,0); lcd.print ("Selected");
    lcd.setCursor( 9,1); lcd.print("#"); lcd.setCursor(10,1);  lcd.print(iSel); lcd.setCursor(11,1);  lcd.print(" ");
    lcd.setCursor(12,1); lcd.print(SerialSpeed);
    lcd.setCursor( 0,2); lcd.print("If no data restart"); lcd.setCursor(0,3); lcd.print("and select another.");
  }
  delay(2000);
  lcd.clear();
}

void myMenu2 ()
{
  uint8_t idx = -1;
  int ipos = 0, isgn = 1 ;

  for ( int i=0; i<7; i++){
    galt0[i] = 0;
    alto[i] = 0;
  }

  lcd.clear();
  lcd.setCursor( 0,0); lcd.print("Enter Altitude feet");
  lcd.setCursor( 0,1); lcd.print("flip sign w/ F4 key");
  lcd.setCursor( 0,1); lcd.print("Green=End Red=Erase");

  String altitude = "";
  
  do {
    if ( keyPad.isPressed() )
    {
     idx = keyPad.getKey();
     switch ( idx ) {
      case 13:
        altitude = altitude + '0'; ipos++ ;
        break;
      case 0:
        altitude = altitude + '1'; ipos++ ;
        break;
      case 1:
        altitude = altitude + '2'; ipos++ ;
        break;
      case 2:
        altitude = altitude + '3'; ipos++ ;
        break;
      case 4:
        altitude = altitude + '4'; ipos++ ;
        break;
      case 5:
        altitude = altitude + '5'; ipos++ ;
        break;
      case 6:
        altitude = altitude + '6'; ipos++ ;
        break;
      case 8:
        altitude = altitude + '7'; ipos++ ;
        break;
      case 9:
        altitude = altitude + '8'; ipos++ ;
        break;
      case 10:
        altitude = altitude + '9'; ipos++ ;
        break;
      case 14:
        if ( ipos > 1 )
        {
          ipos--; altitude = altitude.substring(0,ipos);
        } else {
          ipos = 0; altitude = "";
        }
        break;
      case 15:
        if ( ipos > 0 ) isgn = -1 * isgn ;
        //lcd.setCursor(0,3); lcd.print(isgn);
        break;
     }
    }
    delay(250);
    if ( ipos > 0 )
    {
      ialto = altitude.toInt() ;
      if ( isgn < 0 ) {
        ialto = -ialto ;
        sprintf( Alt, "-%05i", abs(ialto) );
      } else {
        sprintf( Alt, "+%05i", ialto );
      }
      lcd.setCursor(0,2); lcd.print("      ");
      lcd.setCursor(0,2); lcd.print(Alt);
    }
  } while( idx != 12) ;
  if ( ipos > 0 )  {
    if ( ialto < -1300 ) ialto = -1300;
    if ( ialto > 62700 ) ialto = 62700;
  }
  lcd.clear();
}

const int gA1 = 4, gA2=5, gA4=6 ;
const int gB1 = 7, gB2=8, gB4=9 ;
const int gC1 = 10, gC2=11, gC4=12, gD4=3;

char txtgil[11]={ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 0};
char outgil[13] = {0};

bool alt_chg ;
int16_t gval, itemp;

void ShowGrayCode_In ()
{
  if ( digitalRead(gD4) == HIGH ) {
    txtgil[0] = '0';
  } else {
    txtgil[0] = '1';
  }
  if ( digitalRead(gA1) == HIGH ) {
    txtgil[1] = '0';
  } else {
    txtgil[1] = '1';
  }
  if ( digitalRead(gA2) == HIGH ) {
    txtgil[2] = '0';
  } else {
    txtgil[2] = '1';
  }
  if ( digitalRead(gA4) == HIGH ) {
    txtgil[3] = '0';
  } else {
    txtgil[3] = '1';
  }
  if ( digitalRead(gB1) == HIGH ) {
    txtgil[4] = '0';
  } else {
    txtgil[4] = '1';
  }
  if ( digitalRead(gB2) == HIGH ) {
    txtgil[5] = '0';
  } else {
    txtgil[5] = '1';
  }
  if ( digitalRead(gB4) == HIGH ) {
    txtgil[6] = '0';
  } else {
    txtgil[6] = '1';
  }
  if ( digitalRead(gC1) == HIGH ) {
    txtgil[7] = '0';
  } else {
    txtgil[7] = '1';
  }
  if ( digitalRead(gC2) == HIGH ) {
    txtgil[8] = '0';
  } else {
    txtgil[8] = '1';
  }
  if ( digitalRead(gC4) == HIGH ) {
    txtgil[9] = '0';
  } else {
    txtgil[9] = '1';
  }
  // formatted print of Gillham string
  //lcd.setCursor(10,0); lcd.print(txtgil);
  sprintf(outgil,"%.1s %.3s %.3s %.3s", txtgil, txtgil+1, txtgil+4, txtgil+7);
  lcd.setCursor(7,0); lcd.print(outgil);

  
  // convert Gillham string to numeric value
  gval = 0 ;
  for ( int i=0; i<10; i++){
    gval <<= 1;
    if (txtgil[i] == '1') gval^=1; 
  }
  
  // Convert Gillham value to binary using gray code to binary conversion algorithm.
  itemp = gval ^ (gval >> 8);
  itemp ^= (itemp >> 4);
  itemp ^= (itemp >> 2);
  itemp ^= (itemp >> 1);
    
  // Convert gray code converted binary to altitude offset.
  itemp -= ( ((itemp >> 4) * 6) + (((itemp % 16) / 5) * 2) );
  
  // Convert altitude offset to altitude in hundreds of feet.
  itemp = (itemp - 13)*100 ;
  if ( itemp <= -1300 ) itemp = -9980;
  if ( itemp < 0 ) {
    itemp = abs(itemp);
    sprintf( galt, "-%05i", itemp );
  } else {
    sprintf( galt, "+%05i", itemp );
  }
  alt_chg = false;
  for ( int i=0; i<7; i++){
    if ( galt[i] == galt0[i] ) continue;
    galt0[i] = galt[i];
    alt_chg = true;
  }
  if ( alt_chg )
  {
    lcd.setCursor(0,0);
    lcd.print("       ");
    lcd.setCursor(0,0);
    lcd.print(galt);
  }
}

int16_t binary2gray ( int16_t ival )
{
  return ival ^ ( ival >> 1) ;
}

void toBString ( int16_t n, char* buf, int len )
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
 * But, since we are performing whole number arithmetic(discarding fractions), 400/500 is zero
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
 *       datum = -1300 + N*500   = -300
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
void ShowGrayCode_Out ( int16_t ialt )
{
  int16_t N500, F100, ival, g500, g100, irem;
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
  
  g100 = binary2gray( F100 );
  g500 = binary2gray( N500 );

  toBString(g500, &buf[0], 7);
  toBString(g100, &buf[7], 3);
  sprintf(outgil,"%.1s %.3s %.3s %.3s", buf, buf+1, buf+4, buf+7);
  lcd.setCursor(7,2); lcd.print(outgil);
}

void setup() 
{
  pinMode(gD4,INPUT_PULLUP);
  pinMode(gA1,INPUT_PULLUP);
  pinMode(gA2,INPUT_PULLUP);
  pinMode(gA4,INPUT_PULLUP);
  pinMode(gB1,INPUT_PULLUP);
  pinMode(gB2,INPUT_PULLUP);
  pinMode(gB4,INPUT_PULLUP);
  pinMode(gC1,INPUT_PULLUP);
  pinMode(gC2,INPUT_PULLUP);
  pinMode(gC4,INPUT_PULLUP);
   
  lcd.init();
  lcd.backlight();
  
  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin(KEYPAD_ADDRESS) == false)
  {
    lcd.print("ERROR: Please reboot");
    while(1);
  }
  
}

void loop() 
{

 while ( Serial.available()> 0 ) {
   String data = Serial.readStringUntil( SerialEnd );
   if ( data.length() == SerialLen )
   {
    String alt = data.substring(ibeg,iend);
    itemp = alt.toInt() ;
    if ( itemp <= -1300 ) itemp = -9980;
    if ( itemp < 0 ) {
      itemp = abs(itemp);
      sprintf( galt, "-%05i", itemp );
    } else {
      sprintf( galt, "+%05i", itemp );
    }
    lcd.setCursor(0,1);
    lcd.print(galt);
    lcd.setCursor(9,1);  lcd.print("#"); lcd.setCursor(10,1);  lcd.print(iSel); lcd.setCursor(11,1);  lcd.print(" ");
    lcd.setCursor(12,1); lcd.print(SerialSpeed);
   }
  }
  
  ShowGrayCode_In() ;

  if ( ialto < 0 ) {
      sprintf( alto, "-%05i", abs(ialto) );
  } else {
      sprintf( alto, "+%05i", ialto );
  }
  if ( ialto > -1301) {
    lcd.setCursor(0,2); lcd.print(alto);
    ShowGrayCode_Out( ialto ) ;
  }

  if ( keyPad.isPressed() )
  {
     uint8_t lk = keyPad.getKey();
     if ( lk == 3 )
     {
       myMenu();
     } else if ( lk == 7) {
       myMenu2();
     }
  }
  delay (100);
}
