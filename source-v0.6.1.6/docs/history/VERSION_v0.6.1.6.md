# VERSION v0.6.1.6

## User-approved scope
All ten cleanup items from v0.6.1.5 were approved: remove obsolete global mouse contention logic, make hidden actions per-client, scope REC locally, stop unrelated SELL from blocking trade, make priority per-PID, and retain only business/safety locks.

## Added movement invariant
Before either AutoPath or Mount, AutoFight must be authoritatively OFF. The implementation reuses the proven Travel Guard: Stop #1, Stop #2, if still ON run AUTO→Attack reset once, then repeat Stop until OFF. Mount Recovery may still fight for 10 seconds after two failed mount attempts, but no subsequent Mount command can be dispatched until AutoFight is OFF.

## Preserved
Client Freeze, F4 global pause, FIFO/re-lock, tradeHeld, death lifecycle, route ownership reset, AutoPath+AutoFight conflict recovery, semantic hidden clicks, and F8/REC coordinate capture.
