#define _CRT_SECURE_NO_WARNINGS
#include "user.h"
#include <stdlib.h>

int CURRENT_USER_ID = -1;

// 个人信息管理

/**
 * 根据手机号查询学员信息
 * @param phone 手机号
 * @return 返回查找到的学员信息（User*），如果不存在则返回 NULL
 */
User* selectByPhoneForUser(char* phone) {
    // 校验手机号是否为空
    if (phone == NULL) {
        return NULL;
    }

    // 读取数据库中的学员列表
    setFilterStrFlag(phone);
    User* userList = selectAllForUser(filterUserByPhone);
    clearFilterStrFlag();
    if (userList == NULL) {
        return NULL;
    }
    return userList;
}

/**
 * 学员注册
 * @param name 名字
 * @param password 密码
 * @param phone 电话
 * @return 是否成功
 */
bool registerForUser(char* name, char* password, char* phone) {
    // 校验参数是否为空
    if (name == NULL || password == NULL || phone == NULL) {
        warnInfo("注册信息不能为空");
        return false;
    }

    // 校验字符串长度是否超出限制
    if (strlen(name) >= MAX_STR || strlen(password) >= MAX_STR || strlen(phone) >= MAX_STR) {
        warnInfo("输入信息过长");
        return false;
    }

    // 检查手机号是否已注册
    User* checkPhone = selectByPhoneForUser(phone);
    if (checkPhone != NULL) {
        warnInfo("该手机号已被注册");
        freeUserList(checkPhone);
        return false;
    }

    // 创建新学员对象
    User* newUser = (User*)malloc(sizeof(User));
    if (newUser == NULL) {
        warnInfo("内存分配失败");
        return false;
    }

    // 赋值用户信息
    memset(newUser, 0, sizeof(User));
    newUser->status = FILE_STATUS_EXIST;
    strcpy(newUser->name, name);
    char* encryptPassword = generateEncryptPassword(password);
    strcpy(newUser->password, encryptPassword);
    free(encryptPassword);
    strcpy(newUser->phone, phone);

    // 插入数据库
    if (insertForUser(newUser)) {
        warnInfo("注册成功");
        // 释放内存
        free(newUser);
        return true;
    } else {
        errorInfo("注册失败，请重试");
        // 释放内存
        free(newUser);
        return false;
    }
}

/**
 * 登录
 * @param name 姓名
 * @param password 密码
 * @param phone 电话号
 * @return 是否成功
 */
bool loginForUser(char* name, char* password, char* phone) {
    // 参数校验
    if (name == NULL || password == NULL || phone == NULL) {
        warnInfo("输入信息不能为空");
        return false;
    }

    // 根据手机号查询用户信息
    User* user = selectByPhoneForUser(phone);
    if (user == NULL) {
        warnInfo("该手机号未注册");
        return false;
    }

    // 验证用户名和密码是否匹配
    if (strcmp(user->name, name) != 0) {
        errorInfo("用户名错误");
        free(user);
        return false;
    }

    if (!verifyPassword(password, user->password)) {
        errorInfo("密码错误");
        free(user);
        return false;
    }

    // 可以在此处更新全局变量存储当前用户 ID
    CURRENT_USER_ID = user->id;

    free(user);
    return true;
}

/**
 * 修改信息（为NULL则不修改）
 * @param name 姓名
 * @param phone 电话
 */
bool changeInfoForUser(char* name, char* phone) {
    // 获取当前登录用户ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("请先登录");
        return false;
    }

    // 根据ID获取用户信息
    User* user = selectByIdForUser(CURRENT_USER_ID);
    if (user == NULL) {
        warnInfo("获取用户信息失败");
        return false;
    }

    // 修改姓名（如果参数非NULL，则检查长度后更新）
    if (name != NULL) {
        if (strlen(name) >= MAX_STR) {
            warnInfo("姓名长度过长");
            free(user);
            return false;
        }
        strcpy(user->name, name);
    }

    // 修改电话（如果参数非NULL，则检查长度后更新）
    if (phone != NULL) {
        if (strlen(phone) >= MAX_STR) {
            warnInfo("电话长度过长");
            free(user);
            return false;
        }
        User* exists = selectByPhoneForUser(phone);
        if (exists != NULL && exists->id != CURRENT_USER_ID) {
            warnInfo("该电话已被其他人使用");
            free(user);
            free(exists);
            return false;
        }
        strcpy(user->phone, phone);
    }

    // 更新数据库中的用户信息
    if (updateByIdForUser(user, CURRENT_USER_ID)) {
        warnInfo("用户信息修改成功");
        // 释放内存
        free(user);
        return true;
    } else {
        errorInfo("用户信息修改失败");
        // 释放内存
        free(user);
        return false;
    }
}

// 个人信息管理

// 课程报名管理

/**
 * 添加自己所上的课程
 * @param courseId 课程ID
 */
bool addUserCourseRel(int courseId) {
    // 获取当前登录用户ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("请先登录");
        return false;
    }

    // 检查课程是否存在
    Course* course = selectByIdForCourse(courseId);
    if (course == NULL) {
        warnInfo("指定的课程ID不存在\n");
        return false;
    }
    // 判断课程是否已满
    if (course->student_count >= course->student_max) {
        warnInfo("该课程已达到选人的上限，不能选择！");
        freeCourseList(course);
        return false;
    }

    // 检查是否存在关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* exists = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    setFilterIntFlag(courseId);
    exists = filterUserCourseRelByCourseId(exists);
    clearFilterIntFlag();
    if (exists != NULL) {
        warnInfo("已存在相应的关系记录");
        freeUserCourseRelList(exists);
        freeCourseList(course);
        return true;
    }

    // 修改Course表的student_count记录
    course->student_count++;
    updateByIdForCourse(course, courseId);
    freeCourseList(course);

    // 创建新的学员课程关系记录
    UserCourseRel* newRel = (UserCourseRel*)malloc(sizeof(UserCourseRel));
    if (newRel == NULL) {
        errorInfo("内存分配失败");
        return false;
    }

    // 赋值相关字段
    newRel->user_id = CURRENT_USER_ID;
    newRel->course_id = courseId;
    newRel->status = FILE_STATUS_EXIST;
    newRel->nextPointer = NULL;  // 新记录的下一个指针设为NULL

    // 插入到数据表中
    if (insertForUserCourseRel(newRel)) {
        warnInfo("添加课程成功");
        // 释放内存
        free(newRel);
        return true;
    }
    else {
        errorInfo("添加课程失败");
        // 释放内存
        free(newRel);
        return false;
    }
}

/**
 * 删除自己所上的课程
 * @param courseId 课程ID
 */
bool deleteUserCourseRel(int courseId) {
    // 获取当前登录用户ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("请先登录");
        return false;
    }

    // 检查是否存在关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* exists = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    setFilterIntFlag(courseId);
    exists = filterUserCourseRelByCourseId(exists);
    clearFilterIntFlag();

    if (exists != NULL) {
        if (!deleteByIdForUserCourseRel(exists->id)) {
            errorInfo("删除记录失败");
            freeUserCourseRelList(exists);
            return false;
        }
        freeUserCourseRelList(exists);
        return true;
    } else {
        warnInfo("不存在相应的选课记录");
        return true;;
    }
}

// 课程报名管理

// 信息查询

/**
 * 打印当前用户所有课程信息
 */
bool showCourseInfoForUser() {
    // 获取当前登录用户ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("请先登录");
        return false;
    }

    // 获取所有学员课程关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* relList = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (relList == NULL) {
        warnInfo("当前用户没有选课记录");
        return true;
    }

    int count = 0;
    UserCourseRel* p = relList;
    while (p != NULL) {
        // 获取课程信息
        Course* course = selectByIdForCourse(p->course_id);
        if (course != NULL) {
            printLinkForCourse(course);  // 打印该课程信息
            freeCourseList(course);
            count++;
        }
        p = p->nextPointer;
    }

    if (count == 0) {
        warnInfo("当前用户没有选课记录");
    }

    freeUserCourseRelList(relList);
    return true;
}

/**
 * 查询当前用户课程信息（null或者-1的项目则说明不使用这个条件）
 * @param courseName 课程名称（支持部分匹配，传入NULL或空串则忽略此条件）
 * @param courseType 课程类型（传入NULL或空串则忽略此条件）
 * @param location 课程地点（传入NULL或空串则忽略此条件）
 * @param stuMax_low 最大学生（左区间，-1表示忽略）
 * @param stuMax_high 最大学生（右区间，-1表示忽略）
 * @param price_low 价格（左区间，-1表示忽略）
 * @param price_high 价格（右区间，-1表示忽略）
 * @param time_low 时间（左区间，-1表示忽略）
 * @param time_high 时间（右区间，-1表示忽略）
 */
bool searchCourseInfoForUser(char* courseName, char* courseType, char* location,
    int stuMax_low, int stuMax_high, double price_low, double price_high,
    int time_low, int time_high) {
    // 获取当前登录用户ID
    
    if (CURRENT_USER_ID == -1) {
        warnInfo("请先登录");
        return false;
    }

    // 获取所有学员课程关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* relList = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (relList == NULL) {
        warnInfo("当前用户没有选课记录");
        return true;
    }

    int count = 0;
    UserCourseRel* p = relList;
    while (p != NULL) {
        // 获取该课程信息
        Course* course = selectByIdForCourse(p->course_id);
        if (course != NULL) {
            bool match = true;

            // 按课程名称过滤（部分匹配）
            if (courseName != NULL && strlen(courseName) > 0) {
                if (strstr(course->name, courseName) == NULL) {
                    match = false;
                }
            }
            // 按地点过滤（部分匹配）
            if (match && location != NULL && strlen(location) > 0) {
                if (strstr(course->location, location) == NULL) {
                    match = false;
                }
            }
            // 按学生人数过滤
            if (match && stuMax_low != -1) {
                if (course->student_max < stuMax_low) {
                    match = false;
                }
            }
            if (match && stuMax_high != -1) {
                if (course->student_max > stuMax_high) {
                    match = false;
                }
            }
            // 按价格过滤
            if (match && price_low != -1) {
                if (course->price < price_low) {
                    match = false;
                }
            }
            if (match && price_high != -1) {
                if (course->price > price_high) {
                    match = false;
                }
            }
            // 按时间过滤
            if (match && time_low != -1) {
                if (course->time < time_low) {
                    match = false;
                }
            }
            if (match && time_high != -1) {
                if (course->time > time_high) {
                    match = false;
                }
            }
            // 按课程类型过滤（需获取CourseType信息进行匹配）
            if (match && courseType != NULL && strlen(courseType) > 0) {
                CourseType* ct = selectByIdForCourseType(course->type_id);
                if (ct != NULL) {
                    if (strstr(ct->name, courseType) == NULL) {
                        match = false;
                    }
                    free(ct);
                }
                else {
                    match = false;
                }
            }

            // 如果所有条件均匹配，打印该课程信息
            if (match) {
                printLinkForCourse(course);
                count++;
            }
            free(course);
        }
        p = p->nextPointer;
    }

    if (count == 0) {
        warnInfo("未找到符合条件的课程");
    }

    freeUserCourseRelList(relList);
    return true;
}

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
bool sortCourseInfoForUser(int byName, int byTypeId, int byTime, int byLocation,
    int byStuCount, int byStuMax, int byCoachNum, int byPrice) {
    // 获取当前登录ID
    if (CURRENT_USER_ID == -1) {
        warnInfo("请先登录");
        return false;
    }
    
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();

    if (userCourseRels == NULL) {
        warnInfo("当前用户不存在选课信息");
        return true;
    }

    // 构造Course链表
    Course* courses = NULL;
    Course* current = NULL;
    UserCourseRel* currentRel = userCourseRels;
    while (currentRel != NULL) {
        Course* course = selectByIdForCourse(currentRel->course_id);
        if (courses == NULL) {
            courses = course;
            current = courses;
            if (current != NULL) {
                current->nextPointer = NULL;
            }
        } else {
            current->nextPointer = course;
            if (current->nextPointer != NULL) {
                current = current->nextPointer;
            }
        }
        currentRel = currentRel->nextPointer;
    }

    // 排序
    Course* sortHeader = sortCourseInfo(courses, byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice);

    // 显示结果
    printLinkForCourse(sortHeader);
    freeCourseList(sortHeader);
    freeCourseList(courses);
    return true;
}

// 信息排序

// 信息统计

/**
 * 打印当前用户所有类型课程的学员数量
 */
bool countCourseTypeSelectedForUser() {
    // 获取当前登录用户ID

    if (CURRENT_USER_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取学员课程关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("当前用户没有选课记录");
        return true;
    }

    // 统计课程类型对应的学员数量
    typedef struct TypeCount {
        int typeId;
        int studentCount;
        struct TypeCount* next;
    } TypeCount;

    TypeCount* head = NULL;

    // 遍历当前用户的课程
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        // 获取课程信息
        Course* course = selectByIdForCourse(currentUserRel->course_id);
        if (course != NULL) {
            int typeId = course->type_id;
            free(course);

            // 检查当前类型是否已在统计链表中
            TypeCount* typeNode = head;
            while (typeNode != NULL) {
                if (typeNode->typeId == typeId) {
                    typeNode->studentCount++;
                    break;
                }
                typeNode = typeNode->next;
            }

            // 如果未找到，新增统计节点
            if (typeNode == NULL) {
                TypeCount* newNode = (TypeCount*)malloc(sizeof(TypeCount));
                if (newNode == NULL) {
                    puts("操作系统内存不足");
                    return;
                }
                memset(newNode, 0, sizeof(TypeCount));
                newNode->typeId = typeId;
                newNode->studentCount = 1;
                newNode->next = head;
                head = newNode;
            }
        }
        currentUserRel = currentUserRel->nextPointer;
    }

    // 输出统计结果
    puts("当前用户所选课程类型对应的学员数量统计：");
    puts("--------------------------------------");

    if (head == NULL) {
        puts("当前用户没有选课记录");
    }
    else {
        TypeCount* typeNode = head;
        while (typeNode != NULL) {
            // 获取课程类型名称
            CourseType* typeInfo = selectByIdForCourseType(typeNode->typeId);
            if (typeInfo != NULL) {
                printf("课程类型: %s, 选课学员总数: %d\n", typeInfo->name, typeNode->studentCount);
                free(typeInfo);
            }
            typeNode = typeNode->next;
        }
    }

    puts("--------------------------------------");

    // 释放内存
    TypeCount* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
    freeUserCourseRelList(userCourseRels);
    return true;
}

/**
 * 打印当前用户某个类型课程的学员数量
 * @param courseTypeId 课程类型ID
 */
bool countCourseTypeSelectedByIdForUser(int courseTypeId) {
    // 获取当前登录用户ID
    
    if (CURRENT_USER_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有学员课程关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("当前用户没有选课记录");
        return true;
    }

    // 统计该类型课程的学员数量
    int studentCount = 0;

    // 遍历当前用户的课程
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        // 获取课程信息
        Course* course = selectByIdForCourse(currentUserRel->course_id);
        if (course != NULL) {
            if (course->type_id == courseTypeId) {
                studentCount++;
            }
            free(course);
        }
        currentUserRel = currentUserRel->nextPointer;
    }

    // 获取课程类型名称
    CourseType* courseType = selectByIdForCourseType(courseTypeId);
    if (courseType != NULL) {
        printf("课程类型: %s, 选课学员总数: %d\n", courseType->name, studentCount);
        free(courseType);
    }
    else {
        printf("未找到ID为 %d 的课程类型\n", courseTypeId);
    }

    // 释放内存
    freeUserCourseRelList(userCourseRels);
    return true;
}

/**
 * 打印所有教练负责当前用户的健身课程门数
 */
bool countCoachCourseForUser() {
    // 获取当前登录用户ID
    
    if (CURRENT_USER_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取学员课程关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("当前用户没有选课记录");
        return true;
    }

    // 统计教练负责的课程数
    int coachCourseCount = 0;
    int coachIds[100] = { 0 }; // 假设最多有 MAX_COACHES 个教练
    int coachCount = 0;

    // 遍历当前用户的选课记录
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        // 遍历教练课程关系，找到对应课程的教练
        setFilterIntFlag(currentUserRel->course_id);
        CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCourseId);
        clearFilterIntFlag();
        CoachCourseRel* currentCoachRel = coachCourseRels;
        while (currentCoachRel != NULL) {
            // 检查该教练是否已被统计
            int isNewCoach = 1;
            for (int i = 0; i < coachCount; i++) {
                if (coachIds[i] == currentCoachRel->coachId) {
                    isNewCoach = 0;
                    break;
                }
            }

            if (isNewCoach) {
                coachIds[coachCount++] = currentCoachRel->coachId;
            }

            coachCourseCount++;
            currentCoachRel = currentCoachRel->nextPointer;
        }
        freeCoachCourseRelList(coachCourseRels);
        currentUserRel = currentUserRel->nextPointer;
    }

    // 打印统计结果
    puts("当前用户的课程由以下教练负责：\n--------------------------");
    printf("总教练数量: %d\n", coachCount);
    printf("总课程数量: %d\n", coachCourseCount);
    printf("--------------------------\n");

    // 释放内存
    freeUserCourseRelList(userCourseRels);
    return true;
}



/**
 * 打印某教练负责的当前用户健身课程门数
 * @param coachId 教练ID
 */
bool countCoachCourseByIdForUser(int coachId) {
    // 获取当前登录用户ID
  
    if (CURRENT_USER_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取所有学员课程关系记录
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("当前用户没有选课记录");
        return true;
    }

    // 统计指定教练负责的课程数
    int coachCourseCount = 0;

    // 遍历当前用户的选课记录
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        setFilterIntFlag(currentUserRel->course_id);
        CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCourseId);
        setFilterIntFlag(coachId);
        coachCourseRels = filterCoachCourseRelByCoachId(coachCourseRels);
        clearFilterIntFlag();
        // 遍历教练课程关系，找到该教练负责的匹配课程
        if (coachCourseRels != NULL) {
            coachCourseCount++;
            freeCoachCourseRelList(coachCourseRels);
        }
        currentUserRel = currentUserRel->nextPointer;
    }

    // 打印统计结果
    printf("教练ID: %d 负责当前用户的课程数量: %d\n", coachId, coachCourseCount);

    // 释放内存
    freeUserCourseRelList(userCourseRels);
    return true;
}

// 信息统计

// 系统维护

/**
 * 修改用户密码
 * @param newPassword 新密码
 */
bool changePasswordForUser(char* newPassword) {
    // 获取当前登录的用户ID
    
    if (CURRENT_USER_ID == -1) {
        puts("请先登录");
        return false;
    }

    // 获取当前用户信息
    User* user = selectByIdForUser(CURRENT_USER_ID);
    if (user == NULL) {
        puts("用户信息获取失败");
        return false;
    }

    // 确保新密码不超过最大长度
    if (strlen(newPassword) >= MAX_STR) {
        puts("密码过长，请输入较短的密码");
        free(user);
        return false;
    }

    // 更新用户密码
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(user->password, encryptPassword);
    free(encryptPassword);

    // 更新数据库中的用户信息
    if (updateByIdForUser(user, CURRENT_USER_ID)) {
        puts("密码修改成功");
        free(user);
        return true;
    }
    else {
        puts("密码修改失败");
        free(user);
        return false;
    }
}

// 系统维护
