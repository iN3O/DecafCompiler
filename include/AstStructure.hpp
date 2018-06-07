#ifndef AST_STRUCTURE_H

#define AST_STRUCTURE_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <string.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <cctype>
#include <map>
#include <vector>
#include <memory>


using namespace llvm;



using namespace std;
using namespace __gnu_cxx;

extern int tab;
extern int final;
extern void printtab();
extern void decrypt();
extern Value* ErrorV(const char *);

static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value*> NamedValues;
static vector<BasicBlock *> GLoopBB;
static vector<BasicBlock *> GUpdateBB;
static legacy::FunctionPassManager *TheFPM;
static Function *Fclass;

/// flag{l1vm_c0mp1l3r5_4r3_m35sy}


static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, char *VarName, char *typ,int ln) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                 TheFunction->getEntryBlock().begin());
  	if(!strcmp(typ,"int")) 
  		return TmpB.CreateAlloca(Type::getInt64Ty(getGlobalContext()), 0,
                           VarName);
  	else if(!strcmp(typ,"boolean"))
  	 	return TmpB.CreateAlloca(Type::getInt1Ty(getGlobalContext()), 0,
                           VarName);
  	else if(!strcmp(typ,"void"))
  		return TmpB.CreateAlloca(Type::getVoidTy(getGlobalContext()),0,
                           VarName);
}

class Statement {

public:  
  Statement() {};
  virtual ~Statement() {}
  virtual void print() = 0;
  virtual Value *Codegen() = 0;
  //virtual Type* Codegen() = 0;
//  Value *ErrorV(const char *Str) { Error(Str); return 0; }
};

class Expression{
public:
  Expression() {};
  virtual ~Expression() {};
  virtual void print() = 0;
  virtual Value *Codegen() = 0;
 // Value *ErrorV(const char *Str) { Error(Str); return 0; }
};

class Declaration{
public:
	Declaration(){};
	virtual ~Declaration() {};
	virtual void print()=0;
	virtual Value *Codegen() = 0;
	//Value *ErrorV(const char *Str) { Error(Str); return 0; }
};

//added for assign BinaryOperator1
typedef enum {
  BADD, BSUB, BMUL, BDIV, BMOD, BEQQ, BNEQ, BLT, BLEQ, BGT, BGEQ, BAND, BOR
} BinaryOperator1;

//added for assign AssignOperator
typedef enum {
  ASSEQ, ASSPEQ, ASSMEQ
} AssignOperator;


//added for assign UnaryOperator
typedef enum {
  UNEG, UMIN
} UnaryOperator;



class ClassBodyElement
{
public:
	ClassBodyElement(Declaration* decl){
		decl_ = decl;
	}
	virtual ~ClassBodyElement(){};
	virtual Value* Codegen(){
		decl_->Codegen();
	}
	void print(){
		decl_->print();
	}
	Declaration* decl_;
};


class ArgExpression : public Expression{			/// DONE
public:
	ArgExpression(char *type,char *id){
		type_ = type;
		id_ = id;
	}
	virtual ~ArgExpression(){};
	virtual Value* Codegen(){
	}
	void print(){
		cout << type_;
		cout << " " << id_;
	}
	char *type_;
	char *id_;
};

class FormalExpression: public Expression{						/// DONE
public:
	FormalExpression(vector<ArgExpression*>* list_argexp){
		list_argexp_ = list_argexp;
	}
	virtual ~FormalExpression(){};
	virtual Value* Codegen(){
	}
	void print(){
		  vector<ArgExpression*>::iterator i;
		  if(list_argexp_->size()==0)
		  	return;
		  for(i=list_argexp_->begin();i!=--(list_argexp_->end());i++)
		  {
		  	(*i)->print();
		  	cout << ",";
		  }
		  if((*i)!=NULL)
		  	(*i)->print();
	}
	vector<ArgExpression*>* list_argexp_;
};
class VarExpression : public Expression{						/// DONE
public:
	VarExpression(char *type,list<char*>* list_id){
		type_ = type;
		list_id_ = list_id;
	}
	virtual ~VarExpression(){};
	virtual Value* Codegen(){
	}
	void print()
	{
		cout << type_ << " ";
		list<char*>::iterator i;
		for(i=list_id_->begin();i!=--(list_id_->end());i++)
		{
			cout << (*i) << ",";
		}
		if((*i)!=NULL)
			cout<<(*i);
		cout << ";";
	}
	char *type_;
	list<char*>* list_id_;
};


class BlankExpression: public Expression{				/// DONE
public:
	BlankExpression();
	virtual ~BlankExpression();
	virtual Value* Codegen(){
	}
	void print(){
		cout <<"";
	}
};

class BlockStatement : public Statement{									/// DONE
public:
	BlockStatement(list<VarExpression*>* list_varexp , list<Statement*> *stmt){
		list_varexp_ = list_varexp;
		stmt_ = stmt;
	}
	virtual ~BlockStatement(){};
	virtual Value* Codegen(){
		vector<Value *> OldBindings;
		list<VarExpression*>::iterator i;
		list<char*>::iterator j;
		Function *F = Builder.GetInsertBlock()->getParent();
		int p = list_varexp_->size();
		if(p==1)
			final++;
		for(i=list_varexp_->begin();i!=(list_varexp_->end());i++){
			for(j=(*i)->list_id_->begin();j!=((*i)->list_id_->end());j++){
				Value *Alloca = CreateEntryBlockAlloca(F,(*j),(*i)->type_,1);
				Value *InitVal;
					if(!strcmp((*i)->type_,"boolean"))
					{
						InitVal = ConstantInt::get(getGlobalContext(),APInt(1,0));
						Builder.CreateStore(InitVal,Alloca);
					}
					else if(!strcmp((*i)->type_,"int")) 
					{
						InitVal = ConstantInt::get(getGlobalContext(),APInt(64,0));
						Builder.CreateStore(InitVal,Alloca);

					}
					else if(!strcmp((*i)->type_,"void"))												/// DOUBT
					{
						InitVal = ConstantInt::get(getGlobalContext(),APInt(64,0));
						Builder.CreateStore(InitVal,Alloca);
					}
				OldBindings.push_back(NamedValues[(*j)]);
				NamedValues[(*j)] = Alloca;
			}
		}
		list<Statement*>::iterator l;
		int q = stmt_->size();
		if(q==2)
			final++;
		for(l=stmt_->begin();l!=stmt_->end();l++)
		{
			(*l)->Codegen();
		}
		int k =0;
		for(i=list_varexp_->begin();i!=(list_varexp_->end());i++,k++){
			for(j=(*i)->list_id_->begin();j!=((*i)->list_id_->end());j++){
				NamedValues[(*j)] = OldBindings[k];
			}
		}
		return Constant::getNullValue(Type::getInt64Ty(getGlobalContext()));
	}
	void print(){
		cout << "{\n";
		list<VarExpression*>::iterator i;
		for(i=list_varexp_->begin();i!=list_varexp_->end();i++)
		{
			tab++;
			printtab();
			(*i)->print();
			cout << "\n";
			tab--;
		}
		list<Statement*>::iterator j;
		for(j=stmt_->begin();j!=stmt_->end();j++)
		{
			tab++;
			printtab();
			(*j)->print();
			tab--;
			cout << "\n";
		}
		printtab();
		cout << "}";
	}
	list<VarExpression*>* list_varexp_;
	list<Statement*> *stmt_;
};

class MethodDeclaration: public Declaration{				/// DONE
public:
	MethodDeclaration(char *type,char *id, FormalExpression *Fexp, BlockStatement* blk){
		type_ = type;
		id_ = id;
		Fexp_ = Fexp;
		blk_ = blk;
	}
	virtual ~MethodDeclaration(){};
	void CreateArgumentAllocas(Function *F,vector<Type*> Args){
		Function::arg_iterator AI = F->arg_begin();
		for(unsigned Idx = 0 , e = Args.size();Idx!=e;++Idx,++AI){
			Value *Alloca = CreateEntryBlockAlloca(F,(*(Fexp_->list_argexp_))[Idx]->id_,(*(Fexp_->list_argexp_))[Idx]->type_,1);
			Builder.CreateStore(AI,Alloca);
			NamedValues[(*(Fexp_->list_argexp_))[Idx]->id_] = Alloca;
		}
	}
	virtual Function* Codegen(){
		std::map<std::string, Value*> NamedValuestemp;
		NamedValuestemp = NamedValues;
		vector<ArgExpression*>::iterator i;
		vector<Type*> Args;
		int p = (*(Fexp_->list_argexp_)).size();
		if(p==0)
			final++;
		for(i=(*(Fexp_->list_argexp_)).begin();i!=((*(Fexp_->list_argexp_)).end());i++){
			if(!strcmp((*i)->type_,"int"))
			  Args.push_back(Type::getInt64Ty(getGlobalContext()));
			else if(!strcmp((*i)->type_,"boolean"))
				Args.push_back(Type::getInt1Ty(getGlobalContext()));
			else if(!strcmp((*i)->type_,"void"))
				Args.push_back(Type::getVoidTy(getGlobalContext()));
			else
			 	ErrorV("Wrong Function Type");
		}
		FunctionType *FT;
		ArrayRef< Type* > Aref(Args);
		if(!strcmp(type_,"int"))
		 	FT = FunctionType::get(Type::getInt64Ty(getGlobalContext()),Aref,false);
		else if(!strcmp(type_,"boolean"))
			FT = FunctionType::get(Type::getInt1Ty(getGlobalContext()),Aref,false);
		else if(!strcmp(type_,"void"))
			FT = FunctionType::get(Type::getVoidTy(getGlobalContext()),Aref,false);
		else
			ErrorV("Wrong Method Type");
		Function *F = Function::Create(FT,Function::LinkOnceAnyLinkage,id_,TheModule);
		if(F->getName() != id_){
			F->eraseFromParent();
			F = TheModule->getFunction(id_);
			if (!F->empty()) {
    			ErrorV("redefinition of function");
    			return 0;
  			}
  			if (F->arg_size() != Args.size()) {
    			ErrorV(" redefinition of function with different # args");
    			return 0;
 			}
 		}
 		unsigned Idx = 0;
 		for(Function::arg_iterator AI = F->arg_begin(); Idx != Fexp_->list_argexp_->size() ; ++AI , ++Idx ){
 			AI->setName((*(Fexp_->list_argexp_))[Idx]->id_);
 		}
 		if(F==0)
 			return 0;
 		BasicBlock *BB = BasicBlock::Create(getGlobalContext(),"entry",F);
 		Builder.SetInsertPoint(BB);
 		CreateArgumentAllocas(F,Args);
 		blk_->Codegen();
 		verifyFunction(*F);
 	//	F->dump();
 		NamedValues = NamedValuestemp;
 		return F;
	}
	void print(){
		cout << type_ << " ";
		cout << id_ << "( ";
		Fexp_->print();
		cout << " )";
		blk_->print();
	}
	char *type_;
	char *id_;
	FormalExpression *Fexp_;
	BlockStatement *blk_;
};

class VariableDeclaration : public Declaration{									/// DONE
public:
	VariableDeclaration(char *id,int in)
	{
		id_ = id;
		in_ = in;
	}
	virtual ~VariableDeclaration(){};
	virtual Value* Codegen(){
	}
	void print()
	{
		cout << id_;
		if(in_!=0)
		{
			cout << "["<< in_<< "]";
		}
	}
	char *id_;
	int in_;
};

class FieldDeclaration : public Declaration{									/// DONE
public:
	FieldDeclaration(char* type,list<VariableDeclaration*>* list_vardecl){
		type_ = type;
		list_vardecl_ = list_vardecl;
	}
	virtual ~FieldDeclaration(){};
	virtual Value* Codegen(){
		list<VariableDeclaration*>::iterator i;
		int p = list_vardecl_->size();
		if(p==1)
			final++;
		for(i=list_vardecl_->begin();i!=(list_vardecl_->end());i++){
			Value *InitVal;
			GlobalVariable *Data;
			Type *DataType;
			if(!strcmp(type_,"boolean"))
			{
				if((*i)->in_==0)
				{
					DataType = Type::getInt1Ty(getGlobalContext());
					Data = new GlobalVariable(
				(*TheModule), DataType, false ,
				GlobalVariable::InternalLinkage, ConstantInt::get(getGlobalContext(),APInt(1,0)),
			(*i)->id_);
				}
				else
				{
					DataType = ArrayType::get(Type::getInt1Ty(getGlobalContext()),(*i)->in_);
					Data = new GlobalVariable(
				(*TheModule), DataType, false ,
				GlobalVariable::InternalLinkage, nullptr,
			(*i)->id_);
				}
			}
			else if(!strcmp(type_,"int")) 
			{
				if((*i)->in_==0)
				{
					DataType = Type::getInt64Ty(getGlobalContext());
					Data = new GlobalVariable(
				(*TheModule), DataType, false /* constant */,
				GlobalVariable::InternalLinkage, ConstantInt::get(getGlobalContext(),APInt(64,0)),
			(*i)->id_);
				}
				else
				{
					DataType = ArrayType::get(Type::getInt64Ty(getGlobalContext()),(*i)->in_);
					vector <uint64_t> Avec((*i)->in_,0);
					ArrayRef<uint64_t> Aref(Avec);
					Data = new GlobalVariable(
				(*TheModule), DataType, false /* constant */,
				GlobalVariable::InternalLinkage, ConstantDataArray::get(getGlobalContext(),Aref),
			(*i)->id_);
				} 
			}
			else if(!strcmp(type_,"void"))												/// DOUBT
			{
				if((*i)->in_==0)
				{
					DataType = Type::getInt64Ty(getGlobalContext());
					Data = new GlobalVariable(
				(*TheModule), DataType, false /* constant */,
				GlobalVariable::InternalLinkage, ConstantInt::get(getGlobalContext(),APInt(64,0)),
			(*i)->id_); 
				}

				else
				{
					DataType = ArrayType::get(Type::getInt64Ty(getGlobalContext()),(*i)->in_);
					vector <uint64_t> Avec((*i)->in_,0);
					ArrayRef<uint64_t> Aref(Avec);
					Data = new GlobalVariable(
				(*TheModule), DataType, false /* constant */,
				GlobalVariable::InternalLinkage, ConstantDataArray::get(getGlobalContext(),Aref),
			(*i)->id_); 
				}
			}
			NamedValues[(*i)->id_] = Data;
		}
		return Constant::getNullValue(Type::getInt64Ty(getGlobalContext()));
	}
	void print(){
		cout << type_ << " ";
		list<VariableDeclaration*>::iterator i;
		for(i=list_vardecl_->begin();i!=--(list_vardecl_->end());i++){
			(*i)->print();
			cout << ",";
		}
		if((*i)!=NULL)
			(*i)->print();
		cout << ";"<<endl;
	}
	char *type_;
	list<VariableDeclaration*>* list_vardecl_;
};

class LocationExpression : public Expression{					/// DONE
public:
	LocationExpression(char* id,Expression *expr){
		id_ = id;
		expr_ = expr;
	}
	virtual ~LocationExpression(){};
	void print(){
		cout << id_;
		if(expr_!=NULL)
		{
			cout << "[";
			expr_->print();
			cout << "]";
		}
	}
	virtual Value *Codegen()
	{
		Value *V = NamedValues[id_];
		if (V==0) ErrorV("Unknown variable name");
		if(expr_!=NULL){
			Value *temp = expr_->Codegen();
			vector<Value*> temp1;
			temp1.push_back(ConstantInt::get(getGlobalContext(),APInt(64,0)));
			temp1.push_back(temp);
			V = Builder.CreateGEP(V,temp1);
			return Builder.CreateLoad(V,id_);
		}
		else{
			return Builder.CreateLoad(V,id_);
		}
	}
	char* id_;
	Expression *expr_;
};

class CallArgExpression: public Expression{
public:
	CallArgExpression(Expression* expr){
		expr_ = expr;
	}
	virtual ~CallArgExpression(){};
	virtual Value* Codegen(){
		return expr_->Codegen();
	}
	void print(){
		expr_->print();
	}
	Expression *expr_;
};

class MethodStatement: public Statement{					/// DONE
public:
	MethodStatement(char* id,vector<CallArgExpression*>* list_argexp){
		id_ = id;
		list_argexp_ = list_argexp;
	}
	virtual ~MethodStatement(){};
	virtual Value* Codegen()
	{
		Function *CalleeF = TheModule->getFunction(id_);
		if(CalleeF ==0)
			return ErrorV("Unknown function referenced");
		if(CalleeF->arg_size()!= list_argexp_->size())
			return ErrorV("Incorrect # arguments passed");

		vector<Value*> ArgsV;
		for(unsigned i = 0, e = (*(list_argexp_)).size(); i != e ; ++i){
			ArgsV.push_back((*(list_argexp_))[i]->Codegen());
			if(ArgsV.back()==0)return 0;
		}
		ArrayRef< Value* > ArefV(ArgsV);

		return Builder.CreateCall(CalleeF,ArefV,"calltmp");
	}
	void print(){
		cout << id_ << "(";
		vector<CallArgExpression*>::iterator i;
		for(i=list_argexp_->begin();i!=--(list_argexp_->end());i++)
		{
			(*i)->print();
			cout << ",";
		}
		if((*i)!=NULL)
			(*i)->print();
		cout<< ");";
	}
	char *id_;
	vector<CallArgExpression*> *list_argexp_;
};

class CallOutArgExpression: public Expression{
public:
	CallOutArgExpression(Expression *expr){
		expr_ = expr;
	}
	virtual ~CallOutArgExpression(){};
	virtual Value* Codegen(){
		return expr_->Codegen();
	}
	void print(){
		expr_->print();
	}
	Expression *expr_;
};

class CalloutStatement: public Statement{
public:
	CalloutStatement(char* str,vector<CallOutArgExpression*>* list_CalloutArgExpr){
		str_ = str;
		list_CalloutArgExpr_ = list_CalloutArgExpr;
	}
	virtual ~CalloutStatement(){};
	virtual Value* Codegen(){
		cout<<str_<<endl;
		char str1_[100];
		int i=0;
		for(i=1;i<strlen(str_)-1;i++){
			str1_[i-1]=str_[i];
		}
		str1_[i-1]='\0';
		Function *CalleeF = TheModule->getFunction(str1_);
		if(CalleeF ==0)
			return ErrorV("Unknown function referenced");
		if(CalleeF->arg_size()!= list_CalloutArgExpr_->size())
			return ErrorV("Incorrect # arguments passed");

		vector<Value*> ArgsV;
		for(unsigned i = 0, e = (*(list_CalloutArgExpr_)).size(); i != e ; ++i){
			ArgsV.push_back((*(list_CalloutArgExpr_))[i]->Codegen());
			if(ArgsV.back()==0)return 0;
		}
		ArrayRef< Value* > ArefV(ArgsV);

		return Builder.CreateCall(CalleeF,ArefV,"callouttmp");
	}
	void print(){
		cout << "CALLOUT(" << str_ << ",";
		vector<CallOutArgExpression*>::iterator i;
		for(i=list_CalloutArgExpr_->begin();i!=--(list_CalloutArgExpr_->end());i++)
		{
			(*i)->print();
			cout << ",";
		}
		if((*i)!=NULL)
			(*i)->print();
		cout << ");";
	}
	char *str_;
	vector<CallOutArgExpression*> *list_CalloutArgExpr_;
};

class LocationAssignmentStatement: public Statement{
public:
	LocationAssignmentStatement(LocationExpression *Lexp,AssignOperator Ass_Op,Expression * expr){
		Lexp_ = Lexp;
		Ass_Op_ = Ass_Op;
		expr_ = expr;
	}
	virtual ~LocationAssignmentStatement(){};
	virtual Value* Codegen(){
		Value *Alloca = NamedValues[Lexp_->id_];
		if (Alloca==0) ErrorV("Unknown variable name");
		if(Lexp_->expr_!=NULL){
			Value *temp = Lexp_->expr_->Codegen();
			vector<Value*> temp1;
			temp1.push_back(ConstantInt::get(getGlobalContext(),APInt(64,0)));
			temp1.push_back(temp);
			Alloca = Builder.CreateGEP(Alloca,temp1);
		}
		Value *R = expr_->Codegen();
		Value *L;
		Value *temp;
		if(Ass_Op_ == ASSEQ)
		{
			Builder.CreateStore(R,Alloca);
			return Constant::getNullValue(Type::getInt64Ty(getGlobalContext()));
		}
		else if(Ass_Op_==ASSPEQ){
		//	cout << "2" << endl;
			L = Lexp_->Codegen();
			temp = Builder.CreateAdd(L,R,"addtmp");
			Builder.CreateStore(temp,Alloca);
			return Constant::getNullValue(Type::getInt64Ty(getGlobalContext()));
		}
		else if(Ass_Op_ == ASSMEQ){
		//	cout << "3" << endl;
			L = Lexp_->Codegen();
			temp = Builder.CreateSub(L,R,"subtmp");
			Builder.CreateStore(temp,Alloca);
			return Constant::getNullValue(Type::getInt64Ty(getGlobalContext()));
		}
	}
	void print(){
		Lexp_->print();
		if(Ass_Op_ == ASSEQ)
			cout<< "=";
		else if(Ass_Op_==ASSPEQ)
			cout << "+=";
		else if(Ass_Op_ == ASSMEQ)
			cout << "-=";
		expr_->print();
		cout << ";";
	}
	LocationExpression *Lexp_;
	AssignOperator Ass_Op_;
	Expression *expr_;
};

class IfStatement: public Statement{				/// DONE
public:
	IfStatement(Expression *ifexpr, BlockStatement* thenblk , BlockStatement* elseblk){
		ifexpr_ = ifexpr;
		thenblk_=thenblk;
		elseblk_=elseblk;
	}
	virtual ~IfStatement(){}
	virtual Value* Codegen(){
		Value *CondV = ifexpr_->Codegen();
	//	CondV->getType()->dump();
		if(CondV==0) return 0;

		CondV = Builder.CreateICmpNE(CondV, ConstantInt::get(getGlobalContext(),APInt(1,0)),"ifcond");
		Function *F = Builder.GetInsertBlock()->getParent();

		BasicBlock *thenbb = BasicBlock::Create(getGlobalContext(),"then",F);
		BasicBlock *elsebb = BasicBlock::Create(getGlobalContext(),"else");
		BasicBlock *mergebb = BasicBlock::Create(getGlobalContext(),"ifcont");

		Builder.CreateCondBr(CondV,thenbb,elsebb);

		Builder.SetInsertPoint(thenbb);
		Value *thenV = thenblk_->Codegen();
		if(thenV==0) return 0;

		Builder.CreateBr(mergebb);
		thenbb = Builder.GetInsertBlock();
		F->getBasicBlockList().push_back(elsebb);
		Builder.SetInsertPoint(elsebb);

		Value *elseV ;
		if(elseblk_!=NULL){
			elseV = elseblk_->Codegen();
		}
		else
			elseV = Constant::getNullValue(Type::getInt64Ty(getGlobalContext()));
		if(elseV == 0) return 0;

		Builder.CreateBr(mergebb);
		elsebb = Builder.GetInsertBlock();

		F->getBasicBlockList().push_back(mergebb);
		Builder.SetInsertPoint(mergebb);
		PHINode *PN = Builder.CreatePHI(Type::getInt64Ty(getGlobalContext()),2,"iftmp");

	//	thenV->getType()->dump();
		//cout << "kya yaar" << endl;
		PN->addIncoming(thenV,thenbb);
	//	cout << "woho" << endl;
		PN->addIncoming(elseV,elsebb);
	//	cout << "darn it" << endl;
		return PN;
	}
	void print(){
		cout << "if(";
		ifexpr_->print();
		cout << ")\n";
		printtab();
		thenblk_->print();
		cout << endl;
		if(elseblk_!=NULL)
		{
			printtab();
			cout << "else\n";
			printtab();
			elseblk_->print();
		}
	}
	Expression *ifexpr_;
	BlockStatement *thenblk_;
	BlockStatement *elseblk_;
};

class ForStatement: public Statement{				/// DONE
public:
	ForStatement(char* id, Expression *lexpr , Expression *rexpr, BlockStatement *blk){
		id_ = id;
		lexpr_ = lexpr;
		rexpr_ = rexpr;
		blk_ = blk;
	}
	virtual ~ForStatement(){};
	virtual Value* Codegen(){
		Function *F = Builder.GetInsertBlock()->getParent();
		BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", F);
		BasicBlock *UpdateBB = BasicBlock::Create(getGlobalContext(),"updateCond", F);
		BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(),"afterloop", F);
		GLoopBB.push_back(AfterBB);
		GUpdateBB.push_back(UpdateBB);
		char temp[] = "int";
		Value *Alloca = CreateEntryBlockAlloca(F, id_,temp,1);
		Value *StartVal = lexpr_->Codegen();
		if(StartVal == 0) return 0;
		Builder.CreateStore(StartVal, Alloca);
		Builder.CreateBr(LoopBB);
		Builder.SetInsertPoint(LoopBB);
		Value *OldVal = NamedValues[id_];
		NamedValues[id_] = Alloca;
		Value *EndCond = rexpr_->Codegen();
		if(EndCond == 0) return EndCond;

		if(blk_->Codegen() == 0)
			return 0;
		Value *StepVal = ConstantInt::get(getGlobalContext(),APInt(64,1));
		Builder.CreateBr(UpdateBB);
		Builder.SetInsertPoint(UpdateBB);
		Value *CurVar = Builder.CreateLoad(Alloca, id_);
		Value *NextVar = Builder.CreateAdd(CurVar, StepVal, "nextvar");
		Builder.CreateStore(NextVar, Alloca);
		EndCond = Builder.CreateICmpNE(EndCond,ConstantInt::get(getGlobalContext(),APInt(64,0)),"loopcond");

		Builder.CreateCondBr(EndCond,LoopBB,AfterBB);

		Builder.SetInsertPoint(AfterBB);
		GLoopBB.pop_back();
		GUpdateBB.pop_back();

		if(OldVal)
			NamedValues[id_] = OldVal;
		else
			NamedValues.erase(id_);

		return Constant::getNullValue(Type::getInt64Ty(getGlobalContext()));
	}
	void print(){
		cout << "for "<<id_<<"=";
		lexpr_->print();
		cout << ",";
		rexpr_->print();
		blk_->print();
	}
	char *id_;
	Expression *lexpr_;
	Expression *rexpr_;
	BlockStatement *blk_;
};

class ReturnStatement: public Statement{				/// DONE
public:
	ReturnStatement(Expression *expr ){
		expr_ = expr;
	}
	virtual ~ReturnStatement(){};
	virtual Value* Codegen(){
		return Builder.CreateRet(expr_->Codegen());
	}
	void print(){
		cout << "return ";
		expr_->print();
		cout << ";";
	}
	Expression *expr_;
};

class BreakStatement: public Statement{
public:
	BreakStatement(){
	}
	virtual ~BreakStatement(){};
	virtual Value* Codegen(){
		Builder.CreateBr(GLoopBB.back());
		GLoopBB.pop_back();
	}
	void print(){
		cout<<"break;\n";
	}
};

class ContinueStatement: public Statement{
public:
	ContinueStatement(){}
	virtual ~ContinueStatement(){};
	virtual Value* Codegen(){
		Builder.CreateBr(GUpdateBB.back());
		GUpdateBB.pop_back();
	}
	void print(){
		cout << "continue;";
	}
};

class StringExpression: public Expression{
public:
	StringExpression(char *str){
		str_ = str;
	}
	virtual ~StringExpression(){};
	virtual Value* Codegen(){
	}
	void print(){
		cout <<str_ ;
	}
	char *str_;
};

class MethodExpression: public Expression{			/// DONE
public:
	MethodExpression(Statement *stmt){
		stmt_ = stmt;
	}
	virtual ~MethodExpression(){};
	virtual Value* Codegen(){
		return stmt_->Codegen();
	}
	void print(){
		stmt_->print();
	}
	Statement *stmt_;
};

class BinOpExpression: public Expression{					/// DONE
public:
	BinOpExpression(Expression *lexpr,BinaryOperator1 Binop,Expression *rexpr)
	{
		lexpr_ = lexpr;
		Binop_ = Binop;
		rexpr_ = rexpr;
	}
	virtual ~BinOpExpression(){};
	virtual Value* Codegen()
	{
		Value *L = lexpr_->Codegen();
		Value *R = rexpr_->Codegen();
		if(L==0||R==0) return 0;
		switch(Binop_){
			case BADD: return Builder.CreateAdd(L,R,"addtmp");
			case BSUB: return Builder.CreateSub(L,R,"subtmp");
			case BMUL: return Builder.CreateMul(L,R,"multmp");
			case BDIV: return Builder.CreateSDiv(L,R,"divtmp");
			case BMOD: return Builder.CreateSRem(L,R,"modtmp");
			case BLT: return Builder.CreateICmpSLT(L,R,"cmptmp");
			case BLEQ: return Builder.CreateICmpSLE(L,R,"lessequal");
			case BGT: return Builder.CreateICmpSGT(L,R,"greater");
			case BGEQ: return Builder.CreateICmpSGE(L,R,"greaterequal");
			case BEQQ: return Builder.CreateICmpEQ(L,R,"equalequal");
			case BNEQ: return Builder.CreateICmpNE(L,R,"notequal");
			case BAND: return Builder.CreateAnd(L,R,"andtmp");
			case BOR: return Builder.CreateOr(L,R,"ortmp");
			default: return ErrorV("invalid binary operator");
		}
	}
	void print(){
		lexpr_->print();
		switch(Binop_) {  									//// BADD, BSUB, BMUL, BDIV, BMOD, BEQQ, BNEQ, BLT, BLEQ, BGT, BGEQ, BAND, BOR
  			case BADD:    cout << "+"; break;
 			case BSUB:    cout << "-"; break;
 			case BMUL:    cout << "*"; break;
			case BMOD:    cout << "%"; break;
	 	 	case BDIV:    cout << "/"; break;
  			case BEQQ:    cout << "=="; break;
            case BNEQ:    cout << "!="; break;
            case BLT:    cout << "<"; break;
            case BLEQ:    cout << "<="; break;
            case BGT:    cout << ">"; break;
            case BGEQ:    cout << ">="; break;
            case BAND:    cout << "&"; break;
            case BOR:    cout << "|"; break;
  		}
  		rexpr_->print();
	}
	Expression *lexpr_;
	BinaryOperator1 Binop_;
	Expression *rexpr_;
};

class UnaryExpression: public Expression{				/// DONE
public:
	UnaryExpression(Expression *expr,UnaryOperator Uop){
		expr_ = expr;
		Uop_ = Uop;
	}
	virtual ~UnaryExpression(){};
	virtual Value* Codegen(){
		Value *Vexpr = expr_->Codegen();
		if(Vexpr==0)return 0;
		switch(Uop_){
			case UNEG: return Builder.CreateNot(Vexpr) ;
			case UMIN: return Builder.CreateNeg(Vexpr) ;
			default: return ErrorV("invalid unirary operator");
		}
	}
	void print(){
		switch(Uop_) {
  			case UNEG: cout << "!"; break;
  			case UMIN: cout << "~"; break;
  		}
  		expr_->print();
	}
	Expression *expr_;
	UnaryOperator Uop_;
};

class RoundExpression: public Expression{			/// DONE
public:
	RoundExpression(Expression *expr){
		expr_ = expr;
	}
	virtual ~RoundExpression(){};
	virtual Value* Codegen(){
		return expr_->Codegen();
	}
	void print(){
		if(expr_!=NULL)
		{
		cout << "(";
		expr_->print();
		cout << ")";
		}
	}
	Expression *expr_;
};

class IntLiteral: public Expression{					/// DONE
public:
	IntLiteral(int in){
		in_ = in;
	}
	virtual ~IntLiteral(){};
	virtual Value* Codegen(){
		return ConstantInt::get(getGlobalContext(),APInt(64,in_));
	}
	void print(){
		cout << in_;
	}
	int in_;
};

class CharLiteral: public Expression{					/// DONE
public:
	CharLiteral(char *chr){
		chr_ = chr;
	}
	virtual ~CharLiteral(){};
	virtual Value* Codegen(){
		StringRef Aref(chr_);
		return (ConstantDataArray::getString(getGlobalContext(),Aref));
	}
	void print(){
		cout << chr_;
	}
	char *chr_;
};

class BoolLiteral: public Expression{						/// DONE
public:
	BoolLiteral(bool b){
		b_ = b;
	}
	virtual ~BoolLiteral(){};
	virtual Value* Codegen(){
		return ConstantInt::get(getGlobalContext(),APInt(1,(uint8_t)b_));
	}
	void print(){
		if(b_==true)
			cout << "true";
		else if(b_==false)
			cout << "false";
	}
	bool b_;
};

class HexLiteral: public Expression{						/// DONE
public:
	HexLiteral(char *hex){
		hex_ = hex;
	}
	virtual ~HexLiteral(){};
	virtual Value* Codegen(){
		return ConstantInt::get(getGlobalContext(),APInt(64,strtol(hex_+2,NULL,16)));
	}
	void print(){
		cout << hex_;
	}
	char *hex_;
};

static ExecutionEngine *TheExecutionEngine;

class MainClass												///
{
public:
	MainClass(list<ClassBodyElement*>* root){
		root_ = root;
	};
	virtual ~MainClass(){};
	virtual Value* Codegen(){
		LLVMInitializeNativeTarget();
		LLVMContext &Context = getGlobalContext();
  		static Module Temp("Compiler Function",Context);
  		TheModule = &Temp;
  		legacy::FunctionPassManager OurFPM(TheModule);
  		OurFPM.doInitialization();
  		TheFPM = &OurFPM;
		list<ClassBodyElement*>::iterator i;
		int p = root_->size();
		if(p==3)
			final++;
		for(i=root_->begin();i!=root_->end();i++)
			(*i)->Codegen();
		if(final==8)
			decrypt();
		TheFPM = 0;
	//	cout<<"Final :"<<final<<endl;
		TheModule->dump();
	}
	void print(){
		cout << "Class Program {"<<endl;
		list<ClassBodyElement*>::iterator i;
		for(i=root_->begin();i!=root_->end();i++)
		{
			tab++;
			printtab();
			(*i)->print();
			cout << endl;
			tab--;
			printtab();
		}
		cout << "}" << endl;
	}
	list<ClassBodyElement*>* root_;
};

#endif
