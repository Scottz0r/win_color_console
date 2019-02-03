/******************************************************************************
See win_color_console.hh for license details.

This file is for testing and should not be included in other projects.
*******************************************************************************/
#include <string>
#include "win_color_console.hh"

using wincc::ConsoleColor;

// Static variables for easily testing narrow and wide characters.
static bool test_wide = false;
static wchar_t wbuffer[256];

// Quick and dirty conversion from narrow to wide. For testing purposes. Only works with ASCII.
static void ghetto_widen(const char *val, wchar_t *dst, size_t size);

// Write methods that can switched between wchar_t and char types.
static void write_test(wincc::Console &console, const char *val);

// Write methods that can switched between wchar_t and char types with foreground color.
static void write_test(wincc::Console &console, ConsoleColor foreground, const char *val);

// Write methods that can switched between wchar_t and char types with foreground and background color.
static void write_test(wincc::Console &console, ConsoleColor foreground, ConsoleColor background, const char *val);


int main(int argc, const char **argv)
{
    static char text_buffer[1024];
    wincc::Console console;

    if (argc >= 2 && (std::string)argv[1] == "--wide")
    {
        test_wide = true;
        console.writewc(L"Test with wide characters...\n---\n");
    }
    else
    {
        console.write("Testing with narrow characters...\n---\n");
    }

    // Test grabbing default values.
    int fg, bg;
    fg = (int)console.foreground_color();
    bg = (int)console.background_color();
    sprintf(text_buffer, "Default Foreground is %d, Default background is %d\n", fg, bg);
    write_test(console, text_buffer);

    // Using functions that set once, foreground.
    write_test(console, ConsoleColor::Blue, "Blue\n");
    write_test(console, ConsoleColor::Green, "Green\n");
    write_test(console, ConsoleColor::Red, "Red\n");

    write_test(console, ConsoleColor::DarkBlue, "DarkBlue\n");
    write_test(console, ConsoleColor::DarkGreen, "DarkGreen\n");
    write_test(console, ConsoleColor::DarkRed, "DarkRed\n");

    write_test(console, ConsoleColor::Cyan, "Cyan\n");
    write_test(console, ConsoleColor::Violet, "Violet\n");
    write_test(console, ConsoleColor::Yellow, "Yellow\n");

    write_test(console, ConsoleColor::DarkCyan, "DarkCyan\n");
    write_test(console, ConsoleColor::DarkViolet, "DarkViolet\n");
    write_test(console, ConsoleColor::DarkYellow, "DarkYellow\n");

    write_test(console, ConsoleColor::White, "White\n");
    write_test(console, ConsoleColor::Gray, "Gray\n");
    write_test(console, ConsoleColor::Black, "Black\n");

    write_test(console, ConsoleColor::Default, "Default Console Foreground\n");

    // Using functions that set once, background.
    write_test(console, ConsoleColor::White, ConsoleColor::Blue, "Blue\n");
    write_test(console, ConsoleColor::White, ConsoleColor::Green, "Green\n");
    write_test(console, ConsoleColor::White, ConsoleColor::Red, "Red\n");

    write_test(console, ConsoleColor::White, ConsoleColor::DarkBlue, "DarkBlue\n");
    write_test(console, ConsoleColor::White, ConsoleColor::DarkGreen, "DarkGreen\n");
    write_test(console, ConsoleColor::White, ConsoleColor::DarkRed, "DarkRed\n");

    write_test(console, ConsoleColor::White, ConsoleColor::Cyan, "Cyan\n");
    write_test(console, ConsoleColor::White, ConsoleColor::Violet, "Violet\n");
    write_test(console, ConsoleColor::Black, ConsoleColor::Yellow, "Yellow\n");

    write_test(console, ConsoleColor::White, ConsoleColor::DarkCyan, "DarkCyan\n");
    write_test(console, ConsoleColor::White, ConsoleColor::DarkViolet, "DarkViolet\n");
    write_test(console, ConsoleColor::White, ConsoleColor::DarkYellow, "DarkYellow\n");

    write_test(console, ConsoleColor::Black, ConsoleColor::White, "White\n");
    write_test(console, ConsoleColor::Black, ConsoleColor::Gray, "Gray\n");
    write_test(console, ConsoleColor::White, ConsoleColor::Black, "Black\n");

    write_test(console, ConsoleColor::Default, ConsoleColor::Default, "Default Background & Foreground \n");

    // Setting, calling multiple times
    console.foreground_color(ConsoleColor::Red);
    console.background_color(ConsoleColor::White);
    write_test(console, "This is a red on white message\n");
    write_test(console, "This one, too\n");
    
    console.foreground_color(ConsoleColor::Default);
    console.background_color(ConsoleColor::Default);
    write_test(console, "This should be back in default color\n");

    // Reset color function.
    console.foreground_color(ConsoleColor::DarkYellow);
    write_test(console, "This is some non-default text.\n");
    console.reset_colors();
    write_test(console, "This is back to default.\n");

    return 0;
}

static void ghetto_widen(const char *val, wchar_t *dst, size_t size)
{
    size_t i;
    for (i = 0; (i < size - 1) && (val[i] != 0); ++i)
    {
        dst[i] = (wchar_t)val[i];
    }

    dst[i] = 0;
}

static void write_test(wincc::Console &console, const char *val)
{
    if (test_wide)
    {
        ghetto_widen(val, wbuffer, sizeof(wbuffer));
        console.writewc(wbuffer);
    }
    else
    {
        console.write(val);
    }
}

static void write_test(wincc::Console &console, ConsoleColor foreground, const char *val)
{
    if (test_wide)
    {
        ghetto_widen(val, wbuffer, sizeof(wbuffer));
        console.writewc(foreground, wbuffer);
    }
    else
    {
        console.write(foreground, val);
    }
}

static void write_test(wincc::Console &console, ConsoleColor foreground, ConsoleColor background, const char *val)
{
    if (test_wide)
    {
        ghetto_widen(val, wbuffer, sizeof(wbuffer));
        console.writewc(foreground, background, wbuffer);
    }
    else
    {
        console.write(foreground, background, val);
    }
}
