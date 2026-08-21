# PROJECT KNOWLEDGE — v0.6.1.6 CURRENT

> **Residual scheduler limitation:** v0.6.1.6 removes logical cross-window input locks, but `BridgeClient::Call()` is still synchronous on the controller thread. A single Bridge timeout can therefore delay the next scheduler iteration for its bounded timeout. This is no longer mouse contention, but it is not yet a true per-PID asynchronous worker architecture. Do not claim full timing independence until Bridge calls are moved to per-client workers/mailboxes.

## v0.6.1.6 — quyết định kiến trúc hiện tại

1. Không tồn tại global mouse/input ownership giữa các cửa sổ. `coordinatorInputBusy_`, `coordinatorOwnerPid_`, `coordinatorSequenceLease_`, `coordinatorSequenceOwnerPid_`, `coordinatorRecording_` đã xóa.
2. Trade atomicity là nghiệp vụ của `tradeTxn_` + MAIN/CON `tradeHeld`; không phải input lease. SELL acc khác không chặn trade.
3. REC scoped theo PID/cặp MAIN-CON; không freeze toàn scheduler.
4. P1/P2/P3 là priority cục bộ theo PID. F4 vẫn là global pause chủ động.
5. Movement invariant: **Mount và StartPath đều yêu cầu AutoFight authoritative OFF**. Cả hai dùng cùng `EnsureAutoFightOffForTravel`: Stop #1 → Stop #2 → AUTO/Attack reset → lặp đến OFF.
6. Mount Recovery vẫn được phép bật Fight 10s để thoát kẹt sau 2 lần Mount fail, nhưng trước lần `ToggleRide(Mount)` kế tiếp guard bắt buộc tắt Fight.
7. Client Freeze và AutoPath+AutoFight conflict latch giữ nguyên vì đó là safety state của client, không phải mouse contention.


## Quy tắc dự án

- Nền phát triển trực tiếp là v0.5.0; v0.8.4 chỉ là donor nghiên cứu.
- Không thay logic ngoài phạm vi nếu không cần để tích hợp.
- Callback trả OK không phải bằng chứng nghiệp vụ; luôn verify bằng snapshot tương ứng.
- Mỗi version cập nhật README, changelog, source provenance, bảng logic, verifier và hồ sơ runtime/bug.

## Trạng thái hiện tại

- v0.6: **RUNTIME FAIL** đối với XN Lâu Lan và mở chuỗi bán nền. Cùng lỗi: `Không resolve đủ UIObject/UIButton/UIToggle/UIRect/Executor`.
- Điều runtime đã chứng minh: Bridge attach, snapshot, route và `ClickNPC` trước chuỗi UI vẫn hoạt động đủ để FSM đi tới đúng điểm lỗi. Log không chứng minh component cụ thể nào null.
- BUG-001 root cause ở mức **CONFIRMED**: basic UI enumeration bị khóa bởi một readiness gate gộp cả dependency Lua/Executor không cần cho UIButton. `MonoBehaviourExecutor` là component **LIKELY** bị thiếu vì namespace của nó chỉ là giả định từ donor RVA, chưa có metadata proof trong v0.6.
- v0.6.1: source audit + Windows MSVC x64 CI 274 **BUILD PASS**, nhưng vẫn **RUNTIME UNTESTED**. Hotfix chưa được gọi là runtime pass cho tới khi người dùng test lại.
- User runtime sau đó xác nhận chuỗi bán v0.6.1 đi đúng tới tab Trang bị nhưng bước item không giữ hành vi một ô lặp/dồn đồ của v0.5. Đây là BUG-002.
- v0.6.1.1: Windows MSVC x64 CI run 295 **BUILD PASS**, gồm scope audit và đủ năm nhóm self-test. Live runtime vẫn **RUNTIME UNTESTED** cho tới khi người dùng chạy đúng cặp EXE/DLL trên client thật.
- User test v0.6.1.1 xác nhận item stage fail `6/6` với chi tiết `Thiếu RectTransform/Utility/Screen để hit-test ô cố định`. Đây là BUG-003: resolver ép năm class vào riêng CoreModule; lỗi xảy ra trước hit-test/callback nên không liên quan dòng 5.
- v0.6.1.2: Windows MSVC x64 CI run 302 **BUILD PASS**, gồm scope audit và đủ sáu nhóm self-test. Live runtime vẫn **RUNTIME UNTESTED** cho tới khi người dùng chạy đúng cặp EXE/DLL.
- User test v0.6.1.2 trả `Không có UIButton/UIRect callback ... geometry=149`. Resolver BUG-003 đã qua runtime, nhưng mô hình “ô túi là custom callback control” sai; đây là BUG-004.
- v0.6.1.3 thay active item action bằng cặp `InputSyncManager.TryClickUI(0, point) → EndUIDrag(point)`. Windows CI run 306 **BUILD PASS**; người dùng sau đó xác nhận chuỗi 90 click chạy cực kỳ thành công trên client thật. BUG-004 vì vậy là **RUNTIME PASS** cho đường InputSync item.
- AUTO Lua hiện tại vẫn **RUNTIME FAIL** với lỗi không tìm thấy Lua UI/action theo tên; sell pass không chứng minh AUTO pass. Đây là BUG-005.
- v0.6.1.4 mở đường InputSync đã runtime-pass thành command click điểm dùng chung. P3 Start chạy `AUTO (click 1) → chờ 500 ms → ĐÁNH QUÁI (click 2) → verify AutoFight`; Stop dùng điểm `DỪNG AUTO 2` rồi vẫn verify authoritative. Scope audit và bảy pure-test group **PASS**; Windows build **PENDING**, live AUTO **RUNTIME UNTESTED**.
- v0.6.1.5 xóa đường chuột Windows còn lại khỏi trade, chuyển mọi dòng giao dịch sang generic `ClickInternalPoint`, và thêm hard invariant recovery nếu thấy `AutoPath ON + AutoFight ON`. Scope audit, tám pure-test group, local Windows x64 cross-build và GitHub MSVC x64 run 307 **PASS**; live runtime vẫn chờ bằng chứng.

## Kiến trúc action v0.6.1.5

Controller vẫn là scheduler/FSM. `ThanLongCleanRouteBridge.dll` vẫn được nạp bằng `WH_GETMESSAGE` vào game window thread. Protocol `0x00010615` từ chối ghép EXE/DLL khác version.

Không có `CreateRemoteThread` trong controller/Bridge. Một mapping chỉ có một request in-flight; timeout không được ghi đè.

Resolver UI chia hai tầng:

1. `EnsureUiDiscovery`: `UIObject.instances` + các class control hiện có. Dùng namespace đã biết trước, rồi fallback quét metadata có giới hạn và kiểm tra method/field surface; dùng cho scan và UIButton/UIToggle.
2. `EnsureUiLua`: chỉ gọi khi UIRect/Lua action thực sự cần `MonoBehaviourExecutor`, `System.Object`, `LuaSystemAPI_GUI`.

Executor được thử namespace ứng viên rồi quét toàn bộ class của Assembly-CSharp theo simple name; chỉ nhận class có `get_Instance()` và `ExecuteScriptFunction(3)`.

Geometry resolver cũ vẫn được giữ để bảo toàn/chẩn đoán, nhưng không còn là active item-cell dispatcher.

## Ưu tiên toàn cục

1. P1 XN Lâu Lan: điều kiện route ownership/stall của v0.5 giữ nguyên; action đổi sang MessageBox callback.
2. P2 Đầu thai: giữ death lifecycle/World Flow FIFO; action đổi sang death-guarded UIButton callback.
3. P3 AUTO: một Attack request giữ trọn hai pha điểm `AUTO → ĐÁNH QUÁI`; chỉ publish completion sau click 2. Stop phát điểm `DỪNG AUTO 2`. Cả hai đi qua `ClickInternalPoint`/InputSync và được snapshot `AutoFight` xác minh sau action.

AUTO, item và trade đều không tạo mouse input. F8/REC chỉ quan sát thao tác tay để lưu tọa độ; low-level mouse guard và đường injection vật lý đã bị xóa.

## Hidden trade + route invariant

- `CoordinatorInternalPointAction` là primitive chung cho test macro, test trade và runtime trade.
- Mỗi dòng gửi đúng một Bridge request, giữ delay/repeat/group/FIFO ở controller và giữ stable `FreeBagSpace` làm business proof cuối pass.
- Sequence lease chỉ bảo vệ thứ tự nghiệp vụ, không foreground hoặc sở hữu con trỏ.
- `StartPath` luôn qua Travel Guard: Stop #1 → Stop #2 → AUTO/Attack reset → lặp Stop cho tới snapshot OFF.
- Request AUTO/Attack được kiểm tra AutoPath authoritative cả lúc xếp hàng lẫn ngay trước dispatch; request cũ bị hủy nếu route đã bật hoặc state không đọc được.
- Mailbox request/result mang owner Train, Travel Guard hoặc Mount Recovery; cùng slot Attack nhưng khác FSM không thể consume nhầm.
- Nếu phát hiện `AutoPath ON + AutoFight ON`, conflict latch dừng path trước, chạy lại Travel Guard và chỉ clear sau snapshot `AutoPath OFF + AutoFight OFF`.

## Auto-sell

- Quyết định khi nào bán, đi NPC, quay bãi, retry hai pass và ngưỡng MAIN/CON giữ từ v0.5.
- Bridge vẫn nhận dạng các semantic stage qua `Trang bị`; controller delay giữa các stage để không block game thread.
- Riêng item stage dùng tọa độ dòng 5 (hoặc dòng cuối/duy nhất), đổi sang tọa độ Unity và gọi một click trái hoàn chỉnh qua `InputSyncManager`: press `TryClickUI`, xác minh `_uiDragging`, release `EndUIDrag`.
- Vòng đầu 90 callback; sau sale hoàn tất, `FreeBagSpace` ổn định được lưu vào `sellStep5LearnedRepeat` cho vòng sau, cap 90.
- Không dò/cache custom control tại điểm item; không tạo physical mouse input.
- Completion vẫn chỉ được kết luận sau `FreeBagSpace > 0` ổn định 1,5 giây.

## Logic được bảo vệ

World Flow observation/recovery, mount recovery, rotation, F4/F8, trade FIFO/relock/pass/bag stabilization và cấu hình trade macro được giữ nguyên. Chỉ transport của trade đổi từ chuột Windows sang InputSync; route guard được siết thêm conflict latch.

## Rủi ro còn cần test thật

- Mã Kiêu Minh ID 373 là donor đã chứng minh. Dược Đại Phu ID 279 có thể dùng cây shop khác.
- UI scorer phụ thuộc metadata/text của đúng client.
- Game minimized có thể tự dừng update dù action không chiếm chuột.
- Điểm AUTO phụ thuộc layout/profile F8; sai điểm có thể raycast nhầm UI. Bắt buộc test từng điểm trước khi bật nhiều PID.
- Giao dịch MAIN/CON hidden point vẫn phụ thuộc điểm F8/layout và cần test popup/overlay trên client thật.
- `WH_GETMESSAGE` hiện vẫn gọi mutation trực tiếp trong hook. v0.6.1.5 tái sử dụng dispatcher đã runtime-pass; chưa tuyên bố giải quyết MainThread Action boundary dài hạn.

## Test tiếp theo bắt buộc

1. Dùng đúng cặp EXE/DLL v0.6.1.5; protocol không cho trộn Bridge cũ.
2. F8 lại đúng ba điểm `AUTO`, `ĐÁNH QUÁI`, `DỪNG AUTO 2` nếu layout/độ phân giải đã đổi.
3. Bấm TEST từng điểm trên một PID; xác nhận không foreground/di chuyển chuột.
4. Chạy AutoFight OFF tại đúng bãi; log phải có click `1/2 AUTO`, click `2/2 ĐÁNH QUÁI`, rồi `verify AutoFight ON`.
5. Khi Travel Guard cần đi, xác nhận điểm Dừng làm `AutoFight OFF` trước `StartPath`.
6. Chưa bật hàng loạt account trước khi một PID qua đủ Start, Stop và một vòng bán.
7. Test một pass giao dịch MAIN/CON khi cursor đang ở ứng dụng khác; cursor phải không đổi và `FreeBagSpace` MAIN phải verify đúng.
8. Cố tình tạo trạng thái Path ON + Fight ON; log phải StopPath, DỪNG x2/reset nếu cần và không được phát StartPath mới trước khi cả hai OFF.

## Knowledge index

- `docs/bugs/BUG_REGISTRY.md` — BUG-001.
- `docs/evidence/EVIDENCE_REGISTRY.md` — EVID-001.
- `docs/decisions/DECISIONS.md` — DEC-001.
- `docs/features/BACKGROUND_UI_ACTIONS.md`.
- `docs/history/VERSION_v0.6.1.md`.
- `docs/history/VERSION_v0.6.1.1.md`.
- `docs/history/VERSION_v0.6.1.2.md`.
- `docs/history/VERSION_v0.6.1.3.md`.
- `docs/history/VERSION_v0.6.1.4.md`.
- `docs/history/VERSION_v0.6.1.5.md`.
