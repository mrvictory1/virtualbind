#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Precomputed array for ASCII to libevdev key mapping (letters and punctuation)
// Using a static array ensures fast lookup and prevents unnecessary branching.
static const int ascii_to_libevdev[128] = {
    ['q'] = 16, ['w'] = 17, ['e'] = 18, ['r'] = 19, ['t'] = 20,
    ['y'] = 21, ['u'] = 22, ['i'] = 23, ['o'] = 24, ['p'] = 25,
    ['a'] = 30, ['s'] = 31, ['d'] = 32, ['f'] = 33, ['g'] = 34,
    ['h'] = 35, ['j'] = 36, ['k'] = 37, ['l'] = 38, ['z'] = 44,
    ['x'] = 45, ['c'] = 46, ['v'] = 47, ['b'] = 48, ['n'] = 49,
    ['m'] = 50,
    [','] = 51, ['.'] = 52, ['/'] = 53, [';'] = 39, ['\''] = 40,
    ['['] = 25, [']'] = 26, ['\\'] = 27, ['-'] = 12, ['='] = 13,
    ['`'] = 41,

};

// Hash table for string-to-libevdev key mapping
typedef struct {
    const char *key; // The string representation
    int value;       // Corresponding libevdev code
} KeyValue;

// Predefined mappings for special strings
// Using a binary-searchable array (sorted by key for efficiency).
static const KeyValue string_to_libevdev_map[] = {
    {"backspace", 14}, {"down", 108}, {"enter", 28}, {"esc", 1},
    {"f1", 59}, {"f10", 68}, {"f11", 87}, {"f12", 88}, {"f2", 60},
    {"f3", 61}, {"f4", 62}, {"f5", 63}, {"f6", 64}, {"f7", 65},
    {"f8", 66}, {"f9", 67}, {"lalt", 56}, {"lclick", 256},
    {"lctrl", 29}, {"left", 105}, {"lshift", 42}, {"mclick", 258},
    {"ralt", 100}, {"rclick", 257}, {"rctrl", 97}, {"right", 106},
    {"rshift", 54}, {"space", 57}, {"tab", 15}, {"up", 103},
};


#define STRING_MAP_SIZE (sizeof(string_to_libevdev_map) / sizeof(string_to_libevdev_map[0]))

// Find libevdev code from string map using binary search
int string_to_libevdev(const char *string) {
    if (!string || *string == '\0') { // Validate input
        fprintf(stderr, "Error: Null or empty string passed to string_to_libevdev\n");
        return 0; // Return 0 for invalid input
    }

    int low = 0, high = STRING_MAP_SIZE - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcmp(string, string_to_libevdev_map[mid].key);
        if (cmp == 0) {
            return string_to_libevdev_map[mid].value;
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    fprintf(stderr, "Warning: Unknown or invalid value '%s'\n", string);
    return 0; // Return 0 for unrecognized strings
}

// Determine the libevdev key code for a given value
int key_to_libevdev(const char *value) {
    if (!value || *value == '\0') { // Validate input
        fprintf(stderr, "Error: Null or empty string passed to key_to_libevdev\n");
        return 0; 
    }

    if (strlen(value) == 1) { // Single character case
        unsigned char c = (unsigned char)*value; // Cast to handle extended ASCII safely
        if (isdigit(c)) { // Check if it's a digit
            return (c == '0') ? 11 : (c - '0') + 1;
        }
        if (isalpha(c) || ispunct(c)) { // Check for alphabetic or punctuation
            return ascii_to_libevdev[c]; // Fast lookup
        }
        return 0; // Unrecognized character
    }

    // Handle string case
    return string_to_libevdev(value);
}
