#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void indent(int depth) {
  for(int i = 0; i < depth; i++){
    printf("  ");
  }
}

char* basename(char *path) {
  char *p = path;
  char *name = path;

  if(strcmp(path, "/") == 0)
    return path;

  while(*p) {
    if(*p == '/')
      name = p + 1;
    p++;
  }
  return name;
}

void print_node(char *path, int depth, int isdir) {
  indent(depth);

  if(strcmp(path, "/") == 0){
    printf("/\n");
    return;
  }

  if(isdir)
    printf("%s/\n", basename(path));
  else
    printf("%s\n", basename(path));
}

void tree(char *path, int depth) {
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512];
  char name[DIRSIZ + 1];

  if(stat(path, &st) < 0){
    fprintf(2, "tree: cannot stat %s\n", path);
    return;
  }

  print_node(path, depth, st.type == T_DIR);

  if(st.type != T_DIR)
    return;

  fd = open(path, 0);
  if(fd < 0){
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;

    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = '\0';

    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    if(strlen(path) + 1 + strlen(name) + 1 > sizeof(buf)){
      fprintf(2, "tree: path too long\n");
      continue;
    }

    if(strcmp(path, "/") == 0){
      buf[0] = '/';
      buf[1] = '\0';
      memmove(buf + 1, name, strlen(name) + 1);
    } else {
      strcpy(buf, path);
      int len = strlen(buf);
      buf[len] = '/';
      memmove(buf + len + 1, name, strlen(name) + 1);
    }

    tree(buf, depth + 1);
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if(argc > 2){
    fprintf(2, "usage: tree [directory]\n");
    exit(1);
  }

  if(argc == 1)
    tree(".", 0);
  else // argc == 2
    tree(argv[1], 0);

  exit(0);
}
