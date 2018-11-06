/**
 * @file stdint.h
 *
 * Signed and unsigned integer types of exact width.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _STDINT_H_
#define _STDINT_H_

/* SIGNED DEFINITIONS */

typedef signed char				int8_t;

#if __SIZEOF_SHORT__ == 2
typedef short					int16_t;
#endif

#if __SIZEOF_INT__ == 4
typedef int						int32_t;
typedef int						intmax_t;
#else
typedef long int				int32_t;
typedef long int				intmax_t;
#endif

#if __SIZEOF_LONG__ == 8
typedef long int				int64_t;
#else
typedef long long int			int64_t;
#endif

/* UNSIGNED DEFINITIONS */

typedef unsigned char			uint8_t;

#if __SIZEOF_SHORT__ == 2
typedef unsigned short			uint16_t;
#endif

#if __SIZEOF_INT__ == 4
typedef unsigned int			uint32_t;
#else
typedef unsigned long int		uint32_t;
#endif

#if __SIZEOF_LONG__ == 8
typedef unsigned long int		uint64_t;
#else
typedef unsigned long long int	uint64_t;
#endif

/* POINTER DEFINITIONS */

#if __SIZEOF_POINTER__ == 4
#if __SIZEOF_INT__ == 4
typedef unsigned int			uintptr_t;
#else
typedef unsigned long			uintptr_t;
#endif
#endif

#if __SIZEOF_POINTER__ == 8
#if __SIZEOF_LONG__ == 8
typedef unsigned long int		uintptr_t;
#else
typedef unsigned long long int	uintptr_t;
#endif
#endif

/* Limits of integral types.  */

/* Minimum of signed integral types.  */
# define INT8_MIN               (-0x7f-1)
# define INT16_MIN              (-0x7fff-1)
# define INT32_MIN              (-0x7fffffff-1)
# define INT64_MIN              (-0x7fffffffffffffff-1)
/* Maximum of signed integral types.  */
# define INT8_MAX				(0x7f)
# define INT16_MAX              (0x7fff)
# define INT32_MAX				(0x7fffffff)
# define INT64_MAX              (0x7fffffffffffffff)

/* Maximum of unsigned integral types.  */
# define UINT8_MAX              (0xff)
# define UINT16_MAX             (0xffff)
# define UINT32_MAX             (0xffffffff)
# define UINT64_MAX             (0xffffffffffffffff)

#endif /* _STDINT_H_ */
