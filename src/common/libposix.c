
#include "libposix.h"
#include <stdio.h>
#include <stdlib.h>

#define PATH_SPLIT '/'

int rand_int(int min, int max) {
    static int s_seed = 0;
    assert(max > min);
    srand(time(NULL) + s_seed * s_seed);
    s_seed++;

    int _rand = rand();
    _rand = min + (int)((double)((double)(max) - (min) + 1.0) * ((_rand) / ((RAND_MAX) + 1.0)));
    return _rand;
}

char* rand_str(char* buf, int len) {
    static int s_seed = 0;
    static char* s_characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    if (buf == NULL)
        buf = (char*)calloc(1, len + 1);

    int i = 0;
    srand(time(NULL) + s_seed * s_seed);
    s_seed++;
    for (; i < len; i++) {
        buf[i] = s_characters[rand() % 62];
    }
    buf[i] = '\0';
    return buf;
}

int get_proc_name(char* name, size_t len) {
    int i, ret;
    char proc_name[PATH_MAX];
    char* ptr = NULL;
    memset(proc_name, 0, sizeof(proc_name));
    if (-1 == readlink("/proc/self/exe", proc_name, sizeof(proc_name))) {
        fprintf(stderr, "readlink failed!\n");
        return -1;
    }
    ret = strlen(proc_name);
    for (i = ret, ptr = proc_name; i > 0; i--) {
        if (ptr[i] == PATH_SPLIT) {
            ptr += i + 1;
            break;
        }
    }
    if (i == 0) {
        fprintf(stderr, "proc path %s is invalid\n", proc_name);
        return -1;
    }
    if (ret - i > (int)len) {
        fprintf(stderr, "proc name length %d is larger than %d\n", ret - i, (int)len);
        return -1;
    }
    strncpy(name, ptr, ret - i);
    return 0;
}

/*---------------------------------------------------------------------------
                            version
----------------------------------------------------------------------------*/
int version_atoi(const char* str) {
    int hex = 0;

    // trim v1.2.3.4
    const char* pv = strchr(str, 'v');
    const char* pdot = pv ? pv + 1 : str;

    while (1) {
        hex = (hex << 8) | atoi(pdot);
        pdot = strchr(pdot, '.');
        if (pdot == NULL) break;
        ++pdot;
    }

    return hex;
}

void version_itoa(int num, char* str) {
    char* ch = (char*)&num;
    sprintf(str, "%d.%d.%d.%d", ch[3], ch[2], ch[1], ch[0]);

    // trim 0.1.2.3
    const char* p = str;
    while (1) {
        if (p[0] == '0' && p[1] == '.') {
            p += 2;
        } else {
            break;
        }
    }

    if (p != str) {
        strcpy(str, p);
    }
}

/*---------------------------------------------------------------------------
                            File and Path
----------------------------------------------------------------------------*/
char* strrchr_dir(const char* filepath) {
    char* p = (char*)filepath;
    while (*p)
        ++p;
    while (--p >= filepath) {
#ifdef OS_WIN
        if (*p == '/' || *p == '\\')
#else
        if (*p == '/')
#endif
            return p;
    }
    return NULL;
}

const char* file_basename(const char* filepath) {
    const char* pos = strrchr_dir(filepath);
    return pos ? pos + 1 : filepath;
}

const char* file_suffixname(const char* filename) {
    const char* pos = strrchr_dot(filename);
    return pos ? pos + 1 : "";
}

int mkdir_p(const char* dir) {
    if (access(dir, 0) == 0) {
        return EEXIST;
    }
    char tmp[MAX_PATH] = {0};
    strncpy(tmp, dir, sizeof(tmp));
    char* p = tmp;
    char delim = '/';
    while (*p) {
#ifdef OS_WIN
        if (*p == '/' || *p == '\\') {
            delim = *p;
#else
        if (*p == '/') {
#endif
            *p = '\0';
            mkdir_777(tmp);
            *p = delim;
        }
        ++p;
    }
    if (mkdir_777(tmp) != 0) {
        return EPERM;
    }
    return 0;
}

char* get_executable_path(char* buf, int size) {
#ifdef OS_WIN
    GetModuleFileName(NULL, buf, size);
#elif defined(OS_LINUX)
    if (readlink("/proc/self/exe", buf, size) == -1) {
        return NULL;
    }
#elif defined(OS_DARWIN)
    _NSGetExecutablePath(buf, (uint32_t*)&size);
#endif
    return buf;
}

char* get_executable_dir(char* buf, int size) {
    char filepath[MAX_PATH] = {0};
    get_executable_path(filepath, sizeof(filepath));
    char* pos = strrchr_dir(filepath);
    if (pos) {
        *pos = '\0';
        strncpy(buf, filepath, size);
    }
    return buf;
}

char* get_executable_file(char* buf, int size) {
    char filepath[MAX_PATH] = {0};
    get_executable_path(filepath, sizeof(filepath));
    char* pos = strrchr_dir(filepath);
    if (pos) {
        strncpy(buf, pos + 1, size);
    }
    return buf;
}

char* get_run_dir(char* buf, int size) {
    return getcwd(buf, size);
}

