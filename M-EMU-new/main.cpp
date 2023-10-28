#include "unit_test.h"
#include "interface/Platform.h"
#include "misc/status_enum.h"
#include "misc/obj_factory.h"

int main() {
    test();

    Platform_I* platform = Platform_GetInstance({ .desc_str = "default_platform" });
//
    FuncReturnFeedback_e feedback = MEMU_OK;
    feedback = platform->Init_Platform_API();
    assert(feedback == MEMU_OK);
    //
//    do {
        feedback = platform->Run_Platform_API();
//    } while (feedback == MEMU_OK);

    return 0;
}
