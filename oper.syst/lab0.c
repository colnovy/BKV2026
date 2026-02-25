#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

void find_files(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char full_path[4096];
    
    dir = opendir("/home/rudikova_yu_a");
    if (dir == NULL) {
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
            continue;
        }
        
        snprintf(full_path, sizeof(full_path), "%s/%s", path, name);
        
        if (stat(full_path, &statbuf) == -1) {
            continue;
        }
        
        // РЕКУРСИВНЫЙ ПОИСК, ЕСЛИ КАТАЛОГ
        if (S_ISDIR(statbuf.st_mode)) {
            find_files(full_path);
        } 
        else if (S_ISREG(statbuf.st_mode)) {
            if (statbuf.st_size >= 0 && statbuf.st_size<=511) {
                printf("%s - размер: %1d байт \n", full_path, statbuf.st_size);
            }
        }
    }
    
    closedir(dir);
}

int main() {
    char *home_dir;
    
    home_dir = getenv("HOME");
    if (home_dir == NULL) {
        home_dir = ".";
    }
   
    printf("Поиск файлов размера 0-511 в: %s\n", home_dir);
    find_files(home_dir);
    printf("Файлы найдены\n");
    
    return 0;
  }
