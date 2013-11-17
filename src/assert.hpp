#ifndef ASSERT_HPP_INCLUDED
#define ASSERT_HPP_INCLUDED

#ifdef DEBUG

	#ifndef ASSERT_FAIL
		#include <sstream>
		#include <stdexcept>
		void _assertFail(const char* file, int line) {
			std::stringstream ss;
			ss << "Assertion error at " << file << ":" << line;
			throw std::runtime_error(ss.str());
		}
		#define ASSERT_FAIL(file, line) _assertFail(file, line)
	#endif

    #define ASSERT(a) ((void)(!(a)? (ASSERT_FAIL(__FILE__, __LINE__), 1): 1))
#else
    #define ASSERT(a) ((void)sizeof(a))
#endif


#endif // ASSERT_HPP_INCLUDED
