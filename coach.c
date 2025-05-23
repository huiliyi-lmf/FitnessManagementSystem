#define _CRT_SECURE_NO_WARNINGS
#include "coach.h"
#include <string.h>

// 当前登录的教练ID
int CURRENT_COACH_ID = -1;

/**
 * 教练登录
 * @param username 用户名
 * @param password 密码
 * @return 是否成功
 */
bool loginForCoach(char* userName, char* password) {
    // 参数校验
    if (userName == NULL || password == NULL) {
        warnInfo("登录失败：用户名或密码为空");
        return false;
    }

    // 获取所有有效教练
    setFilterStrFlag(userName);
    Coach* coachList = selectAllForCoach(filterCoachByName);
    clearFilterStrFlag();
    if (coachList == NULL) {
        warnInfo("登录失败：无教练账号存在");
        return false;
    }

    bool loginSuccess = false;
    // 安全比较用户名和密码
    if (verifyPassword(password, coachList->password)) {
        loginSuccess = true;
        CURRENT_COACH_ID = coachList->id;
    }
    else {
        warnInfo("登录失败：用户名或密码错误");
        return false;
    }

    // 释放分配内存
    freeCoachList(coachList);

    // 错误提示
    if (!loginSuccess) {
        warnInfo("登录失败：用户名或密码错误");
    }

    return loginSuccess;
}

// 登录

// 教练信息管理

/**
 * 添加教练教授指定项目类型
 * @param typeId 项目ID
 */
bool addNewCoachTypeRel(int typeId) {
    // 获取当前登录的教练ID
    
    if (CURRENT_COACH_ID == -1) {
        warnInfo("请先登录");
        return false;
    }

    // 检查项目类型是否存在
    CourseType* courseType = selectByIdForCourseType(typeId);
    if (courseType == NULL) {
        printf("未找到ID为 %d 的项目类型\n", typeId);
        return false;
    }
    free(courseType); // 释放内存

    // 检查是否已经存在该关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachTypeRel* existingRel = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    setFilterIntFlag(typeId);
    existingRel = filterCoachTypeRelByCourseTypeId(existingRel);
    clearFilterIntFlag();
    if (existingRel != NULL) {
        puts("该教练已经教授该项目类型");
        freeCoachTypeRelList(existingRel); // 释放内存
        return false;
    }

    // 创建新的教练项目类型关系
    CoachTypeRel newRel;
    newRel.coach_id = CURRENT_COACH_ID;
    newRel.type_id = typeId;
    newRel.status = FILE_STATUS_EXIST;

    // 插入到数据库
    if (insertForCoachTypeRel(&newRel)) {
        puts("添加教练教授项目类型成功");
        return true;
    }
    else {
        puts("添加教练教授项目类型失败");
        return false;
    }
}
/**
 * 删除教练教授指定项目类型
 * @param typeId 项目ID
 */
bool removeCoachTypeRel(int typeId) {
    // 获取当前登录的教练ID
    
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有教练项目类型关系
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(NULL);
    if (coachTypeRels == NULL) {
        puts("没有教练项目类型关系记录");
        return false;
    }

    setFilterIntFlag(CURRENT_COACH_ID);
    CoachTypeRel* existingRel = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    setFilterIntFlag(typeId);
    existingRel = filterCoachTypeRelByCourseTypeId(existingRel);
    clearFilterIntFlag();

    // 如果未找到关系
    if (existingRel == NULL) {
        printf("未找到教练ID为 %d 和项目类型ID为 %d 的关系\n", CURRENT_COACH_ID, typeId);
        return false;
    } else {
        deleteByIdForCoachTypeRel(existingRel->id);
        // 释放内存
        freeCoachTypeRelList(coachTypeRels);
        return true;
    }
}

// 教练信息管理

// 信息查询

/**
 * 打印所有当前教练教授的课程信息
 */
bool showCourseInfoForCoach() {
    // 获取当前登录的教练ID
    
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有教练课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("没有教练课程关系记录");
        return true;
    }

    // 遍历所有课程关系，找到当前教练教授的课程
    puts("当前教练教授的课程信息：\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        // 获取课程信息
        Course* course = selectByIdForCourse(currentRel->courseId);
        if (course != NULL) {
            printLinkForCourse(course);
            freeCourseList(course);
        }
        currentRel = currentRel->nextPointer;
    }

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * 查询当前教练教授的课程信息（null和-1表示不限该条件）
 * @param courseName 课程名称（NULL表示不限）
 * @param courseType 课程类型名称（NULL表示不限）
 * @param location 课程地点（NULL表示不限）
 * @param stuMax_low 最大学生人数下限（-1表示不限）
 * @param stuMax_high 最大学生人数上限（-1表示不限）
 * @param price_low 价格下限（-1表示不限）
 * @param price_high 价格上限（-1表示不限）
 * @param time_low 时间下限（秒数，-1表示不限）
 * @param time_high 时间上限（秒数，-1表示不限）
 */
bool searchCourseInfoForCoach(char* courseName, char* courseType, char* location,
    int stuMax_low, int stuMax_high,
    double price_low, double price_high,
    int time_low, int time_high) {
    // 登录状态
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有教练课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("没有教练课程关系记录");
        return true;
    }

    // 遍历所有课程关系，找到当前教练教授的课程
    puts("查询结果：\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        // 获取课程信息
        Course* course = selectByIdForCourse(currentRel->courseId);
        if (course != NULL) {
            // 检查课程是否符合条件
            bool match = true;

            // 检查课程名称
            if (courseName != NULL && strstr(course->name, courseName) == NULL) {
                match = false;
            }

            // 检查课程类型
            if (match && courseType != NULL) {
                CourseType* type = selectByIdForCourseType(course->type_id);
                if (type == NULL || strstr(type->name, courseType) == NULL) {
                    match = false;
                }
                if (type != NULL) {
                    freeCourseTypeList(type);
                }
            }

            // 检查课程地点
            if (match && location != NULL && strstr(course->location, location) == NULL) {
                match = false;
            }

            // 检查最大学生人数
            if (match && stuMax_low != -1 && course->stu_max < stuMax_low) {
                match = false;
            }
            if (match && stuMax_high != -1 && course->stu_max > stuMax_high) {
                match = false;
            }

            // 检查价格
            if (match && price_low != -1 && course->price < price_low) {
                match = false;
            }
            if (match && price_high != -1 && course->price > price_high) {
                match = false;
            }

            // 检查时间
            if (match && time_low != -1 && course->time < time_low) {
                match = false;
            }
            if (match && time_high != -1 && course->time > time_high) {
                match = false;
            }

            // 如果符合条件，打印课程信息
            if (match) {
                printLinkForCourse(course);
                found = true;
            }

            freeCourseList(course);
        }
        currentRel = currentRel->nextPointer;
    }

    if (!found) {
        puts("未找到符合条件的课程");
    }

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}