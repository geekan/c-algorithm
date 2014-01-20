#include <stdio.h>

/* unsigned int count_1bits(uint64_t x):
 *
 * Returns the number of 1-bits in 'x', between 0 and 64 inclusive. */
#if UINTPTR_MAX == UINT64_MAX
static inline unsigned int
count_1bits(uint64_t x)
{
#if __GNUC__ >= 4 && __POPCNT__
    return __builtin_popcountll(x);
#else
    /* This portable implementation is the fastest one we know of for 64
     * bits, and about 3x faster than GCC 4.7 __builtin_popcountll(). */
    const uint64_t h55 = UINT64_C(0x5555555555555555);
    const uint64_t h33 = UINT64_C(0x3333333333333333);
    const uint64_t h0F = UINT64_C(0x0F0F0F0F0F0F0F0F);
    const uint64_t h01 = UINT64_C(0x0101010101010101);
    x -= (x >> 1) & h55;               /* Count of each 2 bits in-place. */
    x = (x & h33) + ((x >> 2) & h33);  /* Count of each 4 bits in-place. */
    x = (x + (x >> 4)) & h0F;          /* Count of each 8 bits in-place. */
    return (x * h01) >> 56;            /* Sum of all bytes. */
#endif
}
#else /* Not 64-bit. */
#if __GNUC__ >= 4 && __POPCNT__
static inline unsigned int
count_1bits_32__(uint32_t x)
{
    return __builtin_popcount(x);
}
#else
#define NEED_COUNT_1BITS_8 1
extern const uint8_t count_1bits_8[256];
static inline unsigned int
count_1bits_32__(uint32_t x)
{
    /* This portable implementation is the fastest one we know of for 32 bits,
     * and faster than GCC __builtin_popcount(). */
    return (count_1bits_8[x & 0xff] +
            count_1bits_8[(x >> 8) & 0xff] +
            count_1bits_8[(x >> 16) & 0xff] +
            count_1bits_8[x >> 24]);
}
#endif
static inline unsigned int
count_1bits(uint64_t x)
{
    return count_1bits_32__(x) + count_1bits_32__(x >> 32);
}
#endif