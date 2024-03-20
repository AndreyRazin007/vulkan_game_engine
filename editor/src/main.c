#include "core/logger.h"
#include "core/asserts.h"

#include "platform/platform.h"

int main(void) {
    FFATAL("A test message: %f", 3.14f);
    FERROR("A test message: %f", 3.14f);
    FWARNING("A test message: %f", 3.14f);
    FINFO("A test message: %f", 3.14f);
    FDEBUG("A test message: %f", 3.14f);
    FTRACE("A test message: %f", 3.14f);

    platformState state;

    if (platformStartup(&state, "Freak Engine", 100, 100, 1280, 720)) {
        while (TRUE) {
            platformPumpMessages(&state);
        }
    }

    platformShutdown(&state);

    return 0;
}
