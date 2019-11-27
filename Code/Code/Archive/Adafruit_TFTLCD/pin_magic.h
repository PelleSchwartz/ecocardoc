#ifndef _pin_magic_
#define _pin_magic_

// This header file serves two purposes:
//
// 1) Isolate non-portable MCU port- and pin-specific identifiers and
//    operations so the library code itself remains somewhat agnostic
//    (PORTs and pin numbers are always referenced through macros).
//
// 2) GCC doesn't always respect the "inline" keyword, so this is a
//    ham-fisted manner of forcing the issue to minimize function calls.
//    This sometimes makes the library a bit bigger than before, but fast++.
//    However, because they're macros, we need to be SUPER CAREFUL about
//    parameters -- for example, write8(x) may expand to multiple PORT
//    writes that all refer to x, so it needs to be a constant or fixed
//    variable and not something like *ptr++ (which, after macro
//    expansion, may increment the pointer repeatedly and run off into
//    la-la land).  Macros also give us fine-grained control over which
//    operations are inlined on which boards (balancing speed against
//    available program space).

// Note that using digitalWrite is required (as opposed to digitalWriteFast)
// when running at 180 MHz to add sufficient delay for the display to be able to keep up.

// Pixel read operations require a minimum 400 nS delay from RD_ACTIVE
// to polling the input pins.  The last line is a radioactive mutant emoticon.
// This actually does not seem to matter that much in my tests...
#define DELAY7        \
asm volatile(       \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop" "\n\t" \
"nop"      "\n"   \
::);

// Teensy 3.6 //
#if defined(__MK66FX1M0__)
// You can freely change the definitions below to any pin you desire! //
#define LCD_D0 13
#define LCD_D1 39
#define LCD_D2 19
#define LCD_D3 18
#define LCD_D4 17
#define LCD_D5 16
#define LCD_D6 15
#define LCD_D7 14

#define LCD_RESET 	8  // Reset
#define LCD_CS 		9  // Chip Select
#define LCD_RS 		10 // LCD RS
#define LCD_WR 		11 // LCD Write
#define LCD_RD 		12 // LCD Read

	#define write8inline(d) {					\
		digitalWriteFast(LCD_D0, d&B00000001);		\
		digitalWriteFast(LCD_D1, d&B00000010);		\
		digitalWriteFast(LCD_D2, d&B00000100);		\
		digitalWriteFast(LCD_D3, d&B00001000);		\
		digitalWriteFast(LCD_D4, d&B00010000);		\
		digitalWriteFast(LCD_D5, d&B00100000);		\
		digitalWriteFast(LCD_D6, d&B01000000);		\
		digitalWriteFast(LCD_D7, d&B10000000);		\
		WR_STROBE;	}
	#define read8inline(result) {			\
		RD_ACTIVE; 							\
									\
		result = digitalReadFast(LCD_D0);		\
		result |= digitalReadFast(LCD_D1) << 1;	\
		result |= digitalReadFast(LCD_D2) << 2;	\
		result |= digitalReadFast(LCD_D3) << 3;	\
		result |= digitalReadFast(LCD_D4) << 4;	\
		result |= digitalReadFast(LCD_D5) << 5;	\
		result |= digitalReadFast(LCD_D6) << 6;	\
		result |= digitalReadFast(LCD_D7) << 7;	\
		RD_IDLE;							\
		}

	#define setWriteDirInline() { \
		pinMode(LCD_D0, OUTPUT); \
		pinMode(LCD_D1, OUTPUT); \
		pinMode(LCD_D2, OUTPUT); \
		pinMode(LCD_D3, OUTPUT); \
		pinMode(LCD_D4, OUTPUT); \
		pinMode(LCD_D5, OUTPUT); \
		pinMode(LCD_D6, OUTPUT); \
		pinMode(LCD_D7, OUTPUT); }
	#define setReadDirInline() { \
		pinMode(LCD_D0, INPUT); \
		pinMode(LCD_D1, INPUT); \
		pinMode(LCD_D2, INPUT); \
		pinMode(LCD_D3, INPUT); \
		pinMode(LCD_D4, INPUT); \
		pinMode(LCD_D5, INPUT); \
		pinMode(LCD_D6, INPUT); \
		pinMode(LCD_D7, INPUT); }

  // All of the functions are inlined on the Arduino Mega.  When using the
  // breakout board, the macro versions aren't appreciably larger than the
  // function equivalents, and they're super simple and fast.  When using
  // the shield, the macros become pretty complicated...but this board has
  // so much code space, the macros are used anyway.  If you need to free
  // up program space, some macros can be removed, at a minor cost in speed.
  #define write8            write8inline
  #define read8             read8inline
  #define setWriteDir       setWriteDirInline
  #define setReadDir        setReadDirInline
  #define writeRegister8    writeRegister8inline
  #define writeRegister16   writeRegister16inline
  #define writeRegisterPair writeRegisterPairInline
  
 // Control signals are ACTIVE LOW (idle is HIGH)
 // Command/Data: LOW = command, HIGH = data
	#define RD_ACTIVE  digitalWriteFast(LCD_RD, LOW);
	#define RD_IDLE    digitalWriteFast(LCD_RD, HIGH);
	#define WR_ACTIVE  digitalWriteFast(LCD_WR, LOW);
	#define WR_IDLE    digitalWrite(LCD_WR, HIGH);
	#define CD_COMMAND digitalWriteFast(LCD_RS, LOW);
	#define CD_DATA    digitalWriteFast(LCD_RS, HIGH);
	#define CS_ACTIVE  digitalWriteFast(LCD_CS, LOW);
	#define CS_IDLE    digitalWriteFast(LCD_CS, HIGH);

#else

 #error "Board type unsupported / not recognized"

#endif

// Data write strobe, ~2 instructions and always inline
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

// These higher-level operations are usually functionalized,
// except on Mega where's there's gobs and gobs of program space.

// Set value of TFT register: 8-bit address, 8-bit value
#define writeRegister8inline(a, d) { \
  CD_COMMAND; write8(a); CD_DATA; write8(d); }

// Set value of TFT register: 16-bit address, 16-bit value
// See notes at top about macro expansion, hence hi & lo temp vars
#define writeRegister16inline(a, d) { \
  uint8_t hi, lo; \
  hi = (a) >> 8; lo = (a); CD_COMMAND; write8(hi); write8(lo); \
  hi = (d) >> 8; lo = (d); CD_DATA   ; write8(hi); write8(lo); }

// Set value of 2 TFT registers: Two 8-bit addresses (hi & lo), 16-bit value
#define writeRegisterPairInline(aH, aL, d) { \
  uint8_t hi = (d) >> 8, lo = (d); \
  CD_COMMAND; write8(aH); CD_DATA; write8(hi); \
  CD_COMMAND; write8(aL); CD_DATA; write8(lo); }

#endif // _pin_magic_