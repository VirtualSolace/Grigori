#ifdef DEBUG

#include <iostream>
#define PRINT_DBG(x) std::cout << x;
#define NPRINT_DBG(x) std::cout << x << std::endl;
#define DBG_LOOP(X) \
    for(const auto& x : (X)) \
        NPRINT_DBG(x);
#define SLEEP sleep(25)
#else

#define PRINT_DBG(x) ((void)0)
#define NPRINT_DBG(x) ((void)0)
#define DBG_LOOP(x) ((void)0)

#endif
