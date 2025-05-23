#ifndef __FILTER_H
#define __FILTER_H

#include <stdbool.h>

// 过滤器标志位
extern char* FILTER_STR_FLAG;
extern int FILTER_INT_FLAG;

// 设置过滤器标志位
void setFilterStrFlag(char* str);
void setFilterIntFlag(int i);

// 清除过滤器标志位
void clearFilterStrFlag();
void clearFilterIntFlag();

// 过滤器函数
void* filterAdminByUsername(void* data);
void* filterCoachByName(void* data);
void* filterCoachByExpertiseStatus(void* data);
void* filterCourseTypeByName(void* data);
void* filterCoachTypeRelByCoachId(void* data);
void* filterCoachTypeRelByTypeId(void* data);
void* filterUserByPhone(void* data);
void* filterCourseByName(void* data);
void* filterCourseByTypeId(void* data);
void* filterCoachCourseRelByCoachId(void* data);
void* filterCoachCourseRelByCourseId(void* data);
void* filterUserCourseRelByUserId(void* data);
void* filterUserCourseRelByCourseId(void* data);

#endif // !__FILTER_H