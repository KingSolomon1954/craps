//----------------------------------------------------------------
//
// File: UndoManagerTest.cpp
//
//----------------------------------------------------------------

#include <controller/UndoManager.h>
#include <doctest/doctest.h>

using namespace Ctrl;

namespace {

// Records the order in which steps are undone, and can be told to fail
struct FakeUndo : public UndoBet
{
    FakeUndo(std::vector<std::string>& log, std::string name, bool fails = false)
        : log_(log), name_(std::move(name)), fails_(fails)
    {}

    Gen::ReturnCode undo(Gen::ErrorPass& ep) override
    {
        if (fails_)
        {
            EP_SET(ep, Gen::ErrorPass::ErrorName::NoPermissionError,
                       Gen::ErrorPass::ErrorType::ProcessingError,
                       Gen::ErrorPass::Severity::Indeterminate, "refused");
            return Gen::ReturnCode::Fail;
        }
        log_.push_back(name_);
        return Gen::ReturnCode::Success;
    }
    std::string describe() const override { return "undo " + name_; }

    std::vector<std::string>& log_;
    std::string name_;
    bool fails_;
};

struct UndoFixture
{
    UndoFixture()  { UndoManager::instance().clear(); }
   ~UndoFixture()  { UndoManager::instance().clear(); }

    UndoManager& um = UndoManager::instance();
    std::vector<std::string> log;
    const Craps::PlayerId a { "player-a" };
    const Craps::PlayerId b { "player-b" };

    void pushFake(const Craps::PlayerId& who, const std::string& name,
                  bool join = false, bool fails = false)
    {
        um.push(who, std::make_unique<FakeUndo>(log, name, fails), join);
    }
};

} // namespace

//----------------------------------------------------------------

TEST_CASE_FIXTURE(UndoFixture, "UndoManager: last in, first out")
{
    CHECK_FALSE(um.canUndo(a));
    pushFake(a, "one");
    pushFake(a, "two");
    pushFake(a, "three");
    CHECK(um.depth(a) == 3);

    std::string what;
    Gen::ErrorPass ep;
    CHECK(um.undo(a, what, ep) == Gen::ReturnCode::Success);
    CHECK(what == "undo three");
    CHECK(um.undo(a, what, ep) == Gen::ReturnCode::Success);
    CHECK(um.undo(a, what, ep) == Gen::ReturnCode::Success);
    CHECK(log == std::vector<std::string>{"three", "two", "one"});
    CHECK_FALSE(um.canUndo(a));
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(UndoFixture, "UndoManager: nothing to undo is an error, not a crash")
{
    std::string what;
    Gen::ErrorPass ep;
    CHECK(um.undo(a, what, ep) == Gen::ReturnCode::Fail);
    CHECK(log.empty());
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(UndoFixture, "UndoManager: each player has their own stack")
{
    pushFake(a, "a1");
    pushFake(b, "b1");
    pushFake(a, "a2");
    CHECK(um.depth(a) == 2);
    CHECK(um.depth(b) == 1);

    std::string what;
    Gen::ErrorPass ep;
    CHECK(um.undo(b, what, ep) == Gen::ReturnCode::Success);
    CHECK(log == std::vector<std::string>{"b1"});
    CHECK(um.depth(a) == 2);                      // Untouched

    um.clear(a);
    CHECK_FALSE(um.canUndo(a));
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(UndoFixture, "UndoManager: a batch joined together is one step")
{
    pushFake(a, "earlier");
    pushFake(a, "r1");
    pushFake(a, "r2", true);
    pushFake(a, "r3", true);
    CHECK(um.depth(a) == 2);                      // earlier, and the batch of 3

    std::string what;
    Gen::ErrorPass ep;
    CHECK(um.undo(a, what, ep) == Gen::ReturnCode::Success);
    CHECK(log == std::vector<std::string>{"r3", "r2", "r1"});
    CHECK(um.depth(a) == 1);

    CHECK(um.undo(a, what, ep) == Gen::ReturnCode::Success);
    CHECK(log.back() == "earlier");
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(UndoFixture, "UndoManager: joining with nothing to join starts a step")
{
    pushFake(a, "only", true);
    CHECK(um.depth(a) == 1);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(UndoFixture, "UndoManager: a failed undo drops that player's history")
{
    pushFake(a, "a1");
    pushFake(a, "a2", false, true);               // The one on top fails
    pushFake(b, "b1");

    std::string what;
    Gen::ErrorPass ep;
    CHECK(um.undo(a, what, ep) == Gen::ReturnCode::Fail);
    CHECK_FALSE(um.canUndo(a));
    CHECK(um.canUndo(b));                         // Others are not affected
    CHECK(log.empty());
    CHECK(ep.description().find("refused") != std::string::npos);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(UndoFixture, "UndoManager: clear forgets everybody")
{
    pushFake(a, "a1");
    pushFake(b, "b1");
    um.clear();
    CHECK_FALSE(um.canUndo(a));
    CHECK_FALSE(um.canUndo(b));
}

//----------------------------------------------------------------
