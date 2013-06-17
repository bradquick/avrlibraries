#include "i2clcd.h"

/*
LiquidTWI High Performance i2c LCD driver
by Matt Falcon (FalconFour) / http://falconfour.com
Adapted and optimized from LiquidCrystal bundled with Arduino

Version 1.1

Changelog:
* 1.1 / 6-17-2011:
  Oops... initial bugchecking was done on one program (performance test).
  LCD_DISPLAYCONTROL functions were sending bit 7 (backlight) to the LCD as
  part of the command, invalidating most commands and setting the cursor to
  address 0 instead of doing their function. That also screwed up the
  initialization routine something fierce. A miracle this slipped by testing.
* 1.0 / 6-12-2011:
  Initial release

Distributed with as free-will a license as is available given this code's long
associative chain of licenses (LiquidCrystal -> Arduino -> Wiring -> ...?). Use
it in any way you feel fit, profit or free, provided it fits in the licenses of
its associated works.

Usage:
Attach CLK pin to ANALOG 5 pin (remember: analog pins double as digital!),
Attach DAT pin to ANALOG 4 pin,
Give it 5V using    5V     pin,
Then,  GND it with  GND    pin, and you're all set!

LiquidTWI lcd(0);
void setup() { lcd.begin(16,2); }
void loop() { lcd.print("Happy!"); delay(500); lcd.clear(); delay(500); }

If you changed the i2c address of the board (multiple LCDs? I dig it!), set
"lcd(0)" to your new 3-bit address.
If you have more than a 16x2 LCD, change "lcd.begin(16,2)" to reflect the
columns and rows of your LCD.
*/

#include <stdio.h>
#include <string.h> 
#include <inttypes.h>
#include "i2cmaster.h"
#include "timers.h"
//#include "WProgram.h"
//#include <Wire.h>

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidTWI constructor is called). This is why we save the init commands
// for when the sketch calls begin(), except configuring the expander, which
// is required by any setup.

uint8_t _displaycontrol;

	
/*********** mid level commands, for sending data/cmds */
inline void i2clcd_command(uint8_t value) 
	{
	i2clcd_send(value, LOW);
	}
	
inline void i2clcd_write(uint8_t value) 
	{
	i2clcd_send(value, HIGH);
	}

char i2clcd_init() 
	{ // return non zero on error - usually means we can't find an lcd
	uint8_t _displayfunction = LCD_2LINE; // in case they forget to call begin() at least we have something

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delaymilli(50);
	// first thing we do is get the GPIO expander's head working straight, with a boatload of junk data.
	char value=i2c_start_wait(MCP23008_ADDRESS+I2C_WRITE);// | _i2cAddr);
   if (value) return(1);
	
	i2c_write(MCP23008_IODIR);
	i2c_write(0xFF);
	i2c_write(0x00);
	i2c_write(0x00);
	i2c_write(0x00);
	i2c_write(0x00);
	i2c_write(0x00);
	i2c_write(0x00);
	i2c_write(0x00);
	i2c_write(0x00);
	i2c_write(0x00);	

	// now we set the GPIO expander's I/O direction to output since it's soldered to an LCD output.
	i2c_rep_start(MCP23008_ADDRESS+I2C_WRITE);// | _i2cAddr);
	i2c_write(MCP23008_IODIR);
	i2c_write(0x00); // all output: 00000000 for pins 1...8
	i2c_stop();

	//put the LCD into 4 bit mode
	// start with a non-standard command to make it realize we're speaking 4-bit here
	// per LCD datasheet, first command is a single 4-bit burst, 0011.
	i2clcd_burstBits(0x94); //B10010100); // buffer that out to I/O pins: 1001110x with enable and backlight on
	i2clcd_burstBits(0x90); //B10010000); // buffer that out to I/O pins: 1001100x with enable low, backlight on
	delaymilli(5); // this shouldn't be necessary, but sometimes 16MHz is stupid-fast.
	
	i2clcd_command(LCD_FUNCTIONSET | _displayfunction); // then send 0010NF00 (N=lines, F=font)
	delaymilli(5); // for safe keeping...
	i2clcd_command(LCD_FUNCTIONSET | _displayfunction); // ... twice.
	delaymilli(5); // done!

	// turn on the LCD with our defaults.
	_displaycontrol = LCD_DISPLAYON;  
	i2clcd_setdisplay(1);
	// clear it off
	i2clcd_clear();

	uint8_t _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	i2clcd_command(LCD_ENTRYMODESET | _displaymode);

	i2clcd_setbacklight(HIGH); // turn the backlight on if so equipped.
	
	return(0);
	}

/********** high level commands, for the user! */
void i2clcd_clear()
	{
	i2clcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	delaymilli(2);  // this command takes a long time!
	}

void i2clcd_home()
	{
	i2clcd_command(LCD_RETURNHOME);  // set cursor position to zero
	delaymilli(2);  // this command takes a long time!
	}

void i2clcd_moveto(uint8_t col, uint8_t row)
	{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	i2clcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
	}

void i2clcd_setdisplay(char on)
	{
	if (on) _displaycontrol |= LCD_DISPLAYON;
	else _displaycontrol &= ~LCD_DISPLAYON;
	i2clcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
	}

void i2clcd_setcursor(char on,char blinking)
	{
	if (on) _displaycontrol |= LCD_CURSORON;
	else _displaycontrol &= ~LCD_CURSORON;
	if (blinking) _displaycontrol |= LCD_BLINKON;
	else _displaycontrol &= ~LCD_BLINKON;
	i2clcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
	}


// These commands scroll the display without changing the RAM
void i2clcd_scrollLeft(void) 
	{
	i2clcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
	}
	
void i2clcd_scrollright(void) 
	{
	i2clcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
	}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void i2clcd_createchar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	i2clcd_command(LCD_SETCGRAMADDR | (location << 3));
	int i;
	for (i=0; i<8; i++) {
		i2clcd_write(charmap[i]);
	}
}

/************ low level data pushing commands **********/

// Allows to set the backlight, if the LCD backpack is used
void i2clcd_setbacklight(uint8_t status) 
	{
	_displaycontrol=(_displaycontrol & ~(LCD_BACKLIGHT)) | (LCD_BACKLIGHT*status);

	i2clcd_burstBits((_displaycontrol & LCD_BACKLIGHT)?0x80:0x00);
	}

 
// write either command or data, burst it to the expander over I2C.
void i2clcd_send(uint8_t value, uint8_t mode) 
	{
	// BURST SPEED, OH MY GOD
	// the (now High Speed!) I/O expander pinout
	// RS pin = 1
	// Enable pin = 2
	// Data pin 4 = 3
	// Data pin 5 = 4
	// Data pin 6 = 5
	// Data pin 7 = 6
	unsigned char buf;
	// crunch the high 4 bits
	buf = (value & 0xf0 /*B11110000*/) >> 1; // isolate high 4 bits, shift over to data pins (bits 6-3: x1111xxx)
	if (mode) buf |= 3 << 1; // here we can just enable enable, since the value is immediately written to the pins
	else buf |= 2 << 1; // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
	buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
	i2clcd_burstBits(buf); // bits are now present at LCD with enable active in the same write
	// no need to delay since these things take WAY, WAY longer than the time required for enable to settle (1us in LCD implementation?)
	buf &= ~(1<<2); // toggle enable low
	i2clcd_burstBits(buf); // send out the same bits but with enable low now; LCD crunches these 4 bits.
	// crunch the low 4 bits
	buf = (value & 0x0F /*B1111*/) << 3; // isolate low 4 bits, shift over to data pins (bits 6-3: x1111xxx)
	if (mode) buf |= 3 << 1; // here we can just enable enable, since the value is immediately written to the pins
	else buf |= 2 << 1; // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
	buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
	i2clcd_burstBits(buf);
	buf &= ~( 1 << 2 ); // toggle enable low (1<<2 = 00000100; NOT = 11111011; with "and", this turns off only that one bit)
	i2clcd_burstBits(buf);
	}

void i2clcd_burstBits(uint8_t value) 
	{
	// we use this to burst bits to the GPIO chip whenever we need to. avoids repetative code.
	i2c_start_wait(MCP23008_ADDRESS+I2C_WRITE);// | _i2cAddr);
	i2c_write(MCP23008_GPIO);
	i2c_write(value); // last bits are crunched, we're done.
	i2c_stop() ;
	}
	
void i2clcd_writestring(char *string)
	{
	while (*string) i2clcd_write(*string++);
	}
	
void i2clcd_printnumber(long num,int digits,int decimals)
   // prints a int number, right justified, using digits # of digits, puting a
   // decimal decimals places from the end, and using blank
   // to fill all blank spaces
   {
   char stg[12];
   char *ptr;
   int x;

   ptr=stg+11;
	
	*ptr='\0';
	if (num<0)
		{
		num=-num;
      *(--ptr)='-';
		}
	else
      *(--ptr)=' ';
		
   for (x=1;x<=digits;++x)
      {
      if (num==0)
         *(--ptr)=' ';
      else
         {
         *(--ptr)=48+num-(num/10)*10;
         num/=10;
         }
      if (x==decimals) *(--ptr)='.';
      }
   i2clcd_writestring(ptr);
   }

