#include "c2.h"

using namespace c2;

template<> const char* getstr<message_s>(message_s value)
{
	switch(value)
	{
	case ErrorExpected1p: return "expected '%1'";
	case ErrorCastType1pTo2p: return "invalid type cast '%1' to '%2'";
	case ErrorOperation1pNotUsedWithOperands2pAnd3p: return "operator '%1' not used with '%2' and '%3' operands";
	case ErrorUnexpectedSymbols: return "unexpected symbols in end of file";
	case ErrorCantFind1pWithName2p: return "can't find %1 '%2'";
	case ErrorNeedLValue: return "need l-value";
	case ErrorNeedSymbol: return "need symbol '%1'";
	case ErrorNeedConstantExpression: return "need constant expression";
	case ErrorNeedIndentifier: return "need identifier";
	case ErrorNeedFunctionMember: return "need function member";
	case ErrorNeedPointerOrArray: return "need pointer or array";
	case ErrorLongNotUseWithThisType: return "keyword 'long' don't use with type '%1'";
	case ErrorShortNotUseWithThisType: return "keyword 'short' don't use with type '%1'";
	case ErrorUnsignedNotUseWithThisType: return "keyword 'unsigned' don't use with type '%1'";
	case Error1pDontUse2pTimes: return "keyword '%1' used %2 times";
	case ErrorArrayOverflow: return "array overflow";
	case ErrorImportAlreadyHavePseudonim: return "import already have pseudonim";
	case ErrorModuleAlreadyImported: return "module already imported";
	case ErrorUnknownInstance: return "unknown instance type";
	case ErrorKeyword1pUsedWithout2p: return "keyword '%1' used without %2";
	case ErrorVoidReturnValue: return "void procedure return value";
	case ErrorInvalid1p2pIn3p: return "invalid %1 '%2' in %3";
	case ErrorWrongParamNumber: return "wrong parameters number in call to %1. You want %3i but function has %2i.";
	case ErrorNotImplement1p2p: return "not implement %1 '%2'";
	case ErrorNotFound1p2p: return "not found %1 '%2'";
	case Error1p2pAlreadyDefined: return "%1 '%2' already defined";
	default: return "unknown error";
	}
}