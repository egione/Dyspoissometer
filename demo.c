/*
Dyspoissometer
Copyright 2016 Russell Leidich
http://dyspoissonism.blogspot.com

This collection of files constitutes the Dyspoissometer Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Dyspoissometer Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Dyspoissometer Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Dyspoissometer Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
Demo
*/
#include "flag.h"
#include "flag_dyspoissometer.h"
#include <math.h>
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include <quadmath.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include "debug_quad.h"
  #include "debug_quad_xtrn.h"
#endif
#include "dyspoissometer.h"
#include "dyspoissometer_xtrn.h"

#define DEMO_MASK_IDX_MAX0 29
#define DEMO_MASK_IDX_MAX1 89
#define DEMO_MASK_MAX 8
#define DEMO_POISSON_IDX_COUNT 20
#define DEMO_POP_LIST1_UINT_COUNT 12
#define DEMO_POP_LIST2_UINT_COUNT 5
#define DEMO_POP_LIST3_UINT_COUNT 13

DYSPOISSOMETER_UINT freq_list1_base[DEMO_MASK_MAX+1]={8, 11, 12, 9, 10, 10, 9, 11, 10};
DYSPOISSOMETER_UINT mask_list0_base[DEMO_MASK_IDX_MAX0+1]={4, 1, 6, 7, 6, 8, 0, 4, 7, 4, 6, 1, 0, 4, 6, 3, 0, 1, 2, 8, 8, 5, 3, 1, 7, 2, 1, 4, 7, 3};
DYSPOISSOMETER_UINT pop_list1_base[DEMO_POP_LIST1_UINT_COUNT]={0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 2, 1};
DYSPOISSOMETER_UINT pop_list2_base[DEMO_POP_LIST2_UINT_COUNT]={1, 2, 3, 2, 1};
DYSPOISSOMETER_UINT pop_list3_base[DEMO_POP_LIST3_UINT_COUNT]={0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 2, 1, 1};

void
demo_out_of_memory_print(void){
  DEBUG_PRINT("\nERROR: Out of memory!\n");
  return;
}

int
main(void){
  u32 bits_in_mask_set;
  DYSPOISSOMETER_UINT mask_count_implied;
  DYSPOISSOMETER_UINT mask_idx_max;
  DYSPOISSOMETER_NUMBER d0;
  DYSPOISSOMETER_NUMBER dyspoissonism;
  DYSPOISSOMETER_UINT *freq_list0_base;
  DYSPOISSOMETER_UINT *freq_list4_base;
  DYSPOISSOMETER_UINT freq_max;
  DYSPOISSOMETER_UINT freq_max_minus_1;
  DYSPOISSOMETER_UINT h0;
  DYSPOISSOMETER_NUMBER information_density;
  DYSPOISSOMETER_NUMBER information_sparsity;
  DYSPOISSOMETER_NUMBER logfreedom;
  DYSPOISSOMETER_NUMBER logfreedom_max;
  DYSPOISSOMETER_NUMBER logfreedom_median;
  DYSPOISSOMETER_NUMBER logfreedom_upper_bound;
  DYSPOISSOMETER_UINT *mask_list_base;
  DYSPOISSOMETER_UINT mask_max;
  u8 neighbor_status;
  DYSPOISSOMETER_UINT poisson_mask_idx_max;
  DYSPOISSOMETER_UINT poisson_mask_count;
  DYSPOISSOMETER_UINT poisson_freq;
  DYSPOISSOMETER_NUMBER poisson_information_sparsity;
  DYSPOISSOMETER_UINT poisson_list_base[DEMO_POISSON_IDX_COUNT];
  DYSPOISSOMETER_NUMBER poisson_logfreedom;
  DYSPOISSOMETER_UINT poisson_mask_span;
  DYSPOISSOMETER_UINT poisson_mask_max;
  DYSPOISSOMETER_UINT poisson_pop;
  DYSPOISSOMETER_UINT pop_down_left;
  DYSPOISSOMETER_UINT pop_down_left_old;
  DYSPOISSOMETER_UINT pop_down_right;
  DYSPOISSOMETER_UINT pop_down_right_old;
  DYSPOISSOMETER_UINT pop_idx;
  DYSPOISSOMETER_UINT pop_idx_delta;
  DYSPOISSOMETER_UINT pop_idx_down;
  DYSPOISSOMETER_UINT pop_idx_up;
  DYSPOISSOMETER_UINT *pop_list0_base;
  DYSPOISSOMETER_UINT *pop_list4_base;
  DYSPOISSOMETER_UINT pop_up_left;
  DYSPOISSOMETER_UINT pop_up_left_old;
  DYSPOISSOMETER_UINT pop_up_right;
  DYSPOISSOMETER_UINT pop_up_right_old;
  u64 random_seed;
  u8 status;
  DYSPOISSOMETER_NUMBER way_count;

  status=dyspoissometer_init(0, 0);
  freq_list0_base=NULL;
  pop_list0_base=NULL;
  pop_list4_base=NULL;
  mask_idx_max=DEMO_MASK_IDX_MAX0;
  mask_max=DEMO_MASK_MAX;
  random_seed=1;
  do{
    if(status){
      DEBUG_PRINT("\nERROR: Outdated source code!\n");
      break;
    }
    status=1;
    DEBUG_PRINT("Dyspoissometer Demo\nCopyright 2016 Russell Leidich\nhttp://dyspoissonism.blogspot.com\n");
    DEBUG_U32("build_id_in_hex", DYSPOISSOMETER_BUILD_ID);
    DEBUG_PRINT("\nFollow along with main() in demo.c, so you can learn how to use Dyspoissometer.\nThe math details are explained at the webpage linked above.\n\n");
    DEBUG_PRINT("First of all, we have already called dyspoissometer_init() to ensure that (1)\nDyspoissometer is compatible with the way we intend to use it and (2) all\ncritical fixes which this demo knows about are present.\n\n");
    DEBUG_PRINT("So let's get on with the demo. Given that:\n\n");
    DEBUG_U64("mask_idx_max", mask_idx_max);
    DEBUG_U64("mask_max", mask_max);
    DEBUG_PRINT("\nWe then have the following list (mask_list0_base) of (mask_idx_max+1) masks,\nall on the interval [0, mask_max]. It looks random -- but how random?\n\n");
    DEBUG_LIST("mask_list0_base", DEMO_MASK_IDX_MAX0+1, (u8 *)(mask_list0_base), DYSPOISSOMETER_UINT_SIZE_LOG2);
    DEBUG_PRINT("\nIn order to answer this question, we first need to count how many of each mask\nthere are in a process called \"frequency accrual\". Begin by calling\ndyspoissometer_uint_list_malloc_zero().\n\n");
    freq_list0_base=dyspoissometer_uint_list_malloc_zero(mask_max);
    if(!freq_list0_base){
      demo_out_of_memory_print();
      break;
    }
    DEBUG_PRINT("OK, that worked. Now we need to determine the frequency of each mask using the\nfreq_list0_base which we just allocated and has been set to zero.\nfreq_list0_base[N] will then contain the frequency of mask N. To do this, call\ndyspoissometer_freq_list_update_autoscale() with (mask_count_implied==0) as\nrequired on the first call. Watch out for the return value, which is ignored for\nthe purposes of this demo.\n\n");
    mask_count_implied=0;
    dyspoissometer_freq_list_update_autoscale(freq_list0_base, &mask_count_implied, mask_idx_max, mask_list0_base, mask_max);
    DEBUG_PRINT("Done. The output is:\n\n");
    DEBUG_LIST("freq_list0_base", DEMO_MASK_MAX+1, (u8 *)(freq_list0_base), DYSPOISSOMETER_UINT_SIZE_LOG2);
    DEBUG_PRINT("\nSo this means that mask zero has frequency 3 (occurs 3 times), mask one has\nfrequency 5, etc. You can confirm this by looking at mask_list0_base above.\n\n");
    DEBUG_PRINT("Now we need to create a population list corresponding to the frequency list\nwhich we just generated. This can be done via\ndyspoissometer_pop_list_init().\n\n");
    pop_list0_base=dyspoissometer_pop_list_init(freq_list0_base, &freq_max_minus_1, &h0, mask_max);
    if(!pop_list0_base){
      demo_out_of_memory_print();
      break;
    }
    DEBUG_PRINT("So then here is the resulting population list:\n\n");
    freq_max=(DYSPOISSOMETER_UINT)(freq_max_minus_1+1);
    DEBUG_LIST("pop_list0_base", (ULONG)(freq_max), (u8 *)(pop_list0_base), DYSPOISSOMETER_UINT_SIZE_LOG2);
    DEBUG_PRINT("\nSo this means that frequency one has population one (occurs once), frequency 2\noccurs once, frequency 3 occurs 3 times, etc. You can confirm this by looking\nat freq_list0_base above. (You can think of population as the frequency of the\nfrequency.)\n\n");
    DEBUG_PRINT("Now that we have a population list, we can answer the \"how random\"\nquestion. Start by getting its logfreedom from\ndyspoissometer_logfreedom_dense_get():\n\n");
    logfreedom=dyspoissometer_logfreedom_dense_get(freq_max_minus_1, 1, mask_idx_max, mask_max, pop_list0_base);
    DEBUG_NUMBER("logfreedom", logfreedom);
    DEBUG_PRINT("\nWhile this may not be very informative on its own, if we take (e==2.71828...)\nraised to logfreedom, we'll get the number of possible sets of\n(mask_idx_max+1) masks, each of which on [0, mask_max], which would result in\nexactly the pop_list0_base given above:\n\n");
    way_count=ROUND(EXP(logfreedom));
    DEBUG_NUMBER("way_count", way_count);
    DEBUG_PRINT("\nTrust me, way_count is an integer, which is most clearly visible on account of\ntrailing zeroes in DYSPOISSOMETER_NUMBER_QUAD mode (make demo_quad).\n\n");
    DEBUG_PRINT("But most usefully, ceil(logfreedom/(log(2)) will provide us with the number of\nbits required to combinatorially encode a mask list adhering to\npop_list0_base -- in other words, to guarantee that we can fully represent an\ninteger on the interval [0, way_count-1]:\n\n");
/*
I don't want to define CEIL() just for the following demo case. So cheat by truncating then adding one.
*/
    bits_in_mask_set=(u32)((u32)(logfreedom/LOG(2))+1);
    DEBUG_U32("bits_in_mask_set", bits_in_mask_set);
    DEBUG_PRINT("\nThat is, if we ignore the cost of encoding pop_list0_base itself, we would\nrequire 0x5A (90) bits of storage to encode mask_list0_base, given\nmask_idx_max and mask_max with their values as given above.\n\n");
    DEBUG_PRINT("Now we can compute the dyspoissonism of pop_list0_base using\ndyspoissometer_dyspoissonism_get():\n\n");
    dyspoissonism=dyspoissometer_dyspoissonism_get(logfreedom, mask_idx_max, mask_max);
    DEBUG_NUMBER("dyspoissonism", dyspoissonism);
    DEBUG_PRINT("\nNow that we know the actual logfreedom, how does it compare to the maximum\npossible logfreedom attainable, given the constraints of mask_idx_max and\nmask_max? We can answer this question with dyspoissometer_logfreedom_max_get(),\nwhich uses a Monte Carlo method to provide an accurate approximation. (In this\ncase, we'll use 100 iterations, but in practice, you might need orders of\nmagnitude more, as convergence happens in somewhat unpredictable leaps.) Note\nthat we need to check for a negative return value, indicating memory\nexhaustion. Also, for the sake of parallel searching, the function requires a\nrandom number seed. Here's the result:\n\n");
    logfreedom_max=dyspoissometer_logfreedom_max_get(100-1, mask_idx_max, mask_max, &random_seed);
    if(logfreedom_max<0.0f){
      demo_out_of_memory_print();
      break;
    }
    DEBUG_NUMBER("logfreedom_max", logfreedom_max);
    DEBUG_PRINT("\nBut how can we have any faith in the accuracy of logfreedom_max (which by the\nway is much more likely to be understated than overstated)? Well, we know that\nthe dyspoissonism floor (D0) is positive but approaches zero as the mask and\nmask counts approach infinity. The denominator is (Q ln Z), where Q is the\nmask count and Z is the mask span. So if our estimate of maximum logfreedom\nis accurate, then it should be slightly less than this value. Let's see:\n\n");
    logfreedom_upper_bound=(DYSPOISSOMETER_UINT)(mask_idx_max+1)*LOG_N_PLUS_1(mask_max);
    DEBUG_NUMBER("logfreedom_upper_bound", logfreedom_upper_bound);
    DEBUG_PRINT("\nSo that worked fine. Now that we have what appears to be an accurate\napproximation logfreedom_max, we can find D0, the dyspoissonism floor, from\ndyspoissometer_dyspoissonism_get():\n\n");
    d0=dyspoissometer_dyspoissonism_get(logfreedom_max, mask_idx_max, mask_max);
    DEBUG_NUMBER("d0", d0);
    DEBUG_PRINT("\nWe could assess randomness by computing the ratio of dyspoissonism to D0, but\nthat approach would only be valid with constant mask count and mask span.\nFortunately, there is a more generic basis of comparison...\n\n");
    DEBUG_PRINT("Returning to the original question, how random is mask_list0_base? One way to\nanswer this is to compute (logfreedom/logfreedom_max), which will give the\nratio of the information content in mask_list0_base to what would be expected\nin the most random mask list compliant with mask_idx_max and mask_max:\n\n");
    information_density=logfreedom/logfreedom_max;
    DEBUG_NUMBER("information_density", information_density);
    DEBUG_PRINT("\nBut because the information density is usually close to one, a more useful\nmetric is information _sparsity_, which is just one minus that quantity (but\nshould be computed using dyspoissometer_sparsity_get() for safety reasons:)\n\n");
    information_sparsity=dyspoissometer_sparsity_get(logfreedom, logfreedom_max);
    DEBUG_NUMBER("information_sparsity", information_sparsity);
    DEBUG_PRINT("\nThe great thing about information sparsity is that it can be used as a fair,\nnormalized, and unambiguous comparator across sets of different mask counts and\nmask spans in order to assess randomness quality. Its only downside is that it\nrequires finding logfreedom_max, which is easily approximated, although the\nerror in which is somewhat unclear at present. This is precisely why\ndyspoissonism is a better randomness comparator, provided that mask counts and\nmask spans are held constant, as is often the case in signal analysis.\n\n");
    DEBUG_PRINT("Note that, while dyspoissonism can only reach zero in the limit of an infinite\nset (although it can readily reach one), information sparsity can reach either\nextreme with any set.\n\n");
    DEBUG_PRINT("Back to the original question: information_sparsity suggests that\nmask_list0_base is almost -- but not quite -- as random as it could possibly\nbe. This is unsurprising for such a small mask list. But for large lists, an\nelevated dyspoissonism or information sparsity can help to identify unseen\nstatistical biases -- and perhaps even predictable mathematical structure -- in\nthe data source.\n\n");
    DEBUG_PRINT("Now let me show you another way to compute logfreedom. Suppose that we\ngenerate a frequency list from a mask list:\n\n");
    DEBUG_LIST("freq_list1_base", (ULONG)(DEMO_MASK_MAX+1), (u8 *)(freq_list1_base), DYSPOISSOMETER_UINT_SIZE_LOG2);
    DEBUG_PRINT("\nThis means that mask zero has frequency 8, mask one has frequency 11, mask 2\nhas frequency 12, etc. But now, without taking the time to produce a population\nlist, we can compute the logfreedom directly using\ndyspoissometer_logfreedom_sparse_get() (and again, checking for success on\nreturn).\n\n");
    DEBUG_PRINT("In order to do this, though, we must first allocate one other frequency list\nequal in size to freq_list1_base above -- namely, 9 (DYSPOISSOMETER_UINT)s\nbecause (mask_max==8). We use dyspoissometer_uint_list_malloc() for this\npurpose. Here we go:\n\n");
    freq_list4_base=dyspoissometer_uint_list_malloc(DEMO_MASK_MAX);
    status=0;
    if(freq_list4_base){
      logfreedom=dyspoissometer_logfreedom_sparse_get(freq_list1_base, freq_list4_base, DEMO_MASK_MAX);
      if(logfreedom<0.0f){
        status=1;
      }else{
        DEBUG_NUMBER("logfreedom", logfreedom);
        DEBUG_PRINT("\nOK, that worked. Now we can deallocate those 2 frequency lists that we just\nallocated, using dyspoissometer_free()...\n");
      }
    }else{
      status=1;
    }
    dyspoissometer_free(freq_list4_base);
    if(status){
      demo_out_of_memory_print();
      break;
    }
    status=1;
    DEBUG_PRINT("\nDone. Now, just to be paranoid, let's compute the population list of\nfreq_list1_base above:\n\n");
    DEBUG_LIST("pop_list1_base", (ULONG)(DEMO_POP_LIST1_UINT_COUNT), (u8 *)(pop_list1_base), DYSPOISSOMETER_UINT_SIZE_LOG2);
    DEBUG_PRINT("\nYou can verify that this is consistent with freq_list1_base: it contains one\nfrequency 8, 2 frequency 9s, etc. So we can now call\ndyspoissometer_logfreedom_dense_get():\n\n");
    logfreedom=dyspoissometer_logfreedom_dense_get(DEMO_POP_LIST1_UINT_COUNT-1, 1, DEMO_MASK_IDX_MAX1, DEMO_MASK_MAX, pop_list1_base);
    DEBUG_NUMBER("logfreedom", logfreedom);
    DEBUG_PRINT("\nAs you can see, this matches the value above within the limits of numerical\nerror.\n\n");
    DEBUG_PRINT("So as mentioned above, the minimum frequency with nonzero population is\nfrequency 8. Conveniently, we can avoid wasting resources dealing with all\nthose leading zeroes by calling dyspoissometer_logfreedom_dense_get() with\n(freq_min==8) and the same freq_max_minus_1:\n\n");
    DEBUG_LIST("pop_list2_base", (ULONG)(DEMO_POP_LIST2_UINT_COUNT), (u8 *)(pop_list2_base), DYSPOISSOMETER_UINT_SIZE_LOG2);
    logfreedom=dyspoissometer_logfreedom_dense_get(DEMO_POP_LIST1_UINT_COUNT-1, 8, DEMO_MASK_IDX_MAX1, DEMO_MASK_MAX, pop_list2_base);
    DEBUG_NUMBER("logfreedom", logfreedom);
    DEBUG_PRINT("\nAs expected, the logfreedom is unchanged.\n\n");
    DEBUG_PRINT("From time to time, you may find it useful to analyze integer approximations of\nPoisson distributions. dyspoissometer_poisson_term_get() can help you with\nthis. Unfortunately, there are many ways to map (analog) Poisson distributions\nto integer approximations thereof. In theory, as the mask counts and mask spans\ngrow, the particular approximation method becomes less important, and the\ninformation sparsity of the resulting discrete distribution approaches zero.\nHere's a simple example using successively larger integer approximations of the\nlambda-one Poisson distribution (LOPD). At each step, the mask count and mask\nspan are both multiplied by about 256. Let's see what happens. (This might\ntake a few minutes):\n\n");
    poisson_mask_count=U8_MAX;
    poisson_mask_span=U8_MAX;
    status=0;
    do{
      poisson_freq=0;
      poisson_mask_idx_max=0;
      poisson_mask_max=0;
      do{
/*
We need to multiply by the mask count in order to obtain an integer approximation of Poisson population. The 0.5f is for fair rounding purposes.
*/
        poisson_pop=(DYSPOISSOMETER_UINT)((poisson_mask_span*dyspoissometer_poisson_term_get(poisson_freq, poisson_mask_count, poisson_mask_span))+0.5f);
        poisson_list_base[poisson_freq]=poisson_pop;
        poisson_mask_idx_max=(DYSPOISSOMETER_UINT)(poisson_mask_idx_max+(poisson_pop*poisson_freq));
        poisson_mask_max=(DYSPOISSOMETER_UINT)(poisson_mask_max+poisson_pop);
        poisson_freq++;
      }while(poisson_freq<DEMO_POISSON_IDX_COUNT);
      poisson_mask_idx_max--;
      poisson_mask_max--;
/*
Strictly speaking according to the C11 specification, poisson_mask_idx_max and poisson_mask_max are only _approximated_ by (poisson_mask_count-1) and (poisson_mask_span-1), respectively, because they were generated by disgusting floating point operations. In theory, they might even have wrapped. Worse, the distributions that we generated might actually exceed the implied mask_idx_max limit of (DYSPOISSOMETER_UINT_MAX-1) or the mask_max limit of DYSPOISSOMETER_UINT_MAX. (But probably not. We took some liberties with overflow checking here because this is merely a demo.)
*/
      poisson_logfreedom=dyspoissometer_logfreedom_dense_get(DEMO_POISSON_IDX_COUNT-2, 0, poisson_mask_idx_max, poisson_mask_max, poisson_list_base);
      logfreedom_max=dyspoissometer_logfreedom_max_get(100000-1, poisson_mask_idx_max, poisson_mask_max, &random_seed);
      if(logfreedom_max<0.0f){
        status=1;
        break;
      }
      poisson_information_sparsity=dyspoissometer_sparsity_get(poisson_logfreedom, logfreedom_max);
      DEBUG_U64("poisson_mask_idx_max", poisson_mask_idx_max);
      DEBUG_U64("poisson_mask_max", poisson_mask_max);
      DEBUG_NUMBER("poisson_information_sparsity", poisson_information_sparsity);
      poisson_mask_count=(DYSPOISSOMETER_UINT)(poisson_mask_count<<U8_BITS);
      poisson_mask_span=(DYSPOISSOMETER_UINT)(poisson_mask_span<<U8_BITS);
    }while(poisson_mask_count);
    if(status){
      demo_out_of_memory_print();
      break;
    }
    status=1;
    DEBUG_PRINT("\nThe trend above is not convincingly monotonic (if you \"make demo_quad\"). Is\nthis merely reflective of a poor approximation of logfreedom_max at high mask\ncounts? Or are Poisson distributions asymptotically but nonmonotonically of zero\ninformation sparsity? This is a hard open question. (I'm even slightly doubtful\nthat the asymptotic hypothesis is true, because I'm only mostly convinced that\nmaximum information density is expected behavior in large mask lists.) In any\nevent, we frequently speak of Poisson distributions as though they are discrete\nbecause their domain is the whole numbers; their range is analog, however,\nwhich leaves a lot to be desired.\n\n");
    DEBUG_PRINT("Speaking of distributions, Dyspoissometer has a way to generate pseudorandom\nmask lists which adhere to a given mask count and mask span, namely\ndyspoissometer_mask_list_pseudorandom_get(). This function is particularly\nuseful for doing statistical surveys of such lists, which for example could\nbe used to find the variance of dyspoissonism. For instance, here's a random\nmask list for (mask_idx_max==37) and (mask_max==14):\n\n");
    mask_list_base=dyspoissometer_uint_list_malloc_zero(37);
    if(!mask_list_base){
      demo_out_of_memory_print();
      break;
    }
    dyspoissometer_mask_list_pseudorandom_get(37, mask_list_base, 14, &random_seed);
    DEBUG_LIST("mask_list_base", (ULONG)(37+1), (u8 *)(mask_list_base), DYSPOISSOMETER_UINT_SIZE_LOG2);  
    DEBUG_PRINT("\nAs you can see, mask_list_base contains 38 masks on [0, 14]. If you run this\ndemo using a different numerical precision, you will get a different random\nmask list due to variances in the evolutionary pathway of\ndyspoissometer_logfreedom_max_get(), as its decision process and therefore the\nnumber of times it iterates the random seed vary with numerical error, despite\nhaving asymtotically constant output.\n\n");
    dyspoissometer_free(mask_list_base);
    DEBUG_PRINT("Let's get on with a more exciting application of this generator, which is the\nasymptotic discovery of median logfreedom, courtesy of\ndyspoissometer_logfreedom_median_get(). It works like this: generate lots of\nrandom mask lists, all of which adhering to the same mask counts and mask\nspans. Then find the logfreedom of each such mask list, and accumulate them in\na logfreedom list. Finally, find the median value thereof. Median logfreedom is\nmore important than expected (mean) logfreedom because only the former\nessentially guarantees that any random modification of the mask list has equal\nprobability to decrease or increase the logfreedom; in other words, median (not\nmean!) logfreedom is an entropy equilibrium point. So a good true random number\ngenerator should make mask lists whose median logfreedom is the aforementioned\nvalue. Just watch the return value for error status!\n\n");
    logfreedom_median=dyspoissometer_logfreedom_median_get(1000-1, 37, 14, &random_seed);
    if(logfreedom_median<0.0f){
      demo_out_of_memory_print();
      break;
    }
    DEBUG_NUMBER("logfreedom_median", logfreedom_median);
    DEBUG_PRINT("\nSo the value above is an approximation of the median logfreedom, obtained from\n1000 random mask lists. If you found the logfreedom of _every_ possible mask\nlist, sorted those logfreedoms, then took the middle value thereof, you should\nfind it to be close to if not exactly the value above (which is, in fact, an\nactual logfreedom as opposed to an interpolated value).\n\n");
    DEBUG_PRINT("Sanity check: the value above is slightly less than ((37+1) ln (14+1)), which\nis about 103. Pass!\n\n");
    DEBUG_PRINT("Now I have something amazing to show you. Take a look at pop_list1_base\nabove. Notice how it looks quite like a Poisson distribution with lambda\n(expected frequency) equal to 10. By contrast, look at pop_list3_base below,\nwhich has the same mask count and mask span.\n\n");
    DEBUG_LIST("pop_list3_base", (ULONG)(DEMO_POP_LIST3_UINT_COUNT), (u8 *)(pop_list3_base), DYSPOISSOMETER_UINT_SIZE_LOG2);
    DEBUG_PRINT("\nIn particular, note that pop_list3_base is bimodal; it has 2 peaks -- very\nunlike a Poisson distribution. So logically, it should have lower logfreedom:\n\n");
    logfreedom=dyspoissometer_logfreedom_dense_get(DEMO_POP_LIST3_UINT_COUNT-1, 1, DEMO_MASK_IDX_MAX1, DEMO_MASK_MAX, pop_list3_base);
    DEBUG_NUMBER("logfreedom", logfreedom);
    DEBUG_PRINT("\nWhoa! What's going on here? How could such a distribution have _greater_\nlogfreedom, corresponding to lesser dyspoissonism and thus greater entropy?\nAnd that's only the tip of the iceburg...\n\n");
    DEBUG_PRINT("Let's look at every single neighbor that pop_list1_base has. A \"neighbor\" of a\npopulation list is simply another population list which corresponds to a single\nmask change. In other words, the population of nonzero frequency A decreases by\none, while the population of frequency (A-1) increases by one; simultaneously,\nthe population of frequency B decreases by one, while the population of\nfrequency (B+1) increases by one, where (A!=(B+1)). (In the code, frequency A\nis called the \"down right\" frequency, and B is called the \"up left\" frequency.)\nThis is simply what happens when one mask is changed into another.\n\nIn classical physics, the Second Law of Thermodynamics implies that the entropy\nof a closed system always increases over time (because if we make ice in the\nfreezer, then enough heat is dissipated into the kitchen to create a net\npositive change the entropy of the entire house. So logically, at least one\nneighbor of pop_list1_base should have greater logfreedom, because we\ndemonstrated above that its logfreedom is less than that of a \"camel humps\"\ndistribution. You'll have to look at the code in demo.c for this exercise, but\nhere are the logfreedoms of each of its neighbors:\n\n");
/*
Allocate a copy of pop_list0_base, with an extra zero at the beginning and another on the end because some neighbors may have a one in the prefix or suffix population position. Thus pop_list0_base will have a maximum index of (DEMO_POP_LIST1_UINT_COUNT+1), or equivalently, an index count of (DEMO_POP_LIST1_UINT_COUNT+2) corresponding to a minimum frequency of zero and a maximum frequency of (DEMO_POP_LIST1_UINT_COUNT+1).
*/
    pop_list4_base=dyspoissometer_uint_list_malloc(DEMO_POP_LIST1_UINT_COUNT+1);
    if(!pop_list4_base){
      demo_out_of_memory_print();
      break;
    }
    dyspoissometer_uint_list_zero(DEMO_POP_LIST1_UINT_COUNT+1, pop_list4_base);
    pop_idx=1;
    do{
      pop_list4_base[pop_idx]=pop_list1_base[pop_idx-1];
    }while((pop_idx++)!=DEMO_POP_LIST1_UINT_COUNT);
    logfreedom=dyspoissometer_logfreedom_dense_get(DEMO_POP_LIST1_UINT_COUNT, 0, DEMO_MASK_IDX_MAX1, DEMO_MASK_MAX, pop_list4_base);
    DEBUG_NUMBER("logfreedom_self", logfreedom);
    pop_idx_down=1;
    do{
      pop_idx_up=1;
      do{
        neighbor_status=0;
        pop_idx_delta=(DYSPOISSOMETER_UINT)(pop_idx_down-pop_idx_up);
/*
Don't waste time moving a population unit down, then back up. Otherwise, see if we can find a neighboring population list, given the constraint of not being allowed to let population go negative.
*/
        if(pop_idx_delta!=1){
          pop_down_left=pop_list4_base[pop_idx_down-1];
          pop_down_left_old=pop_down_left;
          pop_down_right=pop_list4_base[pop_idx_down];
          pop_down_right_old=pop_down_right;
          pop_up_left=pop_list4_base[pop_idx_up];
          pop_up_left_old=pop_up_left;
          pop_up_right=pop_list4_base[pop_idx_up+1];
          pop_up_right_old=pop_up_right;
          if(2<pop_idx_delta){
/*
We have 4 distinct frequencies. Try to move one population unit from pop_down_right to pop_down_left, and another from pop_up_left to pop_up_right.
*/
            if(pop_down_right&&pop_up_left){
              neighbor_status=1;
              pop_down_left++;
              pop_down_right--;
              pop_up_left--;
              pop_up_right++;
            }
          }else if(!pop_idx_delta){
/*
We have only 3 frequencies. Try to move one population unit from pop_down_right to pop_down_left, and another from (pop_up_left==pop_down_right) to pop_up_right.
*/
            if(1<pop_down_right){
              neighbor_status=1;
              pop_down_left++;
              pop_down_right=(DYSPOISSOMETER_UINT)(pop_down_right-2);
              pop_up_left=pop_down_right;
              pop_up_right++;
            }
          }else if(pop_idx_delta==2){
/*
We have only 3 frequencies. Try to move one population unit from pop_down_right to pop_down_left, and another from pop_up_left to (pop_up_right==pop_down_left).
*/
            if(pop_down_left&&pop_up_left){
              neighbor_status=1;
              pop_down_left=(DYSPOISSOMETER_UINT)(pop_down_left+2);
              pop_down_right--;
              pop_up_left--;
              pop_up_right=pop_down_left;
            }
          }
          if(neighbor_status){
            pop_list4_base[pop_idx_down-1]=pop_down_left;
            pop_list4_base[pop_idx_down]=pop_down_right;
            pop_list4_base[pop_idx_up]=pop_up_left;
            pop_list4_base[pop_idx_up+1]=pop_up_right;
            logfreedom=dyspoissometer_logfreedom_dense_get(DEMO_POP_LIST1_UINT_COUNT, 0, DEMO_MASK_IDX_MAX1, DEMO_MASK_MAX, pop_list4_base);
            DEBUG_NUMBER("logfreedom_neighbor", logfreedom);
            pop_list4_base[pop_idx_down-1]=pop_down_left_old;
            pop_list4_base[pop_idx_down]=pop_down_right_old;
            pop_list4_base[pop_idx_up]=pop_up_left_old;
            pop_list4_base[pop_idx_up+1]=pop_up_right_old;
          }
        }
      }while((pop_idx_up++)!=DEMO_POP_LIST1_UINT_COUNT);
    }while((pop_idx_down++)!=DEMO_POP_LIST1_UINT_COUNT);
    logfreedom=dyspoissometer_logfreedom_dense_get(DEMO_POP_LIST1_UINT_COUNT, 0, DEMO_MASK_IDX_MAX1, DEMO_MASK_MAX, pop_list4_base);
    DEBUG_NUMBER("logfreedom_self", logfreedom);
    DEBUG_PRINT("\nNotice how _every_ neighbor has _lesser_ logfreedom. In other words,\npop_list1_base exists at a _local_ entropy maximum even though we just proved\nabove that it does _not_ exist at a _global_ maximum; if we change any single\nmask, the result will be a population list with lesser (never equal)\nlogfreedom. This is profound. It implies that some discrete systems are so\nheavily constrained that _any_ minimum quantum of change will send them into a\nlower entropy state! If the universe is ultimately digital, then perhaps there\nare natural examples of this phenomenon. Note that this goes beyond simple\nquantum systems which occasionally lose entropy by sheer chance; in this case,\n_any_ single step we take will result in lesser entropy!\n\n");
    DEBUG_PRINT("I should add that it's precisely this lack of entropy gradient which makes the\ndeterministic discovery of D0 so difficult. Indeed, I only discovered this\nproblem when I wrote a function intended to do so by riding down the gradient.\ndyspoissometer_logfreedom_max_get() is therefore somewhat nondeterministic,\nalthough it rides gradients eagerly when possible.\n\n");
    DEBUG_PRINT("I hope you learned something from this demo and find Dyspoissometer useful.\nI have just one more suggestion before concluding: if you want a direct and\neasy way to find logfreedom, try one of the\ndyspoissometer_u8/u16/u24/u32_list_logfreedom_get() functions. Yes, we could\nhave used them above, but then you would not have learned as much about\ndyspoissonism.\n\n");
    DEBUG_PRINT("Scroll up or redirect to a file in order to see what you missed!\n");
    status=0;
  }while(0);
  dyspoissometer_free(pop_list4_base);
  dyspoissometer_free(pop_list0_base);
  dyspoissometer_free(freq_list0_base);
  DEBUG_ALLOCATION_CHECK();
  return status;
}
