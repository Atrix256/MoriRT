/*======================================================

Callback.h

Helper macros and such for using callbacks

======================================================*/

#ifndef CALLBACK_H
#define CALLBACK_H

#define DEFINE_MEMBERCALLBACK(ClassName,FuncName) \
	static void StaticCB_##FuncName(void *pUserArgs) \
	{ \
		static_cast<ClassName *>(pUserArgs)->FuncName(); \
	} \
	void FuncName();

#define DEFINE_MEMBERCALLBACK_ONEARG(ClassName,FuncName,ArgType1) \
	static void StaticCB_##FuncName(void *pUserArgs, ArgType1 arg1) \
	{ \
		static_cast<ClassName *>(pUserArgs)->FuncName(arg1); \
	} \
	void FuncName(ArgType1 arg1);

#define DEFINE_SINGLETONCALLBACK_THREEARGS(ClassName,FuncName,ArgType1,ArgType2,ArgType3) \
	static void StaticCB_##FuncName(ArgType1 arg1, ArgType2 arg2, ArgType3 arg3) \
	{ \
		##ClassName::GetSingleton().FuncName(arg1,arg2,arg3); \
	} \
	void FuncName(ArgType1 arg1,ArgType2 arg2,ArgType3 arg3);



#define USE_SINGLETONCALLBACK(FuncName) StaticCB_##FuncName
#define USE_MEMBERCALLBACK(FuncName) StaticCB_##FuncName,this

#endif //CALLBACK_H