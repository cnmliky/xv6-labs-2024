#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  // 1. 打开路径
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  // 2. 获取该路径的状态
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // find 只在目录中进行递归搜索
  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    // 如果直接查找的是文件，检查名字是否匹配
    // 这里为了逻辑统一，通常 find 处理的是目录下的内容
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/'; // 在路径后加上 "/"

    // 3. 读取目录中的每一个条目
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      
      // 拼接完整路径到 buf 中
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      // 获取当前条目的状态
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      // 4. 关键逻辑：如果是目录，且不是 "." 或 ".."，则递归
      if(st.type == T_DIR){
        if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0){
          find(buf, target);
        }
      } 
      // 5. 如果是文件，检查文件名是否与目标匹配
      else if(st.type == T_FILE){
        if(strcmp(de.name, target) == 0){
          printf("%s\n", buf);
        }
      }
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if(argc < 3){
    fprintf(2, "Usage: find <path> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
