/*	$NetBSD: mem2.c,v 1.12 2021/08/01 17:59:47 rillig Exp $	*/

/*
 * Copyright (c) 1994, 1995 Jochen Pohl
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Jochen Pohl for
 *	The NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#include <sys/cdefs.h>
#if defined(__RCSID) && !defined(lint)
__RCSID("$NetBSD: mem2.c,v 1.12 2021/08/01 17:59:47 rillig Exp $");
#endif

#include <sys/param.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "lint2.h"

/* length of new allocated memory blocks */
static size_t	mblklen;

/* offset of next free byte in mbuf */
static size_t	nxtfree;

/* current buffer to server memory requests from */
static void	*mbuf;

void
initmem(void)
{
	int	pgsz;

	pgsz = getpagesize();
	mblklen = ((MBLKSIZ + pgsz - 1) / pgsz) * pgsz;

	nxtfree = mblklen;
}

/*
 * Allocate memory in large chunks to avoid space and time overhead of
 * malloc(). This is possible because memory allocated by xalloc()
 * need never to be freed.
 */
void *
xalloc(size_t sz)
{
	void	*ptr;

	/* Align to at least 8 bytes. */
	sz = (sz + 7) & ~7L;
	if (nxtfree + sz > mblklen) {
		mbuf = xmalloc(mblklen);
		(void)memset(mbuf, 0, mblklen);
		nxtfree = 0;
	}

	ptr = (char *)mbuf + nxtfree;
	nxtfree += sz;

	return ptr;
}
