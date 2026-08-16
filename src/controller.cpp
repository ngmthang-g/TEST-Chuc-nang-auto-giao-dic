#include "controller_support.inl"
#include <cstdlib>

namespace {

struct NearbyPlayer {
    std::uint64_t roleId = 0;
    std::wstring name;
    std::wstring source;
};

class App {
public:
    bool Create(HINSTANCE instance) {
        INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_STANDARD_CLASSES};
        InitCommonControlsEx(&controls);

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = L"ThanLongAutoTradeTestWindow";
        if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

        hwnd_ = CreateWindowExW(0, wc.lpszClassName, kTitle,
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, 790, 595,
                                nullptr, nullptr, instance, this);
        return hwnd_ != nullptr;
    }

    void Show(int cmd) {
        ShowWindow(hwnd_, cmd);
        UpdateWindow(hwnd_);
    }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        App* self = reinterpret_cast<App*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (msg == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lp);
            self = reinterpret_cast<App*>(create->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            self->hwnd_ = hwnd;
        }
        return self ? self->Handle(msg, wp, lp) : DefWindowProcW(hwnd, msg, wp, lp);
    }

    HWND Make(const wchar_t* cls, const wchar_t* text, DWORD style,
              int x, int y, int w, int h, int id) {
        return CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style,
                               x, y, w, h, hwnd_,
                               reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), nullptr, nullptr);
    }

    void BuildUi() {
        HFONT font = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
        auto applyFont = [font](HWND h) {
            if (h) SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
        };

        applyFont(Make(L"STATIC", L"CLIENT GAME", 0, 18, 15, 130, 22, 0));
        clientCombo_ = Make(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_VSCROLL,
                            18, 39, 500, 260, IDC_CLIENT);
        applyFont(clientCombo_);
        applyFont(Make(L"BUTTON", L"QUÉT CLIENT", BS_PUSHBUTTON,
                       532, 38, 110, 30, IDC_SCAN));
        applyFont(Make(L"BUTTON", L"TEST BRIDGE", BS_PUSHBUTTON,
                       650, 38, 110, 30, IDC_PROBE));

        applyFont(Make(L"STATIC", L"NGƯỜI GẦN — direct GetNearByPeacePlayers, không cần tổ đội",
                       0, 18, 84, 470, 22, 0));
        playerCombo_ = Make(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_VSCROLL,
                            18, 108, 615, 260, IDC_TEAM);
        applyFont(playerCombo_);
        applyFont(Make(L"BUTTON", L"QUÉT NGƯỜI GẦN", BS_PUSHBUTTON,
                       645, 107, 115, 30, IDC_TEAM_SCAN));

        applyFont(Make(L"STATIC",
                       L"TEST DUY NHẤT: target nội bộ → gửi Trade Request → kiểm tra bảng Trade",
                       0, 18, 158, 650, 22, 0));
        applyFont(Make(L"BUTTON", L"1. TARGET NỘI BỘ", BS_PUSHBUTTON,
                       18, 184, 225, 48, IDC_TARGET));
        applyFont(Make(L"BUTTON", L"2. TARGET + GỬI GIAO DỊCH", BS_DEFPUSHBUTTON,
                       258, 184, 310, 48, IDC_TRADE));
        applyFont(Make(L"BUTTON", L"CHECK BẢNG TRADE", BS_PUSHBUTTON,
                       583, 184, 177, 48, IDC_CHECK_UI));

        status_ = Make(L"STATIC", L"RUNTIME: CHƯA TEST",
                       SS_CENTER | SS_CENTERIMAGE | WS_BORDER,
                       18, 249, 742, 50, IDC_STATUS);
        applyFont(status_);

        applyFont(Make(L"STATIC",
                       L"v0.1.1 bỏ LuaEnv.DoString. Timeout sẽ ghi stage chính xác; không tự retry/spam request.",
                       0, 18, 311, 742, 22, 0));

        log_ = Make(L"EDIT",
                    L"v0.1.1 — direct semantic Auto Trade; không DoString/SendInput/click tọa độ.\r\n",
                    WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL,
                    18, 339, 742, 190, IDC_LOG);
        applyFont(log_);

        ScanClients();
    }

    void Log(const std::wstring& line) {
        const int length = GetWindowTextLengthW(log_);
        SendMessageW(log_, EM_SETSEL, length, length);
        const std::wstring text = line + L"\r\n";
        SendMessageW(log_, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text.c_str()));
        SendMessageW(log_, EM_SCROLLCARET, 0, 0);
    }

    void StopTradePoll() {
        if (tradePollActive_) KillTimer(hwnd_, kTradePollTimer);
        tradePollActive_ = false;
        tradePollCount_ = 0;
    }

    void ClearPlayers() {
        StopTradePoll();
        players_.clear();
        SendMessageW(playerCombo_, CB_RESETCONTENT, 0, 0);
    }

    void ScanClients() {
        bridge_.Close();
        ClearPlayers();
        clients_ = FindClients();
        SendMessageW(clientCombo_, CB_RESETCONTENT, 0, 0);
        for (const auto& client : clients_) {
            const std::wstring label = L"PID " + std::to_wstring(client.pid) + L"  •  " + client.title;
            SendMessageW(clientCombo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        }
        if (!clients_.empty()) SendMessageW(clientCombo_, CB_SETCURSEL, 0, 0);
        Log(L"Quét thấy " + std::to_wstring(clients_.size()) + L" client GameAssembly.dll");
    }

    bool EnsureAttach(std::wstring& error) {
        const int index = static_cast<int>(SendMessageW(clientCombo_, CB_GETCURSEL, 0, 0));
        if (index < 0 || index >= static_cast<int>(clients_.size())) {
            error = L"Chưa chọn client";
            return false;
        }
        const GameClient& client = clients_[static_cast<std::size_t>(index)];
        if (bridge_.AttachedTo(client.pid)) return true;
        if (!bridge_.Attach(client, error)) return false;
        Log(L"Đã attach PID " + std::to_wstring(client.pid) + L" bằng AutoTrade v0.1.1 bridge");
        return true;
    }

    bool Call(Command command, std::uint64_t roleId, Response& response,
              std::wstring& error, DWORD timeoutMs = 3000) {
        if (!EnsureAttach(error)) return false;
        return bridge_.Call(command, roleId, response, error, timeoutMs);
    }

    void Probe() {
        std::wstring error;
        Response response{};
        if (!Call(Command::Probe, 0, response, error, 1500)) {
            SetControlText(status_, L"HOOK/BRIDGE: FAIL");
            Log(L"PROBE FAIL: " + error);
            return;
        }
        SetControlText(status_, L"HOOK + SHARED MEMORY: PASS");
        Log(response.detail[0] ? response.detail : L"Hook probe PASS");
        if (response.data[0]) Log(std::wstring(L"Probe: ") + response.data);
        Log(L"Probe v0.1.1 cố ý không gọi IL2CPP/Lua; bấm QUÉT NGƯỜI GẦN để test semantic runtime.");
    }

    static bool ParsePlayerLine(const std::wstring& line, NearbyPlayer& player) {
        const std::size_t tab1 = line.find(L'\t');
        if (tab1 == std::wstring::npos) return false;
        const std::size_t tab2 = line.find(L'\t', tab1 + 1);
        if (tab2 == std::wstring::npos) return false;
        const std::wstring roleText = line.substr(0, tab1);
        wchar_t* end = nullptr;
        const unsigned long long role = std::wcstoull(roleText.c_str(), &end, 10);
        if (!role || !end || *end != 0) return false;
        player.roleId = static_cast<std::uint64_t>(role);
        player.name = line.substr(tab1 + 1, tab2 - tab1 - 1);
        player.source = line.substr(tab2 + 1);
        while (!player.source.empty() && (player.source.back() == L'\r' || player.source.back() == L'\n')) {
            player.source.pop_back();
        }
        return true;
    }

    void LoadPlayersFromText(const wchar_t* text) {
        players_.clear();
        SendMessageW(playerCombo_, CB_RESETCONTENT, 0, 0);
        if (!text || !*text) return;
        const std::wstring all(text);
        std::size_t start = 0;
        while (start <= all.size()) {
            const std::size_t end = all.find(L'\n', start);
            const std::wstring line = all.substr(start,
                end == std::wstring::npos ? std::wstring::npos : end - start);
            NearbyPlayer player{};
            if (ParsePlayerLine(line, player)) {
                players_.push_back(player);
                const std::wstring label = (player.name.empty() ? L"(không tên)" : player.name) +
                    L"  |  RoleID " + std::to_wstring(player.roleId) + L"  |  AOI";
                SendMessageW(playerCombo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
            }
            if (end == std::wstring::npos) break;
            start = end + 1;
        }
        if (!players_.empty()) SendMessageW(playerCombo_, CB_SETCURSEL, 0, 0);
    }

    void QueryPlayers() {
        StopTradePoll();
        std::wstring error;
        Response response{};
        if (!Call(Command::QueryTeam, 0, response, error, 4000)) {
            SetControlText(status_, L"QUÉT NGƯỜI GẦN: FAIL");
            Log(L"NEARBY SCAN FAIL: " + error);
            return;
        }
        LoadPlayersFromText(response.data);
        if (players_.empty()) {
            SetControlText(status_, L"NGƯỜI GẦN: API PASS NHƯNG DANH SÁCH RỖNG");
            Log(L"NEARBY PASS nhưng GetNearByPeacePlayers hiện không trả record usable trong AOI");
        } else {
            SetControlText(status_, L"NGƯỜI GẦN: ĐÃ QUÉT " + std::to_wstring(players_.size()) + L" NGƯỜI");
            Log(L"NEARBY PASS: " + std::to_wstring(players_.size()) + L" người từ GetNearByPeacePlayers");
            if (response.detail[0]) Log(std::wstring(L"Scanner: ") + response.detail);
        }
    }

    const NearbyPlayer* SelectedPlayer() const {
        const int index = static_cast<int>(SendMessageW(playerCombo_, CB_GETCURSEL, 0, 0));
        if (index < 0 || index >= static_cast<int>(players_.size())) return nullptr;
        return &players_[static_cast<std::size_t>(index)];
    }

    void SelectTargetOnly() {
        StopTradePoll();
        const NearbyPlayer* player = SelectedPlayer();
        if (!player) {
            SetControlText(status_, L"TARGET: CHƯA CHỌN NGƯỜI");
            Log(L"TARGET: hãy QUÉT NGƯỜI GẦN và chọn một người trước");
            return;
        }
        std::wstring error;
        Response response{};
        if (!Call(Command::SelectTarget, player->roleId, response, error, 3500)) {
            SetControlText(status_, L"TARGET: FAIL");
            Log(L"TARGET FAIL RoleID " + std::to_wstring(player->roleId) + L": " + error);
            return;
        }
        SetControlText(status_, L"TARGET: PASS • RoleID " + std::to_wstring(response.selectedRoleId));
        Log(L"TARGET PASS: " + std::wstring(response.data));
        if (response.detail[0]) Log(std::wstring(L"Target proof: ") + response.detail);
    }

    void SendTrade() {
        StopTradePoll();
        const NearbyPlayer* player = SelectedPlayer();
        if (!player) {
            SetControlText(status_, L"GIAO DỊCH: CHƯA CHỌN NGƯỜI");
            Log(L"TRADE: hãy QUÉT NGƯỜI GẦN và chọn một người trước");
            return;
        }
        std::wstring error;
        Response response{};
        if (!Call(Command::SelectAndTrade, player->roleId, response, error, 4500)) {
            SetControlText(status_, L"TARGET + TRADE REQUEST: FAIL");
            Log(L"TRADE FAIL RoleID " + std::to_wstring(player->roleId) + L": " + error);
            return;
        }
        SetControlText(status_, L"TRADE REQUEST: ĐÃ GỬI • CHỜ ACC KIA CHẤP NHẬN");
        Log(L"TRADE REQUEST PASS: " + std::wstring(response.data));
        if (response.detail[0]) Log(std::wstring(L"Trade path: ") + response.detail);
        Log(L"Đang tự check Trade UI khoảng 30 giây; không tự chấp nhận/chuyển đồ/xác nhận.");
        tradePollCount_ = 0;
        tradePollActive_ = SetTimer(hwnd_, kTradePollTimer, 750, nullptr) != 0;
        if (!tradePollActive_) Log(L"Không tạo được timer auto-check; dùng nút CHECK BẢNG TRADE thủ công");
    }

    void CheckTradeUi(bool manual) {
        std::wstring error;
        Response response{};
        if (!Call(Command::QueryTradeUi, 0, response, error, 2500)) {
            if (manual || tradePollCount_ == 0) Log(L"TRADE UI CHECK FAIL: " + error);
            if (!manual) StopTradePoll();
            return;
        }
        if (response.tradeUiVisible) {
            SetControlText(status_, L"TRADE UI: OPEN • TEST PASS");
            Log(L"TRADE UI PASS: " + std::wstring(response.data));
            StopTradePoll();
            return;
        }
        if (manual) {
            SetControlText(status_, L"TRADE UI: CHƯA PHÁT HIỆN");
            Log(L"TRADE UI: CLOSED/không khớp candidate semantic name");
        }
    }

    void PollTradeUi() {
        ++tradePollCount_;
        CheckTradeUi(false);
        if (!tradePollActive_) return;
        if (tradePollCount_ >= 40) {
            StopTradePoll();
            SetControlText(status_, L"TRADE UI: CHƯA PHÁT HIỆN SAU 30 GIÂY");
            Log(L"TIMEOUT UI không chứng minh packet fail; có thể acc kia chưa nhận hoặc exact Trade UI name chưa được catalog.");
        }
    }

    LRESULT Handle(UINT msg, WPARAM wp, LPARAM lp) {
        (void)lp;
        switch (msg) {
            case WM_CREATE:
                BuildUi();
                return 0;
            case WM_COMMAND:
                switch (LOWORD(wp)) {
                    case IDC_SCAN: ScanClients(); break;
                    case IDC_PROBE: Probe(); break;
                    case IDC_TEAM_SCAN: QueryPlayers(); break;
                    case IDC_TARGET: SelectTargetOnly(); break;
                    case IDC_TRADE: SendTrade(); break;
                    case IDC_CHECK_UI: CheckTradeUi(true); break;
                    default: break;
                }
                return 0;
            case WM_TIMER:
                if (wp == kTradePollTimer) PollTradeUi();
                return 0;
            case WM_DESTROY:
                StopTradePoll();
                bridge_.Close();
                PostQuitMessage(0);
                return 0;
            default:
                return DefWindowProcW(hwnd_, msg, wp, lp);
        }
    }

    HWND hwnd_ = nullptr;
    HWND clientCombo_ = nullptr;
    HWND playerCombo_ = nullptr;
    HWND status_ = nullptr;
    HWND log_ = nullptr;
    std::vector<GameClient> clients_;
    std::vector<NearbyPlayer> players_;
    BridgeClient bridge_;
    bool tradePollActive_ = false;
    unsigned int tradePollCount_ = 0;
};

} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    App app;
    if (!app.Create(instance)) return 2;
    app.Show(show);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
