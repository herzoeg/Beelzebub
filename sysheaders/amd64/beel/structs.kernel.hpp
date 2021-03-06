/*
    Copyright (c) 2016 Alexandru-Mihai Maftei. All rights reserved.


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

#pragma once

#include <metaprogramming.h>

#ifdef __BEELZEBUB_KERNEL
    #define MONIKER(NAME) MCATS(NAME, Base)
#else
    #define MONIKER(NAME) NAME
#endif

namespace Beelzebub
{
    /**
     *  A unit of isolation.
     */
    class MONIKER(Process)
    {
        //  So far, nothing.
    };

    /**
     *  A unit of execution.
     */
    class MONIKER(Thread)
    {
    protected:
        /*  Constructor(s)  */

        inline MONIKER(Thread)(MONIKER(Process) * owner)
            : Owner( owner)
        {
            //  Nothing else.
        }

    public:
        /*  Hierarchy  */

        MONIKER(Process) * const Owner;
    };

    struct SyscallRegisters64
    {
        uint64_t R15;
        uint64_t R14;
        uint64_t R13;
        uint64_t R12;
        uint64_t RFLAGS;    //  R11
        uint64_t R10;
        uint64_t R9;
        uint64_t R8;
        uint64_t RSI;
        uint64_t RDI;
        uint64_t RSP;
        uint64_t RBP;
        uint64_t RDX;
        uint64_t RIP;       //  RCX
        uint64_t RBX;
        uint64_t RAX;
    };

    struct SyscallRegisters32
    {
        uint32_t ESI;
        uint32_t EDI;
        uint32_t ESP;
        uint32_t EBP;
        uint32_t EDX;
        uint32_t ECX;
        uint32_t EBX;
        uint32_t EAX;
    };

    /**
     *  The data available to an individual CPU core.
     */
    struct MONIKER(CpuData)
    {
        MONIKER(CpuData) * SelfPointer;
        size_t Index;

        uintptr_t SyscallStack;
        SyscallRegisters64 SyscallRegisters;

        MONIKER(Thread) * ActiveThread;
        MONIKER(Process) * ActiveProcess;
    };
}

#undef MONIKER
