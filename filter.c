#include "filter.h"

int CURRENT_INT_VAL_FILTER_FLAG = -1;
char* CURRENT_STR_VAL_FILTER_FLAG = -1;

void setFilterIntFlag(int val) {
    CURRENT_INT_VAL_FILTER_FLAG = val;
}

void clearFilterIntFlag() {
    CURRENT_INT_VAL_FILTER_FLAG = -1;
}

void setFilterStrFlag(char* val) {
    CURRENT_STR_VAL_FILTER_FLAG = val;
}

void clearFilterStrFlag() {
    CURRENT_STR_VAL_FILTER_FLAG = -1;
}

#define BASE_EQUAL_MACRO(t1, t2) (t1 == t2)
#define STR_EQUAL_MACRO(s1, s2) (!strcmp(s1, s2))
#define STR_CONTAIN_MACRO(s1, s2) (strstr(s1, s2) != NULL)

/**
* 定义过滤器函数的定义宏
* @param structName 结构体名字
* @param byWhat 根据什么键过滤
* @param whichProperty 在结构体中的属性名是什么
* @param whichExternFlag 哪个extern变量存储待比较值
* @param equalMacro 相等宏，这两个值的相等关系是什么
*/
#define GENERATE_FILTER_DEFINITION(structName, byWhat, whichProperty, whichExternFlag, equalMacro) \
void* filter##structName##By##byWhat##(void* header) {  \
    if (##whichExternFlag## == -1) return header;   \
    structName* newHeader = NULL;   \
    structName* current = NULL;     \
    structName* oldHeader = (structName*)header;    \
    while (oldHeader != NULL) {     \
        if (##equalMacro##(oldHeader->##whichProperty##, ##whichExternFlag##)) {    \
            newHeader = (##structName##*)malloc(sizeof(##structName##));    \
            if (newHeader == NULL) {    \
                return NULL;    \
            }   \
            memcpy(newHeader, oldHeader, sizeof(##structName##));   \
            newHeader->nextPointer = NULL;  \
            current = newHeader;    \
            oldHeader = oldHeader->nextPointer;     \
            break;  \
        }   \
        oldHeader = oldHeader->nextPointer;     \
    }   \
    while (oldHeader != NULL) {     \
        if (##equalMacro##(oldHeader->##whichProperty##, ##whichExternFlag##)) {    \
            current->nextPointer = (##structName##*)malloc(sizeof(##structName##));     \
            if (current->nextPointer == NULL) {     \
                return NULL;        \
            }   \
            memcpy(current->nextPointer, oldHeader, sizeof(##structName##));    \
            current = current->nextPointer;     \
            current->nextPointer = NULL;    \
        }   \
        oldHeader = oldHeader->nextPointer;     \
    }   \
    free##structName##List((##structName##*)header);        \
    return (void*)newHeader;   \
}

/**
* 定义整数过滤器函数的定义宏
* @param structName 结构体名字
* @param byWhat 根据什么键过滤
* @param whichProperty 在结构体中的属性名是什么
*/
#define GENERATE_INT_FILTER_DEFINITION(structName, byWhat, whichProperty) \
GENERATE_FILTER_DEFINITION(structName, byWhat, whichProperty, CURRENT_INT_VAL_FILTER_FLAG, BASE_EQUAL_MACRO)

/**
* 定义字符串过滤器函数的定义宏
* @param structName 结构体名字
* @param byWhat 根据什么键过滤
* @param whichProperty 在结构体中的属性名是什么
*/
#define GENERATE_STR_FILTER_DEFINITION(structName, byWhat, whichProperty) \
GENERATE_FILTER_DEFINITION(structName, byWhat, whichProperty, CURRENT_STR_VAL_FILTER_FLAG, STR_EQUAL_MACRO)

#define GENERATE_SUBSTR_FILTER_DEFINITION(structName, byWhat, whichProperty) \
GENERATE_FILTER_DEFINITION(structName, byWhat, whichProperty, CURRENT_STR_VAL_FILTER_FLAG, STR_CONTAIN_MACRO)

// 根据CourseTypeId筛选Course链表
GENERATE_INT_FILTER_DEFINITION(Course, CourseTypeId, type_id);

// 根据CourseTypeId筛选CoachTypeRel链表
GENERATE_INT_FILTER_DEFINITION(CoachTypeRel, CourseTypeId, type_id);

GENERATE_STR_FILTER_DEFINITION(Admin, Username, username);

GENERATE_STR_FILTER_DEFINITION(CourseType, Name, name);

GENERATE_STR_FILTER_DEFINITION(Coach, Name, name);

GENERATE_INT_FILTER_DEFINITION(CoachCourseRel, CoachId, coachId);

GENERATE_INT_FILTER_DEFINITION(CoachCourseRel, CourseId, courseId);

GENERATE_INT_FILTER_DEFINITION(CoachTypeRel, CoachId, coach_id);

GENERATE_INT_FILTER_DEFINITION(Coach, ExpertiseStatus, expertise_status);

GENERATE_SUBSTR_FILTER_DEFINITION(User, NameContains, name);

GENERATE_INT_FILTER_DEFINITION(UserCourseRel, UserId, user_id);

GENERATE_INT_FILTER_DEFINITION(UserCourseRel, CourseId, course_id);

GENERATE_SUBSTR_FILTER_DEFINITION(CourseType, NameContains, name);

GENERATE_STR_FILTER_DEFINITION(User, Phone, phone);