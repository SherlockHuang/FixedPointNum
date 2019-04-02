
#include <iostream>
#include <cstdio>
#include <string>

#define FLOAT_SIZE 7
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

union FloatAndInt {
    float f;
    int32_t i;
    char c[4];
};

void printBinary(int32_t v);
void printBinary(float v);

class FixedNum {
    private:
        int32_t value;
    public:
        FixedNum() : value(0) {};
        FixedNum(int32_t in_value, bool raw = false) : value(raw ? in_value : in_value << FLOAT_SIZE) {};
        FixedNum(const FixedNum &rhs) : value(rhs.value) {};

        void operator=(FixedNum const &rhs);
        FixedNum operator+(const FixedNum &rhs) const;
        FixedNum operator-(const FixedNum &rhs) const;
        FixedNum operator*(const FixedNum &rhs) const;
        FixedNum operator/(const FixedNum &rhs) const;
        FixedNum& operator+=(const FixedNum &rhs);
        FixedNum& operator-=(const FixedNum &rhs);
        FixedNum& operator*=(const FixedNum &rhs);
        FixedNum& operator/=(const FixedNum &rhs);

        float toFloat() const;
        int32_t toInt() const;
        int32_t getValue() const { return value; }

        static int GetExponent(int32_t v);
        static int32_t GetMantissa(int32_t v);
};

FixedNum FixedNum::operator+(const FixedNum &rhs) const {
    int32_t result = value + rhs.value;
    return FixedNum(result, true);
}

FixedNum FixedNum::operator-(const FixedNum &rhs) const {
    int32_t result = value - rhs.value;
    return FixedNum(result, true);
}

FixedNum FixedNum::operator*(const FixedNum &rhs) const {
    int32_t result = (value * rhs.value) >> FLOAT_SIZE;
    return FixedNum(result, true);
}

FixedNum FixedNum::operator/(const FixedNum &rhs) const {
    int32_t result = (value << FLOAT_SIZE) / rhs.value;
    return FixedNum(result, true);
}

void FixedNum::operator=(FixedNum const &rhs) {
    value = rhs.value;
}

FixedNum& FixedNum::operator-=(const FixedNum &rhs) {
    value -= rhs.value;
    return *this;
}

FixedNum& FixedNum::operator+=(const FixedNum &rhs) {
    value += rhs.value;
    return *this;
}

FixedNum& FixedNum::operator/=(const FixedNum &rhs) {
    value = (value << FLOAT_SIZE) / rhs.value;
    return *this;
}

FixedNum& FixedNum::operator*=(const FixedNum &rhs) {
    value = (value * rhs.value) >> FLOAT_SIZE;
    return *this;
}

int FixedNum::GetExponent(int32_t v) {
    // exponent 
    int expn = 127 - FLOAT_SIZE;
    for (int i = 30; i >= 0; i++) {
        if ((1 << i & v) == 1) {
            return expn += i;
        }
    }

    return 255;
}

int32_t FixedNum::GetMantissa(int32_t v) {
    return 0;
}

float FixedNum::toFloat() const {
    if ((value & 0x7fffffff) == 0) { return 0; }

    int32_t n = 0;

    // exponent
    int32_t expn = 127 - FLOAT_SIZE;
    for (int32_t i = 30; i > 0; i--) {
        if (((1 << i) & value) > 0) {
            n = i;
            break;
        }
    }
    expn += n;

    // mantissa
    int32_t mant = 0;
    if (n > 23) {
        mant = (value & 0x7fffffff) >> (n - 23);
    } else {
        mant = (value & 0x7fffffff) << (23 - n);
    }
    mant &= 0xff7fffff;

    /* std::cout << "expn = " << expn << ", n = " << n << std::endl; */
    /* printf("--- mant --- \n"); */
    /* printBinary(mant); */
    /* printf("--- sign --- \n"); */
    /* printBinary((int32_t)(value & 0x80000000)); */

    FloatAndInt fi;
    fi.i = ((expn << 23) + mant) | (value & 0x80000000);

    return fi.f;
}

int32_t FixedNum::toInt() const {
    return value >> FLOAT_SIZE;
}

void printBinary(int32_t v) {
    FloatAndInt fi;
    fi.i = v;

    printf("v = %d, binary = ", v);
    printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(fi.c[0]));
    printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(fi.c[1]));
    printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(fi.c[2]));
    printf(BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(fi.c[3]));
}

void printBinary(float v) {
    FloatAndInt fi;
    fi.i = v;

    printf("v = %f, binary = ", v);
    printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(fi.c[0]));
    printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(fi.c[1]));
    printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(fi.c[2]));
    printf(BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(fi.c[3]));
}

int main() {
    /* std::cout << "sizeof float = " << sizeof(float) << std::endl; */

    /* int32_t t = 1 + (2 << 8) + (3 << 16) + (4 << 24); */
    /* printBinary(t); */

    FixedNum fn(1);

    printBinary(fn.getValue());
    printBinary(fn.toFloat());
    
    fn += FixedNum(3) / FixedNum(9);

    printBinary(fn.getValue());
    printBinary(fn.toFloat());

    fn *= FixedNum(3) / FixedNum(9);
    printBinary(fn.getValue());
    printBinary(fn.toFloat());

    fn -= FixedNum(3) / FixedNum(9);
    printBinary(fn.getValue());
    printBinary(fn.toFloat());

    return 0;
}

