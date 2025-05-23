#ifndef __USER_H
#define __USER_H

#include "structs.h"
#include <stdbool.h>
#include "admin.h"

// 个人信息管理

/**
 * 学员注册
 * @param name 名字
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
 * 修改信息（为NULL则不修改）
 * @param name 姓名
 * @param phone 电话
 */
bool changeInfoForUser(char* name, char* phone);

// 个人信息管理

// 课程报名管理

/**
 * 添加自己所上的课程
 * @param courseId 课程ID
 */
bool addUserCourseRel(int courseId);

/**
 * 删除自己所上的课程
 * @param courseId 课程ID
 */
bool deleteUserCourseRel(int courseId);

// 课程报名管理

// 信息查询

/**
 * 打印当前用户所有课程信息
 */
bool showCourseInfoForUser();

/**
 * 查询当前用户课程信息（null或者-1的项目则说明不使用这个条件）
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
bool searchCourseInfoForUser(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high);

// 信息查询

// 信息排序

/**
 * 对当前用户课程信息进行排序
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
bool sortCourseInfoForUser(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

// 信息排序

// 信息统计

/**
 * 打印当前用户所有类型课程的学员数量
 */
bool countCourseTypeSelectedForUser();

/**
 * 打印当前用户某个类型课程的学员数量
 * @param courseTypeId 课程类型ID
 */
bool countCourseTypeSelectedByIdForUser(int courseTypeId);

/**
 * 打印所有教练负责当前用户的健身课程门数
 */
bool countCoachCourseForUser();

/**
 * 打印某教练负责的当前用户健身课程门数
 * @param coachId 教练ID
 */
bool countCoachCourseByIdForUser(int coachId);

// 信息统计

// 系统维护

/**
 * 修改用户密码
 * @param newPassword 新密码
 */
bool changePasswordForUser(char* newPassword);

// 系统维护

#endif
