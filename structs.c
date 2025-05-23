#define _CRT_SECURE_NO_WARNINGS
#include "structs.h"

// 定义一些必要的指针函数，使用宏展开避免代码重复
#define GENERATE_FUNCTION_POINTER(structName) \
void idSetterFor##structName(void* data, int id) { \
	((structName*)data)->id = id; \
} \
void offsetSetterFor##structName(void* data, int offset) { \
	((structName*)data)->nextOffset = offset; \
} \
int offsetGetterFor##structName(void* data) { \
	return ((structName*)data)->nextOffset; \
} \
int idGetterFor##structName(void* data) { \
	return ((structName*)data)->id; \
} \
void pointerSetterFor##structName(void* data, void* pointer) { \
	((structName*)data)->nextPointer = (structName*)pointer; \
} \
void* pointerGetterFor##structName(void* data) { \
	return (void*)(((structName*)data)->nextPointer); \
} \
void statusSetterFor##structName(void* data, FileStatus status) { \
	((structName*)data)->status = status; \
} \
FileStatus statusGetterFor##structName(void* data) {	\
	return ((structName*)data)->status;	\
}

// 定义各表基本CRUD操作的函数实现，使用宏避免代码重复
#define GENERATE_BASE_CRUD_FUNCTION_DEFINITION(tableName, structName) \
bool insertFor##structName(structName* data) { \
	return insert(tableName, data, sizeof(structName), idSetterFor##structName, offsetSetterFor##structName, offsetGetterFor##structName, statusSetterFor##structName); \
} \
int getOffsetByIdFor##structName(int id) { \
	return getOffsetById(tableName, id, sizeof(structName), offsetSetterFor##structName, offsetGetterFor##structName, idGetterFor##structName); \
} \
structName* selectByIdFor##structName(int id) { \
	return (structName*)selectById(tableName, id, sizeof(structName), pointerSetterFor##structName, getOffsetByIdFor##structName); \
} \
bool updateByIdFor##structName(structName* newData, int id) { \
	return updateById(tableName, id, newData, sizeof(structName), getOffsetByIdFor##structName, copyPropertyFor##structName); \
} \
bool deleteByIdFor##structName(int id) { \
	return deleteById(tableName, id, sizeof(structName), getOffsetByIdFor##structName, statusSetterFor##structName); \
} \
structName* selectAllFor##structName(void* (*filter)(void*)) { \
	return (structName*)selectAll(tableName, sizeof(structName), pointerSetterFor##structName, getOffsetByIdFor##structName, pointerGetterFor##structName, statusGetterFor##structName, filter); \
}

// 展开各表需要使用的函数指针
GENERATE_FUNCTION_POINTER(Admin);
GENERATE_FUNCTION_POINTER(Course);
GENERATE_FUNCTION_POINTER(CourseType);
GENERATE_FUNCTION_POINTER(Coach);
GENERATE_FUNCTION_POINTER(CoachCourseRel);
GENERATE_FUNCTION_POINTER(CoachTypeRel);
GENERATE_FUNCTION_POINTER(User);
GENERATE_FUNCTION_POINTER(UserCourseRel);

// copyProperty函数对于不同方法定义不同，需要手动实现
void copyPropertyForAdmin(void* dest, void* src) {
	strcpy(((Admin*)dest)->username, ((Admin*)src)->username);
	strcpy(((Admin*)dest)->password, ((Admin*)src)->password);
}
void copyPropertyForCourse(void* dest, void* src) {
	Course* p1 = (Course*)dest;
	Course* p2 = (Course*)src;
	strcpy(p1->location, p2->location);
	strcpy(p1->name, p2->name);
	p1->price = p2->price;
	p1->student_count = p2->student_count;
	p1->student_max = p2->student_max;
	p1->time = p2->time;
	p1->type_id = p2->type_id;
}
void copyPropertyForCourseType(void* dest, void* src) {
	CourseType* p1 = (CourseType*)dest;
	CourseType* p2 = (CourseType*)src;
	strcpy(p1->name, p2->name);
}
void copyPropertyForCoach(void* dest, void* src) {
	Coach* p1 = (Coach*)dest;
	Coach* p2 = (Coach*)src;
	p1->expertise_status = p2->expertise_status;
	p1->level = p2->level;
	strcpy(p1->name, p2->name);
	strcpy(p1->password, p2->password);
}
void copyPropertyForCoachCourseRel(void* dest, void* src) {
	CoachCourseRel* p1 = (CoachCourseRel*)dest;
	CoachCourseRel* p2 = (CoachCourseRel*)src;
	p1->coachId = p2->coachId;
	p1->courseId = p2->courseId;
}
void copyPropertyForCoachTypeRel(void* dest, void* src) {
	CoachTypeRel* p1 = (CoachTypeRel*)dest;
	CoachTypeRel* p2 = (CoachTypeRel*)src;
	p1->coach_id = p2->coach_id;
	p1->type_id = p2->type_id;
}
void copyPropertyForUser(void* dest, void* src) {
	User* p1 = (User*)dest;
	User* p2 = (User*)src;
	strcpy(p1->name, p2->name);
	strcpy(p1->password, p2->password);
	strcpy(p1->phone, p2->phone);
}
void copyPropertyForUserCourseRel(void* dest, void* src) {
	UserCourseRel* p1 = (UserCourseRel*)dest;
	UserCourseRel* p2 = (UserCourseRel*)src;
	p1->course_id = p2->course_id;
	p1->user_id = p2->user_id;
}

// 展开各表的基本CRUD函数定义
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(AdminTable, Admin);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CourseTable, Course);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CourseTypeTable, CourseType);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CoachTable, Coach);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CoachCourseRelTable, CoachCourseRel);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CoachTypeRelTable, CoachTypeRel);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(UserTable, User);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(UserCourseRelTable, UserCourseRel);


// 数据的打印操作—————————

/**
* 打印Admin链表的数据
* @param header 链表头节点
*/
void printLinkForAdmin(Admin* header) {
	if (header == NULL) {
		puts("AdminLink is NULL");
		return;
	}
	puts("AdminLinkInfo:\n--------------------------");
	while (header != NULL) {
		printf("id: %d\n", header->id);
		printf("username: %s\n", header->username);
		printf("password: %s\n", header->password);
		printf("status: %d\n", header->status);
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

void printLinkForCourse(Course* header) {
	while (header != NULL) {
		printf("课程ID: %d\n", header->id);
		printf("课程名称: %s\n", header->name);
		printf("课程类别ID: %d\n", header->type_id);
		printf("时间: %02d:%02d\n", header->time / 60, header->time % 60); // 转换为时分格式
		printf("地点: %s\n", header->location);
		printf("当前学生数量: %d\n", header->student_count);
		printf("最大学生数量: %d\n", header->student_max);
		printf("价格: %.2lf\n", header->price);
		printf("状态: %s\n", header->status == FILE_STATUS_EXIST ? "有效" : "已删除");
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

void printLinkForCourseType(CourseType* header) {
	while (header != NULL) {
		printf("课程类别ID：%d\n", header->id);
		printf("课程类别名称：%s\n", header->name);
		printf("状态: %s\n", header->status == FILE_STATUS_EXIST ? "有效" : "已删除");
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

void printLinkForUser(User* header) {
	while (header != NULL) {
		printf("学员ID: %d\n", header->id);
		printf("姓名: %s\n", header->name);
		printf("密码: %s\n", header->password);
		printf("联系方式: %s\n", header->phone);
		printf("状态: %s\n", header->status == FILE_STATUS_EXIST ? "有效" : "已删除");
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

// —————————数据的打印操作

// 数据的连表打印操作—————————

// 过滤器定义
int CUREENT_USER_ID_FOR_FILTER = 0;
void* filterForUserCourseRelByUserId(void* header) {
	if (header == NULL) {
		return NULL;
	}
	UserCourseRel* p = (UserCourseRel*)header;
	UserCourseRel* q = p->nextPointer;
	while (p != NULL && p->user_id != CUREENT_USER_ID_FOR_FILTER) {
		q = p->nextPointer;
		free(p);
		p = q;
	}
	if (p == NULL) {
		return NULL;
	}
	UserCourseRel* newHeader = p;
	q = p;
	while (p != NULL) {
		if (p->user_id != CUREENT_USER_ID_FOR_FILTER) {
			q->nextPointer = p->nextPointer;
			free(p);
			p = q->nextPointer;
		} else {
			p = p->nextPointer;
			q = q->nextPointer;
		}
	}
	return (void*)newHeader;
}

void printLinkForCoachDetails(Coach* header) {
	while (header != NULL) {
		// 打印教练基本信息
		printf("教练ID: %d\n", header->id);
		printf("姓名: %s\n", header->name);
		printf("星级: %s\n",
			header->level == COACH_LEVEL_FIVE_STAR ? "五星级" :
			header->level == COACH_LEVEL_THREE_STAR ? "三星级" : "一星级");

		// 获取教练提交的擅长项目类型
		setFilterIntFlag(header->id);
		CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
		clearFilterIntFlag();
		CoachTypeRel* currentRel = coachTypeRels;
		bool hasExpertise = false;

		while (currentRel != NULL) {
			hasExpertise = true;
			CourseType* courseType = selectByIdForCourseType(currentRel->type_id);
			if (courseType != NULL) {
				printf("擅长项目类型: %s\n", courseType->name);
				free(courseType); // 释放 selectByIdForCourseType 返回的内存
			}
			CoachTypeRel* temp = currentRel;
			currentRel = currentRel->nextPointer;
			free(temp); // 释放链表节点内存
		}

		if (!hasExpertise) {
			printf("该教练未提交擅长项目类型\n");
		}
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

void printLinkForUserDetails(User* header) {
	if (header == NULL) {
		puts("UserLink is NULL");
		return;
	}
	puts("UserLinkInfo Details:\n--------------------------");
	while (header != NULL) {
		printf("id: %d\n", header->id);
		printf("name: %s\n", header->name);
		printf("password: %s\n", header->password);
		printf("phone: %s\n", header->phone);
		printf("status: %d\n", header->status);

		CUREENT_USER_ID_FOR_FILTER = header->id;
		UserCourseRel* courseRel = selectAllForUserCourseRel(filterForUserCourseRelByUserId);
		if (courseRel == NULL) {
			puts("this user has no courses");
		} else {
			while (courseRel != NULL) {
				printf("this user select course_id: %d\n", courseRel->course_id);
				printLinkForCourse(selectByIdForCourse(courseRel->course_id));
				courseRel = courseRel->nextPointer;
			}
		}

		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

// —————————数据的连表打印操作

// —————————链表回收函数

#define GENERATE_FREE_LINK_DEFINITION(structName) \
void free##structName##List(structName* header) { \
	while (header != NULL) { \
		structName* temp = header; \
		header = header->nextPointer; \
		free(temp); \
	} \
}

GENERATE_FREE_LINK_DEFINITION(Admin);
GENERATE_FREE_LINK_DEFINITION(Course);
GENERATE_FREE_LINK_DEFINITION(CourseType);
GENERATE_FREE_LINK_DEFINITION(Coach);
GENERATE_FREE_LINK_DEFINITION(CoachCourseRel);
GENERATE_FREE_LINK_DEFINITION(CoachTypeRel);
GENERATE_FREE_LINK_DEFINITION(User);
GENERATE_FREE_LINK_DEFINITION(UserCourseRel);

// 链表回收函数—————————

// 密码处理验证—————————

/**
* 生成密码加密后的字符串
* @param password 密码
* @return 加密后的字符串
*/
char* generateEncryptPassword(char* password) {
	int len = strlen(password);
	char* str = (char*)malloc((len + 1) * sizeof(char));
	if (str == NULL) {
		errorInfo("生成密码加密串失败：操作系统空间不足");
		return NULL;
	}
	memset(str, 0, len * sizeof(char));
	for (int i = 0; i < len; i++) {
		str[i] = password[i] ^ PWD_ENCRPY_VAL;
	}
	str[len] = '\0';
	return str;
}

/**
* 验证密码是否正确
* @param password 密码
* @param encryptPassword 加密后的字符串
* @return 是否正确
*/
bool verifyPassword(char* password, char* encryptPassword) {
	if (strlen(password) != strlen(encryptPassword)) {
		return false;
	}
	int len = strlen(password);
	for (int i = 0; i < len; i++) {
		if ((password[i] ^ PWD_ENCRPY_VAL) != encryptPassword[i]) {
			return false;
		}
	}
	return true;
}

// —————————密码处理验证