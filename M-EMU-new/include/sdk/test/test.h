#pragma once

/**
 * NOTE: Please only include this header file from your test case document!
 *       Your test case will not run if you define your test case, include
 *       this header file from other document, and call RunAllTest_Test(),
 *       because all symbols defined here are marked static.
 */

#include <iostream>
#include <cassert>

#include "sdk/console.h"

typedef struct TestCase_Desc {
    const char* name;
    void (*func)(void);
    const char* file;
    int line;
    struct TestCase_Desc* next;
} TestCase_Desc_t;

static TestCase_Desc_t* TestCaseList_Head = nullptr;
static TestCase_Desc_t* TestCaseList_Tail = nullptr;

static void register_test(TestCase_Desc* desc) {
    if(TestCaseList_Tail && TestCaseList_Head) {
        TestCaseList_Tail->next = desc;
        TestCaseList_Tail = desc;
    }
    else {
        TestCaseList_Head = desc;
        TestCaseList_Tail = desc;
    }
}

/**
 * NOTE: Has to be called in the same document!
 *       If you want to call this function at some other place, you could define & export
 *       another function calling this function, and call the function you defined at some
 *       other document.
 */
static void RunAllTest_Test() {
    if(!TestCaseList_Tail) assert(0);
    if(!TestCaseList_Head) assert(0);
    for(TestCase_Desc_t* ptr = TestCaseList_Head; ptr; ptr = ptr->next) {
        std::cout << STYLE_BKG_YELLOW << "[TEST ] Case " << ptr->name << " running..." << STYLE_RST << std::endl;
        ptr->func();
        std::cout << STYLE_BKG_GREEN << "[TEST ] Case " << ptr->name << " passed!" << STYLE_RST << std::endl;
    }
    std::cout << STYLE_BKG_GREEN << "[TEST ] All case passed!" << STYLE_RST << std::endl;
}

#ifdef __GNUC__
#define TEST_CASE(case_name, case_desc)                                  \
static void case_name(void);                                             \
static TestCase_Desc_t case_name##_desc = {                              \
    .name = case_desc,                                                   \
    .func = case_name,                                                   \
    .next = nullptr                                                      \
};                                                                       \
                                                                         \
static void __attribute__((constructor(101))) case_name##_helper() {     \
    register_test(&case_name##_desc);                                    \
}                                                                        \
static void case_name(void)
#endif

