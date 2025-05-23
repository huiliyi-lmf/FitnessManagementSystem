# 健身管理系统 (Fitness Management System)

## 项目简介

健身管理系统是一个基于C语言开发的课程设计项目，旨在提供健身房会员、教练和管理员之间的信息管理平台。系统采用Windows图形用户界面，实现了用户注册、登录、课程管理、教练管理等功能。

## 系统功能

### 用户角色

系统支持三种用户角色：

1. **管理员**：负责系统管理、用户管理、课程类型管理、教练审核等
2. **教练**：管理自己的课程、查看学员信息等
3. **学员**：浏览课程、报名课程、查看个人信息等

### 主要功能

#### 管理员功能
- 系统管理（修改密码、数据备份与恢复）
- 课程类型管理（创建、更新、删除、显示）
- 教练管理（创建、更新、删除、显示、审核）
- 课程管理（创建、更新、删除、分配教练）
- 信息查询（课程、用户）
- 数据排序和统计

#### 教练功能
- 课程类型管理
- 信息查询（课程、用户）
- 数据排序和统计
- 系统设置（修改密码、登出）

#### 学员功能
- 个人信息管理
- 课程管理（报名、退课）
- 信息查询
- 数据排序和统计

## 技术特点

- 基于Windows API的图形用户界面
- 自定义文件存储系统，实现数据的持久化
- 模块化设计，代码结构清晰
- 实现了基本的数据加密功能
- 支持数据的备份与恢复

## 项目结构

```
├── FitnessManageSystem.sln       # Visual Studio解决方案文件
├── FitnessManageSystem/          # 项目主目录
│   ├── FMSData.bin               # 数据文件
│   ├── admin.c/h                 # 管理员功能实现
│   ├── coach.c/h                 # 教练功能实现
│   ├── fileio.c/h                # 文件IO操作
│   ├── filter.c/h                # 数据过滤功能
│   ├── gui.c/h                   # 图形界面实现
│   ├── main.c                    # 程序入口
│   ├── structs.c/h               # 数据结构定义
│   └── user.c/h                  # 用户功能实现
```

## 数据结构

系统主要包含以下数据结构：

- Admin：管理员信息
- Coach：教练信息
- CourseType：课程类型
- Course：课程信息
- User：用户信息
- 各种关系表：如CoachTypeRel（教练与课程类型关系）、CoachCourseRel（教练与课程关系）、UserCourseRel（用户与课程关系）

## 编译与运行

### 环境要求
- Windows操作系统
- Visual Studio（推荐2019或更高版本）
- C语言编译环境

### 编译步骤
1. 使用Visual Studio打开`FitnessManageSystem.sln`解决方案文件
2. 选择`Debug`或`Release`配置
3. 点击`生成解决方案`
4. 编译完成后，可在`x64/Debug`或`x64/Release`目录下找到可执行文件

### 运行
直接双击可执行文件`FitnessManageSystem.exe`即可运行程序

## 初始账户

首次运行时，系统会自动创建初始数据文件。您可以注册新的管理员、教练或学员账户。

## 注意事项

- 数据存储在`FMSData.bin`文件中，请勿手动修改此文件
- 系统支持数据备份功能，建议定期备份数据

## 项目作者

本项目为C语言程序设计课程设计作业
