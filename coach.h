#ifndef __COACH_H
#define __COACH_H

#include "structs.h"
#include <stdbool.h>
#include "admin.h"

// 登录

/**
 * 教练登录
 * @param username 用户名
 * @param password 密码
 * @return 是否成功
 */
bool loginForCoach(char* userName, char* password);

// 登录

// 个人信息管理

/**
 * 增加教练擅长指导的项目类型
 * @param typeId 项目ID
 */
bool addNewCoachTypeRel(int typeId);

/**
 * 删除教练擅长指导的项目类型
 * @param typeId 项目ID
 */
bool removeCoachTypeRel(int typeId);

// 个人信息管理

// 信息查询

/**
 * 打印所有当前教练参与的课程信息
 */
bool showCourseInfoForCoach();

/**
 * 查询当前教练参与的课程信息（null或者-1的项目则说明不使用这个条件）
 * @param courseName 课程名称
 * @param courseType 课程类型
 * @param location 课程地点
 * @param stuMax_low 最大学生（左区间）
 * @param stuMax_high 最大学生（右区间）
 * @param price_low 价格（左区间）
 * @param price_high 价格（右区间）
 * @param time_low 时间（左区间）
 * @param time_high 时间（右区间）
 */
bool searchCourseInfoForCoach(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high);

/**
 * 查看所有与当前教练有关的学员信息
 */
bool showUserInfoForCoach();

/**
 * 查询与当前教练有关的某学员信息
 * @param userName 用户名
 */
bool searchUserInfoForCoach(char* userName);

// 信息查询

// 信息排序

/**
 * 对前教练参与的课程信息进行排序
 * 每个关键字的值为正数时则说明为升序排序，为负数时则为降序排序
 * 如果关键字的值为0则不参与排序，关键字的绝对值越小，则说明优先度越高
 * 两个值非零的关键字的绝对值不应该相同
 * @param byName   按名称
 * @param byTypeId 按类型ID
 * @param byTime 按时间
 * @param byLocation 按地点
 * @param byStuCount 按所选学生数量
 * @param byStuMax 按最大学生数量
 * @param byCoachNum 按教练数量
 * @param byPrice 按价格
 */
bool sortCourseInfoForCoach(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

/**
 * 排序与当前教练有关的学员信息
 * @param byName 按名称
 * @param bySelectCourse 按所选课程数量
 */
bool sortUserInfoForCoach(int byName, int bySelectCourse);

// 信息排序

// 信息统计

/**
 * 打印当前教练所有类型课程的学员数量
 */
bool countCourseTypeSelectedForCoach();

/**
 * 打印当前教练某个类型课程的学员数量
 * @param courseTypeId 课程类型ID
 */
bool countCourseTypeSelectedByIdForCoach(int courseTypeId);

/**
 * 打印当前教练所有学员参加的健身课程的门数
 */
bool countUserCourseForCoach();

/**
 * 打印当前教练某学员参加的健身课程的门数
 * @param userId 学员ID
 */
bool countUserCourseByIdForCoach(int userId);

// 信息统计

// 系统维护

/**
 * 修改密码
 * @param newPassword 新密码
 */
bool changePasswordForCoach(char* newPassword);

// 系统维护

#endif // !__COACH_H

