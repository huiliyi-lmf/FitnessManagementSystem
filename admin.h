#ifndef __ADMIN_H
#define __ADMIN_H

#include <stdbool.h>
#include "structs.h"
#include <stdlib.h>
#include <math.h>
#include "filter.h"

/**
 * ��¼�ж�
 * @param userName �û���
 * @param password ����
 * @return �����Ƿ���ȷ
 */
bool loginForAdmin(char* userName, char* password);

/**
 * ע�����Ա
 * @param userName �û���
 * @param password ����
 * @return �Ƿ�ɹ�
 */
bool registerForAdmin(char* userName, char* password);

// ��Ϣ�������ӣ��޸ģ�ɾ����������

/**
 * ����һ���µ���Ŀ����
 * @param typeName ��Ŀ����
 */
bool createCourseType(char* typeName);

/**
 * �޸�ĳ����Ŀ����
 * @param id ��Ŀ���
 * @param typeName �޸ĺ������
 */
bool updateCourseType(int id, char* typeName);

/**
 * ɾ����Ŀ
 * @param id ��ĿID
 */
bool deleteCourseType(int id);

/**
 * �����µĽ����˻�
 * @param coachName ��������
 * @param coachPassword ��������
 */
bool createCoach(char* coachName, char* coachPassword);

/**
 * �޸Ľ�����Ϣ�����ΪNULL�������޸ģ�
 * @param id ����ID
 * @param newCoachName ��������
 * @param newCoachPassword  ��������
 * @param newLevel  ����������Ϊ-1���޸ģ�
 */
bool updateCoach(int id, char* newCoachName, char* newCoachPassword, CoachLevel newLevel);

/**
 * ɾ��������Ϣ
 * @param id ����ID
 */
bool deleteCoach(int id);

// ������������Ϣ�������ӣ��޸ģ�ɾ��

// ��������

// �鿴���н�����Ϣ
bool showAllCoachInfo();

// �鿴���пγ������Ϣ
bool showAllCourseTypeInfo();

/**
 * �鿴�����ύ���Լ��ó�ָ������Ŀ��������
 */
bool showNeedJudgeCoachInfo();

/**
 * ��˽�����Ϣ�Ľ��
 * @param id ����ID
 * @param status ������Ϣ
 */
bool markJudgeCoachInfoStatus(int id, CoachStatus status);

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
bool createCourse(char* courseName, int courseType, int time, char* location, int stuMax, double price);

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
bool updateCourse(int id, char* courseName, int courseType, int time, char* location, int stuMax, double price);

// �γ̹���

// ���ɺ͵�������γ̵Ľ���

/**
 * ��ӽ����Ϳγ̹�ϵ
 * @param courseId �γ�ID
 * @param coachId ����ID
 */
bool addCoachCourseRel(int courseId, int coachId);

/**
 * ɾ�������Ϳγ̹�ϵ
 * @param courseId �γ�ID
 * @param coachId ����ID
 */
bool deleteCoachCourseRel(int courseId, int coachId);

// ���ɺ͵�������γ̵Ľ���

// ��Ϣ��ѯ

/**
 * ��ӡ���пγ���Ϣ
 */
bool showCourseInfoForAdmin();

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
bool searchCourseInfoForAdmin(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high);

/**
 * �鿴����ѧԱ��Ϣ
 */
bool showUserInfoForAdmin();

/**
 * ��ѯĳѧԱ��Ϣ
 * @param userName �û���
 */
bool searchUserInfoForAdmin(char* userName);

// ��Ϣ��ѯ

// ��Ϣ����

Course* sortCourseInfo(Course* courses, int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

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
bool sortCourseInfoForAdmin(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

User* sortUserInfo(User* users, int byName, int bySelectCourse);

/**
 * ����ѧԱ��Ϣ
 * @param byName ������
 * @param bySelectCourse ����ѡ�γ�����
 */
bool sortUserInfoForAdmin(int byName, int bySelectCourse);

// ��Ϣ����

// ��Ϣͳ��

/**
 * ��ӡ�������Ϳγ̵�ѧԱ����
 */
bool countCourseTypeSelectedForAdmin();

/**
 * ��ӡĳ�����Ϳγ̵�ѧԱ����
 * @param courseTypeId �γ�����ID
 */
bool countCourseTypeSelectedByIdForAdmin(int courseTypeId);

/**
 * ��ӡ���н�������Ľ���γ�����
 */
bool countCoachCourseForAdmin();

/**
 * ��ӡĳ��������Ľ���γ�����
 * @param coachId ����ID
 */
bool countCoachCourseByIdForAdmin(int coachId);

/**
 * ��ӡ����ѧԱ�μӵĽ���γ̵�����
 */
bool countUserCourseForAdmin();

/**
 * ��ӡĳѧԱ�μӵĽ���γ̵�����
 * @param userId ѧԱID
 */
bool countUserCourseByIdForAdmin(int userId);

// ��Ϣͳ��

// ϵͳά��

/**
 * �޸Ĺ���Ա���루ID�ɵ�¼������¼��
 * @param newPassword ������
 * @return �Ƿ�ɹ�
 */
bool changePasswordForAdmin(char* newPassword);

/**
 * �޸Ľ�������
 * @param coachId ����ID
 * @param newPassword ������
 */
bool changeCoachPasswordForAdmin(int coachId, char* newPassword);

/**
 * ���ݱ���
 * @param backupName ��������
 */
bool databaseBackup(char* backupName);

/**
 * ���ݻָ�
 * @param backupName ��������
 */
bool databaseRestore(char* backupName);

// ϵͳά��

#endif // !__ADMIN__H