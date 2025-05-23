#define _CRT_SECURE_NO_WARNINGS
#include "gui.h"

HFONT hFont = INVALID_HANDLE_VALUE;

#define STR_BUFFER_MAXLENGTH 2048

const char* MESSAGE_SORT_INFORMATION = " * �������\r\n * ÿ���ؼ��ֵ�ֵΪ����ʱ��˵��Ϊ��������Ϊ����ʱ��Ϊ��������\r\n * ����ؼ��ֵ�ֵΪ0�򲻲������򣬹ؼ��ֵľ���ֵԽС����˵�����ȶ�Խ��\r\n * ����ֵ����Ĺؼ��ֵľ���ֵ��Ӧ����ͬ";

// ��ʼ������
void initFont() {
    hFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
}

// ��ñ༭�������
char* getEditText(HWND hwnd, int idc) {
    int szStr = GetWindowTextLengthA(GetDlgItem(hwnd, idc));
    char* str = (char*)malloc((szStr + 5) * sizeof(char));
    if (str == NULL) {
        errorInfoForWindow("����ϵͳ�ڴ治��", hwnd);
        return NULL;
    }
    memset(str, 0, (szStr + 5) * sizeof(char));
    GetWindowTextA(GetDlgItem(hwnd, idc), str, szStr + 5);
    return str;
}

#define MyCreateWindow(a, b, c, d, e, f, g, h, i, j, k, l) \
SendMessageA(CreateWindowExA(a, b, c, d, e, f, g, h, i, j, k, l), WM_SETFONT, (WPARAM)hFont, TRUE)

// ע�ᴰ���ಢ��������
int WINAPI registerAndCreateWindow(const char* className, const char* title, WNDPROC procFunc, int x, int y, int w, int h, HWND hParent) {
    if (hFont == INVALID_HANDLE_VALUE) {
        initFont();
    }
    WNDCLASSEXA wc = { 0 };
    if (!GetClassInfoExA(GetModuleHandleA(NULL), className, &wc)) {
        memset(&wc, 0, sizeof(WNDCLASSEXA));
        wc.cbSize = sizeof(WNDCLASSEXA);
        wc.lpfnWndProc = procFunc;
        wc.hInstance = GetModuleHandleA(NULL);
        wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = className;
        RegisterClassExA(&wc);
    }

    HWND hwnd = CreateWindowExA(NULL, className, title, WS_OVERLAPPEDWINDOW, x, y, w, h, hParent, NULL, GetModuleHandleA(NULL), NULL);
    if (hwnd == NULL) {
        errorInfoForWindow("��������ʧ�ܣ�", NULL);
        printf("LastError: %d\n", GetLastError());
        return 0;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}

char* WINSCANF_BUFFER = NULL;
bool WINSCANF_FLAG = false;

LRESULT CALLBACK procWinScanf(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        MyCreateWindow(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 10, 540, 30, hwnd, (HMENU)IDC_EDIT_WINSCANF, NULL, NULL);
        MyCreateWindow(NULL, "BUTTON", "ȷ������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 445, 60, 100, 30, hwnd, (HMENU)IDC_BTN_WINSCANF, NULL, NULL);
        if (WINSCANF_BUFFER != NULL) {
            free(WINSCANF_BUFFER);
            WINSCANF_BUFFER = NULL;
        }
        WINSCANF_FLAG = false;
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BTN_WINSCANF && HIWORD(wParam) == BN_CLICKED) {
            int szLength = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_EDIT_WINSCANF));
            if (WINSCANF_BUFFER != NULL) {
                free(WINSCANF_BUFFER);
                WINSCANF_BUFFER = NULL;
            }
            WINSCANF_BUFFER = (char*)malloc(sizeof(char) * (szLength + 10));
            if (WINSCANF_BUFFER == NULL) {
                errorInfoForWindow("����ϵͳ�ڴ治�㣡", hwnd);
                break;
            }
            GetWindowTextA(GetDlgItem(hwnd, IDC_EDIT_WINSCANF), WINSCANF_BUFFER, szLength + 10);
            WINSCANF_FLAG = true;
            DestroyWindow(hwnd);
            PostQuitMessage(0);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}

// ͼ�λ����������
bool winScanf(HWND hParent, const char* title, const char* format, void* p) {
    while (1) {
        registerAndCreateWindow(SCANFWINDOW_CLASS_NAME, title, procWinScanf, 200, 200, 600, 150, hParent);

        // �ж��û������������ݻ���ȡ��
        if (!WINSCANF_FLAG) {
            break;
        }

        // �����Ѿ��������û��Ѿ�����������
        if (sscanf(WINSCANF_BUFFER, format, p) != 1) {
            warnInfoForWindow("������Ϸ������ݣ�", NULL);
        } else {
            EnableWindow(hParent, TRUE);
            return true;
        }
    }
    EnableWindow(hParent, TRUE);
    return false;
}

bool loginWindowStatus = true;  // Ϊtrueʱ�����¼���棬falseΪע�����
// ��¼���ڻص�����
LRESULT CALLBACK procLoginWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
            // �����ؼ�
            MyCreateWindow(NULL, "STATIC", "�ֻ���:", WS_CHILD | WS_VISIBLE, 20, 20, 80, 25, hwnd, NULL, NULL, NULL);
            MyCreateWindow(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 20, 200, 25, hwnd, (HMENU)IDC_EDIT_PHONE, NULL, NULL);
            MyCreateWindow(NULL, "STATIC", "�û���:", WS_CHILD | WS_VISIBLE, 20, 60, 80, 25, hwnd, NULL, NULL, NULL);
            MyCreateWindow(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 60, 200, 25, hwnd, (HMENU)IDC_EDIT_USERNAME, NULL, NULL);
            MyCreateWindow(NULL, "STATIC", "����:", WS_CHILD | WS_VISIBLE, 20, 100, 80, 25, hwnd, NULL, NULL, NULL);
            MyCreateWindow(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, 100, 100, 200, 25, hwnd, (HMENU)IDC_EDIT_PASSWORD, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "��¼", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 20, 160, 100, 30, hwnd, (HMENU)IDC_BTN_LOGIN, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "ע��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 20, 160, 100, 30, hwnd, (HMENU)IDC_BTN_REGISTER, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "�л���ע��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 200, 160, 100, 30, hwnd, (HMENU)IDC_BTN_SWITCH_LOGIN_REGISTER, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "����Ա", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 20, 128, 100, 30, hwnd, (HMENU)IDC_RADIO_LOGIN_ADMIN, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "����", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 128, 100, 30, hwnd, (HMENU)IDC_RADIO_LOGIN_COACH, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "ѧԱ", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 220, 128, 100, 30, hwnd, (HMENU)IDC_RADIO_LOGIN_USER, NULL, NULL);
            // ��ʼ���ؼ���״̬
            SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_ADMIN, BM_SETCHECK, BST_CHECKED, 0);
            ShowWindow(GetDlgItem(hwnd, IDC_BTN_REGISTER), SW_HIDE);
            EnableWindow(GetDlgItem(hwnd, IDC_EDIT_PHONE), FALSE);
            clearEditHandle();
            loginWindowStatus = true;
			break;
		}
        case WM_COMMAND: {
            if (HIWORD(wParam) != BN_CLICKED) {
                break;
            }
            switch (LOWORD(wParam)) {
                case IDC_BTN_LOGIN: {
                    LRESULT result1 = SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_ADMIN, BM_GETCHECK, 0, 0);
                    LRESULT result2 = SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_COACH, BM_GETCHECK, 0, 0);
                    LRESULT result3 = SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_USER, BM_GETCHECK, 0, 0);
                    char* userName = getEditText(hwnd, IDC_EDIT_USERNAME);
                    char* password = getEditText(hwnd, IDC_EDIT_PASSWORD);
                    char* phone = getEditText(hwnd, IDC_EDIT_PHONE);
                    if (userName == NULL || password == NULL) {
                        return FALSE;
                    }
                    // �����ѡ���û�״̬
                    if (result1 == BST_CHECKED) {
                        if (loginForAdmin(userName, password)) {
                            successInfoForWindow("��¼�ɹ���", hwnd);
                            ShowWindow(hwnd, SW_HIDE);
                            // �򿪹���Աҳ��
                            showAdminDashboard();
                            clearEditHandle();
                            ShowWindow(hwnd, SW_SHOW);
                        } else {
                            errorInfoForWindow("��¼ʧ�ܣ�", hwnd);
                        }
                    } else if (result2 == BST_CHECKED) {
                        if (loginForCoach(userName, password)) {
                            successInfoForWindow("��¼�ɹ���", hwnd);
                            ShowWindow(hwnd, SW_HIDE);
                            showCoachDashboard();
                            clearEditHandle();
                            ShowWindow(hwnd, SW_SHOW);
                        } else {
                            errorInfoForWindow("��¼ʧ�ܣ�", hwnd);
                        }
                    } else if (result3 == BST_CHECKED) {
                        if (loginForUser(userName, password, phone)) {
                            successInfoForWindow("��¼�ɹ���", hwnd);
                            ShowWindow(hwnd, SW_HIDE);
                            showUserDashboard();
                            clearEditHandle();
                            ShowWindow(hwnd, SW_SHOW);
                        } else {
                            errorInfoForWindow("��¼ʧ�ܣ�", hwnd);
                        }
                    } else {
                        warnInfoForWindow("������ѡ��һ����¼��ݣ�", hwnd);
                    }
                    if (userName != NULL) {
                        free(userName);
                    }
                    if (password != NULL) {
                        free(password);
                    }
                    if (phone != NULL) {
                        free(phone);
                    }
                    break;
                }
                case IDC_BTN_REGISTER: {
                    LRESULT result1 = SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_ADMIN, BM_GETCHECK, 0, 0);
                    LRESULT result2 = SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_COACH, BM_GETCHECK, 0, 0);
                    LRESULT result3 = SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_USER, BM_GETCHECK, 0, 0);
                    char* userName = getEditText(hwnd, IDC_EDIT_USERNAME);
                    char* password = getEditText(hwnd, IDC_EDIT_PASSWORD);
                    char* phone = getEditText(hwnd, IDC_EDIT_PHONE);
                    if (userName == NULL || password == NULL) {
                        return FALSE;
                    }
                    // �����ѡ״̬
                    if (result1 == BST_CHECKED) {
                        if (registerForAdmin(userName, password)) {
                            successInfoForWindow("ע�����Ա�ɹ���", hwnd);
                        } else {
                            errorInfoForWindow("ע�����Աʧ�ܣ�", hwnd);
                        }
                    } else if (result2 == BST_CHECKED) {
                        // ��û��Ҫ������ֶ�ע��
                    } else if (result3 == BST_CHECKED) {
                        if (registerForUser(userName, password, phone)) {
                            successInfoForWindow("ע��ѧԱ�ɹ���", hwnd);
                        } else {
                            errorInfoForWindow("ע��ѧԱʧ�ܣ�", hwnd);
                        }
                    } else {
                        warnInfoForWindow("������ѡ��һ��ע����ݣ�", hwnd);
                    }
                    if (userName != NULL) {
                        free(userName);
                    }
                    if (password != NULL) {
                        free(password);
                    }
                    break;
                }
                case IDC_BTN_SWITCH_LOGIN_REGISTER:
                    if (loginWindowStatus) {
                        // �л���ע��״̬
                        ShowWindow(GetDlgItem(hwnd, IDC_RADIO_LOGIN_COACH), SW_HIDE);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_REGISTER), SW_SHOW);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_LOGIN), SW_HIDE);
                        SetWindowTextA(hwnd, "��������ϵͳ | �û�ע��");
                        SetWindowTextA(GetDlgItem(hwnd, IDC_BTN_SWITCH_LOGIN_REGISTER), "�л�����¼");
                    } else {
                        // �л�����¼״̬
                        ShowWindow(GetDlgItem(hwnd, IDC_RADIO_LOGIN_COACH), SW_SHOW);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_REGISTER), SW_HIDE);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_LOGIN), SW_SHOW);
                        SetWindowTextA(hwnd, "��������ϵͳ | �û���¼");
                        SetWindowTextA(GetDlgItem(hwnd, IDC_BTN_SWITCH_LOGIN_REGISTER), "�л���ע��");
                    }
                    // �ָ��ؼ���״̬
                    SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_ADMIN, BM_SETCHECK, BST_CHECKED, 0);
                    SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_COACH, BM_SETCHECK, BST_UNCHECKED, 0);
                    SendDlgItemMessageA(hwnd, IDC_RADIO_LOGIN_USER, BM_SETCHECK, BST_UNCHECKED, 0);
                    EnableWindow(GetDlgItem(hwnd, IDC_EDIT_PHONE), FALSE);
                    loginWindowStatus = !loginWindowStatus;
                    break;
                case IDC_RADIO_LOGIN_ADMIN:
                    EnableWindow(GetDlgItem(hwnd, IDC_EDIT_PHONE), FALSE);
                    break;
                case IDC_RADIO_LOGIN_COACH:
                    EnableWindow(GetDlgItem(hwnd, IDC_EDIT_PHONE), FALSE);
                    break;
                case IDC_RADIO_LOGIN_USER:
                    EnableWindow(GetDlgItem(hwnd, IDC_EDIT_PHONE), TRUE);
                    break;
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
		default:
			return DefWindowProcA(hwnd, msg, wParam, lParam);
	}
	return FALSE;
}

/**
* ��ʾ��¼ҳ��
*/
int WINAPI showLoginWindow() {
    return registerAndCreateWindow(LOGINWINDOW_CLASS_NAME, "�������ϵͳ | �û���¼", procLoginWindow, 200, 200, 350, 250, NULL);
}

LRESULT CALLBACK procAdminDashboardWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // ��ʼ�༭��
            HWND hEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", 
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | ES_READONLY,
                10, 10, 650, 450, hwnd, (HMENU)IDC_EDIT_PRINTF, GetModuleHandleA(NULL), NULL);
            SendMessageA(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            setEditHandle(hEdit);

            // �����˵��ṹ
            HMENU hMenu = CreateMenu();
            HMENU hSysMenu = CreatePopupMenu();
            HMENU hCourseTypeMenu = CreatePopupMenu();
            HMENU hCoachMenu = CreatePopupMenu();
            HMENU hCourseMenu = CreatePopupMenu();
            HMENU hCoachCourseRelMenu = CreatePopupMenu();
            HMENU hSearchMenu = CreatePopupMenu();
            HMENU hSortMenu = CreatePopupMenu();
            HMENU hCountMenu = CreatePopupMenu();

            // ϵͳ����˵�
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_LOGOUT, "�˳���¼");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_CHANGEPWD, "�޸ı��˺�����");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_CHANGECOACHPWD, "�޸Ľ�������");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_DATABACKUP, "���ݱ���");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_DATARESTORE, "���ݻָ�");

            // �γ����͹����Ӳ˵�
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_CREATE, "�����γ�����");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_UPDATE, "�޸Ŀγ�����");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_DELETE, "ɾ���γ�����");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_SHOW, "��ʾ���пγ����");

            // ���������Ӳ˵�
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_CREATE, "��ӽ���");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_UPDATE, "�޸Ľ���");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_DELETE, "ɾ������");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_SHOWJUDGE, "��ӡ����˽�����Ϣ");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_MARKJUDGE, "ͨ��������Ϣ���");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_SHOWALL, "��ӡ���н�����Ϣ");

            // �γ̹����Ӳ˵�
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_ADMIN_COURSE_CREATE, "�����γ�");
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_ADMIN_COURSE_UPDATE, "�޸Ŀγ�");
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_ADMIN_COURSE_DELETE, "ɾ���γ�");

            // �����γ̹�ϵ�Ӳ˵�
            AppendMenuA(hCoachCourseRelMenu, MF_STRING, IDC_MENU_ADMIN_COACHCOURSEREL_ADD, "���������γ�");
            AppendMenuA(hCoachCourseRelMenu, MF_STRING, IDC_MENU_ADMIN_COACHCOURSEREL_DELETE, "��������γ̹���");

            // ���������Ӳ˵�
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SHOWCOURSE, "��ʾ���пγ�");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SEARCHCOURSE, "�γ̲�ѯ");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SHOWUSER, "��ʾ�����û�");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SEARCHUSER, "�û���ѯ");

            // ��������Ӳ˵�
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_ADMIN_SORT_COURSE, "�γ�����");
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_ADMIN_SORT_USER, "�û�����");

            // ͳ�ƹ����Ӳ˵�
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COURSE, "�������Ϳγ�ѧԱ����ͳ��");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COURSEBYID, "ĳ�����Ϳγ�ѧԱ����ͳ��");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COACH, "���н�������Ľ���γ�����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COACHBYID, "ĳ����������Ľ���γ�����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_USER, "����ѧԱ�μӵĽ���γ̵�����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_USERBYID, "ĳ��ѧԱ�μӵĽ���γ̵�����");

            // ���Ӳ˵����ص����˵���
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseTypeMenu, "�γ����͹���");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCoachMenu, "��������");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseMenu, "�γ̹���");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCoachCourseRelMenu, "��ѧ��ϵ����");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSearchMenu, "��Ϣ��ѯ");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSortMenu, "������");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCountMenu, "ͳ�Ʒ���");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSysMenu, "ϵͳ����");
            SetMenu(hwnd, hMenu);

            break;
        }
        case WM_COMMAND: {
            if (HIWORD(wParam) != BN_CLICKED) {
                break;
            }
            // ����տ���̨�����
            clearEditText();
            switch (LOWORD(wParam)) {
                case IDC_MENU_ADMIN_LOGOUT: {
                    // �˳���¼�߼�
                    if (MessageBoxA(hwnd, "�Ƿ�Ҫ�˳���¼��", "ѯ��", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        DestroyWindow(hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_CHANGEPWD: {
                    // �޸ı��˺������߼�
                    char* newPassword = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (newPassword == NULL) {
                        errorInfoForWindow("����ϵͳ�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "�������µ����룺", "%s", newPassword)) {
                        free(newPassword);
                        break;
                    }
                    if (changePasswordForAdmin(newPassword)) {
                        successInfoForWindow("�����޸ĳɹ���", hwnd);
                    } else {
                        warnInfoForWindow("�����޸�ʧ��", hwnd);
                    }
                    free(newPassword);
                    break;
                }
                case IDC_MENU_ADMIN_CHANGECOACHPWD: {
                    // �޸Ľ��������߼�
                    int coachId;
                    if (!winScanf(hwnd, "���������ID��", "%d", &coachId)) {
                        break;
                    }
                    char* newPassword = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (newPassword == NULL) {
                        errorInfoForWindow("os�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "����������������룺", "%s", newPassword)) {
                        free(newPassword);
                        break;
                    }
                    if (changeCoachPasswordForAdmin(coachId, newPassword)) {
                        successInfoForWindow("�޸Ľ�������ɹ���", hwnd);
                    } else {
                        errorInfoForWindow("�޸Ľ�������ʧ�ܣ�", hwnd);
                    }
                    free(newPassword);
                    break;
                }
                case IDC_MENU_ADMIN_DATABACKUP: {
                    // ���ݱ����߼�
                    char* filePath = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (filePath == NULL) {
                        errorInfoForWindow("os�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "�����뱸���ļ�����", "%s", filePath)) {
                        free(filePath);
                        break;
                    }
                    if (databaseBackup(filePath)) {
                        successInfoForWindow("���ݱ��ݳɹ���", hwnd);
                    } else {
                        errorInfoForWindow("���ݱ���ʧ�ܣ�", hwnd);
                    }
                    free(filePath);
                    break;
                }
                case IDC_MENU_ADMIN_DATARESTORE: {
                    // ���ݻָ��߼�
                    char* filePath = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (filePath == NULL) {
                        errorInfoForWindow("os�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "������ָ������ļ�����", "%s", filePath)) {
                        free(filePath);
                        break;
                    }
                    if (databaseRestore(filePath)) {
                        successInfoForWindow("���ݻָ��ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("���ݻָ�ʧ�ܣ�", hwnd);
                    }
                    free(filePath);
                    break;
                }

                // �γ����͹���
                case IDC_MENU_ADMIN_COURSETYPE_CREATE: {
                    char* typeName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!typeName) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "������γ��������ƣ�", "%s", typeName)) {
                        free(typeName);
                        break;
                    }
                    if (createCourseType(typeName)) {
                        successInfoForWindow("�����ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    free(typeName);
                    break;
                }
                case IDC_MENU_ADMIN_COURSETYPE_UPDATE: {
                    int id;
                    if (!winScanf(hwnd, "������γ�����ID��", "%d", &id)) break;
                    char* newName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!newName) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "�����������ƣ�", "%s", newName)) {
                        free(newName);
                        break;
                    }
                    if (updateCourseType(id, newName)) {
                        successInfoForWindow("�޸ĳɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("�޸�ʧ��", hwnd);
                    }
                    free(newName);
                    break;
                }
                case IDC_MENU_ADMIN_COURSETYPE_DELETE: {
                    int id;
                    if (!winScanf(hwnd, "������Ҫɾ���Ŀγ�����ID��", "%d", &id)) break;
                    if (deleteCourseType(id)) {
                        successInfoForWindow("ɾ���ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("ɾ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COURSETYPE_SHOW: {
                    if (showAllCourseTypeInfo()) {
                        successInfoForWindow("��ѯ�ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ�ܣ�", hwnd);
                    }
                    break;
                }

                // ��������
                case IDC_MENU_ADMIN_COACH_CREATE: {
                    char* name = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* pass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!name || !pass) {
                        free(name);
                        free(pass);
                        errorInfoForWindow("�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "�������������", "%s", name) ||
                        !winScanf(hwnd, "����������룺", "%s", pass)) {
                        free(name);
                        free(pass);
                        break;
                    }
                    if (createCoach(name, pass)) {
                        successInfoForWindow("�����ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    free(name);
                    free(pass);
                    break;
                }
                case IDC_MENU_ADMIN_COACH_UPDATE: {
                    int id;
                    if (!winScanf(hwnd, "�������ID��", "%d", &id)) break;

                    char* newName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* newPass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int newLevel = -1;
                    if (!newName || !newPass) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        free(newName);
                        free(newPass);
                        break;
                    }

                    if (!winScanf(hwnd, "������������-���޸ģ���", "%s", newName)) {
                        free(newName);
                        free(newPass);
                        break;
                    }
                    if (!winScanf(hwnd, "���������루-���޸ģ���", "%s", newPass)) {
                        break;
                    }
                    // ��������
                    if (strcmp(newName, "-") == 0) {
                        free(newName);
                        newName = NULL;
                    }
                    // ��������
                    if (strcmp(newPass, "-") == 0) {
                        free(newPass);
                        newPass = NULL;
                    }
                    // ����ȼ�
                    winScanf(hwnd, "�����µȼ���-1���޸ģ���", "%d", &newLevel);

                    if (updateCoach(id, newName, newPass, (CoachLevel)newLevel)) {
                        successInfoForWindow("���³ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }

                    if (newName) free(newName);
                    if (newPass) free(newPass);
                    break;
                }
                case IDC_MENU_ADMIN_COACH_DELETE: {
                    int id;
                    if (!winScanf(hwnd, "����Ҫɾ���Ľ���ID��", "%d", &id)) break;
                    if (deleteCoach(id)) {
                        successInfoForWindow("ɾ���ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("ɾ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACH_SHOWJUDGE: {
                    if (showNeedJudgeCoachInfo()) {
                        successInfoForWindow("����ʾ�������Ϣ", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ȡʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACH_MARKJUDGE: {
                    int id, status;
                    if (!winScanf(hwnd, "�������ID��", "%d", &id) ||
                        !winScanf(hwnd, "�������״̬��0�ܾ���1ͨ������", "%d", &status)) break;
                    if (markJudgeCoachInfoStatus(id, (CoachStatus)status)) {
                        successInfoForWindow("��ǳɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("���ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACH_SHOWALL: {
                    if (showAllCoachInfo()) {
                        successInfoForWindow("��ѯ�ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }
                    break;
                }

                // �γ̹���
                case IDC_MENU_ADMIN_COURSE_CREATE: {
                    char* name = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int type, time, stuMax;
                    char* location = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    double price;

                    if (!name || !location) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        free(name);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "�γ����ƣ�", "%s", name) ||
                        !winScanf(hwnd, "�γ�����ID��", "%d", &type) ||
                        !winScanf(hwnd, "ʱ�䣨��������", "%d", &time) ||
                        !winScanf(hwnd, "�ص㣺", "%s", location) ||
                        !winScanf(hwnd, "���ѧ������", "%d", &stuMax) ||
                        !winScanf(hwnd, "�۸�", "%lf", &price)) {
                        free(name);
                        free(location);
                        break;
                    }

                    if (createCourse(name, type, time, location, stuMax, price)) {
                        successInfoForWindow("�����ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }

                    free(name);
                    free(location);
                    break;
                }
                case IDC_MENU_ADMIN_COURSE_UPDATE: {
                    int id;
                    if (!winScanf(hwnd, "����γ�ID��", "%d", &id)) break;

                    char* name = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int type, time, stuMax;
                    char* location = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    double price;

                    if (!name || !location) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        free(name);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "�¿γ����ƣ�", "%s", name) ||
                        !winScanf(hwnd, "�¿γ�����ID��", "%d", &type) ||
                        !winScanf(hwnd, "��ʱ�䣺", "%d", &time) ||
                        !winScanf(hwnd, "�µص㣺", "%s", location) ||
                        !winScanf(hwnd, "�����ѧ������", "%d", &stuMax) ||
                        !winScanf(hwnd, "�¼۸�", "%lf", &price)) {
                        free(name);
                        free(location);
                        break;
                    }

                    if (updateCourse(id, name, type, time, location, stuMax, price)) {
                        successInfoForWindow("���³ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }

                    free(name);
                    free(location);
                    break;
                }
                case IDC_MENU_ADMIN_COURSE_DELETE: {
                    int id;
                    if (!winScanf(hwnd, "����Ҫɾ���Ŀγ�ID��", "%d", &id)) break;
                    if (deleteByIdForCourse(id)) {
                        successInfoForWindow("ɾ���ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("ɾ��ʧ��", hwnd);
                    }
                    break;
                }

                // �����γ̹�ϵ
                case IDC_MENU_ADMIN_COACHCOURSEREL_ADD: {
                    int courseId, coachId;
                    if (!winScanf(hwnd, "����γ�ID��", "%d", &courseId) ||
                        !winScanf(hwnd, "�������ID��", "%d", &coachId)) break;
                    if (addCoachCourseRel(courseId, coachId)) {
                        successInfoForWindow("�����ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACHCOURSEREL_DELETE: {
                    int courseId, coachId;
                    if (!winScanf(hwnd, "����γ�ID��", "%d", &courseId) ||
                        !winScanf(hwnd, "�������ID��", "%d", &coachId)) break;
                    if (deleteCoachCourseRel(courseId, coachId)) {
                        successInfoForWindow("����ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("���ʧ��", hwnd);
                    }
                    break;
                }

                // ��Ϣ��ѯ
                case IDC_MENU_ADMIN_SEARCH_SHOWCOURSE: {
                    if (showCourseInfoForAdmin()) {
                        successInfoForWindow("����ʾ���пγ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_SEARCH_SEARCHCOURSE: {
                    char* courseName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* courseType = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* location = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int stuMaxLow = -1, stuMaxHigh = -1;
                    double priceLow = -1, priceHigh = -1;
                    int timeLow = -1, timeHigh = -1;

                    if (!courseName || !courseType || !location) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        free(courseName);
                        free(courseType);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "�γ����ƣ�-��ɸѡ����", "%s", courseName)
                        || !winScanf(hwnd, "�γ����ͣ�-��ɸѡ����", "%s", courseType)
                        || !winScanf(hwnd, "�ص㣨-��ɸѡ����", "%s", location)
                        || !winScanf(hwnd, "ѧ���������ޣ�-1�����ƣ���", "%d", &stuMaxLow)
                        || !winScanf(hwnd, "ѧ���������ޣ�-1�����ƣ���", "%d", &stuMaxHigh)
                        || !winScanf(hwnd, "�۸����ޣ�-1�����ƣ���", "%lf", &priceLow)
                        || !winScanf(hwnd, "�۸����ޣ�-1�����ƣ���", "%lf", &priceHigh)
                        || !winScanf(hwnd, "ʱ�����ޣ�-1�����ƣ���", "%d", &timeLow)
                        || !winScanf(hwnd, "ʱ�����ޣ�-1�����ƣ���", "%d", &timeHigh)) {
                        break;
                    }

                    // ��������
                    if (strcmp(courseName, "-") == 0) {
                        free(courseName);
                        courseName = NULL;
                    }
                    if (strcmp(courseType, "-") == 0) {
                        free(courseType);
                        courseType = NULL;
                    }
                    if (strcmp(location, "-") == 0) {
                        free(location);
                        location = NULL;
                    }

                    bool res = searchCourseInfoForAdmin(courseName, courseType, location,
                        stuMaxLow, stuMaxHigh, priceLow, priceHigh, timeLow, timeHigh);
                    if (res) {
                        successInfoForWindow("��ѯ�ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }

                    if (courseName) free(courseName);
                    if (courseType) free(courseType);
                    if (location) free(location);
                    break;
                }
                case IDC_MENU_ADMIN_SEARCH_SHOWUSER: {
                    if (showUserInfoForAdmin()) {
                        successInfoForWindow("����ʾ����ѧԱ", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_SEARCH_SEARCHUSER: {
                    char* userName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!userName) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "����ѧԱ�û�����-��ɸѡ����", "%s", userName)) {
                        break;
                    }
                    if (strcmp(userName, "-") == 0) {
                        free(userName);
                        userName = NULL;
                    }
                    bool res = searchUserInfoForAdmin(userName);
                    if (res) {
                        successInfoForWindow("��ѯ�ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }
                    free(userName);
                    break;
                }

                // ������
                case IDC_MENU_ADMIN_SORT_COURSE: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "˵��", MB_ICONINFORMATION);
                    int byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice;
                    if (!winScanf(hwnd, "�������������ȼ�������", "%d", &byName) ||
                        !winScanf(hwnd, "������ID�������ȼ�������", "%d", &byTypeId) ||
                        !winScanf(hwnd, "��ʱ���������ȼ�������", "%d", &byTime) ||
                        !winScanf(hwnd, "���ص��������ȼ�������", "%d", &byLocation) ||
                        !winScanf(hwnd, "��ѧ�����������ȼ�������", "%d", &byStuCount) ||
                        !winScanf(hwnd, "�����ѧ�����������ȼ�������", "%d", &byStuMax) ||
                        !winScanf(hwnd, "���������������ȼ�������", "%d", &byCoachNum) ||
                        !winScanf(hwnd, "���۸��������ȼ�������", "%d", &byPrice)) break;

                    if (sortCourseInfoForAdmin(byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice)) {
                        successInfoForWindow("����ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_SORT_USER: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "˵��", MB_ICONINFORMATION);
                    int byName, bySelectCourse;
                    if (!winScanf(hwnd, "�������������ȼ�������", "%d", &byName) ||
                        !winScanf(hwnd, "���γ����������ȼ�������", "%d", &bySelectCourse)) break;
                    if (sortUserInfoForAdmin(byName, bySelectCourse)) {
                        successInfoForWindow("����ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    break;
                }

                // ͳ�Ʒ���
                case IDC_MENU_ADMIN_COUNT_COURSE: {
                    if (countCourseTypeSelectedForAdmin()) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_COACH: {
                    if (countCoachCourseForAdmin()) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_USER: {
                    if (countUserCourseForAdmin()) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_COURSEBYID: {
                    int id;
                    if (!winScanf(hwnd, "������γ�����ID��", "%d", &id)) {
                        break;
                    }
                    if (countCourseTypeSelectedByIdForAdmin(id)) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_COACHBYID: {
                    int id;
                    if (!winScanf(hwnd, "���������ID��", "%d", &id)) {
                        break;
                    }
                    if (countCoachCourseByIdForAdmin(id)) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_USERBYID: {
                    int id;
                    if (!winScanf(hwnd, "������ѧԱID��", "%d", &id)) {
                        break;
                    }
                    if (countUserCourseByIdForAdmin(id)) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                default:
                    return DefWindowProcA(hwnd, msg, wParam, lParam);
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return FALSE;
}

// ��ʾ����Ա����̨
int WINAPI showAdminDashboard() {
    return registerAndCreateWindow(ADMINWINDOW_CLASS_NAME, "�������ϵͳ | ����Ա����̨", procAdminDashboardWindow, 200, 200, 700, 500, NULL);
}

LRESULT CALLBACK procCoachDashboardWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // ��ʼ�༭��
            HWND hEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | ES_READONLY,
                10, 10, 650, 450, hwnd, (HMENU)IDC_EDIT_PRINTF, GetModuleHandleA(NULL), NULL);
            SendMessageA(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            setEditHandle(hEdit);

            // �����˵�
            HMENU hMenu = CreateMenu();
            HMENU hSysMenu = CreatePopupMenu();
            HMENU hCourseTypeMenu = CreatePopupMenu();
            HMENU hSearchMenu = CreatePopupMenu();
            HMENU hSortMenu = CreatePopupMenu();
            HMENU hCountMenu = CreatePopupMenu();

            // �������˵�������Ӳ˵�
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseTypeMenu, "�γ�������");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSearchMenu, "���ݲ�ѯ");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSortMenu, "��������");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCountMenu, "����ͳ��");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSysMenu, "ϵͳά��");

            // ���γ��������Ӳ˵�
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_COACH_COURSETYPE_ADD, "����ó��Ŀγ�����");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_COACH_COURSETYPE_DELETE, "ɾ���ó��Ŀγ�����");

            // ������ݲ�ѯ�Ӳ˵�
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_COURSE_SHOW, "��ӡ���вμӵĿγ�");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_COURSE_SEARCH, "��ѯ�μӵĿγ�");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_USER_SHOW, "��ӡ�����Ͽε�ѧԱ");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_USER_SEARCH, "��ѯ�Ͽε�ѧԱ");

            // ������������Ӳ˵�
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_COACH_SORT_COURSE, "���вμӵĿγ�����");
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_COACH_SORT_USER, "�����Ͽε�ѧԱ����");

            // �������ͳ���Ӳ˵�
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_COURSETYPE, "ͳ�Ƶ�ǰ�����������Ϳγ̵�ѧԱ����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_COURSETYPEBYID, "ͳ�Ƶ�ǰ����ĳ�����Ϳγ̵�ѧԱ����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_USERCOURSE, "ͳ�Ƶ�ǰ��������ѧԱ�μӵĽ���γ̵�����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_USERCOURSEBYID, "ͳ�Ƶ�ǰ����ĳ��ѧԱ�μӵĽ���γ̵�����");

            // ���ϵͳά���Ӳ˵�
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_COACH_SYS_CHANGEPWD, "�޸�����");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_COACH_SYS_LOGOUT, "�˳���¼");

            SetMenu(hwnd, hMenu);
            break;
        }
        case WM_COMMAND: {
            if (HIWORD(wParam) != BN_CLICKED) {
                break;
            }
            clearEditText();
            switch (LOWORD(wParam)) {
                //-- �γ������� --
                case IDC_MENU_COACH_COURSETYPE_ADD: {
                    int typeId;
                    if (!winScanf(hwnd, "�������ó�����ID��", "%d", &typeId)) {
                        break;
                    }
                    if (addNewCoachTypeRel(typeId)) {
                        successInfoForWindow("��ӳɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("���ʧ�ܣ�", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COURSETYPE_DELETE: {
                    int typeId;
                    if (!winScanf(hwnd, "������Ҫɾ�����ó�����ID��", "%d", &typeId)) break;
                    if (removeCoachTypeRel(typeId)) {
                        successInfoForWindow("ɾ���ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("ɾ��ʧ�ܣ�", hwnd);
                    }
                    break;
                }

                //-- ���ݲ�ѯ --
                case IDC_MENU_COACH_SEARCH_COURSE_SHOW: {
                    if (showCourseInfoForCoach()) {
                        successInfoForWindow("����ʾ���пγ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ȡʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_SEARCH_COURSE_SEARCH: {
                    char* courseName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* courseType = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* location = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int stuMaxLow = -1, stuMaxHigh = -1;
                    double priceLow = -1, priceHigh = -1;
                    int timeLow = -1, timeHigh = -1;

                    if (!courseName || !courseType || !location) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        free(courseName);
                        free(courseType);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "�γ����ƣ�-��ɸѡ����", "%s", courseName) ||
                        !winScanf(hwnd, "�γ����ͣ�-��ɸѡ����", "%s", courseType) ||
                        !winScanf(hwnd, "�ص㣨-��ɸѡ����", "%s", location) ||
                        !winScanf(hwnd, "ѧ���������ޣ�-1�����ƣ���", "%d", &stuMaxLow) ||
                        !winScanf(hwnd, "ѧ���������ޣ�-1�����ƣ���", "%d", &stuMaxHigh) ||
                        !winScanf(hwnd, "�۸����ޣ�-1�����ƣ���", "%lf", &priceLow) ||
                        !winScanf(hwnd, "�۸����ޣ�-1�����ƣ���", "%lf", &priceHigh) ||
                        !winScanf(hwnd, "ʱ�����ޣ�-1�����ƣ���", "%d", &timeLow) ||
                        !winScanf(hwnd, "ʱ�����ޣ�-1�����ƣ���", "%d", &timeHigh)) {
                        break;
                    }

                    // �����������
                    if (strcmp(courseName, "-") == 0) {
                        free(courseName);
                        courseName = NULL;
                    }
                    if (strcmp(courseType, "-") == 0) {
                        free(courseType);
                        courseType = NULL;
                    }
                    if (strcmp(location, "-") == 0) {
                        free(location);
                        location = NULL;
                    }
                   
                    bool res = searchCourseInfoForCoach(courseName, courseType, location,
                        stuMaxLow, stuMaxHigh, priceLow, priceHigh, timeLow, timeHigh);
                    if (res) {
                        successInfoForWindow("��ѯ�ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }

                    if (courseName) free(courseName);
                    if (courseType) free(courseType);
                    if (location) free(location);
                    break;
                }
                case IDC_MENU_COACH_SEARCH_USER_SHOW: {
                    if (showUserInfoForCoach()) {
                        successInfoForWindow("����ʾ����ѧԱ", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_SEARCH_USER_SEARCH: {
                    char* userName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!userName) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "����ѧԱ�û�����", "%s", userName)) {
                        break;
                    }
                    bool res = searchUserInfoForCoach(userName);
                    if (res) {
                        successInfoForWindow("��ѯ�ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }
                    free(userName);
                    break;
                }

                //-- �������� --
                case IDC_MENU_COACH_SORT_COURSE: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "˵��", MB_ICONINFORMATION);
                    int byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice;
                    if (!winScanf(hwnd, "�������������ȼ�������", "%d", &byName) ||
                        !winScanf(hwnd, "������ID�������ȼ�������", "%d", &byTypeId) ||
                        !winScanf(hwnd, "��ʱ���������ȼ�������", "%d", &byTime) ||
                        !winScanf(hwnd, "���ص��������ȼ�������", "%d", &byLocation) ||
                        !winScanf(hwnd, "��ѧ�����������ȼ�������", "%d", &byStuCount) ||
                        !winScanf(hwnd, "�����ѧ�����������ȼ�������", "%d", &byStuMax) ||
                        !winScanf(hwnd, "���������������ȼ�������", "%d", &byCoachNum) ||
                        !winScanf(hwnd, "���۸��������ȼ�������", "%d", &byPrice)) break;

                    if (sortCourseInfoForCoach(byName, byTypeId, byTime, byLocation,
                        byStuCount, byStuMax, byCoachNum, byPrice)) {
                        successInfoForWindow("����ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_SORT_USER: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "˵��", MB_ICONINFORMATION);
                    int byName, bySelectCourse;
                    if (!winScanf(hwnd, "�������������ȼ�������", "%d", &byName) ||
                        !winScanf(hwnd, "���γ����������ȼ�������", "%d", &bySelectCourse)) break;
                    if (sortUserInfoForCoach(byName, bySelectCourse)) {
                        successInfoForWindow("����ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    break;
                }

                //-- ����ͳ�� --
                case IDC_MENU_COACH_COUNT_COURSETYPE: {
                    if (countCourseTypeSelectedForCoach()) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COUNT_COURSETYPEBYID: {
                    int typeId;
                    if (!winScanf(hwnd, "����γ�����ID��", "%d", &typeId)) break;
                    if (countCourseTypeSelectedByIdForCoach(typeId)) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COUNT_USERCOURSE: {
                    if (countUserCourseForCoach()) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COUNT_USERCOURSEBYID: {
                    int userId;
                    if (!winScanf(hwnd, "����ѧԱID��", "%d", &userId)) break;
                    if (countUserCourseByIdForCoach(userId)) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }

                //-- ϵͳά�� --
                case IDC_MENU_COACH_SYS_CHANGEPWD: {
                    char* newPass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!newPass) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "���������룺", "%s", newPass)) {
                        free(newPass);
                        break;
                    }
                    if (changePasswordForCoach(newPass)) {
                        successInfoForWindow("�����޸ĳɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("�����޸�ʧ��", hwnd);
                    }
                    free(newPass);
                    break;
                }
                case IDC_MENU_COACH_SYS_LOGOUT: {
                    if (MessageBoxA(hwnd, "�Ƿ�Ҫ�˳���¼��", "ѯ��", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
                        DestroyWindow(hwnd);
                    }
                    break;
                }
                default:
                    return DefWindowProcA(hwnd, msg, wParam, lParam);
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return FALSE;
}

// ��ʾ��������̨
int WINAPI showCoachDashboard() {
    return registerAndCreateWindow(COACHWINDOW_CLASS_NAME, "�������ϵͳ | ��������̨", procCoachDashboardWindow, 200, 200, 700, 500, NULL);
}

LRESULT CALLBACK procUserDashboardWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // ���ñ༭��
            HWND hEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | ES_READONLY,
                10, 10, 650, 450, hwnd, (HMENU)IDC_EDIT_PRINTF, GetModuleHandleA(NULL), NULL);
            SendMessageA(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            setEditHandle(hEdit);

            // �����˵�
            HMENU hMenu = CreateMenu();
            HMENU hSysMenu = CreatePopupMenu();
            HMENU hCourseMenu = CreatePopupMenu();
            HMENU hSearchMenu = CreatePopupMenu();
            HMENU hSortMenu = CreatePopupMenu();
            HMENU hCountMenu = CreatePopupMenu();

            // �������˵���
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseMenu, "�γ̲���");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSearchMenu, "���ݲ�ѯ");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSortMenu, "��������");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCountMenu, "����ͳ��");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSysMenu, "ϵͳ����");

            // ϵͳ�����Ӳ˵�
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_USER_SYS_CHANGEINFO, "�޸ĸ�����Ϣ");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_USER_SYS_CHANGEPWD, "�޸�����");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_USER_SYS_LOGOUT, "�˳���¼");

            // �γ̲����Ӳ˵�
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_USER_COURSE_ADD, "��ӿγ�");
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_USER_COURSE_DELETE, "ɾ���γ�");

            // ���������Ӳ˵�
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_USER_SEARCH_SHOWCOURSE, "��ӡ��ǰ�û����пγ���Ϣ");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_USER_SEARCH_SEARCHCOURSE, "��ѯ��ǰ�û��γ���Ϣ");

            // ���������Ӳ˵�
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_USER_SORT_COURSE, "�Ե�ǰ�û��γ���Ϣ��������");

            // ����ͳ���Ӳ˵�
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COURSETYPE, "��ӡ��ǰ�û��������Ϳγ̵�ѧԱ����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COURSETYPEBYID, "��ӡ��ǰ�û�ĳ�����Ϳγ̵�ѧԱ����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COACH, "��ӡ���н�������ǰ�û��Ľ���γ�����");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COACHBYID, "��ӡĳ��������ĵ�ǰ�û�����γ�����");

            SetMenu(hwnd, hMenu);

            break;
        }
        case WM_COMMAND: {
            if (HIWORD(wParam) != BN_CLICKED) {
                break;
            }
            clearEditText();
            switch (LOWORD(wParam)) {
                // ϵͳ����
                case IDC_MENU_USER_SYS_CHANGEINFO: {
                    char* newName = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    char* newPhone = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (newName == NULL || newPhone == NULL) {
                        errorInfoForWindow("os�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "�������µ��û�����-���ı䣩��", "%s", newName) || !winScanf(hwnd, "�������µ��ֻ��ţ�-���ı䣩��", "%s", newPhone)) {
                        free(newName);
                        free(newPhone);
                        break;
                    }

                    if (strcmp(newName, "-") == 0) {
                        free(newName);
                        newName = NULL;
                    }
                    if (strcmp(newPhone, "-") == 0) {
                        free(newPhone);
                        newPhone = NULL;
                    }

                    if (changeInfoForUser(newName, newPhone)) {
                        successInfoForWindow("�޸��û���Ϣ�ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("�޸��û���Ϣʧ�ܣ�", hwnd);
                    }
                    if (newName) free(newName);
                    if (newPhone)free(newPhone);
                    break;
                }
                case IDC_MENU_USER_SYS_CHANGEPWD: {
                    char* newPass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!newPass) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "���������룺", "%s", newPass)) {
                        free(newPass);
                        break;
                    }
                    if (changePasswordForUser(newPass)) {
                        successInfoForWindow("�����޸ĳɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("�����޸�ʧ��", hwnd);
                    }
                    free(newPass);
                    break;
                }
                case IDC_MENU_USER_SYS_LOGOUT: {
                    if (MessageBoxA(hwnd, "�Ƿ��˳���¼��", "ѯ��", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
                        DestroyWindow(hwnd);
                    }
                    break;
                }

                // �γ̲���
                case IDC_MENU_USER_COURSE_ADD: {
                    int courseId;
                    if (!winScanf(hwnd, "����Ҫ�����Ŀγ�ID��", "%d", &courseId)) break;
                    if (addUserCourseRel(courseId)) {
                        successInfoForWindow("�γ̱����ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("�γ̱���ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COURSE_DELETE: {
                    int courseId;
                    if (!winScanf(hwnd, "����Ҫ��ѡ�Ŀγ�ID��", "%d", &courseId)) break;
                    if (deleteUserCourseRel(courseId)) {
                        successInfoForWindow("��ѡ�γ̳ɹ���", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѡ�γ�ʧ��", hwnd);
                    }
                    break;
                }

                // ��������
                case IDC_MENU_USER_SEARCH_SHOWCOURSE: {
                    if (showCourseInfoForUser()) {
                        successInfoForWindow("����ʾ���пγ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_SEARCH_SEARCHCOURSE: {
                    char* courseName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* courseType = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* location = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int stuMaxLow = -1, stuMaxHigh = -1;
                    double priceLow = -1, priceHigh = -1;
                    int timeLow = -1, timeHigh = -1;

                    if (!courseName || !courseType || !location) {
                        errorInfoForWindow("�ڴ治��", hwnd);
                        free(courseName);
                        free(courseType);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "�γ����ƣ�-��ɸѡ����", "%s", courseName) ||
                        !winScanf(hwnd, "�γ����ͣ�-��ɸѡ����", "%s", courseType) ||
                        !winScanf(hwnd, "�ص㣨-��ɸѡ����", "%s", location) ||
                        !winScanf(hwnd, "ѧ���������ޣ�-1�����ƣ���", "%d", &stuMaxLow) ||
                        !winScanf(hwnd, "ѧ���������ޣ�-1�����ƣ���", "%d", &stuMaxHigh) ||
                        !winScanf(hwnd, "�۸����ޣ�-1�����ƣ���", "%lf", &priceLow) ||
                        !winScanf(hwnd, "�۸����ޣ�-1�����ƣ���", "%lf", &priceHigh) ||
                        !winScanf(hwnd, "ʱ�����ޣ�-1�����ƣ���", "%d", &timeLow) ||
                        !winScanf(hwnd, "ʱ�����ޣ�-1�����ƣ���", "%d", &timeHigh)) {
                        break;
                    }

                    // �����������
                    if (strcmp(courseName, "-") == 0) {
                        free(courseName);
                        courseName = NULL;
                    }
                    if (strcmp(courseType, "-") == 0) {
                        free(courseType);
                        courseType = NULL;
                    }
                    if (strcmp(location, "-") == 0) {
                        free(location);
                        location = NULL;
                    }

                    bool res = searchCourseInfoForUser(courseName, courseType, location,
                        stuMaxLow, stuMaxHigh, priceLow, priceHigh, timeLow, timeHigh);
                    if (res) {
                        successInfoForWindow("��ѯ�ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("��ѯʧ��", hwnd);
                    }

                    if (courseName) free(courseName);
                    if (courseType) free(courseType);
                    if (location) free(location);
                    break;
                }

                // ��������
                case IDC_MENU_USER_SORT_COURSE: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "˵��", MB_ICONINFORMATION);
                    int byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice;
                    if (!winScanf(hwnd, "�������������ȼ�������", "%d", &byName) ||
                        !winScanf(hwnd, "������ID�������ȼ�������", "%d", &byTypeId) ||
                        !winScanf(hwnd, "��ʱ���������ȼ�������", "%d", &byTime) ||
                        !winScanf(hwnd, "���ص��������ȼ�������", "%d", &byLocation) ||
                        !winScanf(hwnd, "��ѧ�����������ȼ�������", "%d", &byStuCount) ||
                        !winScanf(hwnd, "�����ѧ�����������ȼ�������", "%d", &byStuMax) ||
                        !winScanf(hwnd, "���������������ȼ�������", "%d", &byCoachNum) ||
                        !winScanf(hwnd, "���۸��������ȼ�������", "%d", &byPrice)) break;

                    if (sortCourseInfoForUser(byName, byTypeId, byTime, byLocation,
                        byStuCount, byStuMax, byCoachNum, byPrice)) {
                        successInfoForWindow("����ɹ�", hwnd);
                    }
                    else {
                        errorInfoForWindow("����ʧ��", hwnd);
                    }
                    break;
                }

                // ����ͳ��
                case IDC_MENU_USER_COUNT_COURSETYPE: {
                    if (countCourseTypeSelectedForUser()) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COUNT_COURSETYPEBYID: {
                    int typeId;
                    if (!winScanf(hwnd, "����γ�����ID��", "%d", &typeId)) break;
                    if (countCourseTypeSelectedByIdForUser(typeId)) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COUNT_COACH: {
                    if (countCoachCourseForUser()) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COUNT_COACHBYID: {
                    int coachId;
                    if (!winScanf(hwnd, "�������ID��", "%d", &coachId)) break;
                    if (countCoachCourseByIdForUser(coachId)) {
                        successInfoForWindow("ͳ�����", hwnd);
                    }
                    else {
                        errorInfoForWindow("ͳ��ʧ��", hwnd);
                    }
                    break;
                }
                default:
                    return DefWindowProcA(hwnd, msg, wParam, lParam);
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return FALSE;
}

// ��ʾѧԱ����̨
int WINAPI showUserDashboard() {
    return registerAndCreateWindow(USERWINDOW_CLASS_NAME, "�������ϵͳ | ѧԱ����̨", procUserDashboardWindow, 200, 200, 700, 500, NULL);
}