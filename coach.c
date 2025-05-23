#define _CRT_SECURE_NO_WARNINGS
#include "coach.h"
#include <string.h>

// ��ǰ��¼�Ľ���ID
int CURRENT_COACH_ID = -1;

/**
 * ������¼
 * @param username �û���
 * @param password ����
 * @return �Ƿ�ɹ�
 */
bool loginForCoach(char* userName, char* password) {
    // ����У��
    if (userName == NULL || password == NULL) {
        warnInfo("��¼ʧ�ܣ��û���������Ϊ��");
        return false;
    }

    // ��ȡ������Ч��¼
    setFilterStrFlag(userName);
    Coach* coachList = selectAllForCoach(filterCoachByName);
    clearFilterStrFlag();
    if (coachList == NULL) {
        warnInfo("��¼ʧ�ܣ��޽����˺�����");
        return false;
    }

    bool loginSuccess = false;
    // ��ȫ�Ƚ��û���������
    if (verifyPassword(password, coachList->password)) {
        loginSuccess = true;
        CURRENT_COACH_ID = coachList->id;
    }
    else {
        warnInfo("��¼ʧ�ܣ��û������������");
        return false;
    }

    // �ͷ������ڴ�
    freeCoachList(coachList);

    // ������ʾ
    if (!loginSuccess) {
        warnInfo("��¼ʧ�ܣ��û������������");
    }

    return loginSuccess;
}

// ��¼

// ������Ϣ����

/**
 * ���ӽ����ó�ָ������Ŀ����
 * @param typeId ��ĿID
 */
bool addNewCoachTypeRel(int typeId) {
    // ��ȡ��ǰ��¼�Ľ���ID
    
    if (CURRENT_COACH_ID == -1) {
        warnInfo("���ȵ�¼");
        return false;
    }

    // �����Ŀ�����Ƿ����
    CourseType* courseType = selectByIdForCourseType(typeId);
    if (courseType == NULL) {
        printf("δ�ҵ�IDΪ %d ����Ŀ����\n", typeId);
        return false;
    }
    free(courseType); // �ͷ��ڴ�

    // ����Ƿ��Ѿ����ڸù�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachTypeRel* existingRel = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    setFilterIntFlag(typeId);
    existingRel = filterCoachTypeRelByCourseTypeId(existingRel);
    clearFilterIntFlag();
    if (existingRel != NULL) {
        puts("�ý����Ѿ��ó�����Ŀ����");
        freeCoachTypeRelList(existingRel); // �ͷ��ڴ�
        return false;
    }

    // �����µĽ�����Ŀ���͹�ϵ
    CoachTypeRel newRel;
    newRel.coach_id = CURRENT_COACH_ID;
    newRel.type_id = typeId;
    newRel.status = FILE_STATUS_EXIST;

    // ���뵽���ݿ�
    if (insertForCoachTypeRel(&newRel)) {
        puts("���ӽ����ó���Ŀ���ͳɹ�");
        return true;
    }
    else {
        puts("���ӽ����ó���Ŀ����ʧ��");
        return false;
    }
}
/**
 * ɾ�������ó�ָ������Ŀ����
 * @param typeId ��ĿID
 */
bool removeCoachTypeRel(int typeId) {
    // ��ȡ��ǰ��¼�Ľ���ID
    
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ���н�����Ŀ���͹�ϵ
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(NULL);
    if (coachTypeRels == NULL) {
        puts("û�н�����Ŀ���͹�ϵ��¼");
        return false;
    }

    setFilterIntFlag(CURRENT_COACH_ID);
    CoachTypeRel* existingRel = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    setFilterIntFlag(typeId);
    existingRel = filterCoachTypeRelByCourseTypeId(existingRel);
    clearFilterIntFlag();

    // ���δ�ҵ���ϵ
    if (existingRel == NULL) {
        printf("δ�ҵ�����IDΪ %d ����Ŀ����IDΪ %d �Ĺ�ϵ\n", CURRENT_COACH_ID, typeId);
        return false;
    } else {
        deleteByIdForCoachTypeRel(existingRel->id);
        // �ͷ��ڴ�
        freeCoachTypeRelList(coachTypeRels);
        return true;
    }
}

// ������Ϣ����

// ��Ϣ��ѯ

/**
 * ��ӡ���е�ǰ��������Ŀγ���Ϣ
 */
bool showCourseInfoForCoach() {
    // ��ȡ��ǰ��¼�Ľ���ID
    
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ���н����γ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("û�н����γ̹�ϵ��¼");
        return true;
    }

    // ���������γ̹�ϵ���ҵ���ǰ��������Ŀγ�
    puts("��ǰ��������Ŀγ���Ϣ��\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        // ��ȡ�γ���Ϣ
        Course* course = selectByIdForCourse(currentRel->courseId);
        if (course != NULL) {
            printLinkForCourse(course);
            freeCourseList(course);
        }
        currentRel = currentRel->nextPointer;
    }

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * ��ѯ��ǰ��������Ŀγ���Ϣ��null��-1��ʾ���Ը�������
 * @param courseName �γ����ƣ�NULL��ʾ���ԣ�
 * @param courseType �γ��������ƣ�NULL��ʾ���ԣ�
 * @param location �γ̵ص㣨NULL��ʾ���ԣ�
 * @param stuMax_low ���ѧ�������ޣ�-1��ʾ���ԣ�
 * @param stuMax_high ���ѧ�������ޣ�-1��ʾ���ԣ�
 * @param price_low �۸����ޣ�-1��ʾ���ԣ�
 * @param price_high �۸����ޣ�-1��ʾ���ԣ�
 * @param time_low ʱ�����ޣ���������-1��ʾ���ԣ�
 * @param time_high ʱ�����ޣ���������-1��ʾ���ԣ�
 */
bool searchCourseInfoForCoach(char* courseName, char* courseType, char* location,
    int stuMax_low, int stuMax_high,
    double price_low, double price_high,
    int time_low, int time_high) {
    // ����¼״̬
    
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ��������Ŀγ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return true;
    }

    // ���ָ���γ��������ƣ��Ȳ�ѯ����ID
    int courseTypeId = -1;
    if (courseType != NULL) {
        setFilterStrFlag(courseType);
        CourseType* typeHeader = selectAllForCourseType(filterCourseTypeByName);
        clearFilterStrFlag();
        if (typeHeader != NULL) {
            courseTypeId = typeHeader->id;
        }
        freeCourseTypeList(typeHeader);
        if (courseTypeId == -1) {
            puts("δ�ҵ�ָ���Ŀγ�����");
            freeCoachCourseRelList(coachCourseRels);
            return true;
        }
    }

    // ���������γ̹�ϵ��ɸѡ���������Ŀγ�
    puts("���������Ŀγ���Ϣ��\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        Course* course = selectByIdForCourse(currentRel->courseId);
        if (course == NULL) {
            currentRel = currentRel->nextPointer;
            continue;
        }

        // Ӧ�ù�������
        bool match = true;

        // �γ�����ƥ�䣨ģ��ƥ�䣬��������NULL��
        if (courseName != NULL && strstr(course->name, courseName) == NULL) {
            match = false;
        }

        // �γ�����ƥ�䣨��������NULL��
        if (courseType != NULL && course->type_id != courseTypeId) {
            match = false;
        }

        // �ص�ƥ�䣨��������NULL��
        if (location != NULL && strcmp(course->location, location) != 0) {
            match = false;
        }

        // ���ѧ��������
        if (stuMax_low != -1 && course->student_max < stuMax_low) match = false;
        if (stuMax_high != -1 && course->student_max > stuMax_high) match = false;

        // �۸�����
        if (price_low != -1 && course->price < price_low) match = false;
        if (price_high != -1 && course->price > price_high) match = false;

        // ʱ�����䣨��������
        if (time_low != -1 && course->time < time_low) match = false;
        if (time_high != -1 && course->time > time_high) match = false;

        // ��ӡƥ��Ŀγ�
        if (match) {
            printLinkForCourse(course);
            found = true;
        }

        free(course);
        currentRel = currentRel->nextPointer;
    }

    if (!found) {
        puts("δ�ҵ����������Ŀγ�");
    }

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * �鿴�����뵱ǰ�����йص�ѧԱ��Ϣ
 */
bool showUserInfoForCoach() {
    // ����¼״̬

    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ���н����γ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return false;
    }

    // ���������γ̹�ϵ���ҵ���ǰ��������Ŀγ�
    puts("�뵱ǰ�����йص�ѧԱ��Ϣ��\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        // ����ѧԱ�γ̹�ϵ���ҵ������ÿγ̵�ѧԱ
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentUserRel = userCourseRels;
        while (currentUserRel != NULL) {
            // ��ȡѧԱ��Ϣ
            User* user = selectByIdForUser(currentUserRel->user_id);
            if (user != NULL) {
                // ��ӡѧԱ��Ϣ
                printLinkForUser(user);
                found = true;

                // �ͷ�ѧԱ�ڴ�
                free(user);
            }
            currentUserRel = currentUserRel->nextPointer;
        }
        freeUserCourseRelList(userCourseRels);
        currentRel = currentRel->nextPointer;
    }

    // ���δ�ҵ�ѧԱ
    if (!found) {
        puts("��ǰ����û����ص�ѧԱ");
    }

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * ��ѯ�뵱ǰ�����йص�ĳѧԱ��Ϣ
 * @param userName �û���
 */
void searchUserInfoForCoach(char* userName) {
    // ����¼״̬
   
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ���н����γ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return true;
    }

    // ���������γ̹�ϵ���ҵ���ǰ��������Ŀγ�
    puts("�뵱ǰ�����йص�ѧԱ��Ϣ��\n--------------------------");
    CoachCourseRel* currentRel = coachCourseRels;
    bool found = false;
    while (currentRel != NULL) {
        // ����ѧԱ�γ̹�ϵ���ҵ������ÿγ̵�ѧԱ
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentUserRel = userCourseRels;
        while (currentUserRel != NULL) {
            // ��ȡѧԱ��Ϣ
            User* user = selectByIdForUser(currentUserRel->user_id);
            if (user != NULL && strcmp(user->name, userName) == 0) {
                // ��ӡѧԱ��Ϣ
                printf("ѧԱID: %d\n", user->id);
                printf("ѧԱ����: %s\n", user->name);
                printf("��ϵ��ʽ: %s\n", user->phone);
                printf("--------------------------\n");
                found = true;

                // �ͷ�ѧԱ�ڴ�
                free(user);
                break; // �ҵ�ƥ��ѧԱ���˳��ڲ�ѭ��
            }
            free(user); // �ͷŲ�ƥ���ѧԱ�ڴ�
            currentUserRel = currentUserRel->nextPointer;
        }
        freeUserCourseRelList(userCourseRels);
        currentRel = currentRel->nextPointer;
    }

    // ���δ�ҵ�ѧԱ
    if (!found) {
        printf("δ�ҵ��뵱ǰ�����йص�ѧԱ: %s\n", userName);
    }

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

// ��Ϣ��ѯ

// ��Ϣ����

/**
 * �Ե�ǰ��������Ŀγ���Ϣ��������
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
bool sortCourseInfoForCoach(int byName, int byTypeId, int byTime, int byLocation,
    int byStuCount, int byStuMax, int byCoachNum, int byPrice) {
    //// ��ȡ��ǰ��¼�Ľ���ID
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ��������Ŀγ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return true;
    }

    // ����Course����
    Course* courses = NULL;
    Course* current = NULL;
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        if (courses == NULL) {
            courses = selectByIdForCourse(currentRel->courseId);
            current = courses;
        } else {
            current->nextPointer = selectByIdForCourse(currentRel->courseId);
            if (current->nextPointer != NULL) {
                current = current->nextPointer;
            }
        }
        currentRel = currentRel->nextPointer;
    }

    // ����
    Course* sortHeader = sortCourseInfo(courses, byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice);

    printLinkForCourse(sortHeader);
    freeCourseList(sortHeader);
    freeCourseList(courses);
    return true;
}

/**
 * �����뵱ǰ�����йص�ѧԱ��Ϣ
 * @param byName �����ƣ�����Ϊ���򣬸���Ϊ����0Ϊ������
 * @param bySelectCourse ����ѡ�γ�����������Ϊ���򣬸���Ϊ����0Ϊ������
 */
bool sortUserInfoForCoach(int byName, int bySelectCourse) {
    // ��ȡ��ǰ��¼�Ľ���ID
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ��������Ŀγ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return true;
    }

    User* users = NULL;
    User* currentUser = NULL;
    CoachCourseRel* coachCourseRel = coachCourseRels;
    while (coachCourseRel != NULL) {
        setFilterIntFlag(coachCourseRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        UserCourseRel* userCourseRel = userCourseRels;
        while (userCourseRel != NULL) {
            User* user = selectByIdForUser(userCourseRel->user_id);
            if (users == NULL) {
                users = user;
                currentUser = users;
            } else {
                currentUser->nextPointer = user;
                if (currentUser->nextPointer != NULL) {
                    currentUser = currentUser->nextPointer;
                }
            }
            userCourseRel = userCourseRel->nextPointer;
        }

        // �ͷ��ڴ�
        freeUserCourseRelList(userCourseRels);
        coachCourseRel = coachCourseRel->nextPointer;
    }

    // ����
    User* sortHeader = sortUserInfo(users, byName, bySelectCourse);
    printLinkForUser(sortHeader);

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    freeUserList(sortHeader);
    freeUserList(users);
    return true;
}
// ��Ϣ����

// ��Ϣͳ��

/**
 * ��ӡ��ǰ�����������Ϳγ̵�ѧԱ����
 */
bool countCourseTypeSelectedForCoach() {
    // ����¼״̬
  
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ���н����ó��Ŀγ�����
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    clearFilterIntFlag();
    
    // ��������
    CoachTypeRel* coachTypeRel = coachTypeRels;
    while (coachTypeRel != NULL) {
        countCourseTypeSelectedByIdForCoach(coachTypeRel->type_id);
        coachTypeRel = coachTypeRel->nextPointer;
    }

    // �ͷ��ڴ�
    freeCoachTypeRelList(coachTypeRels);
    return true;
}

/**
 * ��ӡ��ǰ����ĳ�����Ϳγ̵�ѧԱ����
 * @param courseTypeId �γ�����ID
 */
bool countCourseTypeSelectedByIdForCoach(int courseTypeId) {
    // ����¼״̬
    
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ��ǰ���������пγ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return true;
    }

    int totalStudents = 0;  // ��ѧԱ����
    bool foundCourse = false;

    // ���������γ̹�ϵ
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        // ��ȡ�γ���ϸ��Ϣ
        Course* course = selectByIdForCourse(currentRel->courseId);
        if (course != NULL && course->type_id == courseTypeId) {
            foundCourse = true;
            int courseStudents = 0;

            // ͳ�Ƹÿγ̵�ѧԱ����
            setFilterIntFlag(course->id);
            UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
            clearFilterIntFlag();
            UserCourseRel* currentUserRel = userCourseRels;
            while (currentUserRel != NULL) {
                    courseStudents++;
                currentUserRel = currentUserRel->nextPointer;
            }
            freeUserCourseRelList(userCourseRels);

            // ��ӡ��ǰ�γ̵�ѧԱ����
            printf("�γ�ID: %d\n", course->id);
            printf("�γ�����: %s\n", course->name);
            printf("ѧԱ����: %d\n", courseStudents);
            printf("--------------------------\n");

            totalStudents += courseStudents;
            free(course);  // �ͷſγ��ڴ�
        }
        else if (course != NULL) {
            free(course);  // ��Ŀ�����Ϳγ�ҲҪ�ͷ��ڴ�
        }
        currentRel = currentRel->nextPointer;
    }

    // �������
    if (foundCourse) {
        printf("�γ�����ID: %d\n", courseTypeId);
        printf("��ѧԱ����: %d\n", totalStudents);
    }
    else {
        printf("��ǰ����û�н��ڿγ�����IDΪ %d �Ŀγ�\n", courseTypeId);
    }

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    return true;
}


/**
 * ��ӡ��ǰ��������ѧԱ�μӵĽ���γ̵�����
 */
bool countUserCourseForCoach() {
    // ��ȡ��ǰ��¼�Ľ���ID
   
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ���н����γ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return true;
    }

    // ͳ��ѧԱ�μӵĿγ�����
    int totalCourses = 0; // �ܿγ�����
    int totalStudents = 0; // ��ѧԱ����

    // ���������γ̹�ϵ���ҵ���ǰ��������Ŀγ�
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        // ����ѧԱ�γ̹�ϵ��ͳ�Ƶ�ǰ�γ̵�ѧԱ����
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentUserRel = userCourseRels;
        while (currentUserRel != NULL) {
            totalCourses++;
            currentUserRel = currentUserRel->nextPointer;
        }
        totalStudents++; // ÿ���γ�������һ��ѧԱ
        currentRel = currentRel->nextPointer;
        freeUserCourseRelList(userCourseRels);
    }

    // ��ӡͳ�ƽ��
    puts("��ǰ��������ѧԱ�μӵĽ���γ�����ͳ�ƣ�\n--------------------------");
    printf("�ܿγ�����: %d\n", totalCourses);
    printf("��ѧԱ����: %d\n", totalStudents);
    printf("--------------------------\n");

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * ��ӡ��ǰ����ĳѧԱ�μӵĽ���γ̵�����
 * @param userId ѧԱID
 */
bool countUserCourseByIdForCoach(int userId) {
    // ��ȡ��ǰ��¼�Ľ���ID
  
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ���н����γ̹�ϵ
    setFilterIntFlag(CURRENT_COACH_ID);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("��ǰ����û�в����κογ�");
        return true;
    }

    // ͳ�Ƹ�ѧԱ�μӵĿγ�����
    int totalCourses = 0; // ѧԱ�Ŀγ�����

    // ���������γ̹�ϵ���ҵ���ǰ��������Ŀγ�
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
        // ����ѧԱ�γ̹�ϵ��ͳ�Ƹ�ѧԱ�Ƿ�μ��˵�ǰ�γ�
        setFilterIntFlag(currentRel->courseId);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByCourseId);
        setFilterIntFlag(userId);
        userCourseRels = filterUserCourseRelByUserId(userCourseRels);
        if (userCourseRels != NULL) {
            freeUserCourseRelList(userCourseRels);
            totalCourses++;
        }
        currentRel = currentRel->nextPointer;
    }

    // ��ӡͳ�ƽ��
    puts("��ǰ������ѧԱ�μӵĽ���γ�����ͳ�ƣ�\n--------------------------");
    printf("ѧԱID: %d\n", userId);
    printf("��ѧԱ�μӵĿγ�����: %d\n", totalCourses);
    printf("--------------------------\n");

    // �ͷ��ڴ�
    freeCoachCourseRelList(coachCourseRels);
    return true;
}


// ��Ϣͳ��

// ϵͳά��

/**
 * �޸ĵ�ǰ��¼����������
 * @param newPassword ������
 */
bool changePasswordForCoach(char* newPassword) {
    // ��ȡ��ǰ��¼�Ľ���ID
   
    if (CURRENT_COACH_ID == -1) {
        puts("���ȵ�¼");
        return false;
    }

    // ��ȡ��ǰ������Ϣ
    Coach* coach = selectByIdForCoach(CURRENT_COACH_ID);
    if (coach == NULL) {
        puts("������Ϣ��ȡʧ��");
        return false;
    }

    // �������볤���Ƿ񳬳�����
    if (strlen(newPassword) >= MAX_STR) {
        printf("���볤�Ȳ��ܳ��� %d ���ַ�\n", MAX_STR - 1);
        free(coach);
        return false;
    }

    // ��������
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(coach->password, encryptPassword);
    free(encryptPassword);

    // �������ݿ��еĽ�����Ϣ
    if (updateByIdForCoach(coach, CURRENT_COACH_ID)) {
        puts("�����޸ĳɹ�");
    }
    else {
        puts("�����޸�ʧ�ܣ�������");
    }

    // �ͷ��ڴ�
    free(coach);
    return true;
}
// ϵͳά��
