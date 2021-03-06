/*
    Copyright (c) 2015 Alexandru-Mihai Maftei

    Permission to use, copy, modify, and/or distribute this software for any purpose
    with or without fee is hereby granted, provided that the above copyright notice
    and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
    REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
    INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
    OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
    THIS SOFTWARE.

    ---

    You may also find the text of this license in "LICENSE.md", along with a more
    thorough explanation regarding other files.

    ---

    The feature names here aren't copyrighted by me at least.
*/

//  This file enlists various features identified by CPUID in macros that are
//  expected in the includer.

CPUID_FEATURE(FPU                         ,  0,  0, FPU                         )
CPUID_FEATURE(VME                         ,  0,  1, VME                         )
CPUID_FEATURE(DE                          ,  0,  2, DE                          )
CPUID_FEATURE(PSE                         ,  0,  3, PSE                         )
CPUID_FEATURE(TSC                         ,  0,  4, TSC                         )
CPUID_FEATURE(MSR                         ,  0,  5, MSR                         )
CPUID_FEATURE(PAE                         ,  0,  6, PAE                         )
CPUID_FEATURE(MCE                         ,  0,  7, MCE                         )
CPUID_FEATURE(CX8                         ,  0,  8, CX8                         )
CPUID_FEATURE(APIC                        ,  0,  9, APIC                        )
CPUID_FEATURE(SEP                         ,  0, 11, SEP                         )
CPUID_FEATURE(MTRR                        ,  0, 12, MTRR                        )
CPUID_FEATURE(PGE                         ,  0, 13, PGE                         )
CPUID_FEATURE(MCA                         ,  0, 14, MCA                         )
CPUID_FEATURE(CMOV                        ,  0, 15, CMOV                        )
CPUID_FEATURE(PAT                         ,  0, 16, PAT                         )
CPUID_FEATURE(PSE36                       ,  0, 17, PSE36                       )
CPUID_FEATURE(PSN                         ,  0, 18, PSN                         )
CPUID_FEATURE(CLFSH                       ,  0, 19, CLFSH                       )
CPUID_FEATURE(DS                          ,  0, 21, DS                          )
CPUID_FEATURE(ACPI_Thermal_Monitor        ,  0, 22, ACPI_Thermal_Monitor        )
CPUID_FEATURE(MMX                         ,  0, 23, MMX                         )
CPUID_FEATURE(FXSR                        ,  0, 24, FXSR                        )
CPUID_FEATURE(SSE                         ,  0, 25, SSE                         )
CPUID_FEATURE(SSE2                        ,  0, 26, SSE2                        )
CPUID_FEATURE(SS                          ,  0, 27, SS                          )
CPUID_FEATURE(HTT                         ,  0, 28, HTT                         )
CPUID_FEATURE(TM                          ,  0, 29, TM                          )
CPUID_FEATURE(PBE                         ,  0, 31, PBE                         )
CPUID_FEATURE(SSE3                        ,  1,  0, SSE3                        )
CPUID_FEATURE(PCLMULQDQ                   ,  1,  1, PCLMULQDQ                   )
CPUID_FEATURE(DTES64                      ,  1,  2, DTES64                      )
CPUID_FEATURE(MONITOR                     ,  1,  3, MONITOR                     )
CPUID_FEATURE(DS_CPL                      ,  1,  4, DS_CPL                      )
CPUID_FEATURE(VMX                         ,  1,  5, VMX                         )
CPUID_FEATURE(SMX                         ,  1,  6, SMX                         )
CPUID_FEATURE(EST                         ,  1,  7, EST                         )
CPUID_FEATURE(TM2                         ,  1,  8, TM2                         )
CPUID_FEATURE(SSSE3                       ,  1,  9, SSSE3                       )
CPUID_FEATURE(CNXT_ID                     ,  1, 10, CNXT_ID                     )
CPUID_FEATURE(SDBG                        ,  1, 11, SDBG                        )
CPUID_FEATURE(FMA                         ,  1, 12, FMA                         )
CPUID_FEATURE(CMPXCHG16B                  ,  1, 13, CMPXCHG16B                  )
CPUID_FEATURE(xTPR_Update_Control         ,  1, 14, xTPR_Update_Control         )
CPUID_FEATURE(PDCM                        ,  1, 15, PDCM                        )
CPUID_FEATURE(PCID                        ,  1, 17, PCID                        )
CPUID_FEATURE(DCA                         ,  1, 18, DCA                         )
CPUID_FEATURE(SSE41                       ,  1, 19, SSE41                       )
CPUID_FEATURE(SSE42                       ,  1, 20, SSE42                       )
CPUID_FEATURE(x2APIC                      ,  1, 21, x2APIC                      )
CPUID_FEATURE(MOVBE                       ,  1, 22, MOVBE                       )
CPUID_FEATURE(POPCNT                      ,  1, 23, POPCNT                      )
CPUID_FEATURE(TscDeadline                 ,  1, 24, TSC-Deadline                )
CPUID_FEATURE(AES                         ,  1, 25, AES                         )
CPUID_FEATURE(XSAVE                       ,  1, 26, XSAVE                       )
CPUID_FEATURE(OSXSAVE                     ,  1, 27, OSXSAVE                     )
CPUID_FEATURE(AVX                         ,  1, 28, AVX                         )
CPUID_FEATURE(F16C                        ,  1, 29, F16C                        )
CPUID_FEATURE(RDRAND                      ,  1, 30, RDRAND                      )
CPUID_FEATURE(SMEP                        , 99, 99, SMEP                        )  //  TODO
CPUID_FEATURE(SMAP                        , 99, 99, SMAP                        )  //  TODO
CPUID_FEATURE(SyscallSysret               ,  2, 11, SyscallSysret               )
CPUID_FEATURE(NX                          ,  2, 20, NX                          )
CPUID_FEATURE(Page1GB                     ,  2, 26, Page1GB                     )
CPUID_FEATURE(RDTSP                       ,  2, 27, RDTSP                       )
CPUID_FEATURE(LM                          ,  2, 29, LM                          )

/*
 *  Register indexes:
 *       0: 0x00000001 EDX
 *       1: 0x00000001 ECX
 *       2: 0x80000001 EDX
 *
 *      99:--PLACEHOLDER--
 */
