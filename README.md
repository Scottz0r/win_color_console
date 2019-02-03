# Win Console Color

C++ code for setting console colors for Windows. Provides a `Console` object that allows for easy setting of colors when writing to the console. Supports `char` and `wchar_t`.

Only a header and source file are required: `win_color_console.hh` and `win_color_console.cc`.

## Quick Start

Create a instance of `wincc::Console`. Only one instance should be used per application, but it is up to the caller to decide how to keep it around as a single instance. Making a second instance will not break anything, but it may mess with the "default" colors.

The `wincc::ConsoleColor` enum contains the available colors for Windows consoles. Note: PowerShell console windows will have different behavior for some of the colors.

Use `foreground_color(ConsoleColor value)` and `background_color(ConsoleColor value)` to change the colors used when writing text. Use the `write` methods to change colors only for the given text. These only accept const char *, and it is up to the caller to format text and convert from std::strings, if needed. These internally use `printf` and `wprintf`.

Also available are write methods that support wchar_t. It is undefined behavior to mix the wchar_t and char methods.

## Demo 

```c++
using wincc::ConsoleColor;
wincc::Console console;

// Only changing foreground color.
console.write(ConsoleColor::Blue, "Blue\n");
console.write(ConsoleColor::Green, "Green\n");
console.write(ConsoleColor::Red, "Red\n");

// Changing foreground and background color.
console.write(ConsoleColor::White, ConsoleColor::Cyan, "Cyan\n");
console.write(ConsoleColor::White, ConsoleColor::Violet, "Violet\n");
console.write(ConsoleColor::Black, ConsoleColor::Yellow, "Yellow\n");

// Setting to "default" values from when console object was initialized.
console.write(ConsoleColor::Default, ConsoleColor::Default, "Default Background & Foreground \n");

// Set for all text going forward.
console.foreground_color(ConsoleColor::Red);
console.background_color(ConsoleColor::White);
console.write("This is a red on white message\n");
console.write("This one, too\n");

// Reset to defaults.
console.reset_colors();
console.write("This is back to default.\n");
```
