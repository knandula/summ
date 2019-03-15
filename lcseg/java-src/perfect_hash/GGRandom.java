/*
 * @(#) GGRandom.java - Provides a random generator
 *
 *    Copyright (C) 1997 Jiejun KONG
 *    written by Jiejun KONG (jkong@eos.ncsu.edu)
 *
 * This file is part of KONG's GGPerf.
 *
 * KONG's GGPerf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * KONG's GGPerf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KONG's GGPerf; see the file COPYING.  If not, write to the Free
 * Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111, USA.
 */

package perfect_hash;

import java.util.Random;

/**
 * GGRandom is a random number generator that generates numbers in a
 * specific range.
 * @author Jiejun KONG
 * @see java.util.Random
 */
public // merely for utilizing `javadoc'
class GGRandom extends Random
{
    /**
     * The numbers generated should be in the range of <EM>[0..range-1]</EM>.
     */
    protected long range;

    /**
     * Creates a new random number generator with range restriction.
     * The first argument is a long integer seed. The numbers generated
     * should be in a long range which is the second argument.
     */
    protected GGRandom(long seedValue, long rangeValue)
    {
        super(seedValue);
        range = rangeValue;
    }

    //
    // Accessers & Mutators are unnecessary
    // because `range' will not be modified from initialization on.
    //

    //
    // debugging data for random generator
    //
    private long debug[] =
    {
        /* january */
        11, 3, 22, 4, 2, 14, 8, 11, 0, 7, 12, 3, 4, 21,
        /* feberuary */
        22, 19, 11, 6, 20, 11, 16, 12, 13, 3, 6, 21, 23, 3, 21, 14, 17, 6,
        /* m_rch */
        1, 20, 10, 19, 24, 1, 12, 8,
        /* ap_il */
        17, 13, 6, 3, 15, 20, 7, 7,
        /* __y */
        6, 13,
        /* _u__ */
        1, 9,
        /* __ly */
        8, 20, 24, 10,
        /* __g_st */
        0, 11, 20, 17, 8, 20,
        /* s_ptember */
        0, 9, 20, 1, 16, 16, 23, 10, 20, 10, 1, 24, 18, 13, 6, 3,
        /* october */
        10, 2, 10, 5, 24, 11, 24, 22, 15, 10, 14, 11, 22, 18,
        /* nov_mbe_ */
        24, 22, 19, 7, 16, 3, 6, 21, 0, 20, 11, 19,
        /* d_c_____ */
        10, 17, 21, 0
    };
    private int now=0;
    
    /**
     * Generates the next pseudorandom number in the specified range.
     * It calls nextLong() in java.util.Random.
     */
    protected long next_random()
    {
        //return debug[now++];
        return (Math.abs(super.nextLong())) % range;
    }

    //
    // JDK1.1 documentation says I should override the method next(int bits).
    // However, that saying is for replacing the JDK's algorithm with a new
    // random generating algorithm.  Since I don't change the random
    // generating algorithm, it is not necessary to override the method.
    //
}    

