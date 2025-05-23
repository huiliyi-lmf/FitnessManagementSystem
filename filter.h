#ifndef __FILTER_H
#define __FILTER_H

#include "structs.h"
#include <malloc.h>

// �˴���������������ѯʹ�õ�filter

void setFilterIntFlag(int val);
void clearFilterIntFlag();
void setFilterStrFlag(char* val);
void clearFilterStrFlag();

#define GENERATE_FILTER_DECLARATION(structName, byWhat) \
void* filter##structName##By##byWhat##(void* header)

// ����CourseTypeIdɸѡCourse����
GENERATE_FILTER_DECLARATION(Course, CourseTypeId);

// ����CourseTypeIdɸѡCoachTypeRel����
GENERATE_FILTER_DECLARATION(CoachTypeRel, CourseTypeId);

GENERATE_FILTER_DECLARATION(Admin, Username);

GENERATE_FILTER_DECLARATION(CourseType, Name);

GENERATE_FILTER_DECLARATION(Coach, Name);

GENERATE_FILTER_DECLARATION(CoachCourseRel, CoachId);

GENERATE_FILTER_DECLARATION(CoachCourseRel, CourseId);

GENERATE_FILTER_DECLARATION(CoachTypeRel, CoachId);

GENERATE_FILTER_DECLARATION(Coach, ExpertiseStatus);

GENERATE_FILTER_DECLARATION(User, NameContains);

GENERATE_FILTER_DECLARATION(UserCourseRel, UserId);

GENERATE_FILTER_DECLARATION(UserCourseRel, CourseId);

GENERATE_FILTER_DECLARATION(CourseType, NameContains);

GENERATE_FILTER_DECLARATION(User, Phone);

#endif // !__FILTER_H
