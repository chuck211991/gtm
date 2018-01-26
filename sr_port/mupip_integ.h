/****************************************************************
 *								*
 *	Copyright 2001 Sanchez Computer Associates, Inc.	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#ifndef MUPIP_INTEG_INCLUDED
#define MUPIP_INTEG_INCLUDED

void mupip_integ(void);

enum cum_type
{
	BLKS,
	RECS,
	SIZE,
	CUM_TYPE_MAX
};

enum tot_type
{
	DATA,
	INDX,
	DIRTREE,
	TOT_TYPE_MAX
};

#endif /* MUPIP_INTEG_INCLUDED */
