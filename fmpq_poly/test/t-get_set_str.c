/*
    Copyright (C) 2010 Sebastian Pancratz
    Copyright (C) 2009 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpq_poly.h"
#include "ulong_extras.h"

int
main(void)
{
    int i, result;
    ulong cflags = UWORD(0);

    FLINT_TEST_INIT(state);

    flint_printf("get_set_str....");
    fflush(stdout);

    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        int ans;
        char * str;
        fmpq_poly_t f, g;

        fmpq_poly_init(f);
        fmpq_poly_init(g);
        fmpq_poly_randtest(f, state, n_randint(state, 100), 200);
        
        str = fmpq_poly_get_str(f);
        ans = fmpq_poly_set_str(g, str);

        cflags |= fmpq_poly_is_canonical(f) ? 0 : 1;
        result = (ans == 0 && fmpq_poly_equal(f, g) && !cflags);
        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("f      = "), fmpq_poly_debug(f), flint_printf("\n\n");
            flint_printf("g      = "), fmpq_poly_debug(g), flint_printf("\n\n");
            flint_printf("ans    = %d\n\n", ans);
            flint_printf("cflags = %wu\n\n", cflags);
            abort();
        }

        fmpq_poly_clear(f);
        fmpq_poly_clear(g);
        flint_free(str);
    }
    
    FLINT_TEST_CLEANUP(state);
    
    flint_printf("PASS\n");
    return 0;
}
