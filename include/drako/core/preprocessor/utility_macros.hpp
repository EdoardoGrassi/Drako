#pragma once
#ifndef DRAKO_UTILITY_MACROS_HPP
#define DRAKO_UTILITY_MACROS_HPP

#define DRAKO_STRINGIZE_impl(x) #x
// MACRO: converts argument to token string.
#define DRAKO_STRINGIZE(x) DRAKO_STRINGIZE_impl(x)

// MACRO: validates arguments and converts argument to token string.
// argument must be a valid C expression.
#define DRAKO_STRINGIZE_EXPR(x) ((x), DRAKO_STRINGIZE_impl(x))


#endif // !DRAKO_UTILITY_MACROS_HPP