#include <inttypes.h>
#include "../include/ker/string.hpp"

#define SPEC(type) template<> String String::from<type>(const type &val)

static void reverse(char *str, int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char tmp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = tmp;
        start++;
        end--;
    }
}

static char *_itoa(int num, char *str, int base, bool useUnsigned)
{
    int i = 0;
    int isNegative = 0;

    if(str == nullptr) {
        static char tmp[64];
        str = tmp;
    }

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if ((useUnsigned == false) && (num < 0) && (base == 10))
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'A' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
    {
        str[i++] = '-';
    }
    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

namespace ker
{
    SPEC(bool)
    {
        return val ? "true" : "false";
    }

    SPEC(String)
    {
        return val;
    }

    SPEC(int8_t) {
        static char buffer[16];
        _itoa(val, buffer, 10, false);
        return buffer;
    }

    SPEC(int16_t) {
        static char buffer[16];
        _itoa(val, buffer, 10, false);
        return buffer;
    }

    SPEC(int32_t) {
        static char buffer[16];
        _itoa(val, buffer, 10, false);
        return buffer;
    }

    SPEC(uint8_t) {
        static char buffer[16];
        _itoa(val, buffer, 10, true);
        return buffer;
    }

    SPEC(uint16_t) {
        static char buffer[16];
        _itoa(val, buffer, 10, true);
        return buffer;
    }

    SPEC(uint32_t) {
        static char buffer[16];
        _itoa(val, buffer, 10, true);
        return buffer;
    }

    SPEC(float) {
        (void)val;
        return "f32";
    }

#if defined(CONDUCTANCE_64BIT)

    SPEC(int64_t)
    {
        return "i64";
    }

    SPEC(uint64_t)
    {
        return "u64";
    }

    SPEC(double)
    {
        return "f64";
    }

#endif
}
