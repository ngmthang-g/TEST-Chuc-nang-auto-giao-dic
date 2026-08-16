# Thần Long — TEST Chức năng Auto Giao Dịch

Repo này chỉ dùng để kiểm chứng **một chức năng duy nhất**:

> Tool có thể tự động đọc đồng đội, target đúng người bằng `RoleID`, gửi yêu cầu giao dịch bằng internal/semantic game path và nhận biết bảng giao dịch có mở hay không.

Không phát triển Auto Train, Auto Sell, Auto Buff, trị liệu, revive, chuyển đồ hay tính năng khác trong repo này.

## Mục tiêu test v0.1.0

Luồng thử nghiệm:

```text
quét client GameAssembly.dll
 -> attach bridge vào game thread
 -> chứng minh Unity main thread + LuaEnv
 -> đọc C_TeamData.TeamMember
 -> chọn một đồng đội
 -> Game.SelectTarget(RoleID)
 -> xác nhận Game.SelectedTarget.RoleID == RoleID
 -> đọc C_OtherRoleCommand.Trade và C_TradeCommand.Request trực tiếp từ Lua runtime
 -> Network.SendPacket(200051, Trade:Request:RoleID)
 -> acc bên kia chấp nhận yêu cầu giao dịch
 -> tool tự poll GUI.FindUI(...) để tìm Trade UI
```

**Không dùng tọa độ màn hình, SendInput, mouse_event, UIButton.HandleClickEvent hay click chuột giả lập.**

## Bằng chứng client đã dùng để xây PoC

Nguồn nghiên cứu: `ngmthang-g/clinent-game-than-long-DATA-2222`.

Đã VERIFIED trong knowledge base:

- `C_TeamData.TeamMember[]` có `RoleID`, `RoleName`, `MapID`, HP/MaxHP và tọa độ dự phòng.
- UI tổ đội chọn thành viên bằng `Game.SelectTarget(RoleID)`.
- `Game.SelectedTarget.RoleID` dùng để xác nhận target hiện tại.
- `C_OtherRoleCommand.Trade = 7`.
- packet social action là `CMD_OTHER_ROLE_COMMAND = 200051`.
- Lua gốc gửi lời mời giao dịch theo form `C_OtherRoleCommand.Trade:C_TradeCommand.Request:RoleID`.
- `CMD_TRADE_DATA = 200053` tồn tại trong packet table, nhưng inbound Trade UI lifecycle chưa được knowledge base hiện tại mô tả đủ để hardcode.

Điểm cố ý **không hardcode**: numeric value của `C_TradeCommand.Request`. Tool đọc constant này từ Lua runtime của chính client trước khi gửi request.

## Cách build

Yêu cầu Zig 0.14.1 trong PATH, sau đó chạy:

```bat
build.cmd
```

Output:

```text
dist/ThanLongAutoTradeTest_v0.1.0.exe
dist/ThanLongAutoTradeTestBridge.dll
```

GitHub Actions cũng tự build và upload artifact `ThanLong-AutoTrade-Test-v0.1.0` sau mỗi push lên `main`.

## Cách test

1. Mở game và vào tổ đội với ít nhất một nhân vật khác.
2. Hai nhân vật nên đứng gần nhau/cùng vùng AOI để `Game.SelectTarget(RoleID)` có target object hiện tại.
3. Chạy `ThanLongAutoTradeTest_v0.1.0.exe` cùng mức quyền với game.
4. `QUÉT CLIENT` và chọn PID cần test.
5. Bấm `TEST BRIDGE`.
   - PASS mong đợi: bridge + Unity main thread + LuaEnv.
   - log sẽ in actual runtime `Trade=...` và `Request=...`.
6. Bấm `QUÉT ĐỒNG ĐỘI` và chọn người cần giao dịch.
7. Có thể bấm `1. TARGET NỘI BỘ` trước để test riêng target.
8. Bấm `2. TARGET + GỬI GIAO DỊCH`.
   - tool target lại đúng RoleID;
   - chỉ khi target proof PASS mới gửi packet 200051.
9. Ở acc bên kia, chấp nhận lời mời giao dịch nếu game hiển thị.
10. Tool tự check Trade UI khoảng 30 giây; cũng có thể bấm `CHECK BẢNG TRADE` thủ công.

## Cách đọc kết quả

### PASS target

```text
TARGET PASS: OK|TARGET=<RoleID>|NAME=<Name>
```

Điều này chứng minh auto target semantic hoạt động, không dựa vào click người trên màn hình.

### PASS gửi request

Log dạng:

```text
OK|TARGET=<RoleID>|PACKET=200051|PAYLOAD=<Trade>:<Request>:<RoleID>
```

Điều này chứng minh client-side path đã target đúng người và gọi `Network.SendPacket` với constants lấy trực tiếp từ Lua runtime.

### PASS bảng giao dịch

```text
TRADE UI PASS: OPEN|<semantic-ui-name>
```

Đây là PASS cuối cho mục tiêu của repo.

### Timeout không đồng nghĩa packet chắc chắn fail

Nếu sau 30 giây chưa phát hiện UI, cần phân biệt:

- acc kia chưa chấp nhận/từ chối;
- server từ chối request vì khoảng cách/trạng thái giao dịch;
- request đã đúng nhưng tên Trade UI thực tế chưa nằm trong candidate list;
- Trade UI lifecycle dùng `CMD_TRADE_DATA=200053` nhưng cần thêm một targeted runtime trace để xác định exact UI/script name.

Không được coi `Sleep`/timeout là proof thành công hoặc thất bại của server.

## Scope cố định

Repo này **không** tự:

- chấp nhận giao dịch ở acc bên kia;
- thêm item/tiền;
- khóa giao dịch;
- xác nhận giao dịch;
- hoàn thành/chuyển đồ;
- spam lời mời;
- điều khiển chuột/phím.

Sau khi xác minh được `target -> request -> Trade UI open`, chức năng test coi như hoàn thành.

## Tài liệu kỹ thuật

Xem `docs/TRADE_RESEARCH.md` để biết evidence, giả định, điểm chưa VERIFIED và tiêu chí PASS/FAIL chi tiết.
