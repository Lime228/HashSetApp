#include "pch.h"
#include "HashSetApp.h"
#include "HashSetAppDlg.h"
#include "Logger.h"

BEGIN_MESSAGE_MAP(CHashSetAppApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CHashSetAppApp::CHashSetAppApp() {
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
    Logger::LogInfo("Приложение создано. Инициализация менеджера перезапуска");
}

BOOL CHashSetAppApp::InitInstance() {
    Logger::LogInfo("Начало инициализации приложения (InitInstance)");

    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    if (!InitCommonControlsEx(&InitCtrls)) {
        Logger::LogError("Ошибка инициализации общих элементов управления");
        return FALSE;
    }
    Logger::LogInfo("Общие элементы управления успешно инициализированы");

    CWinApp::InitInstance();
    AfxEnableControlContainer();
    Logger::LogInfo("Базовая инициализация CWinApp завершена");

    CShellManager* pShellManager = new CShellManager;
    if (!pShellManager) {
        Logger::LogError("Ошибка создания CShellManager");
        return FALSE;
    }
    Logger::LogInfo("Менеджер оболочки создан");

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
    Logger::LogInfo("Визуальный менеджер установлен (CMFCVisualManagerWindows)");

    SetRegistryKey(_T("Local AppWizard-Generated Applications"));
    Logger::LogInfo("Ключ реестра установлен");

    Logger::LogInfo("Создание главного диалогового окна");
    CHashSetAppDlg dlg;
    m_pMainWnd = &dlg;

    Logger::LogInfo("Запуск модального диалога");
    INT_PTR nResponse = dlg.DoModal();
    Logger::LogInfo("Диалог закрыт с кодом: " + std::to_string(nResponse));

    if (pShellManager != nullptr) {
        delete pShellManager;
        Logger::LogInfo("Менеджер оболочки удален");
    }

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
    Logger::LogInfo("Очистка ControlBar выполнена");
#endif

    Logger::LogInfo("Завершение работы приложения (InitInstance возвращает FALSE)");
    return FALSE;
}

CHashSetAppApp theApp;