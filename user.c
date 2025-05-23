#define _CRT_SECURE_NO_WARNINGS
#include "user.h"
#include <stdlib.h>

int CURRENT_USER_ID = -1;

// ������Ϣ����

/**
 * �����ֻ��Ų�ѯѧԱ��Ϣ
 * @param phone �ֻ���
 * @return ���ز��ҵ���ѧԱ��Ϣ��User*��������������򷵻� NULL
 */
User* selectByPhoneForUser(char* phone) {
    // У���ֻ����Ƿ�Ϊ��
    if (phone == NULL) {
        return NULL;
    }

    // ��ȡ���ݿ��е�ѧԱ�б�
    setFilterStrFlag(phone);
    User* userList = selectAllForUser(filterUserByPhone);
    clearFilterStrFlag();
    if (userList == NULL) {
        return NULL;
    }
    return userList;
}

/**
 * ѧԱע��
 * @param name ����
 * @param password ����
 * @param phone �绰
 * @return �Ƿ�ɹ�
 */
bool registerForUser(char* name, char* password, char* phone) {
    // У������Ƿ�Ϊ��
    if (name == NULL || password == NULL || phone == NULL) {
        warnInfo("ע����Ϣ����Ϊ��");
        return false;
    }

    // У���ַ��������Ƿ񳬳�����
    if (strlen(name) >= MAX_STR || strlen(password) >= MAX_STR || strlen(phone) >= MAX_STR) {
        warnInfo("������Ϣ����");
        return false;
    }

    // ����ֻ����Ƿ���ע��
    User* checkPhone = selectByPhoneForUser(phone);
    if (checkPhone != NULL) {
        warnInfo("���ֻ����ѱ�ע��");
        freeUserList(checkPhone);
        return false;
    }

    // ������ѧԱ����
    User* newUser = (User*)malloc(sizeof(User));
    if (newUser == NULL) {
        warnInfo("�ڴ����ʧ��");
        return false;
    }

    // ��ֵ�û���Ϣ
    memset(newUser, 0, sizeof(User));
    newUser->status = FILE_STATUS_EXIST;
    strcpy(newUser->name, name);
    char* encryptPassword = generateEncryptPassword(password);
    strcpy(newUser->password, encryptPassword);
    free(encryptPassword);
    strcpy(newUser->phone, phone);

    // �������ݿ�
    if (insertForUser(newUser)) {
        warnInfo("ע��ɹ�");
        // �ͷ��ڴ�
        free(newUser);
        return true;
    } else {
        errorInfo("ע��ʧ�ܣ�������");
        // �ͷ��ڴ�
        free(newUser);
        return false;
    }
}

/**
 * ��¼
 * @param name ����
 * @param password ����
 * @param phone �绰��
 * @return �Ƿ�ɹ�
 */
bool loginForUser(char* name, char* password, char* phone) {
    // ����У��
    if (name == NULL || password == NULL || phone == NULL) {
        warnInfo("������Ϣ����Ϊ��");
        return false;
    }

    // �����ֻ��Ų�ѯ�û���Ϣ
    User* user = selectByPhoneForUser(phone);
    if (user == NULL) {
        warnInfo("���ֻ���δע��");
        return false;
    }

    // ��֤�û����������Ƿ�ƥ��
    if (strcmp(user->name, name) != 0) {
        errorInfo("�û�������");
        free(user);
        return false;
    }

    if (!verifyPassword(password, user->password)) {
        errorInfo("�������");
        free(user);
        return false;
    }

    // �����ڴ˴�����ȫ�ֱ����洢��ǰ�û� ID
    CURRENT_USER_ID = user->id;

    free(user);
    return true;
}

/**
 * �޸���Ϣ��ΪNULL���޸ģ�
 * @param name ����
 * @param phone �绰
 */
bool changeInfoForUser(char* name, char* phone) {
    // ��ȡ��ǰ��¼�û�ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("���ȵ�¼");
        return false;
    }

    // ����ID��ȡ�û���Ϣ
    User* user = selectByIdForUser(CURRENT_USER_ID);
    if (user == NULL) {
        warnInfo("��ȡ�û���Ϣʧ��");
        return false;
    }

    // �޸����������������NULL�����鳤�Ⱥ���£�
    if (name != NULL) {
        if (strlen(name) >= MAX_STR) {
            warnInfo("�������ȹ���");
            free(user);
            return false;
        }
        strcpy(user->name, name);
    }

    // �޸ĵ绰�����������NULL�����鳤�Ⱥ���£�
    if (phone != NULL) {
        if (strlen(phone) >= MAX_STR) {
            warnInfo("�绰���ȹ���");
            free(user);
            return false;
        }
        User* exists = selectByPhoneForUser(phone);
        if (exists != NULL && exists->id != CURRENT_USER_ID) {
            warnInfo("�õ绰�ѱ�������ʹ��");
            free(user);
            free(exists);
            return false;
        }
        strcpy(user->phone, phone);
    }

    // �������ݿ��е��û���Ϣ
    if (updateByIdForUser(user, CURRENT_USER_ID)) {
        warnInfo("�û���Ϣ�޸ĳɹ�");
        // �ͷ��ڴ�
        free(user);
        return true;
    } else {
        errorInfo("�û���Ϣ�޸�ʧ��");
        // �ͷ��ڴ�
        free(user);
        return false;
    }
}

// ������Ϣ����

// �γ̱�������

/**
 * ����Լ����ϵĿγ�
 * @param courseId �γ�ID
 */
bool addUserCourseRel(int courseId) {
    // ��ȡ��ǰ��¼�û�ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("���ȵ�¼");
        return false;
    }

    // ���γ��Ƿ����
    Course* course = selectByIdForCourse(courseId);
    if (course == NULL) {
        warnInfo("ָ���Ŀγ�ID������\n");
        return false;
    }
    // �жϿγ��Ƿ�����
    if (course->student_count >= course->student_max) {
        warnInfo("�ÿγ��Ѵﵽѡ�˵����ޣ�����ѡ��");
        freeCourseList(course);
        return false;
    }

    // ����Ƿ���ڹ�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* exists = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    setFilterIntFlag(courseId);
    exists = filterUserCourseRelByCourseId(exists);
    clearFilterIntFlag();
    if (exists != NULL) {
        warnInfo("�Ѵ�����Ӧ�Ĺ�ϵ��¼");
        freeUserCourseRelList(exists);
        freeCourseList(course);
        return true;
    }

    // �޸�Course���student_count��¼
    course->student_count++;
    updateByIdForCourse(course, courseId);
    freeCourseList(course);

    // �����µ�ѧԱ�γ̹�ϵ��¼
    UserCourseRel* newRel = (UserCourseRel*)malloc(sizeof(UserCourseRel));
    if (newRel == NULL) {
        errorInfo("�ڴ����ʧ��");
        return false;
    }

    // ��ֵ����ֶ�
    newRel->user_id = CURRENT_USER_ID;
    newRel->course_id = courseId;
    newRel->status = FILE_STATUS_EXIST;
    newRel->nextPointer = NULL;  // �¼�¼����һ��ָ����ΪNULL

    // ���뵽���ݱ���
    if (insertForUserCourseRel(newRel)) {
        warnInfo("��ӿγ̳ɹ�");
        // �ͷ��ڴ�
        free(newRel);
        return true;
    }
    else {
        errorInfo("��ӿγ�ʧ��");
        // �ͷ��ڴ�
        free(newRel);
        return false;
    }
}

/**
 * ɾ���Լ����ϵĿγ�
 * @param courseId �γ�ID
 */
bool deleteUserCourseRel(int courseId) {
    // ��ȡ��ǰ��¼�û�ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("���ȵ�¼");
        return false;
    }

    // ����Ƿ���ڹ�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* exists = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    setFilterIntFlag(courseId);
    exists = filterUserCourseRelByCourseId(exists);
    clearFilterIntFlag();

    if (exists != NULL) {
        if (!deleteByIdForUserCourseRel(exists->id)) {
            errorInfo("ɾ����¼ʧ��");
            freeUserCourseRelList(exists);
            return false;
        }
        freeUserCourseRelList(exists);
        return true;
    } else {
        warnInfo("��������Ӧ��ѡ�μ�¼");
        return true;;
    }
}

// �γ̱�������

// ��Ϣ��ѯ

/**
 * ��ӡ��ǰ�û����пγ���Ϣ
 */
bool showCourseInfoForUser() {
    // ��ȡ��ǰ��¼�û�ID
   
    if (CURRENT_USER_ID == -1) {
        warnInfo("���ȵ�¼");
        return false;
    }

    // ��ȡ����ѧԱ�γ̹�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* relList = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (relList == NULL) {
        warnInfo("��ǰ�û�û��ѡ�μ�¼");
        return true;
    }

    int count = 0;
    UserCourseRel* p = relList;
    while (p != NULL) {
        // ��ȡ�γ���Ϣ
        Course* course = selectByIdForCourse(p->course_id);
        if (course != NULL) {
            printLinkForCourse(course);  // ��ӡ�ÿγ���Ϣ
            freeCourseList(course);
            count++;
        }
        p = p->nextPointer;
    }

    if (count == 0) {
        warnInfo("��ǰ�û�û��ѡ�μ�¼");
    }

    freeUserCourseRelList(relList);
    return true;
}

/**
 * ��ѯ��ǰ�û��γ���Ϣ��null����-1����Ŀ��˵����ʹ�����������
 * @param courseName �γ����ƣ�֧�ֲ���ƥ�䣬����NULL��մ�����Դ�������
 * @param courseType �γ����ͣ�����NULL��մ�����Դ�������
 * @param location �γ̵ص㣨����NULL��մ�����Դ�������
 * @param stuMax_low ���ѧ���������䣬-1��ʾ���ԣ�
 * @param stuMax_high ���ѧ���������䣬-1��ʾ���ԣ�
 * @param price_low �۸������䣬-1��ʾ���ԣ�
 * @param price_high �۸������䣬-1��ʾ���ԣ�
 * @param time_low ʱ�䣨�����䣬-1��ʾ���ԣ�
 * @param time_high ʱ�䣨�����䣬-1��ʾ���ԣ�
 */
bool searchCourseInfoForUser(char* courseName, char* courseType, char* location,
    int stuMax_low, int stuMax_high, double price_low, double price_high,
    int time_low, int time_high) {
    // ��ȡ��ǰ��¼�û�ID
    
    if (CURRENT_USER_ID == -1) {
        warnInfo("���ȵ�¼");
        return false;
    }

    // ��ȡ����ѧԱ�γ̹�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* relList = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (relList == NULL) {
        warnInfo("��ǰ�û�û��ѡ�μ�¼");
        return true;
    }

    int count = 0;
    UserCourseRel* p = relList;
    while (p != NULL) {
        // ��ȡ�ÿγ���Ϣ
        Course* course = selectByIdForCourse(p->course_id);
        if (course != NULL) {
            bool match = true;

            // ���γ����ƹ��ˣ�����ƥ�䣩
            if (courseName != NULL && strlen(courseName) > 0) {
                if (strstr(course->name, courseName) == NULL) {
                    match = false;
                }
            }
            // ���ص���ˣ�����ƥ�䣩
            if (match && location != NULL && strlen(location) > 0) {
                if (strstr(course->location, location) == NULL) {
                    match = false;
                }
            }
            // ��ѧ����������
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
            // ���۸����
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
            // ��ʱ�����
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
            // ���γ����͹��ˣ����ȡCourseType��Ϣ����ƥ�䣩
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

            // �������������ƥ�䣬��ӡ�ÿγ���Ϣ
            if (match) {
                printLinkForCourse(course);
                count++;
            }
            free(course);
        }
        p = p->nextPointer;
    }

    if (count == 0) {
        warnInfo("δ�ҵ����������Ŀγ�");
    }

    freeUserCourseRelList(relList);
    return true;
}

// ��Ϣ��ѯ

// ��Ϣ����

/**
 * �Ե�ǰ�û��γ���Ϣ��������
 * ÿ���ؼ��ֵ�ֵΪ����ʱ��˵��Ϊ��������Ϊ����ʱ��Ϊ��������
 * ����ؼ��ֵ�ֵΪ0�򲻲������򣬹ؼ��ֵľ���ֵԽС����˵�����ȶ�Խ��
 * ����ֵ����Ĺؼ��ֵľ���ֵ��Ӧ����ͬ
 * @param byName   ������
 * @param byTypeId ������ID
 * @param byTime ��ʱ��
 * @param byLocation ���ص�
 * @param byStuCount ����ѡѧ������
 * @param byStuMax �����ѧ������
 * @param byCoachNum ����������
 * @param byPrice ���۸�
 */
bool sortCourseInfoForUser(int byName, int byTypeId, int byTime, int byLocation,
    int byStuCount, int byStuMax, int byCoachNum, int byPrice) {
    // ��ȡ��ǰ��¼ID
    if (CURRENT_USER_ID == -1) {
        warnInfo("���ȵ�¼");
        return false;
    }
    
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();

    if (userCourseRels == NULL) {
        warnInfo("��ǰ�û�������ѡ����Ϣ");
        return true;
    }

    // ����Course����
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

    // ����
    Course* sortHeader = sortCourseInfo(courses, byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice);

    // ��ʾ���
    printLinkForCourse(sortHeader);
    freeCourseList(sortHeader);
    freeCourseList(courses);
    return true;
}

// ��Ϣ����

// ��Ϣͳ��

/**
 * ��ӡ��ǰ�û��������Ϳγ̵�ѧԱ����
 */
bool countCourseTypeSelectedForUser() {
    // ��ȡ��ǰ��¼�û�ID

    if (CURRENT_USER_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡѧԱ�γ̹�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("��ǰ�û�û��ѡ�μ�¼");
        return true;
    }

    // ͳ�ƿγ����Ͷ�Ӧ��ѧԱ����
    typedef struct TypeCount {
        int typeId;
        int studentCount;
        struct TypeCount* next;
    } TypeCount;

    TypeCount* head = NULL;

    // ������ǰ�û��Ŀγ�
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        // ��ȡ�γ���Ϣ
        Course* course = selectByIdForCourse(currentUserRel->course_id);
        if (course != NULL) {
            int typeId = course->type_id;
            free(course);

            // ��鵱ǰ�����Ƿ�����ͳ��������
            TypeCount* typeNode = head;
            while (typeNode != NULL) {
                if (typeNode->typeId == typeId) {
                    typeNode->studentCount++;
                    break;
                }
                typeNode = typeNode->next;
            }

            // ���δ�ҵ�������ͳ�ƽڵ�
            if (typeNode == NULL) {
                TypeCount* newNode = (TypeCount*)malloc(sizeof(TypeCount));
                if (newNode == NULL) {
                    puts("����ϵͳ�ڴ治��");
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

    // ���ͳ�ƽ��
    puts("��ǰ�û���ѡ�γ����Ͷ�Ӧ��ѧԱ����ͳ�ƣ�");
    puts("--------------------------------------");

    if (head == NULL) {
        puts("��ǰ�û�û��ѡ�μ�¼");
    }
    else {
        TypeCount* typeNode = head;
        while (typeNode != NULL) {
            // ��ȡ�γ���������
            CourseType* typeInfo = selectByIdForCourseType(typeNode->typeId);
            if (typeInfo != NULL) {
                printf("�γ�����: %s, ѡ��ѧԱ����: %d\n", typeInfo->name, typeNode->studentCount);
                free(typeInfo);
            }
            typeNode = typeNode->next;
        }
    }

    puts("--------------------------------------");

    // �ͷ��ڴ�
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
 * ��ӡ��ǰ�û�ĳ�����Ϳγ̵�ѧԱ����
 * @param courseTypeId �γ�����ID
 */
bool countCourseTypeSelectedByIdForUser(int courseTypeId) {
    // ��ȡ��ǰ��¼�û�ID
    
    if (CURRENT_USER_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ����ѧԱ�γ̹�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("��ǰ�û�û��ѡ�μ�¼");
        return true;
    }

    // ͳ�Ƹ����Ϳγ̵�ѧԱ����
    int studentCount = 0;

    // ������ǰ�û��Ŀγ�
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        // ��ȡ�γ���Ϣ
        Course* course = selectByIdForCourse(currentUserRel->course_id);
        if (course != NULL) {
            if (course->type_id == courseTypeId) {
                studentCount++;
            }
            free(course);
        }
        currentUserRel = currentUserRel->nextPointer;
    }

    // ��ȡ�γ���������
    CourseType* courseType = selectByIdForCourseType(courseTypeId);
    if (courseType != NULL) {
        printf("�γ�����: %s, ѡ��ѧԱ����: %d\n", courseType->name, studentCount);
        free(courseType);
    }
    else {
        printf("δ�ҵ�IDΪ %d �Ŀγ�����\n", courseTypeId);
    }

    // �ͷ��ڴ�
    freeUserCourseRelList(userCourseRels);
    return true;
}

/**
 * ��ӡ���н�������ǰ�û��Ľ���γ�����
 */
bool countCoachCourseForUser() {
    // ��ȡ��ǰ��¼�û�ID
    
    if (CURRENT_USER_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡѧԱ�γ̹�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("��ǰ�û�û��ѡ�μ�¼");
        return true;
    }

    // ͳ�ƽ�������Ŀγ���
    int coachCourseCount = 0;
    int coachIds[100] = { 0 }; // ��������� MAX_COACHES ������
    int coachCount = 0;

    // ������ǰ�û���ѡ�μ�¼
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        // ���������γ̹�ϵ���ҵ���Ӧ�γ̵Ľ���
        setFilterIntFlag(currentUserRel->course_id);
        CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCourseId);
        clearFilterIntFlag();
        CoachCourseRel* currentCoachRel = coachCourseRels;
        while (currentCoachRel != NULL) {
            // ���ý����Ƿ��ѱ�ͳ��
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

    // ��ӡͳ�ƽ��
    puts("��ǰ�û��Ŀγ������½�������\n--------------------------");
    printf("�ܽ�������: %d\n", coachCount);
    printf("�ܿγ�����: %d\n", coachCourseCount);
    printf("--------------------------\n");

    // �ͷ��ڴ�
    freeUserCourseRelList(userCourseRels);
    return true;
}



/**
 * ��ӡĳ��������ĵ�ǰ�û�����γ�����
 * @param coachId ����ID
 */
bool countCoachCourseByIdForUser(int coachId) {
    // ��ȡ��ǰ��¼�û�ID
  
    if (CURRENT_USER_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ����ѧԱ�γ̹�ϵ��¼
    setFilterIntFlag(CURRENT_USER_ID);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("��ǰ�û�û��ѡ�μ�¼");
        return true;
    }

    // ͳ��ָ����������Ŀγ���
    int coachCourseCount = 0;

    // ������ǰ�û���ѡ�μ�¼
    UserCourseRel* currentUserRel = userCourseRels;
    while (currentUserRel != NULL) {
        setFilterIntFlag(currentUserRel->course_id);
        CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCourseId);
        setFilterIntFlag(coachId);
        coachCourseRels = filterCoachCourseRelByCoachId(coachCourseRels);
        clearFilterIntFlag();
        // ���������γ̹�ϵ���ҵ��ý��������ƥ��γ�
        if (coachCourseRels != NULL) {
            coachCourseCount++;
            freeCoachCourseRelList(coachCourseRels);
        }
        currentUserRel = currentUserRel->nextPointer;
    }

    // ��ӡͳ�ƽ��
    printf("����ID: %d ����ǰ�û��Ŀγ�����: %d\n", coachId, coachCourseCount);

    // �ͷ��ڴ�
    freeUserCourseRelList(userCourseRels);
    return true;
}

// ��Ϣͳ��

// ϵͳά��

/**
 * �޸��û�����
 * @param newPassword ������
 */
bool changePasswordForUser(char* newPassword) {
    // ��ȡ��ǰ��¼���û�ID
    
    if (CURRENT_USER_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ��ǰ�û���Ϣ
    User* user = selectByIdForUser(CURRENT_USER_ID);
    if (user == NULL) {
        puts("�û���Ϣ��ȡʧ��");
        return false;
    }

    // ȷ�������벻������󳤶�
    if (strlen(newPassword) >= MAX_STR) {
        puts("���������������϶̵�����");
        free(user);
        return false;
    }

    // �����û�����
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(user->password, encryptPassword);
    free(encryptPassword);

    // �������ݿ��е��û���Ϣ
    if (updateByIdForUser(user, CURRENT_USER_ID)) {
        puts("�����޸ĳɹ�");
        free(user);
        return true;
    }
    else {
        puts("�����޸�ʧ��");
        free(user);
        return false;
    }
}

// ϵͳά��
