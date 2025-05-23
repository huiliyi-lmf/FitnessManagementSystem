#ifndef __USER_H
#define __USER_H

#include "structs.h"
#include <stdbool.h>
#include "admin.h"

// ������Ϣ����

/**
 * ѧԱע��
 * @param name ����
 * @param password ����
 * @param phone �绰
 * @return �Ƿ�ɹ�
 */
bool registerForUser(char* name, char* password, char* phone);

/**
 * ��¼
 * @param name ����
 * @param password ����
 * @param phone �绰��
 * @return �Ƿ�ɹ�
 */
bool loginForUser(char* name, char* password, char* phone);

/**
 * �޸���Ϣ��ΪNULL���޸ģ�
 * @param name ����
 * @param phone �绰
 */
bool changeInfoForUser(char* name, char* phone);

// ������Ϣ����

// �γ̱�������

/**
 * ����Լ����ϵĿγ�
 * @param courseId �γ�ID
 */
bool addUserCourseRel(int courseId);

/**
 * ɾ���Լ����ϵĿγ�
 * @param courseId �γ�ID
 */
bool deleteUserCourseRel(int courseId);

// �γ̱�������

// ��Ϣ��ѯ

/**
 * ��ӡ��ǰ�û����пγ���Ϣ
 */
bool showCourseInfoForUser();

/**
 * ��ѯ��ǰ�û��γ���Ϣ��null����-1����Ŀ��˵����ʹ�����������
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
bool searchCourseInfoForUser(char* courseName, char* courseType, char* location, int stuMax_low, int stuMax_high, double price_low, double price_high, int time_low, int time_high);

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
bool sortCourseInfoForUser(int byName, int byTypeId, int byTime, int byLocation, int byStuCount, int byStuMax, int byCoachNum, int byPrice);

// ��Ϣ����

// ��Ϣͳ��

/**
 * ��ӡ��ǰ�û��������Ϳγ̵�ѧԱ����
 */
bool countCourseTypeSelectedForUser();

/**
 * ��ӡ��ǰ�û�ĳ�����Ϳγ̵�ѧԱ����
 * @param courseTypeId �γ�����ID
 */
bool countCourseTypeSelectedByIdForUser(int courseTypeId);

/**
 * ��ӡ���н�������ǰ�û��Ľ���γ�����
 */
bool countCoachCourseForUser();

/**
 * ��ӡĳ��������ĵ�ǰ�û�����γ�����
 * @param coachId ����ID
 */
bool countCoachCourseByIdForUser(int coachId);

// ��Ϣͳ��

// ϵͳά��

/**
 * �޸��û�����
 * @param newPassword ������
 */
bool changePasswordForUser(char* newPassword);

// ϵͳά��

#endif
