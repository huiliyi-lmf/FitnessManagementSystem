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
* ��������������Ķ����
* @param structName �ṹ������
* @param byWhat ����ʲô������
* @param whichProperty �ڽṹ���е���������ʲô
* @param whichExternFlag �ĸ�extern�����洢���Ƚ�ֵ
* @param equalMacro ��Ⱥ꣬������ֵ����ȹ�ϵ��ʲô
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
* �������������������Ķ����
* @param structName �ṹ������
* @param byWhat ����ʲô������
* @param whichProperty �ڽṹ���е���������ʲô
*/
#define GENERATE_INT_FILTER_DEFINITION(structName, byWhat, whichProperty) \
GENERATE_FILTER_DEFINITION(structName, byWhat, whichProperty, CURRENT_INT_VAL_FILTER_FLAG, BASE_EQUAL_MACRO)

/**
* �����ַ��������������Ķ����
* @param structName �ṹ������
* @param byWhat ����ʲô������
* @param whichProperty �ڽṹ���е���������ʲô
*/
#define GENERATE_STR_FILTER_DEFINITION(structName, byWhat, whichProperty) \
GENERATE_FILTER_DEFINITION(structName, byWhat, whichProperty, CURRENT_STR_VAL_FILTER_FLAG, STR_EQUAL_MACRO)

#define GENERATE_SUBSTR_FILTER_DEFINITION(structName, byWhat, whichProperty) \
GENERATE_FILTER_DEFINITION(structName, byWhat, whichProperty, CURRENT_STR_VAL_FILTER_FLAG, STR_CONTAIN_MACRO)

// ����CourseTypeIdɸѡCourse����
GENERATE_INT_FILTER_DEFINITION(Course, CourseTypeId, type_id);

// ����CourseTypeIdɸѡCoachTypeRel����
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