#define _CRT_SECURE_NO_WARNINGS
#include "structs.h"

// ����һЩ��Ҫ��ָ�뺯����ʹ�ú�չ����������ظ�
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

// ����������CRUD�����ĺ���ʵ�֣�ʹ�ú��������ظ�
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

// չ��������Ҫʹ�õĺ���ָ��
GENERATE_FUNCTION_POINTER(Admin);
GENERATE_FUNCTION_POINTER(Course);
GENERATE_FUNCTION_POINTER(CourseType);
GENERATE_FUNCTION_POINTER(Coach);
GENERATE_FUNCTION_POINTER(CoachCourseRel);
GENERATE_FUNCTION_POINTER(CoachTypeRel);
GENERATE_FUNCTION_POINTER(User);
GENERATE_FUNCTION_POINTER(UserCourseRel);

// copyProperty�������ڲ�ͬ�������岻ͬ����Ҫ�ֶ�ʵ��
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

// չ������Ļ���CRUD��������
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(AdminTable, Admin);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CourseTable, Course);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CourseTypeTable, CourseType);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CoachTable, Coach);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CoachCourseRelTable, CoachCourseRel);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(CoachTypeRelTable, CoachTypeRel);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(UserTable, User);
GENERATE_BASE_CRUD_FUNCTION_DEFINITION(UserCourseRelTable, UserCourseRel);


// ���ݵĴ�ӡ����������������������

/**
* ��ӡAdmin���������
* @param header ����ͷ�ڵ�
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
		printf("�γ�ID: %d\n", header->id);
		printf("�γ�����: %s\n", header->name);
		printf("�γ����ID: %d\n", header->type_id);
		printf("ʱ��: %02d:%02d\n", header->time / 60, header->time % 60); // ת��Ϊʱ�ָ�ʽ
		printf("�ص�: %s\n", header->location);
		printf("��ǰѧ������: %d\n", header->student_count);
		printf("���ѧ������: %d\n", header->student_max);
		printf("�۸�: %.2lf\n", header->price);
		printf("״̬: %s\n", header->status == FILE_STATUS_EXIST ? "��Ч" : "��ɾ��");
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

void printLinkForCourseType(CourseType* header) {
	while (header != NULL) {
		printf("�γ����ID��%d\n", header->id);
		printf("�γ�������ƣ�%s\n", header->name);
		printf("״̬: %s\n", header->status == FILE_STATUS_EXIST ? "��Ч" : "��ɾ��");
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

void printLinkForUser(User* header) {
	while (header != NULL) {
		printf("ѧԱID: %d\n", header->id);
		printf("����: %s\n", header->name);
		printf("����: %s\n", header->password);
		printf("��ϵ��ʽ: %s\n", header->phone);
		printf("״̬: %s\n", header->status == FILE_STATUS_EXIST ? "��Ч" : "��ɾ��");
		printf("--------------------------\n");
		header = header->nextPointer;
	}
}

// ���������������������ݵĴ�ӡ����

// ���ݵ������ӡ����������������������

// ����������
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
		// ��ӡ����������Ϣ
		printf("����ID: %d\n", header->id);
		printf("����: %s\n", header->name);
		printf("�Ǽ�: %s\n",
			header->level == COACH_LEVEL_FIVE_STAR ? "���Ǽ�" :
			header->level == COACH_LEVEL_THREE_STAR ? "���Ǽ�" : "һ�Ǽ�");

		// ��ȡ�����ύ���ó���Ŀ����
		setFilterIntFlag(header->id);
		CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
		clearFilterIntFlag();
		CoachTypeRel* currentRel = coachTypeRels;
		bool hasExpertise = false;

		while (currentRel != NULL) {
			hasExpertise = true;
			CourseType* courseType = selectByIdForCourseType(currentRel->type_id);
			if (courseType != NULL) {
				printf("�ó���Ŀ����: %s\n", courseType->name);
				free(courseType); // �ͷ� selectByIdForCourseType ���ص��ڴ�
			}
			CoachTypeRel* temp = currentRel;
			currentRel = currentRel->nextPointer;
			free(temp); // �ͷ�����ڵ��ڴ�
		}

		if (!hasExpertise) {
			printf("�ý���δ�ύ�ó���Ŀ����\n");
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

// ���������������������ݵ������ӡ����

// ������������������������պ���

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

// ������պ���������������������

// ���봦����֤������������������

/**
* ����������ܺ���ַ���
* @param password ����
* @return ���ܺ���ַ���
*/
char* generateEncryptPassword(char* password) {
	int len = strlen(password);
	char* str = (char*)malloc((len + 1) * sizeof(char));
	if (str == NULL) {
		errorInfo("����������ܴ�ʧ�ܣ�����ϵͳ�ռ䲻��");
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
* ��֤�����Ƿ���ȷ
* @param password ����
* @param encryptPassword ���ܺ���ַ���
* @return �Ƿ���ȷ
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

// ���������������������봦����֤