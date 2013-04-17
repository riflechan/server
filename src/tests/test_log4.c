/* -*- mode: C; c-basic-offset: 4 -*- */
#ident "Copyright (c) 2007 Tokutek Inc.  All rights reserved."

#include "test.h"

/* Test to see if we can do logging and recovery. */
/* This is very specific to TokuDB.  It won't work with Berkeley DB. */

#include <assert.h>
#include <toku_portability.h>
#include <db.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// ENVDIR is defined in the Makefile

static void make_db (void) {
    DB_ENV *env;
    DB *db;
    DB_TXN *tid;
    int r;
    int i;

    system("rm -rf " ENVDIR);
    r=toku_os_mkdir(ENVDIR, S_IRWXU+S_IRWXG+S_IRWXO);       assert(r==0);
    r=db_env_create(&env, 0); assert(r==0);
    r=env->set_lk_max_locks(env, 30000); CKERR(r);
    r=env->open(env, ENVDIR, DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN|DB_CREATE|DB_PRIVATE, S_IRWXU+S_IRWXG+S_IRWXO); CKERR(r);
    r=db_create(&db, env, 0); CKERR(r);
    r=env->txn_begin(env, 0, &tid, 0); assert(r==0);
    r=db->open(db, tid, "foo.db", 0, DB_BTREE, DB_CREATE, S_IRWXU+S_IRWXG+S_IRWXO); CKERR(r);
    r=tid->commit(tid, 0);    assert(r==0);
    r=env->txn_begin(env, 0, &tid, 0); assert(r==0);
    
    for (i=0; i<20000; i++) {
	char hello[30], there[30];
	DBT key,data;
	snprintf(hello, sizeof(hello), "hello%ld.%d", random(), i);
	snprintf(there, sizeof(hello), "there%d", i);
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data  = hello; key.size=strlen(hello)+1;
	data.data = there; data.size=strlen(there)+1;
	r=db->put(db, tid, &key, &data, 0);  CKERR(r);
    }
    r=tid->commit(tid, 0);    assert(r==0);
    r=db->close(db, 0);       assert(r==0);
    r=env->close(env, 0);     assert(r==0);
}

int
test_main (int argc __attribute__((__unused__)), const char *argv[] __attribute__((__unused__))) {
    make_db();
    return 0;
}
