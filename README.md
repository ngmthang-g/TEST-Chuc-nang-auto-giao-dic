# Thần Long — TEST Chức năng Auto Giao Dịch

Repo này chỉ dùng để kiểm chứng **một chức năng duy nhất**:

> Tool có thể tự động quét người đang ở gần, target đúng người bằng `RoleID`, gửi yêu cầu giao dịch bằng internal/semantic game path và xác nhận bảng giao dịch có mở hay không.

Không phát triển Auto Train, Auto Sell, Auto Buff, trị liệu, revive, chuyển đồ hay tính năng khác trong repo này.

## Trạng thái hiện tại — v0.1.1

v0.1.0 build được nhưng runtime test thực tế trả:

```text
PROBE FAIL: Bridge timeout
TEAM SCAN FAIL: Bridge timeout
```

### Nguyên nhân kiến trúc của v0.1.0

v0.1.0 đã đặt `LuaEnv.DoString(...)` trực tiếp trong request callback của `WH_GETMESSAGE` bridge. `completedSeq` chỉ được ghi sau khi Lua chunk trả về. Vì vậy nếu `DoString` mắc/block trong callback, controller chỉ thấy timeout và không biết request đã vào bridge đến đâu.

### v0.1.1 sửa theo hướng fail-fast + semantic direct

`LuaEnv.DoString` đã bị loại khỏi source.

Luồng mới:

```text
TEST BRIDGE
 -> chỉ proof hook + shared memory + callback thread
 -> KHÔNG gọi IL2CPP/Lua action

QUÉT NGƯỜI GẦN
 -> proof Unity main thread
 -> direct IL2CPP GetNearByPeacePlayers(32)
 -> copy RoleID + Name ra snapshot của tool

TARGET
 -> Game.SelectTarget(RoleID)
 -> đọc Game.SelectedTarget
 -> chỉ PASS khi SelectedTarget.RoleID == RoleID

TRADE
 -> target + proof lại RoleID
 -> LuaSystemManager -> LuaEnv -> Global LuaTable
 -> Global["C_OtherRoleCommand"]["Trade"]
 -> Global["C_TradeCommand"]["Request"]
 -> sanity guard Trade == 7
 -> direct LuaSystemAPI_Network.SendPacket(200051, "Trade:Request:RoleID")

TRADE UI
 -> direct GUI.FindUI/MainFindUI candidate semantic names
```

Numeric `C_TradeCommand.Request` **không hardcode và không đoán**. v0.1.1 đọc giá trị runtime từ LuaTable hiện hành của chính client.

## Vì sao đổi “Quét đồng đội” thành “Quét người gần”

Knowledge base đã VERIFIED stock UI dùng:

```text
Game.GetNearByPeacePlayers(limit)
```

và mỗi record có ít nhất `RoleID`, `Name`, `Level`, `FactionID`, `HP`, `MaxHP`, `GuildName`, `AvartaID`, `TeamRank`.

Stock UI cũng dùng chính `Game.SelectTarget(RoleID)`.

Do đó test target/trade không cần ép hai nhân vật phải ở cùng tổ đội; điều kiện quan trọng là nhân vật đích đang trong AOI/client hiện biết tới.

## Stage telemetry v0.1.1

Nếu request vẫn timeout, log không còn chỉ ghi `Bridge timeout` mà kèm:

```text
loaded=<0/1>
busy=<0/1>
callbackSeq=<n>
requestSeq=<n>
stage=<NAME>(<number>)
```

Stage:

| Stage | Ý nghĩa |
|---|---|
| `IDLE` | request chưa vào callback |
| `HOOK_ENTERED` | hook đã thấy wake message |
| `REQUEST_ACCEPTED` | bridge đã nhận request |
| `IL2CPP_READY` | IL2CPP exports/resolver đã sẵn sàng |
| `MAINTHREAD_PROOF` | đang/chưa qua Unity main-thread proof |
| `SCAN_PLAYERS` | đang gọi/đọc nearby player collection |
| `SELECT_TARGET/TRADE` | đang target/verify target |
| `READ_TRADE_CONSTANTS` | đang đọc `Trade` + `Request` từ LuaTable |
| `SEND_TRADE_PACKET` | đang gọi direct `SendPacket(200051, payload)` |
| `QUERY_TRADE_UI` | đang kiểm tra Trade UI |
| `RESPONSE_READY` | bridge đã hoàn tất response |

Điều này cho phép lần test kế tiếp xác định chính xác tầng lỗi mà không retry/spam action.

## Cách build

Yêu cầu Zig 0.14.1 trong PATH:

```bat
build.cmd
```

Output v0.1.1:

```text
dist/ThanLongAutoTradeTest_v0.1.1.exe
dist/ThanLongAutoTradeTestBridge.dll
```

GitHub Actions upload artifact:

```text
ThanLong-AutoTrade-Test-v0.1.1
```

## Cách test v0.1.1

1. Mở hai nhân vật và cho đứng gần nhau để cùng AOI.
2. Chạy `ThanLongAutoTradeTest_v0.1.1.exe` cùng mức quyền với game.
3. Chọn đúng PID.
4. Bấm `TEST BRIDGE`.

PASS bắt buộc:

```text
HOOK + SHARED MEMORY PASS
HOOK_READY|NO_DOSTRING
```

Nếu bước này timeout thì lỗi vẫn nằm ở hook/wake/shared-memory, chưa liên quan IL2CPP/Game/Lua.

5. Bấm `QUÉT NGƯỜI GẦN`.
6. Chọn RoleID cần test.
7. Bấm `1. TARGET NỘI BỘ`.
8. Khi target PASS, bấm `2. TARGET + GỬI GIAO DỊCH`.
9. Xem acc bên kia có nhận lời mời giao dịch không; nếu có, chấp nhận để kiểm tra bảng Trade.

### PASS target

```text
TARGET PASS: OK|TARGET=<RoleID>|NAME=<Name>
```

### PASS gửi request

```text
TRADE REQUEST PASS: OK|TARGET=<RoleID>|PACKET=200051|PAYLOAD=7:<runtime Request>:<RoleID>
```

### PASS bảng giao dịch

Nếu candidate semantic UI name hiện tại khớp:

```text
TRADE UI PASS: OPEN|<ui-name>
```

Nếu game đã mở bảng giao dịch thật nhưng tool vẫn báo UI CLOSED thì target/request vẫn có thể đã PASS; lúc đó chỉ còn targeted trace `CMD_TRADE_DATA = 200053 -> exact Trade UI/script name`.

## Bằng chứng client dùng cho PoC

Nguồn nghiên cứu: `ngmthang-g/clinent-game-than-long-DATA-2222`.

Đã VERIFIED:

- `Game.GetNearByPeacePlayers(limit)` cung cấp structured nearby-player records.
- `Game.SelectTarget(RoleID)` là stock target path.
- `Game.SelectedTarget.RoleID` là target proof.
- `C_OtherRoleCommand.Trade = 7`.
- `CMD_OTHER_ROLE_COMMAND = 200051`.
- Lua gốc gửi lời mời giao dịch theo form `C_OtherRoleCommand.Trade:C_TradeCommand.Request:RoleID`.
- `CMD_TRADE_DATA = 200053` tồn tại trong packet table.

## Scope cố định

Repo này **không** tự:

- chấp nhận giao dịch ở acc bên kia;
- thêm item/tiền;
- khóa giao dịch;
- xác nhận giao dịch;
- hoàn thành/chuyển đồ;
- spam lời mời;
- điều khiển chuột/phím.

Sau khi xác minh được `nearby player -> target -> request -> Trade UI open`, chức năng test coi như hoàn thành.

## Tài liệu kỹ thuật

Xem `docs/TRADE_RESEARCH.md` để biết evidence, giả định, điểm chưa VERIFIED và tiêu chí PASS/FAIL chi tiết.
