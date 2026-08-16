@echo off
setlocal
cd /d "%~dp0"
where zig >nul 2>nul
if errorlevel 1 (echo KHONG TIM THAY ZIG & exit /b 1)
if not exist dist mkdir dist
del /q dist\* >nul 2>nul

echo [1/6] Auto Trade scope + semantic audit...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$ErrorActionPreference='Stop'; $files=(Get-ChildItem 'src' -Recurse -File | Where-Object {$_.Extension -in '.cpp','.h','.inl'} | Select-Object -ExpandProperty FullName); $s=($files|%%{Get-Content $_ -Raw -Encoding UTF8}) -join [Environment]::NewLine;" ^
  "$forbidden=@('AutoFight','NPCShop','RequestSellItem','CMD_NPC_SHOP_SELL_REQUEST','CMD_REVIVE_DATA','RequestUsingSkill','UIButton','HandleClickEvent','CreateRemoteThread','WriteProcessMemory'); foreach($x in $forbidden){if($s -match [regex]::Escape($x)){throw ('Forbidden unrelated feature token: '+$x)}};" ^
  "foreach($rx in @('SendInput\s*\(','mouse_event\s*\(','SetCursorPos\s*\(','keybd_event\s*\(')){if($s -match $rx){throw ('Forbidden visual/input API: '+$rx)}};" ^
  "$lua=Get-Content 'src/bridge_lua.inl' -Raw -Encoding UTF8; foreach($x in @('C_TeamData.TeamMember','Game.SelectTarget','C_OtherRoleCommand.Trade','C_TradeCommand.Request','Network.SendPacket(200051','LuaEnv','DoString')){if($lua -notmatch [regex]::Escape($x)){throw ('Missing Auto Trade semantic token: '+$x)}};" ^
  "$proto=Get-Content 'src/protocol.h' -Raw -Encoding UTF8; foreach($x in @('QueryTeam = 2','SelectTarget = 3','SelectAndTrade = 4','QueryTradeUi = 5')){if($proto -notmatch [regex]::Escape($x)){throw ('Protocol missing '+$x)}};" ^
  "Write-Host 'AUTO TRADE AUDIT PASS: team -> target -> runtime Trade/Request -> packet 200051 -> UI proof; no mouse macro/unrelated feature.'"
if errorlevel 1 exit /b 1

echo [2/6] Build Auto Trade bridge DLL...
zig c++ -target x86_64-windows-gnu -O2 -std=c++17 -Wall -Wextra -Werror -shared -s ^
  src\bridge.cpp -luser32 -lkernel32 -o dist\ThanLongAutoTradeTestBridge.dll
if errorlevel 1 exit /b 1

echo [3/6] Verify bridge PE DLL...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$ErrorActionPreference='Stop'; $p='dist\ThanLongAutoTradeTestBridge.dll'; $b=[IO.File]::ReadAllBytes($p); if($b.Length -lt 256){throw 'Bridge DLL too small'}; if($b[0] -ne 0x4D -or $b[1] -ne 0x5A){throw 'Bridge is not PE/MZ'}; $pe=[BitConverter]::ToInt32($b,0x3C); if($pe -lt 0 -or $pe+24 -ge $b.Length){throw 'Invalid PE offset'}; if($b[$pe] -ne 0x50 -or $b[$pe+1] -ne 0x45){throw 'Missing PE signature'}; $ch=[BitConverter]::ToUInt16($b,$pe+22); if(($ch -band 0x2000) -eq 0){throw 'PE is not DLL'}; Write-Host 'BRIDGE PE DLL PASS'"
if errorlevel 1 exit /b 1

echo [4/6] Build resources...
pushd resources
zig rc /c 65001 /fo ..\dist\app.res app.rc
popd
if errorlevel 1 exit /b 1

echo [5/6] Build controller EXE...
zig c++ -target x86_64-windows-gnu -O2 -std=c++17 -Wall -Wextra -Werror -municode -static -s ^
  src\controller.cpp dist\app.res -Wl,--subsystem,windows ^
  -lcomctl32 -luser32 -lkernel32 -lgdi32 ^
  -o dist\ThanLongAutoTradeTest_v0.1.0.exe
if errorlevel 1 exit /b 1

echo [6/6] Done.
echo BUILD THANH CONG - AUTO TRADE TEST v0.1.0
