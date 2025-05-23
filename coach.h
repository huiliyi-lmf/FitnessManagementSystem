#ifndef __COACH_H
#define __COACH_H

#include "structs.h"
#include "filter.h"

// 当前用户登录的信息
extern int CURRENT_COACH_ID;

/**
 * 登录判断
 * @param name 姓名
 * @param password 密码
 * @return 密码是否正确
 */
bool loginForCoach(char* name, char* password);

/**
 * 修改密码
 * @param oldPassword 旧密码
 * @param newPassword 新密码
 * @return 是否成功
 */
bool changePasswordForCoach(char* oldPassword, char* newPassword);

/**
 * 添加教练课程类型关系
 * @param typeId 课程类型ID
 * @return 是否成功
 */
bool addCoachTypeRel(int typeId);

/**
 * 删除教练课程类型关系
 * @param id 关系ID
 * @return 是否成功
 */
bool deleteCoachTypeRel(int id);

/**
 * 显示所有课程
 */
void showAllCourseForCoach();

/**
 * 搜索课程
 * @param name 课程名称
 */
void searchCourseForCoach(char* name);

/**
 * 显示所有用户
 */
void showAllUserForCoach();

/**
 * 搜索用户
 * @param phone 用户手机号
 */
void searchUserForCoach(char* phone);

/**
 * 按照价格排序课程
 */
void sortCourseByPriceForCoach();

/**
 * 按照姓名排序用户
 */
void sortUserByNameForCoach();

/**
 * 统计课程类型数量
 */
void countCourseTypeForCoach();

/**
 * 统计指定课程类型的课程数量
 * @param typeId 课程类型ID
 */
void countCourseTypeByIdForCoach(int typeId);

/**
 * 统计用户课程数量
 */
void countUserCourseForCoach();

/**
 * 统计指定用户的课程数量
 * @param userId 用户ID
 */
void countUserCourseByIdForCoach(int userId);

#endif // !__COACH_H