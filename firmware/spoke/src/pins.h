// Defines for ATmega48A/PA/88A/PA/168A/PA/328/P.
// Autogenerated by pin-defines v0.0.1
// Updated: Sat Aug 09 2014 18:39:19 GMT-0400 (EDT)

#include <avr/io.h>

#define EEPROM_CS_DDR        DDRD
#define EEPROM_CS_PORT       PORTD
#define EEPROM_CS_PIN        PIND
#define EEPROM_CS_MASK       (1 << 3)

#define EEPROM_CS_ToOutput() EEPROM_CS_DDR |= EEPROM_CS_MASK
#define EEPROM_CS_ToInput()  EEPROM_CS_DDR &= ~EEPROM_CS_MASK

#define EEPROM_CS_High()     EEPROM_CS_PORT |= EEPROM_CS_MASK
#define EEPROM_CS_Low()      EEPROM_CS_PORT &= ~EEPROM_CS_MASK

#define EEPROM_CS_Read()     EEPROM_CS_PIN & EEPROM_CS_MASK
#define EEPROM_CS_IsLow()    !EEPROM_CS_Read()
#define EEPROM_CS_IsHigh()   EEPROM_CS_Read()

#define EEPROM_CS_LoopUntilHigh() do { } while (EEPROM_CS_IsLow())
#define EEPROM_CS_LoopUntilLow()  do { } while (EEPROM_CS_IsHigh())

#define RED_LED_DDR        DDRD
#define RED_LED_PORT       PORTD
#define RED_LED_PIN        PIND
#define RED_LED_MASK       (1 << 5)

#define RED_LED_ToOutput() RED_LED_DDR |= RED_LED_MASK
#define RED_LED_ToInput()  RED_LED_DDR &= ~RED_LED_MASK

#define RED_LED_High()     RED_LED_PORT |= RED_LED_MASK
#define RED_LED_Low()      RED_LED_PORT &= ~RED_LED_MASK

#define RED_LED_Read()     RED_LED_PIN & RED_LED_MASK
#define RED_LED_IsLow()    !RED_LED_Read()
#define RED_LED_IsHigh()   RED_LED_Read()

#define RED_LED_LoopUntilHigh() do { } while (RED_LED_IsLow())
#define RED_LED_LoopUntilLow()  do { } while (RED_LED_IsHigh())

#define YLW_LED_DDR        DDRD
#define YLW_LED_PORT       PORTD
#define YLW_LED_PIN        PIND
#define YLW_LED_MASK       (1 << 6)

#define YLW_LED_ToOutput() YLW_LED_DDR |= YLW_LED_MASK
#define YLW_LED_ToInput()  YLW_LED_DDR &= ~YLW_LED_MASK

#define YLW_LED_High()     YLW_LED_PORT |= YLW_LED_MASK
#define YLW_LED_Low()      YLW_LED_PORT &= ~YLW_LED_MASK

#define YLW_LED_Read()     YLW_LED_PIN & YLW_LED_MASK
#define YLW_LED_IsLow()    !YLW_LED_Read()
#define YLW_LED_IsHigh()   YLW_LED_Read()

#define YLW_LED_LoopUntilHigh() do { } while (YLW_LED_IsLow())
#define YLW_LED_LoopUntilLow()  do { } while (YLW_LED_IsHigh())

#define GRN_LED_DDR        DDRD
#define GRN_LED_PORT       PORTD
#define GRN_LED_PIN        PIND
#define GRN_LED_MASK       (1 << 7)

#define GRN_LED_ToOutput() GRN_LED_DDR |= GRN_LED_MASK
#define GRN_LED_ToInput()  GRN_LED_DDR &= ~GRN_LED_MASK

#define GRN_LED_High()     GRN_LED_PORT |= GRN_LED_MASK
#define GRN_LED_Low()      GRN_LED_PORT &= ~GRN_LED_MASK

#define GRN_LED_Read()     GRN_LED_PIN & GRN_LED_MASK
#define GRN_LED_IsLow()    !GRN_LED_Read()
#define GRN_LED_IsHigh()   GRN_LED_Read()

#define GRN_LED_LoopUntilHigh() do { } while (GRN_LED_IsLow())
#define GRN_LED_LoopUntilLow()  do { } while (GRN_LED_IsHigh())

