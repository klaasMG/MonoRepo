#pragma once
#include "GMAKE_EXCEPTION.h"

namespace gmake {
    inline bool debug = false;
    inline GMAKE_EXCEPTION ExceptionHandler = GMAKE_EXCEPTION{debug};
}
