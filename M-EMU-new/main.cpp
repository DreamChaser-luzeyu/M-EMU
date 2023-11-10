#include "unit_test.h"
#include "interface/Platform.h"
#include "misc/status_enum.h"
#include "misc/obj_factory.h"
#include "sdk/test/test.h"

int main() {
#ifdef CONFIG_TEST_ENABLE
    // ----- Test
//    RUN_TEST_UNIT(Main_Test);
    RUN_TEST_UNIT(GDB_RSP_Test);
#endif

    // -----
    Platform_I* platform = Platform_GetInstance({ .desc_str = "default_platform" });

    FuncReturnFeedback_e feedback = MEMU_OK;
    feedback = platform->Init_Platform_API();
    assert(feedback == MEMU_OK);
    feedback = platform->Run_Platform_API();

    return 0;
}
