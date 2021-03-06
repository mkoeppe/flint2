/*
    Copyright (C) 2017 Daniel Schultz

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
#include "fmpz_poly.h"
#include "fmpz_mpoly.h"
#include "ulong_extras.h"

int
main(void)
{
    int i, j;

    FLINT_TEST_INIT(state);

    flint_printf("resultant_discriminant....");
    fflush(stdout);


    /* Check univariate resultant */
    for (i = 0; i < 50 * flint_test_multiplier(); i++)
    {
        fmpz_mpoly_ctx_t ctx;
        fmpz_mpoly_t a, b, r;
        fmpz_poly_t au, bu;
        fmpz_t ru;
        ordering_t ord;
        slong nvars, len1, len2, exp_bound1, exp_bound2;
        slong coeff_bits;
        slong ashift, bshift;

        ord = mpoly_ordering_randtest(state);
        nvars = 1;

        fmpz_mpoly_ctx_init(ctx, nvars, ord);

        fmpz_mpoly_init(a, ctx);
        fmpz_mpoly_init(b, ctx);
        fmpz_mpoly_init(r, ctx);
        fmpz_poly_init(au);
        fmpz_poly_init(bu);
        fmpz_init(ru);

        len1 = n_randint(state, 100);
        len2 = n_randint(state, 100);
        exp_bound1 = n_randint(state, 50) + 1;
        exp_bound2 = n_randint(state, 50) + 1;
        coeff_bits = n_randint(state, 4);
        fmpz_mpoly_randtest(a, state, len1, exp_bound1, coeff_bits, ctx);
        fmpz_mpoly_randtest(b, state, len2, exp_bound2, coeff_bits, ctx);

        fmpz_mpoly_to_fmpz_poly(au, &ashift, a, 0, ctx);
        fmpz_mpoly_to_fmpz_poly(bu, &bshift, b, 0, ctx);
        fmpz_poly_shift_left(au, au, ashift);
        fmpz_poly_shift_left(bu, bu, bshift);

        fmpz_poly_resultant(ru, au, bu);
        fmpz_mpoly_resultant(r, a, b, 0, ctx);
        if (!fmpz_mpoly_equal_fmpz(r, ru, ctx))
        {
            printf("FAIL\n");
            flint_printf(" Check univariate resultant \ni: %wd\n",i);
            flint_abort();
        }

        fmpz_mpoly_clear(a, ctx);
        fmpz_mpoly_clear(b, ctx);
        fmpz_mpoly_clear(r, ctx);
        fmpz_poly_clear(au);       
        fmpz_poly_clear(bu);
        fmpz_clear(ru);
    }


    /* Check res(a*b,c) = res(a,c)*res(b,c) */
    for (i = 0; i < 30 * flint_test_multiplier(); i++)
    {
        fmpz_mpoly_ctx_t ctx;
        fmpz_mpoly_t a, b, c, ab, ra, rb, rab, p;
        ordering_t ord;
        slong nvars, len1, len2, len3, exp_bound1, exp_bound2, exp_bound3;
        slong coeff_bits;

        ord = mpoly_ordering_randtest(state);
        nvars = n_randint(state, 3) + 1;

        fmpz_mpoly_ctx_init(ctx, nvars, ord);

        fmpz_mpoly_init(a, ctx);
        fmpz_mpoly_init(b, ctx);
        fmpz_mpoly_init(c, ctx);       
        fmpz_mpoly_init(ab, ctx);       
        fmpz_mpoly_init(ra, ctx);
        fmpz_mpoly_init(rb, ctx);
        fmpz_mpoly_init(rab, ctx);
        fmpz_mpoly_init(p, ctx);

        len1 = n_randint(state, 15);
        len2 = n_randint(state, 15);
        len3 = n_randint(state, 15);
        exp_bound1 = n_randint(state, 5) + 1;
        exp_bound2 = n_randint(state, 5) + 1;
        exp_bound3 = n_randint(state, 5) + 1;
        coeff_bits = n_randint(state, 3);
        fmpz_mpoly_randtest(a, state, len1, exp_bound1, coeff_bits, ctx);
        fmpz_mpoly_randtest(b, state, len2, exp_bound2, coeff_bits, ctx);
        fmpz_mpoly_randtest(c, state, len3, exp_bound3, coeff_bits, ctx);

        for (j = 0; j < nvars; j++)
        {
            fmpz_mpoly_mul_johnson(ab, a, b, ctx);
            fmpz_mpoly_resultant(ra, a, c, j, ctx);
            fmpz_mpoly_resultant(rb, b, c, j, ctx);
            fmpz_mpoly_resultant(rab, ab, c, j, ctx);
            fmpz_mpoly_mul_johnson(p, ra, rb, ctx);

            if (!fmpz_mpoly_equal(p,rab,ctx))
            {
                printf("FAIL\n");
                flint_printf(" Check res(a*b,c) = res(a,c)*res(b,c) \n"
                                                       "i: %wd  j: %wd\n",i,j);
                flint_abort();
            }
        }

        fmpz_mpoly_clear(a, ctx);
        fmpz_mpoly_clear(b, ctx);
        fmpz_mpoly_clear(c, ctx);       
        fmpz_mpoly_clear(ab, ctx);       
        fmpz_mpoly_clear(ra, ctx);
        fmpz_mpoly_clear(rb, ctx);
        fmpz_mpoly_clear(rab, ctx);
        fmpz_mpoly_clear(p, ctx);
    }


    /* Check disc(a*b) = disc(a)*disc(b)*res(a,b)^2 */
    for (i = 0; i < 30 * flint_test_multiplier(); i++)
    {
        fmpz_mpoly_ctx_t ctx;
        fmpz_mpoly_t a, b, ab, r, da, db, dab, p;
        ordering_t ord;
        slong nvars, len1, len2, exp_bound1, exp_bound2;
        slong coeff_bits;

        ord = mpoly_ordering_randtest(state);
        nvars = n_randint(state, 3) + 1;

        fmpz_mpoly_ctx_init(ctx, nvars, ord);

        fmpz_mpoly_init(a, ctx);
        fmpz_mpoly_init(b, ctx);
        fmpz_mpoly_init(ab, ctx);
        fmpz_mpoly_init(da, ctx);
        fmpz_mpoly_init(db, ctx);
        fmpz_mpoly_init(dab, ctx);
        fmpz_mpoly_init(r, ctx);
        fmpz_mpoly_init(p, ctx);

        len1 = n_randint(state, 15);
        len2 = n_randint(state, 15);
        exp_bound1 = n_randint(state, 5) + 1;
        exp_bound2 = n_randint(state, 5) + 1;
        coeff_bits = n_randint(state, 3);
        fmpz_mpoly_randtest(a, state, len1, exp_bound1, coeff_bits, ctx);
        fmpz_mpoly_randtest(b, state, len2, exp_bound2, coeff_bits, ctx);

        for (j = 0; j < nvars; j++)
        {
            if (fmpz_mpoly_degree_si(a, j, ctx) < 1)
                continue;
            if (fmpz_mpoly_degree_si(b, j, ctx) < 1)
                continue;

            fmpz_mpoly_mul_johnson(ab, a, b, ctx);
            fmpz_mpoly_resultant(r, a, b, j, ctx);
            fmpz_mpoly_discriminant(da, a, j, ctx);
            fmpz_mpoly_discriminant(db, b, j, ctx);
            fmpz_mpoly_discriminant(dab, ab, j, ctx);
            fmpz_mpoly_mul_johnson(p, da, db, ctx);
            fmpz_mpoly_mul_johnson(p, p, r, ctx);
            fmpz_mpoly_mul_johnson(p, p, r, ctx);

            if (!fmpz_mpoly_equal(dab, p, ctx))
            {
                printf("FAIL\n");
                flint_printf(" Check disc(a*b) = disc(a)*disc(b)*res(a,b)^2 \n"
                                                       "i: %wd  j: %wd\n",i,j);
                flint_abort();
            }
        }

        fmpz_mpoly_clear(a, ctx);
        fmpz_mpoly_clear(b, ctx);
        fmpz_mpoly_clear(ab, ctx);       
        fmpz_mpoly_clear(da, ctx);
        fmpz_mpoly_clear(db, ctx);
        fmpz_mpoly_clear(dab, ctx);
        fmpz_mpoly_clear(r, ctx);
        fmpz_mpoly_clear(p, ctx);
    }



    FLINT_TEST_CLEANUP(state);

    printf("PASS\n");
    return 0;
}

