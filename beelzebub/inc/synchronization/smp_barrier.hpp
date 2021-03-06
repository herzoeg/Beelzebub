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

#pragma once

#include <synchronization/atomic.hpp>
#include <system/cpu.hpp>
#include <system/cpu_instructions.hpp>

namespace Beelzebub { namespace Synchronization
{
	//	The first version (on non-SMP builds) is dumbed down.

	struct SmpBarrier
	{
    public:

        /*  Constructor(s)  */

        SmpBarrier() = default;
        SmpBarrier(SmpBarrier const &) = delete;
        SmpBarrier & operator =(SmpBarrier const &) = delete;

#if   defined(__BEELZEBUB_SETTINGS_NO_SMP)

        /*  Operations  */

        /**
         *  Resets the barrier, so it will open when the specified number
         *  of cores reach it.
         */
        __forceinline void Reset(size_t const coreCount = 1) volatile
        {
            this->Value = 1;
        }

        /**
         *  Reach the barrier, awaiting for other cores if necessary.
         */
        __forceinline void Reach() volatile
        {
            this->Value = 0;
        }

        /*  Properties  */

        /**
         *  Checks whether the spinlock is free or not.
         */
        __forceinline __must_check bool IsOpen() const volatile
        {
            return this->Value == (size_t)0;
        }

        __forceinline size_t GetValue() const volatile
        {
            return this->Value;
        }

        /*  Fields  */

    private:

        size_t volatile Value;

#else

        /*  Operations  */

        /**
         *  Resets the barrier, so it will open when all cores reach it.
         */
        __forceinline void Reset() volatile
        {
            this->Value.Store(System::Cpu::Count);
        }

        /**
         *  Resets the barrier, so it will open when the specified number
         *  of cores reach it.
         */
        __forceinline void Reset(size_t const coreCount) volatile
        {
            this->Value.Store(coreCount);
        }

        /**
         *  Reach the barrier, awaiting for other cores if necessary.
         */
        inline void Reach() volatile
        {
            --this->Value;

            while (this->Value)
            	System::CpuInstructions::DoNothing();
        }

        /*  Properties  */

        /**
         *  Checks whether the spinlock is free or not.
         */
        __forceinline __must_check bool IsOpen() const volatile
        {
            return this->Value == (size_t)0;
        }

        __forceinline size_t GetValue() const volatile
        {
            return this->Value.Load();
        }

        /*  Fields  */

    private:

        Atomic<size_t> volatile Value; 
#endif
	};
}}
