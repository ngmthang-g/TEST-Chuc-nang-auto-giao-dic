#include "trade_coordinator_logic.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace itemtrade_coordinator;

struct MockStep {
    int kind = 0;
    int target = 0;
};

int main() {
    // Same decision helpers used by controller.cpp at runtime.
    assert(ReceivedSlots(40, 32) == 8);
    assert(ReceivedSlots(40, 31) == 9);
    assert(ReceivedSlots(40, 40) == 0);
    assert(ReceivedSlots(40, 42) == 0);
    assert(DecidePass(40, 32) == PassDecision::FinishChild);
    assert(DecidePass(40, 31) == PassDecision::RepeatSameChild);

    // FIFO: older workflow ticket wins even if its CON number is larger.
    assert(EarlierWorkflowEntry(1, 3, 2, 1));
    assert(!EarlierWorkflowEntry(2, 1, 1, 3));
    assert(EarlierWorkflowEntry(10, 1, 10, 2));
    assert(!EarlierWorkflowEntry(10, 2, 10, 1));

    // Queue admission: exact FULL only and max three staged children.
    assert(ShouldAdmitFullChild(true, 0, 0));
    assert(ShouldAdmitFullChild(true, 0, 2));
    assert(!ShouldAdmitFullChild(true, 1, 0));
    assert(!ShouldAdmitFullChild(true, 0, 3));
    assert(!ShouldAdmitFullChild(false, 0, 0));

    // Auto Sell checkbox is the master switch in every mode.
    // OFF means no automatic sell; ON permits the existing mode/role thresholds.
    assert(!ShouldAutoSell(false, 0, false, 0, 6));
    assert(ShouldAutoSell(false, 0, true, 0, 6));
    assert(!ShouldAutoSell(false, 0, true, 1, 6));
    assert(!ShouldAutoSell(true, 1, false, 6, 6));
    assert(ShouldAutoSell(true, 1, true, 6, 6));
    assert(!ShouldAutoSell(true, 1, true, 7, 6));
    assert(!ShouldAutoSell(true, 2, true, 0, 6));
    assert(ShouldAutoSell(true, 0, true, 0, 6));
    assert(!ShouldAutoSell(true, 0, false, 0, 6));

    // Runtime sequence validation helper used by TradeSequenceReady().
    std::vector<MockStep> noTransfer{{0, 0}, {0, 1}};
    std::vector<MockStep> mainTransferOnly{{1, 1}};
    std::vector<MockStep> childTransfer{{0, 0}, {1, 0}};
    assert(!HasChildTransferStep(noTransfer));
    assert(!HasChildTransferStep(mainTransferOnly));
    assert(HasChildTransferStep(childTransfer));

    std::cout << "trade_coordinator_logic_tests PASS\n";
    return 0;
}
