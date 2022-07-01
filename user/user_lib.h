/*
 * header file to be used by applications.
 */

int printu(const char *s, ...);
int exit(int code);
void* naive_malloc();
void naive_free(void* va);
int fork();
void yield();

//TODO(lab3_2_challenge):（3）声明信号量的申请、释放和增减操作函数：
int sem_new(int);
int sem_P(int);
int sem_V(int);
