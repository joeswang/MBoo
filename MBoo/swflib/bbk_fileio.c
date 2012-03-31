#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wchar.h>

#include "bbk_fileio.h"

struct bbk_file_t {
    bbk_pool_t *pool;
    HANDLE filehand;
    BOOLEAN pipe;              /* Is this a pipe of a file? */
    OVERLAPPED *pOverlapped;
    //bbk_interval_time_t timeout;
    bbk_int32_t flags;

    /* File specific info */
    //bbk_finfo_t *finfo;
    char *fname;
    DWORD dwFileAttributes;
    int eof_hit;
    BOOLEAN buffered;          // Use buffered I/O?
    int ungetchar;             // Last char provided by an unget op. (-1 = no char)
    int append;

    /* Stuff for buffered mode */
    char *buffer;
    bbk_size_t bufpos;         // Read/Write position in buffer
    bbk_size_t bufsize;        // The size of the buffer
    bbk_size_t dataRead;       // amount of valid data read into buffer
    int direction;             // buffer being used for 0 = read, 1 = write
    //bbk_off_t filePtr;         // position in file of handle
    //bbk_thread_mutex_t *mutex; // mutex semaphore, must be owned to access the above fields

    /* if there is a timeout set, then this pollset is used */
    //bbk_pollset_t *pollset;

    /* Pipe specific info */
};
