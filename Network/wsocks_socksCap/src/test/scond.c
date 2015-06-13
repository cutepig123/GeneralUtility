
#include <stdlib.h>
#include <stdio.h>

#include "acl_cond.h"

static void* scond_parse(const char *str, int *eat, int *error)
{
	if(eat) *eat = 0;
	if(error) *error = 0;

	return NULL;
}

static int scond_snprint(char *str, size_t size, void *priv)
{
	return snprintf(str, size, "");
}

static int scond_match(void *param, void *priv)
{
	printf("scond matched\n");
	return 0;
}

static void scond_free(void *priv)
{
}

static acl_cond_module_t ac_scond = {
	.name		= "scond",
	.parse		= ACL_PARSE_PTR(&scond_parse),
	.snprint	= ACL_SNPRINT_PTR(&scond_snprint),
	.match		= ACL_MATCH_PTR(&scond_match),
	.free		= ACL_FREE_PTR(&scond_free),
};

ACL_COND_MODULE_DCLR(ac_scond);
