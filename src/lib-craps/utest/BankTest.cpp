//----------------------------------------------------------------
//
// File: BankTest.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>

#include <doctest/doctest.h>
#include <craps/Bank.h>
#include <sstream>

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("Bank:basicFunctionality")
{
    // Setup: initial balance = 1500, threshold = 750, refill = 1000
    Gen::Money init = 1500;
    Gen::Money threshold = init / 2;
    Gen::Money refillAmt = (init * 2) / 3;

    Bank bank(init, threshold, refillAmt);

    SUBCASE("Refill triggers when balance drops below threshold")
    {
        CHECK(bank.getBalance() == 1500);

        // Withdraw 800 (balance becomes 700), should trigger refill
        bool didRefill = bank.withdraw(800);
        CHECK(didRefill);
        CHECK(bank.getBalance() == 700 + refillAmt);  // 700 + 1000 = 1700

        // Withdraw again, no refill should happen
        didRefill = bank.withdraw(600);  // new balance 1700 - 600 = 1100
        CHECK_FALSE(didRefill);
        CHECK(bank.getBalance() == 1100);
    }

    SUBCASE("mergeStats accumulates session stats correctly")
    {
        bank.deposit(100);
        bank.withdraw(1000); // should refill
        bank.mergeStats();

        YAML::Node yaml = bank.toYAML();

        // Deserialize into a new Bank instance
        Bank loaded;
        loaded.fromYAML(yaml);

        // Check key values
        CHECK(loaded.getAmtDeposited() == 0);  // only saved in stats
        auto loadedStats = yaml["bankStats"];
        CHECK(loadedStats["amtDeposited"].as<Gen::Money>() == 100);
        CHECK(loadedStats["amtWithdrawn"].as<Gen::Money>() == 1000);
        CHECK(loadedStats["amtRefilled"].as<Gen::Money>() == refillAmt);
        CHECK(loadedStats["numDeposits"].as<unsigned>() == 1);
        CHECK(loadedStats["numWithdrawals"].as<unsigned>() == 1);
        CHECK(loadedStats["numRefills"].as<unsigned>() == 1);
    }

    SUBCASE("YAML round-trip consistency")
    {
        bank.deposit(123);
        bank.withdraw(456);
        bank.mergeStats();

        YAML::Node node = bank.toYAML();
        Bank recovered;
        recovered.fromYAML(node);

        // Confirm initial parameters restored
        YAML::Node statsNode = node["bankStats"];
        CHECK(statsNode["amtDeposited"].as<Gen::Money>() == 123);
        CHECK(statsNode["amtWithdrawn"].as<Gen::Money>() == 456);
    }
}

//----------------------------------------------------------------
