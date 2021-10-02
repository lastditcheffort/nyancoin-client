// Copyright (c) 2015 The Nyancoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "policy/policy.h"
#include "arith_uint256.h"
#include "nyancoin.h"
#include "txmempool.h"
#include "util.h"
#include "validation.h"





unsigned int CalculateNyancoinNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, uint64_t TargetBlocksSpacingSeconds, uint64_t PastBlocksMin, uint64_t PastBlocksMax) {
    /* current difficulty formula, megacoin - kimoto gravity well */
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    const CBlockHeader *BlockCreating = pblock;
                                            BlockCreating = BlockCreating;
    
const arith_uint256 bnPowLimit = bnPowLimit;
    uint64_t PastBlocksMass = 0;
    int64_t PastRateActualSeconds = 0;


    int64_t PastRateTargetSeconds = 0;
    double PastRateAdjustmentRatio = double(1);
    arith_uint256 PastDifficultyAverage;
    arith_uint256 PastDifficultyAveragePrev;
    double EventHorizonDeviation;
    double EventHorizonDeviationFast;
    double EventHorizonDeviationSlow;

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) { return bnPowLimit.GetCompact(); }

        for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
            if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
            PastBlocksMass++;

            if (i == 1) { 
                PastDifficultyAverage.SetCompact(BlockReading->nBits);
            } else {
                PastDifficultyAverage = ((arith_uint256().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev;
            }
            PastDifficultyAveragePrev = PastDifficultyAverage;

            PastRateActualSeconds = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
            PastRateTargetSeconds = TargetBlocksSpacingSeconds * PastBlocksMass;
            PastRateAdjustmentRatio = double(1);
            if (PastRateActualSeconds < 0) { PastRateActualSeconds = 0; }
            if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
            PastRateAdjustmentRatio = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
            }
            EventHorizonDeviation = 1 + (0.7084 * pow((double(PastBlocksMass)/double(28.2)), -1.228));
            EventHorizonDeviationFast = EventHorizonDeviation;
            EventHorizonDeviationSlow = 1 / EventHorizonDeviation;

            if (PastBlocksMass >= PastBlocksMin) {
                if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast)) { assert(BlockReading); break; }
            }
            if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
            BlockReading = BlockReading->pprev;
        }

        arith_uint256 bnNew(PastDifficultyAverage);
        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
                bnNew *= PastRateActualSeconds;
                bnNew /= PastRateTargetSeconds;
        }
    if (bnNew > bnPowLimit) { bnNew = bnPowLimit; }


    return bnNew.GetCompact();
}


bool CheckAuxPowProofOfWork(const CBlockHeader& block, const Consensus::Params& params)
{
    /* Except for legacy blocks with full version 1, ensure that
       the chain ID is correct.  Legacy blocks are not allowed since
       the merge-mining start, which is checked in AcceptBlockHeader
       where the height is known.  */
    if (!block.IsLegacy() && params.fStrictChainId && block.GetChainId() != params.nAuxpowChainId)
        return error("%s : block does not have our chain ID"
                     " (got %d, expected %d, full nVersion %d)",
                     __func__, block.GetChainId(),
                     params.nAuxpowChainId, block.nVersion);

    /* If there is no auxpow, just check the block hash.  */
    if (!block.auxpow) {
        if (block.IsAuxpow())
            return error("%s : no auxpow on block with auxpow version",
                         __func__);

        if (!CheckProofOfWork(block.GetPoWHash(), block.nBits, params))
            return error("%s : non-AUX proof of work failed", __func__);

        return true;
    }

    /* We have auxpow.  Check it.  */

    if (!block.IsAuxpow())
        return error("%s : auxpow on block with non-auxpow version", __func__);

    if (!block.auxpow->check(block.GetHash(), block.GetChainId(), params))
        return error("%s : AUX POW is not valid", __func__);
    if (!CheckProofOfWork(block.auxpow->getParentBlockPoWHash(), block.nBits, params))
        return error("%s : AUX proof of work failed", __func__);

    return true;
}

CAmount GetNyancoinBlockSubsidy(int nHeight, const Consensus::Params& consensusParams, uint256 prevHash)
{
    int halvings = nHeight / consensusParams.nSubsidyHalvingInterval;
    if(nHeight <= consensusParams.SubnHeight){
    // Force block reward to zero when right shift is undefined.
    if (halvings >= 64)
      {
         return 0;
      }
    
    CAmount nSubsidy = 337 * COIN; 
    
    if(nHeight < 50){
    nSubsidy = 67400 * COIN; // 1% premine
   	 }

    // Subsidy is cut in half every 210,000 blocks which will occur approximately every 4 years.
    nSubsidy >>= halvings;
    return nSubsidy;
    
     }else if(nHeight > consensusParams.SubnHeight && nHeight <= (consensusParams.SubnHeight + consensusParams.SubBlks)){
     CAmount nSubsidy = consensusParams.SubV * COIN;
    return nSubsidy;
     }else {
     // Force block reward to zero when right shift is undefined.
    if (halvings >= 64)
      {
         return 0;
      }
    
    CAmount nSubsidy = 337* COIN; 
    // Subsidy is cut in half every 210,000 blocks which will occur approximately every 4 years.
    nSubsidy >>= halvings;
    return nSubsidy;
     }
}

CAmount GetNyancoinMinRelayFee(const CTransaction& tx, unsigned int nBytes, bool fAllowFree)
{
    {
        LOCK(mempool.cs);
        uint256 hash = tx.GetHash();
        double dPriorityDelta = 0;
        CAmount nFeeDelta = 0;
        mempool.ApplyDeltas(hash, dPriorityDelta, nFeeDelta);
        if (dPriorityDelta > 0 || nFeeDelta > 0)
            return 0;
    }

    CAmount nMinFee = ::minRelayTxFee.GetFee(nBytes);
    nMinFee += GetNyancoinDustFee(tx.vout, ::minRelayTxFee);

    if (fAllowFree)
    {
        // There is a free transaction area in blocks created by most miners,
        // * If we are relaying we allow transactions up to DEFAULT_BLOCK_PRIORITY_SIZE - 1000
        //   to be considered to fall into this category. We don't want to encourage sending
        //   multiple transactions instead of one big transaction to avoid fees.
        if (nBytes < (DEFAULT_BLOCK_PRIORITY_SIZE - 1000))
            nMinFee = 0;
    }

    if (!MoneyRange(nMinFee))
        nMinFee = MAX_MONEY;
    return nMinFee;
}

CAmount GetNyancoinDustFee(const std::vector<CTxOut> &vout, CFeeRate &baseFeeRate) {
    CAmount nFee = 0;

    // To limit dust spam, add base fee for each output less than a COIN
    BOOST_FOREACH(const CTxOut& txout, vout)
        if (txout.IsDust(::minRelayTxFee))
            nFee += baseFeeRate.GetFeePerK();

    return nFee;
}
