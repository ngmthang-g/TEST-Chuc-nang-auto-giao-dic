#pragma once

namespace travel_fight_guard_logic {

// The travel contract is intentionally small and deterministic:
//   Stop #1 -> verify -> Stop #2 -> verify -> AUTO/Attack reset -> repeat.
// A route may be started only after an authoritative AutoFight=OFF snapshot.
constexpr int kStopAttemptsBeforeReset = 2;

inline bool NeedsAnotherStopBeforeReset(int completedStopAttempts) {
    return completedStopAttempts < kStopAttemptsBeforeReset;
}

inline bool HasAutoPathFightConflict(bool autoPathing, bool autoFight) {
    return autoPathing && autoFight;
}

// Any movement command that changes locomotion state (Mount or StartPath) is
// fail-closed unless AutoFight is authoritatively OFF. Unknown is unsafe.
inline bool CanDispatchMovement(bool autoFightStateKnown, bool autoFight) {
    return autoFightStateKnown && !autoFight;
}

// A queued AUTO -> Attack request can outlive the state that created it. Recheck
// AutoPath at dispatch time so a stale request cannot turn fight on after travel
// has started. Unknown is unsafe; the caller waits for an authoritative snapshot.
inline bool CanDispatchFightStart(bool autoPathStateKnown, bool autoPathing) {
    return autoPathStateKnown && !autoPathing;
}

inline bool ConflictRecoveryComplete(bool conflictLatched,
                                     bool autoPathing,
                                     bool autoFight) {
    return conflictLatched && !autoPathing && !autoFight;
}

} // namespace travel_fight_guard_logic
