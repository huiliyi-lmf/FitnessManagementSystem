#define _CRT_SECURE_NO_WARNINGS
#include "admin.h"
#include <string.h>

// 当前登录的管理员ID
int CURRENT_ADMIN_ID = -1;

/**
 * 管理员登录
 * @param username 用户名
 * @param password 密码
 * @return 是否成功
 */
bool loginForAdmin(char* username, char* password) {
    // 参数校验
    if (username == NULL || password == NULL) {
        warnInfo("登录失败：用户名或密码为空");
        return false;
    }

    // 获取所有有效管理员
    setFilterStrFlag(username);
    Admin* adminList = selectAllForAdmin(filterAdminByUsername);
    clearFilterStrFlag();
    if (adminList == NULL) {
        warnInfo("登录失败：无管理员账号存在");
        return false;
    }

    bool loginSuccess = false;
    // 安全比较用户名和密码
    if (verifyPassword(password, adminList->password)) {
        loginSuccess = true;
        CURRENT_ADMIN_ID = adminList->id;
    }
    else {
        warnInfo("登录失败：用户名或密码错误");
        return false;
    }

    // 释放分配内存
    freeAdminList(adminList);

    // 错误提示
    if (!loginSuccess) {
        warnInfo("登录失败：用户名或密码错误");
    }

    return loginSuccess;
}

/**
 * 管理员注册
 * @param username 用户名
 * @param password 密码
 * @return 是否成功
 */
bool registerForAdmin(char* username, char* password) {
    // 参数校验
    if (username == NULL || password == NULL) {
        warnInfo("注册失败：用户名或密码为空");
        return false;
    }

    // 检查用户名是否已存在
    setFilterStrFlag(username);
    Admin* existingAdmin = selectAllForAdmin(filterAdminByUsername);
    clearFilterStrFlag();
    if (existingAdmin != NULL) {
        warnInfo("注册失败：用户名已存在");
        freeAdminList(existingAdmin);
        return false;
    }

    // 创建新管理员
    Admin newAdmin;
    strcpy(newAdmin.username, username);
    encryptPassword(password, newAdmin.password);
    newAdmin.status = FILE_STATUS_EXIST;

    // 插入到数据库
    if (insertForAdmin(&newAdmin)) {
        puts("管理员注册成功");
        return true;
    }
    else {
        puts("管理员注册失败");
        return false;
    }
}