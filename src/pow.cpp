// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"
#include <cmath>
#include "auxpow.h"
#include "arith_uint256.h"
#include "bignum.h"
#include "chain.h"
#include "nyancoin.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

typedef int64_t int64;
typedef uint64_t uint64;


unsigned int static KimotoGravityWell(const CBlockIndex* pindexLast, const CBlockHeader *pblock, uint64_t TargetBlocksSpacingSeconds, uint64_t PastBlocksMin, uint64_t PastBlocksMax, const Consensus::Params& params) {
    /* current difficulty formula, megacoin - kimoto gravity well */
    const CBlockIndex  *BlockLastSolved             = pindexLast;
    const CBlockIndex  *BlockReading                = pindexLast;
    const CBlockHeader *BlockCreating               = pblock;
                        BlockCreating               = BlockCreating;
    uint64_t            PastBlocksMass              = 0;
    int64_t             PastRateActualSeconds       = 0;
    int64_t             PastRateTargetSeconds       = 0;
    double              PastRateAdjustmentRatio     = double(1);
    CBigNum             PastDifficultyAverage;
    CBigNum             PastDifficultyAveragePrev;
    double              EventHorizonDeviation;
    double              EventHorizonDeviationFast;
    double              EventHorizonDeviationSlow;

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) { return UintToArith256(params.powLimit).GetCompact(); }

    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
        PastBlocksMass++;

        if (i == 1) { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
        else        { PastDifficultyAverage = ((CBigNum().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev; }
        PastDifficultyAveragePrev = PastDifficultyAverage;

        PastRateActualSeconds           = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
        PastRateTargetSeconds           = TargetBlocksSpacingSeconds * PastBlocksMass;
        PastRateAdjustmentRatio         = double(1);
        if (PastRateActualSeconds < 0) { PastRateActualSeconds = 0; }
        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
        PastRateAdjustmentRatio         = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
        }
        EventHorizonDeviation           = 1 + (0.7084 * std::pow((double(PastBlocksMass)/double(144)), -1.228));
        EventHorizonDeviationFast       = EventHorizonDeviation;
        EventHorizonDeviationSlow       = 1 / EventHorizonDeviation;

        if (PastBlocksMass >= PastBlocksMin) {
            if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast)) { assert(BlockReading); break; }
        }
        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }

    CBigNum bnNew(PastDifficultyAverage);
    if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
        bnNew *= PastRateActualSeconds;
        bnNew /= PastRateTargetSeconds;
    }
    if (bnNew > CBigNum(params.powLimit)) { bnNew = CBigNum(params.powLimit); }

    return bnNew.GetCompact();
}




unsigned int static GetNextWorkRequired_V2(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
	static const int64_t	BlocksTargetSpacing			= params.nPowTargetSpacing;
	unsigned int			TimeDaySeconds				= 60 * 60 * 24;
	int64_t					PastSecondsMin				= TimeDaySeconds * 0.25;
	int64_t					PastSecondsMax				= TimeDaySeconds * 1;
	uint64_t				PastBlocksMin				= PastSecondsMin / BlocksTargetSpacing;
	uint64_t				PastBlocksMax				= PastSecondsMax / BlocksTargetSpacing;	


	return KimotoGravityWell(pindexLast, pblock, BlocksTargetSpacing, PastBlocksMin, PastBlocksMax, params);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();
    int64_t nTargetTimespan = 3 * 60 * 60;
    int64_t nTargetSpacing = 60;
    int64_t nInterval = nTargetTimespan / nTargetSpacing;	
    int64_t nReTargetHistoryFact = 4;
  
  // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

	

    if(pindexLast->nHeight >= 66999)
    {
        return GetNextWorkRequired_V2(pindexLast, pblock, params);
    }

    // Only change once per interval
    if ((pindexLast->nHeight+1) % nInterval != 0)
    {
        // Special difficulty rule for testnet:
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->nTime > pindexLast->nTime + nTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
		
        return pindexLast->nBits;
    }
	
    // Nyancoin: This fixes an issue where a 51% attack can change difficulty at will.
    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
    int blockstogoback = nInterval-1;
    if ((pindexLast->nHeight+1) != nInterval)
        blockstogoback = nInterval;
    if (pindexLast->nHeight > 15000) 
        blockstogoback = nReTargetHistoryFact * nInterval;
	
    // Go back by what we want to be 14 days worth of blocks
    int nHeightFirst = pindexLast->nHeight - blockstogoback;
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);
	
  

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{

int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
int64_t nReTargetHistoryFact =4;
int64_t nTargetTimespan = 3 * 60 * 60; // NyanCoin: 3 hours
 

   if (pindexLast->nHeight > 15000)
        // obtain average actual timespan
        nActualTimespan = (pindexLast->GetBlockTime() - nFirstBlockTime)/nReTargetHistoryFact;
    else
        nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    
    if (nActualTimespan < nTargetTimespan/4)
        nActualTimespan = nTargetTimespan/4;
    if (nActualTimespan > nTargetTimespan*4)
        nActualTimespan = nTargetTimespan*4;

	
    // Retarget
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;
	

const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
if (bnNew > bnPowLimit)
    bnNew = bnPowLimit;


return bnNew.GetCompact();
}



bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}






