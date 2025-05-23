#define _CRT_SECURE_NO_WARNINGS
#include "admin.h"
#include <string.h>

// 当前用户登录的信息
int CURRENT_ADMIN_ID = -1;

/**
 * 登录判断
 * @param userName 用户名
 * @param password 密码
 * @return 密码是否正确
 */
bool loginForAdmin(char* userName, char* password) {
    // 参数校验
    if (userName == NULL || password == NULL) {
        warnInfo("登录失败：用户名或密码为空");
        return false;
    }

    // 获取有效管理员记录
    setFilterStrFlag(userName);
    Admin* adminList = selectAllForAdmin(filterAdminByUsername);
    clearFilterStrFlag();

    if (adminList == NULL) {
        warnInfo("登录失败：无管理员账号数据");
        return false;
    }
    bool loginSuccess = false;
    // 安全比较用户名和密码
    if (strcmp(adminList->username, userName) == 0) {
        if (verifyPassword(password, adminList->password)) {
            loginSuccess = true;
            CURRENT_ADMIN_ID = adminList->id;
        } else {
            errorInfo("登录失败：用户名或密码错误");
            return false;
        }
    }

    // 释放链表内存
    freeAdminList(adminList);
    // 错误提示
    if (!loginSuccess) {
        errorInfo("登录失败：用户名或密码错误");
    }

    return loginSuccess;
}

/**
 * 注册管理员
 * @param userName 用户名
 * @param password 密码
 * @return 是否成功
 */
bool registerForAdmin(char* userName, char* password) {
    // --------------- 参数校验 ---------------
    if (userName == NULL || password == NULL) {
        warnInfo("注册失败：用户名或密码为空");
        return false;
    }
    if (strlen(userName) == 0 || strlen(password) == 0) {
        warnInfo("注册失败：用户名或密码不能为空");
        return false;
    }
    if (strlen(userName) >= MAX_STR || strlen(password) >= MAX_STR) {
        warnInfo("注册失败：用户名或密码长度超过限制");
        return false;
    }

    // --------------- 用户名查重 ---------------
    setFilterStrFlag(userName);
    Admin* adminList = selectAllForAdmin(filterAdminByUsername);  // 获取所有管理员
    clearFilterStrFlag();
    if (adminList != NULL) {
        warnInfo("注册失败：用户名已存在");
        freeAdminList(adminList);  // 释放链表内存
        return false;
    }

    // --------------- 创建管理员对象 ---------------
    Admin newAdmin;
    memset(&newAdmin, 0, sizeof(Admin));
    strcpy(newAdmin.username, userName);
    char* encryptPassword = generateEncryptPassword(password);
    strcpy(newAdmin.password, encryptPassword);
    free(encryptPassword);
    newAdmin.status = FILE_STATUS_EXIST;

    // --------------- 插入数据 ---------------
    if (insertForAdmin(&newAdmin)) {
        return true;
    } else {
        errorInfo("注册失败：数据写入失败");
        return false;
    }
}

// 信息管理：增加，修改，删除————

/**
 * 创建一个新的项目类型
 * @param typeName 项目名称
 */
bool createCourseType(char* typeName) {
    // --------------- 参数校验 ---------------
    if (typeName == NULL || strlen(typeName) == 0) {
        warnInfo("创建失败：项目名称不能为空");
        return false;
    }
    if (strlen(typeName) >= MAX_STR) {
        warnInfo("创建失败：项目名称长度超过限制");
        return false;
    }

    // --------------- 检查项目名称是否重复 ---------------
    setFilterStrFlag(typeName);
    CourseType* typeList = selectAllForCourseType(filterCourseTypeByName); // 获取所有课程类型
    clearFilterStrFlag();
    if (typeList != NULL) {
        warnInfo("创建失败：项目名称已存在");
        freeCourseTypeList(typeList);
        return false;
    }

    // --------------- 创建新的 CourseType 对象 ---------------
    CourseType newType;
    memset(&newType, 0, sizeof(CourseType));
    strcpy(newType.name, typeName);
    newType.status = FILE_STATUS_EXIST;

    // --------------- 插入数据 ---------------
    if (insertForCourseType(&newType)) {
        return true;
    }
    else {
        errorInfo("创建失败：数据写入错误");
        return false;
    }
}

/**
 * 修改某个项目名称
 * @param id 项目编号
 * @param typeName 修改后的名称
 */
bool updateCourseType(int id, char* typeName) {
    if (typeName == NULL || strlen(typeName) == 0) {
        warnInfo("修改项目名称失败：名称为空");
        return false;
    }
    if (strlen(typeName) >= MAX_STR) {
        warnInfo("修改项目名称失败：名称过长");
        return false;
    }

    // 检查新名称是否已存在
    setFilterStrFlag(typeName);
    CourseType* existingTypes = selectAllForCourseType(filterCourseTypeByName);
    clearFilterStrFlag();
    if (existingTypes != NULL && existingTypes->id != id) {
        warnInfo("修改项目名称失败：名称已存在");
        freeCourseTypeList(existingTypes); // 释放链表内存
        return false;
    }

    // 根据ID查找项目类型
    CourseType* targetType = selectByIdForCourseType(id);
    if (targetType == NULL) {
        warnInfo("修改项目名称失败：项目不存在");
        return false;
    }

    // 更新项目名称
    strcpy(targetType->name, typeName);

    // 更新数据到文件
    if (!updateByIdForCourseType(targetType, id)) {
        errorInfo("修改项目名称失败：数据更新失败");
        freeCourseTypeList(targetType);
        return false;
    }

    freeCourseTypeList(targetType); // 释放 selectByIdForCourseType 返回的内存
    return true;
}

/**
 * 删除项目
 * @param id 项目ID
 */
bool deleteCourseType(int id) {
    // 检查项目类型是否存在
    CourseType* targetType = selectByIdForCourseType(id);
    if (targetType == NULL) {
        warnInfo("删除项目类型失败：项目不存在");
        return false;
    }

    // 检查是否有课程依赖该项目类型
    setFilterIntFlag(id);
    Course* courses = selectAllForCourse(filterCourseByCourseTypeId);
    if (courses != NULL) {
        warnInfo("删除项目类型失败：有课程依赖该项目类型");
        freeCourseList(courses); // 释放链表内存
        free(targetType);        // 释放 selectByIdForCourseType 返回的内存
        return false;
    }
    freeCourseList(courses); // 释放链表内存

    // 检查是否有教练擅长该项目类型
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCourseTypeId);
    if (coachTypeRels == NULL) {
        warnInfo("删除项目类型失败：有教练擅长该项目类型");
        freeCoachTypeRelList(coachTypeRels); // 释放链表内存
        free(targetType);                   // 释放 selectByIdForCourseType 返回的内存
        return false;
    }
    freeCoachTypeRelList(coachTypeRels); // 释放链表内存
    clearFilterIntFlag();

    // 删除项目类型
    if (!deleteByIdForCourseType(id)) {
        errorInfo("删除项目类型失败：数据更新失败");
        freeCourseTypeList(targetType);
        return false;
    }

    freeCourseTypeList(targetType); // 释放 selectByIdForCourseType 返回的内存
    return true;
}

/**
 * 创建新的教练账户
 * @param coachName 教练姓名
 * @param coachPassword 教练密码
 */
bool createCoach(char* coachName, char* coachPassword) {
    if (coachName == NULL || coachPassword == NULL || strlen(coachName) == 0 || strlen(coachPassword) == 0) {
        warnInfo("创建教练账户失败：姓名或密码为空");
        return false;
    }

    if (strlen(coachName) >= MAX_STR || strlen(coachPassword) >= MAX_STR) {
        warnInfo("创建教练账户失败：姓名或密码过长");
        return false;
    }

    // 检查教练姓名是否已存在
    setFilterStrFlag(coachName);
    Coach* existingCoaches = selectAllForCoach(filterCoachByName);
    clearFilterStrFlag();
    if (existingCoaches != NULL) {
        warnInfo("创建教练账户失败：姓名已存在");
        freeCoachList(existingCoaches); // 释放链表内存
        return false;
    }

    // 创建新的教练对象
    Coach newCoach;
    memset(&newCoach, 0, sizeof(Coach));
    strcpy(newCoach.name, coachName);
    char* encryptPassword = generateEncryptPassword(coachPassword);
    strcpy(newCoach.password, encryptPassword);
    free(encryptPassword);
    newCoach.level = COACH_LEVEL_ONE_STAR; // 默认星级为一星级
    newCoach.expertise_status = COACH_STATUS_WAIT_FOR_CHECK; // 默认审核状态为待审核
    newCoach.status = FILE_STATUS_EXIST; // 数据状态为存在

    // 插入数据到文件
    if (!insertForCoach(&newCoach)) {
        errorInfo("创建教练账户失败：数据写入失败");
        return false;
    }
    return true;
}

/**
 * 修改教练信息（如果为NULL的项则不修改）
 * @param id 教练ID
 * @param newCoachName 教练姓名
 * @param newCoachPassword  教练密码
 * @param newLevel  教练评级（为-1则不修改）
 */
bool updateCoach(int id, char* newCoachName, char* newCoachPassword, CoachLevel newLevel) {
    // 检查教练是否存在
    Coach* targetCoach = selectByIdForCoach(id);
    if (targetCoach == NULL) {
        warnInfo("修改教练信息失败：教练不存在");
        return false;
    }

    // 检查新姓名是否已存在（如果提供了新姓名）
    if (newCoachName != NULL && strlen(newCoachName) > 0) {
        if (strlen(newCoachName) >= MAX_STR) {
            warnInfo("修改教练信息失败：姓名过长");
            free(targetCoach); // 释放 selectByIdForCoach 返回的内存
            return false;
        }

        setFilterStrFlag(newCoachName);
        Coach* existingCoaches = selectAllForCoach(filterCoachByName);
        clearFilterStrFlag();
        if (existingCoaches != NULL && existingCoaches->id != id) {
            warnInfo("修改教练信息失败：姓名已存在");
            freeCoachList(existingCoaches); // 释放链表内存
            free(targetCoach);             // 释放 selectByIdForCoach 返回的内存
            return false;
        }
    }

    // 创建新的教练对象用于更新
    Coach updatedCoach;
    memcpy(&updatedCoach, targetCoach, sizeof(Coach)); // 复制原数据

    // 更新姓名（如果提供了新姓名）
    if (newCoachName != NULL && strlen(newCoachName) > 0) {
        strcpy(updatedCoach.name, newCoachName);
    }

    // 更新密码（如果提供了新密码）
    if (newCoachPassword != NULL && strlen(newCoachPassword) > 0) {
        if (strlen(newCoachPassword) >= MAX_STR) {
            warnInfo("修改教练信息失败：密码过长");
            free(targetCoach); // 释放 selectByIdForCoach 返回的内存
            return false;
        }
        char* encryptPassword = generateEncryptPassword(newCoachPassword);
        strcpy(updatedCoach.password, encryptPassword);
        free(encryptPassword);
    }

    // 更新星级（如果提供了新星级）
    if (newLevel != -1) {
        updatedCoach.level = newLevel;
    }

    // 更新数据到文件
    if (!updateByIdForCoach(&updatedCoach, id)) {
        errorInfo("修改教练信息失败：数据更新失败");
        free(targetCoach);
        return false;
    }

    free(targetCoach); // 释放 selectByIdForCoach 返回的内存
    return true;
}

/**
 * 删除教练信息
 * @param id 教练ID
 */
bool deleteCoach(int id) {
    // 检查教练是否存在
    Coach* targetCoach = selectByIdForCoach(id);
    if (targetCoach == NULL) {
        warnInfo("删除教练失败：教练不存在");
        return false;
    }

    // 检查是否有课程依赖该教练
    setFilterIntFlag(id);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels != NULL) {
        warnInfo("删除教练失败：有课程依赖该教练");
        freeCoachCourseRelList(coachCourseRels); // 释放链表内存
        free(targetCoach);                      // 释放 selectByIdForCoach 返回的内存
        return false;
    }

    // 检查是否有项目类型依赖该教练
    setFilterIntFlag(id);
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    clearFilterIntFlag();
    if (coachTypeRels != NULL) {
        warnInfo("删除教练失败：有项目类型依赖该教练");
        freeCoachTypeRelList(coachTypeRels); // 释放链表内存
        free(targetCoach);                  // 释放 selectByIdForCoach 返回的内存
        return false;
    }

    // 逻辑删除教练
    if (!deleteByIdForCoach(id)) {
        errorInfo("删除教练失败：数据更新失败");
        free(targetCoach);
        return false;
    }

    free(targetCoach); // 释放 selectByIdForCoach 返回的内存
    return true;
}

// —————信息管理：增加，修改，删除

// 教练管理

// 查看所有教练信息
bool showAllCoachInfo() {
    Coach* coaches = selectAllForCoach(NULL);
    if (coaches == NULL) {
        warnInfo("没有教练信息");
        return true;
    }
    printLinkForCoachDetails(coaches);
    freeCoachList(coaches);
    return true;
}

// 查看所有课程类别信息
bool showAllCourseTypeInfo() {
    CourseType* courseTypes = selectAllForCourseType(NULL);
    if (courseTypes == NULL) {
        warnInfo("没有课程类别信息");
        return true;
    }
    printLinkForCourseType(courseTypes);
    freeCourseTypeList(courseTypes);
    return true;
}

/**
 * 查看教练提交的自己擅长指导的项目类型申请
 */
bool showNeedJudgeCoachInfo() {
    // 获取所有教练
    setFilterIntFlag(COACH_STATUS_WAIT_FOR_CHECK);
    Coach* coaches = selectAllForCoach(filterCoachByExpertiseStatus);
    clearFilterIntFlag();
    if (coaches == NULL) {
        warnInfo("没有需要审核的教练申请");
        return true;
    }

    // 遍历教练链表
    printLinkForCoachDetails(coaches);
    // 释放教练链表内存
    freeCoachList(coaches);
    return true;
}

/**
 * 审核教练信息的结果
 * @param id 教练ID
 * @param status 教练信息
 */
bool markJudgeCoachInfoStatus(int id, CoachStatus status) {
    // 检查教练是否存在
    Coach* targetCoach = selectByIdForCoach(id);
    if (targetCoach == NULL) {
        warnInfo("审核教练信息失败：教练不存在");
        return false;
    }

    // 更新审核状态
    targetCoach->expertise_status = status;

    // 更新数据到文件
    if (!updateByIdForCoach(targetCoach, id)) {
        errorInfo("审核教练信息失败：数据更新失败");
        free(targetCoach);
        return false;
    }
    else {
        const char* statusStr[] = { "待审核", "通过", "未通过" };
        printf("教练信息审核成功！状态：%s\n", statusStr[status]);
    }

    free(targetCoach); // 释放 selectByIdForCoach 返回的内存
    return true;
}

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
bool createCourse(char* courseName, int courseType, int time, char* location, int stuMax, double price) {
    // 参数校验
    if (courseName == NULL || location == NULL || strlen(courseName) == 0 || strlen(location) == 0) {
        warnInfo("创建课程失败：课程名称或地点为空");
        return false;
    }

    if (time < 0) {
        warnInfo("创建课程失败：时间应为非负数");
        return false;
    }

    if (strlen(courseName) >= MAX_STR || strlen(location) >= MAX_STR) {
        errorInfo("创建课程失败：课程名称或地点过长");
        return false;
    }

    if (stuMax <= 0) {
        errorInfo("创建课程失败：最大学生数量必须大于0");
        return false;
    }

    if (price < 0) {
        errorInfo("创建课程失败：价格不能为负数");
        return false;
    }

    // 检查课程类别是否存在
    CourseType* targetCourseType = selectByIdForCourseType(courseType);
    if (targetCourseType == NULL || targetCourseType->status != FILE_STATUS_EXIST) {
        errorInfo("创建课程失败：课程类别不存在");
        return false;
    }
    free(targetCourseType); // 释放 selectByIdForCourseType 返回的内存

    // 创建新的课程对象
    Course newCourse;
    memset(&newCourse, 0, sizeof(Course));
    strcpy(newCourse.name, courseName);
    newCourse.type_id = courseType;
    newCourse.time = time;
    strcpy(newCourse.location, location);
    newCourse.student_max = stuMax;
    newCourse.student_count = 0; // 初始学生数量为0
    newCourse.price = price;
    newCourse.status = FILE_STATUS_EXIST; // 数据状态为存在

    // 插入数据到文件
    if (!insertForCourse(&newCourse)) {
        errorInfo("创建课程失败：数据写入失败");
        return false;
    }
    return true;
}

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
bool updateCourse(int id, char* courseName, int courseType, int time, char* location, int stuMax, double price) {
    // 参数校验
    if (courseName == NULL || location == NULL || strlen(courseName) == 0 || strlen(location) == 0) {
        warnInfo("创建课程失败：课程名称或地点为空");
        return false;
    }

    if (time < 0) {
        warnInfo("创建课程失败：时间应为非负数");
        return false;
    }

    if (strlen(courseName) >= MAX_STR || strlen(location) >= MAX_STR) {
        errorInfo("创建课程失败：课程名称或地点过长");
        return;
    }

    if (stuMax <= 0) {
        errorInfo("创建课程失败：最大学生数量必须大于0");
        return false;
    }

    if (price < 0) {
        errorInfo("创建课程失败：价格不能为负数");
        return false;
    }
    
    // 检查课程是否存在
    Course* targetCourse = selectByIdForCourse(id);
    if (targetCourse == NULL) {
        warnInfo("修改课程信息失败：课程不存在");
        return false;
    }

    // 检查课程类别是否存在
    if (courseType != -1) {
        CourseType* type = selectByIdForCourseType(courseType);
        if (type == NULL || type->status != FILE_STATUS_EXIST) {
            warnInfo("修改课程信息失败：课程类别不存在");
            free(targetCourse); // 释放 selectByIdForCourse 返回的内存
            return false;
        }
        free(type); // 释放 selectByIdForCourseType 返回的内存
    }

    // 创建新的课程对象用于更新
    Course updatedCourse;
    memcpy(&updatedCourse, targetCourse, sizeof(Course)); // 复制原数据

    // 更新课程名称（如果提供了新名称）
    if (courseName != NULL && strlen(courseName) > 0) {
        if (strlen(courseName) >= MAX_STR) {
            warnInfo("修改课程信息失败：课程名称过长");
            free(targetCourse); // 释放 selectByIdForCourse 返回的内存
            return false;
        }
        strcpy(updatedCourse.name, courseName);
    }

    // 更新课程类别（如果提供了新类别）
    if (courseType != -1) {
        updatedCourse.type_id = courseType;
    }

    // 更新时间（如果提供了新时间）
    if (time != -1) {
        updatedCourse.time = time;
    }

    // 更新地点（如果提供了新地点）
    if (location != NULL && strlen(location) > 0) {
        if (strlen(location) >= MAX_STR) {
            warnInfo("修改课程信息失败：地点名称过长");
            free(targetCourse); // 释放 selectByIdForCourse 返回的内存
            return false;
        }
        strcpy(updatedCourse.location, location);
    }

    // 更新最大学生数量（如果提供了新值）
    if (stuMax != -1) {
        if (stuMax < updatedCourse.student_count) {
            warnInfo("修改课程信息失败：最大学生数量不能小于当前学生数量");
            free(targetCourse); // 释放 selectByIdForCourse 返回的内存
            return false;
        }
        updatedCourse.student_max = stuMax;
    }

    // 更新价格（如果提供了新价格）
    if (fabs(price + 1) < 1e-8) {
        updatedCourse.price = price;
    }

    // 更新数据到文件
    if (!updateByIdForCourse(&updatedCourse, id)) {
        errorInfo("修改课程信息失败：数据更新失败");
        free(targetCourse);
        return false;
    }

    free(targetCourse); // 释放 selectByIdForCourse 返回的内存
    return true;
}

// 课程管理

// 分派和调整健身课程的教练

/**
 * 添加教练和课程关系
 * @param courseId 课程ID
 * @param coachId 教练ID
 */
bool addCoachCourseRel(int courseId, int coachId) {
    // 检查课程是否存在
    Course* course = selectByIdForCourse(courseId);
    if (course == NULL) {
        warnInfo("添加教练课程关系失败：课程不存在");
        return false;
    }
    free(course); // 释放 selectByIdForCourse 返回的内存

    // 检查教练是否存在
    Coach* coach = selectByIdForCoach(coachId);
    if (coach == NULL) {
        warnInfo("添加教练课程关系失败：教练不存在");
        return false;
    }
    free(coach); // 释放 selectByIdForCoach 返回的内存

    // 检查关系是否已存在
    setFilterIntFlag(coachId);
    CoachCourseRel* existingRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    setFilterIntFlag(courseId);
    existingRels = filterCoachCourseRelByCourseId(existingRels);
    clearFilterIntFlag();
    if (existingRels != NULL) {
        errorInfo("添加教练课程关系失败：关系已存在");
        freeCoachCourseRelList(existingRels); // 释放链表内存
        return false;
    }

    // 创建新的教练课程关系对象
    CoachCourseRel newRel;
    memset(&newRel, 0, sizeof(CoachCourseRel));
    newRel.courseId = courseId;
    newRel.coachId = coachId;
    newRel.status = FILE_STATUS_EXIST; // 数据状态为存在

    // 插入数据到文件
    if (!insertForCoachCourseRel(&newRel)) {
        errorInfo("添加教练课程关系失败：数据写入失败");
        return false;
    }
    return true;
}

/**
 * 删除教练和课程关系
 * @param courseId 课程ID
 * @param coachId 教练ID
 */
bool deleteCoachCourseRel(int courseId, int coachId) {
    // 获取所有教练课程关系
    setFilterIntFlag(coachId);
    CoachCourseRel* existingRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    setFilterIntFlag(courseId);
    existingRels = filterCoachCourseRelByCourseId(existingRels);
    clearFilterIntFlag();

    // 如果未找到匹配的关系
    if (existingRels == NULL) {
        errorInfo("删除教练课程关系失败：关系不存在");
        freeCoachCourseRelList(existingRels); // 释放链表内存
        return false;
    }

    int relId = existingRels->id;

    // 释放链表内存
    freeCoachCourseRelList(existingRels);

    // 逻辑删除关系
    if (!deleteByIdForCoachCourseRel(relId)) {
        errorInfo("删除教练课程关系失败：数据更新失败");
        return false;
    }
    return true;
}

// 分派和调整健身课程的教练

// 信息查询

/**
 * 打印所有课程信息
 */
bool showCourseInfoForAdmin() {
    // 获取所有课程
    Course* courses = selectAllForCourse(NULL);
    if (courses == NULL) {
        puts("没有课程信息");
        return true;
    }

    // 打印课程信息
    puts("课程信息列表：\n--------------------------");
    printLinkForCourse(courses);
    
    // 释放链表内存
    freeCourseList(courses);
    return true;
}

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
bool searchCourseInfoForAdmin(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high) {
    // 获取所有课程
    Course* courses = selectAllForCourse(NULL);
    if (courses == NULL) {
        puts("没有课程信息");
        return true;
    }

    // 打印查询结果标题
    puts("查询结果：\n--------------------------");

    // 遍历链表，筛选符合条件的课程
    Course* newHeader = NULL;
    Course* currentNew = NULL;
    Course* current = courses;
    while (current != NULL) {
        // 检查课程名称
        if (courseName != NULL && strlen(courseName) > 0 && strstr(current->name, courseName) == NULL) {
            current = current->nextPointer;
            continue;
        }

        // 检查课程类型
        if (courseType != NULL && strlen(courseType) > 0) {
            CourseType* type = selectByIdForCourseType(current->type_id);
            if (type == NULL || strstr(type->name, courseType) == NULL) {
                current = current->nextPointer;
                continue;
            }
            free(type); // 释放 selectByIdForCourseType 返回的内存
        }

        // 检查地点
        if (location != NULL && strlen(location) > 0 && strstr(current->location, location) == NULL) {
            current = current->nextPointer;
            continue;
        }

        // 检查最大学生数量范围
        if (stuMax_low != -1 && current->student_max < stuMax_low) {
            current = current->nextPointer;
            continue;
        }
        if (stuMax_high != -1 && current->student_max > stuMax_high) {
            current = current->nextPointer;
            continue;
        }

        // 检查价格范围
        if (price_low != -1 && current->price < price_low) {
            current = current->nextPointer;
            continue;
        }
        if (price_high != -1 && current->price > price_high) {
            current = current->nextPointer;
            continue;
        }

        // 检查时间范围
        if (time_low != -1 && current->time < time_low) {
            current = current->nextPointer;
            continue;
        }
        if (time_high != -1 && current->time > time_high) {
            current = current->nextPointer;
            continue;
        }

        if (newHeader == NULL) {
            newHeader = (Course*)malloc(sizeof(Course));
            if (newHeader == NULL) {
                puts("操作系统内存不足");
                continue;
            }
            memcpy(newHeader, current, sizeof(Course));
            newHeader->nextPointer = NULL;
            currentNew = newHeader;
        } else {
            currentNew->nextPointer = (Course*)malloc(sizeof(Course));
            if (currentNew->nextPointer == NULL) {
                puts("操作系统内存不足");
                continue;
            }
            memcpy(currentNew->nextPointer, current, sizeof(Course));
            currentNew = currentNew->nextPointer;
            currentNew->nextPointer = NULL;
        }
        current = current->nextPointer;
    }
    printLinkForCourse(newHeader);
    // 释放链表内存
    freeCourseList(courses);
    freeCourseList(newHeader);
    return true;
}

/**
 * 查看所有学员信息
 */
bool showUserInfoForAdmin() {
    // 获取所有学员信息
    User* users = selectAllForUser(NULL);
    if (users == NULL) {
        puts("没有学员信息");
        return true;
    }

    // 打印学员信息
    puts("学员信息列表：\n--------------------------");
    printLinkForUser(users);

    // 释放链表内存
    freeUserList(users);
    return true;
}

/**
 * 查询某学员信息
 * @param userName 用户名
 */
bool searchUserInfoForAdmin(char* userName) {
    if (userName == NULL || strlen(userName) == 0) {
        errorInfo("查询学员信息失败：用户名为空");
        return false;
    }

    // 获取所有学员信息
    setFilterStrFlag(userName);
    User* users = selectAllForUser(filterUserByNameContains);
    clearFilterStrFlag();
    if (users == NULL) {
        puts("没有学员信息");
        return true;
    }

    // 打印查询结果标题
    puts("查询结果：\n--------------------------");
    printLinkForUser(users);
    // 释放链表内存
    freeUserList(users);
    return true;
}

// 信息查询

// 信息排序

// 定义比较函数
int compareCourseByName(const void* a, const void* b) {
    return strcmp(((Course*)a)->name, ((Course*)b)->name);
}
int compareCourseByTypeId(const void* a, const void* b) {
    return ((Course*)a)->type_id - ((Course*)b)->type_id;
}
int compareCourseByTime(const void* a, const void* b) {
    return ((Course*)a)->time - ((Course*)b)->time;
}
int compareCourseByLocation(const void* a, const void* b) {
    return strcmp(((Course*)a)->location, ((Course*)b)->location);
}
int compareCourseByStuCount(const void* a, const void* b) {
    return ((Course*)a)->student_count - ((Course*)b)->student_count;
}
int compareCourseByStuMax(const void* a, const void* b) {
    return ((Course*)a)->student_max - ((Course*)b)->student_max;
}
int compareCourseByCoachNum(const void* a, const void* b) {
    // 计算教练数量
    int countA = 0, countB = 0;
    CoachCourseRel* rels = selectAllForCoachCourseRel(NULL);
    for (CoachCourseRel* rel = rels; rel != NULL; rel = rel->nextPointer) {
        if (rel->courseId == ((Course*)a)->id) countA++;
        if (rel->courseId == ((Course*)b)->id) countB++;
    }
    freeCoachCourseRelList(rels);
    return (countA - countB);
}
int compareCourseByPrice(const void* a, const void* b) {
    double diff = ((Course*)a)->price - ((Course*)b)->price;
    return diff > 0 ? 1 : (diff < 0 ? -1 : 0);
}

// 定义比较函数数组
#define SORT_KEY_COUNT 8

typedef int (*CompareFunc)(const void*, const void*);

const CompareFunc compareFuncs[SORT_KEY_COUNT] = {
    compareCourseByName,     // SORT_BY_NAME
    compareCourseByTypeId,   // SORT_BY_TYPE_ID
    compareCourseByTime,     // SORT_BY_TIME
    compareCourseByLocation, // SORT_BY_LOCATION
    compareCourseByStuCount, // SORT_BY_STU_COUNT
    compareCourseByStuMax,   // SORT_BY_STU_MAX
    compareCourseByCoachNum, // SORT_BY_COACH_NUM
    compareCourseByPrice     // SORT_BY_PRICE
};

// 定义综合比较函数
int compareCourse(const Course* a, const Course* b, const int context[SORT_KEY_COUNT]) {
    CompareFunc compares[SORT_KEY_COUNT];
    int order[SORT_KEY_COUNT];
    memset(order, 0, sizeof(order));
    memset(compares, 0, sizeof(compares));
    int tot = 0;
    for (int i = 1; i <= SORT_KEY_COUNT; i++) {
        for (int j = 0; j < SORT_KEY_COUNT; j++) {
            if (abs(context[j]) == i) {
                order[tot] = context[j] > 0 ? 1 : -1;
                compares[tot] = compareFuncs[j];
                tot++;
            }
        }
    }
    for (int i = 0; i < tot; i++) {
        int result = compares[i](a, b) * order[i];
        if (result != 0) return result > 0 ? 1 : -1;
    }
    return 0;
}

Course* sortCourseInfo(Course* courses, int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice) {
    if (courses == NULL) {
        puts("没有课程信息");
        return NULL;
    }
    int context[SORT_KEY_COUNT];
    context[0] = byName;
    context[1] = byTypeId;
    context[2] = byTime;
    context[3] = byLocation;
    context[4] = byStuCount;
    context[5] = byStuMax;
    context[6] = byCoachNum;
    context[7] = byPrice;
    Course* sortHeader = NULL;
    Course* sortCurrent = NULL;
    // 使用选择排序进行排序
    while (1) {
        Course* first = NULL;
        Course* orgCurrent = courses;
        while (orgCurrent != NULL) {
            if (orgCurrent->status == FILE_STATUS_EXIST) {
                first = orgCurrent;
                orgCurrent = orgCurrent->nextPointer;
                break;
            }
            orgCurrent = orgCurrent->nextPointer;
        }
        if (first == NULL) {
            break;
        }
        while (orgCurrent != NULL) {
            if (compareCourse(orgCurrent, first, context) < 0 && orgCurrent->status == FILE_STATUS_EXIST) {
                first = orgCurrent;
            }
            orgCurrent = orgCurrent->nextPointer;
        }
        if (sortHeader == NULL) {
            sortHeader = (Course*)malloc(sizeof(Course));
            if (sortHeader == NULL) {
                puts("操作系统内存不足");
                return NULL;
            }
            memcpy(sortHeader, first, sizeof(Course));
            sortHeader->nextPointer = NULL;
            sortCurrent = sortHeader;
            first->status = FILE_STATUS_DELETE;
        }
        else {
            sortCurrent->nextPointer = (Course*)malloc(sizeof(Course));
            if (sortCurrent->nextPointer == NULL) {
                puts("操作系统内存不足");
                return NULL;
            }
            memcpy(sortCurrent->nextPointer, first, sizeof(Course));
            sortCurrent = sortCurrent->nextPointer;
            sortCurrent->nextPointer = NULL;
            first->status = FILE_STATUS_DELETE;
        }
    }
    return sortHeader;
}

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
bool sortCourseInfoForAdmin(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice) {
    // 获取所有课程
    Course* courses = selectAllForCourse(NULL);
    Course* sortHeader = sortCourseInfo(courses, byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice);

    // 打印排序结果
    printLinkForCourse(sortHeader);
    freeCourseList(sortHeader);
    freeCourseList(courses);
    return true;
}

/**
 * 获取学员的选课数量
 * @param userId 学员ID
 * @return 选课数量
 */
int getUserCourseCount(int userId) {
    int count = 0;
    setFilterIntFlag(userId);
    UserCourseRel* rels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    UserCourseRel* current = rels;
    while (current != NULL) {
        count++;
        current = current->nextPointer;
    }
    freeUserCourseRelList(rels);
    return count;
}

/**
 * 比较函数（按名称和选课数量排序）
 */
int compareUser(const User* a, const User* b, int byName, int bySelect) {
    int select1 = getUserCourseCount(a->id);
    int select2 = getUserCourseCount(b->id);
    int dif = select1 - select2;
    int res = strcmp(a->name, b->name);
    if (abs(byName) > abs(bySelect)) {
        if (res == 0) {
            if (dif == 0) return 0;
            else return dif > 0 ? 1 : -1;
        }
        return res > 0 ? 1 : -1;
    } else if (abs(byName) < abs(bySelect)) {
        if (dif == 0) {
            if (res == 0) return 0;
            else return res > 0 ? 1 : -1;
        }
        return dif > 0 ? 1 : -1;
    } 
    return 0;
}

User* sortUserInfo(User* users, int byName, int bySelectCourse) {
    if (users == NULL) {
        puts("没有学员信息");
        return NULL;
    }
    User* sortHeader = NULL;
    User* sortCurrent = NULL;
    while (1) {
        User* first = NULL;
        User* header = users;
        while (header != NULL) {
            if (header->status == FILE_STATUS_EXIST) {
                first = header;
                header = header->nextPointer;
                break;
            }
            header = header->nextPointer;
        }
        if (first == NULL) {
            break;
        }
        while (header != NULL) {
            if (header->status == FILE_STATUS_EXIST && compareUser(header, first, byName, bySelectCourse) < 0) {
                first = header;
            }
            header = header->nextPointer;
        }
        if (sortHeader == NULL) {
            sortHeader = (User*)malloc(sizeof(User));
            if (sortHeader == NULL) {
                puts("操作系统内存不足");
                return NULL;
            }
            memcpy(sortHeader, first, sizeof(User));
            sortHeader->nextPointer = NULL;
            sortCurrent = sortHeader;
            first->status = FILE_STATUS_DELETE;
        }
        else {
            sortCurrent->nextPointer = (User*)malloc(sizeof(User));
            if (sortCurrent->nextPointer == NULL) {
                puts("操作系统内存不足");
                return NULL;
            }
            memcpy(sortCurrent->nextPointer, first, sizeof(User));
            sortCurrent = sortCurrent->nextPointer;
            sortCurrent->nextPointer = NULL;
            first->status = FILE_STATUS_DELETE;
        }
    }
    return sortHeader;
}

/**
 * 排序学员信息
 * @param byName 按名称（正数升序，负数降序，0不参与）
 * @param bySelectCourse 按所选课程数量（正数升序，负数降序，0不参与）
 */
bool sortUserInfoForAdmin(int byName, int bySelectCourse) {
    // 获取所有学员信息
    User* users = selectAllForUser(NULL);
    User* sortHeader = sortUserInfo(users, byName, bySelectCourse);

    // 打印用户信息
    printLinkForUser(sortHeader);
    freeUserList(sortHeader);
    freeUserList(users);
    return true;
}

// 信息排序

// 信息统计
/**
 * 打印所有类型课程的学员数量
 */
bool countCourseTypeSelectedForAdmin() {
    // 获取所有课程类型
    CourseType* courseTypes = selectAllForCourseType(NULL);
    if (courseTypes == NULL) {
        puts("没有课程类型信息");
        return true;
    }

    // 遍历列表
    CourseType* header = courseTypes;
    while (header != NULL) {
        countCourseTypeSelectedByIdForAdmin(header->id);
        header = header->nextPointer;
    }

    // 释放内存
    freeCourseTypeList(courseTypes);
    return true;
}

/**
 * 打印某个类型课程的学员数量
 * @param courseTypeId 课程类型ID
 */
bool countCourseTypeSelectedByIdForAdmin(int courseTypeId) {
    // 获取所有课程
    setFilterIntFlag(courseTypeId);
    Course* courses = selectAllForCourse(filterCourseByCourseTypeId);
    clearFilterIntFlag();
    if (courses == NULL) {
        puts("没有课程信息");
        return true;
    }

    // 查找指定课程类型的课程
    Course* currentCourse = courses;
    int totalStudents = 0; // 学员总数

    while (currentCourse != NULL) {
        int count = 0;

        // 遍历选课关系，统计当前课程的学员数量
        setFilterIntFlag(currentCourse->id);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterCoachCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentRel = userCourseRels;
        while (currentRel != NULL) {
            count++;
            currentRel = currentRel->nextPointer;
        }

        // 打印当前课程的学员数量
        printf("课程ID: %d\n", currentCourse->id);
        printf("课程名称: %s\n", currentCourse->name);
        printf("学员数量: %d\n", count);
        printf("--------------------------\n");

        totalStudents += count; // 累加总学员数量

        // 释放内存
        freeUserCourseRelList(userCourseRels);
        currentCourse = currentCourse->nextPointer;
    }

    // 打印该类型课程的总学员数量
    printf("课程类型ID: %d\n", courseTypeId);
    printf("总学员数量: %d\n", totalStudents);
    printf("--------------------------\n");

    // 释放内存
    freeCourseList(courses);
    return true;
}

/**
 * 打印所有教练负责的健身课程门数
 */
bool countCoachCourseForAdmin() {
    // 获取所有教练
    Coach* coaches = selectAllForCoach(NULL);
    if (coaches == NULL) {
        puts("没有教练信息");
        return true;
    }

    // 遍历教练，统计每个教练负责的课程数量
    puts("教练负责的课程数量统计：\n--------------------------");
    Coach* header = coaches;
    while (header != NULL) {
        countCoachCourseByIdForAdmin(header->id);
        header = header->nextPointer;
    }

    // 释放内存
    freeCoachList(coaches);
    return true;
}

/**
 * 打印某教练负责的健身课程门数
 * @param coachId 教练ID
 */
bool countCoachCourseByIdForAdmin(int coachId) {
    // 获取指定教练
    Coach* coach = selectByIdForCoach(coachId);
    if (coach == NULL) {
        printf("未找到ID为 %d 的教练\n", coachId);
        return false;
    }

    // 获取所有教练课程关系
    setFilterIntFlag(coachId);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("没有教练课程关系记录");
        free(coach); // 释放教练内存
        return true;
    }

    int count = 0; // 负责的课程数量

    // 遍历教练课程关系，统计当前教练负责的课程数量
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
            count++;
        currentRel = currentRel->nextPointer;
    }

    // 打印当前教练负责的课程数量
    puts("教练负责的课程数量统计：\n--------------------------");
    printf("教练ID: %d\n", coach->id);
    printf("教练姓名: %s\n", coach->name);
    printf("负责课程数量: %d\n", count);
    printf("--------------------------\n");

    // 释放内存
    free(coach);
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * 打印所有学员参加的健身课程的门数
 */
bool countUserCourseForAdmin() {
    // 获取所有学员
    User* users = selectAllForUser(NULL);
    if (users == NULL) {
        puts("没有学员信息");
        return true;
    }
    User* header = users;
    while (header != NULL) {
        countUserCourseByIdForAdmin(header->id);
        header = header->nextPointer;
    }

    // 释放内存
    freeUserList(users);
    return true;
}

/**
 * 打印某学员参加的健身课程的门数
 * @param userId 学员ID
 */
bool countUserCourseByIdForAdmin(int userId) {
    // 获取指定学员
    User* user = selectByIdForUser(userId);
    if (user == NULL) {
        printf("未找到ID为 %d 的学员\n", userId);
        return false;
    }

    // 获取所有学员课程关系
    setFilterIntFlag(userId);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("没有学员课程关系记录");
        free(user); // 释放学员内存
        return true;
    }

    int count = 0; // 参加的课程数量

    // 遍历学员课程关系，统计当前学员参加的课程数量
    UserCourseRel* currentRel = userCourseRels;
    while (currentRel != NULL) {
        count++;
        currentRel = currentRel->nextPointer;
    }

    // 打印当前学员参加的课程数量
    puts("学员参加的课程数量统计：\n--------------------------");
    printf("学员ID: %d\n", user->id);
    printf("学员姓名: %s\n", user->name);
    printf("参加课程数量: %d\n", count);
    printf("--------------------------\n");

    // 释放内存
    free(user);
    freeUserCourseRelList(userCourseRels);
    return true;
}

// 信息统计

// 系统维护

/**
 * 修改管理员密码（ID由登录函数记录）
 * @param newPassword 新密码
 * @return 是否成功
 */
bool changePasswordForAdmin(char* newPassword) {
    // 假设当前登录的管理员ID已经记录在全局变量 CURRENT_ADMIN_ID 中
 
    // 获取当前管理员信息
    Admin* admin = selectByIdForAdmin(CURRENT_ADMIN_ID);
    if (admin == NULL) {
        puts("未找到当前管理员信息");
        return false;
    }

    // 更新密码
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(admin->password, encryptPassword);
    free(encryptPassword);

    // 更新数据库中的管理员信息
    if (updateByIdForAdmin(admin, CURRENT_ADMIN_ID)) {
        puts("密码修改成功");
        // 释放内存
        free(admin);
        return true;
    }
    else {
        puts("密码修改失败");
        // 释放内存
        free(admin);
        return false;
    }

    
}

/**
 * 修改教练密码
 * @param coachId 教练ID
 * @param newPassword 新密码
 */
bool changeCoachPasswordForAdmin(int coachId, char* newPassword) {
    // 获取指定教练信息
    Coach* coach = selectByIdForCoach(coachId);
    if (coach == NULL) {
        printf("未找到ID为 %d 的教练\n", coachId);
        return false;
    }

    // 更新密码
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(coach->password, encryptPassword);
    free(encryptPassword);

    // 更新数据库中的教练信息
    if (updateByIdForCoach(coach, coachId)) {
        puts("教练密码修改成功");
        // 释放内存
        free(coach);
        return true;
    }
    else {
        puts("教练密码修改失败");
        // 释放内存
        free(coach);
        return false;
    }

    
}

/**
 * 数据备份
 * @param backupName 备份名称
 */
bool databaseBackup(char* backupName) {
    // 打开原始数据文件
    FILE* sourceFile = fopen(DATAFILE_NAME, "rb");
    if (sourceFile == NULL) {
        printf("无法打开原始数据文件: %s\n", DATAFILE_NAME);
        return false;
    }

    // 创建备份文件
    char backupFilePath[MAX_STR];
    snprintf(backupFilePath, MAX_STR, "%s.bak", backupName); // 备份文件路径
    FILE* backupFile = fopen(backupFilePath, "wb");
    if (backupFile == NULL) {
        printf("无法创建备份文件: %s\n", backupFilePath);
        fclose(sourceFile); // 关闭原始文件
        return false;
    }

    // 复制数据
    char buffer[1024];
    size_t bytesRead;
    while (bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) {
        fwrite(buffer, 1, bytesRead, backupFile);
    }

    // 关闭文件
    fclose(sourceFile);
    fclose(backupFile);

    printf("数据备份成功，备份文件: %s\n", backupFilePath);
    return true;
}

/**
 * 数据恢复
 * @param backupName 备份名称
 */
bool databaseRestore(char* backupName) {
    // 构建备份文件路径
    char backupFilePath[MAX_STR];
    snprintf(backupFilePath, MAX_STR, "%s.bak", backupName); // 备份文件路径

    // 打开备份文件
    FILE* backupFile = fopen(backupFilePath, "rb");
    if (backupFile == NULL) {
        printf("无法打开备份文件: %s\n", backupFilePath);
        return false;
    }

    // 打开原始数据文件
    FILE* sourceFile = fopen(DATAFILE_NAME, "wb");
    if (sourceFile == NULL) {
        printf("无法打开原始数据文件: %s\n", DATAFILE_NAME);
        fclose(backupFile); // 关闭备份文件
        return false;
    }

    // 复制数据
    char buffer[1024];
    size_t bytesRead;
    while (bytesRead = fread(buffer, 1, sizeof(buffer), backupFile)) {
        fwrite(buffer, 1, bytesRead, sourceFile);
    }

    // 关闭文件
    fclose(backupFile);
    fclose(sourceFile);

    printf("数据恢复成功，从备份文件: %s\n", backupFilePath);
    return true;
}