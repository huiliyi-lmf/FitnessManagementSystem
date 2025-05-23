#ifndef __STRUCTS_H
#define __STRUCTS_H

#include "fileio.h"
#include "filter.h"

#define MAX_STR 64
#define MAX_LEN 64

typedef struct Admin {
	int id;								// 管理员ID
	char username[MAX_STR];				// 管理员账户名称
	char password[MAX_STR];				// 管理员密码
	union {
		struct Admin* nextPointer;		// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;					// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;					// 数据状态
}Admin;

typedef enum CoachLevel {
	COACH_LEVEL_FIVE_STAR, COACH_LEVEL_THREE_STAR, COACH_LEVEL_ONE_STAR
}CoachLevel;							// 教练星级 0-五星级，1-四星级， 2-三星级
typedef enum CoachStatus {
	COACH_STATUS_WAIT_FOR_CHECK, COACH_STATUS_PASS, COACH_STATUS_FAIL
}CoachStatus;							// 审核状态 0-待审，1-通过，2-未通过

typedef struct Coach {
	int id;								// 教练编号
	char name[MAX_STR];					// 教练姓名
	char password[MAX_STR];				// 教练密码
	CoachLevel level;					// 教练星级
	CoachStatus expertise_status;		// 擅长的项目审核状态
	union {
		struct Coach* nextPointer;		// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;					// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;					// 数据状态
}Coach;

typedef struct CourseType {
	int id;								// 项目类别序号
	char name[MAX_STR];					// 项目名称
	union {
		struct CourseType* nextPointer;	// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;					// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;					// 数据状态
}CourseType;

typedef struct CoachTypeRel {
	int id;									// 关系ID
	int coach_id;							// 教练ID
	int type_id;							// 项目类型ID
	union {
		struct CoachTypeRel* nextPointer;	// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;						// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;						// 数据状态
}CoachTypeRel;

typedef struct User {
	int id;								// 学员ID
	char name[MAX_STR];					// 姓名
	char password[MAX_STR];				// 登录密码
	char phone[MAX_STR];				// 联系方式同时也是找回密码方式
	union {
		struct User* nextPointer;		// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;					// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;					// 数据状态
}User;

typedef struct Course {
	int id;								// 课程序号
	char name[MAX_STR];					// 课程姓名
	int type_id;						// 课程所属项目
	int time;							// 相对于0点0时的分钟数
	char location[MAX_STR];				// 地点
	int student_count;					// 当前课程报名学生数量
	int student_max;					// 课程最多容纳学生数量
	double price;						// 课程价格
	union {
		struct Course* nextPointer;		// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;					// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;					// 数据状态
}Course;

typedef struct CoachCourseRel {
	int id;										// 教练课程关系ID
	int coachId;								// 教练ID
	int courseId;								// 课程ID
	union {
		struct CoachCourseRel* nextPointer;		// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;							// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;							// 数据状态
}CoachCourseRel;

typedef struct UserCourseRel {
	int id;										// 关系ID
	int user_id;								// 用户ID
	int course_id;								// 课程ID
	union {
		struct UserCourseRel* nextPointer;		// 下一个数据指针（内存中为内存地址，不存在时为NULL）
		int nextOffset;							// 下一个数据偏移（文件中为文件偏移，不存在为-1）
	};
	FileStatus status;							// 数据状态
}UserCourseRel;

// 定义各表基本CRUD操作的函数声明
#define GENERATE_BASE_CRUD_FUNCTION_DECLARATION(structName) \
bool insertFor##structName(structName* data); \
int getOffsetByIdFor##structName(int id); \
structName* selectByIdFor##structName(int id); \
bool updateByIdFor##structName(structName* newData, int id); \
bool deleteByIdFor##structName(int id); \
structName* selectAllFor##structName(void* (*filter)(void*)); \
void printLinkFor##structName(structName* header);

// 展开各表基本CRUD操作的函数声明
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Admin);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Course);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CourseType);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Coach);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CoachCourseRel);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CoachTypeRel);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(User);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(UserCourseRel);

// 数据的连表打印操作—————————

void printLinkForCoachDetails(Coach* header);

void printLinkForUserDetails(User* header);

// —————————数据的连表打印操作

// —————————链表回收函数

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

// 链表回收函数—————————

// 密码处理验证—————————

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

// —————————密码处理验证

#endif // !__STRUCTS_H
