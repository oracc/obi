#ifndef WARNING_H_
#define WARNING_H_
#include <stdio.h>
#undef warning
extern const char *textid;
extern FILE *f_log;
extern const char *file;
extern const char *phase;
extern int lnum;
extern int lstatus;
extern int status;
extern int exit_status;
extern int with_textid;
extern int verbose;
extern void vwarning(const char *fmt,...);
extern void vwarning2(const char *myfile, int mylnum, const char *fmt,...);
extern void notice(const char *str);
extern void notice2(const char *myfile, int mylnum, const char *str);
extern void vnotice(const char *fmt,...);
extern void vnotice2(const char *myfile, int mylnum, const char *fmt,...);
extern void warning(const char *str);
extern void print_error_count(FILE *fp, int force);
extern void warning_init(void);
#endif/*WARNING_H_*/
