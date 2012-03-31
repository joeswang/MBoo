#ifndef BBK_TYPES_H
#define BBK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define BBK_UINT32_MAX (0xffffffffU)

#define BBK_INT_TRUNC_CAST int
#define BBK_UINT32_TRUNC_CAST bbk_uint32_t

/* BBK_ALIGN() is only to be used to align on a power of 2 boundary */
#define BBK_ALIGN(size, boundary) (((size) + ((boundary) -1)) & ~((boundary) -1))

/** Default alignment */
#define BBK_ALIGN_DEFAULT(size)	BBK_ALIGN(size, 8)

#define BBK_DECLARE(type)	type

#define BBK_OS_START_CANONERR	2000

#define BBK_SUCCESS	0

#define BBK_ENOMEM		(BBK_OS_START_CANONERR + 7)

typedef	unsigned char	bbk_byte_t;

typedef short			bbk_int16_t;
typedef	unsigned short	bbk_uint16_t;

typedef	int				bbk_int32_t;
typedef	unsigned int	bbk_uint32_t;

typedef int				bbk_status_t;

typedef size_t			bbk_size_t;

typedef bbk_uint16_t    bbk_wchar_t;

#ifdef __cplusplus
}
#endif


#endif /* BBK_TYPES_H */
