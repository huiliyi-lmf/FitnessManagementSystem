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

    // 获取所有有效记录
    setFilterStrFlag(userName);
    Coach* coachList = selectAllForCoach(filterCoachByName);
    clearFilterStrFlag();
    if (coachList == NULL) {
        warnInfo("登录失败：无教练账号数据");
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

    // 释放链表内存
    freeCoachList(coachList);

    // 错误提示
    if (!loginSuccess) {
        warnInfo("登录失败：用户名或密码错误");
    }

    return loginSuccess;
}

// 登录

// 个人信息管理

/**
 * 增加教练擅长指导的项目类型
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
        puts("该教练已经擅长此项目类型");
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
        puts("增加教练擅长项目类型成功");
        return true;
    }
    else {
        puts("增加教练擅长项目类型失败");
        return false;
    }
}
/**
 * 删除教练擅长指导的项目类型
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
        printf("未找到教练ID为 %d 与项目类型ID为 %d 的关系\n", CURRENT_COACH_ID, typeId);
        return false;
    } else {
        deleteByIdForCoachTypeRel(existingRel->id);
        // 释放内存
        freeCoachTypeRelList(coachTypeRels);
        return true;
    }
}

// 个人信息管理

// 信息查询

/**
 * 打印所有当前教练参与的课程信息
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

    // 遍历教练课程关系，找到当前教练参与的课程
    puts("当前教练参与的课程信息：\n--------------------------");
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
 * 查询当前教练参与的课程信息（null或-1表示忽略该条件）
 * @param courseName 课程名称（NULL表示忽略）
 * @param courseType 课程类型名称（NULL表示忽略）
 * @param location 课程地点（NULL表示忽略）
 * @param stuMax_low 最大学生数下限（-1表示忽略）
 * @param stuMax_high 最大学生数上限（-1表示忽略）
 * @param price_low 价格下限（-1表示忽略）
 * @param price_high 价格上限（-1表示忽略）
 * @param time_low 时间下限（分钟数，-1表示忽略）
 * @param time_high 时间上限（分钟数，-1表示忽略）
 */
bool searchCourseInfoForCoach(char* courseName, char* courseType, char* location,
    int stuMax_low, int stuMax_high,
    double price_low, double price_high,
    int time_low, int time_high) {
    // 检查登录状态
    
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取教练参与的课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("当前教练没有参与任何课程");
        return true;
    }

    // 如果指定课程类型名称，先查询类型ID
    int courseTypeId = -1;
    if (courseType != NULL) {
        setFilterStrFlag(courseType);
        CourseType* typeHeader = selectAllForCourseType(filterCourseTypeByName);
        clearFilterStrFlag();
        if (typeHeader != NULL) {
            courseTypeId = typeHeader->id;
        }
        freeCourseTypeList(typeHeader);
        if (courseTypeId == -1) {
            puts("未找到指定的课程类型");
            freeCoachCourseRelList(coachCourseRels);
            return true;
        }
    }

    // 遍历教练课程关系，筛选符合条件的课程
    puts("符合条件的课程信息：\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        Course* course = selectByIdForCourse(currentRel->courseId);
        if (course == NULL) {
            currentRel = currentRel->nextPointer;
            continue;
        }

        // 应用过滤条件
        bool match = true;

        // 课程名称匹配（模糊匹配，若参数非NULL）
        if (courseName != NULL && strstr(course->name, courseName) == NULL) {
            match = false;
        }

        // 课程类型匹配（若参数非NULL）
        if (courseType != NULL && course->type_id != courseTypeId) {
            match = false;
        }

        // 地点匹配（若参数非NULL）
        if (location != NULL && strcmp(course->location, location) != 0) {
            match = false;
        }

        // 最大学生数区间
        if (stuMax_low != -1 && course->student_max < stuMax_low) match = false;
        if (stuMax_high != -1 && course->student_max > stuMax_high) match = false;

        // 价格区间
        if (price_low != -1 && course->price < price_low) match = false;
        if (price_high != -1 && course->price > price_high) match = false;

        // 时间区间（分钟数）
        if (time_low != -1 && course->time < time_low) match = false;
        if (time_high != -1 && course->time > time_high) match = false;

        // 打印匹配的课程
        if (match) {
            printLinkForCourse(course);
            found = true;
        }

        free(course);
        currentRel = currentRel->nextPointer;
    }

    if (!found) {
        puts("未找到符合条件的课程");
    }

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * 查看所有与当前教练有关的学员信息
 */
bool showUserInfoForCoach() {
    // 检查登录状态

    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有教练课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("当前教练没有参与任何课程");
        return false;
    }

    // 遍历教练课程关系，找到当前教练参与的课程
    puts("与当前教练有关的学员信息：\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        // 遍历学员课程关系，找到报名该课程的学员
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentUserRel = userCourseRels;
        while (currentUserRel != NULL) {
            // 获取学员信息
            User* user = selectByIdForUser(currentUserRel->user_id);
            if (user != NULL) {
                // 打印学员信息
                printLinkForUser(user);
                found = true;

                // 释放学员内存
                free(user);
            }
            currentUserRel = currentUserRel->nextPointer;
        }
        freeUserCourseRelList(userCourseRels);
        currentRel = currentRel->nextPointer;
    }

    // 如果未找到学员
    if (!found) {
        puts("当前教练没有相关的学员");
    }

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * 查询与当前教练有关的某学员信息
 * @param userName 用户名
 */
void searchUserInfoForCoach(char* userName) {
    // 检查登录状态
   
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有教练课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("当前教练没有参与任何课程");
        return true;
    }

    // 遍历教练课程关系，找到当前教练参与的课程
    puts("与当前教练有关的学员信息：\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        // 遍历学员课程关系，找到报名该课程的学员
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentUserRel = userCourseRels;
        while (currentUserRel != NULL) {
            // 获取学员信息
            User* user = selectByIdForUser(currentUserRel->user_id);
            if (user != NULL && strcmp(user->name, userName) == 0) {
                // 打印学员信息
                printf("学员ID: %d\n", user->id);
                printf("学员姓名: %s\n", user->name);
                printf("联系方式: %s\n", user->phone);
                printf("--------------------------\n");
                found = true;

                // 释放学员内存
                free(user);
                break; // 找到匹配学员后退出内层循环
            }
            free(user); // 释放不匹配的学员内存
            currentUserRel = currentUserRel->nextPointer;
        }
        freeUserCourseRelList(userCourseRels);
        currentRel = currentRel->nextPointer;
    }

    // 如果未找到学员
    if (!found) {
        printf("未找到与当前教练有关的学员: %s\n", userName);
    }

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

// 信息查询

// 信息排序

/**
 * 对当前教练参与的课程信息进行排序
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
bool sortCourseInfoForCoach(int byName, int byTypeId, int byTime, int byLocation,
    int byStuCount, int byStuMax, int byCoachNum, int byPrice) {
    //// 获取当前登录的教练ID
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取教练参与的课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("当前教练没有参与任何课程");
        return true;
    }

    // 创建Course链表
    Course* courses = NULL;
    Course* current = NULL;
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        if (courses == NULL) {
            courses = selectByIdForCourse(currentRel->courseId);
            current = courses;
        } else {
            current->nextPointer = selectByIdForCourse(currentRel->courseId);
            if (current->nextPointer != NULL) {
                current = current->nextPointer;
            }
        }
        currentRel = currentRel->nextPointer;
    }

    // 排序
    Course* sortHeader = sortCourseInfo(courses, byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice);

    printLinkForCourse(sortHeader);
    freeCourseList(sortHeader);
    freeCourseList(courses);
    return true;
}

/**
 * 排序与当前教练有关的学员信息
 * @param byName 按名称（正数为升序，负数为降序，0为不排序）
 * @param bySelectCourse 按所选课程数量（正数为升序，负数为降序，0为不排序）
 */
bool sortUserInfoForCoach(int byName, int bySelectCourse) {
    // 获取当前登录的教练ID
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取教练参与的课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("当前教练没有参与任何课程");
        return true;
    }

    User* users = NULL;
    User* currentUser = NULL;
    CoachCourseRel* coachCourseRel = coachCourseRels;
    while (coachCourseRel != NULL) {
        setFilterIntFlag(coachCourseRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        UserCourseRel* userCourseRel = userCourseRels;
        while (userCourseRel != NULL) {
            User* user = selectByIdForUser(userCourseRel->user_id);
            if (users == NULL) {
                users = user;
                currentUser = users;
            } else {
                currentUser->nextPointer = user;
                if (currentUser->nextPointer != NULL) {
                    currentUser = currentUser->nextPointer;
                }
            }
            userCourseRel = userCourseRel->nextPointer;
        }

        // 释放内存
        freeUserCourseRelList(userCourseRels);
        coachCourseRel = coachCourseRel->nextPointer;
    }

    // 排序
    User* sortHeader = sortUserInfo(users, byName, bySelectCourse);
    printLinkForUser(sortHeader);

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    freeUserList(sortHeader);
    freeUserList(users);
    return true;
}
// 信息排序

// 信息统计

/**
 * 打印当前教练所有类型课程的学员数量
 */
bool countCourseTypeSelectedForCoach() {
    // 检查登录状态
  
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有教练擅长的课程类型
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    clearFilterIntFlag();
    
    // 遍历链表
    CoachTypeRel* coachTypeRel = coachTypeRels;
    while (coachTypeRel != NULL) {
        countCourseTypeSelectedByIdForCoach(coachTypeRel->type_id);
        coachTypeRel = coachTypeRel->nextPointer;
    }

    // 释放内存
    freeCoachTypeRelList(coachTypeRels);
    return true;
}

/**
 * 打印当前教练某个类型课程的学员数量
 * @param courseTypeId 课程类型ID
 */
bool countCourseTypeSelectedByIdForCoach(int courseTypeId) {
    // 检查登录状态
    
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取当前教练的所有课程关系
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("当前教练没有参与任何课程");
        return true;
    }

    int totalStudents = 0;  // 总学员数量
    bool foundCourse = false;

    // 遍历教练课程关系
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        // 获取课程详细信息
        Course* course = selectByIdForCourse(currentRel->courseId);
        if (course != NULL && course->type_id == courseTypeId) {
            foundCourse = true;
            int courseStudents = 0;

            // 统计该课程的学员数量
            setFilterIntFlag(course->id);
            UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
            clearFilterIntFlag();
            UserCourseRel* currentUserRel = userCourseRels;
            while (currentUserRel != NULL) {
                    courseStudents++;
                currentUserRel = currentUserRel->nextPointer;
            }
            freeUserCourseRelList(userCourseRels);

            // 打印当前课程的学员数量
            printf("课程ID: %d\n", course->id);
            printf("课程名称: %s\n", course->name);
            printf("学员数量: %d\n", courseStudents);
            printf("--------------------------\n");

            totalStudents += courseStudents;
            free(course);  // 释放课程内存
        }
        else if (course != NULL) {
            free(course);  // 非目标类型课程也要释放内存
        }
        currentRel = currentRel->nextPointer;
    }

    // 结果汇总
    if (foundCourse) {
        printf("课程类型ID: %d\n", courseTypeId);
        printf("总学员数量: %d\n", totalStudents);
    }
    else {
        printf("当前教练没有教授课程类型ID为 %d 的课程\n", courseTypeId);
    }

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}


/**
 * 打印当前教练所有学员参加的健身课程的门数
 */
bool countUserCourseForCoach() {
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
        puts("当前教练没有参与任何课程");
        return true;
    }

    // 统计学员参加的课程数量
    int totalCourses = 0; // 总课程数量
    int totalStudents = 0; // 总学员数量

    // 遍历教练课程关系，找到当前教练参与的课程
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        // 遍历学员课程关系，统计当前课程的学员数量
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentUserRel = userCourseRels;
        while (currentUserRel != NULL) {
            totalCourses++;
            currentUserRel = currentUserRel->nextPointer;
        }
        totalStudents++; // 每个课程至少有一个学员
        currentRel = currentRel->nextPointer;
        freeUserCourseRelList(userCourseRels);
    }

    // 打印统计结果
    puts("当前教练所有学员参加的健身课程门数统计：\n--------------------------");
    printf("总课程数量: %d\n", totalCourses);
    printf("总学员数量: %d\n", totalStudents);
    printf("--------------------------\n");

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * 打印当前教练某学员参加的健身课程的门数
 * @param userId 学员ID
 */
bool countUserCourseByIdForCoach(int userId) {
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
        puts("当前教练没有参与任何课程");
        return true;
    }

    // 统计该学员参加的课程数量
    int totalCourses = 0; // 学员的课程数量

    // 遍历教练课程关系，找到当前教练参与的课程
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        // 遍历学员课程关系，统计该学员是否参加了当前课程
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        setFilterIntFlag(userId);
        userCourseRels = filterUserCourseRelByUserId(userCourseRels);
        if (userCourseRels != NULL) {
            freeUserCourseRelList(userCourseRels);
            totalCourses++;
        }
        currentRel = currentRel->nextPointer;
    }

    // 打印统计结果
    puts("当前教练该学员参加的健身课程门数统计：\n--------------------------");
    printf("学员ID: %d\n", userId);
    printf("该学员参加的课程数量: %d\n", totalCourses);
    printf("--------------------------\n");

    // 释放内存
    freeCoachCourseRelList(coachCourseRels);
    return true;
}


// 信息统计

// 系统维护

/**
 * 修改当前登录教练的密码
 * @param newPassword 新密码
 */
bool changePasswordForCoach(char* newPassword) {
    // 获取当前登录的教练ID
   
    if (CURRENT_COACH_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取当前教练信息
    Coach* coach = selectByIdForCoach(CURRENT_COACH_ID);
    if (coach == NULL) {
        puts("教练信息获取失败");
        return false;
    }

    // 检验密码长度是否超出限制
    if (strlen(newPassword) >= MAX_STR) {
        printf("密码长度不能超过 %d 个字符\n", MAX_STR - 1);
        free(coach);
        return false;
    }

    // 更新密码
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(coach->password, encryptPassword);
    free(encryptPassword);

    // 更新数据库中的教练信息
    if (updateByIdForCoach(coach, CURRENT_COACH_ID)) {
        puts("密码修改成功");
    }
    else {
        puts("密码修改失败，请重试");
    }

    // 释放内存
    free(coach);
    return true;
}
// 系统维护
