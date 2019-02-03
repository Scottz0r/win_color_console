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
#include <assert.h>
#include <stdio.h>
#include <Windows.h>

#include "win_color_console.hh"

// Mask for the foreground color part CONSOLE_SCREEN_BUFFER_INFO wAttributes.
#define WINCC_FOREGROUND_MASK 0x0F

// Mask for the background color part from CONSOLE_SCREEN_BUFFER_INFO wAttributes.
#define WINCC_BACKGROUND_MASK 0xF0

// Number of bits to shift for background part of number for CONSOLE_SCREEN_BUFFER_INFO wAttributes.
#define WINCC_BG_SHIFT 4

wincc::Console::Console()
{
    BOOL winapi_result;

    m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // Get colors the console is using at the time of initialization. This may be different
    // depending on cmd.exe and powershell.exe or if the user configured different colors. The 
    // values from this will be what "ConsoleColor::Default" will set.
    CONSOLE_SCREEN_BUFFER_INFO console_info;
    winapi_result = GetConsoleScreenBufferInfo(m_console_handle, &console_info);

    if (winapi_result)
    {
        // Save to default fields.
        m_current_text_attribute = console_info.wAttributes;
        m_default_foreground = console_info.wAttributes & WINCC_FOREGROUND_MASK;
        m_default_background = console_info.wAttributes & WINCC_BACKGROUND_MASK;
    }
    else
    {
        // Fall back to default gray on black.
        m_default_foreground = _convert_attributes(ConsoleColor::Gray);
        m_default_background = _convert_attributes(ConsoleColor::Black) << WINCC_BG_SHIFT;
        m_current_text_attribute = m_default_foreground | m_default_background;
    }

    // Set enum values. Background needs shifted right for conversion.
    m_foreground_color = _color_from_attribute(m_default_foreground);
    m_background_color = _color_from_attribute(m_default_background >> WINCC_BG_SHIFT);
}

wincc::Console::~Console()
{
    CloseHandle(m_console_handle);
}

void wincc::Console::background_color(ConsoleColor value)
{
    m_background_color = value;
    m_current_text_attribute = _compute_text_attribute(m_foreground_color, m_background_color);
    SetConsoleTextAttribute(m_console_handle, (WORD)m_current_text_attribute);
}

void wincc::Console::foreground_color(ConsoleColor value)
{
    m_foreground_color = value;
    m_current_text_attribute = _compute_text_attribute(m_foreground_color, m_background_color);
    SetConsoleTextAttribute(m_console_handle, (WORD)m_current_text_attribute);
}

void wincc::Console::reset_colors()
{
    // Set enum values. Background needs shifted right for conversion.
    m_foreground_color = _color_from_attribute(m_default_foreground);
    m_background_color = _color_from_attribute(m_default_background >> WINCC_BG_SHIFT);

    m_current_text_attribute = _compute_text_attribute(m_foreground_color, m_background_color);
    SetConsoleTextAttribute(m_console_handle, (WORD)m_current_text_attribute);
}

void wincc::Console::write(const char *msg)
{
    printf(msg);
}

void wincc::Console::write(ConsoleColor foreground, const char *msg)
{
    _single_set_write(foreground, m_background_color, msg, false);
}

void wincc::Console::write(ConsoleColor foreground, ConsoleColor background, const char *msg)
{
    _single_set_write(foreground, background, msg, false);
}

void wincc::Console::writewc(const wchar_t *msg)
{
    wprintf(msg);
}

void wincc::Console::writewc(ConsoleColor foreground, const wchar_t *msg)
{
    _single_set_write(foreground, m_background_color, msg, true);
}

void wincc::Console::writewc(ConsoleColor foreground, ConsoleColor background, const wchar_t *msg)
{
    _single_set_write(foreground, background, msg, true);
}

int wincc::Console::_convert_attributes(ConsoleColor color)
{
    WORD result_attr;

    switch (color)
    {
    case ConsoleColor::Blue:
        result_attr = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::Green:
        result_attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::Red:
        result_attr = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;


    case ConsoleColor::DarkBlue:
        result_attr = FOREGROUND_BLUE;
        break;
    case ConsoleColor::DarkGreen:
        result_attr = FOREGROUND_GREEN;
        break;
    case ConsoleColor::DarkRed:
        result_attr = FOREGROUND_RED;
        break;


    case ConsoleColor::Cyan:
        result_attr = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::Violet:
        result_attr = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::Yellow:
        result_attr = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;


    case ConsoleColor::DarkCyan:
        result_attr = FOREGROUND_BLUE | FOREGROUND_GREEN;
        break;
    case ConsoleColor::DarkViolet:
        result_attr = FOREGROUND_BLUE | FOREGROUND_RED;
        break;
    case ConsoleColor::DarkYellow:
        result_attr = FOREGROUND_GREEN | FOREGROUND_RED;
        break;


    case ConsoleColor::White:
        result_attr = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;

    case ConsoleColor::Gray:
        result_attr = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        break;


    // Black for fall through color.
    case ConsoleColor::Black:
    default:
        result_attr = 0;
        break;
    }

    return result_attr;
}

void wincc::Console::_single_set_write(ConsoleColor foreground, ConsoleColor background, const void* msg, bool is_wide)
{
    // Change foreground and background color.
    int temp_attribute = _compute_text_attribute(foreground, background);
    SetConsoleTextAttribute(m_console_handle, (WORD)temp_attribute);

    // Use implementation function to write.
    if (is_wide)
    {
        wprintf((const wchar_t *)msg);
    }
    else
    {
        printf((const char *)msg);
    }

    // Revert color.
    SetConsoleTextAttribute(m_console_handle, (WORD)m_current_text_attribute);
}

int wincc::Console::_compute_text_attribute(ConsoleColor foreground, ConsoleColor background)
{
    int result = 0;

    // Convert foreground if not default. Otherwise use default found in constructor.
    if (foreground != ConsoleColor::Default)
    {
        result |= _convert_attributes(foreground);
    }
    else
    {
        result |= m_default_foreground;
    }

    // Convert background if not default. Otherwise use default found in constructor.
    if (background != ConsoleColor::Default)
    {
        // Converted color is shifted by 4 bits to place in background color position.
        result |= (_convert_attributes(background) << WINCC_BG_SHIFT);
    }
    else
    {
        // Default background saved in constructor does not need shifted.
        result |= m_default_background;
    }

    return result;
}


wincc::ConsoleColor wincc::Console::_color_from_attribute(int attribute)
{
    ConsoleColor result_color;

    switch (attribute)
    {
    case FOREGROUND_BLUE | FOREGROUND_INTENSITY:
        result_color = ConsoleColor::Blue;
        break;
    case FOREGROUND_GREEN | FOREGROUND_INTENSITY:
        result_color = ConsoleColor::Green;
        break;
    case FOREGROUND_RED | FOREGROUND_INTENSITY:
        result_color = ConsoleColor::Red;
        break;

    case FOREGROUND_BLUE:
        result_color = ConsoleColor::DarkBlue;
        break;
    case FOREGROUND_GREEN:
        result_color = ConsoleColor::DarkGreen;
        break;
    case FOREGROUND_RED:
        result_color = ConsoleColor::DarkRed;
        break;

    case FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY:
        result_color = ConsoleColor::Cyan;
        break;
    case FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY:
        result_color = ConsoleColor::Violet;
        break;
    case FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY:
        result_color = ConsoleColor::Yellow;
        break;


    case FOREGROUND_BLUE | FOREGROUND_GREEN:
        result_color = ConsoleColor::DarkCyan;
        break;
    case FOREGROUND_BLUE | FOREGROUND_RED:
        result_color = ConsoleColor::DarkViolet;
        break;
    case FOREGROUND_GREEN | FOREGROUND_RED:
        result_color = ConsoleColor::DarkYellow;
        break;


    case FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY:
        result_color = ConsoleColor::White;
        break;

    case FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED:
        result_color = ConsoleColor::Gray;
        break;

    // Black for fall through color.
    case 0:
    default:
        result_color = ConsoleColor::Black;
        break;
    }

    return result_color;
}
