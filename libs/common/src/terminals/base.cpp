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

#include <terminals/base.hpp>
// #include <debug.hpp>

using namespace Beelzebub;
using namespace Beelzebub::Terminals;

/*************************
    TerminalBase class
*************************/

/*  Constructor  */

TerminalBase::TerminalBase(const TerminalCapabilities * const caps)
    : Capabilities(caps)
    , CurrentPosition({0, 0})
    , TabulatorWidth(4)
    , Overflown(false)
{

}

/*  Writing  */

TerminalWriteResult TerminalBase::WriteUtf8At(char const * c, int16_t const x, int16_t const y)
{
    return {HandleResult::NotImplemented, 0U, InvalidCoordinates};
}

TerminalWriteResult TerminalBase::WriteUtf8(char const * c)
{
    if (!(this->Capabilities->CanOutput
       && this->Capabilities->CanGetOutputPosition))
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char temp[7] = "\0\0\0\0\0";
    temp[0] = *c;
    //  6 bytes + null terminator in UTF-8 character.

    if ((*c & 0xC0) == 0xC0)
    {
        size_t i = 1;

        do
        {
            temp[i] = c[i];

            ++i;
        } while ((c[i] & 0xC0) == 0x80 && i < 7);

        //  This copies the remainder of the bytes, up to 6.
    }

    return this->WriteAt(temp, this->GetCurrentPosition());
}

TerminalWriteResult TerminalBase::WriteAt(char const c, int16_t const x, int16_t const y)
{
    return this->WriteUtf8At(&c, x, y);
}

TerminalWriteResult TerminalBase::WriteAt(char const c, TerminalCoordinates const pos)
{
    return this->WriteUtf8At(&c, pos.X, pos.Y);
}

TerminalWriteResult TerminalBase::WriteAt(char const * const str, TerminalCoordinates const pos)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};
    //  First of all, the terminal needs to be capable of output.

    TerminalCoordinates size;

    if (this->Capabilities->CanGetBufferSize)
        size = this->GetBufferSize();
    else if (this->Capabilities->CanGetSize)
        size = this->GetSize();
    else
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};
    //  A buffer or window size is required to prevent drawing outside
    //  the boundaries.

    uint16_t tabWidth = TerminalBase::DefaultTabulatorWidth;

    if (this->Capabilities->CanGetTabulatorWidth)
        tabWidth = this->GetTabulatorWidth();
    //  Tabulator width is required for proper handling of \t

    int16_t x = pos.X, y = pos.Y;
    //  These are used for positioning characters.

    //msg("Writing %s at %u2:%u2.%n", str, x, y);

#define NEXTLINE do {                           \
    if (y == size.Y)                            \
    {                                           \
        this->Overflown = true;                 \
        y = 0;                                  \
        for (int16_t _x = 0; _x < size.X; ++_x) \
            this->WriteUtf8At(" ", _x, y);      \
    }                                           \
    else if (this->Overflown)                   \
        for (int16_t _x = 0; _x < size.X; ++_x) \
            this->WriteUtf8At(" ", _x, y);      \
} while (false)

    uint32_t i = 0, u = 0;
    //  Declared here so I know how many characters have been written.

    for (; 0 != str[i]; ++i, ++u)
    {
        //  Stop at null, naturally.

        char c = str[i];

        if (c == '\r')
            x = 0; //  Carriage return.
        else if (c == '\n')
        {
            ++y;   //  Line feed does not return carriage.
            NEXTLINE;
        }
        else if (c == '\t')
        {
            uint16_t diff = tabWidth - (x % tabWidth);

            for (int16_t _x = 0; _x < diff; ++_x)
                this->WriteUtf8At(" ", x + _x, y);

            x += diff;
        }
        else if (c == '\b')
        {
            if (x > 0)
                --x;
            //  Once you go \b, you do actually go back.
        }
        else
        {
            if (x == size.X)
            {  x = 0; y++; NEXTLINE;  }

            TerminalWriteResult tmp = this->WriteUtf8At(str + i, x, y);
            //  str + i = &c;

            if (!tmp.Result.IsOkayResult())
                return {tmp.Result, i, {x, y}};

            i += tmp.Size - 1;
            //  Account for UTF-8 multibyte characters.

            x++;
        }
    }

    if (this->Capabilities->CanSetOutputPosition)
    {
        Handle res = this->SetCurrentPosition(x, y);

        return {res, i, {x, y}};
    }

    return {HandleResult::Okay, u, {x, y}};
}

TerminalWriteResult TerminalBase::Write(char const c)
{
    return this->WriteUtf8(&c);
}

TerminalWriteResult TerminalBase::Write(char const * const str)
{
    if (!(this->Capabilities->CanOutput
       && this->Capabilities->CanGetOutputPosition))
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    return this->WriteAt(str, this->GetCurrentPosition());
}

TerminalWriteResult TerminalBase::Write(char const * const fmt, va_list args)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    TerminalWriteResult res {};
    uint32_t cnt = 0U;

    bool inFormat = false;
    char c;
    char const * fmts = fmt;

    while ((c = *fmts++) != 0)
    {
        if (inFormat)
        {
            if (c == 'u')       //  Unsigned decimal.
            {
                /*  Unsigned decimal integer  */

                char size = *fmts++;

                switch (size)
                {
                    case 's':
                        size = '0' + sizeof(size_t);
                        break;

                    case 'S':
                        size = '0' + sizeof(psize_t);
                        break;

                    case 'p':
                        size = '0' + sizeof(void *);
                        break;

                    case 'P':
                        size = '0' + sizeof(paddr_t);
                        break;

                    default:
                        break;
                }

                switch (size)
                {
                    case '8':
                        {
                            uint64_t val8 = va_arg(args, uint64_t);
                            TERMTRY1(this->WriteUIntD(val8), res, cnt);
                        }
                        break;

                    case '4':
                    case '2':
                    case '1':
                        /*  Apparently chars and shorts are promoted to integers!  */

                        {
                            uint32_t val4 = va_arg(args, uint32_t);
                            TERMTRY1(this->WriteUIntD(val4), res, cnt);
                        }
                        break;

                    default:
                        return {HandleResult::FormatBadArgumentSize, cnt, InvalidCoordinates};
                }
            }
            else if (c == 'i')       //  Unsigned decimal.
            {
                /*  Unsigned decimal integer  */

                char size = *fmts++;

                switch (size)
                {
                    case 's':
                        size = '0' + sizeof(size_t);
                        break;

                    case 'S':
                        size = '0' + sizeof(psize_t);
                        break;

                    case 'p':
                        size = '0' + sizeof(void *);
                        break;

                    case 'P':
                        size = '0' + sizeof(paddr_t);
                        break;

                    default:
                        break;
                }

                switch (size)
                {
                    case '8':
                        {
                            int64_t val8 = va_arg(args, int64_t);
                            TERMTRY1(this->WriteIntD(val8), res, cnt);
                        }
                        break;

                    case '4':
                    case '2':
                    case '1':
                        /*  Apparently chars and shorts are promoted to integers!  */

                        {
                            int32_t val4 = va_arg(args, int32_t);
                            TERMTRY1(this->WriteIntD(val4), res, cnt);
                        }
                        break;

                    default:
                        return {HandleResult::FormatBadArgumentSize, cnt, InvalidCoordinates};
                }
            }
            else if (c == 'X')       //  Hexadecimal.
            {
                /*  Unsigned decimal integer  */

                char size = *fmts++;

                switch (size)
                {
                    case 's':
                        size = '0' + sizeof(size_t);
                        break;

                    case 'S':
                        size = '0' + sizeof(psize_t);
                        break;

                    case 'p':
                        size = '0' + sizeof(void *);
                        break;

                    case 'P':
                        size = '0' + sizeof(paddr_t);
                        break;

                    default:
                        break;
                }

                switch (size)
                {
                    case '8':
                        {
                            uint64_t val8 = va_arg(args, uint64_t);
                            TERMTRY1(this->WriteHex64(val8), res, cnt);
                        }
                        break;

                    case '4':
                        {
                            uint32_t val4 = va_arg(args, uint32_t);
                            TERMTRY1(this->WriteHex32(val4), res, cnt);
                        }
                        break;

                    case '2':
                        /*  Apparently chars and shorts are promoted to integers!  */

                        {
                            uint32_t val2 = va_arg(args, uint32_t);
                            TERMTRY1(this->WriteHex16((uint16_t)val2), res, cnt);
                        }
                        break;

                    case '1':
                        /*  Apparently chars and shorts are promoted to integers!  */

                        {
                            uint32_t val1 = va_arg(args, uint32_t);
                            TERMTRY1(this->WriteHex8((uint8_t)val1), res, cnt);
                        }
                        break;

                    default:
                        return {HandleResult::FormatBadArgumentSize, cnt, InvalidCoordinates};
                }
            }
            else if (c == 'H')  //  Handle.
            {
                Handle h = va_arg(args, Handle);

                TERMTRY1(this->WriteHandle(h), res, cnt);
            }
            else if (c == 'B')  //  Boolean (T/F)
            {
                uint32_t val = va_arg(args, uint32_t);

                TERMTRY1(this->WriteUtf8((bool)val ? "T" : "F"), res, cnt);
            }
            else if (c == 'b')  //  Boolean/bit (1/0)
            {
                uint32_t val = va_arg(args, uint32_t);

                TERMTRY1(this->WriteUtf8((bool)val ? "1" : "0"), res, cnt);
            }
            else if (c == 't')  //  Tick (X/ )
            {
                uint32_t val = va_arg(args, uint32_t);

                TERMTRY1(this->WriteUtf8((bool)val ? "X" : " "), res, cnt);
            }
            else if (c == 's')  //  String.
            {
                char * str = va_arg(args, char *);

                TERMTRY1(this->Write(str), res, cnt);
            }
            else if (c == 'S')  //  Solid String.
            {
                size_t len = va_arg(args, size_t);
                char * str = va_arg(args, char *);

                for (size_t i = 0; i < len; ++i)
                    TERMTRY1(this->WriteUtf8(str + i), res, cnt);
            }
            else if (c == 'C')  //  Character from pointer.
            {
                char * chr = va_arg(args, char *);

                TERMTRY1Ex(this->WriteUtf8(chr), res, cnt);
            }
            else if (c == 'c')  //  Character.
            {
                char const chr = (char)(va_arg(args, uint32_t));

                TERMTRY1(this->WriteUtf8(&chr), res, cnt);
            }
            else if (c == '#')  //  Get current character count.
            {
                size_t * const ptr = va_arg(args, size_t *);

                *ptr = res.Size;
            }
            else if (c == '*')  //  Fill with spaces.
            {
                size_t const n = va_arg(args, size_t);

                cnt = res.Size;

                for (size_t i = n; i > 0; --i)
                {
                    res = this->WriteUtf8(" ");

                    if (!res.Result.IsOkayResult())
                        return res;
                }

                res.Size = cnt + (uint32_t)n;
            }
            else if (c == 'n')  //  Newline.
            {
                TERMTRY1(this->Write("\r\n"), res, cnt);
            }
            else if (c == '%')
                TERMTRY1(this->WriteUtf8("%"), res, cnt);
            else
                return {HandleResult::FormatBadSpecifier, cnt, InvalidCoordinates};

            inFormat = false;
        }
        else if (c == '%')
            inFormat = true;
        else
        {
            cnt = res.Size;

            res = this->WriteUtf8(fmts - 1);
            //  `fmts - 1` = &c

            if (res.Result.IsOkayResult())
            {
                fmts += res.Size - 1;
                //  Skip continuation bytes of this UTF-8 character.

                res.Size = cnt + 1;
                //  Only one character was added.
            }
            else
                return res;
        }
    }

    return res;
}

TerminalWriteResult TerminalBase::WriteLine(char const * const str)
{
    TerminalWriteResult tmp = this->Write(str);

    if (!tmp.Result.IsOkayResult())
        return tmp;

    return this->Write("\r\n");
}

TerminalWriteResult TerminalBase::WriteFormat(char const * const fmt, ...)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};
    //  I do the check here to avoid messing with the varargs.

    TerminalWriteResult res;
    va_list args;

    va_start(args, fmt);
    res = this->Write(fmt, args);
    va_end(args);

    return res;
}

/*  Positioning  */

Handle TerminalBase::SetCursorPosition(int16_t const x, int16_t const y)
{
    return HandleResult::UnsupportedOperation;
}

Handle TerminalBase::SetCursorPosition(TerminalCoordinates const pos)
{
    return this->SetCursorPosition(pos.X, pos.Y);
}

TerminalCoordinates TerminalBase::GetCursorPosition()
{
    return InvalidCoordinates;
}


Handle TerminalBase::SetCurrentPosition(int16_t const x, int16_t const y)
{
    return this->SetCurrentPosition({x, y});
}

Handle TerminalBase::SetCurrentPosition(TerminalCoordinates const pos)
{
    if (!this->Capabilities->CanSetOutputPosition)
        return HandleResult::UnsupportedOperation;

    this->CurrentPosition = pos;

    return HandleResult::Okay;
}

TerminalCoordinates TerminalBase::GetCurrentPosition()
{
    if (!this->Capabilities->CanGetOutputPosition)
        return InvalidCoordinates;

    return this->CurrentPosition;
}


Handle TerminalBase::SetSize(int16_t const w, int16_t const h)
{
    return HandleResult::UnsupportedOperation;
}

Handle TerminalBase::SetSize(TerminalCoordinates const pos)
{
    return this->SetSize(pos.X, pos.Y);
}

TerminalCoordinates TerminalBase::GetSize()
{
    return InvalidCoordinates;
}


Handle TerminalBase::SetBufferSize(int16_t const w, int16_t const h)
{
    return HandleResult::UnsupportedOperation;
}

Handle TerminalBase::SetBufferSize(TerminalCoordinates const pos)
{
    return this->SetBufferSize(pos.X, pos.Y);
}

TerminalCoordinates TerminalBase::GetBufferSize()
{
    return InvalidCoordinates;
}


Handle TerminalBase::SetTabulatorWidth(uint16_t const w)
{
    if (!this->Capabilities->CanSetTabulatorWidth)
        return HandleResult::UnsupportedOperation;

    this->TabulatorWidth = w;

    return HandleResult::Okay;
}

uint16_t TerminalBase::GetTabulatorWidth()
{
    if (!this->Capabilities->CanGetTabulatorWidth)
        return ~((uint16_t)0);

    return this->TabulatorWidth;
}

/*  Utility  */

TerminalWriteResult TerminalBase::WriteHandle(const Handle val)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char str[22] = "<    | |            >";
    //  <type|global/fatal|result/index>

    char const * const strType = val.GetTypeString();
    char const * const strRes  = val.GetResultString();

    for (size_t i = 0; 0 != strType[i] && i < 4; ++i)
        str[1 + i] = strType[i];

    if (!val.IsValid())
    {
        str[6] = '-';

        for (size_t i = 8; i < 20; ++i)
            str[i] = '-';
    }
    else if (val.IsType(HandleType::Result))
    {
        if (val.IsFatalResult())
            str[6] = 'F';

        char * const str2 = str + 8;

        for (size_t i = 0; 0 != strRes[i] && i < 12; ++i)
            str2[i] = strRes[i];
    }
    else
    {
        if (val.IsGlobal())
            str[6] = 'G';

        char * const str2 = str + 8;
        uint64_t const ind = val.GetIndex();

        for (size_t i = 0; i < 12; ++i)
        {
            uint8_t nib = (ind >> (i << 2)) & 0xF;

            str2[11 - i] = (nib > 9 ? '7' : '0') + nib;
        }
    }

    return this->Write(str);
}

TerminalWriteResult TerminalBase::WriteIntD(int64_t const val)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char str[21];
    str[20] = 0;

    size_t i = 0;
    uint64_t x = val > 0 ? (uint64_t)val : ((uint64_t)(-val));

    do
    {
        uint8_t digit = x % 10;

        str[19 - i++] = '0' + digit;

        x /= 10;
    }
    while (x > 0 && i < 20);

    if (val < 0)
        str[19 - i++] = '-';

    return this->Write(str + 20 - i);
}

TerminalWriteResult TerminalBase::WriteUIntD(uint64_t const val)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char str[21];
    str[20] = 0;

    size_t i = 0;
    uint64_t x = val;

    do
    {
        uint8_t digit = x % 10;

        str[19 - i++] = '0' + digit;

        x /= 10;
    }
    while (x > 0 && i < 20);

    return this->Write(str + 20 - i);
}

TerminalWriteResult TerminalBase::WriteHex8(uint8_t const val)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char str[3];
    str[2] = '\0';

    for (size_t i = 0; i < 2; ++i)
    {
        uint8_t nib = (val >> (i << 2)) & 0xF;

        str[1 - i] = (nib > 9 ? '7' : '0') + nib;
    }

    return this->Write(str);
}

TerminalWriteResult TerminalBase::WriteHex16(uint16_t const val)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char str[5];
    str[4] = '\0';

    for (size_t i = 0; i < 4; ++i)
    {
        uint8_t nib = (val >> (i << 2)) & 0xF;

        str[3 - i] = (nib > 9 ? '7' : '0') + nib;
    }

    return this->Write(str);
}

TerminalWriteResult TerminalBase::WriteHex32(uint32_t const val)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char str[9];
    str[8] = '\0';

    for (size_t i = 0; i < 8; ++i)
    {
        uint8_t nib = (val >> (i << 2)) & 0xF;

        str[7 - i] = (nib > 9 ? '7' : '0') + nib;
    }

    return this->Write(str);
}

TerminalWriteResult TerminalBase::WriteHex64(uint64_t const val)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char str[17];
    str[16] = '\0';

    for (size_t i = 0; i < 16; ++i)
    {
        uint8_t nib = (val >> (i << 2)) & 0xF;

        str[15 - i] = (nib > 9 ? '7' : '0') + nib;
    }

    return this->Write(str);
}

TerminalWriteResult TerminalBase::WriteHexDump(uintptr_t const start, size_t const length, size_t const charsPerLine)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char addrhexstr[sizeof(size_t) * 2 + 1], wordhexstr[5], spaces[11] = "          ";
    addrhexstr[sizeof(size_t) * 2] = '\0'; wordhexstr[4] = '\0';

    TerminalWriteResult res {};
    uint32_t cnt;

    for (size_t i = 0; i < length; i += charsPerLine)
    {
        uintptr_t lStart = start + i;

        for (size_t j = 0; j < sizeof(size_t) * 2; ++j)
        {
            uint8_t nib = (lStart >> (j << 2)) & 0xF;

            addrhexstr[sizeof(size_t) * 2 - 1 - j] = (nib > 9 ? '7' : '0') + nib;
        }

        TERMTRY1(this->Write(addrhexstr), res, cnt);
        TERMTRY1(this->Write(": "), res, cnt);

        for (size_t j = 0; j < charsPerLine; j += 2)
        {
            size_t spacesOffset = 9;

            if (j > 0)
            {
                if ((j & 0x003) == 0) { --spacesOffset;
                if ((j & 0x007) == 0) { --spacesOffset;
                if ((j & 0x00F) == 0) { --spacesOffset;
                if ((j & 0x01F) == 0) { --spacesOffset;
                if ((j & 0x03F) == 0) { --spacesOffset;
                if ((j & 0x07F) == 0) { --spacesOffset;
                if ((j & 0x0FF) == 0) { --spacesOffset;
                if ((j & 0x1FF) == 0) { --spacesOffset;
                if ((j & 0x3FF) == 0)   --spacesOffset; }}}}}}}}

                //  This is undoubtedly the ugliest hack I've ever written.
            }

            TERMTRY1(this->Write(spaces + spacesOffset), res, cnt);

            uint16_t val = *(uint16_t *)(lStart + j);

            for (size_t j = 0; j < 4; ++j)
            {
                uint8_t nib = (val >> (j << 2)) & 0xF;

                wordhexstr[j ^ 1] = (nib > 9 ? '7' : '0') + nib;
                //  This may be THE smartest, though. It flips the last
                //  bit, so bytes are big endian but the whole word is not.
            }

            TERMTRY1(this->Write(wordhexstr), res, cnt);
        }

        TERMTRY1(this->Write("\n\r"), res, cnt);
    }

    return res;
}

TerminalWriteResult TerminalBase::WriteHexTable(uintptr_t const start, size_t const length, size_t const charsPerLine, bool const ascii)
{
    if (!this->Capabilities->CanOutput)
        return {HandleResult::UnsupportedOperation, 0U, InvalidCoordinates};

    char addrhexstr[sizeof(size_t) * 2 + 1], wordhexstr[4] = "   ";
    addrhexstr[sizeof(size_t) * 2] = '\0';

    TerminalWriteResult res = {HandleResult::Okay, 0U, InvalidCoordinates};
    uint32_t cnt = 0U;

    size_t actualCharsPerLine = charsPerLine;

    if (0 == actualCharsPerLine)
    {
        if (this->Capabilities->CanGetBufferSize)
        {
            TerminalCoordinates size = this->GetBufferSize();
    
            if (ascii)
            {
                actualCharsPerLine = (size.X - (sizeof(uintptr_t) * 2) - 4) >> 2;
                //  Per line, there is an address and the ':', ' |', '|'.
                //  The latter three make 4 characters.
                //  Besides this, there is a space, two hexadecimal digits and
                //  an ASCII character per byte: 4 chars (thus the >> 2).
            }
            else
            {
                actualCharsPerLine = (size.X - (sizeof(uintptr_t) * 2) - 1) / 3;
                //  Per line, there is an address and the ':'.
                //  The latter three make 1 character.
                //  Besides this, there is a space abd two hexadecimal digits
                //  per byte: 4 chars (thus the >> 2).
            }
        }
        else
            return {HandleResult::ArgumentOutOfRange, 0U, InvalidCoordinates};
    }

    for (size_t i = 0; i < length; i += actualCharsPerLine)
    {
        uintptr_t lStart = start + i;

        for (size_t j = 0; j < sizeof(size_t) * 2; ++j)
        {
            uint8_t nib = (lStart >> (j << 2)) & 0xF;

            addrhexstr[sizeof(size_t) * 2 - 1 - j] = (nib > 9 ? '7' : '0') + nib;
        }

        TERMTRY1(this->Write(addrhexstr), res, cnt);
        TERMTRY1(this->WriteUtf8(":"), res, cnt);

        for (size_t j = 0; j < actualCharsPerLine; ++j)
        {
            uint8_t val = *(uint8_t *)(lStart + j);

            uint8_t nib = val & 0xF;
            wordhexstr[2] = (nib > 9 ? '7' : '0') + nib;
            nib = (val >> 4) & 0xF;
            wordhexstr[1] = (nib > 9 ? '7' : '0') + nib;

            TERMTRY1(this->Write(wordhexstr), res, cnt);
        }

        if (ascii)
        {
            TERMTRY1(this->Write(" |"), res, cnt);

            for (size_t j = 0; j < actualCharsPerLine; ++j)
            {
                uint8_t val = *(uint8_t *)(lStart + j);

                if (val >= 32 && val != 127)
                    TERMTRY1(this->WriteUtf8(reinterpret_cast<char *>(&val)), res, cnt);
                else
                    TERMTRY1(this->WriteUtf8("."), res, cnt);
            }

            TERMTRY1(this->Write("|\n\r"), res, cnt);
        }
        else
            TERMTRY1(this->Write("\n\r"), res, cnt);
    }

    return res;
}

/*********************************
    TerminalCoordinates struct
*********************************/

inline TerminalCoordinates TerminalCoordinates::operator+(TerminalCoordinates const other)
{
    return { (int16_t)(this->X + other.X), (int16_t)(this->Y + other.Y) };
}

inline TerminalCoordinates TerminalCoordinates::operator-(TerminalCoordinates const other)
{
    return { (int16_t)(this->X - other.X), (int16_t)(this->Y - other.Y) };
}