/*
    Copyright (c) 2015 Alexandru-Mihai Maftei. All rights reserved.


    Developed by: Alexandru-Mihai Maftei
    aka Vercas
    http://vercas.com | https://github.com/vercas/Beelzebub

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal with the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

      * Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimers.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimers in the
        documentation and/or other materials provided with the distribution.
      * Neither the names of Alexandru-Mihai Maftei, Vercas, nor the names of
        its contributors may be used to endorse or promote products derived from
        this Software without specific prior written permission.


    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    WITH THE SOFTWARE.

    ---

    You may also find the text of this license in "LICENSE.md", along with a more
    thorough explanation regarding other files.
*/

#include <terminals/vbe.hpp>
#include <terminals/font.hpp>

using namespace Beelzebub;
using namespace Beelzebub::Terminals;

/*  VBE terminal descriptor  */

TerminalCapabilities VbeTerminalCapabilities = {
    true,   //  bool CanOutput;            //  Characters can be written to the terminal.
    false,  //  bool CanInput;             //  Characters can be received from the terminal.
    false,  //  bool CanRead;              //  Characters can be read back from the terminal's output.

    true,   //  bool CanGetOutputPosition; //  Position of next output character can be retrieved.
    true,   //  bool CanSetOutputPosition; //  Position of output characters can be set arbitrarily.
    false,  //  bool CanPositionCursor;    //  Terminal features a positionable cursor.

    true,   //  bool CanGetSize;           //  Terminal (window) size can be retrieved.
    false,  //  bool CanSetSize;           //  Terminal (window) size can be changed.

    false,  //  bool Buffered;             //  Terminal acts as a window over a buffer.
    false,  //  bool CanGetBufferSize;     //  Buffer size can be retrieved.
    false,  //  bool CanSetBufferSize;     //  Buffer size can be changed.
    false,  //  bool CanPositionWindow;    //  The "window" can be positioned arbitrarily over the buffer.

    false,  //  bool CanColorBackground;   //  Area behind/around output characters can be colored.
    false,  //  bool CanColorForeground;   //  Output characters can be colored.
    false,  //  bool FullColor;            //  32-bit BGRA, or ARGB in little endian.
    false,  //  bool ForegroundAlpha;      //  Alpha channel of foreground color is supported. (ignored if false)
    false,  //  bool BackgroundAlpha;      //  Alpha channel of background color is supported. (ignored if false)

    false,  //  bool CanBold;              //  Output characters can be made bold.
    false,  //  bool CanUnderline;         //  Output characters can be underlined.
    false,  //  bool CanBlink;             //  Output characters can blink.

    false,  //  bool CanGetStyle;          //  Current style settings can be retrieved.

    true,   //  bool CanGetTabulatorWidth; //  Tabulator width may be retrieved.
    true,   //  bool CanSetTabulatorWidth; //  Tabulator width may be changed.

    true,   //  bool SequentialOutput;     //  Character sequences can be output without explicit position.

    false,  //  bool SupportsTitle;        //  Supports assignment of a title.

    TerminalType::PixelMatrix    //  TerminalType Type;         //  The known type of the terminal.
};

/*************************
    VbeTerminal struct
*************************/

/*  Constructors    */

VbeTerminal::VbeTerminal(const uintptr_t mem, uint16_t wid, uint16_t hei, uint32_t pit, uint8_t bytesPerPixel)
    : TerminalBase(&VbeTerminalCapabilities)
    , Width(wid)
    , Height(hei)
    , Pitch(pit)
    , VideoMemory(mem)
    , BytesPerPixel(bytesPerPixel)
{
    TerminalCoordinates size = this->GetSize();

    for (int x = 0; x < size.X; ++x)
        for (int y = 0; y < size.Y; ++y)
            this->WriteUtf8At(" ", x, y);
}

/*  Writing  */

TerminalWriteResult VbeTerminal::WriteUtf8At(char const * c, const int16_t cx, const int16_t cy)
{
    /* TEMP */ uint32_t const colf = 0xFFDFE0E6;
    /* TEMP */ uint32_t const colb = 0xFF262223;

    size_t const w = FontWidth;
    size_t const h = FontHeight;
    size_t const x = cx * w;
    size_t const y = cy * h;

    size_t lx, ly, bit;
    size_t line = this->VideoMemory + y * this->Pitch + x * this->BytesPerPixel;
    size_t const byteWidth = w / 8;

    uint32_t i = 1U;   //  Number of bytes in this character.

    if (*c == ' ')
    {
        if (colb == NOCOL)
            return {HandleResult::Okay, 1U, {cx, cy}};

        for (ly = 0; ly < h; ++ly)
        {
            size_t col = 0;

            for (lx = 0; lx < byteWidth; ++lx)
                for (bit = 0; bit < 8; ++bit)
                {
                    *((uint32_t *)(col + line)) = colb;

                    col += this->BytesPerPixel;
                }

            line += this->Pitch;
        }
    }
    else
    {
        uint8_t const * bmp = Font[*c - FontMin];

        if (*c >= 0x80)
        {
            //  This belongs to a multibyte character... Uh oh...

            goto skip_bitmap;
        }

        for (ly = 0; ly < h; ++ly)
        {
            size_t ind = ly * w / 8, col = 0;

            for (lx = 0; lx < byteWidth; ++lx)
                for (bit = 0; bit < 8; ++bit)
                {
                    if (bmp[ind + lx] & (0x80 >> bit))
                    {
                        if (colf == INVCOL)
                            *((uint32_t *)(col + line)) = ~*((uint32_t *)(col + line));
                        else if (colf != NOCOL)
                            *((uint32_t *)(col + line)) = colf;
                    }
                    else if (colb != NOCOL)
                        *((uint32_t *)(col + line)) = colb;

                    col += this->BytesPerPixel;
                }

            line += this->Pitch;
        }
    }

skip_bitmap:

    return {HandleResult::Okay, i, {cx, cy}};
}

/*  Positioning  */

TerminalCoordinates VbeTerminal::GetSize()
{
    return {(int16_t)((int32_t)this->Width / (int32_t)FontWidth), (int16_t)((int32_t)this->Height / (int32_t)FontHeight)};
}

/*  Remapping  */

void VbeTerminal::RemapMemory(uintptr_t newAddr)
{
    this->VideoMemory = newAddr;
}
