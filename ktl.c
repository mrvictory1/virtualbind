// Key to libevdev converter
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Map ASCII letters to libevdev key codes
int letter_to_libevdev(int ascii) {
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

// Map ASCII punctuation marks to libevdev key codes
int punc_to_libevdev(int ascii) {
    switch (ascii) {
        case ',': return 51;
        case '.': return 52;
        case '/': return 53;
        case ';': return 39;
        case '\'': return 40;
        case '[': return 25;
        case ']': return 26;
        case '\\': return 27;
        case '-': return 12;
        case '=': return 13;
        case '`': return 41;
        default: return 0;
    }
}

// Map string names to libevdev key codes
int string_to_libevdev(char* string) {
    printf("Converting %s\n", string);
    if (strcmp(string, "lshift") == 0) return 42;
    if (strcmp(string, "lclick") == 0) return 256; // Custom
    if (strcmp(string, "lctrl") == 0) return 29;
    if (strcmp(string, "lalt") == 0) return 56;
    if (strcmp(string, "rshift") == 0) return 54;
    if (strcmp(string, "rclick") == 0) return 257; // Custom
    if (strcmp(string, "rctrl") == 0) return 97;
    if (strcmp(string, "ralt") == 0) return 100;
    if (strcmp(string, "mclick") == 0) return 258; // Custom
    if (strcmp(string, "space") == 0) return 57;
    if (strcmp(string, "esc") == 0) return 1;
    if (strcmp(string, "f1") == 0) return 59;
    if (strcmp(string, "f2") == 0) return 60;
    if (strcmp(string, "f3") == 0) return 61;
    if (strcmp(string, "f4") == 0) return 62;
    if (strcmp(string, "f5") == 0) return 63;
    if (strcmp(string, "f6") == 0) return 64;
    if (strcmp(string, "f7") == 0) return 65;
    if (strcmp(string, "f8") == 0) return 66;
    if (strcmp(string, "f9") == 0) return 67;
    if (strcmp(string, "f10") == 0) return 68;
    if (strcmp(string, "f11") == 0) return 87;
    if (strcmp(string, "f12") == 0) return 88;
    if (strcmp(string, "tab") == 0) return 15;
    if (strcmp(string, "backspace") == 0) return 14;
    if (strcmp(string, "enter") == 0) return 28;
    if (strcmp(string, "up") == 0) return 103;
    if (strcmp(string, "down") == 0) return 108;
    if (strcmp(string, "left") == 0) return 105;
    if (strcmp(string, "right") == 0) return 106;

    printf("Unknown or invalid value: %s\n", string);
    return 0;
}

// Determine the libevdev key code for a given value
int key_to_libevdev(char* value) {
    if (strlen(value) == 1) {
        printf("Length 1, string %s\n", value);
        if (isdigit(*value)) {
            return (*value == '0') ? 11 : atoi(value) + 1;
        }
        if (isalpha(*value)) {
            return letter_to_libevdev(*value);
        }
        return punc_to_libevdev(*value);
    }
    return string_to_libevdev(value);
}
