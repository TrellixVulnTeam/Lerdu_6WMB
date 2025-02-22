//===- LoopVectorize.cpp - A Loop Vectorizer ------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a simple loop vectorizer. We currently only support single block
// loops. We have a very simple and restrictive legality check: we need to read
// and write from disjoint memory locations. We still don't have a cost model.
// This pass has three parts:
// 1. The main loop pass that drives the different parts.
// 2. LoopVectorizationLegality - A helper class that checks for the legality
//    of the vectorization.
// 3. SingleBlockLoopVectorizer - A helper class that performs the actual
//    widening of instructions.
//
//===----------------------------------------------------------------------===//
#define LV_NAME "loop-vectorize"
#define DEBUG_TYPE LV_NAME
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/LLVMContext.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Value.h"
#include "llvm/Function.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Module.h"
#include "llvm/Type.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/DataLayout.h"
#include "llvm/Transforms/Utils/Local.h"
#include <algorithm>
using namespace llvm;

static cl::opt<unsigned>
DefaultVectorizationFactor("default-loop-vectorize-width",
                          cl::init(4), cl::Hidden,
                          cl::desc("Set the default loop vectorization width"));

namespace {

/// Vectorize a simple loop. This class performs the widening of simple single
/// basic block loops into vectors. It does not perform any
/// vectorization-legality checks, and just does it.  It widens the vectors
/// to a given vectorization factor (VF).
class SingleBlockLoopVectorizer {
public:
  /// Ctor.
  SingleBlockLoopVectorizer(Loop *OrigLoop, ScalarEvolution *Se, LoopInfo *Li,
                            LPPassManager *Lpm, unsigned VecWidth):
  Orig(OrigLoop), SE(Se), LI(Li), LPM(Lpm), VF(VecWidth),
   Builder(0), Induction(0), OldInduction(0) { }

  ~SingleBlockLoopVectorizer() {
    delete Builder;
  }

  // Perform the actual loop widening (vectorization).
  void vectorize() {
    ///Create a new empty loop. Unlink the old loop and connect the new one.
    createEmptyLoop();
    /// Widen each instruction in the old loop to a new one in the new loop.
    vectorizeLoop();
    // register the new loop.
    cleanup();
 }

private:
  /// Create an empty loop, based on the loop ranges of the old loop.
  void createEmptyLoop();
  /// Copy and widen the instructions from the old loop.
  void vectorizeLoop();
  /// Insert the new loop to the loop hierarchy and pass manager.
  void cleanup();

  /// This instruction is un-vectorizable. Implement it as a sequence
  /// of scalars.
  void scalarizeInstruction(Instruction *Instr);

  /// Create a broadcast instruction. This method generates a broadcast
  /// instruction (shuffle) for loop invariant values and for the induction
  /// value. If this is the induction variable then we extend it to N, N+1, ...
  /// this is needed because each iteration in the loop corresponds to a SIMD
  /// element.
  Value *getBroadcastInstrs(Value *V);

  /// This is a helper function used by getBroadcastInstrs. It adds 0, 1, 2 ..
  /// for each element in the vector. Starting from zero.
  Value *getConsecutiveVector(Value* Val);

  /// Check that the GEP operands are all uniform except for the last index
  /// which has to be the induction variable.
  bool isConsecutiveGep(GetElementPtrInst *Gep);

  /// When we go over instructions in the basic block we rely on previous
  /// values within the current basic block or on loop invariant values.
  /// When we widen (vectorize) values we place them in the map. If the values
  /// are not within the map, they have to be loop invariant, so we simply
  /// broadcast them into a vector.
  Value *getVectorValue(Value *V);

  typedef DenseMap<Value*, Value*> ValueMap;

  /// The original loop.
  Loop *Orig;
  // Scev analysis to use.
  ScalarEvolution *SE;
  // Loop Info.
  LoopInfo *LI;
  // Loop Pass Manager;
  LPPassManager *LPM;
  // The vectorization factor to use.
  unsigned VF;

  // The builder that we use
  IRBuilder<> *Builder;

  // --- Vectorization state ---

  /// The new Induction variable which was added to the new block.
  PHINode *Induction;
  /// The induction variable of the old basic block.
  PHINode *OldInduction;
  // Maps scalars to widened vectors.
  ValueMap WidenMap;
};

/// Perform the vectorization legality check. This class does not look at the
/// profitability of vectorization, only the legality. At the moment the checks
/// are very simple and focus on single basic block loops with a constant
/// iteration count and no reductions.
class LoopVectorizationLegality {
public:
  LoopVectorizationLegality(Loop *Lp, ScalarEvolution *Se, DataLayout *Dl):
  TheLoop(Lp), SE(Se), DL(Dl) { }

  /// Returns the maximum vectorization factor that we *can* use to vectorize
  /// this loop. This does not mean that it is profitable to vectorize this
  /// loop, only that it is legal to do so. This may be a large number. We
  /// can vectorize to any SIMD width below this number.
  unsigned getLoopMaxVF();

private:
  /// Check if a single basic block loop is vectorizable.
  /// At this point we know that this is a loop with a constant trip count
  /// and we only need to check individual instructions.
  bool canVectorizeBlock(BasicBlock &BB);

  // Check if a pointer value is known to be disjoint.
  // Example: Alloca, Global, NoAlias.
  bool isIdentifiedSafeObject(Value* Val);

  /// The loop that we evaluate.
  Loop *TheLoop;
  /// Scev analysis.
  ScalarEvolution *SE;
  /// DataLayout analysis.
  DataLayout *DL;
};

struct LoopVectorize : public LoopPass {
  static char ID; // Pass identification, replacement for typeid

  LoopVectorize() : LoopPass(ID) {
    initializeLoopVectorizePass(*PassRegistry::getPassRegistry());
  }

  ScalarEvolution *SE;
  DataLayout *DL;
  LoopInfo *LI;

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) {
    // Only vectorize innermost loops.
    if (!L->empty())
      return false;

    SE = &getAnalysis<ScalarEvolution>();
    DL = getAnalysisIfAvailable<DataLayout>();
    LI = &getAnalysis<LoopInfo>();

    DEBUG(dbgs() << "LV: Checking a loop in \"" <<
          L->getHeader()->getParent()->getName() << "\"\n");

    // Check if it is legal to vectorize the loop.
    LoopVectorizationLegality LVL(L, SE, DL);
    unsigned MaxVF = LVL.getLoopMaxVF();

    // Check that we can vectorize using the chosen vectorization width.
    if (MaxVF < DefaultVectorizationFactor) {
      DEBUG(dbgs() << "LV: non-vectorizable MaxVF ("<< MaxVF << ").\n");
      return false;
    }

    DEBUG(dbgs() << "LV: Found a vectorizable loop ("<< MaxVF << ").\n");

    // If we decided that is is *legal* to vectorizer the loop. Do it.
    SingleBlockLoopVectorizer LB(L, SE, LI, &LPM, DefaultVectorizationFactor);
    LB.vectorize();

    DEBUG(verifyFunction(*L->getHeader()->getParent()));
    return true;
  }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    LoopPass::getAnalysisUsage(AU);
    AU.addRequiredID(LoopSimplifyID);
    AU.addRequired<LoopInfo>();
    AU.addRequired<ScalarEvolution>();
  }

};

Value *SingleBlockLoopVectorizer::getBroadcastInstrs(Value *V) {
  // Instructions that access the old induction variable
  // actually want to get the new one.
  if (V == OldInduction)
    V = Induction;
  // Create the types.
  LLVMContext &C = V->getContext();
  Type *VTy = VectorType::get(V->getType(), VF);
  Type *I32 = IntegerType::getInt32Ty(C);
  Constant *Zero = ConstantInt::get(I32, 0);
  Value *Zeros = ConstantAggregateZero::get(VectorType::get(I32, VF));
  Value *UndefVal = UndefValue::get(VTy);
  // Insert the value into a new vector.
  Value *SingleElem = Builder->CreateInsertElement(UndefVal, V, Zero);
  // Broadcast the scalar into all locations in the vector.
  Value *Shuf = Builder->CreateShuffleVector(SingleElem, UndefVal, Zeros,
                                             "broadcast");
  // We are accessing the induction variable. Make sure to promote the
  // index for each consecutive SIMD lane. This adds 0,1,2 ... to all lanes.
  if (V == Induction)
    return getConsecutiveVector(Shuf);
  return Shuf;
}

Value *SingleBlockLoopVectorizer::getConsecutiveVector(Value* Val) {
  assert(Val->getType()->isVectorTy() && "Must be a vector");
  assert(Val->getType()->getScalarType()->isIntegerTy() &&
         "Elem must be an integer");
  // Create the types.
  Type *ITy = Val->getType()->getScalarType();
  VectorType *Ty = cast<VectorType>(Val->getType());
  unsigned VLen = Ty->getNumElements();
  SmallVector<Constant*, 8> Indices;

  // Create a vector of consecutive numbers from zero to VF.
  for (unsigned i = 0; i < VLen; ++i)
    Indices.push_back(ConstantInt::get(ITy, i));

  // Add the consecutive indices to the vector value.
  Constant *Cv = ConstantVector::get(Indices);
  assert(Cv->getType() == Val->getType() && "Invalid consecutive vec");
  return Builder->CreateAdd(Val, Cv, "induction");
}


bool SingleBlockLoopVectorizer::isConsecutiveGep(GetElementPtrInst *Gep) {
  if (!Gep)
    return false;

  unsigned NumOperands = Gep->getNumOperands();
  Value *LastIndex = Gep->getOperand(NumOperands - 1);

  // Check that all of the gep indices are uniform except for the last.
  for (unsigned i = 0; i < NumOperands - 1; ++i)
    if (!SE->isLoopInvariant(SE->getSCEV(Gep->getOperand(i)), Orig))
      return false;

  // We can emit wide load/stores only of the last index is the induction
  // variable.
  const SCEV *Last = SE->getSCEV(LastIndex);
  if (const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(Last)) {
    const SCEV *Step = AR->getStepRecurrence(*SE);

    // The memory is consecutive because the last index is consecutive
    // and all other indices are loop invariant.
    if (Step->isOne())
      return true;
  }

  return false;
}

Value *SingleBlockLoopVectorizer::getVectorValue(Value *V) {
  // If we saved a vectorized copy of V, use it.
  ValueMap::iterator it = WidenMap.find(V);
  if (it != WidenMap.end())
     return it->second;

  // Broadcast V and save the value for future uses.
  Value *B = getBroadcastInstrs(V);
  WidenMap[V] = B;
  return B;
}

void SingleBlockLoopVectorizer::scalarizeInstruction(Instruction *Instr) {
  assert(!Instr->getType()->isAggregateType() && "Can't handle vectors");
  // Holds vector parameters or scalars, in case of uniform vals.
  SmallVector<Value*, 8> Params;

  // Find all of the vectorized parameters.
  for (unsigned op = 0, e = Instr->getNumOperands(); op != e; ++op) {
    Value *SrcOp = Instr->getOperand(op);

    // If we are accessing the old induction variable, use the new one.
    if (SrcOp == OldInduction) {
      Params.push_back(getBroadcastInstrs(Induction));
      continue;
    }

    // Try using previously calculated values.
    Instruction *SrcInst = dyn_cast<Instruction>(SrcOp);

    // If the src is an instruction that appeared earlier in the basic block
    // then it should already be vectorized.
    if (SrcInst && SrcInst->getParent() == Instr->getParent()) {
      assert(WidenMap.count(SrcInst) && "Source operand is unavailable");
      // The parameter is a vector value from earlier.
      Params.push_back(WidenMap[SrcInst]);
    } else {
      // The parameter is a scalar from outside the loop. Maybe even a constant.
      Params.push_back(SrcOp);
    }
  }

  assert(Params.size() == Instr->getNumOperands() &&
         "Invalid number of operands");

  // Does this instruction return a value ?
  bool IsVoidRetTy = Instr->getType()->isVoidTy();
  Value *VecResults = 0;

  // If we have a return value, create an empty vector. We place the scalarized
  // instructions in this vector.
  if (!IsVoidRetTy)
    VecResults = UndefValue::get(VectorType::get(Instr->getType(), VF));

  // For each scalar that we create.
  for (unsigned i = 0; i < VF; ++i) {
    Instruction *Cloned = Instr->clone();
    if (!IsVoidRetTy)
      Cloned->setName(Instr->getName() + ".cloned");
    // Replace the operands of the cloned instrucions with extracted scalars.
    for (unsigned op = 0, e = Instr->getNumOperands(); op != e; ++op) {
      Value *Op = Params[op];
      // Param is a vector. Need to extract the right lane.
      if (Op->getType()->isVectorTy())
        Op = Builder->CreateExtractElement(Op, Builder->getInt32(i));
      Cloned->setOperand(op, Op);
    }

    // Place the cloned scalar in the new loop.
    Builder->Insert(Cloned);

    // If the original scalar returns a value we need to place it in a vector
    // so that future users will be able to use it.
    if (!IsVoidRetTy)
      VecResults = Builder->CreateInsertElement(VecResults, Cloned,
                                               Builder->getInt32(i));
  }

  if (!IsVoidRetTy)
    WidenMap[Instr] = VecResults;
}

void SingleBlockLoopVectorizer::createEmptyLoop() {
  /*
   In this function we generate a new loop. The new loop will contain
   the vectorized instructions while the old loop will continue to run the
   scalar remainder.

   [  ] <-- vector loop bypass.
  /  |
 /   v
|   [ ]     <-- vector pre header.
|    |
|    v
|   [  ] \
|   [  ]_|   <-- vector loop.
|    |
 \   v
   >[ ]   <--- middle-block.
  /  |
 /   v
|   [ ]     <--- new preheader.
|    |
|    v
|   [ ] \
|   [ ]_|   <-- old scalar loop to handle remainder.
 \   |
  \  v
   >[ ]     <-- exit block.
   ...
   */

  // This is the original scalar-loop preheader.
  BasicBlock *BypassBlock = Orig->getLoopPreheader();
  BasicBlock *ExitBlock = Orig->getExitBlock();
  assert(ExitBlock && "Must have an exit block");

  assert(Orig->getNumBlocks() == 1 && "Invalid loop");
  assert(BypassBlock && "Invalid loop structure");

  BasicBlock *VectorPH =
      BypassBlock->splitBasicBlock(BypassBlock->getTerminator(), "vector.ph");
  BasicBlock *VecBody = VectorPH->splitBasicBlock(VectorPH->getTerminator(),
                                                 "vector.body");

  BasicBlock *MiddleBlock = VecBody->splitBasicBlock(VecBody->getTerminator(),
                                                  "middle.block");
  BasicBlock *ScalarPH =
          MiddleBlock->splitBasicBlock(MiddleBlock->getTerminator(),
                                       "scalar.preheader");

  // Find the induction variable.
  BasicBlock *OldBasicBlock = Orig->getHeader();
  OldInduction = dyn_cast<PHINode>(OldBasicBlock->begin());
  assert(OldInduction && "We must have a single phi node.");
  Type *IdxTy = OldInduction->getType();

  // Use this IR builder to create the loop instructions (Phi, Br, Cmp)
  // inside the loop.
  Builder = new IRBuilder<>(VecBody);
  Builder->SetInsertPoint(VecBody->getFirstInsertionPt());

  // Generate the induction variable.
  Induction = Builder->CreatePHI(IdxTy, 2, "index");
  Constant *Zero = ConstantInt::get(IdxTy, 0);
  Constant *Step = ConstantInt::get(IdxTy, VF);

  // Find the loop boundaries.
  const SCEV *ExitCount = SE->getExitCount(Orig, Orig->getHeader());
  assert(ExitCount != SE->getCouldNotCompute() && "Invalid loop count");

  // Get the total trip count from the count by adding 1.
  ExitCount = SE->getAddExpr(ExitCount,
                             SE->getConstant(ExitCount->getType(), 1));

  // Expand the trip count and place the new instructions in the preheader.
  // Notice that the pre-header does not change, only the loop body.
  SCEVExpander Exp(*SE, "induction");
  Instruction *Loc = BypassBlock->getTerminator();

  // We may need to extend the index in case there is a type mismatch.
  // We know that the count starts at zero and does not overflow.
  // We are using Zext because it should be less expensive.
  if (ExitCount->getType() != Induction->getType())
    ExitCount = SE->getZeroExtendExpr(ExitCount, IdxTy);

  // Count holds the overall loop count (N).
  Value *Count = Exp.expandCodeFor(ExitCount, Induction->getType(), Loc);
  // Now we need to generate the expression for N - (N % VF), which is
  // the part that the vectorized body will execute.
  Constant *CIVF = ConstantInt::get(IdxTy, VF);
  Value *R = BinaryOperator::CreateURem(Count, CIVF, "n.mod.vf", Loc);
  Value *CountRoundDown = BinaryOperator::CreateSub(Count, R, "n.vec", Loc);

  // Now, compare the new count to zero. If it is zero, jump to the scalar part.
  Value *Cmp = CmpInst::Create(Instruction::ICmp, CmpInst::ICMP_EQ,
                               CountRoundDown, ConstantInt::getNullValue(IdxTy),
                               "cmp.zero", Loc);
  BranchInst::Create(MiddleBlock, VectorPH, Cmp, Loc);
  // Remove the old terminator.
  Loc->eraseFromParent();

  // Add a check in the middle block to see if we have completed
  // all of the iterations in the first vector loop.
  // If (N - N%VF) == N, then we *don't* need to run the remainder.
  Value *CmpN = CmpInst::Create(Instruction::ICmp, CmpInst::ICMP_EQ, Count,
                                CountRoundDown, "cmp.n",
                                MiddleBlock->getTerminator());

  BranchInst::Create(ExitBlock, ScalarPH, CmpN, MiddleBlock->getTerminator());
  // Remove the old terminator.
  MiddleBlock->getTerminator()->eraseFromParent();

  // Create i+1 and fill the PHINode.
  Value *NextIdx = Builder->CreateAdd(Induction, Step, "index.next");
  Induction->addIncoming(Zero, VectorPH);
  Induction->addIncoming(NextIdx, VecBody);
  // Create the compare.
  Value *ICmp = Builder->CreateICmpEQ(NextIdx, CountRoundDown);
  Builder->CreateCondBr(ICmp, MiddleBlock, VecBody);

  // Now we have two terminators. Remove the old one from the block.
  VecBody->getTerminator()->eraseFromParent();

  // Fix the scalar body iteration count.
  unsigned BlockIdx = OldInduction->getBasicBlockIndex(ScalarPH);
  OldInduction->setIncomingValue(BlockIdx, CountRoundDown);

  // Get ready to start creating new instructions into the vectorized body.
  Builder->SetInsertPoint(VecBody->getFirstInsertionPt());

  // Register the new loop.
  Loop* Lp = new Loop();
  LPM->insertLoop(Lp, Orig->getParentLoop());

  Lp->addBasicBlockToLoop(VecBody, LI->getBase());

  Loop *ParentLoop = Orig->getParentLoop();
  if (ParentLoop) {
    ParentLoop->addBasicBlockToLoop(ScalarPH, LI->getBase());
    ParentLoop->addBasicBlockToLoop(VectorPH, LI->getBase());
    ParentLoop->addBasicBlockToLoop(MiddleBlock, LI->getBase());
  }
}

void SingleBlockLoopVectorizer::vectorizeLoop() {
  BasicBlock &BB = *Orig->getHeader();

  // For each instruction in the old loop.
  for (BasicBlock::iterator it = BB.begin(), e = BB.end(); it != e; ++it) {
    Instruction *Inst = it;

    switch (Inst->getOpcode()) {
      case Instruction::PHI:
      case Instruction::Br:
        // Nothing to do for PHIs and BR, since we already took care of the
        // loop control flow instructions.
        continue;

      case Instruction::Add:
      case Instruction::FAdd:
      case Instruction::Sub:
      case Instruction::FSub:
      case Instruction::Mul:
      case Instruction::FMul:
      case Instruction::UDiv:
      case Instruction::SDiv:
      case Instruction::FDiv:
      case Instruction::URem:
      case Instruction::SRem:
      case Instruction::FRem:
      case Instruction::Shl:
      case Instruction::LShr:
      case Instruction::AShr:
      case Instruction::And:
      case Instruction::Or:
      case Instruction::Xor: {
        // Just widen binops.
        BinaryOperator *BinOp = dyn_cast<BinaryOperator>(Inst);
        Value *A = getVectorValue(Inst->getOperand(0));
        Value *B = getVectorValue(Inst->getOperand(1));
        // Use this vector value for all users of the original instruction.
        WidenMap[Inst] = Builder->CreateBinOp(BinOp->getOpcode(), A, B);
        break;
      }
      case Instruction::Select: {
        // Widen selects.
        Value *A = getVectorValue(Inst->getOperand(0));
        Value *B = getVectorValue(Inst->getOperand(1));
        Value *C = getVectorValue(Inst->getOperand(2));
        WidenMap[Inst] = Builder->CreateSelect(A, B, C);
        break;
      }

      case Instruction::ICmp:
      case Instruction::FCmp: {
        // Widen compares. Generate vector compares.
        bool FCmp = (Inst->getOpcode() == Instruction::FCmp);
        CmpInst *Cmp = dyn_cast<CmpInst>(Inst);
        Value *A = getVectorValue(Inst->getOperand(0));
        Value *B = getVectorValue(Inst->getOperand(1));
        if (FCmp)
          WidenMap[Inst] = Builder->CreateFCmp(Cmp->getPredicate(), A, B);
        else
          WidenMap[Inst] = Builder->CreateICmp(Cmp->getPredicate(), A, B);
        break;
      }

      case Instruction::Store: {
        // Attempt to issue a wide store.
        StoreInst *SI = dyn_cast<StoreInst>(Inst);
        Type *StTy = VectorType::get(SI->getValueOperand()->getType(), VF);
        Value *Ptr = SI->getPointerOperand();
        unsigned Alignment = SI->getAlignment();
        GetElementPtrInst *Gep = dyn_cast<GetElementPtrInst>(Ptr);
        // This store does not use GEPs.
        if (!isConsecutiveGep(Gep)) {
          scalarizeInstruction(Inst);
          break;
        }

        // Create the new GEP with the new induction variable.
        GetElementPtrInst *Gep2 = cast<GetElementPtrInst>(Gep->clone());
        unsigned NumOperands = Gep->getNumOperands();
        Gep2->setOperand(NumOperands - 1, Induction);
        Ptr = Builder->Insert(Gep2);
        Ptr = Builder->CreateBitCast(Ptr, StTy->getPointerTo());
        Value *Val = getVectorValue(SI->getValueOperand());
        Builder->CreateStore(Val, Ptr)->setAlignment(Alignment);
        break;
      }
      case Instruction::Load: {
        // Attempt to issue a wide load.
        LoadInst *LI = dyn_cast<LoadInst>(Inst);
        Type *RetTy = VectorType::get(LI->getType(), VF);
        Value *Ptr = LI->getPointerOperand();
        unsigned Alignment = LI->getAlignment();
        GetElementPtrInst *Gep = dyn_cast<GetElementPtrInst>(Ptr);

        // We don't have a gep. Scalarize the load.
        if (!isConsecutiveGep(Gep)) {
          scalarizeInstruction(Inst);
          break;
        }

        // Create the new GEP with the new induction variable.
        GetElementPtrInst *Gep2 = cast<GetElementPtrInst>(Gep->clone());
        unsigned NumOperands = Gep->getNumOperands();
        Gep2->setOperand(NumOperands - 1, Induction);
        Ptr = Builder->Insert(Gep2);
        Ptr = Builder->CreateBitCast(Ptr, RetTy->getPointerTo());
        LI = Builder->CreateLoad(Ptr);
        LI->setAlignment(Alignment);
        // Use this vector value for all users of the load.
        WidenMap[Inst] = LI;
        break;
      }
      case Instruction::ZExt:
      case Instruction::SExt:
      case Instruction::FPToUI:
      case Instruction::FPToSI:
      case Instruction::FPExt:
      case Instruction::PtrToInt:
      case Instruction::IntToPtr:
      case Instruction::SIToFP:
      case Instruction::UIToFP:
      case Instruction::Trunc:
      case Instruction::FPTrunc:
      case Instruction::BitCast: {
        /// Vectorize bitcasts.
        CastInst *CI = dyn_cast<CastInst>(Inst);
        Value *A = getVectorValue(Inst->getOperand(0));
        Type *DestTy = VectorType::get(CI->getType()->getScalarType(), VF);
        WidenMap[Inst] = Builder->CreateCast(CI->getOpcode(), A, DestTy);
        break;
      }

      default:
        /// All other instructions are unsupported. Scalarize them.
        scalarizeInstruction(Inst);
        break;
    }// end of switch.
  }// end of for_each instr.
}

void SingleBlockLoopVectorizer::cleanup() {
  // The original basic block.
  SE->forgetLoop(Orig);
}

unsigned LoopVectorizationLegality::getLoopMaxVF() {
  if (!TheLoop->getLoopPreheader()) {
    assert(false && "No preheader!!");
    DEBUG(dbgs() << "LV: Loop not normalized." << "\n");
    return  1;
  }

  // We can only vectorize single basic block loops.
  unsigned NumBlocks = TheLoop->getNumBlocks();
  if (NumBlocks != 1) {
    DEBUG(dbgs() << "LV: Too many blocks:" << NumBlocks << "\n");
    return 1;
  }

  // We need to have a loop header.
  BasicBlock *BB = TheLoop->getHeader();
  DEBUG(dbgs() << "LV: Found a loop: " << BB->getName() << "\n");

  // Go over each instruction and look at memory deps.
  if (!canVectorizeBlock(*BB)) {
    DEBUG(dbgs() << "LV: Can't vectorize this loop header\n");
    return 1;
  }

  // ScalarEvolution needs to be able to find the exit count.
  const SCEV *ExitCount = SE->getExitCount(TheLoop, BB);
  if (ExitCount == SE->getCouldNotCompute()) {
    DEBUG(dbgs() << "LV: SCEV could not compute the loop exit count.\n");
    return 1;
  }

  DEBUG(dbgs() << "LV: We can vectorize this loop!\n");

  // Okay! We can vectorize. At this point we don't have any other mem analysis
  // which may limit our maximum vectorization factor, so just return the
  // maximum SIMD size.
  return DefaultVectorizationFactor;
}

bool LoopVectorizationLegality::canVectorizeBlock(BasicBlock &BB) {
  // Holds the read and write pointers that we find.
  typedef SmallVector<Value*, 10> ValueVector;
  ValueVector Reads;
  ValueVector Writes;

  SmallPtrSet<Value*, 16> AnalyzedPtrs;
  unsigned NumPhis = 0;
  for (BasicBlock::iterator it = BB.begin(), e = BB.end(); it != e; ++it) {
    Instruction *I = it;

    PHINode *Phi = dyn_cast<PHINode>(I);
    if (Phi) {
      NumPhis++;
      // We only look at integer phi nodes.
      if (!Phi->getType()->isIntegerTy()) {
        DEBUG(dbgs() << "LV: Found an non-int PHI.\n");
        return false;
      }

      // If we found an induction variable.
      if (NumPhis > 1) {
        DEBUG(dbgs() << "LV: Found more than one PHI.\n");
        return false;
      }

      // This should not happen because the loop should be normalized.
      if (Phi->getNumIncomingValues() != 2) {
        DEBUG(dbgs() << "LV: Found an invalid PHI.\n");
        return false;
      }

      // Check that the PHI is consecutive and starts at zero.
      const SCEV *PhiScev = SE->getSCEV(Phi);
      const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(PhiScev);
      if (!AR) {
        DEBUG(dbgs() << "LV: PHI is not a poly recurrence.\n");
        return false;
      }

      const SCEV *Step = AR->getStepRecurrence(*SE);
      const SCEV *Start = AR->getStart();

      if (!Step->isOne() || !Start->isZero()) {
        DEBUG(dbgs() << "LV: PHI does not start at zero or steps by one.\n");
        return false;
      }
    }

    // If this is a load, record its pointer. If it is not a load, abort.
    // Notice that we don't handle function calls that read or write.
    if (I->mayReadFromMemory()) {
      LoadInst *Ld = dyn_cast<LoadInst>(I);
      if (!Ld) return false;
      if (!Ld->isSimple()) {
        DEBUG(dbgs() << "LV: Found a non-simple load.\n");
        return false;
      }

      Value* Ptr = Ld->getPointerOperand();
      if (AnalyzedPtrs.insert(Ptr))
        GetUnderlyingObjects(Ptr, Reads, DL);
    }

    // Record store pointers. Abort on all other instructions that write to
    // memory.
    if (I->mayWriteToMemory()) {
      StoreInst *St = dyn_cast<StoreInst>(I);
      if (!St) return false;
      if (!St->isSimple()) {
        DEBUG(dbgs() << "LV: Found a non-simple store.\n");
        return false;
      }

      Value* Ptr = St->getPointerOperand();
      if (AnalyzedPtrs.insert(Ptr))
        GetUnderlyingObjects(St->getPointerOperand(), Writes, DL);
    }

    // We still don't handle functions.
    CallInst *CI = dyn_cast<CallInst>(I);
    if (CI) {
      DEBUG(dbgs() << "LV: Found a call site:"<<
            CI->getCalledFunction()->getName() << "\n");
      return false;
    }

    // We do not re-vectorize vectors.
    if (!VectorType::isValidElementType(I->getType()) &&
        !I->getType()->isVoidTy()) {
      DEBUG(dbgs() << "LV: Found unvectorizable type." << "\n");
      return false;
    }
    //Check that all of the users of the loop are inside the BB.
    for (Value::use_iterator it = I->use_begin(), e = I->use_end();
         it != e; ++it) {
      Instruction *U = cast<Instruction>(*it);
      BasicBlock *Parent = U->getParent();
      if (Parent != &BB) {
        DEBUG(dbgs() << "LV: Found an outside user for : "<< *U << "\n");
        return false;
      }
    }
  } // next instr.

  if (NumPhis != 1) {
      DEBUG(dbgs() << "LV: Did not find a Phi node.\n");
      return false;
  }

  // Check that the underlying objects of the reads and writes are either
  // disjoint memory locations, or that they are no-alias arguments.
  ValueVector::iterator r, re, w, we;
  for (r = Reads.begin(), re = Reads.end(); r != re; ++r) {
    if (!isIdentifiedSafeObject(*r)) {
      DEBUG(dbgs() << "LV: Found a bad read Ptr: "<< **r << "\n");
      return false;
    }
  }

  for (w = Writes.begin(), we = Writes.end(); w != we; ++w) {
    if (!isIdentifiedSafeObject(*w)) {
      DEBUG(dbgs() << "LV: Found a bad write Ptr: "<< **w << "\n");
      return false;
    }
  }

  // Check that there are no multiple write locations to the same pointer.
  SmallPtrSet<Value*, 8> WritePointerSet;
  for (w = Writes.begin(), we = Writes.end(); w != we; ++w) {
    if (!WritePointerSet.insert(*w)) {
      DEBUG(dbgs() << "LV: Multiple writes to the same index :"<< **w << "\n");
      return false;
    }
  }

  // Check that the reads and the writes are disjoint.
  for (r = Reads.begin(), re = Reads.end(); r != re; ++r) {
    if (WritePointerSet.count(*r)) {
      DEBUG(dbgs() << "Vectorizer: Found a read/write ptr:"<< **r << "\n");
      return false;
    }
  }

  // All is okay.
  return true;
}

/// Checks if the value is a Global variable or if it is an Arguments
/// marked with the NoAlias attribute.
bool LoopVectorizationLegality::isIdentifiedSafeObject(Value* Val) {
  assert(Val && "Invalid value");
  if (dyn_cast<GlobalValue>(Val))
    return true;
  if (dyn_cast<AllocaInst>(Val))
    return true;
  Argument *A = dyn_cast<Argument>(Val);
  if (!A)
    return false;
  return A->hasNoAliasAttr();
}

} // namespace

char LoopVectorize::ID = 0;
static const char lv_name[] = "Loop Vectorization";
INITIALIZE_PASS_BEGIN(LoopVectorize, LV_NAME, lv_name, false, false)
INITIALIZE_AG_DEPENDENCY(AliasAnalysis)
INITIALIZE_PASS_DEPENDENCY(ScalarEvolution)
INITIALIZE_PASS_DEPENDENCY(LoopSimplify)
INITIALIZE_PASS_END(LoopVectorize, LV_NAME, lv_name, false, false)

namespace llvm {
  Pass *createLoopVectorizePass() {
    return new LoopVectorize();
  }

}

