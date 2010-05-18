/*        $NetBSD: dm_target_error.c,v 1.11 2010/05/18 15:10:38 haad Exp $      */

/*
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Adam Hamsik.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file implements initial version of device-mapper error target.
 */
#include <sys/types.h>
#include <sys/param.h>

#include <sys/buf.h>

#include "dm.h"

/* dm_target_error.c */
int dm_target_error_init(dm_dev_t *, void**, char *);
char * dm_target_error_status(void *);
int dm_target_error_strategy(dm_table_entry_t *, struct buf *);
int dm_target_error_sync(dm_table_entry_t *);
int dm_target_error_deps(dm_table_entry_t *, prop_array_t);
int dm_target_error_destroy(dm_table_entry_t *);
int dm_target_error_upcall(dm_table_entry_t *, struct buf *);

#ifdef DM_TARGET_MODULE
/*
 * Every target can be compiled directly to dm driver or as a
 * separate module this part of target is used for loading targets
 * to dm driver.
 * Target can be unloaded from kernel only if there are no users of
 * it e.g. there are no devices which uses that target.
 */
#include <sys/kernel.h>
#include <sys/module.h>

MODULE(MODULE_CLASS_MISC, dm_target_error, "dm");

static int
dm_target_error_modcmd(modcmd_t cmd, void *arg)
{
	dm_target_t *dmt;
	int r;
	dmt = NULL;

	switch (cmd) {
	case MODULE_CMD_INIT:
		if ((dmt = dm_target_lookup("error")) != NULL) {
			dm_target_unbusy(dmt);
			return EEXIST;
		}
		dmt = dm_target_alloc("error");

		dmt->version[0] = 1;
		dmt->version[1] = 0;
		dmt->version[2] = 0;
		strlcpy(dmt->name, "error", DM_MAX_TYPE_NAME);
		dmt->init = &dm_target_error_init;
		dmt->status = &dm_target_error_status;
		dmt->strategy = &dm_target_error_strategy;
		dmt->sync = &dm_target_error_sync;
		dmt->deps = &dm_target_error_deps;
		dmt->destroy = &dm_target_error_destroy;
		dmt->upcall = &dm_target_error_upcall;

		r = dm_target_insert(dmt);

		break;

	case MODULE_CMD_FINI:
		r = dm_target_rem("error");
		break;

	case MODULE_CMD_STAT:
		return ENOTTY;

	default:
		return ENOTTY;
	}

	return r;
}
#endif

/* Init function called from dm_table_load_ioctl. */
int
dm_target_error_init(dm_dev_t * dmv, void **target_config, char *argv)
{

	printf("Error target init function called!!\n");

	*target_config = NULL;

	dmv->dev_type = DM_ERROR_DEV;

	return 0;
}
/* Status routine called to get params string. */
char *
dm_target_error_status(void *target_config)
{
	return NULL;
}
/* Strategy routine called from dm_strategy. */
int
dm_target_error_strategy(dm_table_entry_t * table_en, struct buf * bp)
{

	printf("Error target read function called!!\n");

	bp->b_error = EIO;
	bp->b_resid = 0;

	biodone(bp);

	return 0;
}
/* Sync underlying disk caches. */
int
dm_target_error_sync(dm_table_entry_t * table_en)
{

	return 0;
}
/* Doesn't do anything here. */
int
dm_target_error_destroy(dm_table_entry_t * table_en)
{
	table_en->target_config = NULL;

	/* Unbusy target so we can unload it */
	dm_target_unbusy(table_en->target);

	return 0;
}
/* Doesn't not need to do anything here. */
int
dm_target_error_deps(dm_table_entry_t * table_en, prop_array_t prop_array)
{
	return 0;
}
/* Unsupported for this target. */
int
dm_target_error_upcall(dm_table_entry_t * table_en, struct buf * bp)
{
	return 0;
}
