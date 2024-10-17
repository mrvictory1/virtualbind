#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <stdbool.h>
#include "libevdev/libevdev.h"
#include "libevdev/libevdev-uinput.h"

#include "ktl.h"


struct pollfd polls[2];
struct libevdev *inputdev1;
struct libevdev *inputdev2;
struct input_event ev;
struct libevdev *touchdev;
struct libevdev_uinput *uidev;
int err;
int action;
int cameraenabled = 0;
int fd, fd1, fd2;
int avail1, avail2;
struct input_absinfo xabsinfo;
struct input_absinfo yabsinfo;
struct input_absinfo mtslot;
struct input_absinfo mtposx;
struct input_absinfo mtposy;
struct input_absinfo mttrackid;

int readerror, error;
int profile;
int xlength, ylength;
int pushx, pushy;
FILE * fp;
char * line;
char * cprofile;
char keyboard[1024];
char mouse[1024];
size_t len = 0;
int ascii;
char twobuffer[2];
char section[32];
char key[32];
char value[1024];
char filepath[1024];
char confdir[1024];
int slot = 1;
int i, sp, check1, check2, file1, file2;
// This array contains 16 columns.
// 0: Type (odd: not shortcut, even: shortcut)
// 1-4: Buttons
// 1: Shortcut destination (if [0] is even)
// 1: New profile number (if [0] is switch (9))
// 5-8: Status of buttons
// 9,10: x, y
// 11: offset (for firsttouch joystick)
// 12: slot (libevdev supports only up to 100 slots :( )
// 13-15: Unused
int bindings[16][260];
int linetowrite = 0;
char key1[10];
char key2[10];
char key3[10];
char key4[10];
int value5, value6, value7, value8;
float cameraspeed;
float camera, camerax, cameray, camerapushed;
    
void write_slot() {
    bindings[12][linetowrite] = slot;
    slot++;
}

void read_new_section()
{
    sscanf(line, "%*c%[qwertyuoipasdfghjklzxcvbnm]", section);
}

void read_new_section_profile()
{
    sscanf(line, "%*c%[qwertyuoipasdfghjklzxcvbnm]", section);
}

int open_file(char * __filepath, char * __friendlyname)
{
    fp = fopen(__filepath, "r");
    if (fp == NULL)
    {
        printf("Could not open %s file.\n"
        "File Path: %s", __friendlyname, __filepath);
        return 1;
    }
    return 0;
}

// init refers to [init] section in virtualbind.conf file, not the file itself.
int read_init(char * ___input)
{
    readerror = sscanf(line, "%[qwertyuoipasdfghjklzxcvbnm]=%s", key, value);
    if (readerror != 2)
    {
        printf("Syntax Error: Failed to parse string %s\n", line);
        return 1;
    }
    if (strcmp(key, "keyboard") == 0)
        strncpy(keyboard, value, 1024);
    else if (strcmp(key, "mouse") == 0)
        strncpy(mouse, value, 1024);
    else if (strcmp(key, "xlength") == 0)
        xlength = atoi(value);
    else if (strcmp(key, "ylength") == 0)
        ylength = atoi(value);
    else
        printf("Unknown key: %s\n", key);
    return 0;
}

// game refers to [game] section in com.game.name.conf file, not the file
// itself.
int read_game(char * ___input)
{
    readerror = sscanf(line, "%[qwertyuoipasdfghjklzxcvbnm]=%s", key, value);
    if (readerror != 2)
    {
        printf("Syntax Error: Failed to parse string %s\n", line);
        return 1;
    }
    if (strcmp(key, "default") == 0)
    {
        cprofile = value;
        profile = atoi(value);
        printf("Default profile will be %d.\n", profile);
    }
    else
        printf("Unknown key: %s", key);
    return 0;
}

// profile refers to [profile] section in com.game.name.#.conf file, not the
// file itself.
int read_profile(char * ___input)
{
    readerror = sscanf(line, "%[qwertyuoipasdfghjklzxcvbnm]=%s", key, value);
    if (readerror != 2)
    {
        printf("Syntax Error: Failed to parse string %s\n", line);
        return 1;
    }
    if (strcmp(key, "capturemouse") == 0 && strcmp(value, "1") == 0)
        libevdev_grab(inputdev2, LIBEVDEV_GRAB);
    else
        printf("Unknown key: %s\n", key);
    return 0;
}


int read_button(char * ___input)
{
    readerror = sscanf(line, "%[qwertyuoipasdfghjklzxcvbnm]=%s", key, value);
    if (readerror != 2)
    {
        printf("Syntax Error: Failed to parse string %s\n", line);
        return 1;
    }
    if (strcmp(key, "key") == 0)
    {
        linetowrite = key_to_libevdev(value);
        write_slot();
        bindings[0][linetowrite] = 1;
    }
    else if (strcmp(key, "locx") == 0)
        bindings[9][linetowrite] = atoi(value);
    else if (strcmp(key, "locy") == 0)
        bindings[10][linetowrite] = atoi(value);
    else
        printf("Unknown key: %s\n", key);
    return 0;
}

int read_switch(char * ___input)
{
    readerror = sscanf(line, "%[qwertyuoipasdfghjklzxcvbnm]=%s", key, value);
    if (readerror != 2)
    {
        printf("Syntax Error: Failed to parse string %s\n", line);
        return 1;
    }
    if (strcmp(key, "key") == 0)
    {
        linetowrite = key_to_libevdev(value);
        bindings[0][linetowrite] = 9;
    }
    else if (strcmp(key, "profile") == 0)
        bindings[1][linetowrite] = atoi(value);
    else
        printf("Unknown key: %s\n", key);
    return 0;
}

int read_camera(char * ___input)
{
    if (camera == 0)
        camera = 1;
    readerror = sscanf(line, "%[qwertyuoipasdfghjklzxcvbnm]=%s", key, value);
    if (readerror != 2)
    {
        printf("Syntax Error: Failed to parse string %s\n", line);
        return 1;
    }
    if (strcmp(key, "locx") == 0)
        camerax = atoi(value);
    else if (strcmp(key, "locy") == 0)
        cameray = atoi(value);
    else if (strcmp(key, "speed") == 0)
    {
        cameraspeed = atoi(value);
        cameraspeed = cameraspeed / 100;
    }
    else
        printf("Unknown key: %s\n", key);
    return 0;
}

int read_fjoystick(char * ___input)
{
    readerror = sscanf(line, "%[qwertyuoipasdfghjklzxcvbnm]=%s %s %s %s", key, key1, key2, key3, key4);
    if (readerror < 2)
    {
        printf("Syntax Error: Failed to parse string %s\n", line);
        return 1;
    }
    if (strcmp(key, "key") == 0)
    {
        if (readerror != 5)
        {
            printf("readerror: %d, read: %s %s %s %s\n", readerror, key1, key2, key3, key4);
            return 1;
        }
        printf("4 key: %s %s %s %s\n", key1, key2, key3, key4);
        value5 = key_to_libevdev(key1);
        value6 = key_to_libevdev(key2);
        value7 = key_to_libevdev(key3);
        value8 = key_to_libevdev(key4);
        linetowrite = value5;
        bindings[0][linetowrite] = 5;
        bindings[1][linetowrite] = value5;
        write_slot();
        bindings[2][linetowrite] = value6;
        bindings[3][linetowrite] = value7;
        bindings[4][linetowrite] = value8;
        bindings[1][value6] = value5;
        bindings[0][value6] = 6;
        bindings[1][value7] = value5;
        bindings[0][value7] = 6;
        bindings[1][value8] = value5;
        bindings[0][value8] = 6;
    }
    else if (strcmp(key, "locx") == 0)
        bindings[9][linetowrite] = atoi(key1);
    else if (strcmp(key, "locy") == 0)
        bindings[10][linetowrite] = atoi(key1);
    else if (strcmp(key, "offset") == 0)
        bindings[11][linetowrite] = atoi(key1);
    else
        printf("Unknown key: %s\n", key);

    return 0;
}
// call_main_read determines which function to call based on current section.
// On virtualbind.conf there is only [init] at the moment.
void call_main_read(char * __section, char * __input)
{
    if (strcmp(__section, "init") == 0)
        read_init(__input);
    else
        printf("Syntax Error: Empty or unknown section.\n"
        "Current section: %s", __section);
}

// call_game_read determines which function to call based on current section.
// On com.game.name.conf there is only [game] at the moment.
void call_game_read(char * __section, char * __input)
{
    if (strcmp(__section, "game") == 0)
        read_game(__input);
    else if(strcmp(__section, "somethingelse") == 0)
        printf("eee\n");
    else
        printf("Syntax Error: You must specify a section.");

}

// call_profile_read determines which function to call based on current section.
// On com.game.name.#.conf there are [profile], [button], [fjoystick] and more.
void call_profile_read(char * __section, char * __input)
{
    if (strcmp(__section, "profile") == 0)
        read_profile(__input);
    else if(strcmp(__section, "button") == 0)
        read_button(__input);
    else if(strcmp(__section, "fjoystick") == 0)
        read_fjoystick(__input);
    else if(strcmp(__section, "camera") == 0)
        read_camera(__input);
    else if(strcmp(__section, "switch") == 0)
        read_switch(__input);
    else if(__section)
        printf("Unknown section: %s\n", __section);
    else
        printf("Syntax Error: You must specify a section.\n");

}

int create_touchscreen()
{
    xabsinfo.value = 0;
    xabsinfo.minimum = 0;
    xabsinfo.maximum = 1599;
    xabsinfo.fuzz = 0;
    xabsinfo.flat = 0;
    xabsinfo.resolution = 3;

    yabsinfo.value = 0;
    yabsinfo.minimum = 0;
    yabsinfo.maximum = 899;
    yabsinfo.fuzz = 0;
    yabsinfo.flat = 0;
    yabsinfo.resolution = 3;

    // adapted from http://who-t.blogspot.com/2016/09/understanding-evdev.html

    mtslot.value = 0;
    mtslot.minimum = 0;
    mtslot.maximum = 99;
    mtslot.fuzz = 0;
    mtslot.flat = 0;
    mtslot.resolution = 0;

    mtposx.value = 0;
    mtposx.minimum = 0;
    mtposx.maximum = 1599;
    mtposx.fuzz = 0;
    mtposx.flat = 0;
    mtposx.resolution = 0;

    mtposy.value = 0;
    mtposy.minimum = 0;
    mtposy.maximum = 899;
    mtposy.fuzz = 0;
    mtposy.flat = 0;
    mtposy.resolution = 0;

    mttrackid.value = 0;
    mttrackid.minimum = 0;
    mttrackid.maximum = 260;
    mttrackid.fuzz = 0;
    mttrackid.flat = 0;
    mttrackid.resolution = 0;

    touchdev = libevdev_new();
    libevdev_set_name(touchdev, "Virtualbind Touchscreen");
    libevdev_enable_event_type(touchdev, EV_KEY);
    libevdev_enable_event_code(touchdev, EV_KEY, BTN_TOUCH, NULL);
    libevdev_enable_event_type(touchdev, EV_ABS);
    libevdev_enable_event_code(touchdev, EV_ABS, ABS_X, &xabsinfo);
    libevdev_enable_event_code(touchdev, EV_ABS, ABS_Y, &yabsinfo);
    libevdev_enable_event_code(touchdev, EV_ABS, ABS_MT_SLOT, &mtslot);
    libevdev_enable_event_code(touchdev, EV_ABS, ABS_MT_POSITION_X, &mtposx);
    libevdev_enable_event_code(touchdev, EV_ABS, ABS_MT_POSITION_Y, &mtposy);
    libevdev_enable_event_code(touchdev, EV_ABS, ABS_MT_TRACKING_ID, &mttrackid);

    error = libevdev_uinput_create_from_device(touchdev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);

    if (error) {
        printf("Failed to create uinput device.\n"
        "Please check if you have root permissions.\n");
        return 1;
    }
    else
        printf("Touchscreen created.\n");
    check1 = libevdev_new_from_fd(fd1, &inputdev1);
    if (check1 < 0)
        printf("Failed to init libevdev for keyboard\n");
    check2 = libevdev_new_from_fd(fd2, &inputdev2);
    if (check2 < 0)
        printf("Failed to init libevdev for mouse\n");
    if (check1 < 0 || check2 < 0)
    {
        printf("Failed to initialize at least 1 input device.\n");
        return 1;
    }
    else
        return 0;
}

void read_profile_file()
{
    readerror = getline(&line, &len, fp);
    while (readerror != -1)
    {
        if (*line == 91) // chacks if 1st character is "["
            read_new_section_profile();
        else
            call_profile_read(section, line);
        readerror = getline(&line, &len, fp);
    }

    int i, j;

    for (i = 0; i < 260; i++)
    {
        printf("Code %3d |", i);
        for (j = 0; j < 15; j++)
        {
            printf("%4d|", bindings[j][i]);
        }
        printf("\n");
    }
    printf("Initializing...\n");

}

void action_button(int ecode, int evalue) // Only 1 key, FIXME
{
    libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_SLOT, bindings[12][ecode]+1);
    if (evalue == 0)
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_TRACKING_ID, -1);
    else if (evalue == 1)
    {
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_TRACKING_ID, bindings[12][ecode]+1);
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_X, bindings[9][ecode]);
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_Y, bindings[10][ecode]);
    }
    libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
}

void action_fjoystick(int scut, int ecode, int evalue)
{
    if (scut == 0)
        scut = ecode;
    libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_SLOT, bindings[12][ecode]+1);
    if (evalue == 1)
    {
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_TRACKING_ID, bindings[12][ecode]+1);
        if (bindings[5][ecode] == 0 && bindings[6][ecode] == 0 && bindings[7][ecode] == 0 && bindings[8][ecode] == 0)
        {
            libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_X, bindings[9][ecode]);
            libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_Y, bindings[10][ecode]);
            libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
        }
        for (i = 5; i < 9; i++)
        {
            printf("bit%d: %d\n", i, bindings[i][ecode]);
            if (bindings[i-4][ecode] == scut)
            {
                bindings[i][ecode] = 1;
                printf("Bit switch! ecode: %d, i: %d\n", ecode, i);
            }
        }
        pushx = bindings[9][ecode] - (bindings[6][ecode] * bindings[11][ecode]) + (bindings[8][ecode] * bindings[11][ecode]);
        pushy = bindings[10][ecode] - (bindings[5][ecode] * bindings[11][ecode]) + (bindings[7][ecode] * bindings[11][ecode]);
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_X, pushx);
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_Y, pushy);
        libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
    }
    else if (evalue == 0)
    {
        for (i = 5; i < 9; i++)
        {
            printf("bit%d: %d\n", i, bindings[i][ecode]);
            if (bindings[i-4][ecode] == scut)
            {
                bindings[i][ecode] = 0;
                printf("Bit switch! ecode: %d\n", ecode);
            }
        }
        if (bindings[5][ecode] == 0 && bindings[6][ecode] == 0 && bindings[7][ecode] == 0 && bindings[8][ecode] == 0)
        {
            libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_TRACKING_ID, -1);
            libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
        }
        else {
            pushx = bindings[9][ecode] - (bindings[6][ecode] * bindings[11][ecode]) + (bindings[8][ecode] * bindings[11][ecode]);
            pushy = bindings[10][ecode] - (bindings[5][ecode] * bindings[11][ecode]) + (bindings[7][ecode] * bindings[11][ecode]);
            libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_X, pushx);
            libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_Y, pushy);
        }
    }
    libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
}

void action_camera(int ecode, int evalue)
{
    libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_SLOT, 99);
    if (camerapushed == 0)
    {
        camerapushed = 1;
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_TRACKING_ID, 99);
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_X, camerax);
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_Y, cameray);
        libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
    }
    if (ecode == 0)
    {
        camerax = camerax + evalue*cameraspeed;
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_X, camerax);
    }
    else if(ecode == 1)
    {
        cameray = cameray + evalue*cameraspeed;
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_POSITION_Y, cameray);
    }
    libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
}

void read_keyboard_event(struct input_event *ev) {
    if(ev->type == 1) // EV_KEY
    {
        if (bindings[0][ev->code] == 0)
            printf("Dropping Input\n");
        else
        {
            action = bindings[0][ev->code];
            printf("action: %d, code: %d\n", action, ev->code);
            if (action == 1)
                action_button(ev->code, ev->value);
            else if (action == 5)
                action_fjoystick(0, ev->code, ev->value);
            else if (action == 6)
                action_fjoystick(ev->code ,bindings[1][ev->code], ev->value);
            else if(action == 9)
                sp = bindings[1][ev->code];
            else
                printf("action: %d, code: %d\n", action, ev->code);
        }
    }
}

void read_mouse_event(struct input_event *ev) {
    if(ev->type == 1) // EV_KEY
    {
        if (bindings[0][ev->code - 16] == 0)
            printf("Dropping Input\n");
        else
        {
            action = bindings[0][ev->code - 16];
            printf("action: %d, code: %d\n", action, ev->code - 16);
            if (action == 1)
                action_button(ev->code - 16, ev->value);
            else if(action == 9)
                sp = bindings[1][ev->code - 16];
            else
                printf("action: %d, code: %d\n", action, ev->code);
        }
    }
    else if(ev->type == 2 && camera == 1)
        action_camera(ev->code, ev->value);
}

void perform_keybinding() {
    while (sp == 0)
    {
        avail1 = libevdev_has_event_pending(inputdev1);
        if (avail1)
        {
            libevdev_next_event(inputdev1, LIBEVDEV_READ_FLAG_NORMAL|LIBEVDEV_READ_FLAG_BLOCKING, &ev);
            read_keyboard_event(&ev);
        }
        avail2 = libevdev_has_event_pending(inputdev2);
        if (avail2)
        {
            libevdev_next_event(inputdev2, LIBEVDEV_READ_FLAG_NORMAL|LIBEVDEV_READ_FLAG_BLOCKING, &ev);
            read_mouse_event(&ev);

        }
        if (avail1 == 0 && avail2 == 0)
            poll(polls, 2, -1);
    }
}

int switch_profile()
{
    for (i = 0; i < slot + 1; i++)
    {
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_SLOT, i);
        libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_TRACKING_ID, -1);
        libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
    }
    libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_SLOT, 99);
    libevdev_uinput_write_event(uidev, EV_ABS, ABS_MT_TRACKING_ID, -1);
    libevdev_uinput_write_event(uidev, EV_SYN, 0, 0);
    slot = 1;
    libevdev_grab(inputdev2, LIBEVDEV_UNGRAB);

    sprintf(cprofile, "%d", sp);
    printf("Profile switching to: %s", cprofile);
    int i, j;
    for (i = 0; i < 260; i++)
        for (j = 0; j < 15; j++)
            bindings[j][i] = 0;
    sp = 0;
    camerapushed = 0;
    camera = 0;
    camerax = 0;
    cameray = 0;
    cameraspeed = 0;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Welcome to virtualbind! The program takes a single argument:\n"
        "the name of the configuration file set you want to load.\n");
        return 0;
    }

    // Read main configuration file ie. virtualbind.conf
    strcpy(confdir, "/etc/virtualbind/");
    open_file("/etc/virtualbind/virtualbind.conf", "main configuration");

    readerror = getline(&line, &len, fp);
    while (readerror != -1)
    {
        if (*line == 91) // chacks if 1st character is "["
            read_new_section();
        else
            call_main_read(section, line);
        readerror = getline(&line, &len, fp);
    }
    fd1 = open(keyboard, O_RDONLY);
    if (fd1 < 0)
        printf("Failed to open device 1.\n");
    fd2 = open(mouse, O_RDONLY);
    if (fd2 < 0)
        printf("Failed to open device 2.\n");
    polls[0].fd = fd1;
    polls[1].fd = fd2;
    polls[0].events = POLLIN;
    polls[1].events = POLLIN;

    strcpy(filepath, confdir);
    strcat(filepath, argv[1]);
    strcat(filepath, ".conf");

    open_file(filepath, "game configuration");

    readerror = getline(&line, &len, fp);
    while (readerror != -1)
    {
        if (*line == 91) // chacks if 1st character is "["
            read_new_section();
        else
            call_game_read(section, line);
        readerror = getline(&line, &len, fp);
    }

    error = create_touchscreen();
    if (error)
        return 1;
    while (true) {
        strcpy(filepath, confdir);
        strcat(filepath, argv[1]);
        strcat(filepath, ".");
        strcat(filepath, cprofile);
        strcat(filepath, ".conf");
        open_file(filepath, "game profile");
        read_profile_file();
        perform_keybinding();
        switch_profile();
        libevdev_grab(inputdev2, LIBEVDEV_UNGRAB);
    }
    return 0;
}
