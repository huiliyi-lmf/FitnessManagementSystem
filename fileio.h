#ifndef __FILEIO_H
#define __FILEIO_H

#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>

#define TABLE_NAME_LEN 36
#define DATAFILE_NAME "FMSData.bin"
#define DATAFILE_SIGN "VLSMB_DATAFILE"	// 文件签名，最多15字节
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
}FileStatus;						// 文件状态（删除了还是没删除）

struct DataTableInfo {
	unsigned id;				// 数据表ID
	int dataTableHeaderOffset;		// 数据表头文件偏移（-1代表没有）
	char tableName[36];				// 数据表名称
};

typedef struct DataFileHeader {
	char signature[16];				// 文件签名，字符串，"VLSMB_DATAFILE\0\0"
	unsigned numbersOfTable;		// 当前这个文件头存储的数据表信息数量
	int nextDataFileHeaderOffset;	// 下一个数据文件头部存储的文件偏移（-1代表没有）
	unsigned maxTables;				// 头文件能存储数据表信息应为11个
	struct DataTableInfo tables[11];	// 数据表信息
}DataFileHeader;

typedef struct DataTableHeader {
	char signature[16];				// 文件签名，字符串，"VLSMB_DATATABLE\0"
	unsigned firstDataItemOffset;	// 第一个数据的偏移
}DataTableHeader;

// 错误处理与图形界面相关函数

/**
* 输出错误信息函数
* @param message 错误信息
*/
void errorInfo(const char* message);

/**
* 输出错误信息函数
* @param message 错误信息
* @param hwnd 窗口句柄
*/
void errorInfoForWindow(const char* message, HWND hwnd);

void successInfoForWindow(const char* message, HWND hwnd);

/**
* 输出警告信息函数
* @param message 错误信息
*/
void warnInfo(const char* message);

/**
* 输出警告信息函数
* @param message 错误信息
* @param hwnd 窗口句柄
*/
void warnInfoForWindow(const char* message, HWND hwnd);

// 设置编辑框句柄
void setEditHandle(HWND hEdit);

void clearEditHandle();

void clearEditText();

// 重定义printf和puts
int winPrintf(const char* _format, ...);
int winPuts(const char* str);

#define printf winPrintf
#define puts winPuts

// 图形界面相关函数结束（带关联数据）

/**
* 初始化数据文件
* @return 是否成功
*/
bool initDataFile();

/**
* 检查数据文件是否存在
* @return 是否存在
*/
bool checkDataFileSign();

/**
* 创建新的数据表信息
* @param tableName 数据表名
* @return 是否成功
*/
bool createNewTable(char* tableName);

/**
* 判断数据表是否存在，并返回DataTableHeader所在的文件偏移
* @param tableName 数据表名
* @return 文件偏移，为-1代表不存在
*/
int getTableHeaderOffset(char* tableName);

/**
 * 向指定的数据表插入一条数据
 * 这个函数是一个统一的接口，无法事先知道结构体信息，因此将数据的函数指针作为参数
 * @param tableName 数据表名
 * @param data 数据字节
 * @param structSize 数据大小
 * @param idSetter 函数指针，该函数用于设置data->id
 * @param offsetSetter 函数指针，该函数用于设置data->nextOffset
 * @param offsetGetter 函数指针，该函数用于获取data->nexttOffset
 * @param statusSetter 函数指针，该函数用于设置data->status
 * @return 是否成功
 */
bool insert(char* tableName, void* data, unsigned structSize, void (*idSetter)(void*, int), void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), void (*statusSetter)(void*, FileStatus));

/**
 * 根据ID查找数据在文件中偏移位置
 * 这个函数是一个统一的接口，无法事先知道结构体信息，因此将数据的函数指针作为参数
 * @param tableName 数据表名
 * @param id 数据ID
 * @param structSize 数据大小
 * @param offsetSetter 函数指针，该函数用于设置data->nextOffset
 * @param offsetGetter 函数指针，该函数用于获取data->nextOffset
 * @param idGetter 函数指针，该函数用于获取data->id
 * @return 文件偏移，返回-1
 */
int getOffsetById(char* tableName, int id, unsigned structSize, void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), int (*idGetter)(void*));

/**
 * 根据ID查找数据
 * 这个函数是一个统一的接口，无法事先知道结构体信息，因此将数据的函数指针作为参数
 * @param tableName 数据表名
 * @param id 数据ID
 * @param structSize 数据大小
 * @param pointerSetter 函数指针，该函数用于设置data->nextPointer
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数针对该表的具体实现函数
 * @return 字节数组
 */
void* selectById(char* tableName, int id, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int));

/**
 * 根据ID更新某条数据记录
 * 这个函数是一个统一的接口，无法事先知道结构体信息，因此将数据的函数指针作为参数
 * @param tableName 数据表名
 * @param id 数据ID
 * @param newData 新的数据字节
 * @param structSize 数据大小
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数针对该表的具体实现函数
 * @param copyProperty 函数指针，该函数用于将可更新部分属性从新记录上复制到旧记录上，第一个参数为目标，第二个参数为源
 * @return 是否成功
 */
bool updateById(char* tableName, int id, void* newData, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*copyProperty)(void*, void*));

/**
 * 根据ID删除某条数据记录
 * 这个函数是一个统一的接口，无法事先知道结构体信息，因此将数据的函数指针作为参数
 * @param tableName 数据表名
 * @param id 数据ID
 * @param structSize 数据大小
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数针对该表的具体实现函数
 * @param statusSetter 函数指针，该函数用于设置data->status
 * @return 是否成功
 */
bool deleteById(char* tableName, int id, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*statusSetter)(void*, FileStatus));

/**
 * 获取一个数据表的所有记录，也可以过滤记录。
 * 这个函数是一个统一的接口，无法事先知道结构体信息，因此将数据的函数指针作为参数
 * @param tableName 数据表名
 * @param structSize 数据大小
 * @param pointerSetter 函数指针，该函数用于设置data->nextPointer
 * @param getOffsetByIdForTable 函数指针，值为getOffsetById函数针对该表的具体实现函数
 * @param pointerGetter 函数指针，该函数用于获取data->nextPointer
 * @param statusGetter 函数指针，该函数用于获取data->status
 * @param filter 函数指针，在返回链表数据前会将链表数据传入这个函数，在此函数内根据条件返回过滤后的链表，返回链表头节点。
 *			filter函数的参数为：第一个参数为链表头节点。具体使用方法可见struct.c中的filterForUserCourseRelByUserId函数。
 *			filter函数为NULL则不过滤任何节点
 * @return 链表数据
 */
void* selectAll(char* tableName, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int), void* (*pointerGetter)(void*), FileStatus(*statusGetter)(void*), void* (*filter)(void*));

#endif 
