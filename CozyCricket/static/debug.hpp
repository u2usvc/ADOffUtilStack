#define DEBUG_LEVEL 0

#if DEBUG_LEVEL >= 1
    #define DEBUG_ERR(fmt, ...) fprintf(stderr, "[-] ERROR: " fmt "\n", ##__VA_ARGS__)
    #define DEBUG_INFO(fmt, ...) printf("[+] INFO:  " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_ERR(fmt, ...) do {} while(0)
    #define DEBUG_INFO(fmt, ...) do {} while(0)
#endif

#if DEBUG_LEVEL >= 2
    #define DEBUG_VERBOSE(fmt, ...) printf("[*] VERB:  " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_VERBOSE(fmt, ...) do {} while(0)
#endif

#if DEBUG_LEVEL >= 3
    #define DEBUG_TRACE(fmt, ...) printf("[#] TRACE:  " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_TRACE(fmt, ...) do {} while(0)
#endif
