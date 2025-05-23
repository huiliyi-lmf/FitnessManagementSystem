#define _CRT_SECURE_NO_WARNINGS
#include "gui.h"

HFONT hFont = INVALID_HANDLE_VALUE;

#define STR_BUFFER_MAXLENGTH 2048

const char* MESSAGE_SORT_INFORMATION = " * 排序规则：\r\n * 每个关键字的值为正数时则说明为升序排序，为负数时则为降序排序\r\n * 如果关键字的值为0则不参与排序，关键字的绝对值越小，则说明优先度越高\r\n * 两个值非零的关键字的绝对值不应该相同";

// 初始化字体
void initFont() {
    hFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
}

// 获得编辑框的输入
char* getEditText(HWND hwnd, int idc) {
    int szStr = GetWindowTextLengthA(GetDlgItem(hwnd, idc));
    char* str = (char*)malloc((szStr + 5) * sizeof(char));
    if (str == NULL) {
        errorInfoForWindow("操作系统内存不足", hwnd);
        return NULL;
    }
    memset(str, 0, (szStr + 5) * sizeof(char));
    GetWindowTextA(GetDlgItem(hwnd, idc), str, szStr + 5);
    return str;
}

#define MyCreateWindow(a, b, c, d, e, f, g, h, i, j, k, l) \
SendMessageA(CreateWindowExA(a, b, c, d, e, f, g, h, i, j, k, l), WM_SETFONT, (WPARAM)hFont, TRUE)

// 注册窗口类并创建窗口
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
        errorInfoForWindow("创建窗口失败！", NULL);
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
        MyCreateWindow(NULL, "BUTTON", "确定输入", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 445, 60, 100, 30, hwnd, (HMENU)IDC_BTN_WINSCANF, NULL, NULL);
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
                errorInfoForWindow("操作系统内存不足！", hwnd);
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

// 图形化界面的输入
bool winScanf(HWND hParent, const char* title, const char* format, void* p) {
    while (1) {
        registerAndCreateWindow(SCANFWINDOW_CLASS_NAME, title, procWinScanf, 200, 200, 600, 150, hParent);

        // 判断用户是输入了数据还是取消
        if (!WINSCANF_FLAG) {
            break;
        }

        // 窗口已经结束，用户已经输入了数据
        if (sscanf(WINSCANF_BUFFER, format, p) != 1) {
            warnInfoForWindow("请输入合法的数据！", NULL);
        } else {
            EnableWindow(hParent, TRUE);
            return true;
        }
    }
    EnableWindow(hParent, TRUE);
    return false;
}

bool loginWindowStatus = true;  // 为true时代表登录界面，false为注册界面
// 登录窗口回调函数
LRESULT CALLBACK procLoginWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
            // 创建控件
            MyCreateWindow(NULL, "STATIC", "手机号:", WS_CHILD | WS_VISIBLE, 20, 20, 80, 25, hwnd, NULL, NULL, NULL);
            MyCreateWindow(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 20, 200, 25, hwnd, (HMENU)IDC_EDIT_PHONE, NULL, NULL);
            MyCreateWindow(NULL, "STATIC", "用户名:", WS_CHILD | WS_VISIBLE, 20, 60, 80, 25, hwnd, NULL, NULL, NULL);
            MyCreateWindow(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 60, 200, 25, hwnd, (HMENU)IDC_EDIT_USERNAME, NULL, NULL);
            MyCreateWindow(NULL, "STATIC", "密码:", WS_CHILD | WS_VISIBLE, 20, 100, 80, 25, hwnd, NULL, NULL, NULL);
            MyCreateWindow(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, 100, 100, 200, 25, hwnd, (HMENU)IDC_EDIT_PASSWORD, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "登录", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 20, 160, 100, 30, hwnd, (HMENU)IDC_BTN_LOGIN, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "注册", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 20, 160, 100, 30, hwnd, (HMENU)IDC_BTN_REGISTER, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "切换到注册", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 200, 160, 100, 30, hwnd, (HMENU)IDC_BTN_SWITCH_LOGIN_REGISTER, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "管理员", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 20, 128, 100, 30, hwnd, (HMENU)IDC_RADIO_LOGIN_ADMIN, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "教练", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 128, 100, 30, hwnd, (HMENU)IDC_RADIO_LOGIN_COACH, NULL, NULL);
            MyCreateWindow(NULL, "BUTTON", "学员", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 220, 128, 100, 30, hwnd, (HMENU)IDC_RADIO_LOGIN_USER, NULL, NULL);
            // 初始化控件的状态
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
                    // 检测所选的用户状态
                    if (result1 == BST_CHECKED) {
                        if (loginForAdmin(userName, password)) {
                            successInfoForWindow("登录成功！", hwnd);
                            ShowWindow(hwnd, SW_HIDE);
                            // 打开管理员页面
                            showAdminDashboard();
                            clearEditHandle();
                            ShowWindow(hwnd, SW_SHOW);
                        } else {
                            errorInfoForWindow("登录失败！", hwnd);
                        }
                    } else if (result2 == BST_CHECKED) {
                        if (loginForCoach(userName, password)) {
                            successInfoForWindow("登录成功！", hwnd);
                            ShowWindow(hwnd, SW_HIDE);
                            showCoachDashboard();
                            clearEditHandle();
                            ShowWindow(hwnd, SW_SHOW);
                        } else {
                            errorInfoForWindow("登录失败！", hwnd);
                        }
                    } else if (result3 == BST_CHECKED) {
                        if (loginForUser(userName, password, phone)) {
                            successInfoForWindow("登录成功！", hwnd);
                            ShowWindow(hwnd, SW_HIDE);
                            showUserDashboard();
                            clearEditHandle();
                            ShowWindow(hwnd, SW_SHOW);
                        } else {
                            errorInfoForWindow("登录失败！", hwnd);
                        }
                    } else {
                        warnInfoForWindow("请至少选择一个登录身份！", hwnd);
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
                    // 检测所选状态
                    if (result1 == BST_CHECKED) {
                        if (registerForAdmin(userName, password)) {
                            successInfoForWindow("注册管理员成功！", hwnd);
                        } else {
                            errorInfoForWindow("注册管理员失败！", hwnd);
                        }
                    } else if (result2 == BST_CHECKED) {
                        // 并没有要求教练手动注册
                    } else if (result3 == BST_CHECKED) {
                        if (registerForUser(userName, password, phone)) {
                            successInfoForWindow("注册学员成功！", hwnd);
                        } else {
                            errorInfoForWindow("注册学员失败！", hwnd);
                        }
                    } else {
                        warnInfoForWindow("请至少选择一个注册身份！", hwnd);
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
                        // 切换到注册状态
                        ShowWindow(GetDlgItem(hwnd, IDC_RADIO_LOGIN_COACH), SW_HIDE);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_REGISTER), SW_SHOW);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_LOGIN), SW_HIDE);
                        SetWindowTextA(hwnd, "健身房管理系统 | 用户注册");
                        SetWindowTextA(GetDlgItem(hwnd, IDC_BTN_SWITCH_LOGIN_REGISTER), "切换到登录");
                    } else {
                        // 切换到登录状态
                        ShowWindow(GetDlgItem(hwnd, IDC_RADIO_LOGIN_COACH), SW_SHOW);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_REGISTER), SW_HIDE);
                        ShowWindow(GetDlgItem(hwnd, IDC_BTN_LOGIN), SW_SHOW);
                        SetWindowTextA(hwnd, "健身房管理系统 | 用户登录");
                        SetWindowTextA(GetDlgItem(hwnd, IDC_BTN_SWITCH_LOGIN_REGISTER), "切换到注册");
                    }
                    // 恢复控件的状态
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
* 显示登录页面
*/
int WINAPI showLoginWindow() {
    return registerAndCreateWindow(LOGINWINDOW_CLASS_NAME, "健身管理系统 | 用户登录", procLoginWindow, 200, 200, 350, 250, NULL);
}

LRESULT CALLBACK procAdminDashboardWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // 初始编辑框
            HWND hEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", 
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | ES_READONLY,
                10, 10, 650, 450, hwnd, (HMENU)IDC_EDIT_PRINTF, GetModuleHandleA(NULL), NULL);
            SendMessageA(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            setEditHandle(hEdit);

            // 创建菜单结构
            HMENU hMenu = CreateMenu();
            HMENU hSysMenu = CreatePopupMenu();
            HMENU hCourseTypeMenu = CreatePopupMenu();
            HMENU hCoachMenu = CreatePopupMenu();
            HMENU hCourseMenu = CreatePopupMenu();
            HMENU hCoachCourseRelMenu = CreatePopupMenu();
            HMENU hSearchMenu = CreatePopupMenu();
            HMENU hSortMenu = CreatePopupMenu();
            HMENU hCountMenu = CreatePopupMenu();

            // 系统管理菜单
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_LOGOUT, "退出登录");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_CHANGEPWD, "修改本账号密码");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_CHANGECOACHPWD, "修改教练密码");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_DATABACKUP, "数据备份");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_ADMIN_DATARESTORE, "数据恢复");

            // 课程类型管理子菜单
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_CREATE, "创建课程类型");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_UPDATE, "修改课程类型");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_DELETE, "删除课程类型");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_ADMIN_COURSETYPE_SHOW, "显示所有课程类别");

            // 教练管理子菜单
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_CREATE, "添加教练");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_UPDATE, "修改教练");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_DELETE, "删除教练");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_SHOWJUDGE, "打印待审核教练信息");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_MARKJUDGE, "通过教练信息审核");
            AppendMenuA(hCoachMenu, MF_STRING, IDC_MENU_ADMIN_COACH_SHOWALL, "打印所有教练信息");

            // 课程管理子菜单
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_ADMIN_COURSE_CREATE, "创建课程");
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_ADMIN_COURSE_UPDATE, "修改课程");
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_ADMIN_COURSE_DELETE, "删除课程");

            // 教练课程关系子菜单
            AppendMenuA(hCoachCourseRelMenu, MF_STRING, IDC_MENU_ADMIN_COACHCOURSEREL_ADD, "关联教练课程");
            AppendMenuA(hCoachCourseRelMenu, MF_STRING, IDC_MENU_ADMIN_COACHCOURSEREL_DELETE, "解除教练课程关联");

            // 搜索管理子菜单
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SHOWCOURSE, "显示所有课程");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SEARCHCOURSE, "课程查询");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SHOWUSER, "显示所有用户");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_ADMIN_SEARCH_SEARCHUSER, "用户查询");

            // 排序管理子菜单
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_ADMIN_SORT_COURSE, "课程排序");
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_ADMIN_SORT_USER, "用户排序");

            // 统计管理子菜单
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COURSE, "所有类型课程学员数量统计");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COURSEBYID, "某个类型课程学员数量统计");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COACH, "所有教练负责的健身课程门数");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_COACHBYID, "某个教练负责的健身课程门数");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_USER, "所有学员参加的健身课程的门数");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_ADMIN_COUNT_USERBYID, "某个学员参加的健身课程的门数");

            // 将子菜单挂载到主菜单栏
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseTypeMenu, "课程类型管理");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCoachMenu, "教练管理");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseMenu, "课程管理");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCoachCourseRelMenu, "教学关系管理");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSearchMenu, "信息查询");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSortMenu, "排序功能");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCountMenu, "统计分析");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSysMenu, "系统管理");
            SetMenu(hwnd, hMenu);

            break;
        }
        case WM_COMMAND: {
            if (HIWORD(wParam) != BN_CLICKED) {
                break;
            }
            // 先清空控制台的输出
            clearEditText();
            switch (LOWORD(wParam)) {
                case IDC_MENU_ADMIN_LOGOUT: {
                    // 退出登录逻辑
                    if (MessageBoxA(hwnd, "是否要退出登录？", "询问", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        DestroyWindow(hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_CHANGEPWD: {
                    // 修改本账号密码逻辑
                    char* newPassword = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (newPassword == NULL) {
                        errorInfoForWindow("操作系统内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "请输入新的密码：", "%s", newPassword)) {
                        free(newPassword);
                        break;
                    }
                    if (changePasswordForAdmin(newPassword)) {
                        successInfoForWindow("密码修改成功！", hwnd);
                    } else {
                        warnInfoForWindow("密码修改失败", hwnd);
                    }
                    free(newPassword);
                    break;
                }
                case IDC_MENU_ADMIN_CHANGECOACHPWD: {
                    // 修改教练密码逻辑
                    int coachId;
                    if (!winScanf(hwnd, "请输入教练ID：", "%d", &coachId)) {
                        break;
                    }
                    char* newPassword = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (newPassword == NULL) {
                        errorInfoForWindow("os内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "请输入教练的新密码：", "%s", newPassword)) {
                        free(newPassword);
                        break;
                    }
                    if (changeCoachPasswordForAdmin(coachId, newPassword)) {
                        successInfoForWindow("修改教练密码成功！", hwnd);
                    } else {
                        errorInfoForWindow("修改教练密码失败！", hwnd);
                    }
                    free(newPassword);
                    break;
                }
                case IDC_MENU_ADMIN_DATABACKUP: {
                    // 数据备份逻辑
                    char* filePath = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (filePath == NULL) {
                        errorInfoForWindow("os内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "请输入备份文件名：", "%s", filePath)) {
                        free(filePath);
                        break;
                    }
                    if (databaseBackup(filePath)) {
                        successInfoForWindow("数据备份成功！", hwnd);
                    } else {
                        errorInfoForWindow("数据备份失败！", hwnd);
                    }
                    free(filePath);
                    break;
                }
                case IDC_MENU_ADMIN_DATARESTORE: {
                    // 数据恢复逻辑
                    char* filePath = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (filePath == NULL) {
                        errorInfoForWindow("os内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "请输入恢复备份文件名：", "%s", filePath)) {
                        free(filePath);
                        break;
                    }
                    if (databaseRestore(filePath)) {
                        successInfoForWindow("数据恢复成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("数据恢复失败！", hwnd);
                    }
                    free(filePath);
                    break;
                }

                // 课程类型管理
                case IDC_MENU_ADMIN_COURSETYPE_CREATE: {
                    char* typeName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!typeName) {
                        errorInfoForWindow("内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "请输入课程类型名称：", "%s", typeName)) {
                        free(typeName);
                        break;
                    }
                    if (createCourseType(typeName)) {
                        successInfoForWindow("创建成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("创建失败", hwnd);
                    }
                    free(typeName);
                    break;
                }
                case IDC_MENU_ADMIN_COURSETYPE_UPDATE: {
                    int id;
                    if (!winScanf(hwnd, "请输入课程类型ID：", "%d", &id)) break;
                    char* newName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!newName) {
                        errorInfoForWindow("内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "请输入新名称：", "%s", newName)) {
                        free(newName);
                        break;
                    }
                    if (updateCourseType(id, newName)) {
                        successInfoForWindow("修改成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("修改失败", hwnd);
                    }
                    free(newName);
                    break;
                }
                case IDC_MENU_ADMIN_COURSETYPE_DELETE: {
                    int id;
                    if (!winScanf(hwnd, "请输入要删除的课程类型ID：", "%d", &id)) break;
                    if (deleteCourseType(id)) {
                        successInfoForWindow("删除成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("删除失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COURSETYPE_SHOW: {
                    if (showAllCourseTypeInfo()) {
                        successInfoForWindow("查询成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败！", hwnd);
                    }
                    break;
                }

                // 教练管理
                case IDC_MENU_ADMIN_COACH_CREATE: {
                    char* name = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* pass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!name || !pass) {
                        free(name);
                        free(pass);
                        errorInfoForWindow("内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "输入教练姓名：", "%s", name) ||
                        !winScanf(hwnd, "输入教练密码：", "%s", pass)) {
                        free(name);
                        free(pass);
                        break;
                    }
                    if (createCoach(name, pass)) {
                        successInfoForWindow("创建成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("创建失败", hwnd);
                    }
                    free(name);
                    free(pass);
                    break;
                }
                case IDC_MENU_ADMIN_COACH_UPDATE: {
                    int id;
                    if (!winScanf(hwnd, "输入教练ID：", "%d", &id)) break;

                    char* newName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    char* newPass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int newLevel = -1;
                    if (!newName || !newPass) {
                        errorInfoForWindow("内存不足", hwnd);
                        free(newName);
                        free(newPass);
                        break;
                    }

                    if (!winScanf(hwnd, "输入新姓名（-不修改）：", "%s", newName)) {
                        free(newName);
                        free(newPass);
                        break;
                    }
                    if (!winScanf(hwnd, "输入新密码（-不修改）：", "%s", newPass)) {
                        break;
                    }
                    // 处理姓名
                    if (strcmp(newName, "-") == 0) {
                        free(newName);
                        newName = NULL;
                    }
                    // 处理密码
                    if (strcmp(newPass, "-") == 0) {
                        free(newPass);
                        newPass = NULL;
                    }
                    // 处理等级
                    winScanf(hwnd, "输入新等级（-1不修改）：", "%d", &newLevel);

                    if (updateCoach(id, newName, newPass, (CoachLevel)newLevel)) {
                        successInfoForWindow("更新成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("更新失败", hwnd);
                    }

                    if (newName) free(newName);
                    if (newPass) free(newPass);
                    break;
                }
                case IDC_MENU_ADMIN_COACH_DELETE: {
                    int id;
                    if (!winScanf(hwnd, "输入要删除的教练ID：", "%d", &id)) break;
                    if (deleteCoach(id)) {
                        successInfoForWindow("删除成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("删除失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACH_SHOWJUDGE: {
                    if (showNeedJudgeCoachInfo()) {
                        successInfoForWindow("已显示待审核信息", hwnd);
                    }
                    else {
                        errorInfoForWindow("获取失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACH_MARKJUDGE: {
                    int id, status;
                    if (!winScanf(hwnd, "输入教练ID：", "%d", &id) ||
                        !winScanf(hwnd, "输入审核状态（0拒绝，1通过）：", "%d", &status)) break;
                    if (markJudgeCoachInfoStatus(id, (CoachStatus)status)) {
                        successInfoForWindow("标记成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("标记失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACH_SHOWALL: {
                    if (showAllCoachInfo()) {
                        successInfoForWindow("查询成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }
                    break;
                }

                // 课程管理
                case IDC_MENU_ADMIN_COURSE_CREATE: {
                    char* name = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int type, time, stuMax;
                    char* location = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    double price;

                    if (!name || !location) {
                        errorInfoForWindow("内存不足", hwnd);
                        free(name);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "课程名称：", "%s", name) ||
                        !winScanf(hwnd, "课程类型ID：", "%d", &type) ||
                        !winScanf(hwnd, "时间（整数）：", "%d", &time) ||
                        !winScanf(hwnd, "地点：", "%s", location) ||
                        !winScanf(hwnd, "最大学生数：", "%d", &stuMax) ||
                        !winScanf(hwnd, "价格：", "%lf", &price)) {
                        free(name);
                        free(location);
                        break;
                    }

                    if (createCourse(name, type, time, location, stuMax, price)) {
                        successInfoForWindow("创建成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("创建失败", hwnd);
                    }

                    free(name);
                    free(location);
                    break;
                }
                case IDC_MENU_ADMIN_COURSE_UPDATE: {
                    int id;
                    if (!winScanf(hwnd, "输入课程ID：", "%d", &id)) break;

                    char* name = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    int type, time, stuMax;
                    char* location = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    double price;

                    if (!name || !location) {
                        errorInfoForWindow("内存不足", hwnd);
                        free(name);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "新课程名称：", "%s", name) ||
                        !winScanf(hwnd, "新课程类型ID：", "%d", &type) ||
                        !winScanf(hwnd, "新时间：", "%d", &time) ||
                        !winScanf(hwnd, "新地点：", "%s", location) ||
                        !winScanf(hwnd, "新最大学生数：", "%d", &stuMax) ||
                        !winScanf(hwnd, "新价格：", "%lf", &price)) {
                        free(name);
                        free(location);
                        break;
                    }

                    if (updateCourse(id, name, type, time, location, stuMax, price)) {
                        successInfoForWindow("更新成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("更新失败", hwnd);
                    }

                    free(name);
                    free(location);
                    break;
                }
                case IDC_MENU_ADMIN_COURSE_DELETE: {
                    int id;
                    if (!winScanf(hwnd, "输入要删除的课程ID：", "%d", &id)) break;
                    if (deleteByIdForCourse(id)) {
                        successInfoForWindow("删除成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("删除失败", hwnd);
                    }
                    break;
                }

                // 教练课程关系
                case IDC_MENU_ADMIN_COACHCOURSEREL_ADD: {
                    int courseId, coachId;
                    if (!winScanf(hwnd, "输入课程ID：", "%d", &courseId) ||
                        !winScanf(hwnd, "输入教练ID：", "%d", &coachId)) break;
                    if (addCoachCourseRel(courseId, coachId)) {
                        successInfoForWindow("关联成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("关联失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COACHCOURSEREL_DELETE: {
                    int courseId, coachId;
                    if (!winScanf(hwnd, "输入课程ID：", "%d", &courseId) ||
                        !winScanf(hwnd, "输入教练ID：", "%d", &coachId)) break;
                    if (deleteCoachCourseRel(courseId, coachId)) {
                        successInfoForWindow("解除成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("解除失败", hwnd);
                    }
                    break;
                }

                // 信息查询
                case IDC_MENU_ADMIN_SEARCH_SHOWCOURSE: {
                    if (showCourseInfoForAdmin()) {
                        successInfoForWindow("已显示所有课程", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
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
                        errorInfoForWindow("内存不足", hwnd);
                        free(courseName);
                        free(courseType);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "课程名称（-不筛选）：", "%s", courseName)
                        || !winScanf(hwnd, "课程类型（-不筛选）：", "%s", courseType)
                        || !winScanf(hwnd, "地点（-不筛选）：", "%s", location)
                        || !winScanf(hwnd, "学生数量下限（-1不限制）：", "%d", &stuMaxLow)
                        || !winScanf(hwnd, "学生数量上限（-1不限制）：", "%d", &stuMaxHigh)
                        || !winScanf(hwnd, "价格下限（-1不限制）：", "%lf", &priceLow)
                        || !winScanf(hwnd, "价格上限（-1不限制）：", "%lf", &priceHigh)
                        || !winScanf(hwnd, "时间下限（-1不限制）：", "%d", &timeLow)
                        || !winScanf(hwnd, "时间上限（-1不限制）：", "%d", &timeHigh)) {
                        break;
                    }

                    // 处理输入
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
                        successInfoForWindow("查询成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }

                    if (courseName) free(courseName);
                    if (courseType) free(courseType);
                    if (location) free(location);
                    break;
                }
                case IDC_MENU_ADMIN_SEARCH_SHOWUSER: {
                    if (showUserInfoForAdmin()) {
                        successInfoForWindow("已显示所有学员", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_SEARCH_SEARCHUSER: {
                    char* userName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!userName) {
                        errorInfoForWindow("内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "输入学员用户名（-不筛选）：", "%s", userName)) {
                        break;
                    }
                    if (strcmp(userName, "-") == 0) {
                        free(userName);
                        userName = NULL;
                    }
                    bool res = searchUserInfoForAdmin(userName);
                    if (res) {
                        successInfoForWindow("查询成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }
                    free(userName);
                    break;
                }

                // 排序功能
                case IDC_MENU_ADMIN_SORT_COURSE: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "说明", MB_ICONINFORMATION);
                    int byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice;
                    if (!winScanf(hwnd, "按名称排序（优先级±）：", "%d", &byName) ||
                        !winScanf(hwnd, "按类型ID排序（优先级±）：", "%d", &byTypeId) ||
                        !winScanf(hwnd, "按时间排序（优先级±）：", "%d", &byTime) ||
                        !winScanf(hwnd, "按地点排序（优先级±）：", "%d", &byLocation) ||
                        !winScanf(hwnd, "按学生数排序（优先级±）：", "%d", &byStuCount) ||
                        !winScanf(hwnd, "按最大学生数排序（优先级±）：", "%d", &byStuMax) ||
                        !winScanf(hwnd, "按教练数排序（优先级±）：", "%d", &byCoachNum) ||
                        !winScanf(hwnd, "按价格排序（优先级±）：", "%d", &byPrice)) break;

                    if (sortCourseInfoForAdmin(byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice)) {
                        successInfoForWindow("排序成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("排序失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_SORT_USER: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "说明", MB_ICONINFORMATION);
                    int byName, bySelectCourse;
                    if (!winScanf(hwnd, "按名称排序（优先级±）：", "%d", &byName) ||
                        !winScanf(hwnd, "按课程数排序（优先级±）：", "%d", &bySelectCourse)) break;
                    if (sortUserInfoForAdmin(byName, bySelectCourse)) {
                        successInfoForWindow("排序成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("排序失败", hwnd);
                    }
                    break;
                }

                // 统计分析
                case IDC_MENU_ADMIN_COUNT_COURSE: {
                    if (countCourseTypeSelectedForAdmin()) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_COACH: {
                    if (countCoachCourseForAdmin()) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_USER: {
                    if (countUserCourseForAdmin()) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_COURSEBYID: {
                    int id;
                    if (!winScanf(hwnd, "请输入课程类型ID：", "%d", &id)) {
                        break;
                    }
                    if (countCourseTypeSelectedByIdForAdmin(id)) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_COACHBYID: {
                    int id;
                    if (!winScanf(hwnd, "请输入教练ID：", "%d", &id)) {
                        break;
                    }
                    if (countCoachCourseByIdForAdmin(id)) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_ADMIN_COUNT_USERBYID: {
                    int id;
                    if (!winScanf(hwnd, "请输入学员ID：", "%d", &id)) {
                        break;
                    }
                    if (countUserCourseByIdForAdmin(id)) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
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

// 显示管理员控制台
int WINAPI showAdminDashboard() {
    return registerAndCreateWindow(ADMINWINDOW_CLASS_NAME, "健身管理系统 | 管理员控制台", procAdminDashboardWindow, 200, 200, 700, 500, NULL);
}

LRESULT CALLBACK procCoachDashboardWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // 初始编辑框
            HWND hEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | ES_READONLY,
                10, 10, 650, 450, hwnd, (HMENU)IDC_EDIT_PRINTF, GetModuleHandleA(NULL), NULL);
            SendMessageA(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            setEditHandle(hEdit);

            // 创建菜单
            HMENU hMenu = CreateMenu();
            HMENU hSysMenu = CreatePopupMenu();
            HMENU hCourseTypeMenu = CreatePopupMenu();
            HMENU hSearchMenu = CreatePopupMenu();
            HMENU hSortMenu = CreatePopupMenu();
            HMENU hCountMenu = CreatePopupMenu();

            // 创建主菜单项并关联子菜单
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseTypeMenu, "课程类别管理");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSearchMenu, "数据查询");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSortMenu, "数据排序");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCountMenu, "数据统计");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSysMenu, "系统维护");

            // 填充课程类别管理子菜单
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_COACH_COURSETYPE_ADD, "添加擅长的课程类型");
            AppendMenuA(hCourseTypeMenu, MF_STRING, IDC_MENU_COACH_COURSETYPE_DELETE, "删除擅长的课程类型");

            // 填充数据查询子菜单
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_COURSE_SHOW, "打印所有参加的课程");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_COURSE_SEARCH, "查询参加的课程");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_USER_SHOW, "打印所有上课的学员");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_COACH_SEARCH_USER_SEARCH, "查询上课的学员");

            // 填充数据排序子菜单
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_COACH_SORT_COURSE, "所有参加的课程排序");
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_COACH_SORT_USER, "所有上课的学员排序");

            // 填充数据统计子菜单
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_COURSETYPE, "统计当前教练所有类型课程的学员数量");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_COURSETYPEBYID, "统计当前教练某个类型课程的学员数量");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_USERCOURSE, "统计当前教练所有学员参加的健身课程的门数");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_COACH_COUNT_USERCOURSEBYID, "统计当前教练某个学员参加的健身课程的门数");

            // 填充系统维护子菜单
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_COACH_SYS_CHANGEPWD, "修改密码");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_COACH_SYS_LOGOUT, "退出登录");

            SetMenu(hwnd, hMenu);
            break;
        }
        case WM_COMMAND: {
            if (HIWORD(wParam) != BN_CLICKED) {
                break;
            }
            clearEditText();
            switch (LOWORD(wParam)) {
                //-- 课程类别管理 --
                case IDC_MENU_COACH_COURSETYPE_ADD: {
                    int typeId;
                    if (!winScanf(hwnd, "请输入擅长类型ID：", "%d", &typeId)) {
                        break;
                    }
                    if (addNewCoachTypeRel(typeId)) {
                        successInfoForWindow("添加成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("添加失败！", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COURSETYPE_DELETE: {
                    int typeId;
                    if (!winScanf(hwnd, "请输入要删除的擅长类型ID：", "%d", &typeId)) break;
                    if (removeCoachTypeRel(typeId)) {
                        successInfoForWindow("删除成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("删除失败！", hwnd);
                    }
                    break;
                }

                //-- 数据查询 --
                case IDC_MENU_COACH_SEARCH_COURSE_SHOW: {
                    if (showCourseInfoForCoach()) {
                        successInfoForWindow("已显示所有课程", hwnd);
                    }
                    else {
                        errorInfoForWindow("获取失败", hwnd);
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
                        errorInfoForWindow("内存不足", hwnd);
                        free(courseName);
                        free(courseType);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "课程名称（-不筛选）：", "%s", courseName) ||
                        !winScanf(hwnd, "课程类型（-不筛选）：", "%s", courseType) ||
                        !winScanf(hwnd, "地点（-不筛选）：", "%s", location) ||
                        !winScanf(hwnd, "学生数量下限（-1不限制）：", "%d", &stuMaxLow) ||
                        !winScanf(hwnd, "学生数量上限（-1不限制）：", "%d", &stuMaxHigh) ||
                        !winScanf(hwnd, "价格下限（-1不限制）：", "%lf", &priceLow) ||
                        !winScanf(hwnd, "价格上限（-1不限制）：", "%lf", &priceHigh) ||
                        !winScanf(hwnd, "时间下限（-1不限制）：", "%d", &timeLow) ||
                        !winScanf(hwnd, "时间上限（-1不限制）：", "%d", &timeHigh)) {
                        break;
                    }

                    // 处理输入参数
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
                        successInfoForWindow("查询成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }

                    if (courseName) free(courseName);
                    if (courseType) free(courseType);
                    if (location) free(location);
                    break;
                }
                case IDC_MENU_COACH_SEARCH_USER_SHOW: {
                    if (showUserInfoForCoach()) {
                        successInfoForWindow("已显示所有学员", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_SEARCH_USER_SEARCH: {
                    char* userName = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!userName) {
                        errorInfoForWindow("内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "输入学员用户名：", "%s", userName)) {
                        break;
                    }
                    bool res = searchUserInfoForCoach(userName);
                    if (res) {
                        successInfoForWindow("查询成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }
                    free(userName);
                    break;
                }

                //-- 数据排序 --
                case IDC_MENU_COACH_SORT_COURSE: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "说明", MB_ICONINFORMATION);
                    int byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice;
                    if (!winScanf(hwnd, "按名称排序（优先级±）：", "%d", &byName) ||
                        !winScanf(hwnd, "按类型ID排序（优先级±）：", "%d", &byTypeId) ||
                        !winScanf(hwnd, "按时间排序（优先级±）：", "%d", &byTime) ||
                        !winScanf(hwnd, "按地点排序（优先级±）：", "%d", &byLocation) ||
                        !winScanf(hwnd, "按学生数排序（优先级±）：", "%d", &byStuCount) ||
                        !winScanf(hwnd, "按最大学生数排序（优先级±）：", "%d", &byStuMax) ||
                        !winScanf(hwnd, "按教练数排序（优先级±）：", "%d", &byCoachNum) ||
                        !winScanf(hwnd, "按价格排序（优先级±）：", "%d", &byPrice)) break;

                    if (sortCourseInfoForCoach(byName, byTypeId, byTime, byLocation,
                        byStuCount, byStuMax, byCoachNum, byPrice)) {
                        successInfoForWindow("排序成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("排序失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_SORT_USER: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "说明", MB_ICONINFORMATION);
                    int byName, bySelectCourse;
                    if (!winScanf(hwnd, "按名称排序（优先级±）：", "%d", &byName) ||
                        !winScanf(hwnd, "按课程数排序（优先级±）：", "%d", &bySelectCourse)) break;
                    if (sortUserInfoForCoach(byName, bySelectCourse)) {
                        successInfoForWindow("排序成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("排序失败", hwnd);
                    }
                    break;
                }

                //-- 数据统计 --
                case IDC_MENU_COACH_COUNT_COURSETYPE: {
                    if (countCourseTypeSelectedForCoach()) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COUNT_COURSETYPEBYID: {
                    int typeId;
                    if (!winScanf(hwnd, "输入课程类型ID：", "%d", &typeId)) break;
                    if (countCourseTypeSelectedByIdForCoach(typeId)) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COUNT_USERCOURSE: {
                    if (countUserCourseForCoach()) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_COACH_COUNT_USERCOURSEBYID: {
                    int userId;
                    if (!winScanf(hwnd, "输入学员ID：", "%d", &userId)) break;
                    if (countUserCourseByIdForCoach(userId)) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }

                //-- 系统维护 --
                case IDC_MENU_COACH_SYS_CHANGEPWD: {
                    char* newPass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!newPass) {
                        errorInfoForWindow("内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "输入新密码：", "%s", newPass)) {
                        free(newPass);
                        break;
                    }
                    if (changePasswordForCoach(newPass)) {
                        successInfoForWindow("密码修改成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("密码修改失败", hwnd);
                    }
                    free(newPass);
                    break;
                }
                case IDC_MENU_COACH_SYS_LOGOUT: {
                    if (MessageBoxA(hwnd, "是否要退出登录？", "询问", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
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

// 显示教练控制台
int WINAPI showCoachDashboard() {
    return registerAndCreateWindow(COACHWINDOW_CLASS_NAME, "健身管理系统 | 教练控制台", procCoachDashboardWindow, 200, 200, 700, 500, NULL);
}

LRESULT CALLBACK procUserDashboardWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // 设置编辑框
            HWND hEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | ES_READONLY,
                10, 10, 650, 450, hwnd, (HMENU)IDC_EDIT_PRINTF, GetModuleHandleA(NULL), NULL);
            SendMessageA(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            setEditHandle(hEdit);

            // 创建菜单
            HMENU hMenu = CreateMenu();
            HMENU hSysMenu = CreatePopupMenu();
            HMENU hCourseMenu = CreatePopupMenu();
            HMENU hSearchMenu = CreatePopupMenu();
            HMENU hSortMenu = CreatePopupMenu();
            HMENU hCountMenu = CreatePopupMenu();

            // 创建主菜单项
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCourseMenu, "课程操作");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSearchMenu, "数据查询");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSortMenu, "数据排序");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hCountMenu, "数据统计");
            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSysMenu, "系统管理");

            // 系统管理子菜单
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_USER_SYS_CHANGEINFO, "修改个人信息");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_USER_SYS_CHANGEPWD, "修改密码");
            AppendMenuA(hSysMenu, MF_STRING, IDC_MENU_USER_SYS_LOGOUT, "退出登录");

            // 课程操作子菜单
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_USER_COURSE_ADD, "添加课程");
            AppendMenuA(hCourseMenu, MF_STRING, IDC_MENU_USER_COURSE_DELETE, "删除课程");

            // 数据搜索子菜单
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_USER_SEARCH_SHOWCOURSE, "打印当前用户所有课程信息");
            AppendMenuA(hSearchMenu, MF_STRING, IDC_MENU_USER_SEARCH_SEARCHCOURSE, "查询当前用户课程信息");

            // 数据排序子菜单
            AppendMenuA(hSortMenu, MF_STRING, IDC_MENU_USER_SORT_COURSE, "对当前用户课程信息进行排序");

            // 数据统计子菜单
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COURSETYPE, "打印当前用户所有类型课程的学员数量");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COURSETYPEBYID, "打印当前用户某个类型课程的学员数量");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COACH, "打印所有教练负责当前用户的健身课程门数");
            AppendMenuA(hCountMenu, MF_STRING, IDC_MENU_USER_COUNT_COACHBYID, "打印某教练负责的当前用户健身课程门数");

            SetMenu(hwnd, hMenu);

            break;
        }
        case WM_COMMAND: {
            if (HIWORD(wParam) != BN_CLICKED) {
                break;
            }
            clearEditText();
            switch (LOWORD(wParam)) {
                // 系统管理
                case IDC_MENU_USER_SYS_CHANGEINFO: {
                    char* newName = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    char* newPhone = (char*)malloc(sizeof(char) * STR_BUFFER_MAXLENGTH);
                    if (newName == NULL || newPhone == NULL) {
                        errorInfoForWindow("os内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "请输入新的用户名（-不改变）：", "%s", newName) || !winScanf(hwnd, "请输入新的手机号（-不改变）：", "%s", newPhone)) {
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
                        successInfoForWindow("修改用户信息成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("修改用户信息失败！", hwnd);
                    }
                    if (newName) free(newName);
                    if (newPhone)free(newPhone);
                    break;
                }
                case IDC_MENU_USER_SYS_CHANGEPWD: {
                    char* newPass = (char*)malloc(STR_BUFFER_MAXLENGTH);
                    if (!newPass) {
                        errorInfoForWindow("内存不足", hwnd);
                        break;
                    }
                    if (!winScanf(hwnd, "输入新密码：", "%s", newPass)) {
                        free(newPass);
                        break;
                    }
                    if (changePasswordForUser(newPass)) {
                        successInfoForWindow("密码修改成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("密码修改失败", hwnd);
                    }
                    free(newPass);
                    break;
                }
                case IDC_MENU_USER_SYS_LOGOUT: {
                    if (MessageBoxA(hwnd, "是否退出登录？", "询问", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
                        DestroyWindow(hwnd);
                    }
                    break;
                }

                // 课程操作
                case IDC_MENU_USER_COURSE_ADD: {
                    int courseId;
                    if (!winScanf(hwnd, "输入要报名的课程ID：", "%d", &courseId)) break;
                    if (addUserCourseRel(courseId)) {
                        successInfoForWindow("课程报名成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("课程报名失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COURSE_DELETE: {
                    int courseId;
                    if (!winScanf(hwnd, "输入要退选的课程ID：", "%d", &courseId)) break;
                    if (deleteUserCourseRel(courseId)) {
                        successInfoForWindow("退选课程成功！", hwnd);
                    }
                    else {
                        errorInfoForWindow("退选课程失败", hwnd);
                    }
                    break;
                }

                // 数据搜索
                case IDC_MENU_USER_SEARCH_SHOWCOURSE: {
                    if (showCourseInfoForUser()) {
                        successInfoForWindow("已显示所有课程", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
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
                        errorInfoForWindow("内存不足", hwnd);
                        free(courseName);
                        free(courseType);
                        free(location);
                        break;
                    }

                    if (!winScanf(hwnd, "课程名称（-不筛选）：", "%s", courseName) ||
                        !winScanf(hwnd, "课程类型（-不筛选）：", "%s", courseType) ||
                        !winScanf(hwnd, "地点（-不筛选）：", "%s", location) ||
                        !winScanf(hwnd, "学生数量下限（-1不限制）：", "%d", &stuMaxLow) ||
                        !winScanf(hwnd, "学生数量上限（-1不限制）：", "%d", &stuMaxHigh) ||
                        !winScanf(hwnd, "价格下限（-1不限制）：", "%lf", &priceLow) ||
                        !winScanf(hwnd, "价格上限（-1不限制）：", "%lf", &priceHigh) ||
                        !winScanf(hwnd, "时间下限（-1不限制）：", "%d", &timeLow) ||
                        !winScanf(hwnd, "时间上限（-1不限制）：", "%d", &timeHigh)) {
                        break;
                    }

                    // 处理输入参数
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
                        successInfoForWindow("查询成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("查询失败", hwnd);
                    }

                    if (courseName) free(courseName);
                    if (courseType) free(courseType);
                    if (location) free(location);
                    break;
                }

                // 数据排序
                case IDC_MENU_USER_SORT_COURSE: {
                    MessageBoxA(hwnd, MESSAGE_SORT_INFORMATION, "说明", MB_ICONINFORMATION);
                    int byName, byTypeId, byTime, byLocation, byStuCount, byStuMax, byCoachNum, byPrice;
                    if (!winScanf(hwnd, "按名称排序（优先级±）：", "%d", &byName) ||
                        !winScanf(hwnd, "按类型ID排序（优先级±）：", "%d", &byTypeId) ||
                        !winScanf(hwnd, "按时间排序（优先级±）：", "%d", &byTime) ||
                        !winScanf(hwnd, "按地点排序（优先级±）：", "%d", &byLocation) ||
                        !winScanf(hwnd, "按学生数排序（优先级±）：", "%d", &byStuCount) ||
                        !winScanf(hwnd, "按最大学生数排序（优先级±）：", "%d", &byStuMax) ||
                        !winScanf(hwnd, "按教练数排序（优先级±）：", "%d", &byCoachNum) ||
                        !winScanf(hwnd, "按价格排序（优先级±）：", "%d", &byPrice)) break;

                    if (sortCourseInfoForUser(byName, byTypeId, byTime, byLocation,
                        byStuCount, byStuMax, byCoachNum, byPrice)) {
                        successInfoForWindow("排序成功", hwnd);
                    }
                    else {
                        errorInfoForWindow("排序失败", hwnd);
                    }
                    break;
                }

                // 数据统计
                case IDC_MENU_USER_COUNT_COURSETYPE: {
                    if (countCourseTypeSelectedForUser()) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COUNT_COURSETYPEBYID: {
                    int typeId;
                    if (!winScanf(hwnd, "输入课程类型ID：", "%d", &typeId)) break;
                    if (countCourseTypeSelectedByIdForUser(typeId)) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COUNT_COACH: {
                    if (countCoachCourseForUser()) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
                    }
                    break;
                }
                case IDC_MENU_USER_COUNT_COACHBYID: {
                    int coachId;
                    if (!winScanf(hwnd, "输入教练ID：", "%d", &coachId)) break;
                    if (countCoachCourseByIdForUser(coachId)) {
                        successInfoForWindow("统计完成", hwnd);
                    }
                    else {
                        errorInfoForWindow("统计失败", hwnd);
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

// 显示学员控制台
int WINAPI showUserDashboard() {
    return registerAndCreateWindow(USERWINDOW_CLASS_NAME, "健身管理系统 | 学员控制台", procUserDashboardWindow, 200, 200, 700, 500, NULL);
}