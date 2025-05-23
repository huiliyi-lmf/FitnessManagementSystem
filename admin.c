#define _CRT_SECURE_NO_WARNINGS
#include "admin.h"
#include <string.h>

// ��ǰ�û���¼����Ϣ
int CURRENT_ADMIN_ID = -1;

/**
 * ��¼�ж�
 * @param userName �û���
 * @param password ����
 * @return �����Ƿ���ȷ
 */
bool loginForAdmin(char* userName, char* password) {
    // ����У��
    if (userName == NULL || password == NULL) {
        warnInfo("��¼ʧ�ܣ��û���������Ϊ��");
        return false;
    }

    // ��ȡ��Ч����Ա��¼
    setFilterStrFlag(userName);
    Admin* adminList = selectAllForAdmin(filterAdminByUsername);
    clearFilterStrFlag();

    if (adminList == NULL) {
        warnInfo("��¼ʧ�ܣ��޹���Ա�˺�����");
        return false;
    }
    bool loginSuccess = false;
    // ��ȫ�Ƚ��û���������
    if (strcmp(adminList->username, userName) == 0) {
        if (verifyPassword(password, adminList->password)) {
            loginSuccess = true;
            CURRENT_ADMIN_ID = adminList->id;
        } else {
            errorInfo("��¼ʧ�ܣ��û������������");
            return false;
        }
    }

    // �ͷ������ڴ�
    freeAdminList(adminList);
    // ������ʾ
    if (!loginSuccess) {
        errorInfo("��¼ʧ�ܣ��û������������");
    }

    return loginSuccess;
}

/**
 * ע�����Ա
 * @param userName �û���
 * @param password ����
 * @return �Ƿ�ɹ�
 */
bool registerForAdmin(char* userName, char* password) {
    // --------------- ����У�� ---------------
    if (userName == NULL || password == NULL) {
        warnInfo("ע��ʧ�ܣ��û���������Ϊ��");
        return false;
    }
    if (strlen(userName) == 0 || strlen(password) == 0) {
        warnInfo("ע��ʧ�ܣ��û��������벻��Ϊ��");
        return false;
    }
    if (strlen(userName) >= MAX_STR || strlen(password) >= MAX_STR) {
        warnInfo("ע��ʧ�ܣ��û��������볤�ȳ�������");
        return false;
    }

    // --------------- �û������� ---------------
    setFilterStrFlag(userName);
    Admin* adminList = selectAllForAdmin(filterAdminByUsername);  // ��ȡ���й���Ա
    clearFilterStrFlag();
    if (adminList != NULL) {
        warnInfo("ע��ʧ�ܣ��û����Ѵ���");
        freeAdminList(adminList);  // �ͷ������ڴ�
        return false;
    }

    // --------------- ��������Ա���� ---------------
    Admin newAdmin;
    memset(&newAdmin, 0, sizeof(Admin));
    strcpy(newAdmin.username, userName);
    char* encryptPassword = generateEncryptPassword(password);
    strcpy(newAdmin.password, encryptPassword);
    free(encryptPassword);
    newAdmin.status = FILE_STATUS_EXIST;

    // --------------- �������� ---------------
    if (insertForAdmin(&newAdmin)) {
        return true;
    } else {
        errorInfo("ע��ʧ�ܣ�����д��ʧ��");
        return false;
    }
}

// ��Ϣ�������ӣ��޸ģ�ɾ����������

/**
 * ����һ���µ���Ŀ����
 * @param typeName ��Ŀ����
 */
bool createCourseType(char* typeName) {
    // --------------- ����У�� ---------------
    if (typeName == NULL || strlen(typeName) == 0) {
        warnInfo("����ʧ�ܣ���Ŀ���Ʋ���Ϊ��");
        return false;
    }
    if (strlen(typeName) >= MAX_STR) {
        warnInfo("����ʧ�ܣ���Ŀ���Ƴ��ȳ�������");
        return false;
    }

    // --------------- �����Ŀ�����Ƿ��ظ� ---------------
    setFilterStrFlag(typeName);
    CourseType* typeList = selectAllForCourseType(filterCourseTypeByName); // ��ȡ���пγ�����
    clearFilterStrFlag();
    if (typeList != NULL) {
        warnInfo("����ʧ�ܣ���Ŀ�����Ѵ���");
        freeCourseTypeList(typeList);
        return false;
    }

    // --------------- �����µ� CourseType ���� ---------------
    CourseType newType;
    memset(&newType, 0, sizeof(CourseType));
    strcpy(newType.name, typeName);
    newType.status = FILE_STATUS_EXIST;

    // --------------- �������� ---------------
    if (insertForCourseType(&newType)) {
        return true;
    }
    else {
        errorInfo("����ʧ�ܣ�����д�����");
        return false;
    }
}

/**
 * �޸�ĳ����Ŀ����
 * @param id ��Ŀ���
 * @param typeName �޸ĺ������
 */
bool updateCourseType(int id, char* typeName) {
    if (typeName == NULL || strlen(typeName) == 0) {
        warnInfo("�޸���Ŀ����ʧ�ܣ�����Ϊ��");
        return false;
    }
    if (strlen(typeName) >= MAX_STR) {
        warnInfo("�޸���Ŀ����ʧ�ܣ����ƹ���");
        return false;
    }

    // ����������Ƿ��Ѵ���
    setFilterStrFlag(typeName);
    CourseType* existingTypes = selectAllForCourseType(filterCourseTypeByName);
    clearFilterStrFlag();
    if (existingTypes != NULL && existingTypes->id != id) {
        warnInfo("�޸���Ŀ����ʧ�ܣ������Ѵ���");
        freeCourseTypeList(existingTypes); // �ͷ������ڴ�
        return false;
    }

    // ����ID������Ŀ����
    CourseType* targetType = selectByIdForCourseType(id);
    if (targetType == NULL) {
        warnInfo("�޸���Ŀ����ʧ�ܣ���Ŀ������");
        return false;
    }

    // ������Ŀ����
    strcpy(targetType->name, typeName);

    // �������ݵ��ļ�
    if (!updateByIdForCourseType(targetType, id)) {
        errorInfo("�޸���Ŀ����ʧ�ܣ����ݸ���ʧ��");
        freeCourseTypeList(targetType);
        return false;
    }

    freeCourseTypeList(targetType); // �ͷ� selectByIdForCourseType ���ص��ڴ�
    return true;
}

/**
 * ɾ����Ŀ
 * @param id ��ĿID
 */
bool deleteCourseType(int id) {
    // �����Ŀ�����Ƿ����
    CourseType* targetType = selectByIdForCourseType(id);
    if (targetType == NULL) {
        warnInfo("ɾ����Ŀ����ʧ�ܣ���Ŀ������");
        return false;
    }

    // ����Ƿ��пγ���������Ŀ����
    setFilterIntFlag(id);
    Course* courses = selectAllForCourse(filterCourseByCourseTypeId);
    if (courses != NULL) {
        warnInfo("ɾ����Ŀ����ʧ�ܣ��пγ���������Ŀ����");
        freeCourseList(courses); // �ͷ������ڴ�
        free(targetType);        // �ͷ� selectByIdForCourseType ���ص��ڴ�
        return false;
    }
    freeCourseList(courses); // �ͷ������ڴ�

    // ����Ƿ��н����ó�����Ŀ����
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCourseTypeId);
    if (coachTypeRels == NULL) {
        warnInfo("ɾ����Ŀ����ʧ�ܣ��н����ó�����Ŀ����");
        freeCoachTypeRelList(coachTypeRels); // �ͷ������ڴ�
        free(targetType);                   // �ͷ� selectByIdForCourseType ���ص��ڴ�
        return false;
    }
    freeCoachTypeRelList(coachTypeRels); // �ͷ������ڴ�
    clearFilterIntFlag();

    // ɾ����Ŀ����
    if (!deleteByIdForCourseType(id)) {
        errorInfo("ɾ����Ŀ����ʧ�ܣ����ݸ���ʧ��");
        freeCourseTypeList(targetType);
        return false;
    }

    freeCourseTypeList(targetType); // �ͷ� selectByIdForCourseType ���ص��ڴ�
    return true;
}

/**
 * �����µĽ����˻�
 * @param coachName ��������
 * @param coachPassword ��������
 */
bool createCoach(char* coachName, char* coachPassword) {
    if (coachName == NULL || coachPassword == NULL || strlen(coachName) == 0 || strlen(coachPassword) == 0) {
        warnInfo("���������˻�ʧ�ܣ�����������Ϊ��");
        return false;
    }

    if (strlen(coachName) >= MAX_STR || strlen(coachPassword) >= MAX_STR) {
        warnInfo("���������˻�ʧ�ܣ��������������");
        return false;
    }

    // �����������Ƿ��Ѵ���
    setFilterStrFlag(coachName);
    Coach* existingCoaches = selectAllForCoach(filterCoachByName);
    clearFilterStrFlag();
    if (existingCoaches != NULL) {
        warnInfo("���������˻�ʧ�ܣ������Ѵ���");
        freeCoachList(existingCoaches); // �ͷ������ڴ�
        return false;
    }

    // �����µĽ�������
    Coach newCoach;
    memset(&newCoach, 0, sizeof(Coach));
    strcpy(newCoach.name, coachName);
    char* encryptPassword = generateEncryptPassword(coachPassword);
    strcpy(newCoach.password, encryptPassword);
    free(encryptPassword);
    newCoach.level = COACH_LEVEL_ONE_STAR; // Ĭ���Ǽ�Ϊһ�Ǽ�
    newCoach.expertise_status = COACH_STATUS_WAIT_FOR_CHECK; // Ĭ�����״̬Ϊ�����
    newCoach.status = FILE_STATUS_EXIST; // ����״̬Ϊ����

    // �������ݵ��ļ�
    if (!insertForCoach(&newCoach)) {
        errorInfo("���������˻�ʧ�ܣ�����д��ʧ��");
        return false;
    }
    return true;
}

/**
 * �޸Ľ�����Ϣ�����ΪNULL�������޸ģ�
 * @param id ����ID
 * @param newCoachName ��������
 * @param newCoachPassword  ��������
 * @param newLevel  ����������Ϊ-1���޸ģ�
 */
bool updateCoach(int id, char* newCoachName, char* newCoachPassword, CoachLevel newLevel) {
    // �������Ƿ����
    Coach* targetCoach = selectByIdForCoach(id);
    if (targetCoach == NULL) {
        warnInfo("�޸Ľ�����Ϣʧ�ܣ�����������");
        return false;
    }

    // ����������Ƿ��Ѵ��ڣ�����ṩ����������
    if (newCoachName != NULL && strlen(newCoachName) > 0) {
        if (strlen(newCoachName) >= MAX_STR) {
            warnInfo("�޸Ľ�����Ϣʧ�ܣ���������");
            free(targetCoach); // �ͷ� selectByIdForCoach ���ص��ڴ�
            return false;
        }

        setFilterStrFlag(newCoachName);
        Coach* existingCoaches = selectAllForCoach(filterCoachByName);
        clearFilterStrFlag();
        if (existingCoaches != NULL && existingCoaches->id != id) {
            warnInfo("�޸Ľ�����Ϣʧ�ܣ������Ѵ���");
            freeCoachList(existingCoaches); // �ͷ������ڴ�
            free(targetCoach);             // �ͷ� selectByIdForCoach ���ص��ڴ�
            return false;
        }
    }

    // �����µĽ����������ڸ���
    Coach updatedCoach;
    memcpy(&updatedCoach, targetCoach, sizeof(Coach)); // ����ԭ����

    // ��������������ṩ����������
    if (newCoachName != NULL && strlen(newCoachName) > 0) {
        strcpy(updatedCoach.name, newCoachName);
    }

    // �������루����ṩ�������룩
    if (newCoachPassword != NULL && strlen(newCoachPassword) > 0) {
        if (strlen(newCoachPassword) >= MAX_STR) {
            warnInfo("�޸Ľ�����Ϣʧ�ܣ��������");
            free(targetCoach); // �ͷ� selectByIdForCoach ���ص��ڴ�
            return false;
        }
        char* encryptPassword = generateEncryptPassword(newCoachPassword);
        strcpy(updatedCoach.password, encryptPassword);
        free(encryptPassword);
    }

    // �����Ǽ�������ṩ�����Ǽ���
    if (newLevel != -1) {
        updatedCoach.level = newLevel;
    }

    // �������ݵ��ļ�
    if (!updateByIdForCoach(&updatedCoach, id)) {
        errorInfo("�޸Ľ�����Ϣʧ�ܣ����ݸ���ʧ��");
        free(targetCoach);
        return false;
    }

    free(targetCoach); // �ͷ� selectByIdForCoach ���ص��ڴ�
    return true;
}

/**
 * ɾ��������Ϣ
 * @param id ����ID
 */
bool deleteCoach(int id) {
    // �������Ƿ����
    Coach* targetCoach = selectByIdForCoach(id);
    if (targetCoach == NULL) {
        warnInfo("ɾ������ʧ�ܣ�����������");
        return false;
    }

    // ����Ƿ��пγ������ý���
    setFilterIntFlag(id);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels != NULL) {
        warnInfo("ɾ������ʧ�ܣ��пγ������ý���");
        freeCoachCourseRelList(coachCourseRels); // �ͷ������ڴ�
        free(targetCoach);                      // �ͷ� selectByIdForCoach ���ص��ڴ�
        return false;
    }

    // ����Ƿ�����Ŀ���������ý���
    setFilterIntFlag(id);
    CoachTypeRel* coachTypeRels = selectAllForCoachTypeRel(filterCoachTypeRelByCoachId);
    clearFilterIntFlag();
    if (coachTypeRels != NULL) {
        warnInfo("ɾ������ʧ�ܣ�����Ŀ���������ý���");
        freeCoachTypeRelList(coachTypeRels); // �ͷ������ڴ�
        free(targetCoach);                  // �ͷ� selectByIdForCoach ���ص��ڴ�
        return false;
    }

    // �߼�ɾ������
    if (!deleteByIdForCoach(id)) {
        errorInfo("ɾ������ʧ�ܣ����ݸ���ʧ��");
        free(targetCoach);
        return false;
    }

    free(targetCoach); // �ͷ� selectByIdForCoach ���ص��ڴ�
    return true;
}

// ������������Ϣ�������ӣ��޸ģ�ɾ��

// ��������

// �鿴���н�����Ϣ
bool showAllCoachInfo() {
    Coach* coaches = selectAllForCoach(NULL);
    if (coaches == NULL) {
        warnInfo("û�н�����Ϣ");
        return true;
    }
    printLinkForCoachDetails(coaches);
    freeCoachList(coaches);
    return true;
}

// �鿴���пγ������Ϣ
bool showAllCourseTypeInfo() {
    CourseType* courseTypes = selectAllForCourseType(NULL);
    if (courseTypes == NULL) {
        warnInfo("û�пγ������Ϣ");
        return true;
    }
    printLinkForCourseType(courseTypes);
    freeCourseTypeList(courseTypes);
    return true;
}

/**
 * �鿴�����ύ���Լ��ó�ָ������Ŀ��������
 */
bool showNeedJudgeCoachInfo() {
    // ��ȡ���н���
    setFilterIntFlag(COACH_STATUS_WAIT_FOR_CHECK);
    Coach* coaches = selectAllForCoach(filterCoachByExpertiseStatus);
    clearFilterIntFlag();
    if (coaches == NULL) {
        warnInfo("û����Ҫ��˵Ľ�������");
        return true;
    }

    // ������������
    printLinkForCoachDetails(coaches);
    // �ͷŽ��������ڴ�
    freeCoachList(coaches);
    return true;
}

/**
 * ��˽�����Ϣ�Ľ��
 * @param id ����ID
 * @param status ������Ϣ
 */
bool markJudgeCoachInfoStatus(int id, CoachStatus status) {
    // �������Ƿ����
    Coach* targetCoach = selectByIdForCoach(id);
    if (targetCoach == NULL) {
        warnInfo("��˽�����Ϣʧ�ܣ�����������");
        return false;
    }

    // �������״̬
    targetCoach->expertise_status = status;

    // �������ݵ��ļ�
    if (!updateByIdForCoach(targetCoach, id)) {
        errorInfo("��˽�����Ϣʧ�ܣ����ݸ���ʧ��");
        free(targetCoach);
        return false;
    }
    else {
        const char* statusStr[] = { "�����", "ͨ��", "δͨ��" };
        printf("������Ϣ��˳ɹ���״̬��%s\n", statusStr[status]);
    }

    free(targetCoach); // �ͷ� selectByIdForCoach ���ص��ڴ�
    return true;
}

// ��������

// �γ̹���

/**
 * ����һ���µĿγ�
 * @param courseName �γ�����
 * @param courseType �γ����
 * @param time ʱ��
 * @param location �ص�
 * @param stuMax ���ѧ������
 * @param price �۸�
 */
bool createCourse(char* courseName, int courseType, int time, char* location, int stuMax, double price) {
    // ����У��
    if (courseName == NULL || location == NULL || strlen(courseName) == 0 || strlen(location) == 0) {
        warnInfo("�����γ�ʧ�ܣ��γ����ƻ�ص�Ϊ��");
        return false;
    }

    if (time < 0) {
        warnInfo("�����γ�ʧ�ܣ�ʱ��ӦΪ�Ǹ���");
        return false;
    }

    if (strlen(courseName) >= MAX_STR || strlen(location) >= MAX_STR) {
        errorInfo("�����γ�ʧ�ܣ��γ����ƻ�ص����");
        return false;
    }

    if (stuMax <= 0) {
        errorInfo("�����γ�ʧ�ܣ����ѧ�������������0");
        return false;
    }

    if (price < 0) {
        errorInfo("�����γ�ʧ�ܣ��۸���Ϊ����");
        return false;
    }

    // ���γ�����Ƿ����
    CourseType* targetCourseType = selectByIdForCourseType(courseType);
    if (targetCourseType == NULL || targetCourseType->status != FILE_STATUS_EXIST) {
        errorInfo("�����γ�ʧ�ܣ��γ���𲻴���");
        return false;
    }
    free(targetCourseType); // �ͷ� selectByIdForCourseType ���ص��ڴ�

    // �����µĿγ̶���
    Course newCourse;
    memset(&newCourse, 0, sizeof(Course));
    strcpy(newCourse.name, courseName);
    newCourse.type_id = courseType;
    newCourse.time = time;
    strcpy(newCourse.location, location);
    newCourse.student_max = stuMax;
    newCourse.student_count = 0; // ��ʼѧ������Ϊ0
    newCourse.price = price;
    newCourse.status = FILE_STATUS_EXIST; // ����״̬Ϊ����

    // �������ݵ��ļ�
    if (!insertForCourse(&newCourse)) {
        errorInfo("�����γ�ʧ�ܣ�����д��ʧ��");
        return false;
    }
    return true;
}

/**
 * �޸Ŀγ���Ϣ
 * @param id �γ�ID
 * @param courseName �γ�����
 * @param courseType �γ����
 * @param time ʱ��
 * @param location �ص�
 * @param stuMax ���ѧ������
 * @param price �۸�
 */
bool updateCourse(int id, char* courseName, int courseType, int time, char* location, int stuMax, double price) {
    // ����У��
    if (courseName == NULL || location == NULL || strlen(courseName) == 0 || strlen(location) == 0) {
        warnInfo("�����γ�ʧ�ܣ��γ����ƻ�ص�Ϊ��");
        return false;
    }

    if (time < 0) {
        warnInfo("�����γ�ʧ�ܣ�ʱ��ӦΪ�Ǹ���");
        return false;
    }

    if (strlen(courseName) >= MAX_STR || strlen(location) >= MAX_STR) {
        errorInfo("�����γ�ʧ�ܣ��γ����ƻ�ص����");
        return;
    }

    if (stuMax <= 0) {
        errorInfo("�����γ�ʧ�ܣ����ѧ�������������0");
        return false;
    }

    if (price < 0) {
        errorInfo("�����γ�ʧ�ܣ��۸���Ϊ����");
        return false;
    }
    
    // ���γ��Ƿ����
    Course* targetCourse = selectByIdForCourse(id);
    if (targetCourse == NULL) {
        warnInfo("�޸Ŀγ���Ϣʧ�ܣ��γ̲�����");
        return false;
    }

    // ���γ�����Ƿ����
    if (courseType != -1) {
        CourseType* type = selectByIdForCourseType(courseType);
        if (type == NULL || type->status != FILE_STATUS_EXIST) {
            warnInfo("�޸Ŀγ���Ϣʧ�ܣ��γ���𲻴���");
            free(targetCourse); // �ͷ� selectByIdForCourse ���ص��ڴ�
            return false;
        }
        free(type); // �ͷ� selectByIdForCourseType ���ص��ڴ�
    }

    // �����µĿγ̶������ڸ���
    Course updatedCourse;
    memcpy(&updatedCourse, targetCourse, sizeof(Course)); // ����ԭ����

    // ���¿γ����ƣ�����ṩ�������ƣ�
    if (courseName != NULL && strlen(courseName) > 0) {
        if (strlen(courseName) >= MAX_STR) {
            warnInfo("�޸Ŀγ���Ϣʧ�ܣ��γ����ƹ���");
            free(targetCourse); // �ͷ� selectByIdForCourse ���ص��ڴ�
            return false;
        }
        strcpy(updatedCourse.name, courseName);
    }

    // ���¿γ��������ṩ�������
    if (courseType != -1) {
        updatedCourse.type_id = courseType;
    }

    // ����ʱ�䣨����ṩ����ʱ�䣩
    if (time != -1) {
        updatedCourse.time = time;
    }

    // ���µص㣨����ṩ���µص㣩
    if (location != NULL && strlen(location) > 0) {
        if (strlen(location) >= MAX_STR) {
            warnInfo("�޸Ŀγ���Ϣʧ�ܣ��ص����ƹ���");
            free(targetCourse); // �ͷ� selectByIdForCourse ���ص��ڴ�
            return false;
        }
        strcpy(updatedCourse.location, location);
    }

    // �������ѧ������������ṩ����ֵ��
    if (stuMax != -1) {
        if (stuMax < updatedCourse.student_count) {
            warnInfo("�޸Ŀγ���Ϣʧ�ܣ����ѧ����������С�ڵ�ǰѧ������");
            free(targetCourse); // �ͷ� selectByIdForCourse ���ص��ڴ�
            return false;
        }
        updatedCourse.student_max = stuMax;
    }

    // ���¼۸�����ṩ���¼۸�
    if (fabs(price + 1) < 1e-8) {
        updatedCourse.price = price;
    }

    // �������ݵ��ļ�
    if (!updateByIdForCourse(&updatedCourse, id)) {
        errorInfo("�޸Ŀγ���Ϣʧ�ܣ����ݸ���ʧ��");
        free(targetCourse);
        return false;
    }

    free(targetCourse); // �ͷ� selectByIdForCourse ���ص��ڴ�
    return true;
}

// �γ̹���

// ���ɺ͵�������γ̵Ľ���

/**
 * ��ӽ����Ϳγ̹�ϵ
 * @param courseId �γ�ID
 * @param coachId ����ID
 */
bool addCoachCourseRel(int courseId, int coachId) {
    // ���γ��Ƿ����
    Course* course = selectByIdForCourse(courseId);
    if (course == NULL) {
        warnInfo("��ӽ����γ̹�ϵʧ�ܣ��γ̲�����");
        return false;
    }
    free(course); // �ͷ� selectByIdForCourse ���ص��ڴ�

    // �������Ƿ����
    Coach* coach = selectByIdForCoach(coachId);
    if (coach == NULL) {
        warnInfo("��ӽ����γ̹�ϵʧ�ܣ�����������");
        return false;
    }
    free(coach); // �ͷ� selectByIdForCoach ���ص��ڴ�

    // ����ϵ�Ƿ��Ѵ���
    setFilterIntFlag(coachId);
    CoachCourseRel* existingRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    setFilterIntFlag(courseId);
    existingRels = filterCoachCourseRelByCourseId(existingRels);
    clearFilterIntFlag();
    if (existingRels != NULL) {
        errorInfo("��ӽ����γ̹�ϵʧ�ܣ���ϵ�Ѵ���");
        freeCoachCourseRelList(existingRels); // �ͷ������ڴ�
        return false;
    }

    // �����µĽ����γ̹�ϵ����
    CoachCourseRel newRel;
    memset(&newRel, 0, sizeof(CoachCourseRel));
    newRel.courseId = courseId;
    newRel.coachId = coachId;
    newRel.status = FILE_STATUS_EXIST; // ����״̬Ϊ����

    // �������ݵ��ļ�
    if (!insertForCoachCourseRel(&newRel)) {
        errorInfo("��ӽ����γ̹�ϵʧ�ܣ�����д��ʧ��");
        return false;
    }
    return true;
}

/**
 * ɾ�������Ϳγ̹�ϵ
 * @param courseId �γ�ID
 * @param coachId ����ID
 */
bool deleteCoachCourseRel(int courseId, int coachId) {
    // ��ȡ���н����γ̹�ϵ
    setFilterIntFlag(coachId);
    CoachCourseRel* existingRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    setFilterIntFlag(courseId);
    existingRels = filterCoachCourseRelByCourseId(existingRels);
    clearFilterIntFlag();

    // ���δ�ҵ�ƥ��Ĺ�ϵ
    if (existingRels == NULL) {
        errorInfo("ɾ�������γ̹�ϵʧ�ܣ���ϵ������");
        freeCoachCourseRelList(existingRels); // �ͷ������ڴ�
        return false;
    }

    int relId = existingRels->id;

    // �ͷ������ڴ�
    freeCoachCourseRelList(existingRels);

    // �߼�ɾ����ϵ
    if (!deleteByIdForCoachCourseRel(relId)) {
        errorInfo("ɾ�������γ̹�ϵʧ�ܣ����ݸ���ʧ��");
        return false;
    }
    return true;
}

// ���ɺ͵�������γ̵Ľ���

// ��Ϣ��ѯ

/**
 * ��ӡ���пγ���Ϣ
 */
bool showCourseInfoForAdmin() {
    // ��ȡ���пγ�
    Course* courses = selectAllForCourse(NULL);
    if (courses == NULL) {
        puts("û�пγ���Ϣ");
        return true;
    }

    // ��ӡ�γ���Ϣ
    puts("�γ���Ϣ�б�\n--------------------------");
    printLinkForCourse(courses);
    
    // �ͷ������ڴ�
    freeCourseList(courses);
    return true;
}

/**
 * ��ѯ�γ���Ϣ��null����-1����Ŀ��˵����ʹ�����������
 * @param courseName �γ�����
 * @param courseType �γ�����
 * @param location �γ̵ص�
 * @param stuMax_low ���ѧ���������䣩
 * @param stuMax_high ���ѧ���������䣩
 * @param price_low �۸������䣩
 * @param price_high �۸������䣩
 * @param time_low ʱ�䣨�����䣩
 * @param time_high ʱ�䣨�����䣩
 */
bool searchCourseInfoForAdmin(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high) {
    // ��ȡ���пγ�
    Course* courses = selectAllForCourse(NULL);
    if (courses == NULL) {
        puts("û�пγ���Ϣ");
        return true;
    }

    // ��ӡ��ѯ�������
    puts("��ѯ�����\n--------------------------");

    // ��������ɸѡ���������Ŀγ�
    Course* newHeader = NULL;
    Course* currentNew = NULL;
    Course* current = courses;
    while (current != NULL) {
        // ���γ�����
        if (courseName != NULL && strlen(courseName) > 0 && strstr(current->name, courseName) == NULL) {
            current = current->nextPointer;
            continue;
        }

        // ���γ�����
        if (courseType != NULL && strlen(courseType) > 0) {
            CourseType* type = selectByIdForCourseType(current->type_id);
            if (type == NULL || strstr(type->name, courseType) == NULL) {
                current = current->nextPointer;
                continue;
            }
            free(type); // �ͷ� selectByIdForCourseType ���ص��ڴ�
        }

        // ���ص�
        if (location != NULL && strlen(location) > 0 && strstr(current->location, location) == NULL) {
            current = current->nextPointer;
            continue;
        }

        // ������ѧ��������Χ
        if (stuMax_low != -1 && current->student_max < stuMax_low) {
            current = current->nextPointer;
            continue;
        }
        if (stuMax_high != -1 && current->student_max > stuMax_high) {
            current = current->nextPointer;
            continue;
        }

        // ���۸�Χ
        if (price_low != -1 && current->price < price_low) {
            current = current->nextPointer;
            continue;
        }
        if (price_high != -1 && current->price > price_high) {
            current = current->nextPointer;
            continue;
        }

        // ���ʱ�䷶Χ
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
                puts("����ϵͳ�ڴ治��");
                continue;
            }
            memcpy(newHeader, current, sizeof(Course));
            newHeader->nextPointer = NULL;
            currentNew = newHeader;
        } else {
            currentNew->nextPointer = (Course*)malloc(sizeof(Course));
            if (currentNew->nextPointer == NULL) {
                puts("����ϵͳ�ڴ治��");
                continue;
            }
            memcpy(currentNew->nextPointer, current, sizeof(Course));
            currentNew = currentNew->nextPointer;
            currentNew->nextPointer = NULL;
        }
        current = current->nextPointer;
    }
    printLinkForCourse(newHeader);
    // �ͷ������ڴ�
    freeCourseList(courses);
    freeCourseList(newHeader);
    return true;
}

/**
 * �鿴����ѧԱ��Ϣ
 */
bool showUserInfoForAdmin() {
    // ��ȡ����ѧԱ��Ϣ
    User* users = selectAllForUser(NULL);
    if (users == NULL) {
        puts("û��ѧԱ��Ϣ");
        return true;
    }

    // ��ӡѧԱ��Ϣ
    puts("ѧԱ��Ϣ�б�\n--------------------------");
    printLinkForUser(users);

    // �ͷ������ڴ�
    freeUserList(users);
    return true;
}

/**
 * ��ѯĳѧԱ��Ϣ
 * @param userName �û���
 */
bool searchUserInfoForAdmin(char* userName) {
    if (userName == NULL || strlen(userName) == 0) {
        errorInfo("��ѯѧԱ��Ϣʧ�ܣ��û���Ϊ��");
        return false;
    }

    // ��ȡ����ѧԱ��Ϣ
    setFilterStrFlag(userName);
    User* users = selectAllForUser(filterUserByNameContains);
    clearFilterStrFlag();
    if (users == NULL) {
        puts("û��ѧԱ��Ϣ");
        return true;
    }

    // ��ӡ��ѯ�������
    puts("��ѯ�����\n--------------------------");
    printLinkForUser(users);
    // �ͷ������ڴ�
    freeUserList(users);
    return true;
}

// ��Ϣ��ѯ

// ��Ϣ����

// ����ȽϺ���
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
    // �����������
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

// ����ȽϺ�������
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

// �����ۺϱȽϺ���
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
        puts("û�пγ���Ϣ");
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
    // ʹ��ѡ�������������
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
                puts("����ϵͳ�ڴ治��");
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
                puts("����ϵͳ�ڴ治��");
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
 * �Կγ���Ϣ��������
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
bool sortCourseInfoForAdmin(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice) {
    // ��ȡ���пγ�
    Course* courses = selectAllForCourse(NULL);
    Course* sortHeader = sortCourseInfo(courses, byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice);

    // ��ӡ������
    printLinkForCourse(sortHeader);
    freeCourseList(sortHeader);
    freeCourseList(courses);
    return true;
}

/**
 * ��ȡѧԱ��ѡ������
 * @param userId ѧԱID
 * @return ѡ������
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
 * �ȽϺ����������ƺ�ѡ����������
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
        puts("û��ѧԱ��Ϣ");
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
                puts("����ϵͳ�ڴ治��");
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
                puts("����ϵͳ�ڴ治��");
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
 * ����ѧԱ��Ϣ
 * @param byName �����ƣ��������򣬸�������0�����룩
 * @param bySelectCourse ����ѡ�γ��������������򣬸�������0�����룩
 */
bool sortUserInfoForAdmin(int byName, int bySelectCourse) {
    // ��ȡ����ѧԱ��Ϣ
    User* users = selectAllForUser(NULL);
    User* sortHeader = sortUserInfo(users, byName, bySelectCourse);

    // ��ӡ�û���Ϣ
    printLinkForUser(sortHeader);
    freeUserList(sortHeader);
    freeUserList(users);
    return true;
}

// ��Ϣ����

// ��Ϣͳ��
/**
 * ��ӡ�������Ϳγ̵�ѧԱ����
 */
bool countCourseTypeSelectedForAdmin() {
    // ��ȡ���пγ�����
    CourseType* courseTypes = selectAllForCourseType(NULL);
    if (courseTypes == NULL) {
        puts("û�пγ�������Ϣ");
        return true;
    }

    // �����б�
    CourseType* header = courseTypes;
    while (header != NULL) {
        countCourseTypeSelectedByIdForAdmin(header->id);
        header = header->nextPointer;
    }

    // �ͷ��ڴ�
    freeCourseTypeList(courseTypes);
    return true;
}

/**
 * ��ӡĳ�����Ϳγ̵�ѧԱ����
 * @param courseTypeId �γ�����ID
 */
bool countCourseTypeSelectedByIdForAdmin(int courseTypeId) {
    // ��ȡ���пγ�
    setFilterIntFlag(courseTypeId);
    Course* courses = selectAllForCourse(filterCourseByCourseTypeId);
    clearFilterIntFlag();
    if (courses == NULL) {
        puts("û�пγ���Ϣ");
        return true;
    }

    // ����ָ���γ����͵Ŀγ�
    Course* currentCourse = courses;
    int totalStudents = 0; // ѧԱ����

    while (currentCourse != NULL) {
        int count = 0;

        // ����ѡ�ι�ϵ��ͳ�Ƶ�ǰ�γ̵�ѧԱ����
        setFilterIntFlag(currentCourse->id);
        UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterCoachCourseRelByCourseId);
        clearFilterIntFlag();
        UserCourseRel* currentRel = userCourseRels;
        while (currentRel != NULL) {
            count++;
            currentRel = currentRel->nextPointer;
        }

        // ��ӡ��ǰ�γ̵�ѧԱ����
        printf("�γ�ID: %d\n", currentCourse->id);
        printf("�γ�����: %s\n", currentCourse->name);
        printf("ѧԱ����: %d\n", count);
        printf("--------------------------\n");

        totalStudents += count; // �ۼ���ѧԱ����

        // �ͷ��ڴ�
        freeUserCourseRelList(userCourseRels);
        currentCourse = currentCourse->nextPointer;
    }

    // ��ӡ�����Ϳγ̵���ѧԱ����
    printf("�γ�����ID: %d\n", courseTypeId);
    printf("��ѧԱ����: %d\n", totalStudents);
    printf("--------------------------\n");

    // �ͷ��ڴ�
    freeCourseList(courses);
    return true;
}

/**
 * ��ӡ���н�������Ľ���γ�����
 */
bool countCoachCourseForAdmin() {
    // ��ȡ���н���
    Coach* coaches = selectAllForCoach(NULL);
    if (coaches == NULL) {
        puts("û�н�����Ϣ");
        return true;
    }

    // ����������ͳ��ÿ����������Ŀγ�����
    puts("��������Ŀγ�����ͳ�ƣ�\n--------------------------");
    Coach* header = coaches;
    while (header != NULL) {
        countCoachCourseByIdForAdmin(header->id);
        header = header->nextPointer;
    }

    // �ͷ��ڴ�
    freeCoachList(coaches);
    return true;
}

/**
 * ��ӡĳ��������Ľ���γ�����
 * @param coachId ����ID
 */
bool countCoachCourseByIdForAdmin(int coachId) {
    // ��ȡָ������
    Coach* coach = selectByIdForCoach(coachId);
    if (coach == NULL) {
        printf("δ�ҵ�IDΪ %d �Ľ���\n", coachId);
        return false;
    }

    // ��ȡ���н����γ̹�ϵ
    setFilterIntFlag(coachId);
    CoachCourseRel* coachCourseRels = selectAllForCoachCourseRel(filterCoachCourseRelByCoachId);
    clearFilterIntFlag();
    if (coachCourseRels == NULL) {
        puts("û�н����γ̹�ϵ��¼");
        free(coach); // �ͷŽ����ڴ�
        return true;
    }

    int count = 0; // ����Ŀγ�����

    // ���������γ̹�ϵ��ͳ�Ƶ�ǰ��������Ŀγ�����
    CoachCourseRel* currentRel = coachCourseRels;
    while (currentRel != NULL) {
            count++;
        currentRel = currentRel->nextPointer;
    }

    // ��ӡ��ǰ��������Ŀγ�����
    puts("��������Ŀγ�����ͳ�ƣ�\n--------------------------");
    printf("����ID: %d\n", coach->id);
    printf("��������: %s\n", coach->name);
    printf("����γ�����: %d\n", count);
    printf("--------------------------\n");

    // �ͷ��ڴ�
    free(coach);
    freeCoachCourseRelList(coachCourseRels);
    return true;
}

/**
 * ��ӡ����ѧԱ�μӵĽ���γ̵�����
 */
bool countUserCourseForAdmin() {
    // ��ȡ����ѧԱ
    User* users = selectAllForUser(NULL);
    if (users == NULL) {
        puts("û��ѧԱ��Ϣ");
        return true;
    }
    User* header = users;
    while (header != NULL) {
        countUserCourseByIdForAdmin(header->id);
        header = header->nextPointer;
    }

    // �ͷ��ڴ�
    freeUserList(users);
    return true;
}

/**
 * ��ӡĳѧԱ�μӵĽ���γ̵�����
 * @param userId ѧԱID
 */
bool countUserCourseByIdForAdmin(int userId) {
    // ��ȡָ��ѧԱ
    User* user = selectByIdForUser(userId);
    if (user == NULL) {
        printf("δ�ҵ�IDΪ %d ��ѧԱ\n", userId);
        return false;
    }

    // ��ȡ����ѧԱ�γ̹�ϵ
    setFilterIntFlag(userId);
    UserCourseRel* userCourseRels = selectAllForUserCourseRel(filterUserCourseRelByUserId);
    clearFilterIntFlag();
    if (userCourseRels == NULL) {
        puts("û��ѧԱ�γ̹�ϵ��¼");
        free(user); // �ͷ�ѧԱ�ڴ�
        return true;
    }

    int count = 0; // �μӵĿγ�����

    // ����ѧԱ�γ̹�ϵ��ͳ�Ƶ�ǰѧԱ�μӵĿγ�����
    UserCourseRel* currentRel = userCourseRels;
    while (currentRel != NULL) {
        count++;
        currentRel = currentRel->nextPointer;
    }

    // ��ӡ��ǰѧԱ�μӵĿγ�����
    puts("ѧԱ�μӵĿγ�����ͳ�ƣ�\n--------------------------");
    printf("ѧԱID: %d\n", user->id);
    printf("ѧԱ����: %s\n", user->name);
    printf("�μӿγ�����: %d\n", count);
    printf("--------------------------\n");

    // �ͷ��ڴ�
    free(user);
    freeUserCourseRelList(userCourseRels);
    return true;
}

// ��Ϣͳ��

// ϵͳά��

/**
 * �޸Ĺ���Ա���루ID�ɵ�¼������¼��
 * @param newPassword ������
 * @return �Ƿ�ɹ�
 */
bool changePasswordForAdmin(char* newPassword) {
    // ���赱ǰ��¼�Ĺ���ԱID�Ѿ���¼��ȫ�ֱ��� CURRENT_ADMIN_ID ��
 
    // ��ȡ��ǰ����Ա��Ϣ
    Admin* admin = selectByIdForAdmin(CURRENT_ADMIN_ID);
    if (admin == NULL) {
        puts("δ�ҵ���ǰ����Ա��Ϣ");
        return false;
    }

    // ��������
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(admin->password, encryptPassword);
    free(encryptPassword);

    // �������ݿ��еĹ���Ա��Ϣ
    if (updateByIdForAdmin(admin, CURRENT_ADMIN_ID)) {
        puts("�����޸ĳɹ�");
        // �ͷ��ڴ�
        free(admin);
        return true;
    }
    else {
        puts("�����޸�ʧ��");
        // �ͷ��ڴ�
        free(admin);
        return false;
    }

    
}

/**
 * �޸Ľ�������
 * @param coachId ����ID
 * @param newPassword ������
 */
bool changeCoachPasswordForAdmin(int coachId, char* newPassword) {
    // ��ȡָ��������Ϣ
    Coach* coach = selectByIdForCoach(coachId);
    if (coach == NULL) {
        printf("δ�ҵ�IDΪ %d �Ľ���\n", coachId);
        return false;
    }

    // ��������
    char* encryptPassword = generateEncryptPassword(newPassword);
    strcpy(coach->password, encryptPassword);
    free(encryptPassword);

    // �������ݿ��еĽ�����Ϣ
    if (updateByIdForCoach(coach, coachId)) {
        puts("���������޸ĳɹ�");
        // �ͷ��ڴ�
        free(coach);
        return true;
    }
    else {
        puts("���������޸�ʧ��");
        // �ͷ��ڴ�
        free(coach);
        return false;
    }

    
}

/**
 * ���ݱ���
 * @param backupName ��������
 */
bool databaseBackup(char* backupName) {
    // ��ԭʼ�����ļ�
    FILE* sourceFile = fopen(DATAFILE_NAME, "rb");
    if (sourceFile == NULL) {
        printf("�޷���ԭʼ�����ļ�: %s\n", DATAFILE_NAME);
        return false;
    }

    // ���������ļ�
    char backupFilePath[MAX_STR];
    snprintf(backupFilePath, MAX_STR, "%s.bak", backupName); // �����ļ�·��
    FILE* backupFile = fopen(backupFilePath, "wb");
    if (backupFile == NULL) {
        printf("�޷����������ļ�: %s\n", backupFilePath);
        fclose(sourceFile); // �ر�ԭʼ�ļ�
        return false;
    }

    // ��������
    char buffer[1024];
    size_t bytesRead;
    while (bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) {
        fwrite(buffer, 1, bytesRead, backupFile);
    }

    // �ر��ļ�
    fclose(sourceFile);
    fclose(backupFile);

    printf("���ݱ��ݳɹ��������ļ�: %s\n", backupFilePath);
    return true;
}

/**
 * ���ݻָ�
 * @param backupName ��������
 */
bool databaseRestore(char* backupName) {
    // ���������ļ�·��
    char backupFilePath[MAX_STR];
    snprintf(backupFilePath, MAX_STR, "%s.bak", backupName); // �����ļ�·��

    // �򿪱����ļ�
    FILE* backupFile = fopen(backupFilePath, "rb");
    if (backupFile == NULL) {
        printf("�޷��򿪱����ļ�: %s\n", backupFilePath);
        return false;
    }

    // ��ԭʼ�����ļ�
    FILE* sourceFile = fopen(DATAFILE_NAME, "wb");
    if (sourceFile == NULL) {
        printf("�޷���ԭʼ�����ļ�: %s\n", DATAFILE_NAME);
        fclose(backupFile); // �رձ����ļ�
        return false;
    }

    // ��������
    char buffer[1024];
    size_t bytesRead;
    while (bytesRead = fread(buffer, 1, sizeof(buffer), backupFile)) {
        fwrite(buffer, 1, bytesRead, sourceFile);
    }

    // �ر��ļ�
    fclose(backupFile);
    fclose(sourceFile);

    printf("���ݻָ��ɹ����ӱ����ļ�: %s\n", backupFilePath);
    return true;
}