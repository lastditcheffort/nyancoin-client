// Copyright (c) 2015 The Nyancoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "arith_uint256.h"
#include "chainparams.h"
#include "nyancoin.h"
#include "test/test_bitcoin.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(nyancoin_tests, TestingSetup)

/**
 * the maximum block reward at a given height for a block without fees
 */
uint64_t expectedMaxSubsidy(int height) {
    if (height < 100000) {
        return 1000000 * COIN;
    } else if (height < 145000) {
        return 500000 * COIN;
    } else if (height < 200000) {
        return 250000 * COIN;
    } else if (height < 300000) {
        return 125000 * COIN;
    } else if (height < 400000) {
        return  62500 * COIN;
    } else if (height < 500000) {
        return  31250 * COIN;
    } else if (height < 600000) {
        return  15625 * COIN;
    } else {
        return  10000 * COIN;
    }
}

/**
 * the minimum possible value for the maximum block reward at a given height
 * for a block without fees
 */
uint64_t expectedMinSubsidy(int height) {
    if (height < 100000) {
        return 0;
    } else if (height < 145000) {
        return 0;
    } else if (height < 200000) {
        return 250000 * COIN;
    } else if (height < 300000) {
        return 125000 * COIN;
    } else if (height < 400000) {
        return  62500 * COIN;
    } else if (height < 500000) {
        return  31250 * COIN;
    } else if (height < 600000) {
        return  15625 * COIN;
    } else {
        return  10000 * COIN;
    }
}

BOOST_AUTO_TEST_CASE(subsidy_limit_test)
{
    int nHeight = 0;
    int nStepSize= 1;
    const CChainParams& mainParams = Params(CBaseChainParams::MAIN);
    CAmount nSum = 0;
    uint256 prevHash = uint256S("0");

    for (nHeight = 0; nHeight <= 100000; nHeight++) {
        const Consensus::Params& params = mainParams.GetConsensus(nHeight);
        CAmount nSubsidy = GetNyancoinBlockSubsidy(nHeight, params, prevHash);
        BOOST_CHECK(MoneyRange(nSubsidy));
        BOOST_CHECK(nSubsidy <= 1000000 * COIN);
        nSum += nSubsidy * nStepSize;
    }
    for (; nHeight <= 145000; nHeight++) {
        const Consensus::Params& params = mainParams.GetConsensus(nHeight);
        CAmount nSubsidy = GetNyancoinBlockSubsidy(nHeight, params, prevHash);
        BOOST_CHECK(MoneyRange(nSubsidy));
        BOOST_CHECK(nSubsidy <= 500000 * COIN);
        nSum += nSubsidy * nStepSize;
    }
    for (; nHeight < 600000; nHeight++) {
        const Consensus::Params& params = mainParams.GetConsensus(nHeight);
        CAmount nSubsidy = GetNyancoinBlockSubsidy(nHeight, params, prevHash);
        CAmount nExpectedSubsidy = (500000 >> (nHeight / 100000)) * COIN;
        BOOST_CHECK(MoneyRange(nSubsidy));
        BOOST_CHECK_EQUAL(nSubsidy, nExpectedSubsidy);
        nSum += nSubsidy * nStepSize;
    }

    //test sum +- ~10billion
    arith_uint256 upperlimit = arith_uint256("95e14ec776380000"); //108 billion doge
    BOOST_CHECK(nSum <= upperlimit);

    arith_uint256 lowerlimit = arith_uint256("7a1fe16027700000"); //88 billion doge
    BOOST_CHECK(nSum >= lowerlimit);

    // Test reward at 600k+ is constant
    const Consensus::Params& params = mainParams.GetConsensus(nHeight);
    CAmount nConstantSubsidy = GetNyancoinBlockSubsidy(nHeight, params, prevHash);
    BOOST_CHECK_EQUAL(nConstantSubsidy, 10000 * COIN);

    nConstantSubsidy = GetNyancoinBlockSubsidy(700000, params, prevHash);
    BOOST_CHECK_EQUAL(nConstantSubsidy, 10000 * COIN);
}





BOOST_AUTO_TEST_CASE(hardfork_parameters)
{
    SelectParams(CBaseChainParams::MAIN);
    const Consensus::Params& initialParams = Params().GetConsensus(0);

    BOOST_CHECK_EQUAL(initialParams.nPowTargetTimespan, 14400);
    BOOST_CHECK_EQUAL(initialParams.fAllowLegacyBlocks, true);
    BOOST_CHECK_EQUAL(initialParams.fDigishieldDifficultyCalculation, false);

    const Consensus::Params& initialParamsEnd = Params().GetConsensus(144999);
    BOOST_CHECK_EQUAL(initialParamsEnd.nPowTargetTimespan, 14400);
    BOOST_CHECK_EQUAL(initialParamsEnd.fAllowLegacyBlocks, true);
    BOOST_CHECK_EQUAL(initialParamsEnd.fDigishieldDifficultyCalculation, false);

    const Consensus::Params& digishieldParams = Params().GetConsensus(145000);
    BOOST_CHECK_EQUAL(digishieldParams.nPowTargetTimespan, 60);
    BOOST_CHECK_EQUAL(digishieldParams.fAllowLegacyBlocks, true);
    BOOST_CHECK_EQUAL(digishieldParams.fDigishieldDifficultyCalculation, true);

    const Consensus::Params& digishieldParamsEnd = Params().GetConsensus(371336);
    BOOST_CHECK_EQUAL(digishieldParamsEnd.nPowTargetTimespan, 60);
    BOOST_CHECK_EQUAL(digishieldParamsEnd.fAllowLegacyBlocks, true);
    BOOST_CHECK_EQUAL(digishieldParamsEnd.fDigishieldDifficultyCalculation, true);

    const Consensus::Params& auxpowParams = Params().GetConsensus(11327960);
    BOOST_CHECK_EQUAL(auxpowParams.nHeightEffective, 11327960);
    BOOST_CHECK_EQUAL(auxpowParams.nPowTargetTimespan, 60);
    BOOST_CHECK_EQUAL(auxpowParams.fAllowLegacyBlocks, false);
    BOOST_CHECK_EQUAL(auxpowParams.fDigishieldDifficultyCalculation, true);

    const Consensus::Params& auxpowHighParams = Params().GetConsensus(700000); // Arbitrary point after last hard-fork
    BOOST_CHECK_EQUAL(auxpowHighParams.nPowTargetTimespan, 60);
    BOOST_CHECK_EQUAL(auxpowHighParams.fAllowLegacyBlocks, false);
    BOOST_CHECK_EQUAL(auxpowHighParams.fDigishieldDifficultyCalculation, true);
}

BOOST_AUTO_TEST_SUITE_END()
