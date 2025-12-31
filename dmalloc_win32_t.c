#include <stdlib.h>
#include "conf.h"

#include "append.h"
#include "debug_tok.h"
#include "error_val.h"

/* We do not want macros as we want to test whether MS Detours hooks are set */
#define DMALLOC_DISABLE
#include "dmalloc.h"

int main(int argc, char **argv) {
  char setup[128];
  int final = 1, prev_errno;

  free(NULL);
  if (dmalloc_errno != DMALLOC_ERROR_NONE) {
    loc_printf("   ERROR: free of 0L returned error.\n");
    prev_errno = dmalloc_errno;
    final = 0;
  }

  if (dmalloc_logpath) {
    (void)loc_snprintf(setup, sizeof(setup), "debug=%#x,log=%s",
                       DMALLOC_DEBUG_ERROR_FREE_NULL, dmalloc_logpath);
  } else {
    (void)loc_snprintf(setup, sizeof(setup), "debug=%#x", DMALLOC_DEBUG_ERROR_FREE_NULL);
  }
  dmalloc_debug_setup(setup);

#ifdef _MSC_VER
  //__debugbreak();
#endif

  free(NULL);
  if (dmalloc_errno == DMALLOC_ERROR_NONE) {
    loc_printf("   ERROR: free of 0L failed to return an error.\n");
    prev_errno = dmalloc_errno;
    final = 0;
  } else {
    dmalloc_errno = DMALLOC_ERROR_NONE;
  }

  if (final != 0) {
    if (dmalloc_errno == DMALLOC_ERROR_NONE) {
      loc_printf("No final dmalloc errno set.\n");
    } else {
      loc_printf("Final dmalloc error: %s (err %d)\n", dmalloc_strerror(dmalloc_errno),
                 dmalloc_errno);
    }
  }
  return 0;
}
