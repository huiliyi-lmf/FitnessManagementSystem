#ifndef __USER_H
#define __USER_H

#include "structs.h"
#include "filter.h"

// 当前用户登录的信息
extern int CURRENT_USER_ID;

/**
 * 根据手机号查询学员信息
 * @param phone 手机号
 * @return 返回查找到的学员信息（User*），如果不存在则返回 NULL
 */
User* selectByPhoneForUser(char* phone);

/**
 * 学员注册
 * @param name 姓名
 * @param password 密码
 * @param phone 电话
 * @return 是否成功
 */
bool registerForUser(char* name, char* password, char* phone);

/**
 * 登录
 * @param name 姓名
 * @param password 密码
 * @param phone 电话号
 * @return 是否成功
 */
bool loginForUser(char* name, char* password, char* phone);

/**
 * 修改个人信息
 * @param name 姓名
 * @param phone 电话
 * @return 是否成功
 */
bool changeInfoForUser(char* name, char* phone);

/**
 * 修改密码
 * @param oldPassword 旧密码
 * @param newPassword 新密码
 * @return 是否成功
 */
bool changePasswordForUser(char* oldPassword, char* newPassword);

/**
 * 添加用户课程关系
 * @param courseId 课程ID
 * @return 是否成功
 */
bool addUserCourseRel(int courseId);

/**
 * 删除用户课程关系
 * @param id 关系ID
 * @return 是否成功
 */
bool deleteUserCourseRel(int id);

/**
 * 显示所有课程
 */
void showAllCourseForUser();

/**
 * 搜索课程
 * @param name 课程名称
 */
void searchCourseForUser(char* name);

/**
 * 按照价格排序课程
 */
void sortCourseByPriceForUser();

/**
 * 统计课程类型数量
 */
void countCourseTypeForUser();

/**
 * 统计指定课程类型的课程数量
 * @param typeId 课程类型ID
 */
void countCourseTypeByIdForUser(int typeId);

/**
 * 统计教练数量
 */
void countCoachForUser();

/**
 * 统计指定教练的课程数量
 * @param coachId 教练ID
 */
void countCoachByIdForUser(int coachId);

#endif // !__USER_H