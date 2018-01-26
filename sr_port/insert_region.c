/****************************************************************
 *								*
 * Copyright (c) 2001-2017 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

/* 	insert_region.c
 *	requirement
 *		reg		initialized (gvcst_init)
 *	parameters
 *		reg		the region to be inserted
 *		reg_list	pointer to the pointer to the list head
 *		reg_free_list	pointer to the pointer to the free list
 *		size		size of the structure of each item in the list
 *	return
 *		pointer to the item in the list that is corresponding to the region.
 *		*reg_list and *reg_free_list are also updated if needed.
 *	fid_index field in csa and tp_reg_list is maintained by gvcst_init. Maintaining tp_reg_list is
 *	important, since the regions might be re-sorted in between insert_region() calls (i.e. new
 *	regions opening). All callers of insert_region except for dse_all() either use tp_reg_list or do not
 *	have the regions open.  dse_all() opens the regions before it calls insert_region(), so maintaining
 *	fid_index in tp_reg_list is sufficient.
 */

#include "mdef.h"

#include "gtm_ipc.h"		/* needed for FTOK */

#include "gtm_string.h"
#include "gdsroot.h"
#include "gtm_facility.h"
#include "fileinfo.h"
#include "gdsbt.h"
#include "gdsfhead.h"
#include "gdscc.h"
#include "gdskill.h"
#include "filestruct.h"
#include "jnl.h"
#include "buddy_list.h"		/* needed for tp.h */
#include "hashtab_int4.h"	/* needed for tp.h */
#include "tp.h"
#include "iosp.h"
#include "dbfilop.h"
#include "gtmmsg.h"
#include "is_file_identical.h"
#include "t_retry.h"
#include "wcs_mm_recover.h"
#include "gtmimagename.h"

GBLREF	gd_region	*gv_cur_region;
GBLREF	uint4		dollar_tlevel;
GBLREF	unsigned int	t_tries;

error_def(ERR_DBFILOPERR);

tp_region	*insert_region(	gd_region	*reg,
		   		tp_region	**reg_list,
		   		tp_region	**reg_free_list,
		   		int4		size)
{
	boolean_t	t_retry_needed;
	enc_info_t	*encr_ptr;
	int4		local_fid_index, match;
	sgmnt_addrs	*csa, *tr_csa;
	tp_region	*tr, *tr_last, *tr_new;
	unique_file_id	local_id;
	int		save_errno;
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	assert(SIZEOF(tp_region) <= size);
	assert(!IS_GTM_IMAGE || dollar_tlevel);
	if (reg->open)
		csa = (sgmnt_addrs *)&FILE_INFO(reg)->s_addrs;
	if (!reg->open)
	{
		if (!mupfndfil(reg, NULL, LOG_ERROR_TRUE))
			return NULL;
		if (SS_NORMAL != (save_errno = filename_to_id(&local_id.uid, (char *)reg->dyn.addr->fname)))
		{	/* WARNING: assignment above */
			gtm_putmsg_csa(CSA_ARG(NULL) VARLSTCNT(5) ERR_DBFILOPERR, 2, LEN_AND_STR(reg->dyn.addr->fname), save_errno);
			return NULL;
		}
	} else
		local_fid_index = csa->fid_index;
	/* See if the region is already on the list or if we have to add it */
	for (tr = *reg_list, tr_last = NULL; NULL != tr; tr = tr->fPtr)
	{
		if (reg->open)
		{	/* gvcst_init must have sorted them and filled in the fid_index field of node_local */
			assert(tr->reg->open);
			/* note that it is possible that "reg" and "tr->reg" are different although their "fid_index" is the same.
			 * this is possible if both regions point to the same physical file.
			 * in this case we return the existing "tr" instead of creating a new one.
			 */
			if (local_fid_index == tr->file.fid_index)	/* Region is found */
			{	/* assert we are not in final retry or we are in TP and have crit on the region already */
				assert((CDB_STAGNATE > t_tries)
					|| (dollar_tlevel && reg->open && csa->now_crit));
				return tr;
			}
			if ((tr->file.fid_index > local_fid_index))
				break;				/* .. we have found our insertion point */
		} else
		{
			if (reg == tr->reg)	/* Region is found */
			{	/* assert we are not in final retry or we are in TP and have crit on the region already */
				assert((CDB_STAGNATE > t_tries)
					|| (dollar_tlevel && reg->open && csa->now_crit));
				return tr;
			}
			/* let's sort here */
			if (!tr->reg->open)
			{	/* all regions closed */
				match = gdid_cmp(&(tr->file.file_id), &(local_id.uid));
			} else
			{	/* the other regions are open, i.e. file is pointing to fid_index, use file_id
				 * from node_local */
				tr_csa = (sgmnt_addrs *)&FILE_INFO(tr->reg)->s_addrs;
				match = gdid_cmp(&(tr_csa->nl->unique_id.uid), &(local_id.uid));
			}
			if (0 == match)
				return tr;
			if (0 < match)
				break;				/* .. we have found our insertion point */
		}
		tr_last = tr;
	}
	if ((NULL != reg_free_list) && (NULL != *reg_free_list))	/* Get a used block off our unused queue */
	{
		tr_new = *reg_free_list;		/* Get element */
		*reg_free_list = tr_new->fPtr;		/* Remove from queue */
	} else						/* get a new one */
	{
		tr_new = (tp_region *)malloc(size);
		if (size > SIZEOF(tp_region))
			memset(tr_new, 0, size);
	}
	tr_new->reg = reg;				/* Add this region to end of list */
	if (!reg->open)
		tr_new->file.file_id = local_id.uid;
	else
		tr_new->file.fid_index = local_fid_index;
	if (NULL == tr_last)
	{	/* First element on the list */
		tr_new->fPtr = *reg_list;
		*reg_list = tr_new;
	} else
	{	/* Insert into list */
		tr_new->fPtr = tr_last->fPtr;
		tr_last->fPtr = tr_new;
	}
	if ((CDB_STAGNATE <= t_tries) && dollar_tlevel && reg->open && !csa->now_crit)
	{	/* Final retry in TP and this region not locked down. Get crit on it if it is open.
		 * reg->open needs to be checked above to take care of the case where we do an insert_region() from gvcst_init()
		 * 	in the 3rd retry in TP when we have not yet opened the region. In case region is not open,
		 * 	tp_restart() (invoked through t_retry from gvcst_init) will open "reg" as well as get crit on it for us.
		 */
		DEBUG_ONLY(TREF(ok_to_call_wcs_recover) = TRUE;)
		t_retry_needed = (FALSE == grab_crit_immediate(reg, OK_FOR_WCS_RECOVER_TRUE));	/* Attempt lockdown now */
		if (!t_retry_needed)
		{	/* The "grab_crit_immediate" returned successfully. Check if encryption cycles match.
			 * If they dont, we need to do "grab_crit_encr_cycle_check" but that uses grab_crit and
			 * is not easily possible to convert it to a "grab_crit_immediate". So assume as if
			 * the "grab_crit_immediate" failed in this case too and do "t_retry" instead which will
			 * do the right thing of releasing crit on all regions and fixing the encryption cycles
			 * on all regions before getting crit on all of them.
			 */
			encr_ptr = csa->encr_ptr;
			if ((NULL != encr_ptr) && (csa->nl->reorg_encrypt_cycle != encr_ptr->reorg_encrypt_cycle))
			{
				rel_crit(reg);
				t_retry_needed = TRUE;
			}
		}
		if (t_retry_needed)
		{
			DEBUG_ONLY(TREF(ok_to_call_wcs_recover) = FALSE;)
			t_retry(cdb_sc_needcrit);	/* avoid deadlock -- restart transaction */
			assert(FALSE);			/* should not come here as t_retry() does not return */
		}
		DEBUG_ONLY(TREF(ok_to_call_wcs_recover) = FALSE;)
		assert(csa->now_crit);	/* ensure we have crit now */
		CHECK_MM_DBFILEXT_REMAP_IF_NEEDED(csa, reg);
#		ifdef UNIX
		if (MISMATCH_ROOT_CYCLES(csa, csa->nl))
		{	/* Going into this retry, we have already checked in tp_restart for moved root blocks in tp_reg_list.
			 * Since we haven't yet checked this region, we check it here and reset clues for an globals in the
			 * newly inserted region. We don't want to reset ALL gvt clues because the current retry may have made
			 * use (and valid use at that) of clues for globals in other regions.
			 */
			RESET_ALL_GVT_CLUES_REG(csa);
			csa->root_search_cycle = csa->nl->root_search_cycle;
		}
#		endif
	}
	DBG_CHECK_TP_REG_LIST_SORTING(*reg_list);
	return tr_new;
}
