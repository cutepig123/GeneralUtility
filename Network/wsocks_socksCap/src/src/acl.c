/*
 *  Copyright (C) 2006- xiaosuo <xiaosuo@gmail.com> 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "misc.h"
#include "list.h"
#include "acl.h"
#include "acl_cond.h"
#include "acl_act.h"
#include "acl_rule.h"

void acl_init(acl_t *acl)
{
	assert(acl != NULL);

	list_init(&acl->rule, NULL);
	list_init(&acl->cond_mod, NULL);
	list_init(&acl->act_mod, NULL);
	list_init(&acl->cond_path, NULL);
	list_init(&acl->act_path, NULL);
}

acl_t* acl_new(void)
{
	acl_t *acl;

	acl = malloc(sizeof(*acl));
	if(!acl)
		return NULL;
	acl_init(acl);

	return acl;
}

void acl_free(acl_t *acl)
{
	assert(acl);

	list_free(&acl->rule);
	list_free(&acl->cond_mod);
	list_free(&acl->act_mod);
	list_free(&acl->cond_path);
	list_free(&acl->act_path);

	free(acl);
}

/*
 * @brief parse a rule and add the rule to acl.
 * @return 0 if success.
 */
int acl_parse(acl_t *acl, const char *rule_str)
{
	acl_rule_t *rule;
	acl_cond_t *cond;
	acl_cond_module_t *cond_mod;
	acl_act_t *act;
	acl_act_module_t *act_mod;
	const char *ptr, *ptr2;
	int retval, err;

	assert(acl && rule_str);

	rule = acl_rule_new();
	if(!rule) return -1;
	
	ptr = rule_str;
	while(1){
		skip_blank(ptr);
		if(*ptr == '\0')
			break;
		if(strncmp(ptr, "cond", 4) == 0){
			ptr += 4;
			if(*ptr != ' ' && *ptr != '\t'){
				fprintf(stderr, "UNKNOWN sub command: %s\n", ptr - 4);
				goto bad;
			}
			skip_blank(ptr);
			if(*ptr == '\0'){
				fprintf(stderr, "LOST parameter for sub command: cond\n");
				goto bad;
			}
			ptr2 = ptr;
			skip_word(ptr2);
			if(!(cond_mod = request_acl_cond_module(acl, ptr, ptr2 - ptr))){
				fprintf(stderr, "cond(ition) module [%s] isn't found\n", ptr);
				goto bad;
			}
			ptr = ptr2;
			skip_blank(ptr);
			if(!(cond = acl_cond_new())){
				fprintf(stderr, "acl_cond_new() failed\n");
				goto bad;
			}
			cond->mod = cond_mod;
			cond->priv = cond_mod->parse(ptr, &retval, &err);
			if(err){
				fprintf(stderr, "cond_mod->parse() failed\n");
				acl_cond_free(cond);
				goto bad;
			}
			list_append(&rule->cond, cond, (void(*)(void*))acl_cond_free);
			ptr += retval;
		}else if(strncmp(ptr, "act", 3) == 0){
			ptr += 3;
			if(*ptr != ' ' && *ptr != '\t'){
				fprintf(stderr, "UNKNOWN sub command: %s\n", ptr - 3);
				goto bad;
			}
			skip_blank(ptr);
			if(*ptr == '\0'){
				fprintf(stderr, "LOST parameter for sub command: act\n");
				goto bad;
			}
			ptr2 = ptr;
			skip_word(ptr2);
			if(!(act_mod = request_acl_act_module(acl, ptr, ptr2 - ptr))){
				fprintf(stderr, "the act(ion) module [%s] isn't found\n", ptr);
				goto bad;
			}
			ptr = ptr2;
			skip_blank(ptr);
			if(!(act = acl_act_new())){
				fprintf(stderr, "acl_act_new() failed\n");
				goto bad;
			}
			act->mod = act_mod;
			act->priv = act_mod->parse(ptr, &retval, &err);
			if(err){
				fprintf(stderr, "act_mod->parse() failed\n");
				acl_act_free(act);
				goto bad;
			}
			list_append(&rule->act, act, (void(*)(void*))acl_act_free);
			ptr += retval;
		}else{
			fprintf(stderr, "UNKNOWN sub command: %s\n", ptr);
			goto bad;
		}
	}

	if(list_empty(&rule->cond) || list_empty(&rule->act)){
		fprintf(stderr, "this rule [%s] isn't completed\n", rule_str);
		goto bad;
	}

	list_append(&acl->rule, rule, (void(*)(void*))&acl_rule_free);

	return 0;

bad:
	acl_rule_free(rule);

	return -1;
}

int acl_load_from_FILE(acl_t *acl, FILE *fh)
{
	char buf[4096];
	char *ptr, *buf_ptr = buf;
	int line = 0, buf_left = sizeof(buf), slen;

	assert(acl && fh);

	while(fgets(buf_ptr, buf_left, fh)){
		line ++;
		slen = strlen(buf_ptr);
		if(buf_left == slen + 1
				&& buf_ptr[slen - 1] != '\n'){
			fprintf(stderr, "[%d] too long rule\n", line);
			return -1;
		}
		/* comment */
		ptr = buf_ptr;
		skip_blank(ptr);
		if(*ptr == '#')
			continue;
		if(buf_ptr == buf){
			ptr = buf;
			skip_blank(ptr);
			if(*ptr == '\n')
				continue; /* blank line between every rule */
			if(strncmp(ptr, "acl", 3) != 0){
				fprintf(stderr, "[%d] unknown command\n", line);
				return -1;
			}
			/* a new rule */
			buf[slen - 1] = ' ';
			buf_left -= slen;
			buf_ptr += slen;
			continue;
		}else{
			ptr = buf_ptr;
			skip_blank(ptr);
			if(*ptr != '\n'){ /* append the line to the rule */
				buf_ptr[slen - 1] = ' ';
				buf_left -= slen;
				buf_ptr += slen;
				continue;
			}else{
				*buf_ptr = '\0';
				buf_left = sizeof(buf);
				buf_ptr = buf;
			}
		}

		/* deal with the new rule */
		ptr = buf;
		skip_blank(ptr);
		ptr += 3; /* skip the acl command */
		if(*ptr != ' ' && *ptr != '\t'){
			fprintf(stderr, "[%d] NOT a acl command\n", line);
			return -1;
		}
		skip_blank(ptr);
		if(*ptr == '\0'){
			fprintf(stderr, "[%d] lost parameter\n", line);
			return -1;
		}
		if(acl_parse(acl, ptr) != 0){
			fprintf(stderr, "[%d] acl_parse() error\n", line);
			return -1;
		}
	}

	if(buf_ptr != buf){
		ptr = buf;
		skip_blank(ptr);
		ptr += 3;
		if(*ptr != ' ' && *ptr != '\t'){
			fprintf(stderr, "[%d] NOT a acl command\n", line);
			return -1;
		}
		skip_blank(ptr);
		if(*ptr == '\0'){
			fprintf(stderr, "[%d] lost parameter\n", line);
			return -1;
		}
		if(acl_parse(acl, ptr) != 0){
			fprintf(stderr, "[%d] acl_parse() error\n", line);
			return -1;
		}
	}

	return 0;
}

int acl_load_from_file(acl_t *acl, const char *file)
{
	FILE *fh;
	int retval;

	assert(acl && file);
	fh = fopen(file, "r");
	if(!fh) return -1;
	retval = acl_load_from_FILE(acl, fh);
	fclose(fh);

	return retval;
}

int acl_load_to_FILE(acl_t *acl, FILE *fh)
{
	struct list *i, *j, *k;
	acl_rule_t *ar;
	acl_cond_t *ac;
	acl_act_t *aa;
	char buf[4096];
	char *buf_ptr;
	int buf_left, retval;

	assert(acl && fh);

	list_for_each(i, &acl->rule){
		buf_ptr = buf;
		buf_left = sizeof(buf);
		ar = ACL_RULE_PTR(i->data);
		retval = snprintf(buf_ptr, buf_left, "acl\t");
		buf_left -= retval; buf_ptr += retval;
		list_for_each(j, &ar->cond){
			ac = ACL_COND_PTR(j->data);
			assert(ac->mod && ac->mod->snprint);
			retval = snprintf(buf_ptr, buf_left, "cond\t%s ",
					ac->mod->name);
			buf_left -= retval; buf_ptr += retval;
			retval = ac->mod->snprint(buf_ptr, buf_left, ac->priv);
			buf_left -= retval; buf_ptr += retval;
			retval = snprintf(buf_ptr, buf_left, "\n\t");
			buf_left -= retval; buf_ptr += retval;
		}
		list_for_each(k, &ar->act){
			aa = ACL_ACT_PTR(k->data);
			assert(aa->mod && aa->mod->snprint);
			retval = snprintf(buf_ptr, buf_left, "act\t%s ",
					aa->mod->name);
			buf_left -= retval; buf_ptr += retval;
			retval = aa->mod->snprint(buf_ptr, buf_left, aa->priv);
			buf_left -= retval; buf_ptr += retval;
			retval = snprintf(buf_ptr, buf_left, "\n\t");
			buf_left -= retval; buf_ptr += retval;
		}
		fprintf(fh, "%s\n", buf);
	}

	return 0;
}

int acl_load_to_file(acl_t *acl, const char *file)
{
	FILE *fh;

	assert(acl && file);
	fh = fopen(file, "w");
	if(!fh) return -1;
	acl_load_to_FILE(acl, fh);
	fclose(fh);

	return 0;
}

/* @brief travel the acl and find the right target to do.
 * @return 0 if success. 
 * @return 1 if match nothing
 * @return -1 if error */
int acl_do(acl_t *acl, void *cond_param, void *act_param)
{
	struct list *i, *j, *k;
	acl_rule_t *ar;
	acl_cond_t *ac;
	acl_act_t *aa;

	assert(acl);

	list_for_each(i, &acl->rule){
		ar = ACL_RULE_PTR(i->data);
		/* match all the conditions in the rule */
		list_for_each(j, &ar->cond){
			ac = ACL_COND_PTR(j->data);
			assert(ac->mod);
			/* if the match is NULL, success */
			if(ac->mod->match
					&& ac->mod->match(cond_param, ac->priv) != 0)
				goto next;
		}
		/* the rule is matched */
		list_for_each(k, &ar->act){
			aa = ACL_ACT_PTR(k->data);
			assert(aa->mod);
			/* if the act is NULL, success */
			if(aa->mod->target
					&& aa->mod->target(act_param, aa->priv) != 0)
				goto fail;
		}

		return 0;
next:
		continue;
	}

	return 1;

fail:
	return -1;
}
