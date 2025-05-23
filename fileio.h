#ifndef __FILEIO_H
#define __FILEIO_H

#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>

#define TABLE_NAME_LEN 36
#define DATAFILE_NAME "FMSData.bin"
#define DATAFILE_SIGN "VLSMB_DATAFILE"	// �ļ�ǩ�������15�ֽ�
#define DATATABLE_SIGN "VLSMB_DATATABLE"

#define AdminTable "Admin"
#define CoachTable "Coach"
#define CourseTypeTable "CourseType"
#define CoachTypeRelTable "CoachTypeRel"
#define UserTable "User"
#define CourseTable "Course"
#define CoachCourseRelTable "CoachCourseRel"
#define UserCourseRelTable "UserCourseRel"

typedef enum FileStatus {
	FILE_STATUS_EXIST, FILE_STATUS_DELETE
}FileStatus;							// �ļ�״̬��ɾ������ûɾ����

struct DataTableInfo {
	unsigned id;					// ���ݱ�ID
	int dataTableHeaderOffset;		// ���ݱ�ͷ�ļ�ƫ�ƣ�-1�����û�У�
	char tableName[36];				// ���ݱ�����
};

typedef struct DataFileHeader {
	char signature[16];				// �ļ�ǩ�����ַ�����"VLSMB_DATAFILE\0\0"
	unsigned numbersOfTable;		// ��ǰ�����ļ�ͷ�洢�����ݱ���Ϣ����
	int nextDataFileHeaderOffset;	// ��һ�������ļ�ͷ����洢���ļ�ƫ�ƣ�-1�����û�У�
	unsigned maxTables;				// ͷ�ļ����洢���ݱ���Ϣ��ӦΪ11��
	struct DataTableInfo tables[11];// ���ݱ���Ϣ
}DataFileHeader;

typedef struct DataTableHeader {
	char signature[16];				// �ļ�ǩ�����ַ�����"VLSMB_DATATABLE\0"
	unsigned firstDataItemOffset;	// ��һ�����ݵ�ƫ��
}DataTableHeader;

// ������������������ͼ�ν�������

/**
* ���������Ϣ����
* @param message ������Ϣ
*/
void errorInfo(const char* message);

/**
* ���������Ϣ����
* @param message ������Ϣ
* @param hwnd ���ھ��
*/
void errorInfoForWindow(const char* message, HWND hwnd);

void successInfoForWindow(const char* message, HWND hwnd);

/**
* ���徯����Ϣ����
* @param message ������Ϣ
*/
void warnInfo(const char* message);

/**
* ���徯����Ϣ����
* @param message ������Ϣ
* @param hwnd ���ھ��
*/
void warnInfoForWindow(const char* message, HWND hwnd);

// ���ñ༭����
void setEditHandle(HWND hEdit);

void clearEditHandle();

void clearEditText();

// ����printf��puts
int winPrintf(const char* _format, ...);
int winPuts(const char* str);

#define printf winPrintf
#define puts winPuts

// ͼ�ν�������������������������

/**
* ���������ļ�
* @return �Ƿ�ɹ�
*/
bool initDataFile();

/**
* ��������ļ��Ƿ�����
* @return �Ƿ�����
*/
bool checkDataFileSign();

/**
* ����µ����ݱ���Ϣ
* @param tableName ���ݱ���
* @return �Ƿ�ɹ�
*/
bool createNewTable(char* tableName);

/**
* �ж����ݱ��Ƿ���ڣ�������DataTableHeader���ڵ��ļ�ƫ��
* @param tableName ���ݱ���
* @return �ļ�ƫ�ƣ�Ϊ-1��������
*/
int getTableHeaderOffset(char* tableName);

/**
 * ��ָ�������ݱ������һ������
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param data �����ֽ�
 * @param structSize ���ݴ�С
 * @param idSetter ����ָ�룬�ú���������������data->id
 * @param offsetSetter ����ָ�룬�ú���������������data->nextOffset
 * @param offsetGetter ����ָ�룬�ú����������ǻ��data->nexttOffset
 * @param statusSetter ����ָ�룬�ú���������������data->status
 * @return �Ƿ�ɹ�
 */
bool insert(char* tableName, void* data, unsigned structSize, void (*idSetter)(void*, int), void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), void (*statusSetter)(void*, FileStatus));

/**
 * ����ID����������ļ���ƫ��λ��
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param id ����ID
 * @param structSize ���ݴ�С
 * @param offsetSetter ����ָ�룬�ú���������������data->nextOffset
 * @param offsetGetter ����ָ�룬�ú����������ǻ��data->nextOffset
 * @param idGetter ����ָ�룬�ú����������ǻ��data->id
 * @return �ļ�ƫ�ƣ�����-1
 */
int getOffsetById(char* tableName, int id, unsigned structSize, void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), int (*idGetter)(void*));

/**
 * ����ID�������
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param id ����ID
 * @param structSize ���ݴ�С
 * @param pointerSetter ����ָ�룬�ú���������������data->nextPointer
 * @param getOffsetByIdForTable ����ָ�룬ֵΪgetOffsetById�������ڸñ�ľ���ʵ�ֺ���
 * @return �ֽ�����
 */
void* selectById(char* tableName, int id, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int));

/**
 * ����ID����ĳ�����ݼ�¼
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param id ����ID
 * @param newData �µ������ֽ�
 * @param structSize ���ݴ�С
 * @param getOffsetByIdForTable ����ָ�룬ֵΪgetOffsetById�������ڸñ�ľ���ʵ�ֺ���
 * @param copyProperty ����ָ�룬�ú������ڽ������ݿɸ��²��������ھɼ�¼�ϣ���һ������ΪĿ�꣬�ڶ�������ΪԴ
 * @return �Ƿ�ɹ�
 */
bool updateById(char* tableName, int id, void* newData, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*copyProperty)(void*, void*));

/**
 * ����IDɾ��ĳ�����ݼ�¼
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param id ����ID
 * @param structSize ���ݴ�С
 * @param getOffsetByIdForTable ����ָ�룬ֵΪgetOffsetById�������ڸñ�ľ���ʵ�ֺ���
 * @param statusSetter ����ָ�룬�ú���������������data->status
 * @return �Ƿ�ɹ�
 */
bool deleteById(char* tableName, int id, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*statusSetter)(void*, FileStatus));

/**
 * ��ȡһ�����ݱ�����м�¼��Ҳ���Թ��˼�¼��
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param structSize ���ݴ�С
 * @param pointerSetter ����ָ�룬�ú���������������data->nextPointer
 * @param getOffsetByIdForTable ����ָ�룬ֵΪgetOffsetById�������ڸñ�ľ���ʵ�ֺ���
 * @param pointerGetter ����ָ�룬�ú����������ǻ��data->nextPointer
 * @param statusGetter ����ָ�룬�ú����������ǻ��data->status
 * @param filter ����ָ�룬�ڷ�����������ǰ�Ὣ���������ݵ��������������ڴ˺����ڸ��������ع�����������������ͷ�ڵ㡣
 *			filter�����Ĳ���Ϊ��һ������Ϊ����ͷ�ڵ㡣����ʹ�÷������Լ�struct.c�����filterForUserCourseRelByUserId������
 *			filter����ΪNULL���򲻹����κνڵ�
 * @return ��������
 */
void* selectAll(char* tableName, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int), void* (*pointerGetter)(void*), FileStatus(*statusGetter)(void*), void* (*filter)(void*));

#endif 

