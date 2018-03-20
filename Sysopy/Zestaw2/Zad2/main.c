#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <ftw.h>
#include <libgen.h>

time_t reference_time;
int comp_sign;


void wr_file_info(char * path, char * prefix, struct stat file_stat){
    if((comp_sign == -1 && reference_time > file_stat.st_mtime) ||
       (comp_sign ==  0 && reference_time == file_stat.st_mtime) ||
       (comp_sign ==  1 && reference_time < file_stat.st_mtime)) {
        
        switch (file_stat.st_mode & S_IFMT) {
            case S_IFBLK:
                return;                                     //block device
            case S_IFCHR:
                return;                                     //character device
            case S_IFDIR:
                return;                                     //directory
            case S_IFIFO:
                return;                                     //FIFO/pipe
            case S_IFLNK:
                return;                                     //symlink
            case S_IFREG:
                break;                                      //regular file
            case S_IFSOCK:
                return;                                     //socket
            default:
                return;                                     //unknown?
        }
        
        
        printf("%s", prefix);
        /* FILE PERMISSION */
        printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
        printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
        printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
        printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
        printf("\t");
        
        /* FILE SIZE IN BYTES */
        printf("%d bytes\t", (int) file_stat.st_size);
        
        /* LAST MODIFICATION */
        char data[32];
        strftime(data, 32, "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
        printf("%s   ", data);
        
        /* ABSOLUTE PATH */
        printf("%s\n", realpath(path, NULL));
    }
}

void read_dir_tree_path(char * path, int lvl){
    DIR * d_path = opendir(path);
    if(d_path == NULL){
        perror(path);
        exit(EXIT_FAILURE);
    }
    
    char new_path[PATH_MAX];
    
    char * is_prefix = "|--> ";
    char real_prefix [PATH_MAX];
    if(lvl > 0) strcpy(real_prefix,"|    ");
    for(int i = 0; i < lvl-1; i++){
        strcat(real_prefix,"|    ");
    }
    if(lvl == 0) strcpy(real_prefix,is_prefix);
    else strcat(real_prefix,is_prefix);
    
    struct dirent * r_dir;
    struct stat file_stat;
    
    if(lvl == 0) printf("%s\n",basename(path));
    while ((r_dir = readdir(d_path)) != NULL) {
        strcpy(new_path,path);
        strcat(new_path, "/");
        strcat(new_path, r_dir->d_name);
        
        if(lstat(new_path,&file_stat) < 0) exit(EXIT_FAILURE);
        if(file_stat.st_mode & S_IFMT & S_IFDIR) {
            if(strcmp(r_dir->d_name,".") != 0 && strcmp(r_dir->d_name, "..") != 0){
                printf ("%s%s\n",real_prefix,r_dir->d_name);
                read_dir_tree_path(new_path,lvl+1);
            }
        }
        wr_file_info(new_path,real_prefix,file_stat);
    }
    closedir (d_path);
}

int wr_file_info_nftw(const char *path, const struct stat *file_stat, int type_flag, struct FTW *ftwbuf){
    int lvl = ftwbuf->level;
    
    char *is_prefix = "|--> ";
    char real_prefix[PATH_MAX];
    if (lvl > 1) strcpy(real_prefix, "|    ");
    for (int i = 1; i < lvl - 1; i++) {
        strcat(real_prefix, "|    ");
    }
    if (lvl <= 1) strcpy(real_prefix, is_prefix);
    else strcat(real_prefix, is_prefix);
    
    
    if (S_ISDIR(file_stat->st_mode) ) {
        if(lvl > 0) printf("%s", real_prefix);
        printf("%s\n", basename((char *)path));
        return 0;
    }
    
    if((comp_sign == -1 && reference_time > file_stat->st_mtime) ||
       (comp_sign ==  0 && reference_time == file_stat->st_mtime) ||
       (comp_sign ==  1 && reference_time < file_stat->st_mtime)) {      //
        if (lvl > 0) printf("%s", real_prefix);
        
        /* FILE PERMISSION */
        printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
        printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
        printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
        printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");
        printf("\t");
        
        /* FILE SIZE IN BYTES */
        printf("%d bytes\t", (int) file_stat->st_size);
        
        /* LAST MODIFICATION */
        char data[32];
        strftime(data, 32, "%Y-%m-%d %H:%M:%S", localtime(&file_stat->st_mtime));
        printf("%s   ", data);
        
        /* ABSOLUTE PATH */
        printf("%s\n", realpath(path, NULL));
    }
    return 0;
}

void read_dir_tree_nftw(const char * path, int fd_limit, int flags){
    if(nftw(path,wr_file_info_nftw,fd_limit,flags) == -1){
        perror("nftw");
        exit(EXIT_FAILURE);
    }
}

void set_reference_time(char * time,char * c_sign, char * info){
    struct tm tm;
    strptime(time,"%Y-%m-%d %H:%M:%S",&tm);
    reference_time = mktime(&tm);
    
    if(strcmp(c_sign,"<") == 0) comp_sign = 1;
    else if(strcmp(c_sign,">") == 0) comp_sign = -1;
    else if(strcmp(c_sign,"==") == 0) comp_sign = 0;
    else{
        printf("%s",info);
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[]){
    char * info = "Arguments should be: [stat|ntfw] [path] ['<'|'=='|'>'] [YYYY-mm-dd] [HH:MM:SS]";
    
    if(argc < 6){
        printf("%s",info);
    }
    
    else if((strcmp(argv[1],"stat") == 0 || strcmp(argv[1],"ntfw") == 0)){
        char * abs_path = realpath(argv[2],NULL);
        char thetime[20];
        strcpy(thetime,argv[4]);
        thetime[10] = ' ';
        strcat(thetime,argv[5]);
        thetime[19] = '\0';
        set_reference_time(thetime,argv[3],info);
        
        if(strcmp(argv[1],"stat") == 0) read_dir_tree_path(abs_path,0);
        else read_dir_tree_nftw(abs_path,10,FTW_PHYS);
        
        
    }
    exit(EXIT_SUCCESS);
}
