#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char *nargv[MAXARG];
    char line[512];
    char ch;
    int base = 0;
    int len = 0;
    int pid;
  
    if (argc < 2) {
        fprintf(2, "usage: xargs command [args ...]\n");
        exit(1);
    }
  
    if (argc + 1 > MAXARG) {
        fprintf(2, "xargs: too many arguments\n");
        exit(1);
    }
  
    for (int i = 1; i < argc; i++) {
        nargv[base++] = argv[i];
    } 
  
    while(1) {
        int n = read(0, &ch, 1);
    
        if (n < 0) {
        fprintf(2, "xargs: read error\n");
        exit(1);
        }
  
        if (n == 0) {
            if (len == 0)
                break;
  
            line[len] = 0;
            nargv[base] = line;
            nargv[base + 1] = 0;
  
            pid = fork();
            if (pid == 0) {
                exec(nargv[0], nargv);
                fprintf(2, "xargs: exec %s failed\n", nargv[0]);
                exit(1);
            }
        wait(0);
        break;
        }
    
        if (ch == '\n') {
        line[len] = 0;
      
        nargv[base] = line;
        nargv[base + 1] = 0;
      
        pid = fork();
            if (pid == 0) {
                exec(nargv[0], nargv);
                fprintf(2, "xargs: exec %s failed\n", nargv[0]);
                exit(1);
            }
      
        wait(0);
        len = 0;
        } else {
            if (len + 1 >= sizeof(line)) {
                fprintf(2, "xargs: line too long\n");
                exit(1);
            }
            line[len++] = ch;
        }
    }
  
    exit(0);
}