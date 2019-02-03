/******************************************************************************
MIT License

Copyright (c) 2019 Scott Clewell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/
#ifndef _WIN_COLOR_CONSOLE_HH
#define _WIN_COLOR_CONSOLE_HH

namespace wincc
{
    // Colors available for windows consoles. These match what the results are from macros defined
    // in Windows.h. PowerShell terminals will not end up with correct colors for some of these.
    enum class ConsoleColor
    {
        Black,
        Blue,
        Green,
        Red,
        DarkBlue,
        DarkGreen,
        DarkRed,
        Cyan,
        Violet,
        Yellow,
        DarkCyan,
        DarkViolet,
        DarkYellow,
        White,
        Gray,

        Default
    };

    // Allows writing to the console with various foreground and background colors. Supports 
    // narrow and wide character arrays. PowerShell consoles may display different results
    // than the default gray-on-black command line.
    class Console
    {
    public:

        Console();
        
        ~Console();

        // Get the current background color.
        ConsoleColor background_color() const { return m_background_color; }

        // Set the current background color.
        void background_color(ConsoleColor value);

        // Get the current foreground color.
        ConsoleColor foreground_color() const { return m_foreground_color; }

        // Set the current foreground color.
        void foreground_color(ConsoleColor value);

        // Reset colors back to default.
        void reset_colors();

        // Write to the console without setting new colors.
        void write(const char *msg);

        // Write to the console with the given foreground color.
        void write(ConsoleColor foreground, const char *msg);

        // Write to the console with the given foreground and background color.
        void write(ConsoleColor foreground, ConsoleColor background, const char *msg);

        // Write wide characters to the console without setting new colors.
        void writewc(const wchar_t *msg);

        // Write wide characters to the console with the given foreground color.
        void writewc(ConsoleColor foreground, const wchar_t *msg);

        // Write wide characters to the console with the given foreground and background color.
        void writewc(ConsoleColor foreground, ConsoleColor background, const wchar_t *msg);

    private:
        // Convert a color into a number used with SetConsoleTextAttribute.
        int _convert_attributes(ConsoleColor color);

        // Write with colors given once. This is an implementation for wide and narrow text.
        void _single_set_write(ConsoleColor foreground, ConsoleColor background, const void* msg, bool is_wide);

        // Compute a number for SetConsoleTextAttribute from a foreground and background color.
        int _compute_text_attribute(ConsoleColor foreground, ConsoleColor background);

        // Get color enum from integer.
        ConsoleColor _color_from_attribute(int attribute);

        void* m_console_handle;

        ConsoleColor m_background_color;
        ConsoleColor m_foreground_color;

        int m_current_text_attribute;

        int m_default_foreground;
        int m_default_background;
    };
}


#endif /* _WIN_COLOR_CONSOLE_HH */