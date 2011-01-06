#ifndef __RUN_BG_H__
#define __RUN_BG_H__

int run_bg(void (*fun)(), int wait);
int run_bg_with_argv(void (*fun)(void *), void * argv, int wait);
int exec_bg_and_wait(char* path, char * arg, ...);

#endif /* __RUN_BG_H__ */

