/*

      ##  ##  #####    #####   $$$$$   $$$$   $$$$$$    
      ##  ##  ##  ##  ##      $$      $$  $$    $$      
      ##  ##  #####   ##       $$$$   $$$$$$    $$      
      ##  ##  ##  ##  ##          $$  $$  $$    $$      
       ####   #####    #####  $$$$$   $$  $$    $$      
  ======================================================
  SLS SAT Solver from The University of British Columbia
  ======================================================
  ...Developed by Dave Tompkins (davet [@] cs.ubc.ca)...
  ------------------------------------------------------
  .......consult legal.txt for legal information........
  ......consult revisions.txt for revision history......
  ------------------------------------------------------
  ... project website: http://www.satlib.org/ubcsat ....
  ------------------------------------------------------
  .....e-mail ubcsat-help [@] cs.ubc.ca for support.....
  ------------------------------------------------------

*/

#include "ubcsat.h"

#ifdef __cplusplus 
namespace ubcsat {
#endif

void AdaptNoveltyNoiseW() {

  /* weighted varaint -- see regular algorithm for comments */

  if (iStep-iLastAdaptStep > iNumClauses/iInvTheta) {

    iNovNoise += (PROBABILITY) ((UINT32MAX - iNovNoise)/iInvPhi);
    iLastAdaptStep = iStep;
    fLastAdaptSumFalseW = fSumFalseW;
  
  } else if (fSumFalseW < fLastAdaptSumFalseW) {

    iNovNoise -= (PROBABILITY) (iNovNoise / iInvPhi / 2);
    
    iLastAdaptStep = iStep;
    fLastAdaptSumFalseW = fSumFalseW;
  }
}

void PickG2WSatW() {

  /* weighted varaint -- see regular algorithm for comments */
 
  UINT32 j;
  UINT32 iVar;
  FLOAT fScore;

  if (iNumDecPromVarsW > 0 ) {
    iFlipCandidate = aDecPromVarsListW[0];
    fBestScore = aVarScoreW[iFlipCandidate];
    for (j=1;j<iNumDecPromVarsW;j++) {
      iVar = aDecPromVarsListW[j];
      fScore = aVarScoreW[iVar];
      if (fScore < fBestScore) {
        iFlipCandidate = iVar;
        fBestScore = aVarScoreW[iVar];
      } else {
        if (fScore == fBestScore) {
          if (aVarLastChange[iVar] < aVarLastChange[iFlipCandidate]) {
            iFlipCandidate = iVar;
          }
        }
      }
    }
  } else {
    PickNoveltyPlusPlusW();
  }
}

void PickG2WSatNoveltyPlusOldestW() {

  /* weighted varaint -- see regular algorithm for comments */
 
  UINT32 j;
  UINT32 iVar;

  if (iNumDecPromVars > 0 ) {
    iFlipCandidate = aDecPromVarsList[0];
    for (j=1;j<iNumDecPromVars;j++) {
      iVar = aDecPromVarsList[j];
      if (aVarLastChange[iVar] < aVarLastChange[iFlipCandidate]) {
        iFlipCandidate = iVar;
      }
    }
  } else {
    PickNoveltyPlusW();
  }
}

void PickGSatTabuW() {
  
  UINT32 j;
  FLOAT fScore;
  UBIGINT iTabuCutoff;

  /* calculation of tabu cutoff */

  if (iStep > iTabuTenure) {
    iTabuCutoff = iStep - iTabuTenure;
    if (iVarLastChangeReset > iTabuCutoff) {
      iTabuCutoff = iVarLastChangeReset;
    }
  } else {
    iTabuCutoff = 1;
  }

  iNumCandidates = 0;
  fBestScore = fTotalWeight;

  for (j=1;j<=iNumVars;j++) {
    
    /* check score of all non-tabu variables */

    if (aVarLastChange[j] < iTabuCutoff) { 
      
      /* use cached value of weighted score */

      fScore = aVarScoreW[j];

      /* build candidate list of best vars */

      if (fScore <= fBestScore) {
        if (fScore < fBestScore) {
          iNumCandidates=0;
          fBestScore = fScore;
        }
        aCandidateList[iNumCandidates++] = j;
      }
    }
  }

  /* select flip candidate uniformly from candidate list */

  if (iNumCandidates > 1) {
    iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
  } else {
    iFlipCandidate = aCandidateList[0];
  }
}

void PickGSatW() {
  
  UINT32 j;
  FLOAT fScore;

  iNumCandidates = 0;
  fBestScore = fTotalWeight;

  /* check score of all variables */

  for (j=1;j<=iNumVars;j++) {
    
    /* use cached value of weighted score */

    fScore = aVarScoreW[j];

    /* build candidate list of best vars */

    if (fScore <= fBestScore) {
      if (fScore < fBestScore) {
        iNumCandidates=0;
        fBestScore = fScore;
      }
      aCandidateList[iNumCandidates++] = j;
    }
  }
  
  /* select flip candidate uniformly from candidate list */
  
  if (iNumCandidates > 1) {
    iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
  } else {
    iFlipCandidate = aCandidateList[0];
  }
}

void PickGWSatW() {
  UINT32 j;
  FLOAT fScore;

  /* with probability (iWp) uniformly choose a variable from all
     variables that appear in false clauses */

  if (RandomProb(iWp)) {
    if (iNumVarsInFalseList) {
      iFlipCandidate = aVarInFalseList[RandomInt(iNumVarsInFalseList)];
    } else {
      iFlipCandidate = 0;
    }
  } else {

    iNumCandidates = 0;
    fBestScore = fTotalWeight;

    /* check score of all variables */

    for (j=1;j<=iNumVars;j++) {
      
      /* use cached value of weighted score */

      fScore = aVarScoreW[j];

      /* build candidate list of best vars */

      if (fScore <= fBestScore) {
        if (fScore < fBestScore) {
          iNumCandidates=0;
          fBestScore = fScore;
        }
        aCandidateList[iNumCandidates++] = j;
      }
    }
  
    /* select flip candidate uniformly from candidate list */
    
    if (iNumCandidates > 1) {
      iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
    } else {
      iFlipCandidate = aCandidateList[0];
    }
  }
}

void PickHSatW() {
  
  UINT32 j;
  FLOAT fScore;

  iNumCandidates = 0;
  fBestScore = fTotalWeight;

  /* check score of all variables */

  for (j=1;j<=iNumVars;j++) {

    /* use cached value of weighted score */

    fScore = aVarScoreW[j];

    /* build candidate list of best vars */

    if (fScore <= fBestScore) {
      
      /* if 2 variables are tied, and one is 'older' then choose older var */

      if ((fScore < fBestScore)||(aVarLastChange[j]<aVarLastChange[*aCandidateList])) {
        iNumCandidates=0;
        fBestScore = fScore;
      }
      aCandidateList[iNumCandidates++] = j;
    }

  }
  
  /* select flip candidate uniformly from candidate list */
  
  if (iNumCandidates > 1) {
    iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
  } else {
    iFlipCandidate = aCandidateList[0];
  }
}

void PickHWSatW() {
  UINT32 iClause;
  LITTYPE litPick;

  /* with probability (iWp) uniformly choose an unsatisfied clause,
     and then uniformly choose a literal from that clause */

  if (RandomProb(iWp)) {
    if (iNumFalse) {
      iClause = aFalseList[RandomInt(iNumFalse)];
      litPick = pClauseLits[iClause][RandomInt(aClauseLen[iClause])];
      iFlipCandidate = GetVarFromLit(litPick);
    } else {
      iFlipCandidate = 0;
    }
  } else {
    
     /* otherwise, perform a regular HWSAT step */

     PickHSatW();
  }
}

void PickNoveltyW() {

  /* weighted varaint -- see regular algorithm for comments */
 
  UINT32 i;
  UINT32 j;
  FLOAT fScore;
  UINT32 iClause;
  UINT32 iClauseLen;
  LITTYPE *pLit;
  UINT32 *pClause;
  UINT32 iNumOcc;
  UINT32 iVar;
  UINT32 iYoungestVar;
  FLOAT fSecondBestScore;
  UINT32 iBestVar=0;
  UINT32 iSecondBestVar=0;

  fBestScore = fTotalWeight;
  fSecondBestScore = fTotalWeight;

  /* select the clause according to a weighted scheme */

  if (iNumFalse) {
    iClause = PickClauseWCS();
    iClauseLen = aClauseLen[iClause];
  } else {
    iFlipCandidate = 0;
    return;
  }
  pLit = pClauseLits[iClause];
  iYoungestVar = GetVarFromLit(*pLit);
  for (j=0;j<iClauseLen;j++) {
    fScore = FLOATZERO;
    iVar = GetVarFromLit(*pLit);
    iNumOcc = aNumLitOcc[*pLit];
    pClause = pLitClause[*pLit];
    for (i=0;i<iNumOcc;i++) {
      if (aNumTrueLit[*pClause]==0) {
        fScore -= aClauseWeight[*pClause];
      }
      pClause++;
    }
    iNumOcc = aNumLitOcc[GetNegatedLit(*pLit)];
    pClause = pLitClause[GetNegatedLit(*pLit)];
    for (i=0;i<iNumOcc;i++) {
      if (aNumTrueLit[*pClause]==1) {
        fScore += aClauseWeight[*pClause];
      }
      pClause++;
    }
    if (aVarLastChange[iVar] > aVarLastChange[iYoungestVar]) {
      iYoungestVar = iVar;
    }
    if ((fScore < fBestScore) || ((fScore == fBestScore) && (aVarLastChange[iVar] < aVarLastChange[iBestVar]))) {
      iSecondBestVar = iBestVar;
      iBestVar = iVar;
      fSecondBestScore = fBestScore;
      fBestScore = fScore;
    } else if ((fScore < fSecondBestScore) || ((fScore == fSecondBestScore) && (aVarLastChange[iVar] < aVarLastChange[iSecondBestVar]))) {
      iSecondBestVar = iVar;
      fSecondBestScore = fScore;
    }
    pLit++;
  }
  iFlipCandidate = iBestVar;
  if (iFlipCandidate != iYoungestVar) {
    return;
  }
  if (RandomProb(iNovNoise)) {
    iFlipCandidate = iSecondBestVar;
  }
}

void PickNoveltyPlusW() {

  /* weighted varaint -- see regular algorithm for comments */
 
  UINT32 iClause;
  UINT32 iClauseLen;
  LITTYPE litPick;
  
  if (RandomProb(iWp)) {
    if (iNumFalse) {
      iClause = aFalseList[RandomInt(iNumFalse)];
      iClauseLen = aClauseLen[iClause];
      litPick = (pClauseLits[iClause][RandomInt(iClauseLen)]);
      iFlipCandidate = GetVarFromLit(litPick);
    } else {
      iFlipCandidate = 0;
    }
  } else {
    PickNoveltyW();
  }
}

void PickNoveltyPlusPlusW() {

  /* weighted varaint -- see regular algorithm for comments */
 
  UINT32 j;
  UINT32 iClause;
  UINT32 iClauseLen;
  UINT32 iVar;
  LITTYPE *pLit;

  if (RandomProb(iDp)) {
    if (iNumFalse) {
      iClause = aFalseList[RandomInt(iNumFalse)];
      iClauseLen = aClauseLen[iClause];
      pLit = pClauseLits[iClause];
      iFlipCandidate = GetVarFromLit(*pLit);
      pLit++;
      for (j=1;j<iClauseLen;j++) {
        iVar = GetVarFromLit(*pLit);
        if (aVarLastChange[iVar] < aVarLastChange[iFlipCandidate]) {
          iFlipCandidate = iVar;
        }
        pLit++;
      }
    } else {
      iFlipCandidate = 0;
    }
  } else {
    PickNoveltyW();
  }
}

void PickRGSatW() {

  /* weighted varaint -- see regular algorithm for comments */
  
  UINT32 j;
  FLOAT fScore;

  iNumCandidates = 0;
  fBestScore = fTotalWeight;
  for (j=1;j<=iNumVars;j++) {
    fScore = aVarScoreW[j];
    if (fScore <= fBestScore) {
      if (fScore < fBestScore) {
        iNumCandidates=0;
        fBestScore = fScore;
      }
      aCandidateList[iNumCandidates++] = j;
    }
  }
  if (fBestScore < FLOATZERO) {   
    if (iNumCandidates > 1) {
      iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
    } else {
      iFlipCandidate = aCandidateList[0];
    }
  } else {
    iFlipCandidate = 0;
    bRestart = TRUE;
  }
}

void PickRoTSW() {

  /* weighted varaint -- see regular algorithm for comments */
  
  UINT32 j;
  FLOAT fScore;
  UBIGINT iTabuCutoff;

  if (iTabuTenureLow != iTabuTenureHigh) {
    if ((iStep % iNumVars)==0) {
      iTabuTenure = iTabuTenureLow + RandomInt(iTabuTenureHigh - iTabuTenureLow);
    }
  }
  if (iStep > iTabuTenure) {
    iTabuCutoff = iStep - iTabuTenure;
    if (iVarLastChangeReset > iTabuCutoff) {
      iTabuCutoff = iVarLastChangeReset;
    }
  } else {
    iTabuCutoff = 1;
  }
  iNumCandidates = 0;
  fBestScore = fTotalWeight;
  for (j=1;j<=iNumVars;j++) {
    fScore = aVarScoreW[j];
    if (aVarLastChange[j] >= iTabuCutoff) { 
      if ((fSumFalseW + fScore) < fBestSumFalseW) {
        iFlipCandidate = j;
        return;
      }
    } else if ((iStep - aVarLastChange[j]) > (iNumVars * 10)) {
      iFlipCandidate = j;
      return;
    } else { 
      if (fScore <= fBestScore) {
        if (fScore < fBestScore) {
          iNumCandidates=0;
          fBestScore = fScore;
        }
        aCandidateList[iNumCandidates++] = j;
      }
    }
  }
  if (iNumCandidates > 1) {
    iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
  } else {
    iFlipCandidate = aCandidateList[0];
  }
}

void SAMDUpdateVarLastChangeW() {
  if (fBestScore >= FLOATZERO) {
    UpdateVarLastChange();
  }
}

void SmoothSAPSWSmooth() {
  
  UINT32 j;
  UINT32 k;
  FLOAT fOld;
  FLOAT fDiff;
  LITTYPE *pLit;

  fTotalPenaltyFL = FLOATZERO;
 
  for(j=0;j<iNumClauses;j++) {

  /* smooth penalties back towards original clause weights */

    fOld = aClausePenaltyFL[j];
    aClausePenaltyFL[j] = aClausePenaltyFL[j] * fRho + aClauseWeight[j] * (1-fRho);
    fDiff = aClausePenaltyFL[j] - fOld;

    if (aNumTrueLit[j]==0) {
      pLit = pClauseLits[j];
      for (k=0;k<aClauseLen[j];k++) {
        aMakePenaltyFL[GetVarFromLit(*pLit)] += fDiff;
        pLit++;
      }
    }
    if (aNumTrueLit[j]==1) {
      aBreakPenaltyFL[aCritSat[j]] += fDiff;
    }
    fTotalPenaltyFL += aClausePenaltyFL[j];
  }

}

void PostFlipSAPSWSmooth() {
  if (iFlipCandidate) {
    return;
  }

  if (RandomProb(iPs)) {
    SmoothSAPSWSmooth();
  }

  /* no call to AdjustPenalties() for WSmooth variant*/
  
  ScaleSAPS();

}

UINT32 PickClauseWCS() {

  /* this routine randomly selects a weighted clause so that
     clauses with larger weights are more likely to be selected 
     ('roulette' selection) */

  UINT32 j;
  FLOAT fRandClause;
  FLOAT fClauseSum;
  UINT32 iClause = 0;

  fRandClause = RandomFloat() * fSumFalseW;

  fClauseSum = FLOATZERO;

  for (j=0;j<iNumFalse;j++) {
    iClause = aFalseList[j];
    fClauseSum += aClauseWeight[iClause];
    if (fRandClause < fClauseSum) {
      break;
    }
  }
  return(iClause);
}


void PickWalkSatSKCW() {

  /* weighted varaint -- see regular algorithm for comments */

  UINT32 i;
  UINT32 j;
  FLOAT fScore;
  UINT32 iClause;
  UINT32 iClauseLen;
  UINT32 iVar;
  LITTYPE *pLit;
  UINT32 *pClause;
  LITTYPE litPick;
  UINT32 iNumOcc;

  iNumCandidates = 0;
  fBestScore = fTotalWeight;

  /* select the clause according to a weighted scheme */

  if (iNumFalse) {
    iClause = PickClauseWCS();
    iClauseLen = aClauseLen[iClause];
  } else {
    iFlipCandidate = 0;
    return;
  }

  pLit = pClauseLits[iClause];
  for (j=0;j<iClauseLen;j++) {
    fScore = FLOATZERO;
    iVar = GetVarFromLit(*pLit);
    iNumOcc = aNumLitOcc[GetNegatedLit(*pLit)];
    pClause = pLitClause[GetNegatedLit(*pLit)];
    for (i=0;i<iNumOcc;i++) {
      if (aNumTrueLit[*pClause]==1) {
        fScore += aClauseWeight[*pClause];
      }
      pClause++;
    }
    if (fScore <= fBestScore) {
      if (fScore < fBestScore) {
        iNumCandidates=0;
        fBestScore = fScore;
      }
      aCandidateList[iNumCandidates++] = iVar;
    }
    pLit++;
  }
  if (fBestScore > FLOATZERO) {
    if (RandomProb(iWp)) {
      litPick = pClauseLits[iClause][RandomInt(iClauseLen)];
      iFlipCandidate = GetVarFromLit(litPick);
      return;
    }
  }
  if (iNumCandidates > 1) {
    iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
  } else {
    iFlipCandidate = aCandidateList[0];
  }
}

void PickWalkSatTabuW() {
 
  UINT32 i;
  UINT32 j;
  FLOAT fScore;
  UINT32 iClauseLen;
  LITTYPE *pLit;
  UINT32 *pClause;
  UINT32 iNumOcc;
  UINT32 iVar;
  
  UBIGINT iTabuCutoff;

  iNumCandidates = 0;
  fBestScore = fTotalWeight;

  /* calculation of tabu cutoff */

  if (iStep > iTabuTenure) {
    iTabuCutoff = iStep - iTabuTenure;
    if (iVarLastChangeReset > iTabuCutoff) {
      iTabuCutoff = iVarLastChangeReset;
    }
  } else {
    iTabuCutoff = 1;
  }

  /* select the clause according to a weighted scheme */

  iWalkSATTabuClause = PickClauseWCS();
  iClauseLen = aClauseLen[iWalkSATTabuClause];

  pLit = pClauseLits[iWalkSATTabuClause];

  for (j=0;j<iClauseLen;j++) {

    /* for WalkSAT variants, it's faster to calculate the
       score for each literal than to cache the values 
    
       note that in this case, score is the breakcount[] */

    fScore = FLOATZERO;
      
    pClause = pLitClause[GetNegatedLit(*pLit)];

    iVar = GetVarFromLit(*pLit);
    
    iNumOcc = aNumLitOcc[GetNegatedLit(*pLit)];
    
    for (i=0;i<iNumOcc;i++) {
      if (aNumTrueLit[*pClause]==1) {
        fScore += aClauseWeight[*pClause];
      }
      pClause++;
    }
    
    /* variables with breakcount (score) = 0 are never tabu */

    if ((fScore==FLOATZERO)||(aVarLastChange[iVar] < iTabuCutoff)) { 

      /* build candidate list of best vars */

      if (fScore <= fBestScore) {
        if (fScore < fBestScore) {
          iNumCandidates=0;
          fBestScore = fScore;
        }
        aCandidateList[iNumCandidates++] = iVar;
      }
    }
    pLit++;
  }

  /* perform a null flip if no candidates exist */

  if (iNumCandidates == 0) {
    iFlipCandidate = 0;
    return;
  }

  /* select flip candidate uniformly from candidate list */
  
  if (iNumCandidates > 1) {
    iFlipCandidate = aCandidateList[RandomInt(iNumCandidates)];
  } else {
    iFlipCandidate = aCandidateList[0];
  }
}




void AddWeighted() {

  ALGORITHM *pCurAlg;

  pCurAlg = CreateAlgorithm("adaptnovelty+","",TRUE,
    "Adaptive Novelty+: Novelty+ with adaptive noise (weighted)",
    "Hoos [AAAI 02]",
    "PickNoveltyPlusW",
    "DefaultProceduresW,Flip+FalseClauseListW,AdaptNoveltyNoiseW,VarLastChange",
    "default_w","default");
  CopyParameters(pCurAlg,"adaptnovelty+","",FALSE);
  CreateTrigger("AdaptNoveltyNoiseW",PostFlip,AdaptNoveltyNoiseW,"InitAdaptNoveltyNoise","");

  pCurAlg = CreateAlgorithm("g2wsat","",TRUE,
    "G2WSAT: Gradient-based Greedy WalkSAT (weighted)",
    "Li, Huang  [SAT 05]",
    "PickG2WSatW",
    "DefaultProceduresW,Flip+TrackChanges+FCL+W,DecPromVarsW,FalseClauseList,VarLastChange",
    "default_w","default");
  CopyParameters(pCurAlg,"g2wsat","",FALSE);
  CreateTrigger("PickG2WSatW",ChooseCandidate,PickG2WSatW,"","");

  pCurAlg = CreateAlgorithm("g2wsat","novelty+oldest",TRUE,
    "G2WSAT: uses Nov+ & oldest dec. prom var) (weighted)",
    "Li, Wei, and Zhang [SAT 07]",
    "PickG2WSatNoveltyPlusOldestW",
    "DefaultProcedures,Flip+TrackChanges+FCL,DecPromVars,FalseClauseList,VarLastChange",
    "default","default");
  CopyParameters(pCurAlg,"novelty+","",FALSE);
  CreateTrigger("PickG2WSatNoveltyPlusOldestW",ChooseCandidate,PickG2WSatNoveltyPlusOldestW,"","");

  pCurAlg = CreateAlgorithm("gsat-tabu","",TRUE,
    "GSAT-TABU: GSAT with Tabu search (weighted)",
    "Mazure, Sais, Gregoire [AAAI 97]",
    "PickGSatTabuW",
    "DefaultProceduresW,Flip+VarScoreW,VarLastChange",
    "default_w","default");
  CopyParameters(pCurAlg,"gsat-tabu","",FALSE);
  CreateTrigger("PickGSatTabuW",ChooseCandidate,PickGSatTabuW,"","");

  pCurAlg = CreateAlgorithm("gsat","",TRUE,
    "GSAT: Greedy search for SAT (weighted)",
    "Selman, Levesque, Mitchell [AAAI 93]",
    "PickGSatW",
    "DefaultProceduresW,Flip+VarScoreW",
    "default_w","default");
  CreateTrigger("PickGSatW",ChooseCandidate,PickGSatW,"","");

  pCurAlg = CreateAlgorithm("gwsat","",TRUE,
    "GWSAT: GSAT with random walk (weighted)",
    "Selman, Kautz [IJCAI 93]",
    "PickGWSatW",
    "DefaultProceduresW,VarScoreW,VarInFalse",
    "default_w","default");
  CopyParameters(pCurAlg,"gwsat","",FALSE);
  CreateTrigger("PickGWSatW",ChooseCandidate,PickGWSatW,"","");

  pCurAlg = CreateAlgorithm("hsat","",TRUE,
    "HSAT (weighted)",
    "Gent, Walsh [AAAI 93]",
    "PickHSatW",
    "DefaultProceduresW,Flip+VarScoreW,VarLastChange",
    "default_w","default");
  CreateTrigger("PickHSatW",ChooseCandidate,PickHSatW,"","");

  pCurAlg = CreateAlgorithm("hwsat","",TRUE,
    "HWSAT: HSAT with random walk (weighted)",
    "Gent, Walsh [Hybrid Problems... 95]",
    "PickHWSatW",
    "DefaultProceduresW,Flip+VarScoreW,VarLastChange,FalseClauseList",
    "default_w","default");
  CopyParameters(pCurAlg,"hwsat","",FALSE);
  CreateTrigger("PickHWSatW",ChooseCandidate,PickHWSatW,"","");

  pCurAlg = CreateAlgorithm("irots","",TRUE,
    "IRoTS: Iterated Robust TABU Search (weighted)",
    "Smyth, Hoos, Stuetzle [AI 2003]",
    "PickRoTSW,PostStepIRoTS",
    "DefaultProceduresW,Flip+VarScoreW,CreateIRoTSBackup",
    "default_w","default");
  CopyParameters(pCurAlg,"irots","",FALSE);

  pCurAlg = CreateAlgorithm("novelty","",TRUE,
    "Novelty: (weighted)",
    "McAllester, Selman, Kautz [AAAI 97]",
    "PickNoveltyW",
    "DefaultProceduresW,Flip+FalseClauseListW,VarLastChange",
    "default_w","default");
  CopyParameters(pCurAlg,"novelty","",FALSE);
  CreateTrigger("PickNoveltyW",ChooseCandidate,PickNoveltyW,"","");

  pCurAlg = CreateAlgorithm("novelty+","",TRUE,
    "Novelty+: Novelty with random walk (weighted)",
    "Hoos [AAAI 99]",
    "PickNoveltyPlusW",
    "DefaultProceduresW,Flip+FalseClauseListW,VarLastChange",
    "default_w","default");
  CopyParameters(pCurAlg,"novelty+","",FALSE);
  CreateTrigger("PickNoveltyPlusW",ChooseCandidate,PickNoveltyPlusW,"","");
 
  pCurAlg = CreateAlgorithm("novelty++","",TRUE,
    "Novelty++: Novelty+ with a modified diversification mechanism (weighted)",
    "Li, Huang  [SAT 05]",
    "PickNoveltyPlusPlusW",
    "DefaultProceduresW,Flip+FalseClauseListW,VarLastChange",
    "default_w","default");
  CopyParameters(pCurAlg,"novelty++","",FALSE);
  CreateTrigger("PickNoveltyPlusPlusW",ChooseCandidate,PickNoveltyPlusPlusW,"","");

  pCurAlg = CreateAlgorithm("urwalk","",TRUE,
    "Uninformed Random Walk: flip any var at random (weighted)",
    "",
    "PickURWalk",
    "DefaultProceduresW",
    "default_w","default");

  pCurAlg = CreateAlgorithm("crwalk","",TRUE,
    "Conflict-Directed Random Walk (weighted)",
    "Papadimitriou [FOCS 91]",
    "PickCRWalk",
    "DefaultProceduresW,Flip+FalseClauseListW",
    "default_w","default");
  
  pCurAlg = CreateAlgorithm("crwalk","schoening",TRUE,
    "Conflict-Directed Random Walk, restart every 3n steps (weighted)",
    "Schoening [FOCS 99]",
    "PickCRWalk,SchoeningRestart",
    "DefaultProceduresW,Flip+FalseClauseListW",
    "default_w","default");

  pCurAlg = CreateAlgorithm("rgsat","",TRUE,
    "RGSAT: Restarting GSAT (poor algorithm -- academic use only) (weighted)",
    "Tompkins, Hoos [SAIM 04]",
    "PickRGSatW",
    "DefaultProceduresW,Flip+VarScoreW",
    "default_w","default");
  CreateTrigger("PickRGSatW",CheckRestart,PickRGSatW,"","");

  pCurAlg = CreateAlgorithm("rots","",TRUE,
    "RoTS: Robust TABU Search (weighted)",
    "Taillard [Parallel Computing 1991], based on implementation by Stuetzle",
    "PickRoTSW",
    "DefaultProceduresW,Flip+VarScoreW",
    "default_w","default");
  CopyParameters(pCurAlg,"rots","",FALSE);
  CreateTrigger("PickRoTSW",ChooseCandidate,PickRoTSW,"InitRoTS,VarLastChange,BestFalse","");

  pCurAlg = CreateAlgorithm("samd","",TRUE,
    "SAMD: Steepest Ascent Mildest Descent (weighted)",
    "Hansen and Jaumard [Computing 1990]",
    "PickGSatTabuW,SAMDUpdateVarLastChangeW",
    "DefaultProceduresW,Flip+VarScoreW",
    "default_w","default");
  CopyParameters(pCurAlg,"gsat-tabu","",1);
  CreateTrigger("SAMDUpdateVarLastChangeW",UpdateStateInfo,SAMDUpdateVarLastChangeW,"VarLastChange","UpdateVarLastChange");

   pCurAlg = CreateAlgorithm("saps","winit",TRUE,
    "SAPS: (weighted -- init to weights)",
    "Hutter, Tompkins, Hoos [CP 02]",
    "PickSAPS,PostFlipSAPS",
    "DefaultProceduresW,InitClausePenaltyFLW,Flip+MBPFL+FCL+VIF+W",
    "default_w","default");
  CopyParameters(pCurAlg,"saps","",FALSE);

  pCurAlg = CreateAlgorithm("saps","wsmooth",TRUE,
    "SAPS: (weighted -- init and smooth to weights)",
    "Hutter, Tompkins, Hoos [CP 02]",
    "PickSAPS,PostFlipSAPSWSmooth",
    "DefaultProceduresW,InitClausePenaltyFLW,Flip+MBPFL+FCL+VIF+W",
    "default_w","default");
  CopyParameters(pCurAlg,"saps","",FALSE);
  CreateTrigger("PostFlipSAPSWSmooth",PostFlip,PostFlipSAPSWSmooth,"","");

  pCurAlg = CreateAlgorithm("walksat","",TRUE,
    "WALKSAT: Original WalkSAT algorithm (SKC variant) (weighted)",
    "Selman, Kautz, Cohen [AAAI 94]",
    "PickWalkSatSKCW",
    "DefaultProceduresW,Flip+FalseClauseListW",
    "default_w","default");
  CopyParameters(pCurAlg,"walksat","",FALSE);
  CreateTrigger("PickWalkSatSKCW",ChooseCandidate,PickWalkSatSKCW,"","");

  pCurAlg = CreateAlgorithm("walksat-tabu","",TRUE,
    "WALKSAT-TABU: WalkSAT with TABU search (weighted)",
    "McAllester, Selman, Kautz [AAAI 97]",
    "PickWalkSatTabuW",
    "DefaultProceduresW,Flip+FalseClauseListW,VarLastChange",
    "default_w","default");
  CopyParameters(pCurAlg,"walksat-tabu","",FALSE);
  CreateTrigger("PickWalkSatTabuW",ChooseCandidate,PickWalkSatTabuW,"","");

 
}


#ifdef __cplusplus

}
#endif
