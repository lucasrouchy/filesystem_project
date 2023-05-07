#include "image.h"
#include <fcntl.h>
#include <unistd.h>

int image_fd;

int image_open(char *filename, int truncate){
    if (truncate == 1){
        image_fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0600);
        return image_fd;
    }
    else{
        image_fd = open(filename, O_CREAT | O_RDWR, 0600);
        return image_fd;
    }
    
    return image_fd;
}
int image_close(){
    image_fd = close(image_fd);
    return image_fd;
}