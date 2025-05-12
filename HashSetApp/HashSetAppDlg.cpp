#include "pch.h"
#include "HashSetApp.h"
#include "HashSetAppDlg.h"
#include "afxdialogex.h"
#include <atlconv.h>
#include <thread>
#include "Logger.h"

#define WM_LOAD_PROGRESS (WM_USER + 100)
#define WM_LOAD_COMPLETE (WM_USER + 101)

BEGIN_MESSAGE_MAP(CHashSetAppDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_LOAD, &CHashSetAppDlg::OnBnClickedButtonLoad)
    ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CHashSetAppDlg::OnBnClickedButtonProcess)
    ON_MESSAGE(WM_LOAD_PROGRESS, &CHashSetAppDlg::OnLoadProgress)
    ON_MESSAGE(WM_LOAD_COMPLETE, &CHashSetAppDlg::OnLoadComplete)
END_MESSAGE_MAP()

CHashSetAppDlg::CHashSetAppDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_HASHSETAPP_DIALOG, pParent), m_isLoading(false) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    Logger::LogInfo("Диалоговое окно приложения создано");
}

void CHashSetAppDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS, m_progress);
    DDX_Control(pDX, IDC_BUTTON_LOAD, m_btnLoad);
    DDX_Control(pDX, IDC_BUTTON_PROCESS, m_btnProcess);
    DDX_Control(pDX, IDC_LIST_ALL_WORDS, m_listAllWords);
    DDX_Control(pDX, IDC_LIST_SHORT_WORDS, m_listShortWords);
    DDX_Control(pDX, IDC_LIST_PALINDROMES, m_listPalindromes);
}

BOOL CHashSetAppDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    Logger::LogInfo("Инициализация диалогового окна");

    m_progress.SetRange(0, 100);
    m_progress.SetPos(0);
    EnableControls(true);

    Logger::LogInfo("Прогресс-бар инициализирован (диапазон 0-100)");
    return TRUE;
}

void CHashSetAppDlg::EnableControls(bool enable) {
    m_btnLoad.EnableWindow(enable);
    m_btnProcess.EnableWindow(enable && !m_wordSet.empty());
    Logger::LogInfo(enable ? "Элементы управления включены" : "Элементы управления отключены");
}

UINT CHashSetAppDlg::LoadFileThread(LPVOID pParam) {
    CHashSetAppDlg* pDlg = (CHashSetAppDlg*)pParam;
    CString filePath = pDlg->m_filePathToLoad;
    CT2CA filePathConverted(filePath);
    std::string narrowPath(filePathConverted);

    Logger::LogInfo("Начало загрузки файла: " + narrowPath);

    try {
        std::ifstream file(narrowPath);
        if (!file.is_open()) {
            Logger::LogError("Ошибка открытия файла: " + narrowPath);
            pDlg->PostMessage(WM_LOAD_COMPLETE, 0, (LPARAM)_T("Ошибка открытия файла"));
            return 0;
        }

        HashSet tempSet;
        std::string word;
        size_t totalWords = 0;
        size_t progress = 0;

        Logger::LogInfo("Начало подсчета слов в файле");
        while (file >> word) {
            totalWords++;
        }
        Logger::LogInfo("Всего слов в файле: " + std::to_string(totalWords));

        file.clear();
        file.seekg(0);

        Logger::LogInfo("Начало обработки слов из файла");
        size_t processed = 0;
        while (file >> word) {
            word.erase(std::remove_if(word.begin(), word.end(),
                [](char c) { return !isalpha(c); }), word.end());

            if (!word.empty()) {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                tempSet.insert(word);
            }

            processed++;
            size_t newProgress = (processed * 100) / totalWords;
            if (newProgress > progress) {
                progress = newProgress;
                pDlg->PostMessage(WM_LOAD_PROGRESS, progress, 0);
            }
        }

        Logger::LogInfo("Файл успешно обработан. Уникальных слов: " + std::to_string(tempSet.size()));
        pDlg->PostMessage(WM_LOAD_COMPLETE, 1, (LPARAM)new HashSet(std::move(tempSet)));
    }
    catch (const std::exception& e) {
        Logger::LogError("Исключение при обработке файла: " + std::string(e.what()));
        pDlg->PostMessage(WM_LOAD_COMPLETE, 0, (LPARAM)_T("Ошибка обработки файла"));
    }
    catch (...) {
        Logger::LogError("Неизвестное исключение при обработке файла");
        pDlg->PostMessage(WM_LOAD_COMPLETE, 0, (LPARAM)_T("Ошибка обработки файла"));
    }

    return 0;
}

LRESULT CHashSetAppDlg::OnLoadProgress(WPARAM wParam, LPARAM lParam) {
    int progress = (int)wParam;
    m_progress.SetPos(progress);
    Logger::LogInfo("Прогресс загрузки: " + std::to_string(progress) + "%");
    return 0;
}

LRESULT CHashSetAppDlg::OnLoadComplete(WPARAM wParam, LPARAM lParam) {
    m_isLoading = false;
    m_progress.SetPos(100);

    if (wParam == 1) {
        HashSet* pResult = (HashSet*)lParam;
        m_wordSet = std::move(*pResult);
        delete pResult;

        m_listAllWords.ResetContent();
        for (const auto& word : m_wordSet) {
            m_listAllWords.AddString(CString(word.c_str()));
        }

        CString msg;
        msg.Format(_T("Загружено %d слов"), m_wordSet.size());
        MessageBox(msg, _T("Успешно"), MB_OK);

        Logger::LogInfo("Слова успешно загружены в интерфейс. Всего: " + std::to_string(m_wordSet.size()));
    }
    else {
        CString errorMsg = (LPCTSTR)lParam;
        CT2CA errorMsgConverted(errorMsg);
        Logger::LogError("Ошибка загрузки: " + std::string(errorMsgConverted));
        MessageBox(errorMsg, _T("Ошибка"), MB_ICONERROR);
    }

    EnableControls(true);
    return 0;
}

void CHashSetAppDlg::OnBnClickedButtonLoad() {
    Logger::LogInfo("Нажата кнопка 'Загрузить'");
    CFileDialog dlg(TRUE, _T("txt"), NULL,
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        _T("Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*||"), this);

    if (dlg.DoModal() == IDOK) {
        m_filePathToLoad = dlg.GetPathName();
        CT2CA filePathConverted(m_filePathToLoad);
        Logger::LogInfo("Выбран файл для загрузки: " + std::string(filePathConverted));

        m_isLoading = true;
        EnableControls(false);
        m_progress.SetPos(0);

        AfxBeginThread(LoadFileThread, this);
        Logger::LogInfo("Запущен поток для загрузки файла");
    }
    else {
        Logger::LogInfo("Пользователь отменил выбор файла");
    }
}

void CHashSetAppDlg::OnBnClickedButtonProcess() {
    Logger::LogInfo("Нажата кнопка 'Обработать'");
    if (m_wordSet.empty()) {
        Logger::LogWarning("Попытка обработки при пустом наборе слов");
        MessageBox(_T("Сначала загрузите слова из файла"), _T("Ошибка"), MB_ICONERROR);
        return;
    }

    m_listShortWords.ResetContent();
    m_listPalindromes.ResetContent();

    Logger::LogInfo("Начало обработки коротких слов");
    auto shortWords = m_wordSet.getShortWords();
    for (const auto& word : shortWords) {
        m_listShortWords.AddString(CString(word.c_str()));
    }

    Logger::LogInfo("Начало обработки палиндромов");
    auto palindromes = m_wordSet.getPalindromes();
    for (const auto& word : palindromes) {
        m_listPalindromes.AddString(CString(word.c_str()));
    }

    CString msg;
    msg.Format(_T("Найдено:\n- Коротких слов: %d\n- Перевёртышей: %d"),
        shortWords.size(), palindromes.size());
    MessageBox(msg, _T("Результаты"), MB_OK);

    Logger::LogInfo("Обработка завершена. Результаты: " +
        std::to_string(shortWords.size()) + " коротких слов, " +
        std::to_string(palindromes.size()) + " палиндромов");
}

void CHashSetAppDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        Logger::LogInfo("Открыто окно 'О программе'");
        CDialogEx aboutDlg(IDD_ABOUTBOX);
        aboutDlg.DoModal();
    }
    else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CHashSetAppDlg::OnPaint() {
    if (IsIconic()) {
        Logger::LogInfo("Отрисовка иконки свернутого окна");
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

HCURSOR CHashSetAppDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}