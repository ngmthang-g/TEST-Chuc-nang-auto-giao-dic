#include "travel_fight_guard_logic.h"

#include <cassert>

int main() {
    using namespace travel_fight_guard_logic;

    static_assert(kStopAttemptsBeforeReset == 2);
    assert(NeedsAnotherStopBeforeReset(0));
    assert(NeedsAnotherStopBeforeReset(1));
    assert(!NeedsAnotherStopBeforeReset(2));

    assert(!HasAutoPathFightConflict(false, false));
    assert(!HasAutoPathFightConflict(true, false));
    assert(!HasAutoPathFightConflict(false, true));
    assert(HasAutoPathFightConflict(true, true));

    assert(!CanDispatchMovement(false, false));
    assert(!CanDispatchMovement(false, true));
    assert(!CanDispatchMovement(true, true));
    assert(CanDispatchMovement(true, false));

    assert(!CanDispatchFightStart(false, false));
    assert(!CanDispatchFightStart(false, true));
    assert(!CanDispatchFightStart(true, true));
    assert(CanDispatchFightStart(true, false));

    assert(!ConflictRecoveryComplete(false, false, false));
    assert(!ConflictRecoveryComplete(true, true, false));
    assert(!ConflictRecoveryComplete(true, false, true));
    assert(ConflictRecoveryComplete(true, false, false));
    return 0;
}
