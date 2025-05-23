#ifndef __ADMIN_H
#define __ADMIN_H

#include "structs.h"
#include "filter.h"

// 当前用户登录的信息
extern int CURRENT_ADMIN_ID;

/**
 * 登录判断
 * @param userName 用户名
 * @param password 密码
 * @return 密码是否正确
 */
bool loginForAdmin(char* userName, char* password);

/**
 * 注册管理员
 * @param userName 用户名
 * @param password 密码
 * @return 是否成功
 */
bool registerForAdmin(char* userName, char* password);

/**
 * 修改密码
 * @param oldPassword 旧密码
 * @param newPassword 新密码
 * @return 是否成功
 */
bool changePasswordForAdmin(char* oldPassword, char* newPassword);

/**
 * 修改教练密码
 * @param coachId 教练ID
 * @param newPassword 新密码
 * @return 是否成功
 */
bool changePasswordForCoach(int coachId, char* newPassword);

/**
 * 备份数据
 * @return 是否成功
 */
bool backupData();

/**
 * 恢复数据
 * @return 是否成功
 */
bool restoreData();

/**
 * 创建课程类型
 * @param name 课程类型名称
 * @return 是否成功
 */
bool createCourseType(char* name);

/**
 * 更新课程类型
 * @param id 课程类型ID
 * @param name 课程类型名称
 * @return 是否成功
 */
bool updateCourseType(int id, char* name);

/**
 * 删除课程类型
 * @param id 课程类型ID
 * @return 是否成功
 */
bool deleteCourseType(int id);

/**
 * 显示所有课程类型
 */
void showAllCourseType();

/**
 * 创建教练
 * @param name 教练姓名
 * @param password 教练密码
 * @param level 教练星级
 * @return 是否成功
 */
bool createCoach(char* name, char* password, CoachLevel level);

/**
 * 更新教练
 * @param id 教练ID
 * @param name 教练姓名
 * @param level 教练星级
 * @return 是否成功
 */
bool updateCoach(int id, char* name, CoachLevel level);

/**
 * 删除教练
 * @param id 教练ID
 * @return 是否成功
 */
bool deleteCoach(int id);

/**
 * 显示所有教练
 */
void showAllCoach();

/**
 * 显示待审核教练
 */
void showJudgeCoach();

/**
 * 标记教练审核状态
 * @param id 教练ID
 * @param status 审核状态
 * @return 是否成功
 */
bool markJudgeCoach(int id, CoachStatus status);

/**
 * 创建课程
 * @param name 课程名称
 * @param typeId 课程类型ID
 * @param time 课程时间（相对于0年0时的分钟数）
 * @param location 课程地点
 * @param studentMax 课程最大学生数
 * @param price 课程价格
 * @return 是否成功
 */
bool createCourse(char* name, int typeId, int time, char* location, int studentMax, double price);

/**
 * 更新课程
 * @param id 课程ID
 * @param name 课程名称
 * @param typeId 课程类型ID
 * @param time 课程时间（相对于0年0时的分钟数）
 * @param location 课程地点
 * @param studentMax 课程最大学生数
 * @param price 课程价格
 * @return 是否成功
 */
bool updateCourse(int id, char* name, int typeId, int time, char* location, int studentMax, double price);

/**
 * 删除课程
 * @param id 课程ID
 * @return 是否成功
 */
bool deleteCourse(int id);

/**
 * 添加教练课程关系
 * @param coachId 教练ID
 * @param courseId 课程ID
 * @return 是否成功
 */
bool addCoachCourseRel(int coachId, int courseId);

/**
 * 删除教练课程关系
 * @param id 关系ID
 * @return 是否成功
 */
bool deleteCoachCourseRel(int id);

/**
 * 显示所有课程
 */
void showAllCourse();

/**
 * 搜索课程
 * @param name 课程名称
 */
void searchCourse(char* name);

/**
 * 显示所有用户
 */
void showAllUser();

/**
 * 搜索用户
 * @param phone 用户手机号
 */
void searchUser(char* phone);

/**
 * 按照价格排序课程
 */
void sortCourseByPrice();

/**
 * 按照姓名排序用户
 */
void sortUserByName();

/**
 * 统计课程数量
 */
void countCourse();

/**
 * 统计指定课程的学生数量
 * @param courseId 课程ID
 */
void countCourseById(int courseId);

/**
 * 统计教练数量
 */
void countCoach();

/**
 * 统计指定教练的课程数量
 * @param coachId 教练ID
 */
void countCoachById(int coachId);

/**
 * 统计用户数量
 */
void countUser();

/**
 * 统计指定用户的课程数量
 * @param userId 用户ID
 */
void countUserById(int userId);

#endif // !__ADMIN_H