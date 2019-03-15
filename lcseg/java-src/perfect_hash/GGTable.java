//   Provides a table structure
//   Copyright (C) 1997 Jiejun KONG
//   written by Jiejun KONG (jkong@eos.ncsu.edu)
//
//This file is part of KONG's GGPerf.
//
//KONG's GGPerf is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 1, or (at your option)
//any later version.
//
//KONG's GGPerf is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with KONG's GGPerf; see the file COPYING.  If not, write to the Free
//Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111, USA.
//

package perfect_hash;

import java.lang.*;
import java.io.*;
import java.util.*;

/**
  * This class is the internal representation of the table T1 and T2.
  * The meaning of T1 and T2 could be found in explanations of GGPerf's
  * algorithm.
  * @author Jiejun KONG
  */
public // merely for utilizing `javadoc'
class GGTable
{
    /**
      * Variable holding all table heads for T1 and T2, respectively.
      * Thus this variable is an instance variable.  A table head is a
      * java.util.Vector holding character-value pairs for a whole row.
      */
    protected Vector heads;
    /**
      * Number of heads in a table.
      * T1 and T2 actually have same number of heads, thus this variable
      * is a class variable.
      */
    protected static int n_heads;
    /**
      * Minimal ASCII value in T1 and T2.
      * Table length would be shortened with this value being recorded.
      * T1 and T2 actually have same set of characters, thus this variable
      * is a class variable.
      */
    protected static char min_char_val;
    /**
      * Maximal ASCII value in T1 and T2.
      * Table length would be shortened with this value being recorded.
      * T1 and T2 actually have same set of characters, thus this variable
      * is a class variable.
      */
    protected static char max_char_val;
    /**
      * The random number generator for generating values of T1 and T2.
      * It is shared by all tables, thus this variable is a class variable.
      */
    protected static GGRandom random_generator;

    // Accessors & Mutators
    /**
      * Returns how many rows there are in T1 and T2.
      * @return number of rows as an integer value
      */
    protected static int getNHeads()
    {
        return n_heads;
    }
    /**
      * Set number of rows of T1 and T2 to be the argument.
      * @param n number of rows
      */
    protected static void setNHeads(int n)
    {
        n_heads = n;
    }

    /**
      * Return the minimal ASCII value in all keys.
      * @return The minimal ASCII value.
      */
    protected static char getMinCharVal()
    {
        return min_char_val;
    }
    /**
      * Set the minimal ASCII value to be the argument.
      * @param l The new minimal ASCII value.
      */
    protected static void setMinCharVal(char l)
    {
        min_char_val = l;
    }


    /**
      * Return the maximal ASCII value in all keys.
      * @return The maximal ASCII value.
      */
    protected static char getMaxCharVal()
    {
        return max_char_val;
    }
    /**
      * Set the maximal ASCII value to be the argument.
      * @param h The new maximal ASCII value.
      */
    protected static void setMaxCharVal(char h)
    {
        max_char_val = h;
    }

    /**
      * Set the random generator of table values to be the argument.
      * @param ran The random number generator.
      */
    protected static void setRandomGenerator(GGRandom ran)
    {
        random_generator = ran;
    }

    // Constructor
    /**
      * Creates an instance of table representing one of T1 and T2.
      */
    protected GGTable()
    {
        // class variables are assumed to have already been set,
        // merely allocate vectors here.

        // create heads. One head for every rows of a table
        heads = new Vector(n_heads);

        // create a Hashtable for every head
        for(int i=0; i<n_heads; i++)
        {
            Hashtable items = new Hashtable();
            heads.addElement(items);
        }
    }

    /**
      * Insert the character at specified index of the key into
      * all GGTables. A randomly generated value is assigned to be
      * the table value correspondent to the character.
      * @param key The key.
      * @param index The string index.
      * @return Return the value assigned to the character.
      */
    protected long insert_into_table(String key, int index)
    {
        //System.out.print("key="+key+"\tindex="+index);

        Character c = new Character(key.charAt(index));
        Hashtable items = (Hashtable)heads.elementAt(index);
        Long assigned = (Long)items.get(c);

        // if it is already there, just return the old value
        if(assigned != null)
            return assigned.longValue();

        // otherwise, call the random generator
        long value = random_generator.next_random();

        items.put(c, new Long(value));
        //System.out.println("    as value "+value);
        return value;
    }

    /**
      * Returns the value previously assigned to the character
      * that is at the specified index of the key.
      * @param key The key.
      * @param index The string index.
      * @return The previously assigned value.
      */
    protected long table_value(String key, int index)
    {
        Character c = new Character(key.charAt(index));
        Hashtable items = (Hashtable)heads.elementAt(index);
        Long assigned = (Long)items.get(c); 

        // it should be there
        if(assigned == null)
        {
            System.err.println("Internal fatal error: can't find table items for " + key);
            System.exit(-1);
        }
        return assigned.longValue();
    }

    /**
      * A debugging routine.
      * It is not an API function.
      * It eligibly shows the contents of the table.
      */
    protected void print_table()
    {
        for(int i=0; i<n_heads; i++)
        {
            System.out.println("Row " + i);

            Hashtable items = (Hashtable)heads.elementAt(i);

            Enumeration e = items.keys();
            while(e.hasMoreElements())
            {
                Character next = (Character)e.nextElement();
                char c = next.charValue();
                System.out.print("|" + c + ":" + ((Long)items.get(next)).longValue());
            }
            System.out.println("|");
        }
    }

    /**
      * Output the contents of the GGTable as a data structure,
      * normally an array.
      * @param table_name The name of the output data structure. 
      * @param out The output stream.
      * @param java Output in Java if the argument is true; otherwise, output in C.
      */
    protected void output_table(String table_name, PrintWriter out, boolean java)
    {
        String indent = new String();

        if(java)
        {
            indent = new String("    ");
            out.println(indent+"private static final int "+table_name+"[][]=\n"+indent+"{");
        }
        else
            out.print("static long "+table_name+"["+n_heads+"]["+(max_char_val-min_char_val+1)+"]=\n{");

        for(int i=0; i<n_heads; i++)
        {
            Hashtable items = (Hashtable)heads.elementAt(i);
            int outed = 0;

            out.print(indent+"{");
            for(char c=min_char_val; c<=max_char_val; c++, outed++)
            {
                Character ch = new Character(c);

                if(c != min_char_val)
                    out.print(",");
                
                if((outed % 0x10) == 0xf)
                    out.print("\n    ");
                
                if(items.containsKey(ch))
                {
                   out.print(((Long)items.get(ch)).longValue());
                   // remaining is all -1, it's okay not to output them
                   //if(outed == items.size())
                   //   break;
                }
                else
                    out.print("-1");
            }
            out.println("},");
        }

        out.println(indent+"};");
    }
}
