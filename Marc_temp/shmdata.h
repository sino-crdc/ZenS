#ifndef SHAREMEMORY_SHMDATA_H
#define SHAREMEMORY_SHMDATA_H
#define TEXT_SIZE 1024
struct shared_data{
    int written;
    char text[TEXT_SIZE];
};
#endif //SHAREMEMORY_SHMDATA_H
