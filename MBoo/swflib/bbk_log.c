#include <stdio.h>
#include <stdlib.h>

#include "bbk_log.h"

typedef struct _TRANS {
	char    *t_name;
	int     t_val;
} TRANS;

static const TRANS priorities[] = {
    {"emerg",   BBKLOG_EMERG},
    {"alert",   BBKLOG_ALERT},
    {"crit",    BBKLOG_CRIT},
    {"error",   BBKLOG_ERR},
    {"warn",    BBKLOG_WARNING},
    {"notice",  BBKLOG_NOTICE},
    {"info",    BBKLOG_INFO},
    {"debug",   BBKLOG_DEBUG},
    {NULL,      -1},
};

static bbk_pool_t *stderr_pool = NULL;
static bbk_file_t *stderr_log = NULL;
/*
BBK_DECLARE(bbk_status_t) bbk_atomic_init(bbk_pool_t *p)
{
	return BBK_SUCCESS;
}
*/