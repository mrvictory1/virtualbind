// Key to libevdev converter
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int letter_to_libevdev(int __ascii)
{
    if(__ascii == 113)
        return 16;
    else if(__ascii == 119)
        return 17;
    else if(__ascii == 101)
        return 18;
    else if(__ascii == 114)
        return 19;
    else if(__ascii == 116)
        return 20;
    else if(__ascii == 121)
        return 21;
    else if(__ascii == 117)
        return 22;
    else if(__ascii == 105)
        return 23;
    else if(__ascii == 111)
        return 24;
    else if(__ascii == 112)
        return 25;
    else if(__ascii == 97)
        return 30;
    else if(__ascii == 115)
        return 31;
    else if(__ascii == 100)
        return 32;
    else if(__ascii == 102)
        return 33;
    else if(__ascii == 103)
        return 34;
    else if(__ascii == 104)
        return 35;
    else if(__ascii == 106)
        return 36;
    else if(__ascii == 107)
        return 37;
    else if(__ascii == 108)
        return 38;
    else if(__ascii == 122)
        return 44;
    else if(__ascii == 120)
        return 45;
    else if(__ascii == 99)
        return 46;
    else if(__ascii == 118)
        return 47;
    else if(__ascii == 98)
        return 48;
    else if(__ascii == 110)
        return 49;
    else if(__ascii == 109)
        return 50;
    else
        return 0;
}

int punc_to_libevdev(int __ascii) {
    if (__ascii == 44) // comma
        return 51;
    else if(__ascii == 46) // period
        return 52;
    else if(__ascii == 47) // slash
        return 53;
    else if(__ascii == 59) // semicolon
        return 39;
    else if(__ascii == 39) // apostrophe
        return 40;
    else if(__ascii == 91) // square bracket (open)
        return 25;
    else if(__ascii == 93) // square bracket (closed)
        return 26;
    else if(__ascii == 32) // backslash
        return 27;
    else if(__ascii == 45) // minus
        return 12;
    else if(__ascii == 61) // equals
        return 13;
    else
        return 0;
}

int string_to_libevdev(char * __string) {
    printf("Converting %s\n", __string);
    if(strcmp(__string, "lshift") == 0)
        return 42;
    else if(strcmp(__string, "lclick") == 0)
        return 256; // Technically not libevdev code, 256 & 257 are added to the bottom of bindings array.
    else if(strcmp(__string, "lctrl") == 0)
        return 29;
    else if(strcmp(__string, "lalt") == 0)
        return 56;
    else if(strcmp(__string, "rshift") == 0)
        return 54;
    else if(strcmp(__string, "rclick") == 0)
        return 257;
    else if(strcmp(__string, "rctrl") == 0)
        return 97;
    else if(strcmp(__string, "ralt") == 0)
        return 100;
    else if(strcmp(__string, "space") == 0)
        return 57;
    else {
        printf("Unknown or invalid value: %s\n", __string);
        return 0;
    }
}

int key_to_libevdev (char * __value) {

    if (strlen(__value) == 1) // we have either a letter, number or a punc. mark
    {
        printf("Length 1, string %s\n", __value);
        if (isdigit(*__value))
        {
            if (atoi(__value) == 0)
                return 11; // KEY_0 has code 11
            else
                return atoi(__value) + 1; 
        }
        else if (isalpha(*__value))
            return letter_to_libevdev(*__value);
        else
            return punc_to_libevdev(*__value);
    }
    else
        return string_to_libevdev(__value);

}
