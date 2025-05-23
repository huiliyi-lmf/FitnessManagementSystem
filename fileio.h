#ifndef __FILEIO_H
#define __FILEIO_H

#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>

#define TABLE_NAME_LEN 36
#define DATAFILE_NAME "FMSData.bin"
#define DATAFILE_SIGN "VLSMB_DATAFILE"	// 文件签名，最大15字节
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
}FileStatus;							// 文件状态（删除或者没删除）

struct DataTableInfo {
	unsigned id;					// 数据表ID
	int dataTableHeaderOffset;		// 数据表头文件偏移（-1则代表没有）
	char tableName[36];				// 数据表名称
};

typedef struct DataFileHeader {
	char signature[16];				// 文件签名，字符数组"VLSMB_DATAFILE\0\0"
	unsigned numbersOfTable;		// 当前数据文件头存储的数据表信息数量
	int nextDataFileHeaderOffset;	// 下一个数据文件头定义存储的文件偏移（-1则代表没有）
	unsigned maxTables;				// 头文件最多存储数据表信息（应为11）
	struct DataTableInfo tables[11];// 数据表信息
}DataFileHeader;

typedef struct DataTableHeader {
	char signature[16];				// 文件签名，字符数组"VLSMB_DATATABLE\0"
	unsigned firstDataItemOffset;	// 第一个数据的偏移
}DataTableHeader;

// —————————图形界面的输出

/**
* 定义错误信息处理
* @param message 错误信息
*/
void errorInfo(const char* message);

/**
* 定义错误信息处理
* @param message 错误信息
* @param hwnd 窗口句柄
*/
void errorInfoForWindow(const char* message, HWND hwnd);

void successInfoForWindow(const char* message, HWND hwnd);

/**
* 定义警告信息处理
* @param message 警告信息
*/
void warnInfo(const char* message);

/**
* 定义警告信息处理
* @param message 警告信息
* @param hwnd 窗口句柄
*/
void warnInfoForWindow(const char* message, HWND hwnd);

// 设置编辑框句柄
void setEditHandle(HWND hEdit);

void clearEditHandle();

void clearEditText();

// 重载printf和puts
int winPrintf(const char* _format, ...);
int winPuts(const char* str);

#define printf winPrintf
#define puts winPuts

// 图形界面的输出—————————

/**
* 创建数据文件
* @return 是否成功
*/
bool initDataFile();

/**
* 检查数据文件是否正常
* @return 是否正常
*/
bool checkDataFileSign();

/**
* 添加新的数据表信息
* @param tableName 数据表名
* @return 是否成功
*/
bool createNewTable(char* tableName);

/**
* 判断数据表是否存在，并返回DataTableHeader所在的文件偏移
* @param tableName 数据表名
* @return 文件偏移，为-1则代表出错
*/
int getTableHeaderOffset(char* tableName);

/**
 * 向指定的数据表中添加一条数据
 * 本函数是一个统一的接口，无法得知具体的结构体信息，因此借助传递的函数指针进行操作
 * @param tableName 数据表名
 * @param data 数据字节
 * @param structSize 数据大小
 * @param idSetter 函数指针，该函数的作用是设置data->id
 * @param offsetSetter 函数指针，该函数的作用是设置data->nextOffset
 * @param offsetGetter 函数指针，该函数的作用是获得data->nexttOffset
 * @param statusSetter 函数指针，该函数的作用是设置data->status
 * @return 是否成功
 */
bool insert(char* tableName, void* data, unsigned structSize, void (*idSetter)(void*, int), void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), void (*statusSetter)(void*, FileStatus));

/**
 * 根据ID获得数据在文件的偏移位置
 * 本函数是一个统一的接口，无法得知具体的结构体信息，因此借助传递的函数指针进行操作
 * @param tableName 数据表名
 * @param id 数据ID
 * @param structSize 数据大小
 * @param offsetSetter 函数指针，该函数的作用是设置data->nextOffset
 * @param offsetGetter 函数指针，该函数的作用是获得data->nextOffset
 * @param idGetter 函数指针，该函数的作用是获得data->id
 * @return 文件偏移，或者-1
 */
int getOffsetById(char* tableName, int id, unsigned structSize, void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), int (*idGetter)(void*));

/**
 * 根据ID获得数据
 * 本函数是一个统一的接口，无法得知具体的结构体信息，因此借助传递的函数指针进行操作
 * @param tableName 数据表名
 * @param id 数据ID
 * @param structSize 数据大小
 * @param pointerSetter 函数指针，该函数的作用是设置data->nextPointer
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数对于该表的具体实现函数
 * @return 字节数据
 */
void* selectById(char* tableName, int id, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int));

/**
 * 根据ID更新某个数据记录
 * 本函数是一个统一的接口，无法得知具体的结构体信息，因此借助传递的函数指针进行操作
 * @param tableName 数据表名
 * @param id 数据ID
 * @param newData 新的数据字节
 * @param structSize 数据大小
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数对于该表的具体实现函数
 * @param copyProperty 函数指针，该函数用于将新数据可更新部分作用于旧记录上，第一个参数为目标，第二个参数为源
 * @return 是否成功
 */
bool updateById(char* tableName, int id, void* newData, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*copyProperty)(void*, void*));

/**
 * 根据ID删除某个数据记录
 * 本函数是一个统一的接口，无法得知具体的结构体信息，因此借助传递的函数指针进行操作
 * @param tableName 数据表名
 * @param id 数据ID
 * @param structSize 数据大小
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数对于该表的具体实现函数
 * @param statusSetter 函数指针，该函数的作用是设置data->status
 * @return 是否成功
 */
bool deleteById(char* tableName, int id, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*statusSetter)(void*, FileStatus));

/**
 * 获取一个数据表的所有记录（也可以过滤记录）
 * 本函数是一个统一的接口，无法得知具体的结构体信息，因此借助传递的函数指针进行操作
 * @param tableName 数据表名
 * @param structSize 数据大小
 * @param pointerSetter 函数指针，该函数的作用是设置data->nextPointer
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数对于该表的具体实现函数
 * @param pointerGetter 函数指针，该函数的作用是获得data->nextPointer
 * @param statusGetter 函数指针，该函数的作用是获得data->status
 * @param filter 函数指针，在返回数据链表前会将数据链表传递到这个函数里，可以在此函数内根据条件重构链表，并返回新链表头节点。
 *			filter函数的参数为第一个参数为链表头节点。具体使用方法可以见struct.c定义个filterForUserCourseRelByUserId函数。
 *			filter可以为NULL，则不过滤任何节点
 * @return 数据链表
 */
void* selectAll(char* tableName, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int), void* (*pointerGetter)(void*), FileStatus(*statusGetter)(void*), void* (*filter)(void*));

#endif 

