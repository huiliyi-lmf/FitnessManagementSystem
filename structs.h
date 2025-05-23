#ifndef __STRUCTS_H
#define __STRUCTS_H

#include "fileio.h"
#include "filter.h"

#define MAX_STR 64
#define MAX_LEN 64

typedef struct Admin {
	int id;						// 管理员ID
	char username[MAX_STR];				// 管理员账户名称
	char password[MAX_STR];				// 管理员密码
	union {
		struct Admin* nextPointer;		// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;				// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;				// 存在状态
}Admin;

typedef enum CoachLevel {
	COACH_LEVEL_FIVE_STAR, COACH_LEVEL_THREE_STAR, COACH_LEVEL_ONE_STAR
}CoachLevel;						// 教练星级 0-五星级，1-三星级， 2-一星级
typedef enum CoachStatus {
	COACH_STATUS_WAIT_FOR_CHECK, COACH_STATUS_PASS, COACH_STATUS_FAIL
}CoachStatus;						// 教练状态 0-待审，1-通过，2-未通过

typedef struct Coach {
	int id;						// 教练编号
	char name[MAX_STR];				// 教练姓名
	char password[MAX_STR];				// 教练密码
	CoachLevel level;				// 教练星级
	CoachStatus expertise_status;		// 该教练项目审核状态
	union {
		struct Coach* nextPointer;		// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;				// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;				// 存在状态
}Coach;

typedef struct CourseType {
	int id;						// 项目类型编号
	char name[MAX_STR];				// 项目名称
	union {
		struct CourseType* nextPointer;	// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;				// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;				// 存在状态
}CourseType;

typedef struct CoachTypeRel {
	int id;							// 关系ID
	int coach_id;						// 教练ID
	int type_id;						// 项目类型ID
	union {
		struct CoachTypeRel* nextPointer;	// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;					// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;					// 存在状态
}CoachTypeRel;

typedef struct User {
	int id;						// 学员ID
	char name[MAX_STR];				// 姓名
	char password[MAX_STR];				// 登录密码
	char phone[MAX_STR];				// 联系方式同时也是一种登录方式
	union {
		struct User* nextPointer;		// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;				// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;				// 存在状态
}User;

typedef struct Course {
	int id;						// 课程编号
	char name[MAX_STR];				// 课程名称
	int type_id;					// 课程所属项目
	int time;					// 相对于0年0时的分钟数
	char location[MAX_STR];				// 地点
	int student_count;				// 当前课程报名学生数量
	int student_max;				// 课程最大允许学生数量
	double price;					// 课程价格
	union {
		struct Course* nextPointer;		// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;				// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;				// 存在状态
}Course;

typedef struct CoachCourseRel {
	int id;								// 教练课程关系ID
	int coachId;						// 教练ID
	int courseId;						// 课程ID
	union {
		struct CoachCourseRel* nextPointer;		// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;						// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;						// 存在状态
}CoachCourseRel;

typedef struct UserCourseRel {
	int id;								// 关系ID
	int user_id;						// 用户ID
	int course_id;						// 课程ID
	union {
		struct UserCourseRel* nextPointer;		// 下一个结点指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;						// 下一个结点偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;						// 存在状态
}UserCourseRel;

// 生成基础数据的CRUD函数声明宏
#define GENERATE_BASE_CRUD_FUNCTION_DECLARATION(structName) \
bool insertFor##structName(structName* data); \
int getOffsetByIdFor##structName(int id); \
structName* selectByIdFor##structName(int id); \
bool updateByIdFor##structName(structName* newData, int id); \
bool deleteByIdFor##structName(int id); \
structName* selectAllFor##structName(void* (*filter)(void*)); \
void printLinkFor##structName(structName* header);

// 展开基础数据的CRUD函数声明宏
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Admin);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Course);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CourseType);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Coach);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CoachCourseRel);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CoachTypeRel);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(User);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(UserCourseRel);

// 数据的详情打印函数声明（带关联数据）

void printLinkForCoachDetails(Coach* header);

void printLinkForUserDetails(User* header);

// 数据详情打印函数声明结束（带关联数据）

// 生成链表内存释放函数声明宏

#define GENERATE_FREE_LINK_DECLARATION(structName) \
void free##structName##List(structName* header);

GENERATE_FREE_LINK_DECLARATION(Admin);
GENERATE_FREE_LINK_DECLARATION(Course);
GENERATE_FREE_LINK_DECLARATION(CourseType);
GENERATE_FREE_LINK_DECLARATION(Coach);
GENERATE_FREE_LINK_DECLARATION(CoachCourseRel);
GENERATE_FREE_LINK_DECLARATION(CoachTypeRel);
GENERATE_FREE_LINK_DECLARATION(User);
GENERATE_FREE_LINK_DECLARATION(UserCourseRel);

// 内存释放函数声明宏结束（带关联数据）

// 密码处理与验证函数声明（带关联数据）

#define PWD_ENCRPY_VAL 0x0

/**
* 生成密码加密后的字符串
* @param password 密码
* @return 加密后的字符串
*/
char* generateEncryptPassword(char* password);

/**
* 验证密码是否正确
* @param password 密码
* @param encryptPassword 加密后的字符串
* @return 是否正确
*/
bool verifyPassword(char* password, char* encryptPassword);

// 密码处理与验证函数声明结束（带关联数据）

#endif // !__STRUCTS_H