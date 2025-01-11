#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Original functions (copied from your original code)

// Map ASCII letters to libevdev key codes
int original_letter_to_libevdev(int ascii) {
    switch (ascii) {
        case 'q': return 16;
        case 'w': return 17;
        case 'e': return 18;
        case 'r': return 19;
        case 't': return 20;
        case 'y': return 21;
        case 'u': return 22;
        case 'i': return 23;
        case 'o': return 24;
        case 'p': return 25;
        case 'a': return 30;
        case 's': return 31;
        case 'd': return 32;
        case 'f': return 33;
        case 'g': return 34;
        case 'h': return 35;
        case 'j': return 36;
        case 'k': return 37;
        case 'l': return 38;
        case 'z': return 44;
        case 'x': return 45;
        case 'c': return 46;
        case 'v': return 47;
        case 'b': return 48;
        case 'n': return 49;
        case 'm': return 50;
        default: return 0;
    }
}

int original_key_to_libevdev(const char* value) {
    if (strlen(value) == 1) {
        if (isdigit(*value)) {
            return (*value == '0') ? 11 : atoi(value) + 1;
        }
        if (isalpha(*value)) {
            return original_letter_to_libevdev(*value);
        }
        return 0;
    }
    return 0; // Simplified for brevity
}

// Optimized functions (copied from optimized code)
static const int ascii_to_libevdev[128] = {
    ['q'] = 16, ['w'] = 17, ['e'] = 18, ['r'] = 19, ['t'] = 20,
    ['y'] = 21, ['u'] = 22, ['i'] = 23, ['o'] = 24, ['p'] = 25,
    ['a'] = 30, ['s'] = 31, ['d'] = 32, ['f'] = 33, ['g'] = 34,
    ['h'] = 35, ['j'] = 36, ['k'] = 37, ['l'] = 38, ['z'] = 44,
    ['x'] = 45, ['c'] = 46, ['v'] = 47, ['b'] = 48, ['n'] = 49,
    ['m'] = 50,
};

int optimized_key_to_libevdev(const char* value) {
    if (strlen(value) == 1) {
        char c = *value;
        if (isdigit(c)) {
            return (c == '0') ? 11 : (c - '0') + 1;
        }
        if (isalpha(c)) {
            return ascii_to_libevdev[(unsigned char)c];
        }
        return 0;
    }
    return 0; // Simplified for brevity
}

// Benchmarking function
void benchmark(const char* name, int (*func)(const char*), const char* test_values[], int test_count, int iterations) {
    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < test_count; j++) {
            func(test_values[j]);
        }
    }
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("%s: %.6f seconds\n", name, elapsed);
}

int main() {
    // Test cases
    const char* test_values[] = {
        "a", "b", "c", "z", "q", "1", "9", "0", "m", "f"
    };
    int test_count = sizeof(test_values) / sizeof(test_values[0]);
    int iterations = 1000000;

    printf("Benchmarking original and optimized functions...\n");

    benchmark("Original key_to_libevdev", original_key_to_libevdev, test_values, test_count, iterations);
    benchmark("Optimized key_to_libevdev", optimized_key_to_libevdev, test_values, test_count, iterations);

    return 0;
}
