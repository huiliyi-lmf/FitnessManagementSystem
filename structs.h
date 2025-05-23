#ifndef __STRUCTS_H
#define __STRUCTS_H

#include "fileio.h"
#include "filter.h"

#define MAX_STR 64
#define MAX_LEN 64

typedef struct Admin {
	int id;								// ����ԱID
	char username[MAX_STR];				// ����Ա�˻�����
	char password[MAX_STR];				// ����Ա����
	union {
		struct Admin* nextPointer;		// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;					// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;					// ����״̬
}Admin;

typedef enum CoachLevel {
	COACH_LEVEL_FIVE_STAR, COACH_LEVEL_THREE_STAR, COACH_LEVEL_ONE_STAR
}CoachLevel;							// �����Ǽ� 0-���Ǽ���1-���Ǽ��� 2-���Ǽ�
typedef enum CoachStatus {
	COACH_STATUS_WAIT_FOR_CHECK, COACH_STATUS_PASS, COACH_STATUS_FAIL
}CoachStatus;							// ���״̬ 0-����1-ͨ����2-δͨ��

typedef struct Coach {
	int id;								// �������
	char name[MAX_STR];					// ��������
	char password[MAX_STR];				// ��������
	CoachLevel level;					// �����Ǽ�
	CoachStatus expertise_status;		// �ó�����Ŀ���״̬
	union {
		struct Coach* nextPointer;		// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;					// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;					// ����״̬
}Coach;

typedef struct CourseType {
	int id;								// ��Ŀ������
	char name[MAX_STR];					// ��Ŀ����
	union {
		struct CourseType* nextPointer;	// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;					// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;					// ����״̬
}CourseType;

typedef struct CoachTypeRel {
	int id;									// ��ϵID
	int coach_id;							// ����ID
	int type_id;							// ��Ŀ����ID
	union {
		struct CoachTypeRel* nextPointer;	// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;						// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;						// ����״̬
}CoachTypeRel;

typedef struct User {
	int id;								// ѧԱID
	char name[MAX_STR];					// ����
	char password[MAX_STR];				// ��¼����
	char phone[MAX_STR];				// ��ϵ��ʽͬʱҲ���һ����뷽ʽ
	union {
		struct User* nextPointer;		// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;					// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;					// ����״̬
}User;

typedef struct Course {
	int id;								// �γ����
	char name[MAX_STR];					// �γ�����
	int type_id;						// �γ�������Ŀ
	int time;							// �����0��0ʱ�ķ�����
	char location[MAX_STR];				// �ص�
	int student_count;					// ��ǰ�γ̱���ѧ������
	int student_max;					// �γ��������ѧ������
	double price;						// �γ̼۸�
	union {
		struct Course* nextPointer;		// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;					// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;					// ����״̬
}Course;

typedef struct CoachCourseRel {
	int id;										// �����γ̹�ϵID
	int coachId;								// ����ID
	int courseId;								// �γ�ID
	union {
		struct CoachCourseRel* nextPointer;		// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;							// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;							// ����״̬
}CoachCourseRel;

typedef struct UserCourseRel {
	int id;										// ��ϵID
	int user_id;								// �û�ID
	int course_id;								// �γ�ID
	union {
		struct UserCourseRel* nextPointer;		// ��һ������ָ�루�ڴ���Ϊ�ڴ��ַ��������ʱΪNULL��
		int nextOffset;							// ��һ������ƫ�ƣ��ļ���Ϊ�ļ�ƫ�ƣ�������Ϊ-1��
	};
	FileStatus status;							// ����״̬
}UserCourseRel;

// ����������CRUD�����ĺ�������
#define GENERATE_BASE_CRUD_FUNCTION_DECLARATION(structName) \
bool insertFor##structName(structName* data); \
int getOffsetByIdFor##structName(int id); \
structName* selectByIdFor##structName(int id); \
bool updateByIdFor##structName(structName* newData, int id); \
bool deleteByIdFor##structName(int id); \
structName* selectAllFor##structName(void* (*filter)(void*)); \
void printLinkFor##structName(structName* header);

// չ���������CRUD�����ĺ�������
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Admin);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Course);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CourseType);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(Coach);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CoachCourseRel);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(CoachTypeRel);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(User);
GENERATE_BASE_CRUD_FUNCTION_DECLARATION(UserCourseRel);

// ���ݵ������ӡ����������������������

void printLinkForCoachDetails(Coach* header);

void printLinkForUserDetails(User* header);

// ���������������������ݵ������ӡ����

// ������������������������պ���

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

// ������պ���������������������

// ���봦����֤������������������

#define PWD_ENCRPY_VAL 0x0

/**
* ����������ܺ���ַ���
* @param password ����
* @return ���ܺ���ַ���
*/
char* generateEncryptPassword(char* password);

/**
* ��֤�����Ƿ���ȷ
* @param password ����
* @param encryptPassword ���ܺ���ַ���
* @return �Ƿ���ȷ
*/
bool verifyPassword(char* password, char* encryptPassword);

// ���������������������봦����֤

#endif // !__STRUCTS_H
