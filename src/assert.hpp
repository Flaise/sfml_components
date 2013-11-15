#ifndef ASSERT_HPP_INCLUDED
#define ASSERT_HPP_INCLUDED


#ifndef ASSERT_FAIL
	#include <iostream>
	#define ASSERT_FAIL(file, line) std::cout << "Assertion fail: " << "\nFile: " << file << "\nLine: " << line << "\n"
#endif

#ifdef DEBUG
    #define ASSERT(a) \
        do { if (!(a)) { ASSERT_FAIL(__FILE__, __LINE__); } } while(0)
#else
    #define ASSERT(a) \
        do { (void)sizeof(a); } while(0)
#endif


#endif // ASSERT_HPP_INCLUDED
