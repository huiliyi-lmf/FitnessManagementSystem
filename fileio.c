#define _CRT_SECURE_NO_WARNINGS
#include "fileio.h"

/**
* 定义错误信息处理
* @param message 错误信息
*/
void errorInfo(const char* message) {
	printf("Error: %s\n", message);
}

/**
* 定义错误信息处理
* @param message 错误信息
* @param hwnd 窗口句柄
*/
void errorInfoForWindow(const char* message, HWND hwnd) {
	MessageBoxA(hwnd, message, "错误", MB_ICONERROR);
}

/**
* 定义警告信息处理
* @param message 警告信息
*/
void warnInfo(const char* message) {
	printf("Warning: %s\n", message);
}

void successInfoForWindow(const char* message, HWND hwnd) {
	MessageBoxA(hwnd, message, "成功", MB_ICONINFORMATION);
}

/**
* 定义警告信息处理
* @param message 警告信息
* @param hwnd 窗口句柄
*/
void warnInfoForWindow(const char* message, HWND hwnd) {
	MessageBoxA(hwnd, message, "提示", MB_ICONWARNING);
}

HWND PRINTF_EDIT_HWND = INVALID_HANDLE_VALUE;

// 设置编辑框句柄
void setEditHandle(HWND hEdit) {
	PRINTF_EDIT_HWND = hEdit;
}

void clearEditHandle() {
	PRINTF_EDIT_HWND = INVALID_HANDLE_VALUE;
}

void clearEditText() {
	if (PRINTF_EDIT_HWND != INVALID_HANDLE_VALUE) {
		SetWindowTextA(PRINTF_EDIT_HWND, "");
	}
}

// 重载printf和puts
int winPrintf(const char* _format, ...) {
	// 检验_format的字符串\n情况，如果要打印到hEdit上时替换为\r\n
	char* format = (char*)malloc(sizeof(char) * (strlen(_format) * 2));
	if (format == NULL) {
		MessageBoxA(NULL, "操作系统内存不足！", "错误", MB_ICONERROR);
		return 0;
	}
	memset(format, 0, sizeof(char) * (strlen(_format) * 2));
	if (PRINTF_EDIT_HWND != INVALID_HANDLE_VALUE) {
		// 将\n替换为\r\n
		char* p = format;
		char* q = _format;
		while (*q) {
			if (*q == '\n' && (q == _format || (q != _format && *(q - 1) != '\r'))) {
				*p = '\r';
				p++;
			}
			*p = *q;
			p++;
			q++;
		}
		*p = '\0';
	} else {
		strcpy(format, _format);
	}

	va_list args;
	va_start(args, format);
	int szSize = vsnprintf(NULL, 0, format, args) + 10;
	va_end(args);

	char* buffer = (char*)malloc(szSize);
	if (buffer == NULL) {
		MessageBoxA(NULL, "操作系统内存不足！", "错误", MB_ICONERROR);
		free(format);
		return 0;
	}
	memset(buffer, 0, szSize);

	// 获得格式化后的字符串
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	if (PRINTF_EDIT_HWND == INVALID_HANDLE_VALUE) {
		// 如果没有编辑框句柄则将输出写入标准输出
		int ans = fwrite(buffer, sizeof(char), strlen(buffer), stdout);
		free(buffer);
		free(format);
		return ans;
	} else {
		// 向编辑框插入文本
		int orgLength = GetWindowTextLengthA(PRINTF_EDIT_HWND);
		SendMessageA(PRINTF_EDIT_HWND, EM_SETSEL, (WPARAM)orgLength, (LPARAM)orgLength);
		SendMessageA(PRINTF_EDIT_HWND, EM_REPLACESEL, TRUE, (LPARAM)buffer);
		free(buffer);
		free(format);
		int curLength = GetWindowTextLengthA(PRINTF_EDIT_HWND);
		return curLength - orgLength;
	}
}

int winPuts(const char* str) {
	char* buffer = (char*)malloc(sizeof(char) * (strlen(str) + 10));
	if (buffer == NULL) {
		MessageBoxA(NULL, "操作系统内存不足", "错误", MB_ICONERROR);
		return 0;
	}
	// 插入回车\n
	memset(buffer, 0, sizeof(char) * (strlen(str) + 10));
	strcpy(buffer, str);
	char* p = buffer;
	while (*p) {
		p++;
	}
	*p = '\n';
	*(p + 1) = '\0';
	// 调用winPrintf
	int ans = winPrintf(buffer);
	free(buffer);
	return ans;
}

/**
* 创建数据文件
*/
bool initDataFile() {
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("数据文件初始化失败，原因：操作系统内存申请失败");
		return false;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	fileHeader->maxTables = 11;
	strcpy(fileHeader->signature, DATAFILE_SIGN);
	fileHeader->nextDataFileHeaderOffset = -1;
	fileHeader->numbersOfTable = 0;
	FILE* file = fopen(DATAFILE_NAME, "wb");
	if (file == NULL) {
		errorInfo("数据文件初始化失败，原因：文件打开失败");
		return false;
	}
	if (fwrite(fileHeader, sizeof(DataFileHeader), 1, file) != 1) {
		errorInfo("数据文件初始化失败，原因：文件写入失败");
		fclose(file);
		free(fileHeader);
		return false;
	}
	fclose(file);
	free(fileHeader);
	return true;
}

/**
* 检查数据文件是否正常
* @return 是否正常
*/
bool checkDataFileSign() {
	FILE* fp = fopen(DATAFILE_NAME, "rb");
	if (fp == NULL) {
		errorInfo("检查数据文件失败，原因：文件打开失败");
		return false;
	}
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("检查数据文件失败，原因：操作系统内存分配失败");
		fclose(fp);
		return false;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	fileHeader->nextDataFileHeaderOffset = 0;
	do {
		fseek(fp, fileHeader->nextDataFileHeaderOffset, SEEK_SET);
		if (fread(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
			errorInfo("检查数据文件失败，原因：文件读取失败");
			fclose(fp);
			free(fileHeader);
			return false;
		}
		if (strcmp(fileHeader->signature, DATAFILE_SIGN)) {
			errorInfo("数据文件签名错误，文件已损坏！");
			fclose(fp);
			free(fileHeader);
			return false;
		}
	} while (fileHeader->nextDataFileHeaderOffset != -1);
	fclose(fp);
	free(fileHeader);
	return true;
}

/**
* 添加新的数据表信息
* @param tableName 数据表名
* @return 是否成功
*/
bool createNewTable(char* tableName) {
	if (strlen(tableName) >= TABLE_NAME_LEN) {
		errorInfo("创建数据表失败，原因：数据表名称过长");
		return false;
	}
	if (!checkDataFileSign()) {
		errorInfo("创建数据表失败，原因：数据文件已经损坏");
		return false;
	}
	if (getTableHeaderOffset(tableName) != -1) {
		errorInfo("创建数据表失败，原因：数据表已经存在");
		return false;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("创建数据表失败，原因：文件不存在");
		return false;
	}
	fseek(fp, 0, SEEK_SET);
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("创建数据表失败，原因：操作系统内存申请失败");
		fclose(fp);
		return false;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	if (fread(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
		errorInfo("创建数据表失败，原因：读取数据文件头失败");
		fclose(fp);
		free(fileHeader);
		return false;
	}
	int curOffset = 0;
	int idxOffset = 0;
	if (fileHeader->numbersOfTable >= fileHeader->maxTables) {
		while (fileHeader->nextDataFileHeaderOffset != -1) {
			idxOffset += fileHeader->numbersOfTable;
			curOffset = fileHeader->nextDataFileHeaderOffset;
			fseek(fp, curOffset, SEEK_SET);
			if (fread(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
				errorInfo("创建数据表失败，原因：读取数据文件头失败");
				fclose(fp);
				free(fileHeader);
				return false;
			}
			if (fileHeader->numbersOfTable < fileHeader->maxTables) {
				break;
			}
		}
	}
	if (fileHeader->numbersOfTable >= fileHeader->maxTables) {
		// 创建新的数据文件头信息
		idxOffset += fileHeader->numbersOfTable;
		DataFileHeader* newFileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
		if (newFileHeader == NULL) {
			errorInfo("创建数据表失败，原因：操作系统内存申请失败");
			fclose(fp);
			free(fileHeader);
			return false;
		}
		memset(newFileHeader, 0, sizeof(DataFileHeader));
		fseek(fp, 0, SEEK_END);
		fileHeader->nextDataFileHeaderOffset = ftell(fp);
		newFileHeader->maxTables = 11;
		newFileHeader->nextDataFileHeaderOffset = -1;
		newFileHeader->numbersOfTable = 0;
		strcpy(newFileHeader->signature, DATAFILE_SIGN);
		if (fwrite(newFileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
			errorInfo("创建数据表失败，原因：数据写入失败");
			fclose(fp);
			free(fileHeader);
			free(newFileHeader);
			return false;
		}

		// 写入父节点信息
		fseek(fp, curOffset, SEEK_SET);
		if (fwrite(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
			errorInfo("创建数据表失败，原因：数据写入失败");
			fclose(fp);
			free(fileHeader);
			free(newFileHeader);
			return false;
		}
		curOffset = fileHeader->nextDataFileHeaderOffset;
		free(fileHeader);
		fileHeader = newFileHeader;
	}
	// 可以直接在此处创建数据表信息
	fileHeader->tables[fileHeader->numbersOfTable].id = idxOffset + fileHeader->numbersOfTable + 1;
	strcpy(fileHeader->tables[fileHeader->numbersOfTable].tableName, tableName);
	fileHeader->tables[fileHeader->numbersOfTable].dataTableHeaderOffset = -1;
	fileHeader->numbersOfTable++;
	fseek(fp, curOffset, SEEK_SET);
	if (fwrite(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
		errorInfo("创建数据表失败，原因：写入数据文件头失败");
		free(fileHeader);
		fclose(fp);
		return false;
	}
	free(fileHeader);
	fclose(fp);
	return true;
}

/**
* 判断数据表是否存在，并返回DataTableHeader所在的文件偏移
* @param tableName 数据表名
* @return 文件偏移，为-1则代表出错
*/
int getTableHeaderOffset(char* tableName) {
	if (strlen(tableName) >= TABLE_NAME_LEN) {
		return -1;
	}
	if (!checkDataFileSign()) {
		errorInfo("查找数据表失败，原因：数据文件损坏");
		return -1;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("查找数据表失败，原因：文件打开失败");
		return -1;
	}
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("查找数据表失败，原因：操作系统内存不足");
		fclose(fp);
		return -1;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	int curOffset = 0;
	do {
		curOffset = fileHeader->nextDataFileHeaderOffset;
		fseek(fp, curOffset, SEEK_SET);
		if (fread(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
			errorInfo("查找数据表失败，原因：文件读取错误");
			fclose(fp);
			free(fileHeader);
			return -1;
		}
		for (int i = 0; i < fileHeader->numbersOfTable; i++) {
			if (!strcmp(fileHeader->tables[i].tableName, tableName)) {
				if (fileHeader->tables[i].dataTableHeaderOffset == -1) {
					// 创建新的数据表头
					DataTableHeader* tableHeader = (DataTableHeader*)malloc(sizeof(DataTableHeader));
					if (tableHeader == NULL) {
						errorInfo("查找数据表失败，原因：操作系统内存错误");
						free(fileHeader);
						fclose(fp);
						return -1;
					}
					memset(tableHeader, 0, sizeof(DataTableHeader));
					strcpy(tableHeader->signature, DATATABLE_SIGN);
					tableHeader->firstDataItemOffset = -1;
					fseek(fp, 0, SEEK_END);
					fileHeader->tables[i].dataTableHeaderOffset = ftell(fp);
					if (fwrite(tableHeader, sizeof(DataTableHeader), 1, fp) != 1) {
						errorInfo("查找数据表失败，原因：文件写入错误");
						free(fileHeader);
						free(tableHeader);
						fclose(fp);
						return -1;
					}
					fseek(fp, curOffset, SEEK_SET);
					free(tableHeader);
					if (fwrite(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
						errorInfo("查找数据表失败，原因：文件写入错误");
						free(fileHeader);
						fclose(fp);
						return -1;
					}
				}
				int ans = fileHeader->tables[i].dataTableHeaderOffset;
				fclose(fp);
				free(fileHeader);
				return ans;
			}
		}
	} while(fileHeader->nextDataFileHeaderOffset != -1);
	fclose(fp);
	free(fileHeader);
	return -1;
}

/**
 * 向指定的数据表中添加一条数据
 * 本函数是一个统一的接口，无法得知具体的结构体信息，因此借助传递的函数指针进行操作
 * @param tableName 数据表名
 * @param data 数据字节
 * @param structSize 数据大小
 * @param idSetter 函数指针，该函数的作用是设置data->id
 * @param offsetSetter 函数指针，该函数的作用是设置data->nextOffset
 * @param offsetGetter 函数指针，该函数的作用是获得data->nextOffset
 * @param statusSetter 函数指针，该函数的作用是设置data->status
 * @return 是否成功
 */
bool insert(char* tableName, void* data, unsigned structSize, void (*idSetter)(void*, int), void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), void (*statusSetter)(void*, FileStatus)) {
	statusSetter(data, FILE_STATUS_EXIST);
	int offset = getTableHeaderOffset(tableName);
	if (offset == -1) {
		createNewTable(tableName);
		offset = getTableHeaderOffset(tableName);
		if (offset == -1) {
			return false;
		}
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("数据插入失败，原因：文件打开失败");
		return false;
	}
	fseek(fp, offset, SEEK_SET);
	DataTableHeader* tableHeader = (DataTableHeader*)malloc(sizeof(DataTableHeader));
	if (tableHeader == NULL) {
		errorInfo("数据插入失败，原因：操作系统内存不足");
		fclose(fp);
		return false;
	}
	memset(tableHeader, 0, sizeof(DataTableHeader));
	if (fread(tableHeader, sizeof(DataTableHeader), 1, fp) != 1) {
		errorInfo("数据插入失败，原因：文件读取失败");
		fclose(fp);
		free(tableHeader);
		return false;
	}
	if (strcmp(tableHeader->signature, DATATABLE_SIGN)) {
		errorInfo("数据插入失败，原因：DataTableHeader签名错误");
		fclose(fp);
		free(tableHeader);
		return false;
	}
	if (tableHeader->firstDataItemOffset == -1) {
		// 插入的数据是第一个数据
		fseek(fp, 0, SEEK_END);
		tableHeader->firstDataItemOffset = ftell(fp);
		offsetSetter(data, -1);
		idSetter(data, 1);
		if (fwrite(data, structSize, 1, fp) != 1) {
			errorInfo("数据插入失败，原因：数据写入失败");
			fclose(fp);
			free(tableHeader);
			return false;
		}
		// 写入数据头
		fseek(fp, offset, SEEK_SET);
		if (fwrite(tableHeader, sizeof(DataTableHeader), 1, fp) != 1) {
			errorInfo("数据插入失败，原因：数据写入失败");
			fclose(fp);
			free(tableHeader);
			return false;
		}
	}
	else {
		int curIdx = 0;
		int curOffset = -1;
		void* pointer = malloc(structSize);
		if (pointer == NULL) {
			errorInfo("数据插入失败，原因：操作系统内存不足");
			fclose(fp);
			free(tableHeader);
			return false;
		}
		memset(pointer, 0, structSize);
		offsetSetter(pointer, tableHeader->firstDataItemOffset);
		do {
			curOffset = offsetGetter(pointer);
			fseek(fp, curOffset, SEEK_SET);
			curIdx++;
			if (fread(pointer, structSize, 1, fp) != 1) {
				errorInfo("数据插入失败，原因：文件读取失败");
				fclose(fp);
				free(tableHeader);
				free(pointer);
				return false;
			}
		} while (offsetGetter(pointer) != -1);
		fseek(fp, 0, SEEK_END);
		offsetSetter(pointer, ftell(fp));
		offsetSetter(data, -1);
		idSetter(data, curIdx + 1);
		if (fwrite(data, structSize, 1, fp) != 1) {
			errorInfo("数据插入失败，原因：文件写入失败");
			fclose(fp);
			free(tableHeader);
			free(pointer);
			return false;
		}
		fseek(fp, curOffset, SEEK_SET);
		if (fwrite(pointer, structSize, 1, fp) != 1) {
			errorInfo("数据插入失败，原因：文件写入失败");
			fclose(fp);
			free(tableHeader);
			free(pointer);
			return false;
		}
		free(pointer);
	}
	fclose(fp);
	free(tableHeader);
	return true;
}

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
int getOffsetById(char* tableName, int id, unsigned structSize, void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), int (*idGetter)(void*)) {
	int offset = getTableHeaderOffset(tableName);
	if (offset == -1) {
		return -1;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb");
	if (fp == NULL) {
		errorInfo("查询信息错误，原因：文件打开失败");
		return -1;
	}
	DataTableHeader* tableHeader = (DataTableHeader*)malloc(sizeof(DataTableHeader));
	if (tableHeader == NULL) {
		errorInfo("查询信息错误，原因：操作系统内存不足");
		fclose(fp);
		return -1;
	}
	memset(tableHeader, 0, sizeof(DataTableHeader));
	fseek(fp, offset, SEEK_SET);
	if (fread(tableHeader, sizeof(DataTableHeader), 1, fp) != 1) {
		errorInfo("查询信息错误，原因：文件读取失败");
		fclose(fp);
		free(tableHeader);
		return -1;
	}
	if (tableHeader->firstDataItemOffset == -1) {
		fclose(fp);
		free(tableHeader);
		return -1;
	}
	void* header = malloc(structSize);
	if (header == NULL) {
		errorInfo("查询信息错误，原因：操作系统内存不足");
		fclose(fp);
		free(tableHeader);
		return -1;
	}
	memset(header, 0, structSize);
	offsetSetter(header, tableHeader->firstDataItemOffset);
	free(tableHeader);
	int curOffset = 0;
	do {
		curOffset = offsetGetter(header);
		fseek(fp, curOffset, SEEK_SET);
		if (fread(header, structSize, 1, fp) != 1) {
			errorInfo("查询信息错误，原因：文件读取失败");
			fclose(fp);
			free(header);
			return -1;
		}
		if (idGetter(header) == id) {
			fclose(fp);
			free(header);
			return curOffset;
		}
	} while (offsetGetter(header) != -1);
	fclose(fp);
	free(header);
	return -1;
}

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
void* selectById(char* tableName, int id, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int)) {
	int offset = getOffsetByIdForTable(id);
	if (offset == -1) {
		return NULL;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb");
	if (fp == NULL) {
		errorInfo("查询信息错误，原因：文件打开失败");
		return NULL;
	}
	void* item = malloc(structSize);
	if (item == NULL) {
		errorInfo("查询信息错误，原因：操作系统内存不足");
		fclose(fp);
		return NULL;
	}
	fseek(fp, offset, SEEK_SET);
	if (fread(item, structSize, 1, fp) != 1) {
		errorInfo("查询信息错误，原因：读取文件失败");
		fclose(fp);
		free(item);
		return NULL;
	}
	pointerSetter(item, NULL);
	fclose(fp);
	return item;
}

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
bool updateById(char* tableName, int id, void* newData, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*copyProperty)(void*, void*)) {
	int offset = getOffsetByIdForTable(id);
	if (offset == -1) {
		return false;
	}
	void* item = malloc(structSize);
	if (item == NULL) {
		errorInfo("更新数据失败，原因：操作系统内存不足");
		return false;
	}
	memset(item, 0, structSize);
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("更新数据失败，原因：文件打开失败");
		free(item);
		return false;
	}
	fseek(fp, offset, SEEK_SET);
	if (fread(item, structSize, 1, fp) != 1) {
		errorInfo("更新数据失败，原因：文件读取失败");
		free(item);
		fclose(fp);
		return false;
	}

	// 拷贝属性
	copyProperty(item, newData);

	fseek(fp, offset, SEEK_SET);
	if (fwrite(item, structSize, 1, fp) != 1) {
		errorInfo("更新数据失败，原因：文件写入失败");
		free(item);
		fclose(fp);
		return false;
	}
	free(item);
	fclose(fp);
	return true;
}

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
bool deleteById(char* tableName, int id, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*statusSetter)(void*, FileStatus)) {
	int offset = getOffsetByIdForTable(id);
	if (offset == -1) {
		return false;
	}
	void* item = malloc(structSize);
	if (item == NULL) {
		errorInfo("删除数据失败，原因：操作系统内存不足");
		return false;
	}
	memset(item, 0, structSize);
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("删除数据失败，原因：文件打开失败");
		free(item);
		return false;
	}
	fseek(fp, offset, SEEK_SET);
	if (fread(item, structSize, 1, fp) != 1) {
		errorInfo("删除数据失败，原因：文件读取失败");
		free(item);
		fclose(fp);
		return false;
	}

	// 设置删除属性
	statusSetter(item, FILE_STATUS_DELETE);

	fseek(fp, offset, SEEK_SET);
	if (fwrite(item, structSize, 1, fp) != 1) {
		errorInfo("删除数据失败，原因：文件写入失败");
		free(item);
		fclose(fp);
		return false;
	}
	free(item);
	fclose(fp);
	return true;
}

/**
* 将已删除的数据过滤掉
* @param header 链表头节点
* @param structSize 结构体大小
* @param statusGetter 函数指针，获得data->status
* @param pointerSetter 函数指针，设置data->nextPointer
* @param pointerGetter 函数指针，获得data->nextPointer
* @return 链表头节点
*/
void* fileStatusFilter(void* header, unsigned structSize, FileStatus(*statusGetter)(void*), void (*pointerSetter)(void*, void*), void* (*pointerGetter)(void*)) {
	void* newHeader = NULL;
	while (header != NULL) {
		if (statusGetter(header) == FILE_STATUS_EXIST) {
			newHeader = malloc(structSize);
			if (newHeader == NULL) {
				errorInfo("操作系统内存不足");
				return header;
			}
			memcpy(newHeader, header, structSize);
			pointerSetter(newHeader, NULL);
			void* t = pointerGetter(header);
			free(header);
			header = t;
			break;
		}
		void* t = pointerGetter(header);
		free(header);
		header = t;
	}
	void* current = newHeader;
	while (header != NULL) {
		if (statusGetter(header) == FILE_STATUS_EXIST) {
			pointerSetter(current, malloc(structSize));
			if (pointerGetter(current) == NULL) {
				errorInfo("操作系统内存不足");
				return header;
			}
			memcpy(pointerGetter(current), header, structSize);
			current = pointerGetter(current);
			pointerSetter(current, NULL);
		}
		void* t = pointerGetter(header);
		free(header);
		header = t;
	}
	return newHeader;
}

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
void* selectAll(char* tableName, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int), void* (*pointerGetter)(void*), FileStatus(*statusGetter)(void*), void* (*filter)(void*)) {
	void* header = selectById(tableName, 1, structSize, pointerSetter, getOffsetByIdForTable);
	if (header == NULL) {
		return header;
	}
	void* next = header;
	pointerSetter(next, NULL);
	for (int i = 2; i; i++) {
		pointerSetter(next, selectById(tableName, i, structSize, pointerSetter, getOffsetByIdForTable));
		if (pointerGetter(next) == NULL) {
			header = fileStatusFilter(header, structSize, statusGetter, pointerSetter, pointerGetter);
			// 如果有条件过滤，则执行条件过滤
			if (filter != NULL) {
				header = filter(header);
			}
			return header;
		}
		next = pointerGetter(next);
	}
	errorInfo("未知错误");
	return NULL;
}