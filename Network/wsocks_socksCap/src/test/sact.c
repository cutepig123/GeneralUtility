
#include <stdlib.h>
#include <stdio.h>

#include "acl_act.h"

static void* sact_parse(const char *str, int *eat, int *error)
{
	if(eat) *eat = 0;
	if(error) *error = 0;

	return NULL;
}

static int sact_snprint(char *str, size_t size, void *priv)
{
	return snprintf(str, size, "");
}

static int sact_target(void *param, void *priv)
{
	printf("sact target.\n");
	return 0;
}

static void sact_free(void *priv)
{
}

static acl_act_module_t ac_sact = {
	.name		= "sact",
	.parse		= ACL_PARSE_PTR(&sact_parse),
	.snprint	= ACL_SNPRINT_PTR(&sact_snprint),
	.target		= ACL_TARGET_PTR(&sact_target),
	.free		= ACL_FREE_PTR(&sact_free),
};

ACL_ACT_MODULE_DCLR(ac_sact);
