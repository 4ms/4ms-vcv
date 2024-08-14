// #define HAVE_ENDIAN_H    0
// #define HAVE_FCNTL_H     0
// #define HAVE_SCHED_H     0
// #define HAVE_UNISTD_H    0
// #define HAVE_SYS_PARAM_H 0
// #define HAVE_SYS_STAT_H  0
// #define HAVE_SYS_TIME_H  0
// #define HAVE_SYS_TYPES_H 0
#define HAVE_STDINT_H 1

// #define HAVE_CLOSE        0
// #define HAVE_GETPID       0
// #define HAVE_GETTIMEOFDAY 0
// #define HAVE_OPEN         0
// #define HAVE_READ         0

#ifdef HAVE_SCHED_YIELD
#undef HAVE_SCHED_YIELD
#endif

// #define HAVE_SYNC_BUILTINS   0
#define HAVE_ATOMIC_BUILTINS 1

// #define HAVE_LOCALE_H  0
// #define HAVE_SETLOCALE 0

#define HAVE_INT32_T  1
#define HAVE_UINT32_T 1
#define HAVE_UINT16_T 1
#define HAVE_UINT8_T  1
#define HAVE_SSIZE_T  1

// #define USE_URANDOM           0
// #define USE_WINDOWS_CRYPTOAPI 0

#define INITIAL_HASHTABLE_ORDER 3
