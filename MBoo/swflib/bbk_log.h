#ifndef BBK_LOG_H
#define BBK_LOG_H

#include "bbk_types.h"
#include "bbk_pools.h"
#include "bbk_fileio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BBKLOG_EMERG     0       /* system is unusable */
#define BBKLOG_ALERT     1       /* action must be taken immediately */
#define BBKLOG_CRIT      2       /* critical conditions */
#define BBKLOG_ERR       3       /* error conditions */
#define BBKLOG_WARNING   4       /* warning conditions */
#define BBKLOG_NOTICE    5       /* normal but significant condition */
#define BBKLOG_INFO      6       /* informational */
#define BBKLOG_DEBUG     7       /* debug-level messages */


#ifdef __cplusplus
}
#endif


#endif /* BBK_LOG_H */
