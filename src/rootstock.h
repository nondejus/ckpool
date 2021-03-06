/*
 * Copyright 2016
 */

#ifndef ROOTSTOCK_H
#define ROOTSTOCK_H

#include "config.h"

struct rootstock_data {
	ckpool_t *ckp;

	/* Current server instance */
	server_instance_t *si;

	/* Id for last request */
	int lastreqid;

	/* Last getwork parameters */
	char blockhashmergebin[32];
	char target[65];
	double minerfees;
	int notify;
	char blockhashmerge[65];
	char parentblockhash[65];

	/* Last hash being worked on */
	char lastblockhashmerge[65];
	char lastparentblockhash[65];
};

typedef struct rootstock_data rdata_t;

struct rsk_getwork {
	char blockhashmergebin[32];
	char target[65];
	double minerfees;
	int notify;

	char blockhashmerge[65];
	char parentblockhash[65];
};

typedef struct rsk_getwork rsk_getwork_t;

void *rootstock(void *arg);

#endif /* ROOTSTOCK_H */
