/*=============================================================================
	Copyright (c) 2015 Paul Fultz II
	Originally cloak.h (found at https://github.com/pfultz2/Cloak)
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#pragma once

#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)

#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t
#define IIF(c) PRIMITIVE_CAT(IIF_, c)

#define COMPL_0 1
#define COMPL_1 0
#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)

#define BITAND_0(y) 0
#define BITAND_1(y) y
#define BITAND(x) PRIMITIVE_CAT(BITAND_, x)

#define INC_0 1
#define INC_1 2
#define INC_2 3
#define INC_3 4
#define INC_4 5
#define INC_5 6
#define INC_6 7
#define INC_7 8
#define INC_8 9
#define INC_9 9
#define INC(x) PRIMITIVE_CAT(INC_, x)

#define DEC_0 0
#define DEC_1 0
#define DEC_2 1
#define DEC_3 2
#define DEC_4 3
#define DEC_5 4
#define DEC_6 5
#define DEC_7 6
#define DEC_8 7
#define DEC_9 8
#define DEC(x) PRIMITIVE_CAT(DEC_, x)

#ifdef _WIN32
#define MSVC_VA_ARGS_WORKAROUND(define, args) define args
#define CHECK_N(x, n, ...) n
#define CHECK(...) MSVC_VA_ARGS_WORKAROUND(CHECK_N, (__VA_ARGS__, 0))
#else
#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
#endif
#define PROBE(x) x, 1,
#define IS_PAREN_PROBE(...) PROBE(~)
#define IS_PAREN(x) CHECK(IS_PAREN_PROBE x)

#define NOT_0 PROBE(~)
#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))

#define BOOL(x) COMPL(NOT(x))
#define IF(c) IIF(BOOL(c))

#define COUNT_IMPL_(_1, _2, _3, _4, _5, N, ...) N
#ifdef _WIN32
#define ARG_EXPAND(x) x
#define COUNT(...) ARG_EXPAND(COUNT_IMPL_(__VA_ARGS__, 5, 4, 3, 2, 1))
#else
#endif

#define DISPATCH_IMPL2(Func, ArgCount) \
	Func ## ArgCount

#define DISPATCH_IMPL(Func, ArgCount) \
	DISPATCH_IMPL2(Func, ArgCount)

#ifdef _WIN32
#define DISPATCH(Func, ...)  \
	ARG_EXPAND(DISPATCH_IMPL(Func, COUNT(__VA_ARGS__))(__VA_ARGS__))
#else
#define DISPATCH(Func, ...)  \
	DISPATCH_IMPL(Func, COUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

#define EAT(...)
#define EXPAND(...) __VA_ARGS__
#define WHEN(c) IF(c)(EXPAND, EAT)

#define EMPTY()
#define DEFER(id) id EMPTY()
#define EXPAND(...) __VA_ARGS__

#define EVAL5(...) __VA_ARGS__
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))

#define REPEAT(count, macro, ...) \
	WHEN(count) \
	( \
		DEFER(REPEAT_INDIRECT) () \
		( \
			DEC(count), macro, __VA_ARGS__ \
		) \
		DEFER(macro) \
		( \
			DEC(count), __VA_ARGS__ \
		) \
	)
#define REPEAT_INDIRECT() REPEAT

#define WHILE(pred, op, ...) \
	IF(pred(__VA_ARGS__)) \
	( \
		DEFER(WHILE_INDIRECT) () \
		( \
			pred, op, op(__VA_ARGS__) \
		), \
		__VA_ARGS__ \
	)
#define WHILE_INDIRECT() WHILE