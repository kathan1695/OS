00027 #ifndef __semaphore_h__
00028 #define __semaphore_h__
00029 
00030 #ifdef  __cplusplus
00031 extern "C" {
00032 #endif
00033 
00034 #include <config.h>
00035 #include <time.h> /* time_t */
00036 #include <atomic.h>
00037 
00038 #ifdef CONF_SEMAPHORES
00039 
00041 //
00042 // Definitions
00043 //
00045 
00046 typedef atomic_t sem_t;                         
00047 
00048 #define EAGAIN  0xffff                          
00049 
00050 
00051 //
00052 // Functions
00053 //
00055 
00057 
00064 extern inline int sem_init(sem_t * sem, int pshared, unsigned int value)
00065 {
00066   *sem = (sem_t) value;
00067   return 0;
00068 }
00069 
00071 
00078 extern int sem_wait(sem_t * sem);
00079 
00100 extern int sem_timedwait(sem_t * sem,
00101                          const time_t abs_timeout);
00102 
00104 
00114 extern int sem_trywait(sem_t * sem);
00115 
00117 
00123 extern inline int sem_post(sem_t * sem) 
00124 { 
00125         atomic_inc(sem);
00126         return 0;
00127 }
00128 
00129 //
00131 //
00132 extern inline int sem_getvalue(sem_t * sem, int *sval)
00133 {
00134   *sval = *sem;
00135   return 0;
00136 }
00137 
00139 
00147 extern inline int sem_destroy(sem_t * sem)
00148 {
00149   return 0;
00150 }
00151 
00152 #endif // CONF_SEMAPHORES
00153 
00154 #ifdef  __cplusplus
00155 }
00156 #endif
00157#endif // __semaphore_h__
