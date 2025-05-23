#ifndef __ADMIN_H
#define __ADMIN_H

#include <stdbool.h>
#include "structs.h"
#include <stdlib.h>
#include <math.h>
#include "filter.h"

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

// 信息管理：增加，修改，删除————

/**
 * 创建一个新的项目类型
 * @param typeName 项目名称
 */
bool createCourseType(char* typeName);

/**
 * 修改某个项目名称
 * @param id 项目编号
 * @param typeName 修改后的名称
 */
bool updateCourseType(int id, char* typeName);

/**
 * 删除项目
 * @param id 项目ID
 */
bool deleteCourseType(int id);

/**
 * 创建新的教练账户
 * @param coachName 教练姓名
 * @param coachPassword 教练密码
 */
bool createCoach(char* coachName, char* coachPassword);

/**
 * 修改教练信息（如果为NULL的项则不修改）
 * @param id 教练ID
 * @param newCoachName 教练姓名
 * @param newCoachPassword  教练密码
 * @param newLevel  教练评级（为-1则不修改）
 */
bool updateCoach(int id, char* newCoachName, char* newCoachPassword, CoachLevel newLevel);

/**
 * 删除教练信息
 * @param id 教练ID
 */
bool deleteCoach(int id);

// —————信息管理：增加，修改，删除

// 教练管理

// 查看所有教练信息
bool showAllCoachInfo();

// 查看所有课程类别信息
bool showAllCourseTypeInfo();

/**
 * 查看教练提交的自己擅长指导的项目类型申请
 */
bool showNeedJudgeCoachInfo();

/**
 * 审核教练信息的结果
 * @param id 教练ID
 * @param status 教练信息
 */
bool markJudgeCoachInfoStatus(int id, CoachStatus status);

// 教练管理

// 课程管理

/**
 * 创建一个新的课程
 * @param courseName 课程名称
 * @param courseType 课程类别
 * @param time 时间
 * @param location 地点
 * @param stuMax 最大学生数量
 * @param price 价格
 */
bool createCourse(char* courseName, int courseType, int time, char* location, int stuMax, double price);

/**
 * 修改课程信息
 * @param id 课程ID
 * @param courseName 课程名称
 * @param courseType 课程类别
 * @param time 时间
 * @param location 地点
 * @param stuMax 最大学生数量
 * @param price 价格
 */
bool updateCourse(int id, char* courseName, int courseType, int time, char* location, int stuMax, double price);

// 课程管理

// 分派和调整健身课程的教练

/**
 * 添加教练和课程关系
 * @param courseId 课程ID
 * @param coachId 教练ID
 */
bool addCoachCourseRel(int courseId, int coachId);

/**
 * 删除教练和课程关系
 * @param courseId 课程ID
 * @param coachId 教练ID
 */
bool deleteCoachCourseRel(int courseId, int coachId);

// 分派和调整健身课程的教练

// 信息查询

/**
 * 打印所有课程信息
 */
bool showCourseInfoForAdmin();

/**
 * 查询课程信息（null或者-1的项目则说明不使用这个条件）
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
bool searchCourseInfoForAdmin(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high);

/**
 * 查看所有学员信息
 */
bool showUserInfoForAdmin();

/**
 * 查询某学员信息
 * @param userName 用户名
 */
bool searchUserInfoForAdmin(char* userName);

// 信息查询

// 信息排序

Course* sortCourseInfo(Course* courses, int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

/**
 * 对课程信息进行排序
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
bool sortCourseInfoForAdmin(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

User* sortUserInfo(User* users, int byName, int bySelectCourse);

/**
 * 排序学员信息
 * @param byName 按名称
 * @param bySelectCourse 按所选课程数量
 */
bool sortUserInfoForAdmin(int byName, int bySelectCourse);

// 信息排序

// 信息统计

/**
 * 打印所有类型课程的学员数量
 */
bool countCourseTypeSelectedForAdmin();

/**
 * 打印某个类型课程的学员数量
 * @param courseTypeId 课程类型ID
 */
bool countCourseTypeSelectedByIdForAdmin(int courseTypeId);

/**
 * 打印所有教练负责的健身课程门数
 */
bool countCoachCourseForAdmin();

/**
 * 打印某教练负责的健身课程门数
 * @param coachId 教练ID
 */
bool countCoachCourseByIdForAdmin(int coachId);

/**
 * 打印所有学员参加的健身课程的门数
 */
bool countUserCourseForAdmin();

/**
 * 打印某学员参加的健身课程的门数
 * @param userId 学员ID
 */
bool countUserCourseByIdForAdmin(int userId);

// 信息统计

// 系统维护

/**
 * 修改管理员密码（ID由登录函数记录）
 * @param newPassword 新密码
 * @return 是否成功
 */
bool changePasswordForAdmin(char* newPassword);

/**
 * 修改教练密码
 * @param coachId 教练ID
 * @param newPassword 新密码
 */
bool changeCoachPasswordForAdmin(int coachId, char* newPassword);

/**
 * 数据备份
 * @param backupName 备份名称
 */
bool databaseBackup(char* backupName);

/**
 * 数据恢复
 * @param backupName 备份名称
 */
bool databaseRestore(char* backupName);

// 系统维护

#endif // !__ADMIN__H