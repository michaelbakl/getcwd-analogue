#include "apue/include/apue.h"
#include <dirent.h>
#include <cstdio>
#include <sys/dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

__uint8_t DT_LNK = 10;

bool haveSameStats(struct stat s1, struct stat s2) {
    return s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino;
}

int countAllSymbolicLinksInDir(DIR *dir, struct stat start_info) {
    int counter = 0;
    struct dirent *entity;
    struct stat tmp_info;
    struct stat file_info;
    lstat(".", &file_info);

    while ((entity = readdir(dir))) {
        if (entity->d_type == DT_LNK) {
            stat(entity->d_name, &tmp_info);
            if (haveSameStats(tmp_info, start_info)) {
                printf("Sym link: %s\n", entity->d_name);
                counter++;
            }
        }
    }
    return counter;
}

int countSymLinkV2() {
    int counter = 0;

    struct stat start_dir_info;
    struct stat sand_box_info;

    ino_t inode_num;
    char *myDir;

    uid_t my_uid = getuid();
    uid_t root_uid = 0;
    struct passwd *userInfo = getpwuid(my_uid);
    myDir = userInfo->pw_dir;

    lstat(myDir, &sand_box_info);
    //lstat("/", &sand_box_info);

    lstat(".", &start_dir_info);


    for (DIR* dir; true;) {

        if ((dir = opendir(".")) == nullptr) {
            break;
        }

        counter += countAllSymbolicLinksInDir(dir, start_dir_info);


        struct stat dir_stat;

        if (stat(".", &dir_stat) == -1) {
            break;
        }

//        if (haveSameStats(dir_stat, sand_box_info)) {
//            counter = 0;
//        }

        const dev_t current_dir_dev = dir_stat.st_dev;
        const ino_t current_dir_ino = dir_stat.st_ino;

        if (stat("..", &dir_stat) == -1) {
            break;
        }

        if (dir_stat.st_dev == current_dir_dev && dir_stat.st_ino == current_dir_ino) {
            break;
        }

        if (chdir("..") == -1) {
            break;
        }
    }
    return counter;
}

int main(int _argc, char *_argv[]) {
    int counter = countSymLinkV2();

    printf("Counter: %d\n", counter);
}