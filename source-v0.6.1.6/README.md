# Thần Long Item Consolidator v0.6.1.6

> **Residual scheduler limitation:** v0.6.1.6 removes logical cross-window input locks, but `BridgeClient::Call()` is still synchronous on the controller thread. A single Bridge timeout can therefore delay the next scheduler iteration for its bounded timeout. This is no longer mouse contention, but it is not yet a true per-PID asynchronous worker architecture. Do not claim full timing independence until Bridge calls are moved to per-client workers/mailboxes.

## v0.6.1.6 — Per-client cleanup + Movement Guard

- Xóa toàn bộ global hidden-input owner/busy/sequence lease còn sót từ kiến trúc click vật lý. SELL, Train, Revive, XN và các client không liên quan không còn nhường nhau một tài nguyên input chung.
- Trade giữ thứ tự MAIN↔CON bằng `tradeTxn_`/`tradeHeld` nghiệp vụ, không dùng input lease. SELL của PID khác không làm trade dừng.
- REC chỉ tạm giữ acc đang capture; REC trade giữ đúng MAIN + CON đang capture. Các acc còn lại vẫn chạy.
- Priority runtime chuyển sang cục bộ theo từng PID: P1 XN → P2 Đầu thai → P3 AUTO trong chính acc đó; không còn global mouse barrier.
- **Invariant mới:** cả `Action::Mount` và `Action::StartPath` đều phải qua `EnsureAutoFightOffForTravel()`. AutoFight không authoritative hoặc còn ON → không được lên ngựa/AutoPath. Recovery giữ nguyên DỪNG #1 → DỪNG #2 → AUTO→ĐÁNH QUÁI reset → lặp.
- `Client Freeze`, F4 global pause, FIFO trade, death lifecycle, route ownership reset và AutoPath/Fight invariant giữ nguyên.
- Protocol EXE/DLL: `0x00010616`.


Nền phát triển trực tiếp: source v0.5.0 do người dùng cung cấp. Source v0.8.4 chỉ là donor để nghiên cứu cơ chế callback UI nội bộ; không ghép nguyên remote worker của donor.

## Tải bản Windows x64

- v0.6.1.5 loại bỏ đường auto-click chuột Windows còn lại trong chuỗi giao dịch MAIN/CON; mọi dòng tọa độ dùng generic InputSync hidden action.
- Gói runtime: GitHub Actions artifact `ThanLongItemConsolidator-v0.6.1.5-win-x64` từ [run 307](https://github.com/ngmthang-g/TEST-chuc-nang-than-long-2/actions/runs/32427510483).
- Source: branch [`codex/v0.6.1.5-hidden-trade-autopath-guard`](https://github.com/ngmthang-g/TEST-chuc-nang-than-long-2/tree/codex/v0.6.1.5-hidden-trade-autopath-guard), tree `b350db03588e2317a2e03e5b706b35ac9c695304`.
- Artifact ZIP: `dd21bf0d3f96a4e8b37d623cf5c54beb5a4e8f067033f5fb42f4261596e7988a`; EXE: `6d2bdfd6015ab6a6b9c503a8f3329228d73d556a31f4059a54b3927d39046c51`; DLL: `da1febd514af0951ebcab56de575dfa794f00ee99614157684a456d65faf554c`.
- Bản v0.6.1.2 được giữ nguyên để đối chiếu lỗi `geometry=149`.
- Bản v0.6.1.1 được giữ nguyên để đối chiếu lỗi runtime.
- Bản v0.6.1 gốc vẫn nằm tại [release/ThanLongItemConsolidator-v0.6.1-win-x64.zip](release/ThanLongItemConsolidator-v0.6.1-win-x64.zip) để đối chiếu.
- v0.6 cũ được giữ tại [release/ThanLongItemConsolidator-v0.6-win-x64.zip](release/ThanLongItemConsolidator-v0.6-win-x64.zip) để bảo toàn lịch sử, nhưng **không nên dùng để test callback UI**.
- Luôn giữ EXE cùng thư mục với `ThanLongCleanRouteBridge.dll`; protocol v0.6.1.5 cố ý từ chối DLL cũ.

v0.6.1.3 Windows CI run 306 đã PASS và người dùng xác nhận luồng item chạy đủ 90 click cực kỳ thành công. v0.6.1.5 Windows MSVC x64 run 307 đã PASS verifier, build và đủ tám nhóm self-test; live trade/AUTO vẫn **RUNTIME UNTESTED** cho tới khi chạy đúng cặp EXE/DLL trên client thật.

## Hidden trade + AutoPath invariant v0.6.1.5

- Xóa hoàn toàn đường tự động `SetForegroundWindow → SetCursorPos → SendInput`; F8/REC chỉ còn ghi thao tác tay để cấu hình tọa độ.
- Mọi dòng giao dịch MAIN/CON được scale + normalize rồi gửi `ClickInternalPoint` tới đúng PID/window thread. Bridge thực hiện `TryClickUI → EndUIDrag` và fail-closed nếu raycast/drag lifecycle sai.
- Sequence lease chỉ giữ thứ tự nghiệp vụ; nó không sở hữu con trỏ Windows. Delay/repeat/group/FIFO và `FreeBagSpace` verify cuối pass được giữ nguyên.
- Mọi `StartPath` vẫn phải qua Travel Guard: DỪNG lần 1 → verify; DỪNG lần 2 → verify; nếu vẫn ON thì `AUTO → ĐÁNH QUÁI` một lần để đồng bộ rồi lặp DỪNG.
- Thêm invariant runtime: nếu snapshot từng thấy `AutoPath ON + AutoFight ON`, tool dừng path ngay, chạy lại Travel Guard và chỉ cho route resume sau khi thấy cả hai OFF.
- Một request bật Đánh quái dù đã xếp hàng cũng bị kiểm tra lại ngay trước từng dispatch; AutoPath chưa đọc được hoặc đang ON thì request bị hủy, không được dùng state cũ để bật AutoFight.
- Mailbox P3 gắn owner riêng cho Train, Travel Guard và Mount Recovery; workflow mới không thể nhận nhầm kết quả `Attack` còn sót của workflow cũ.

## Hotfix AUTO point sequence v0.6.1.4

- F8 chỉ ghi tọa độ tương đối theo client game; F8 không tự click và không lấy pointer UI.
- Bridge có command dùng chung `ClickInternalPoint`: đổi điểm sang Unity screen rồi gọi đủ `TryClickUI(0, point) → EndUIDrag(point)`.
- Một request bật Train chạy đúng thứ tự: điểm `AUTO` (click 1) → chờ 500 ms bằng state machine → điểm `ĐÁNH QUÁI` (click 2) → chờ snapshot `AutoFight ON`.
- Travel Guard/Dừng dùng điểm `DỪNG AUTO 2` và chỉ cho route chạy khi snapshot xác nhận `AutoFight OFF`.
- Nút TEST của ba dòng phát riêng đúng điểm F8 tương ứng để kiểm tra tọa độ trước khi bật nhiều account.
- Không foreground, không `SetCursorPos`, không `SendInput`; click trả OK vẫn chưa phải thành công nghiệp vụ nếu state AutoFight chưa đổi.

## Hotfix click item v0.6.1.3

- Log v0.6.1.2 `geometry=149` chứng minh resolver đã hoạt động, nhưng ô item không phải một `UIButton/UIRect` callback nằm dưới tọa độ; vì vậy cách dò 149 control không thể bấm ô đó.
- Repo dữ liệu client hiện tại chứng minh `InputSyncManager` có đúng chuỗi input UI của game. Một click hoàn chỉnh là `TryClickUI(0, point)` (nhấn trái) rồi `EndUIDrag(point)` (nhả/click), không phải gọi riêng `TryClickUI`.
- Bước item mới phát đúng cặp nhấn/nhả qua Unity EventSystem trên game thread. Nó không foreground, không `SetCursorPos`, không `SendInput` và không dùng chuột Windows.
- Trước nhấn phải không có `_uiDragging`; sau nhấn phải raycast trúng và tạo drag state; sau nhả state phải sạch. Nếu pha nhả lỗi, Bridge chỉ hủy drag do chính lượt đó tạo rồi dừng fail-closed.
- Tọa độ vẫn lấy từ dòng click đã gán; nên đặt ở tâm ô trang bị thứ 2 sau khi đã mở tab Trang bị.

## Hotfix resolver Unity geometry v0.6.1.2

- Log v0.6.1.1 chứng minh Bridge chưa hề dùng tọa độ hoặc callback item: nó dừng khi tìm class geometry.
- v0.6.1.1 sai ở giả định mọi class đều thuộc `UnityEngine.CoreModule`.
- v0.6.1.2 tìm riêng từng class qua `CoreModule`, `UIModule` và `UnityEngine.dll`. `RectTransformUtility` ưu tiên UIModule.
- Nếu vẫn thiếu, log mới nêu chính xác class nào thiếu và assembly nào thực sự mở được.
- Không đổi cách chọn dòng: chuỗi 6 click vẫn lấy dòng số 5; `6/6` trong log cũ là sáu lần retry callback, không phải dòng số 6.

## Logic ô trang bị cố định được giữ từ v0.5

- Chuỗi đang tốt `ClickNPC → shop → Bán vật phẩm → Bán nhanh → Trang bị` giữ nguyên.
- Chỉ bước item tay nải đổi: Bridge dùng tọa độ đã lấy để phát một click UI nội bộ của chính client. Không foreground, không `SendInput`, không di chuyển chuột.
- Profile v0.5 đủ 5 dòng dùng dòng 5. Nếu profile chỉ còn một dòng thì dùng chính dòng đó; với 2–4 dòng dùng dòng cuối.
- Vòng bán đầu gọi ô cố định 90 lần. Sau khi đóng UI và `FreeBagSpace` ổn định 1,5 giây, vòng bán sau dùng đúng số ô trống đã học, tối đa 90. Giá trị học chỉ sống trong phiên chạy tool như v0.5.
- Delay của dòng tọa độ vẫn là delay giữa từng callback. Cột `Lặp` cũ không quyết định vòng đầu trong hotfix này vì yêu cầu đã cố định là 90.
- Nên lấy F8 tại tâm ô trang bị thứ 2 sau khi đã mở đúng tab Trang bị. Từ v0.6.1.5, `TEST DÒNG`, chuỗi AUTO BÁN và chuỗi giao dịch đều dùng đường callback/InputSync nội bộ.

## Sửa lỗi resolver v0.6.1

- Basic discovery chỉ cần `UIObject.instances` và ít nhất một control class. `UIButton`/`UIToggle` không còn bị chặn vì thiếu Lua Executor.
- Namespace cũ vẫn là đường nhanh; nếu client đặt lớp ở namespace khác, Bridge quét metadata có giới hạn và chỉ nhận UIObject/Button/Toggle/Rect có đúng field/method cần dùng.
- `UIRectTransform` mới tải `MonoBehaviourExecutor` khi chính nó được chọn.
- AUTO Lua mới tải thêm `LuaSystemAPI_GUI`, `System.Object` và Executor.
- Executor được thử nhiều namespace, sau đó quét metadata theo tên lớp và bắt buộc có đúng `get_Instance()` + `ExecuteScriptFunction(3)`.
- Nếu vẫn lỗi, log trả chính xác component thiếu thay vì gộp bảy dependency thành một câu.

## Thay đổi chính

- `Xác nhận ra map`: vẫn giữ nguyên điều kiện watchdog Lâu Lan của v0.5, nhưng Bridge tìm nút đồng ý duy nhất bên trong `MessageBox` rồi gọi callback nội bộ. Không foreground cửa sổ, không di chuyển chuột.
- `Đầu thai`: Bridge đọc lại `IsDeath=true`, tìm đúng control `Đầu thai` và gọi `UIButton.HandleClickEvent()`.
- `AUTO → Đánh quái`: active P3 dùng hai điểm InputSync cấu hình; `Dừng AUTO` dùng điểm InputSync thứ ba. Controller vẫn xác minh kết quả bằng snapshot AutoFight authoritative.
- `Tự bán đồ`: các stage semantic qua Trang bị giữ nguyên; v0.6.1.3 gọi lặp một ô trang bị do tọa độ cấu hình chọn, rồi đóng UI và xác minh `FreeBagSpace` ổn định.
- Chuỗi giao dịch MAIN/CON dùng tọa độ cấu hình nhưng phát qua InputSync nội bộ; không foreground và không chiếm chuột Windows.

## Kiến trúc tích hợp

v0.6.1.5 tiếp tục dùng Bridge DLL `WH_GETMESSAGE` vốn có của v0.5. Mỗi Bridge request chỉ phát một click hoàn chỉnh trên đúng message thread; controller tách repeat/delay thành scheduler ticks và không chạy vòng lặp dài trong hook. Không thêm `CreateRemoteThread` hoặc worker IL2CPP thứ hai.

Điểm XN/Đầu thai vẫn chỉ để tương thích vì hai action này dùng semantic callback. Ba điểm AUTO/Đánh quái/Dừng đã active lại qua InputSync. Editor macro bán vẫn cung cấp tọa độ/delay ô item; bốn click mở shop cũ không quay lại active runtime.

## Build và kiểm tra

Workflow Windows x64 chạy:

1. `tools/verify_v0615_logic.py`.
2. Build controller + Bridge bằng MSVC.
3. Route, rotation, trade coordinator self-tests.
4. Background UI scoring self-test.
5. Fixed-slot selection/adaptive-count self-test.
6. Unity geometry resolver search-order self-test.
7. Internal UI press/release dispatch-plan self-test.
8. Travel Guard + AutoPath/AutoFight invariant self-test.

Windows MSVC x64 [run 307](https://github.com/ngmthang-g/TEST-chuc-nang-than-long-2/actions/runs/32427510483) PASS toàn bộ các bước trên. Artifact ID `9427940998`, GitHub digest `sha256:dd21bf0d3f96a4e8b37d623cf5c54beb5a4e8f067033f5fb42f4261596e7988a`. Build pass không được suy thành live-client pass.

Đọc [DONOR_0.8.4_BACKGROUND_ACTION_ANALYSIS.md](DONOR_0.8.4_BACKGROUND_ACTION_ANALYSIS.md) để xem diễn giải chi tiết donor 0.8.4 và quyết định chuyển đổi.
