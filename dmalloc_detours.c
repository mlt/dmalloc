#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <dbghelp.h>
#include <detours/detours.h>
#include <stdio.h>
#include <stdlib.h>

#undef DLL_EXPORT
#include "user_malloc_loc.h"

#define FUNCS                                                                            \
  X(free)                                                                                \
  X(malloc)                                                                              \
  X(calloc)                                                                              \
  X(realloc)                                                                             \
  X(strdup)

#define X(x) real_##x,
typedef enum { FUNCS } real_idx;
#undef X
#define X(x) x,
static void *real_addr[] = { FUNCS };
#undef X

void resolve_symbols() {
  char buf[1024], buf2[1024];
  IMAGEHLP_LINE64 Line;
  Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
  //   _dmalloc_reopen_log(); /* close log file */
  const HANDLE hProcess = GetCurrentProcess();
  if (!SymInitialize(hProcess, NULL, TRUE)) {
    DWORD err = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, sizeof(buf), NULL);
    return;
  }
  FILE *ifile = fopen(dmalloc_logpath, "r");
  if (ifile == NULL) {
    goto exit;
  }
  sprintf(buf, "%s.sym", dmalloc_logpath);
  FILE *ofile = fopen(buf, "w");
  if (ofile == NULL) {
    goto exit2;
  }
  while (fgets(buf, sizeof(buf), ifile) != NULL) {
    char *addrstr = strstr(buf, " from 'ra=0x");
    if (addrstr != NULL) {
      addrstr += 7;
      char *endptr;
      uintptr_t addr = _strtoui64(addrstr + 5, &endptr, 16);
      DWORD displacement;
      if (SymGetLineFromAddr64(hProcess, addr, &displacement, &Line)) {
        *addrstr = '\0';
        fputs(buf, ofile);
        snprintf(buf2, sizeof(buf2), "%.*s:%lu%s", MAX_PATH, Line.FileName,
                 Line.LineNumber, endptr);
        fputs(buf2, ofile);
        continue;
      }
    }
    fputs(buf, ofile);
  }
  fclose(ofile);
exit2:
  fclose(ifile);
exit:
  SymCleanup(hProcess);
}

__declspec(dllimport) int auto_shutdown_b;
__declspec(dllimport) char *dmalloc_logpath;

__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason,
                                          LPVOID reserved) {
  if (DetourIsHelperProcess()) {
    return TRUE;
  }

  switch (dwReason) {
  case DLL_PROCESS_ATTACH:
    auto_shutdown_b = 0;

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
#define X(x) DetourAttach(&real_addr[real_##x], our_##x);
    FUNCS
#undef X
    DetourTransactionCommit();
    break;
  case DLL_PROCESS_DETACH:
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
#define X(x) DetourDetach(&real_addr[real_##x], our_##x);
    FUNCS
#undef X
    DetourTransactionCommit();

    dmalloc_shutdown();
    if (dmalloc_logpath)
      resolve_symbols();

    break;
  }
  return TRUE;
}
