#ifndef __COACH_H
#define __COACH_H

#include "structs.h"
#include <stdbool.h>
#include "admin.h"

// ��¼

/**
 * ������¼
 * @param username �û���
 * @param password ����
 * @return �Ƿ�ɹ�
 */
bool loginForCoach(char* userName, char* password);

// ��¼

// ������Ϣ����

/**
 * ���ӽ����ó�ָ������Ŀ����
 * @param typeId ��ĿID
 */
bool addNewCoachTypeRel(int typeId);

/**
 * ɾ�������ó�ָ������Ŀ����
 * @param typeId ��ĿID
 */
bool removeCoachTypeRel(int typeId);

// ������Ϣ����

// ��Ϣ��ѯ

/**
 * ��ӡ���е�ǰ��������Ŀγ���Ϣ
 */
bool showCourseInfoForCoach();

/**
 * ��ѯ��ǰ��������Ŀγ���Ϣ��null����-1����Ŀ��˵����ʹ�����������
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
bool searchCourseInfoForCoach(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high);

/**
 * �鿴�����뵱ǰ�����йص�ѧԱ��Ϣ
 */
bool showUserInfoForCoach();

/**
 * ��ѯ�뵱ǰ�����йص�ĳѧԱ��Ϣ
 * @param userName �û���
 */
bool searchUserInfoForCoach(char* userName);

// ��Ϣ��ѯ

// ��Ϣ����

/**
 * ��ǰ��������Ŀγ���Ϣ��������
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
bool sortCourseInfoForCoach(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

/**
 * �����뵱ǰ�����йص�ѧԱ��Ϣ
 * @param byName ������
 * @param bySelectCourse ����ѡ�γ�����
 */
bool sortUserInfoForCoach(int byName, int bySelectCourse);

// ��Ϣ����

// ��Ϣͳ��

/**
 * ��ӡ��ǰ�����������Ϳγ̵�ѧԱ����
 */
bool countCourseTypeSelectedForCoach();

/**
 * ��ӡ��ǰ����ĳ�����Ϳγ̵�ѧԱ����
 * @param courseTypeId �γ�����ID
 */
bool countCourseTypeSelectedByIdForCoach(int courseTypeId);

/**
 * ��ӡ��ǰ��������ѧԱ�μӵĽ���γ̵�����
 */
bool countUserCourseForCoach();

/**
 * ��ӡ��ǰ����ĳѧԱ�μӵĽ���γ̵�����
 * @param userId ѧԱID
 */
bool countUserCourseByIdForCoach(int userId);

// ��Ϣͳ��

// ϵͳά��

/**
 * �޸�����
 * @param newPassword ������
 */
bool changePasswordForCoach(char* newPassword);

// ϵͳά��

#endif // !__COACH_H

