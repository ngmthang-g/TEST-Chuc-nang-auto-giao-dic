# Phân tích cơ chế thao tác nền của donor v0.8.4 và cách tích hợp vào nền v0.5.0

## Kết luận ngắn

Các thao tác người dùng thấy “chạy ẩn” trong v0.8.4 không phải auto-click tọa độ, `PostMessage` hay thủ thuật trả chuột về vị trí cũ. Donor gọi trực tiếp API/callback IL2CPP mà chính client dùng khi người chơi bấm UI. Vì Windows không phát sinh mouse input nên cửa sổ không cần foreground và con trỏ không bị chiếm.

v0.6 giữ nguyên controller/FSM/business rules của v0.5, nhưng chuyển các action phù hợp sang Bridge DLL hiện hữu. Không mang nguyên `RemoteExecutor` của donor sang.

> Correction v0.6.1: donor gọi `MonoBehaviourExecutor` bằng native RVA; donor không chứng minh namespace managed `FGStudio.LuaSystem` mà v0.6 đã giả định. Runtime v0.6 cho thấy readiness gate gộp làm XN và Sell cùng fail trước khi scan control. v0.6.1 tách Button/Toggle discovery khỏi Executor và chỉ nhận Executor tìm được bằng metadata nếu có đúng method surface.

## 1. Kênh thực thi của donor v0.8.4

Donor mở process game với quyền đọc/ghi/tạo thread, cấp phát hai vùng nhớ từ xa và tạo một worker bằng `CreateRemoteThread`:

- vùng code chứa `remote_worker.S`;
- vùng packet chứa địa chỉ hàm, tối đa 5 tham số, kết quả và cờ lệnh;
- worker attach vào IL2CPP domain một lần rồi chờ packet;
- mỗi call được serialize bằng mutex; packet cũ chưa hoàn tất thì tuyệt đối không ghi đè;
- timeout không đồng nghĩa hàm đã dừng, nên donor giữ packet bận để tránh race/crash.

Trước khi gọi RVA, donor khóa đúng client build bằng PE timestamp, `SizeOfImage` và chữ ký byte của nhiều hàm. Điều này giảm rủi ro gọi nhầm địa chỉ nhưng làm source phụ thuộc đúng bản `GameAssembly.dll` đã nghiên cứu.

## 2. Donor tìm và gọi control UI như thế nào

Donor resolve các class `UIObject`, `UIButton`, `UIToggle`, `UIRectTransform`, `UIInput` và static field `UIObject.instances`. Với từng object còn sống và đang active/interactable, nó đọc tên control, text hiển thị, Lua pointer-click handler và tên control cha/con để xác định đúng ngữ cảnh.

| Loại control | Cách donor kích hoạt | Ý nghĩa |
|---|---|---|
| `UIButton` | `UIButton.HandleClickEvent()` | Gọi đúng callback như click nút của game |
| `UIToggle` | set `Selected=true`, cần thì gọi `HandleSelectEvent(true)` | Chọn tab/toggle mà không tạo mouse event |
| `UIRectTransform` có Lua handler | tạo `object[3]`, rồi gọi `MonoBehaviourExecutor.ExecuteScriptFunction` | Chạy Lua callback gắn với vùng UI |

Nó không click chỉ vì một chuỗi gần giống. Candidate được chấm điểm; nếu hai control đồng hạng thì fail-closed.

## 3. Chuỗi trị liệu của v0.8.4

Luồng thực tế:

1. Xác minh NPC gần nhất đúng tên NPC đã chọn.
2. Dùng `ResID` chuẩn, không dùng runtime `RoleID`, rồi gọi `LuaSystemAPI_Game.ClickNPC(ResID)`.
3. Chờ control có vai trò `Trị liệu`, sau đó gọi callback nội bộ.
4. Chờ control `Xác nhận`, gọi callback.
5. Chờ control `Ta biết rồi`, gọi callback.
6. Dò và đóng các lớp shop/tay nải liên quan.

NPC built-in `Đỗ Thanh Đằng` dùng ResID 339 theo dữ liệu client. Mỗi bước được resolve lại sau khi UI đổi; donor không giữ pointer control transient qua nhiều màn hình.

Điểm đúng cần học là “mỗi bước = phát hiện state UI mới → gọi một callback → chờ state kế tiếp”, không phải bê nguyên các delay hoặc pointer.

## 4. Chuỗi bán đồ của v0.8.4

Luồng mở UI:

1. Xác minh đúng NPC và gọi `ClickNPC(373)` cho Mã Kiêu Minh.
2. Gọi control `Mua thú cưỡi` nếu shop chưa mở.
3. Gọi `Bán vật phẩm`.
4. Bật `Bán vật phẩm nhanh`.
5. Chọn tab `Trang bị`.

Luồng bán item:

- Chỉ nhận control có tên giống item/slot/cell và có cây cha thuộc bag/inventory/package/item-grid.
- Loại control nằm trong product/shop/NPC item list để tránh mua hoặc tác động nhầm hàng của NPC.
- Sau mỗi callback, đọc `GetFreeBagSpace`. Chỉ khi số ô trống tăng mới tính là đã bán.
- Một item không tạo tiến triển sau 3 lần bị bỏ qua.
- Sau mỗi item đã bán, quét lại từ đầu vì UI có thể tái sử dụng/reorder cell.
- Chặn cứng 90 callback, sau đó đóng các lớp shop/tay nải.

Đây là khác biệt căn bản với macro v0.5: số lần thao tác được quyết định bởi trạng thái thực, không phải “bước 5 lặp N lần”.

## 5. Xác nhận ra map của v0.8.4

Donor chỉ thao tác khi `MessageBox` đang tồn tại. Nó thu các button trong đúng cây MessageBox, tăng điểm cho `Đồng ý/Xác nhận/OK/Yes`, trừ mạnh cho `Hủy/Không/Close`, rồi yêu cầu candidate thắng duy nhất. Sau đó gọi `UIButton.HandleClickEvent()`.

Controller donor còn giữ các gate ngoài UI: map sẵn sàng, không chờ chuyển map, đã tới/stall ở cổng và có thời gian retry. Callback thành công chỉ có nghĩa action đã được gửi; state machine tiếp tục đợi `WaitingChangeMap`/map transition để xác nhận kết quả nghiệp vụ.

## 6. AUTO → Đánh quái và Dừng AUTO

Donor không còn mở menu AUTO rồi quét label con. Từ Lua asset của client, nó dùng đúng quick action:

- `TopIcon.AutoTrainClick()` → `AutoFight_Main.StartAutoFight(Train)`;
- `TopIcon.AutoStopClick()` → `AutoFight_Main.StartAutoFight(None)`.

Một chi tiết dễ làm ngược: `Game.EnableAutoF1=false` nghĩa train đang bật; khi dừng, giá trị trở lại `true`. Donor gọi action trước rồi đọc lại flag nhiều lần để xác minh.

> Runtime correction v0.6.1.4: contract Lua trên vẫn đúng theo source client, nhưng Bridge hiện tại không resolve được named Lua UI/action trên client test. Theo yêu cầu người dùng, active P3 tạm dùng dispatcher InputSync đã runtime-pass: điểm `AUTO` → điểm `ĐÁNH QUÁI`, hoặc điểm `DỪNG AUTO 2`; snapshot `AutoFight` vẫn là bằng chứng. Đây là fallback runtime có phạm vi, không phủ nhận semantic contract và không biến click-return thành business proof.

> Runtime correction v0.6.1.5: generic InputSync point action được dùng tiếp cho toàn bộ test/runtime trade MAIN/CON. Đường foreground, di con trỏ, `SendInput` và User Mouse Guard đã bị xóa khỏi controller. Mọi `StartPath` vẫn đi qua Travel Guard; ngoài ra snapshot `AutoPath ON + AutoFight ON` được latch fail-closed: dừng path trước, sau đó DỪNG AUTO hai lần, nếu chưa OFF thì bật lại một lần để reset rồi lặp cho tới khi cả Path và Fight đều OFF.

## 7. Đầu thai

Ngay trước action, donor đọc `IsDeath`. Nếu false hoặc không đọc được thì không gọi gì. Khi true, nó tìm duy nhất button `Đầu thai` đang active/interactable và gọi `UIButton.HandleClickEvent()`. Sau đó state machine chờ `IsDeath` tắt ổn định thay vì tin mù vào return value của callback.

## 8. Vì sao không bê nguyên donor vào v0.5

Nền v0.5 đã có Bridge DLL được nạp bằng `WH_GETMESSAGE` và xử lý request trên đúng message thread của từng game. Nếu ghép thêm worker `CreateRemoteThread` của v0.8.4 sẽ tạo hai kênh mutable action, hai cơ chế timeout và hai nơi có thể gọi IL2CPP chồng nhau.

| Hạng mục | Donor v0.8.4 | Tích hợp v0.6 |
|---|---|---|
| Nơi chạy action | remote IL2CPP worker | Bridge trên game message thread |
| Delay chuỗi | worker/FSM donor | controller v0.5 giữ state/delay |
| XN | quét cây MessageBox + callback | cùng nguyên tắc, bắt buộc ancestor MessageBox |
| Revive | check chết + callback | Bridge check chết lần cuối + callback |
| AUTO | Lua quick action | active fallback InputSync point sequence, snapshot v0.5 xác minh |
| Sell | vòng lặp trong donor worker | một semantic action mỗi request; không block UI thread |
| Tọa độ click cũ | không cần | dùng có chủ đích cho AUTO/trade generic point action; F8/REC chỉ ghi cấu hình |

## 9. Các điểm donor chưa nói rõ hoặc có thể gây lỗi

1. `UIObject.instances` là Dictionary. Donor lặp `0..Count-1`; sau khi Dictionary có bucket bị xóa, live entry có thể nằm sau chỉ số `Count-1`. v0.6 quét toàn bộ entries capacity hợp lệ, giới hạn 32768.
2. Chuỗi shop `Mua thú cưỡi` được chứng minh với Mã Kiêu Minh. Preset Dược Đại Phu ID 279 của v0.5 có thể có cây UI khác; v0.6 có probe bỏ qua bước đã mở, nhưng vẫn cần test thật.
3. Return thành công của callback không bảo đảm server đã bán/đầu thai/chuyển map. Vì vậy v0.6 giữ verify bằng `FreeBagSpace`, `IsDeath`, AutoFight và map transition.
4. Nhận dạng UI vẫn phụ thuộc tên/text/handler của client. Bản game hoặc ngôn ngữ khác có thể làm scorer không tìm thấy; hành vi đúng khi đó là dừng, không click mù.
5. Scan toàn bộ UI trên message thread có thể tạo một nhịp trễ ngắn. v0.6 chỉ gọi một action mỗi request và có hard cap, không chạy vòng `Sleep` dài trong Bridge.
6. Tự bán chỉ kích hoạt khi logic v0.5 quyết định túi đạt ngưỡng. Việc thay input không tự sửa mọi quyết định nghiệp vụ về MAIN/CON; phần đó được giữ nguyên theo yêu cầu.

## 10. Ma trận test runtime ưu tiên

1. Đang thao tác chuột ở cửa sổ khác, kích hoạt XN Lâu Lan: cursor không đổi, đúng acc chuyển map.
2. Một acc chết trong khi acc khác đang chạy giao dịch: đúng acc chết đầu thai; trade giữ nguyên step/repeat và không di chuyển cursor.
3. Ở đúng bãi, AutoFight OFF: một action nội bộ bật train, không popup AUTO, snapshot chuyển ON.
4. Đang AutoFight rồi bắt đầu route: phát điểm `DỪNG AUTO 2` tối đa hai lần; vẫn ON thì phát AUTO → Đánh quái reset một lần rồi lặp, snapshot OFF rồi mới `StartPath`.
5. Mã Kiêu Minh với túi full: mở đủ 4 semantic stage, FreeBagSpace tăng sau từng món, UI đóng, quay bãi.
6. Một món không bán được: thử tối đa 3 lần rồi bỏ, không kẹt vô hạn.
7. Dược Đại Phu ID 279: xác định cây UI có tương thích hay cần role riêng ở version sau.
8. Cửa sổ game minimized/background: action nội bộ vẫn không cần foreground; nếu game tự pause render/update khi minimized thì đây là hạn chế của client, không phải chiếm chuột.
