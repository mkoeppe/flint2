/*
    Copyright (C) 2017 William Hart

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
#include "fmpz_mpoly.h"
#include "ulong_extras.h"

int
main(void)
{
    int i, j, result;
    FLINT_TEST_INIT(state);

    flint_printf("mul_johnson....");
    fflush(stdout);

    /* Check mul_johnson matches mul_array */
    for (i = 0; i < 10 * flint_test_multiplier(); i++)
    {
       fmpz_mpoly_ctx_t ctx;
       fmpz_mpoly_t f, g, h, k;
       ordering_t ord;
       slong nvars, len, len1, len2, exp_bound, exp_bound1, exp_bound2;
       slong coeff_bits, exp_bits, exp_bits1, exp_bits2;
       int ok1;

       ord = mpoly_ordering_randtest(state);
       nvars = n_randint(state, 10) + 1;

       fmpz_mpoly_ctx_init(ctx, nvars, ord);

       fmpz_mpoly_init(f, ctx);
       fmpz_mpoly_init(g, ctx);
       fmpz_mpoly_init(h, ctx);
       fmpz_mpoly_init(k, ctx);

       len = n_randint(state, 100);
       len1 = n_randint(state, 100);
       len2 = n_randint(state, 100);

       exp_bits = n_randint(state, 20/(nvars + 
                            mpoly_ordering_isdeg(ctx->minfo) + (nvars == 1)) + 1) + 1;
       exp_bits1 = n_randint(state, 20/(nvars + 
                            mpoly_ordering_isdeg(ctx->minfo) + (nvars == 1)) + 1) + 1;
       exp_bits2 = n_randint(state, 20/(nvars + 
                            mpoly_ordering_isdeg(ctx->minfo) + (nvars == 1)) + 1) + 1;
       exp_bound = n_randbits(state, exp_bits);
       exp_bound1 = n_randbits(state, exp_bits1);
       exp_bound2 = n_randbits(state, exp_bits2);

       coeff_bits = n_randint(state, 200);

       for (j = 0; j < 4; j++)
       {
          fmpz_mpoly_randtest(f, state, len1, exp_bound1, coeff_bits, ctx);
          fmpz_mpoly_randtest(g, state, len2, exp_bound2, coeff_bits, ctx);
          fmpz_mpoly_randtest(h, state, len, exp_bound, coeff_bits, ctx);
          fmpz_mpoly_randtest(k, state, len, exp_bound, coeff_bits, ctx);

          ok1 = fmpz_mpoly_mul_array(h, f, g, ctx);
          fmpz_mpoly_test(h, ctx);

          fmpz_mpoly_mul_johnson(k, f, g, ctx);
          fmpz_mpoly_test(k, ctx);

          result = ok1 == 0 || fmpz_mpoly_equal(h, k, ctx);

          if (!result)
          {
             printf("FAIL\n");

             printf("ord = "); mpoly_ordering_print(ord);
             printf(", len = %ld, exp_bits = %ld, exp_bound = %lx, "
                    "len1 = %ld, exp_bits1 = %ld, exp_bound1 = %lx, "
                    "len2 = %ld, exp_bits2 = %ld, exp_bound2 = %lx, "
                                      "coeff_bits = %ld, nvars = %ld\n\n",
                       len, exp_bits, exp_bound, len1, exp_bits1, exp_bound1,
                               len2, exp_bits2, exp_bound2, coeff_bits, nvars);

             fmpz_mpoly_print_pretty(f, NULL, ctx); printf("\n\n");
             fmpz_mpoly_print_pretty(g, NULL, ctx); printf("\n\n");
             fmpz_mpoly_print_pretty(h, NULL, ctx); printf("\n\n");
             fmpz_mpoly_print_pretty(k, NULL, ctx); printf("\n\n");
          
             flint_abort();
          }
       }

       fmpz_mpoly_clear(f, ctx);  
       fmpz_mpoly_clear(g, ctx);  
       fmpz_mpoly_clear(h, ctx);  
       fmpz_mpoly_clear(k, ctx);  
    }

    /* Check f*(g + h) = f*g + f*h */
    for (i = 0; i < 20 * flint_test_multiplier(); i++)
    {
        fmpz_mpoly_ctx_t ctx;
        fmpz_mpoly_t f, g, h, k1, k2, t1, t2;
        slong len, len1, len2;
        mp_bitcnt_t coeff_bits, exp_bits, exp_bits1, exp_bits2;

        fmpz_mpoly_ctx_init_rand(ctx, state, 20);

        fmpz_mpoly_init(f, ctx);
        fmpz_mpoly_init(g, ctx);
        fmpz_mpoly_init(h, ctx);
        fmpz_mpoly_init(k1, ctx);
        fmpz_mpoly_init(k2, ctx);
        fmpz_mpoly_init(t1, ctx);
        fmpz_mpoly_init(t2, ctx);

        len = n_randint(state, 100);
        len1 = n_randint(state, 100);
        len2 = n_randint(state, 100);

        exp_bits = n_randint(state, 200) + 2;
        exp_bits1 = n_randint(state, 200) + 2;
        exp_bits2 = n_randint(state, 200) + 2;

        coeff_bits = n_randint(state, 200);

        fmpz_mpoly_randbits(k1, state, len, coeff_bits, exp_bits, ctx);
        fmpz_mpoly_randbits(k2, state, len, coeff_bits, exp_bits, ctx);

        for (j = 0; j < 4; j++)
        {
            fmpz_mpoly_randbits(f, state, len1, coeff_bits, exp_bits1, ctx);
            fmpz_mpoly_randbits(g, state, len2, coeff_bits, exp_bits2, ctx);
            fmpz_mpoly_randbits(h, state, len2, coeff_bits, exp_bits2, ctx);

            fmpz_mpoly_add(t1, g, h, ctx);
            fmpz_mpoly_test(t1, ctx);
            fmpz_mpoly_mul_johnson(k1, f, t1, ctx);
            fmpz_mpoly_test(k1, ctx);
            fmpz_mpoly_mul_johnson(t1, f, g, ctx);
            fmpz_mpoly_test(t1, ctx);
            fmpz_mpoly_mul_johnson(t2, f, h, ctx);
            fmpz_mpoly_test(t2, ctx);
            fmpz_mpoly_add(k2, t1, t2, ctx);
            fmpz_mpoly_test(k2, ctx);
            result = fmpz_mpoly_equal(k1, k2, ctx);

            if (!result)
            {
                printf("FAIL\n");
                flint_printf("Check f*(g + h) = f*g + f*h\ni = %wd, j = %wd\n", i ,j);
                flint_abort();
            }
        }

        fmpz_mpoly_clear(f, ctx);
        fmpz_mpoly_clear(g, ctx);
        fmpz_mpoly_clear(h, ctx);
        fmpz_mpoly_clear(k1, ctx);
        fmpz_mpoly_clear(k2, ctx);
        fmpz_mpoly_clear(t1, ctx);
        fmpz_mpoly_clear(t2, ctx);
        fmpz_mpoly_ctx_clear(ctx);
    }

    /* Check aliasing first argument */
    for (i = 0; i < 10 * flint_test_multiplier(); i++)
    {
        fmpz_mpoly_ctx_t ctx;
        fmpz_mpoly_t f, g, h;
        slong len, len1, len2;
        mp_bitcnt_t coeff_bits, exp_bits, exp_bits1, exp_bits2;

        fmpz_mpoly_ctx_init_rand(ctx, state, 20);

        fmpz_mpoly_init(f, ctx);
        fmpz_mpoly_init(g, ctx);
        fmpz_mpoly_init(h, ctx);

        len = n_randint(state, 100);
        len1 = n_randint(state, 100);
        len2 = n_randint(state, 100);

        exp_bits = n_randint(state, 200) + 2;
        exp_bits1 = n_randint(state, 200) + 2;
        exp_bits2 = n_randint(state, 200) + 2;

        coeff_bits = n_randint(state, 200);

        for (j = 0; j < 4; j++)
        {
            fmpz_mpoly_randbits(f, state, len1, coeff_bits, exp_bits1, ctx);
            fmpz_mpoly_randbits(g, state, len2, coeff_bits, exp_bits2, ctx);
            fmpz_mpoly_randbits(h, state, len, coeff_bits, exp_bits, ctx);

            fmpz_mpoly_mul_johnson(h, f, g, ctx);
            fmpz_mpoly_test(h, ctx);
            fmpz_mpoly_mul_johnson(f, f, g, ctx);
            fmpz_mpoly_test(f, ctx);
            result = fmpz_mpoly_equal(h, f, ctx);

            if (!result)
            {
                printf("FAIL\n");
                flint_printf("Check aliasing second arg\ni = %wd, j = %wd\n", i ,j);
                flint_abort();
            }
        }

        fmpz_mpoly_clear(f, ctx);
        fmpz_mpoly_clear(g, ctx);
        fmpz_mpoly_clear(h, ctx);
        fmpz_mpoly_ctx_clear(ctx);
    }

    /* Check aliasing second argument */
    for (i = 0; i < 10 * flint_test_multiplier(); i++)
    {
        fmpz_mpoly_ctx_t ctx;
        fmpz_mpoly_t f, g, h;
        slong len, len1, len2;
        mp_bitcnt_t coeff_bits, exp_bits, exp_bits1, exp_bits2;

        fmpz_mpoly_ctx_init_rand(ctx, state, 20);

        fmpz_mpoly_init(f, ctx);
        fmpz_mpoly_init(g, ctx);
        fmpz_mpoly_init(h, ctx);

        len = n_randint(state, 100);
        len1 = n_randint(state, 100);
        len2 = n_randint(state, 100);

        exp_bits = n_randint(state, 200) + 2;
        exp_bits1 = n_randint(state, 200) + 2;
        exp_bits2 = n_randint(state, 200) + 2;

        coeff_bits = n_randint(state, 200);

        for (j = 0; j < 4; j++)
        {
            fmpz_mpoly_randbits(f, state, len1, coeff_bits, exp_bits1, ctx);
            fmpz_mpoly_randbits(g, state, len2, coeff_bits, exp_bits2, ctx);
            fmpz_mpoly_randbits(h, state, len, coeff_bits, exp_bits, ctx);

            fmpz_mpoly_mul_johnson(h, f, g, ctx);
            fmpz_mpoly_test(h, ctx);
            fmpz_mpoly_mul_johnson(g, f, g, ctx);
            fmpz_mpoly_test(g, ctx);
            result = fmpz_mpoly_equal(h, g, ctx);

            if (!result)
            {
                printf("FAIL\n");
                flint_printf("Check aliasing second arg\ni = %wd, j = %wd\n", i ,j);
                flint_abort();
            }
        }

        fmpz_mpoly_clear(f, ctx);
        fmpz_mpoly_clear(g, ctx);
        fmpz_mpoly_clear(h, ctx);
        fmpz_mpoly_ctx_clear(ctx);
    }

    FLINT_TEST_CLEANUP(state);
    
    flint_printf("PASS\n");
    return 0;
}

