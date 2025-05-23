#define _CRT_SECURE_NO_WARNINGS
#include "fileio.h"

/**
* ���������Ϣ����
* @param message ������Ϣ
*/
void errorInfo(const char* message) {
	printf("Error: %s\n", message);
}

/**
* ���������Ϣ����
* @param message ������Ϣ
* @param hwnd ���ھ��
*/
void errorInfoForWindow(const char* message, HWND hwnd) {
	MessageBoxA(hwnd, message, "����", MB_ICONERROR);
}

/**
* ���徯����Ϣ����
* @param message ������Ϣ
*/
void warnInfo(const char* message) {
	printf("Warning: %s\n", message);
}

void successInfoForWindow(const char* message, HWND hwnd) {
	MessageBoxA(hwnd, message, "�ɹ�", MB_ICONINFORMATION);
}

/**
* ���徯����Ϣ����
* @param message ������Ϣ
* @param hwnd ���ھ��
*/
void warnInfoForWindow(const char* message, HWND hwnd) {
	MessageBoxA(hwnd, message, "��ʾ", MB_ICONWARNING);
}

HWND PRINTF_EDIT_HWND = INVALID_HANDLE_VALUE;

// ���ñ༭����
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

// ����printf��puts
int winPrintf(const char* _format, ...) {
	// ����_format���ַ���\n��������Ҫ��ӡ��hEdit��ʱ�滻Ϊ\r\n
	char* format = (char*)malloc(sizeof(char) * (strlen(_format) * 2));
	if (format == NULL) {
		MessageBoxA(NULL, "����ϵͳ�ڴ治�㣡", "����", MB_ICONERROR);
		return 0;
	}
	memset(format, 0, sizeof(char) * (strlen(_format) * 2));
	if (PRINTF_EDIT_HWND != INVALID_HANDLE_VALUE) {
		// ��\n�滻Ϊ\r\n
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
		MessageBoxA(NULL, "����ϵͳ�ڴ治�㣡", "����", MB_ICONERROR);
		free(format);
		return 0;
	}
	memset(buffer, 0, szSize);

	// ��ø�ʽ������ַ���
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	if (PRINTF_EDIT_HWND == INVALID_HANDLE_VALUE) {
		// ���û�б༭���������д���׼���
		int ans = fwrite(buffer, sizeof(char), strlen(buffer), stdout);
		free(buffer);
		free(format);
		return ans;
	} else {
		// ��༭������ı�
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
		MessageBoxA(NULL, "����ϵͳ�ڴ治��", "����", MB_ICONERROR);
		return 0;
	}
	// ����س�\n
	memset(buffer, 0, sizeof(char) * (strlen(str) + 10));
	strcpy(buffer, str);
	char* p = buffer;
	while (*p) {
		p++;
	}
	*p = '\n';
	*(p + 1) = '\0';
	// ����winPrintf
	int ans = winPrintf(buffer);
	free(buffer);
	return ans;
}

/**
* ���������ļ�
*/
bool initDataFile() {
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("�����ļ���ʼ��ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ�����ʧ��");
		return false;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	fileHeader->maxTables = 11;
	strcpy(fileHeader->signature, DATAFILE_SIGN);
	fileHeader->nextDataFileHeaderOffset = -1;
	fileHeader->numbersOfTable = 0;
	FILE* file = fopen(DATAFILE_NAME, "wb");
	if (file == NULL) {
		errorInfo("�����ļ���ʼ��ʧ�ܣ�ԭ���ļ���ʧ��");
		return false;
	}
	if (fwrite(fileHeader, sizeof(DataFileHeader), 1, file) != 1) {
		errorInfo("�����ļ���ʼ��ʧ�ܣ�ԭ���ļ�д��ʧ��");
		fclose(file);
		free(fileHeader);
		return false;
	}
	fclose(file);
	free(fileHeader);
	return true;
}

/**
* ��������ļ��Ƿ�����
* @return �Ƿ�����
*/
bool checkDataFileSign() {
	FILE* fp = fopen(DATAFILE_NAME, "rb");
	if (fp == NULL) {
		errorInfo("��������ļ�ʧ�ܣ�ԭ���ļ���ʧ��");
		return false;
	}
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("��������ļ�ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ����ʧ��");
		fclose(fp);
		return false;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	fileHeader->nextDataFileHeaderOffset = 0;
	do {
		fseek(fp, fileHeader->nextDataFileHeaderOffset, SEEK_SET);
		if (fread(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
			errorInfo("��������ļ�ʧ�ܣ�ԭ���ļ���ȡʧ��");
			fclose(fp);
			free(fileHeader);
			return false;
		}
		if (strcmp(fileHeader->signature, DATAFILE_SIGN)) {
			errorInfo("�����ļ�ǩ�������ļ����𻵣�");
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
* ����µ����ݱ���Ϣ
* @param tableName ���ݱ���
* @return �Ƿ�ɹ�
*/
bool createNewTable(char* tableName) {
	if (strlen(tableName) >= TABLE_NAME_LEN) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ�����ݱ����ƹ���");
		return false;
	}
	if (!checkDataFileSign()) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ�������ļ��Ѿ���");
		return false;
	}
	if (getTableHeaderOffset(tableName) != -1) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ�����ݱ��Ѿ�����");
		return false;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ���ļ�������");
		return false;
	}
	fseek(fp, 0, SEEK_SET);
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ�����ʧ��");
		fclose(fp);
		return false;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	if (fread(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ�򣺶�ȡ�����ļ�ͷʧ��");
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
				errorInfo("�������ݱ�ʧ�ܣ�ԭ�򣺶�ȡ�����ļ�ͷʧ��");
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
		// �����µ������ļ�ͷ��Ϣ
		idxOffset += fileHeader->numbersOfTable;
		DataFileHeader* newFileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
		if (newFileHeader == NULL) {
			errorInfo("�������ݱ�ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ�����ʧ��");
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
			errorInfo("�������ݱ�ʧ�ܣ�ԭ������д��ʧ��");
			fclose(fp);
			free(fileHeader);
			free(newFileHeader);
			return false;
		}

		// д�븸�ڵ���Ϣ
		fseek(fp, curOffset, SEEK_SET);
		if (fwrite(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
			errorInfo("�������ݱ�ʧ�ܣ�ԭ������д��ʧ��");
			fclose(fp);
			free(fileHeader);
			free(newFileHeader);
			return false;
		}
		curOffset = fileHeader->nextDataFileHeaderOffset;
		free(fileHeader);
		fileHeader = newFileHeader;
	}
	// ����ֱ���ڴ˴��������ݱ���Ϣ
	fileHeader->tables[fileHeader->numbersOfTable].id = idxOffset + fileHeader->numbersOfTable + 1;
	strcpy(fileHeader->tables[fileHeader->numbersOfTable].tableName, tableName);
	fileHeader->tables[fileHeader->numbersOfTable].dataTableHeaderOffset = -1;
	fileHeader->numbersOfTable++;
	fseek(fp, curOffset, SEEK_SET);
	if (fwrite(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ��д�������ļ�ͷʧ��");
		free(fileHeader);
		fclose(fp);
		return false;
	}
	free(fileHeader);
	fclose(fp);
	return true;
}

/**
* �ж����ݱ��Ƿ���ڣ�������DataTableHeader���ڵ��ļ�ƫ��
* @param tableName ���ݱ���
* @return �ļ�ƫ�ƣ�Ϊ-1��������
*/
int getTableHeaderOffset(char* tableName) {
	if (strlen(tableName) >= TABLE_NAME_LEN) {
		return -1;
	}
	if (!checkDataFileSign()) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ�������ļ���");
		return -1;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ���ļ���ʧ��");
		return -1;
	}
	DataFileHeader* fileHeader = (DataFileHeader*)malloc(sizeof(DataFileHeader));
	if (fileHeader == NULL) {
		errorInfo("�������ݱ�ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ治��");
		fclose(fp);
		return -1;
	}
	memset(fileHeader, 0, sizeof(DataFileHeader));
	int curOffset = 0;
	do {
		curOffset = fileHeader->nextDataFileHeaderOffset;
		fseek(fp, curOffset, SEEK_SET);
		if (fread(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
			errorInfo("�������ݱ�ʧ�ܣ�ԭ���ļ���ȡ����");
			fclose(fp);
			free(fileHeader);
			return -1;
		}
		for (int i = 0; i < fileHeader->numbersOfTable; i++) {
			if (!strcmp(fileHeader->tables[i].tableName, tableName)) {
				if (fileHeader->tables[i].dataTableHeaderOffset == -1) {
					// �����µ����ݱ�ͷ
					DataTableHeader* tableHeader = (DataTableHeader*)malloc(sizeof(DataTableHeader));
					if (tableHeader == NULL) {
						errorInfo("�������ݱ�ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ����");
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
						errorInfo("�������ݱ�ʧ�ܣ�ԭ���ļ�д�����");
						free(fileHeader);
						free(tableHeader);
						fclose(fp);
						return -1;
					}
					fseek(fp, curOffset, SEEK_SET);
					free(tableHeader);
					if (fwrite(fileHeader, sizeof(DataFileHeader), 1, fp) != 1) {
						errorInfo("�������ݱ�ʧ�ܣ�ԭ���ļ�д�����");
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
 * ��ָ�������ݱ������һ������
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param data �����ֽ�
 * @param structSize ���ݴ�С
 * @param idSetter ����ָ�룬�ú���������������data->id
 * @param offsetSetter ����ָ�룬�ú���������������data->nextOffset
 * @param offsetGetter ����ָ�룬�ú����������ǻ��data->nextOffset
 * @param statusSetter ����ָ�룬�ú���������������data->status
 * @return �Ƿ�ɹ�
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
		errorInfo("���ݲ���ʧ�ܣ�ԭ���ļ���ʧ��");
		return false;
	}
	fseek(fp, offset, SEEK_SET);
	DataTableHeader* tableHeader = (DataTableHeader*)malloc(sizeof(DataTableHeader));
	if (tableHeader == NULL) {
		errorInfo("���ݲ���ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ治��");
		fclose(fp);
		return false;
	}
	memset(tableHeader, 0, sizeof(DataTableHeader));
	if (fread(tableHeader, sizeof(DataTableHeader), 1, fp) != 1) {
		errorInfo("���ݲ���ʧ�ܣ�ԭ���ļ���ȡʧ��");
		fclose(fp);
		free(tableHeader);
		return false;
	}
	if (strcmp(tableHeader->signature, DATATABLE_SIGN)) {
		errorInfo("���ݲ���ʧ�ܣ�ԭ��DataTableHeaderǩ������");
		fclose(fp);
		free(tableHeader);
		return false;
	}
	if (tableHeader->firstDataItemOffset == -1) {
		// ����������ǵ�һ������
		fseek(fp, 0, SEEK_END);
		tableHeader->firstDataItemOffset = ftell(fp);
		offsetSetter(data, -1);
		idSetter(data, 1);
		if (fwrite(data, structSize, 1, fp) != 1) {
			errorInfo("���ݲ���ʧ�ܣ�ԭ������д��ʧ��");
			fclose(fp);
			free(tableHeader);
			return false;
		}
		// д������ͷ
		fseek(fp, offset, SEEK_SET);
		if (fwrite(tableHeader, sizeof(DataTableHeader), 1, fp) != 1) {
			errorInfo("���ݲ���ʧ�ܣ�ԭ������д��ʧ��");
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
			errorInfo("���ݲ���ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ治��");
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
				errorInfo("���ݲ���ʧ�ܣ�ԭ���ļ���ȡʧ��");
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
			errorInfo("���ݲ���ʧ�ܣ�ԭ���ļ�д��ʧ��");
			fclose(fp);
			free(tableHeader);
			free(pointer);
			return false;
		}
		fseek(fp, curOffset, SEEK_SET);
		if (fwrite(pointer, structSize, 1, fp) != 1) {
			errorInfo("���ݲ���ʧ�ܣ�ԭ���ļ�д��ʧ��");
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
int getOffsetById(char* tableName, int id, unsigned structSize, void (*offsetSetter)(void*, int), int (*offsetGetter)(void*), int (*idGetter)(void*)) {
	int offset = getTableHeaderOffset(tableName);
	if (offset == -1) {
		return -1;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb");
	if (fp == NULL) {
		errorInfo("��ѯ��Ϣ����ԭ���ļ���ʧ��");
		return -1;
	}
	DataTableHeader* tableHeader = (DataTableHeader*)malloc(sizeof(DataTableHeader));
	if (tableHeader == NULL) {
		errorInfo("��ѯ��Ϣ����ԭ�򣺲���ϵͳ�ڴ治��");
		fclose(fp);
		return -1;
	}
	memset(tableHeader, 0, sizeof(DataTableHeader));
	fseek(fp, offset, SEEK_SET);
	if (fread(tableHeader, sizeof(DataTableHeader), 1, fp) != 1) {
		errorInfo("��ѯ��Ϣ����ԭ���ļ���ȡʧ��");
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
		errorInfo("��ѯ��Ϣ����ԭ�򣺲���ϵͳ�ڴ治��");
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
			errorInfo("��ѯ��Ϣ����ԭ���ļ���ȡʧ��");
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
 * ����ID�������
 * ��������һ��ͳһ�Ľӿڣ��޷���֪����Ľṹ����Ϣ����˽������ݵĺ���ָ����в���
 * @param tableName ���ݱ���
 * @param id ����ID
 * @param structSize ���ݴ�С
 * @param pointerSetter ����ָ�룬�ú���������������data->nextPointer
 * @param getOffsetByIdForTable ����ָ�룬ֵΪgetOffsetById�������ڸñ�ľ���ʵ�ֺ���
 * @return �ֽ�����
 */
void* selectById(char* tableName, int id, unsigned structSize, void (*pointerSetter)(void*, void*), int (*getOffsetByIdForTable)(int)) {
	int offset = getOffsetByIdForTable(id);
	if (offset == -1) {
		return NULL;
	}
	FILE* fp = fopen(DATAFILE_NAME, "rb");
	if (fp == NULL) {
		errorInfo("��ѯ��Ϣ����ԭ���ļ���ʧ��");
		return NULL;
	}
	void* item = malloc(structSize);
	if (item == NULL) {
		errorInfo("��ѯ��Ϣ����ԭ�򣺲���ϵͳ�ڴ治��");
		fclose(fp);
		return NULL;
	}
	fseek(fp, offset, SEEK_SET);
	if (fread(item, structSize, 1, fp) != 1) {
		errorInfo("��ѯ��Ϣ����ԭ�򣺶�ȡ�ļ�ʧ��");
		fclose(fp);
		free(item);
		return NULL;
	}
	pointerSetter(item, NULL);
	fclose(fp);
	return item;
}

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
bool updateById(char* tableName, int id, void* newData, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*copyProperty)(void*, void*)) {
	int offset = getOffsetByIdForTable(id);
	if (offset == -1) {
		return false;
	}
	void* item = malloc(structSize);
	if (item == NULL) {
		errorInfo("��������ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ治��");
		return false;
	}
	memset(item, 0, structSize);
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("��������ʧ�ܣ�ԭ���ļ���ʧ��");
		free(item);
		return false;
	}
	fseek(fp, offset, SEEK_SET);
	if (fread(item, structSize, 1, fp) != 1) {
		errorInfo("��������ʧ�ܣ�ԭ���ļ���ȡʧ��");
		free(item);
		fclose(fp);
		return false;
	}

	// ��������
	copyProperty(item, newData);

	fseek(fp, offset, SEEK_SET);
	if (fwrite(item, structSize, 1, fp) != 1) {
		errorInfo("��������ʧ�ܣ�ԭ���ļ�д��ʧ��");
		free(item);
		fclose(fp);
		return false;
	}
	free(item);
	fclose(fp);
	return true;
}

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
bool deleteById(char* tableName, int id, unsigned structSize, int (*getOffsetByIdForTable)(int), void (*statusSetter)(void*, FileStatus)) {
	int offset = getOffsetByIdForTable(id);
	if (offset == -1) {
		return false;
	}
	void* item = malloc(structSize);
	if (item == NULL) {
		errorInfo("ɾ������ʧ�ܣ�ԭ�򣺲���ϵͳ�ڴ治��");
		return false;
	}
	memset(item, 0, structSize);
	FILE* fp = fopen(DATAFILE_NAME, "rb+");
	if (fp == NULL) {
		errorInfo("ɾ������ʧ�ܣ�ԭ���ļ���ʧ��");
		free(item);
		return false;
	}
	fseek(fp, offset, SEEK_SET);
	if (fread(item, structSize, 1, fp) != 1) {
		errorInfo("ɾ������ʧ�ܣ�ԭ���ļ���ȡʧ��");
		free(item);
		fclose(fp);
		return false;
	}

	// ����ɾ������
	statusSetter(item, FILE_STATUS_DELETE);

	fseek(fp, offset, SEEK_SET);
	if (fwrite(item, structSize, 1, fp) != 1) {
		errorInfo("ɾ������ʧ�ܣ�ԭ���ļ�д��ʧ��");
		free(item);
		fclose(fp);
		return false;
	}
	free(item);
	fclose(fp);
	return true;
}

/**
* ����ɾ�������ݹ��˵�
* @param header ����ͷ�ڵ�
* @param structSize �ṹ���С
* @param statusGetter ����ָ�룬���data->status
* @param pointerSetter ����ָ�룬����data->nextPointer
* @param pointerGetter ����ָ�룬���data->nextPointer
* @return ����ͷ�ڵ�
*/
void* fileStatusFilter(void* header, unsigned structSize, FileStatus(*statusGetter)(void*), void (*pointerSetter)(void*, void*), void* (*pointerGetter)(void*)) {
	void* newHeader = NULL;
	while (header != NULL) {
		if (statusGetter(header) == FILE_STATUS_EXIST) {
			newHeader = malloc(structSize);
			if (newHeader == NULL) {
				errorInfo("����ϵͳ�ڴ治��");
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
				errorInfo("����ϵͳ�ڴ治��");
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
			// ������������ˣ���ִ����������
			if (filter != NULL) {
				header = filter(header);
			}
			return header;
		}
		next = pointerGetter(next);
	}
	errorInfo("δ֪����");
	return NULL;
}