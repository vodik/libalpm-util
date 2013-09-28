
#line 1 "db/db_parser.rl"
#include "db_parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>


#line 34 "db/db_parser.rl"



#line 17 "db/db_parser.c"
static const int alpm_db_start = 5;
static const int alpm_db_first_final = 5;
static const int alpm_db_error = 0;

static const int alpm_db_en_main = 5;


#line 37 "db/db_parser.rl"

void db_parser_init(db_parser *pr, field_cb cb, void *data)
{
    *pr = (db_parser){
        .cb   = cb,
        .data = data
    };

    str_init(&pr->title, 1024);
    str_init(&pr->value, 1024);

    
#line 49 "db/db_parser.rl"
    
#line 40 "db/db_parser.c"
	{
	 pr->cs = alpm_db_start;
	}

#line 50 "db/db_parser.rl"
}

int db_parser_feed(db_parser *pr, const char *entry, size_t len)
{
    int rc = 0;
    const char *p = entry;
    const char *pe = entry + len;

    
#line 59 "db/db_parser.rl"
    
#line 57 "db/db_parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  pr->cs )
	{
tr1:
#line 25 "db/db_parser.rl"
	{
        pr->cb(pr->title.ptr, pr->value.ptr, pr->data);
        str_clear(&pr->value);
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 74 "db/db_parser.c"
	switch( (*p) ) {
		case 10: goto st5;
		case 37: goto tr6;
	}
	goto tr0;
tr0:
#line 19 "db/db_parser.rl"
	{
        rc = str_append(&pr->value, (*p));
        if (rc < 0)
            return rc;
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 92 "db/db_parser.c"
	if ( (*p) == 10 )
		goto tr1;
	goto tr0;
tr6:
#line 12 "db/db_parser.rl"
	{ str_clear(&pr->title); }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 104 "db/db_parser.c"
	if ( (*p) == 37 )
		goto st0;
	goto tr2;
tr2:
#line 13 "db/db_parser.rl"
	{
        rc = str_append(&pr->title, (*p));
        if (rc < 0)
            return rc;
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 120 "db/db_parser.c"
	if ( (*p) == 37 )
		goto st4;
	goto tr2;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 10 )
		goto st5;
	goto st0;
st0:
 pr->cs = 0;
	goto _out;
	}
	_test_eof5:  pr->cs = 5; goto _test_eof; 
	_test_eof1:  pr->cs = 1; goto _test_eof; 
	_test_eof2:  pr->cs = 2; goto _test_eof; 
	_test_eof3:  pr->cs = 3; goto _test_eof; 
	_test_eof4:  pr->cs = 4; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 60 "db/db_parser.rl"

    if (pr->cs == alpm_db_error) {
        return -EINVAL;
    }

    return rc;
}

void db_parser_free(db_parser *pr)
{
    str_free(&pr->title);
    str_free(&pr->value);
}
