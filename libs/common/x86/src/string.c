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

/**
 *  The loops here should be perfectly compatible with single machine code
 *  instructions. Maybe with a branch or two mixed in.
 */

#include <string.h>

bool memeq(void const * src1, void const * src2, size_t len)
{
    if (src1 == src2)
        return true;

    bool ret;

    asm volatile ( "repe cmpsb \n\t"
                   "sete %3    \n\t"
                 : "+D" (src1), "+S" (src2), "+c" (len), "=a"(ret)
                 :
                 : "memory" );

    return ret;

    /* The following code is equivalent to the assembly above

    byte const * s1 = (byte *)src1;
    byte const * s2 = (byte *)src2;

    for (; len > 0; ++s1, ++s2, --len)
        if (*s1 != *s2)
            return false;

    return true; //*/
}

comp_t memcmp(void const * src1, void const * src2, size_t len)
{
    if (src1 == src2)
        return 0;

    comp_t ret = 0;

    asm volatile ( "repe cmpsb \n\t"
                   "sete %%al  \n\t"
                 : "+D" (src1), "+S" (src2), "+c" (len), "=a"(ret)
                 :
                 : "memory" );

    //msg("!! AFTER memcmp: DI = %Xp (%X1), SI = %Xp (%X1), C = %Xs, A = %s8 !!", src1, *((byte *)src1), src2, *((byte *)src2), len, ret);

    return ret;

    /* The following code is equivalent to the assembly above
    byte const * s1 = (byte *)src1;
    byte const * s2 = (byte *)src2;

    sbyte res = 0;    //  Used to store subtraction/comparison results.

    for (; len > 0; ++s1, ++s2, --len)
        if ((res = (sbyte)(*s1) - (sbyte)(*s2)) != 0)
            return (comp_t)res;

    return 0; //*/
}

void * memcpy(void * dst, void const * src, size_t len)
{
    void * ret = dst;

    if (src != dst)
    {
        asm volatile ( "rep movsb \n\t"
                     : "+D"(dst), "+S"(src), "+c"(len)
                     : : "memory" );
    }

    return ret;

    /* The following code is equivalent to the assembly above
    if (src == dst)
        return dst;

          byte * d = (byte *)dst;
    byte const * s = (byte *)src;

    for (; len > 0; ++d, ++s, --len)
        *d = *s;

    return dst; //*/
}

void * memmove(void * dst, void const * src, size_t len)
{
    void * ret = dst;

    if (src > dst)
    {
        //  Loop forward.

        asm volatile ( "rep movsb \n\t"
                       : "+D"(dst), "+S"(src), "+c"(len)
                       : : "memory" );
    }
    else if (src < dst)
    {
        //  Loop backward.

        dst += len;
        src += len;

        asm volatile ( "std       \n\t"
                       "rep movsb \n\t"
                       "cld       \n\t"
                       : "+D"(dst), "+S"(src), "+c"(len)
                       : : "memory" );
    }

    return ret;

    /* The following code is equivalent to the code & assembly above
    if (src == dst)
        return dst;

          byte * d = (byte *)dst;
    byte const * s = (byte *)src;

    if (dst < src)
        for (; len > 0; ++d, ++s, --len)
            *d = *s;
    else
    {
        dst += len;
        src += len;

        for (; i < len; --d, --s, --len)
            *d = *s;
    }

    return dst; //*/
}

void * mempcpy(void * dst, void const * src, size_t len)
{
    if (src != dst)
    {
        asm volatile ( "rep movsb \n\t"
                     : "+D"(dst), "+S"(src), "+c"(len)
                     : : "memory" );
    }

    return dst;
}

void * mempmove(void * dst, void const * src, size_t len)
{
    if (src > dst)
    {
        //  Loop forward.

        asm volatile ( "rep movsb \n\t"
                       : "+D"(dst), "+S"(src), "+c"(len)
                       : : "memory" );

        return dst;
    }
    else if (src < dst)
    {
        //  Loop backward.

        dst += len;
        src += len;

        void * ret = dst;

        asm volatile ( "std       \n\t"
                       "rep movsb \n\t"
                       "cld       \n\t"
                       : "+D"(dst), "+S"(src), "+c"(len)
                       : : "memory" );

        return ret;
    }

    return dst + len;
}

void * memset(void * dst, int const val, size_t len)
{
    void * ret = dst;

    asm volatile ( "rep stosb \n\t"
                 : "+D" (dst), "+c" (len)
                 : "a" (val)
                 : "memory" );

    /* The following code is equivalent to the assembly above
          byte * d = (byte *)dst;
    byte const   v = (byte  )val;

    for (; len > 0; ++d, --len)
        *d = v; //*/

    return ret;
}

void * mempset(void * dst, int const val, size_t len)
{
    asm volatile ( "rep stosb \n\t"
                 : "+D" (dst), "+c" (len)
                 : "a" (val)
                 : "memory" );

    return dst;
}

void * memset16(void * dst, int const val, size_t cnt)
{
    void * ret = dst;

    asm volatile ( "rep stosw \n\t"
                 : "+D" (dst), "+c" (cnt)
                 : "a" (val)
                 : "memory" );

    return ret;
}

void * mempset16(void * dst, int const val, size_t cnt)
{
    asm volatile ( "rep stosw \n\t"
                 : "+D" (dst), "+c" (cnt)
                 : "a" (val)
                 : "memory" );

    return dst;
}

void * memset32(void * dst, long const val, size_t cnt)
{
    void * ret = dst;

    asm volatile ( "rep stosl \n\t"
                 : "+D" (dst), "+c" (cnt)
                 : "a" (val)
                 : "memory" );

    return ret;
}

void * mempset32(void * dst, long const val, size_t cnt)
{
    asm volatile ( "rep stosl \n\t"
                 : "+D" (dst), "+c" (cnt)
                 : "a" (val)
                 : "memory" );

    return dst;
}

size_t strlen(char const * str)
{
#if   defined(__BEELZEBUB__ARCH_AMD64)
    register size_t ret  asm("rcx") = ~((size_t)0);
    register size_t null asm("rax") =   (size_t)0;
#else
    register size_t ret  asm("ecx") = ~((size_t)0);
    register size_t null asm("eax") =   (size_t)0;
#endif

    asm volatile ( "repne scasb \n\t"
                 : "+D"(str), "+c"(ret)
                 : "a"(null)
                 : "memory" );
    //  `ret` started at -1, and is decremented once for every character
    //  encountered up to 0 inclusively. Therefore, `ret` will contain
    //  `-length - 2` after the assembly block. `~ret` flips all the bits
    //  so the value becomes `length + 2 - 1` (`length + 1`), so the `- 1`
    //  brings it to the desired result.

    return ~ret - 1;
}

size_t strnlen(char const * str, size_t len)
{
    if unlikely(len == 0)
        return 0;
    //  GCC seems to be nice and treat this as an unlikely case. It XORs
    //  (E|R)AX with itself, tests the length, and if it's 0, it jumps to
    //  the `ret` statement at the end. Neat-o.

#if   defined(__BEELZEBUB__ARCH_AMD64)
    register size_t ret  asm("rcx") = len;
    register size_t null asm("rax") = (size_t)0;
#else
    register size_t ret  asm("ecx") = len;
    register size_t null asm("eax") = (size_t)0;
#endif

    len = (size_t)(uintptr_t)str;

    asm volatile ( "repne scasb \n\t"
                 : "+D"(str), "+c"(ret)
                 : "a"(null)
                 : "memory" );

    null = (size_t)(uintptr_t)str - len;
    //  Why null? Because it's the register which contains the return value!

    if (*(str - 1) == 0)
        --null;
    //  If the last character checked (before incrementing (E|R)DI) is null,
    //  it means the string's actual length is <= `len`, so the result is 1
    //  unit higher than it should be.

    return null;
}
