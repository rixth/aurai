#define clockCyclesPerMicrosecond()  (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC  ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX  (1000 >> 3)


void Time_init();
unsigned long Time_micros();