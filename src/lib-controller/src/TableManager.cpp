//----------------------------------------------------------------
//
// File: TableManager.cpp
//
//----------------------------------------------------------------

#include <filesystem>
#include <controller/TableManager.h>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/UndoManager.h>
#include <craps/CrapsTable.h>
#include <craps/Player.h>
#include <gen/Logger.h>
#include <memory>
#include <stdexcept>

using namespace Ctrl;

namespace {

using En = Gen::ErrorPass::ErrorName;
using Et = Gen::ErrorPass::ErrorType;
using Es = Gen::ErrorPass::Severity;

// Top-level section of a table file
constexpr const char* KeyTable = "Table";

} // namespace

//----------------------------------------------------------------
//
// Design note:
//
// Private pCurrentCrapsTable_ is used to manage the lifetime of the
// current global CrapsTable. This differs from how CrapsGame class
// manages lifetime of all the other globals. Unlike the other
// globals, which remain for the life of the program, the CrapsTable
// global needs the ability to be swapped out for a different one
// if/when user selects different table at runtime.
//
TableManager::TableManager()
{
    manifest_.loadFromFile();           // Load tables.yaml manifest
    pCurrentCrapsTable_ = loadStartingCrapsTable(pCurrentFile_);
    Gbl::pTable = pCurrentCrapsTable_;  // Make it accessible via globals
    tables_.push_back(pCurrentCrapsTable_);
}

//----------------------------------------------------------------

TableManager::~TableManager()
{
    delete pCurrentCrapsTable_;
}

//----------------------------------------------------------------

const TableManifest::TableList&
TableManager::getTableList() const
{
    return manifest_.getTableList();
}

//----------------------------------------------------------------

//
// File name (no directory) of the current table, e.g. "las-vegas-strip.yaml".
//
std::string
TableManager::getCurrentFileName() const
{
    return pCurrentFile_ ? pCurrentFile_->getPath().filename().string()
                         : std::string();
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::getTable(const Craps::TableId& tableId,
                       Gen::ErrorPass& ep) const
{
    for (auto t : tables_)
    {
        if (t->getTableId() == tableId)
        {
            return t;
        }
    }
    ep.setDescription("TableManager::getTable(): no such tableId; \"" +
                      tableId + "\"");
    return nullptr;
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadCrapsTable(const std::string& fileName,
                             std::unique_ptr<YamlFile>& file)
{
    auto tableId = manifest_.getTableId(fileName);
    if (tableId.empty())
    {
        std::string diag = "TableManager::loadCrapsTable(): unable to load "
            "table file; lookup failed; file name:\"" + fileName +
            "\" is not found in manifest file:\"tables.yaml\".";
        throw std::runtime_error(diag);
    }

    Craps::TableConfig config;
    config.maxSessions    = TableManager::retrieveMaxSessions();
    config.maxRecentRolls = TableManager::MaxRecentRolls;

    auto newFile = std::make_unique<YamlFile>(
        TableManager::formTablePath(fileName));
    newFile->load(KeyTable);                                   // throws

    std::unique_ptr<Craps::CrapsTable> table(
        new Craps::CrapsTable(tableId, config));
    try
    {
        table->fromYAML(newFile->getSection(KeyTable));        // throws
    }
    catch (const std::runtime_error& e)
    {
        throw std::runtime_error("TableManager::loadCrapsTable(): \"" +
            newFile->getPath().string() + "\"; " + e.what());
    }

    file = std::move(newFile);
    return table.release();
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadStartingCrapsTable(std::unique_ptr<YamlFile>& file)
{
    std::string fileName =
        Gbl::pConfigMgr->getString(ConfigManager::KeyTableStart).value();
    return loadCrapsTable(fileName, file);
}

//----------------------------------------------------------------
//
// Called once at quit. The table shuts down its players too, so this
// must come before PlayerManager::shutdown().
//
void
TableManager::shutdown()
{
    pCurrentCrapsTable_->shutdown();   // Players and table; merges stats
    saveCurrentTable();
}

//----------------------------------------------------------------
//
// Writes the current table's file from a fresh snapshot of the table.
// A failure is logged.
//
void
TableManager::saveCurrentTable() const
{
    try
    {
        pCurrentFile_->setSection(KeyTable, pCurrentCrapsTable_->toYAML());
        pCurrentFile_->save();                                 // throws
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(std::string("TableManager::saveCurrentTable(): ") + e.what());
    }
}

//----------------------------------------------------------------

Gen::ReturnCode
TableManager::switchCrapsTable(const Craps::TableId& toTableId,
                               bool                  buddiesFollow,
                               SwitchResult&         result,
                               Gen::ErrorPass&       ep)
{
    const std::string fname("TableManager::switchCrapsTable(): ");

    // ---- Check and load, change nothing -------------------------

    if (pCurrentCrapsTable_ == nullptr || Gbl::pPlayerMgr == nullptr)
    {
        EP_SET(ep, En::IllegalStateError, Et::ProcessingError,
               Es::Indeterminate, fname + "there is no table or no player "
               "manager.");
        return Gen::ReturnCode::Fail;
    }

    const std::string fileName = manifest_.getFileName(toTableId);
    if (fileName.empty())
    {
        EP_SET(ep, En::NotFoundError, Et::ProcessingError,
               Es::Indeterminate, fname + "no such table: \"" + toTableId +
               "\".");
        return Gen::ReturnCode::Fail;
    }
    if (pCurrentCrapsTable_->getTableId() == toTableId)
    {
        EP_SET(ep, En::ExistsError, Et::ProcessingError,
               Es::Indeterminate, fname + "you are already at " +
               pCurrentCrapsTable_->getTableName() + ".");
        return Gen::ReturnCode::Fail;
    }
    if (!pCurrentCrapsTable_->isBettingOpen())
    {
        EP_SET(ep, En::IllegalStateError, Et::ProcessingError,
               Es::Indeterminate, fname + "betting is closed. Try again "
               "when the roll is over.");
        return Gen::ReturnCode::Fail;
    }

    std::unique_ptr<YamlFile>        newFile;
    std::unique_ptr<Craps::CrapsTable> newTable;
    try
    {
        newTable.reset(loadCrapsTable(fileName, newFile));     // throws
    }
    catch (const std::exception& e)
    {
        EP_SET(ep, En::CaughtException, Et::ProcessingError,
               Es::Indeterminate, fname + e.what());
        return Gen::ReturnCode::Fail;
    }

    // ---- Leave the old table -------------------------------------

    Craps::CrapsTable* pOld = pCurrentCrapsTable_;
    Craps::Player* pUser = Gbl::pPlayerMgr->getUserPlayer();
    const std::vector<Craps::Player*> seated = pOld->getPlayers();  // a copy

    result = SwitchResult{};
    result.fromTableId = pOld->getTableId();
    result.toTableId   = toTableId;
    result.toTableName = newTable->getTableName();
    for (auto* p : seated)
    {
        result.betsReturned += p->getNumBetsOnTable();
    }

    for (auto* p : seated)
    {
        p->endSession();      // Bets come down, money back, stats merged
    }
    UndoManager::instance().clear();     // Their bets are gone
    pOld->close(static_cast<unsigned>(seated.size()));
    saveCurrentTable();      // A failure is logged and must not block us

    // ---- Take the new table --------------------------------------

    retired_.emplace_back(pOld);         // Not deleted, see the class notes
    pCurrentCrapsTable_ = newTable.release();
    pCurrentFile_       = std::move(newFile);
    tables_[0]          = pCurrentCrapsTable_;
    Gbl::pTable         = pCurrentCrapsTable_;

    bool userSeated = false;
    for (auto* p : seated)
    {
        const bool isUser = (p == pUser);
        if (!isUser && !buddiesFollow)
        {
            ++result.buddiesLeft;
            continue;
        }
        auto joined = p->joinTable(*pCurrentCrapsTable_);
        if (!joined)
        {
            LOG_ERROR(fname + joined.error().fullDesc());
            if (isUser)
            {
                ep = std::move(joined).error();
                ep.prepend(fname);
            }
            else
            {
                ++result.buddiesLeft;
            }
            continue;
        }
        if (isUser) userSeated = true;
        else        ++result.buddiesFollowed;
    }
    if (userSeated)
    {
        pCurrentCrapsTable_->setShooter(*pUser);
    }

    // ---- Tidy up ---------------------------------------------------

    Gbl::pPlayerMgr->savePlayers();      // Their sessions have ended
    Gbl::pPlayerMgr->userPrefs().autoOdds().forgetHandled();

    LOG_INFO(fname + "now at " + result.toTableName + " (was " +
             result.fromTableId + "); " +
             std::to_string(result.buddiesFollowed) + " buddies followed, " +
             std::to_string(result.buddiesLeft) + " left");

    if (!userSeated)
    {
        // Cannot happen with an empty table, but never claim success if
        // the user is not at the table.
        if (ep.description().empty())
        {
            EP_SET(ep, En::IllegalStateError, Et::ProcessingError,
                   Es::Indeterminate, fname + "the user player is not "
                   "seated at the new table.");
        }
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

size_t
TableManager::retrieveMaxSessions()
{
    return Gbl::pConfigMgr->getInt(
        Ctrl::ConfigManager::KeyTableMaxSessions).value();
}

//----------------------------------------------------------------

std::filesystem::path
TableManager::formTablePath(const std::string& fileName)
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsUsrTables).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / fileName;
    return path;
}

//----------------------------------------------------------------
