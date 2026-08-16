# Auto Trade Research — Target + Open Trade UI PoC

Status meanings:

- **VERIFIED**: recovered from shipped client Lua/API/packet knowledge in the frozen data repo.
- **RUNTIME PROOF**: this PoC is intended to prove it live.
- **UNKNOWN/TARGETED**: not enough exact evidence yet; do not guess.

## 1. Exact scope

Only prove:

```text
team member -> RoleID -> select target -> send trade request -> observe trade UI open
```

No item/money/accept/confirm/complete-trade automation.

## 2. VERIFIED target path

From `analysis/25_TEAM_RUNTIME_FOLLOW.md` in the data repo:

```text
C_TeamData.TeamMember[]
  RoleID
  RoleName
  Level
  FactionID
  MapID
  Hp
  MaxHp
  AvartaID
  PosX
  PosY
```

The shipped team UI selects a member with:

```text
Game.SelectTarget(RoleID)
```

From `analysis/14_NEARBY_ENTITY_UI_SCHEMA.md`, `Game.SelectedTarget` is a semantic object and exposes `RoleID`/`Name` among other fields.

PoC rule:

```text
Game.SelectTarget(expectedRoleID)
 -> fresh Game.SelectedTarget
 -> require SelectedTarget.RoleID == expectedRoleID
```

Do not send Trade Request if this proof fails.

## 3. VERIFIED trade invitation request construction

From `analysis/16_PLAYER_INTERACTION_UI_API.md`:

```text
C_OtherRoleCommand.Trade = 7
CMD_OTHER_ROLE_COMMAND = 200051
```

Shipped Lua constructs Trade invitation as:

```text
C_OtherRoleCommand.Trade : C_TradeCommand.Request : RoleID
```

Therefore request shape is VERIFIED, but the current compact knowledge base does not record the numeric value of `C_TradeCommand.Request`.

### PoC decision

Do **not** guess the Request number.

The bridge resolves the live `LuaSystemManager.LuaEnv` and evaluates a fixed diagnostic/action chunk which reads:

```lua
C_OtherRoleCommand.Trade
C_TradeCommand.Request
```

from the client that is actually running.

The final send is performed inside the game's Lua environment:

```lua
local payload = tostring(C_OtherRoleCommand.Trade)
  .. ':' .. tostring(C_TradeCommand.Request)
  .. ':' .. tostring(RoleID)

Network.SendPacket(200051, payload)
```

This keeps enum authority in the shipped runtime rather than duplicating an unverified number in the external tool.

## 4. VERIFIED packet vocabulary relevant to Trade

From `database/PACKET_IDS.csv`:

```text
CMD_OTHER_ROLE_COMMAND = 200051
CMD_TRADE_DATA         = 200053
```

Only `200051` request construction above is source-verified for the invitation.

The existence of `CMD_TRADE_DATA=200053` does **not** by itself prove direction, payload or exact Trade UI name.

## 5. Main-thread execution boundary

The donor foundation comes from the separate AutoFight feature-test repo and is reused only as execution plumbing:

```text
controller
 -> shared memory request
 -> WH_GETMESSAGE hook callback in target game window thread
 -> verify UnitySynchronizationContext
 -> verify managed current thread == Unity main thread
 -> resolve live LuaEnv
 -> execute one fixed semantic Lua operation
```

The Auto Trade repo does not carry AutoFight behavior.

World mutation also checks:

```text
Game.IsMapReady()
SessionData.get_WaitingChangeMap()
```

before target/trade actions.

## 6. Runtime team scan

PoC asks the live Lua runtime for `C_TeamData.TeamMember`, excludes the local `Game.RoleData.RoleID`, and serializes only:

```text
RoleID \t RoleName \t MapID
```

The controller stores values, not Lua object pointers.

## 7. Trade UI proof — current boundary

The frozen knowledge base has not yet documented an exact `CMD_TRADE_DATA -> GUI.CallUI("...")` lifecycle comparable to NPCShop/GameDialog.

Therefore v0.1.0 uses a conservative UI observer after the other account accepts:

```lua
GUI.FindUI(candidateName)
```

Candidate names in v0.1.0:

```text
Trade
Trade_Main
TradeFrame
TradeWindow
TradeUI
PlayerTrade
RoleTrade
```

This is intentionally marked **RUNTIME PROOF**, not VERIFIED static knowledge.

If the invitation visibly reaches the other account and trade opens in game but the tool reports CLOSED, the next research task is narrow:

```text
trace inbound CMD_TRADE_DATA=200053
 -> exact TCPCmdHandler branch
 -> exact GUI.CallUI/FindUI script name
```

Do not broad-reverse the client again.

## 8. Test matrix

| Stage | Expected proof | Interpretation |
|---|---|---|
| Bridge | `LUA_OK|Trade=...|Request=...` | Unity main thread + live LuaEnv + constants available |
| Team scan | one or more RoleID/RoleName rows | `C_TeamData` runtime access works |
| Target | `OK|TARGET=<RoleID>` | semantic auto target works |
| Trade request | `PACKET=200051|PAYLOAD=...` | client-side invitation send path executed |
| Remote invite | other account receives invitation | server accepted/forwarded request path |
| UI | `OPEN|<name>` | final repo goal PASS |

## 9. Failure interpretation

### Team list empty

Possible causes:

- not currently in a team;
- only local member visible in team state;
- `C_TeamData` not initialized yet.

### Target fails

Most likely test target is not currently represented in local AOI/targetable world state even though team metadata exists. Put both characters close together and retest.

### Trade request executes but no remote invitation

This is a server/state acceptance question. Do not solve it by repeating packets quickly. Check distance/busy/trade restrictions and capture one runtime result.

### Remote invitation works but UI observer says CLOSED

This strongly narrows the remaining gap to exact Trade UI/script naming or inbound lifecycle, not target/request construction.

## 10. Safety/reliability rules for this PoC

- one explicit button press -> at most one Trade Request;
- no automatic retry/spam;
- no stale RoleID from a previous PID after client rescan;
- no target request during map transition;
- no mouse/keyboard macro fallback;
- no calling inbound/update handlers to fake Trade state;
- timeout is only a guard, never success proof.

## 11. Source references in frozen data repo

Primary references:

```text
analysis/14_NEARBY_ENTITY_UI_SCHEMA.md
analysis/16_PLAYER_INTERACTION_UI_API.md
analysis/25_TEAM_RUNTIME_FOLLOW.md
database/API_QUICK_REFERENCE.md
database/PACKET_IDS.csv
database/UI_PACKET_LIFECYCLE.md
analysis/13_UI_RUNTIME_ACTION_SURFACE.md
```
