#ifndef CONFIG_H_
#define CONFIG_H_

#define USE_PRINTF_ON_RUN	1




// enable or disable printf macro(depends on flag)
#if USE_PRINTF_ON_RUN == 1
#define RUN_PRINTF(...)			printf(__VA_ARGS__)
#else 
#define RUN_PRINTF(...)
#endif

#endif
