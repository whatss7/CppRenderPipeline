#include "jit.h"

ExitOnError ExitOnErr;
bool JITHelper::inited = false;

void JITHelper::addGlobalValue(const std::string& name, Constant* initialValue)
{
	assert(!JITStarted && "JIT has started!");
	assert(initialValue && "Initial value must be provided!");
	auto type = initialValue->getType();
	LinkageModule->getOrInsertGlobal(name, type);
	auto var = LinkageModule->getNamedGlobal(name);
	var->setInitializer(initialValue);
	if (VectorType *vecTy = dyn_cast<VectorType>(type)) {
		auto voidTy = Type::getVoidTy(getContext());
		auto int32Ty = Type::getInt32Ty(getContext());

		// Function for getting i-th element
		{
			Function* getF = Function::Create(
				FunctionType::get(vecTy->getElementType(), int32Ty, false),
				Function::ExternalLinkage, __vector_get_prefix + name, LinkageModule.get());
			BasicBlock* getBB = BasicBlock::Create(getContext(), "", getF);
			IRBuilder<> getIRB(getBB, getBB->getFirstInsertionPt());
			getIRB.CreateRet(
				getIRB.CreateExtractElement(
					getIRB.CreateLoad(vecTy, var), getF->getArg(0)));
		}

		// Function for setting i-th element
		{
			Function* setF = Function::Create(
				FunctionType::get(voidTy, { vecTy->getElementType(), int32Ty }, false),
				Function::ExternalLinkage, __vector_set_prefix + name, LinkageModule.get());
			BasicBlock* setBB = BasicBlock::Create(getContext(), "", setF);
			IRBuilder<> setIRB(setBB, setBB->getFirstInsertionPt());
			setIRB.CreateStore(
				setIRB.CreateInsertElement(
					setIRB.CreateLoad(vecTy, var), setF->getArg(0), setF->getArg(1)),
				var);
			setIRB.CreateRetVoid();
		}

		addGlobalValue(__vector_len_prefix + name, ConstantInt::get(int32Ty, vecTy->getElementCount().getFixedValue()));
	}
}

void JITHelper::addFunction(const std::string& name, FunctionType* type, llvm::JITTargetAddress func)
{
	assert(!JITStarted && "JIT has started!");
	assert(type && "Function type must be provided!");
	assert(func && "Function pointer must be provided!");
	LLVMContext& Context = LinkageModule->getContext();
	auto int64Ty = Type::getInt64Ty(Context);
	Function* F = Function::Create(type, Function::ExternalLinkage, name,
		LinkageModule.get());
	BasicBlock* BB = BasicBlock::Create(Context, "", F);
	IRBuilder<> IRB(BB, BB->getFirstInsertionPt());
	SmallVector<Value*> args;
	for (auto& A : F->args())
		args.push_back(&A);
	auto call = IRB.CreateCall(
		type,
		IRB.CreateIntToPtr(ConstantInt::get(int64Ty, (uint64_t)func),
			PointerType::getUnqual(type)),
		args);
	if(type->getReturnType() == Type::getVoidTy(getContext())) IRB.CreateRetVoid();
	else IRB.CreateRet(call);
}

Error JITHelper::addIRFile(const std::string& path, const std::string& funcPrefix) {
	SMDiagnostic Err;
	auto M = parseIRFile(path, Err, getContext());
	if (!M) return createStringError(std::errc::invalid_argument, "Failed to read file %s", path.c_str());
	Type* voidTy = Type::getVoidTy(getContext());
	// Add prefix for function and global value definitions in module
	// This works like the namespace
	for (Function& F : M->functions()) {
		if (!F.isDeclaration()) F.setName(funcPrefix + F.getName());
	}
	for (GlobalValue& GV : M->globals()) {
		if (!GV.isDeclaration()) GV.setName(funcPrefix + GV.getName());
	}
	for (auto i : M->getIdentifiedStructTypes()) {
		structTypes[i->getName()] = i;
	}
	auto result = JITInstance->addIRModule(ThreadSafeModule(std::move(M), context));
	return result;
}

void JITHelper::startJIT()
{
	if (JITStarted)
		return;
	JITStarted = true;
	ExitOnErr(JITInstance->addIRModule(
		ThreadSafeModule(std::move(LinkageModule), context)));
}

llvm::JITTargetAddress JITHelper::getFunction(const std::string& name)
{
	assert(JITStarted && "JIT has not started!");
	JITTargetAddress addr = funcAddrs[name];
	if (addr) return addr;
	auto symbol = ExitOnErr(JITInstance->lookup(name));
	return funcAddrs[name] = symbol.getValue();
}

void INIT_LLVM_Impl(int argc, const char** argv)
{
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	cl::ParseCommandLineOptions(argc, argv, "RayTracingIRLoader");
	ExitOnErr.setBanner(std::string(argv[0]) + ": ");
	JITHelper::inited = true;
}

void INIT_LLVM_Impl(int argc, char** argv) {
	INIT_LLVM_Impl(argc, const_cast<const char**&>(argv));
}