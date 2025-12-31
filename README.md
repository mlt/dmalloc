Debug Malloc Library (modified)
===============================
[![CI](https://github.com/mlt/dmalloc/actions/workflows/ci.yml/badge.svg)](https://github.com/mlt/dmalloc/actions/workflows/ci.yml)

This is a specialized version of dmalloc. Original code can be found at
https://dmalloc.com/ .

About this fork
---------------
This is an effort to enhance dmalloc experience on Windows with Microsoft
compiler. Besides the standard practice of including `dmalloc.h` and linking
against the library, the shared library build leverages [Microsoft Detours
library](https://github.com/microsoft/Detours/) to hook memory allocation
functions. Along with withdll.exe, this gives an experience very close to that
of using dmalloc with LD_PRELOAD and unmodified executable.

### Synopsis
Consider the following `test.c` code below. Compile it with `cl -MD -Zi test.c`.
Note that **both flags are a must**: `-MD` makes use of DLL runtime that can be
hooked with Detours and `-Zi` generates PDB program database with symbols to look
up from addresses using DbgHelp.

```c
#include <stdlib.h>
int main() {
  free(NULL);
  *(int *)NULL = 0;
  return 0;
}
```

Set dmalloc tokens `dmalloc -l logfile -c -p error-free-null -p catch-signals -b`  
with `export DMALLOC_OPTIONS=debug=0x10020000,log=logfile` if you are using
MSYS2 bash. Finally, you can run `withdll.exe -d:dmalloc_detours.dll test.exe`. You
should see `logfile.sym` file:

```log
1768197062: 11: Dmalloc version '5.6.5' from 'https://dmalloc.com/'
1768197062: 11: flags = 0x10020000, logfile 'logfile'
1768197062: 11: interval = 0, addr = 0x0, seen # = 0, limit = 0
1768197062: 11: starting time = 1768197062
1768197062: 11: process pid = 8500
1768197062: 11: WARNING: tried to free(0) from 'D:\a\dmalloc\dmalloc\.libs\test.c:4'
1768197062: 11:   error details: invalid 0L pointer
1768197062: 11:   from 'D:\a\dmalloc\dmalloc\.libs\test.c:4' prev access 'unknown'
1768197062: 11: ERROR: free: pointer is null (err 20)
1768197062: 11: caught unhandled exception 0xc0000005 from 'D:\a\dmalloc\dmalloc\.libs\test.c:4'
1768197062: 21: ending time = 1768197062, elapsed since start = 0:00:00
```

Note that both errors point to the same line number. That is because return
address is the next instructions whereas exception information has the address
of the faulty instruction.

### Internals
This is achieved in two steps:
1) GET_RET_ADDR macro uses
   [_AddressOfReturnAddress()](https://learn.microsoft.com/en-us/cpp/intrinsics/addressofreturnaddress)
   cl compiler intrinsic to get the return address without relying on frame
   pointers as commonly found on other platforms.
2) [The debug help library,
   DbgHelp](https://learn.microsoft.com/en-us/windows/win32/debug/debug-help-library),
   is used at exit to re-parse logfile and replace addresses with file name and
   line number information.

### Limitations
- Attempt to use DbgHelp on-the-go makes dmalloc go recursive:( TODOs:
  - It might worth a try to use noinst_LIBRARY with -MT flag while linking with
    DbgHelp.
  - or patch withdll to set `DEBUG_PROCESS` _dwCreationFlags_ while calling
    `DetourCreateProcessWithDllsA` to become a debugger for said process and use
    DbgHelp in unaltered environment. But this way we will lose general
    debugging capability.
- You would want to have PDB program database with symbols (`-Zi` flag for
  compiler and `-DEBUG` for link.exe, or, alternatively,
  `program_LDFLAGS=-Wl,-Xlinker,-DEBUG,-Xlinker,-PDB:your_exe.pdb` if you are
  using autotools) besides using `MD` flag
- You may want to consider disabling address randomization with either
  [`-DYNAMICBASE:NO` link.exe
  option](https://learn.microsoft.com/en-us/cpp/build/reference/dynamicbase-use-address-space-layout-randomization)
  or corresponding [EDITBIN
  option](https://learn.microsoft.com/en-us/cpp/build/reference/dynamicbase) if
  for some reason you prefer to use actual address instead of source file line
  numbers.

How to build
------------
You will need Visual Studio, vcpkg, and MSYS2 installed. I prefer UCRT64
environment for MSYS2. Install Detours with `vcpkg install detours` and make
sure you have `VCPKG_ROOT` environment variable defined as it is used in
`configure.ac`.

Have a bash script `conf.sh` akin to
```sh
#!/bin/sh

export WindowsSdkDir="C:\\Program Files (x86)\\Windows Kits\\10\\"
export WindowsSDKLibVersion="10.0.26100.0\\"
VCROOT="C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.44.35207"
SDKINC="$WindowsSdkDir\\include\\$WindowsSDKLibVersion"
SDKLIB="$WindowsSdkDir\\lib\\$WindowsSDKLibVersion"
export INCLUDE="$VCROOT\\include;$SDKINC\\ucrt;$SDKINC\\um;$SDKINC\\shared"
export Platform=x64
export LIB="$VCROOT\\lib\\$Platform;$SDKLIB\\ucrt\\$Platform;$SDKLIB\\um\\$Platform"
if ! command -v cl > /dev/null ; then
  echo "Adding cl and mt to path"
  export PATH="$VCROOT\\bin\\Host$Platform\\$Platform:$WindowsSdkDir\\bin\\$WindowsSDKLibVersion\\$Platform:$PATH"
fi
export CC="cl -nologo"
export CXX=$CC
export CPPFLAGS="-we4274 -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -Zi -W4 -we4996 -we4311 -we4022 -we4312 -we4365 -wd4018 -wd4245 -wd4389 -wd4100 -wd4127 -wd4152 -wd4210 -wd4459 -wd4232 -wd4267 -we4005"

../configure -C ac_cv_prog_cc_g=no && make install DESTDIR=$PWD/stage
```
that sets PATH to cl compiler along with some necessary environment variables. Save it in a sub-directory to be a build root. Run it as `. ./conf.sh`. Note the standalone dot to souce in environment variables that you might use later. You might want to use `autoreconf -fi` the very first time if you are missing `configure`.

### withdll
For whatever reason [vcpkg port for detours](https://vcpkg.link/ports/detours) does not offer tools feature. So
you'd need to open up x64 Native Tools command prompt and `cd
%VCPKG_ROOT%\buildtrees\detours\x64-windows-rel\samples`. Then you'll need to cd
into `syelog` and `withdll` and run `nmake` in both directories. Your final
executable should be in
`$%VCPKG_ROOT%$\buildtrees\detours\x64-windows-rel\bin.X64`.
