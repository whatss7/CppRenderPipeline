#ifndef RAYTRACING_JIT_H
#define RAYTRACING_JIT_H
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/CFG.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

using namespace llvm;
using namespace llvm::orc;

extern ExitOnError ExitOnErr;

#define INIT_LLVM()         \
	InitLLVM INIT_LLVM_X(argc, argv); \
	INIT_LLVM_Impl(argc, argv);

void INIT_LLVM_Impl(int argc, char** argv);
void INIT_LLVM_Impl(int argc, const char** argv);

class JITHelper
{
public:
	JITHelper()
		: context(ThreadSafeContext(std::move(std::make_unique<LLVMContext>()))), LinkageModule(createLinkageModule()),
		JITInstance(createJITInstance()), JITStarted(false) {
		assert(inited && "INIT_LLVM() has not been called!");
	}

	/// <summary>
	/// Add a global value to the module.
	/// </summary>
	/// <param name="name">Global value name.</param>
	/// <param name="initialValue">Initial value.</param>
	void addGlobalValue(const std::string& name, Constant* initialValue);

	/// <summary>
	/// Add a function written in C++ to the module.
	/// </summary>
	/// <param name="name">Function name.</param>
	/// <param name="type">Function type.</param>
	/// <param name="func">Pointer to the function.</param>
	void addFunction(const std::string& name, FunctionType* type, llvm::JITTargetAddress func);

	// Read an IR file and add it to JIT.
	llvm::Error addIRFile(const std::string& path, const std::string& funcPrefix = "");

	// Add a vector to JIT. This is a shortcut as it does not need a initial value.
	void addVector(const std::string& name, llvm::Type* type, unsigned length) {
		addGlobalValue(name, ConstantAggregateZero::get(FixedVectorType::get(type, length)));
	}

	/// <summary>
	/// Add the module to JIT instance. All values and functions must be inserted
	/// before it.
	/// </summary>
	void startJIT();

	/// <summary>
	/// Get a function pointer from JIT'd module.
	/// </summary>
	/// <param name="name">Function name.</param>
	llvm::JITTargetAddress getFunction(const std::string& name);

	template<typename T>
	void setGlobalValue(const std::string &name, const T& value) {
		assert(JITStarted && "JIT has not started!");
		JITTargetAddress addr = gvAddrs[name];
		if (!addr) addr = gvAddrs[name] = ExitOnErr(JITInstance->lookup(name)).getAddress();
		*(T*)(addr) = value;
	}

	template<typename T>
	std::vector<T> getVector(const std::string &name) {
		assert(JITStarted && "JIT has not started!");
		using getFTy = T(*)(int);
		int len = getGlobalValue<int>(__vector_len_prefix + name);
		getFTy getF = (getFTy)getFunction(__vector_get_prefix + name);
		std::vector<T> v;
		for (int i = 0; i < len; i++) {
			v.push_back(getF(i));
		}
		return v;
	}

	template<typename T>
	void setVector(const std::string &name, llvm::ArrayRef<T> value) {
		assert(JITStarted && "JIT has not started!");
		using setFTy = void(*)(T, int);
		setFTy setF = (setFTy)getFunction(__vector_set_prefix + name);
		for (int i = 0; i < value.size(); i++) {
			setF(value[i], i);
		}
	}

	template <typename T>
	T getGlobalValue(const std::string& name)
	{
		assert(JITStarted && "JIT has not started!");
		JITTargetAddress addr = gvAddrs[name];
		if (!addr) addr = gvAddrs[name] = ExitOnErr(JITInstance->lookup(name)).getValue();
		return *(T*)(addr);
	}

	/// <summary>
	/// Add a function written in C++ to the module.
	/// </summary>
	/// <param name="name">Function name.</param>
	/// <param name="type">Function type.</param>
	/// <param name="func">Pointer to the function.</param>
	LLVMContext &getContext() { return *(context.getContext()); }

	StructType* getStructType(const std::string name) {
		return structTypes[name];
	}

private:
	std::unique_ptr<Module> createLinkageModule()
	{
		return std::make_unique<Module>("", *context.getContext());
	}
	std::unique_ptr<LLJIT> createJITInstance()
	{
		return ExitOnErr(LLJITBuilder().create());
	}
	ThreadSafeContext context;
	std::unique_ptr<Module> LinkageModule;
	std::unique_ptr<LLJIT> JITInstance;
	bool JITStarted;
	static bool inited;
	StringMap<JITTargetAddress> gvAddrs;
	StringMap<JITTargetAddress> funcAddrs;
	StringMap<StructType*> structTypes;
	friend void INIT_LLVM_Impl(int argc, const char** argv);
	const std::string __vector_get_prefix = "__rtjit_get_";
	const std::string __vector_set_prefix = "__rtjit_set_";
	const std::string __vector_len_prefix = "__rtjit_len_";
};

#endif // RAYTRACING_JIT_H
