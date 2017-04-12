/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013-2017 François Tigeot
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef	_LINUX_ATOMIC_H_
#define	_LINUX_ATOMIC_H_

#include <asm/atomic.h>

typedef struct {
	volatile u_int counter;
} atomic_t;

typedef struct {
	volatile u_long counter;
} atomic64_t;

#define	atomic_add(i, v)		atomic_add_return((i), (v))
#define	atomic_sub(i, v)		atomic_sub_return((i), (v))
#define	atomic_inc_return(v)		atomic_add_return(1, (v))
#define	atomic_add_negative(i, v)	(atomic_add_return((i), (v)) < 0)
#define	atomic_sub_and_test(i, v)	(atomic_sub_return((i), (v)) == 0)
#define	atomic_dec_and_test(v)		(atomic_sub_return(1, (v)) == 0)
#define	atomic_inc_and_test(v)		(atomic_add_return(1, (v)) == 0)
#define atomic_dec_return(v)             atomic_sub_return(1, (v))

#define	atomic64_add(i, v)		atomic_add_return_long((i), (v))
#define	atomic64_sub(i, v)		atomic_sub_return_long((i), (v))

#define atomic_xchg(p, v)		atomic_swap_int(&((p)->counter), v)
#define atomic64_xchg(p, v)		atomic_swap_long(&((p)->counter), v)

#define atomic_cmpset(p, o, n)		atomic_cmpset_32(&((p)->counter), o, n)

#define atomic64_cmpxchg(p, o, n)						\
	(atomic_cmpset_long((volatile uint64_t *)(p),(o),(n)) ? (o) : (0))

static inline int
atomic_add_return(int i, atomic_t *v)
{
	return i + atomic_fetchadd_int(&v->counter, i);
}

static inline int64_t
atomic_add_return_long(int64_t i, atomic64_t *v)
{
	return i + atomic_fetchadd_long(&v->counter, i);
}

static inline int
atomic_sub_return(int i, atomic_t *v)
{
	return atomic_fetchadd_int(&v->counter, -i) - i;
}

static inline int64_t
atomic_sub_return_long(int64_t i, atomic64_t *v)
{
	return atomic_fetchadd_long(&v->counter, -i) - i;
}

static inline void
atomic_set(atomic_t *v, int i)
{
	atomic_store_rel_int(&v->counter, i);
}

static inline void
atomic64_set(atomic64_t *v, long i)
{
	atomic_store_rel_long(&v->counter, i);
}

static inline int
atomic_read(atomic_t *v)
{
	return atomic_load_acq_int(&v->counter);
}

static inline int64_t
atomic64_read(atomic64_t *v)
{
	return atomic_load_acq_long(&v->counter);
}

static inline int
atomic_inc(atomic_t *v)
{
	return atomic_fetchadd_int(&v->counter, 1) + 1;
}

static inline int
atomic_dec(atomic_t *v)
{
	return atomic_fetchadd_int(&v->counter, -1) - 1;
}

static inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
	return atomic_cmpxchg_int(&v->counter, old, new);
}

static inline int atomic_add_unless(atomic_t *v, int add, int unless)
{
        int c, old;
        c = atomic_read(v);
        for (;;) {
                if (unlikely(c == unless))
                        break;
                old = atomic_cmpxchg_int(&v->counter, c, c + add);
                if (likely(old == c))
                        break;
                c = old;
        }
        return c != unless;
}

#define atomic_inc_not_zero(v) atomic_add_unless((v), 1, 0)

/* atomic_clear_mask: atomically clears a variable from the bit set in mask */
#define atomic_clear_mask(mask, addr)		\
	/* atomic *addr &= ~mask; */		\
	__asm __volatile("lock andl %0, %1"	\
		:				\
		: "r" (~mask), "m" (*addr)	\
		: "memory");

#define smp_mb__before_atomic()	cpu_ccfence()
#define smp_mb__after_atomic()	cpu_ccfence()

static inline void
atomic_andnot(int i, atomic_t *v)
{
	/* v->counter = v->counter & ~i; */
	atomic_clear_int(&v->counter, i);
}

#endif	/* _LINUX_ATOMIC_H_ */
