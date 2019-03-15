/*    The GGPerf main driver
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

import java.lang.*;
import java.io.*;
import java.util.*;
import perfect_hash.*;

/**
  * GGPerf is the main driver of the package.
  * @author Jiejun KONG
  */
public // merely for utilizing `javadoc'
class GGPerf
{
    // constants indicate current type of the intermediate graph,
    // if CYCLIC, then the algorithm requires a reset
    private static final boolean CYCLIC = true;
    private static final boolean ACYCLIC = false;

    // constants for option flag
    // for C
    private static final int GNUC_STYLE = 01;
    private static final int ANSIC_STYLE = 02;
    private static final int GLOBAL_KEYWORD_TABLE = 04;
    // for Java
    private static final int JAVA_STYLE = 010;
    // for both C and Java
    private static final int RETURN_BOOLEAN = 020;
    private static final int PRINT_STRUCTURE = 040;
    private static int options = 0;

    // strings from command line
    private static String hash_func_name = new String("hash");
    private static String lookup_func_name = new String("in_word_set");
    private static String wordlist_name = new String("wordlist");

    // for parsing declaration part
    // declaration is in LISP's symbol expression format
    private static Vector sym_exp;

    // C, N, M. Czech named them.
    // With respect to the article, I don't change the name
    private static double C=5; // or 25.0/12.0;
    private static long N=0;
    private static int M=0;

    // how long are the longest and the shortest key
    private static int longest = 0;
    private static int shortest = 2048; /* should be POSIX's LINE_MAX */

    // I/O variables
    private static String input_file_name = new String();
    private static RandomAccessFile input;
    private static String output_file_name = new String();
    private static PrintWriter output;

    // holding the verbatim part enclosed in %{ %}
    private static String verba = new String();
    // holding the verbatim additional function section
    private static boolean has_verba_func = false;
    private static String verba_func = new String();

    // holding keys and their attributes
    // keywords is a Java's Vector of Vector of (a key, its attributes).
    // The first element of a sub-Vector is a key, followed by its attribute(s)
    // The hash value for that key is implicitly indicated by where the key is stored
    // in the Vector `keywords', i.e. keywords.elementAt(i)'s hash value is i.
    private static Vector keywords = new Vector();
    // There are at least 1 elements in a keyword vector: key.
    // Intuitively speaking, number of data members in result structure/class
    private static int n_member = 1;

    // the T1 and T2, refer to document for their meanings
    private static GGTable T1, T2;
    // the vector for holding all nodes in the intermediate graph
    private static Vector graph;

    // the lowest ASCII value in the keys,
    // initial value '~' is the highest possible value
    private static char min_char_val = '~';
    // the highest ASCII value in the keys,
    // initial value SPACE is the lowest possible value
    private static char max_char_val = ' ';

    //============================================================
    // declaration part parser issue
    //============================================================
    // read and skip the declaration part
    // Assume that file is already open and
    // file pointer is already set correctly
    private static void read_declaration_part()
    {
        String struct_decl = new String();
        boolean verbatim = false;

        //System.out.println("begin of declaration.");
        // read until meet /^%%/
        while(true)
        {
            String line = new String();

            try
            {
                // have met End Of File
                // According to source code of java.io.RandomAccessFile
                // a `null' return value means EOF
                if((line = input.readLine()) == null)
                {
                    System.err.println("Unexpected EOF in declaration part: incomplete input file.");
                    System.exit(-1);
                }
            }
            catch (IOException e)
            {
                System.err.println("Error in reading file.");
                System.exit(-1);
            }
            //System.out.println("line:"+line);
            if(line.startsWith("%%"))
            {
                //System.out.println("end of declaration.");
                break;
            }
            if(line.startsWith("%{"))
                verbatim = true;
            else if(line.startsWith("%}"))
                verbatim = false;
            else if(verbatim)
                // jdk1.1.1 on SPARCstation has a nasty bug.
                // RandomAccessFile doesn't include newline.
                // So I have to include it
                verba = verba.concat(line+"\n");
            // recording
            else
                struct_decl = struct_decl.concat(line.trim());
        }

        // parsing
        GGSymExp s = new GGSymExp(struct_decl);
        sym_exp = s.getSExp();

        // structure name occupies one place in the vector, thus -1
        if(sym_exp != null)
            n_member = sym_exp.size() -1;
        else
            n_member = 1;
    }

    //============================================================
    // Graph issues
    //============================================================

    // tell whether there is already a node tagged with value <tag>
    // in the intermediate graph
    private static GGNode find_node(long tag)
    {
        if(tag >= N)
        {
            System.err.println("Internal fatal error: try to find invalid vertex tag " + tag + ".");
            System.exit(-1);
        }

        for(int i=0; i<graph.size(); i++)
        {
            GGNode node = (GGNode)graph.elementAt(i);
            if(node.getVertex() == tag)
                return node;
        }
        return null;
    }

    // declare reachability
    // whether there is a path from <node1> to <node2>
    // @param node1 the from node; node2: the to node
    // @return True: reachable; False: not reachable
    // use Depth First Search recursive programming to solve the problem
    private static boolean arrive(GGNode node1, GGNode node2)
    {
        if(node1 == node2)
            return true;

        // deepth first search the adjacency list
        node1.setVisited(true);
        Enumeration e = node1.getAdjacencyNodes();
        while(e.hasMoreElements())
        {
            GGNode adj = (GGNode)e.nextElement();
            if(adj.getVisited() == false)
                if(arrive(adj, node2))
                    return true;
        }
        return false;
    }

    // insert
    // @param tag for node1, node2, and edge between them
    // @return ACYCLIC: okay; CYCLIC: invalid graph
    private static boolean insert_into_graph(long value1, long value2, long edge)
    {
        //System.out.println("Inserting "+value1+", "+value2+" with weight "+edge);
        if(value1 >= N)
            System.err.println("Internal error: invalid vertex value " + value1 + ".");
        if(value2 >= N)
            System.err.println("Internal error: invalid vertex value " + value2 + ".");

        if(value1 == value2)
        {
            //System.out.println("\007Cyclic! " + value1 + " --> " + value2);
            return CYCLIC;
        }

        GGNode node1 = find_node(value1);
        GGNode node2 = find_node(value2);

        if((node1 != null) && (node2 != null))
        {
            // clear all `visited' flag for `arrive' predicate
            for(int i=0; i<graph.size(); i++)
                ((GGNode)graph.elementAt(i)).setVisited(false);

            if(arrive(node1, node2))
            {
                //System.out.println("\007Cyclic! " + node1.getVertex() + " --> " + node2.getVertex());
                return CYCLIC;
            }
        }

        // need to allocate space for node1
        if(node1 == null)
        {
            node1 = new GGNode(value1);
            graph.addElement(node1);
        }

        // need to allocate space for node2
        if(node2 == null)
        {
            node2 = new GGNode(value2);
            graph.addElement(node2);
        }

        // setup the adjacency list
        node1.addAdjacency(node2, new Long(edge));
        node2.addAdjacency(node1, new Long(edge));

        return ACYCLIC;
    }

    // recursively assign g_value for all nodes that could be
    // reached from current node
    private static void assign_g_value(GGNode current, long g_value)
    {
        // check for valid input
        if((g_value < 0) || (g_value >=M))
            System.err.println("Invalid g_value.");

        // already assigned, just return
        if(current.getGValue() != -1)
            return;

        current.setGValue(g_value);

        Enumeration e = current.getAdjacencyNodes();
        while(e.hasMoreElements())
        {
            GGNode adj = (GGNode)e.nextElement();
            long edge = current.getAdjacencyEdge(adj);
            long g = (edge - current.getGValue()) % M;
            assign_g_value(adj, (g<0)? g+M : g);
        }
    }

    // debugging routine
    // print all nodes in the intermediate graph
    private static void print_graph()
    {
        for(int i=0; i<graph.size(); i++)
        {
            GGNode node = (GGNode)graph.elementAt(i);
            node.print_node();
        }
    }

    // output G values
    private static void output_graph(PrintWriter out, boolean java)
    {
        int n_node = graph.size();
        int outed, i;

        if(java)
            out.print("    private static final int node[]=\n    {");
        else
            out.print("static long node["+n_node+"]=\n{");
        for(i=0, outed=0; i<n_node; i++)
        {
            GGNode node = (GGNode)graph.elementAt(i);
            if(i != 0)
                out.print(",");
            if((++outed % 0x10) == 0)
                out.print("\n    ");
            out.print(node.getVertex());
        }
        out.println("};");

        if(java)
            out.print("    private static final int G[]=\n    {");
        else
            out.print("static long G["+n_node+"]=\n{");
        for(i=0, outed=0; i<n_node; i++)
        {
            GGNode node = (GGNode)graph.elementAt(i);
            if(i != 0)
                out.print(",");
            if((++outed % 0x10) == 0)
                out.print("\n    ");
            out.print(node.getGValue());
        }
        out.println("};");
    }

    //============================================================
    // other issues
    //============================================================

    // process table and graph for the `key -> value' pair
    private static boolean table_assignment(String key, long value)
    {
        long sum1=0, sum2=0;

        //System.out.println("Inserting `"+key+"' with value "+value);
        // insert every character of the key into T1 and T2
        for(int i=0; i<key.length(); i++)
        {
            sum1 += T1.insert_into_table(key, i);
            sum1 %= N;
            sum2 += T2.insert_into_table(key, i);
            sum2 %= N;
        }

        // insert correspondent nodes into the graph
        return insert_into_graph(sum1, sum2, value);
    }

    /**
      * The hashing function.
      * After T1, T2 and intermediate graph are successfully constructed,
      * this function could be used to get hash value for a key.
      * @param key The key to be hashed
      * @return The value correspondent to the argument.
      */
    public static long hash(String key)
    {
        long f1=0, f2=0;
        GGNode node1, node2;

        for(int i=0; i<key.length(); i++)
        {
            f1 += T1.table_value(key, i);
            f1 %= N;
            f2 += T2.table_value(key, i);
            f2 %= N;
        }

        node1 = find_node(f1);
        // I am sure the following statement would not be executed :)
        if(node1 == null)
            System.err.println("Internal fatal error: no node labelled " + f1);
        node2 = find_node(f2);
        // I am sure the following statement would not be executed :)
        if(node2 == null)
            System.err.println("Internal fatal error: no node labelled " + f2);
        return (node1.getGValue() + node2.getGValue()) % M;
    }

    // send output as Java program
    private static void send_java_output(PrintWriter out)
    {
        String struct_name = new String("String");

        if(sym_exp != null)
            struct_name = (String)sym_exp.elementAt(0);

        out.println(verba);

        if((options & PRINT_STRUCTURE) == PRINT_STRUCTURE)
        {
            // contradictory
            if(sym_exp == null)
            {
                System.err.println("Error: Can't apply `-printstructure' option to a string-only keyword list.");
                System.exit(-1);
            }

            // print the structure definition
            out.println("class "+struct_name+"\n{");
            // print members
            for(int i=1; i<sym_exp.size(); i++)
            {
                Vector member = (Vector)sym_exp.elementAt(i);
                out.println("    "+(String)member.elementAt(0)+" "+(String)member.elementAt(1)+";");
            }
            // print Constructor
            out.print("\n    // Constructor\n    public "+struct_name+"(");
            for(int i=1; i<sym_exp.size(); i++)
            {
                Vector member = (Vector)sym_exp.elementAt(i);
                if(i > 1)
                    out.print(", ");
                out.print((String)member.elementAt(0)+" "+(String)member.elementAt(1)+i);
            }
            out.println(")\n    {");
            for(int i=1; i<sym_exp.size(); i++)
            {
                Vector member = (Vector)sym_exp.elementAt(i);
                out.println("\t"+(String)member.elementAt(1)+" = "+(String)member.elementAt(1)+i+";");
            }
            out.println("    }");
            // print Accessors
            out.print("\n    // Accessors");
            for(int i=1; i<sym_exp.size(); i++)
            {
                Vector member = (Vector)sym_exp.elementAt(i);
            
                out.println("\n    public "+(String)member.elementAt(0)+" "+(String)member.elementAt(1)+"()\n    {\n\treturn "+(String)member.elementAt(1)+";\n    }");
            }
            out.println("}\n");
        }

        // print the hashing class
        out.println("class "+output_file_name.substring(0, output_file_name.indexOf('.')));
        out.println("{");
        out.println("    public static final int TOTAL_KEYWORDS = "+M+";");
        out.println("    public static final int MIN_HASH_VAL = 0;");
        out.println("    public static final int MAX_HASH_VAL = "+(M-1)+";");

        char c;

        if((c = GGTable.getMinCharVal()) == '\'')
            out.println("    public static final int MIN_CHAR_VAL = '\\'';");
        else
            out.println("    public static final int MIN_CHAR_VAL = '"+GGTable.getMinCharVal()+"';");
        if((c = GGTable.getMaxCharVal()) == '\'')
            out.println("    public static final int MAX_CHAR_VAL = '\\'';");
        else
            out.println("    public static final int MAX_CHAR_VAL = '"+GGTable.getMaxCharVal()+"';");

        out.println("    public static final int MIN_WORD_LENG = "+shortest+";");
        out.println("    public static final int MAX_WORD_LENG = "+longest+";");

        out.println("    private static final int MAX_TABLE_HEIGHT = "+GGTable.getNHeads()+";");
        out.println("    private static final int MAX_GRAPH_NODE_VAL = "+N+";");
        T1.output_table("T1", out, true);
        T2.output_table("T2", out, true);

        out.println("    private static final int MAX_NODE_NUM = "+graph.size()+";");
        
        output_graph(out, true);

        // output keyword lists
        if((options & GLOBAL_KEYWORD_TABLE) == GLOBAL_KEYWORD_TABLE)
            out.print("    public");
        else
            out.print("    private");
        out.println(" static "+struct_name+"[] "+wordlist_name+" = new "+struct_name+"["+M+"];\n");
        out.println("    private static void init()\n    {");
        for(int i=0; i<keywords.size(); i++)
        {
            Vector v = (Vector)keywords.elementAt(i);
            String current_key = (String)v.firstElement();

            //System.out.println(current_key+"--->"+hash(current_key));

            out.print("\t"+wordlist_name+"["+i+"] = new "+struct_name+"(\""+current_key+"\"");
            for(int j=1; j<n_member; j++)
                out.print(", "+(String)v.elementAt(j));
            out.println(");");
        }
        out.println("    }\n");

        // output G_index()
        out.println("    private static int G_index(int n)\n    {\n\tint i;\n\n\tfor(i=0; i<MAX_NODE_NUM; i++)\n\t    if(node[i] == n)\n\t\treturn i;\n\n\treturn -1;\n    }\n");

        // output hash()
        out.println("    public static int "+hash_func_name+"(String key)\n    {\n\tint i, leng=key.length(), f1=0, f2=0, n1=-1, n2=-1, t1, t2;\n\n\tfor(i=0; i<leng; i++)\n\t{\n\t    char c = key.charAt(i);\n\t    if(c < MIN_CHAR_VAL || c > MAX_CHAR_VAL)\n\t\treturn -1;\n\t    t1 = T1[i][key.charAt(i)-MIN_CHAR_VAL];\n\t    t2 = T2[i][key.charAt(i)-MIN_CHAR_VAL];\n\t    if(t1 == -1 || t2 == -1)\n\t\treturn -1;\n\t    f1 += t1;\n\t    f2 += t2;\n\t}\n\tf1 %= MAX_GRAPH_NODE_VAL;\n\tf2 %= MAX_GRAPH_NODE_VAL;\n\tn1 = G_index(f1);\n\tn2 = G_index(f2);\n\treturn (G[n1]+G[n2]) % TOTAL_KEYWORDS;\n    }\n");

        // output in_word_set()
        out.print("    public static ");
        if((options & RETURN_BOOLEAN) == RETURN_BOOLEAN)
            out.print("boolean");
        else
            out.print(struct_name);
        out.print(" "+lookup_func_name + "(String key)\n    {\n\tint len=key.length();\n\n\tif(len <= MAX_WORD_LENG && len >= MIN_WORD_LENG)\n\t{\n\t    int ind = "+hash_func_name+"(key);\n\n\t    if(ind <= MAX_HASH_VAL && ind >= 0)\n\t    {\n\t\t"+struct_name+" rec = "+wordlist_name+"[ind];\n\n\t\tif(rec");
        if(sym_exp != null)
            out.print("."+(String)((Vector)sym_exp.elementAt(1)).elementAt(1)+"()");
        out.print(".compareTo(key) == 0)\n\t\t    return ");
        if((options & RETURN_BOOLEAN) != RETURN_BOOLEAN)
            out.println("rec;\n\t    }\n\t}\n\treturn null;\n    }\n");
        else
            out.println("true;\n\t    }\n\t}\n\treturn false;\n    }\n");

        // output the 3rd section
        if(has_verba_func)
            out.print(verba_func);

        out.println("}");
    }

    // send output as C program
    private static void send_c_output(PrintWriter out)
    {
        String struct_name = new String("char *");

        if(sym_exp != null)
            struct_name = (String)sym_exp.elementAt(0);

        out.print(verba);

        if((options & PRINT_STRUCTURE) == PRINT_STRUCTURE)
        {
            // contradictory
            if(sym_exp == null)
            {
                System.err.println("Error: Can't apply `-printstructure' option to a string-only keyword list.");
                System.exit(-1);
            }

            // print the structure definition
            out.println("struct "+struct_name+"\n{");
            // print members
            for(int i=1; i<sym_exp.size(); i++)
            {
                Vector member = (Vector)sym_exp.elementAt(i);
                out.println("    "+(String)member.elementAt(0)+" "+(String)member.elementAt(1)+";");
            }
            out.println("};\n");
        }

        out.println("/* for strlen, strchr */\n#include <string.h>");
        out.println("#define TOTAL_KEYWORDS "+M);
        out.println("#define MIN_HASH_VAL 0");
        out.println("#define MAX_HASH_VAL "+(M-1));

        char c;
        if((c = GGTable.getMinCharVal()) == '\'')
            out.println("#define MIN_CHAR_VAL '\\''");
        else
            out.println("#define MIN_CHAR_VAL '"+c+"'");
        if((c = GGTable.getMaxCharVal()) == '\'')
            out.println("#define MAX_CHAR_VAL '\\''");
        else
            out.println("#define MAX_CHAR_VAL '"+c+"'");

        out.println("#define MAX_WORD_LENG "+longest);
        out.println("#define MIN_WORD_LENG "+shortest);
        out.println("#define MAX_TABLE_HEIGHT "+GGTable.getNHeads());
        out.println("#define MAX_GRAPH_NODE_VAL "+N);
        T1.output_table("T1", out, false);
        T2.output_table("T2", out, false);

        out.println("#define MAX_NODE_NUM "+graph.size());
        output_graph(out, false);

        // only key is there
        if(sym_exp == null)
        {
            if((options & GLOBAL_KEYWORD_TABLE) != GLOBAL_KEYWORD_TABLE)
                out.print("static ");
            out.println("char *"+wordlist_name+"[] = \n{");
            for(int i=0; i<keywords.size(); i++)
            {
                String current_key = new String();
                Vector element = (Vector)keywords.elementAt(i);
                current_key = (String)element.firstElement();
                //System.out.println(current_key+"--->"+hash(current_key));
                out.print("\""+current_key+"\",");
                if((i % 5) == 4)
                    out.print("\n    ");
            }
            output.println("};\n");
        }
        else
        {
            // output keyword lists
            if((options & GLOBAL_KEYWORD_TABLE) != GLOBAL_KEYWORD_TABLE)
                out.print("static ");
            out.println("struct "+(String)sym_exp.elementAt(0)+" "+wordlist_name+"[] =\n{");
            for(int i=0; i<keywords.size(); i++)
            {
                String current_key = new String();
                Vector element = (Vector)keywords.elementAt(i);
                current_key = (String)element.firstElement();
                //System.out.println(current_key+"--->"+hash(current_key));
                out.print("    {\""+current_key+"\"");
                for(int j=1; j<n_member; j++)
                    out.print(", "+(String)element.elementAt(j));
                out.println("},");
            }
            output.println("};\n");
        }

        // output G_index()
        if((options & GNUC_STYLE) == GNUC_STYLE)
            out.println("#ifdef __GNUC__\ninline\n#endif");
        out.println("static unsigned int\nG_index(register long n)");
        out.println("{\n    register int i;\n\n    for(i=0; i<MAX_NODE_NUM; i++)\n\tif(node[i] == n)\n\t    return i;\n\n    return -1;\n}\n");

        // output hash()
        if((options & GNUC_STYLE) == GNUC_STYLE)
            out.println("#ifdef __GNUC__\ninline\n#endif");
        out.println("unsigned int\n"+hash_func_name+"(register char *key)");
        out.println("{\n    register int i, leng=strlen(key);\n    long f1=0, f2=0, n1=-1, n2=-1, t1, t2;\n\n    for(i=0; i<leng; i++)\n    {\n\tchar c = key[i];\n\tif(c < MIN_CHAR_VAL || c > MAX_CHAR_VAL)\n\t    return -1;\n\tt1 = T1[i][key[i]-MIN_CHAR_VAL], t2 = T2[i][key[i]-MIN_CHAR_VAL];\n\tif(t1 == -1 || t2 == -1)\n\t    return -1;\n\tf1 += t1, f2 += t2;\n    }\n    f1 %= MAX_GRAPH_NODE_VAL, f2 %= MAX_GRAPH_NODE_VAL;\n    n1 = G_index(f1), n2 = G_index(f2);\n    return (G[n1]+G[n2]) % TOTAL_KEYWORDS;\n}\n");

        // output the in_word_set()
        if((options & GNUC_STYLE) == GNUC_STYLE)
            out.println("#ifdef __GNUC__\ninline\n#endif");
        if((options & RETURN_BOOLEAN) != RETURN_BOOLEAN)
        {
            if(sym_exp == null)
                out.println("char *");
            else
                out.println("struct "+(String)sym_exp.elementAt(0) + " *");
        }
        else
            out.println("int /* actually boolean */");
        out.println(lookup_func_name + "(register char *str)");
        out.print("{\n    int len = strlen(str);\n    if(len <= MAX_WORD_LENG && len >= MIN_WORD_LENG)\n    {\n\tregister unsigned int key = "+hash_func_name+"(str);\n\n\tif (key <= MAX_HASH_VAL && key >= 0)\n\t{\n\t    register char *s = "+wordlist_name+"[key]");
        if(sym_exp != null)
            out.print("."+(String)((Vector)sym_exp.elementAt(1)).elementAt(1));
        out.print(";\n\n\t    if (*s == *str && !strcmp (str + 1, s + 1))\n\t\treturn ");
        if((options & RETURN_BOOLEAN) != RETURN_BOOLEAN)
        {
            if(sym_exp != null)
                out.print("&");
            out.println(wordlist_name+"[key];\n\t}\n    }\n    return 0;\n}");
        }
        else
            out.println("1;\n\t}\n    }\n    return 0;\n}");

        // output the 3rd section
        if(has_verba_func)
            out.print("\n"+verba_func);
    }

    // recognize escape sequences
    private static char escape(char c)
    {
        switch(c)
        {
            case 'b':
                return '\b';
            case 't':
                return '\t';
            case 'n':
                return '\n';
            case 'r':
                return '\r';
            case 'f':
                return '\f';
            case '\"': // although I can merge them into default:, they are somewhat special
            case '\'':
            case '\\':
            case ' ':
            case ',':  // they are somewhat special
            default: // escaping a normal character is the char itself
                return c;
        }
    }

    // parse a keyword line
    // since I need to deal with escape sequences, StringTokenizer is not used
    // @param line the parsing keyword line
    // @return the value to be hashed for the key (user assigned or default)
    private static String parse_line(String line)
    {
        Vector vec = new Vector();
        String str = new String();
        int n_item = 0;
        int start;

        for(int i=0; i<line.length(); i++)
        {
            char c = line.charAt(i);

            if(c == '\\')
                str = str.concat((new Character(escape(line.charAt(++i)))).toString());
            else if(c == ',')
            {
                n_item++;
                vec.addElement(str);
                str = new String();
                continue;
            }
            else if(Character.isWhitespace(c))
            {}
            else
                str = str.concat((new Character(c)).toString());
        }

        if(n_item == n_member) // has a ',' ending
        {}
        else if(n_item == (n_member - 1)) // doesn't have a ',' ending
            vec.addElement(str);
        else
        {
            System.err.println("Error: number of fields in a keyword line is incorrect.");
            System.exit(-1);
        }
        keywords.addElement(vec);
        return (String)vec.firstElement();
    }

    // print usage information
    private static void usage(String args[])
    {
        System.err.println("java perfecthash.GGPerf <options> input_file_name output_file_name");
        System.err.println("where options are:");
        System.err.println("\t-C=<float>\t\tset C value to <float>");
        System.err.println("\t-java\t\t\toutput java code");
        System.err.println("\t-ansic\t\t\toutput ANSI C code");
        System.err.println("\t-gnuc\t\t\toutput GCC code");
        System.err.println("\t-printstructure\t\tprint structure/class");
        System.err.println("\t-global\t\t\tmake keyword array global");
        System.err.println("\t-returnboolean\t\tin_word_set() returns boolean");
        System.err.println("\t-hash_name=<name>\tuse <name> instead of default \"hash\"");
        System.err.println("\t-lookup_name=<name>\tuse <name> instead of default \"in_word_set\"");
        System.err.println("\t-wordlist_name=<name>\tuse <name> instead of default \"wordlist\"");
        System.exit(-1);
    }

    // process options from command line or AWT interface
    private static void options(String args[])
    {
        for(int i=0; i<args.length; i++)
        {
            if(args[i].charAt(0) == '-')
            {
                if(args[i].startsWith("-C="))
                    C = new Double(args[i].substring(3)).doubleValue();
                else if(args[i].startsWith("-hash_name="))
                    hash_func_name = args[i].substring(11);
                else if(args[i].startsWith("-lookup_name="))
                    lookup_func_name = args[i].substring(13);
                else if(args[i].startsWith("-wordlist_name="))
                    wordlist_name = args[i].substring(15);
                else if(args[i].compareTo("-returnboolean") == 0)
                    options |= RETURN_BOOLEAN;
                else if(args[i].compareTo("-java") == 0)
                    options |= JAVA_STYLE;
                else if(args[i].compareTo("-ansic") == 0)
                    options |= ANSIC_STYLE;
                else if(args[i].compareTo("-gnuc") == 0)
                    options |= GNUC_STYLE;
                else if(args[i].compareTo("-printstructure") == 0)
                    options |= PRINT_STRUCTURE;
                else if(args[i].compareTo("-global") == 0)
                    options |= GLOBAL_KEYWORD_TABLE;
                else
                    usage(args);
            }
            else
            {
                if(input_file_name.length() == 0)
                    input_file_name = args[i];
                else if(output_file_name.length() == 0)
                {
                    output_file_name = args[i];
                    if(((options & JAVA_STYLE) == JAVA_STYLE) &&
                       (output_file_name.indexOf('.') == -1))
                    {
                        output_file_name = output_file_name.concat(".java");
                        System.err.println("Extension name of output file defaults to `java'.");
                    }
                    else if(output_file_name.indexOf('.') == -1)
                    {
                        output_file_name = output_file_name.concat(".c");
                        System.err.println("Extension name of output file defaults to `c'.");
                    }
                }
                else
                    usage(args);
            }
        }

        if(((options & JAVA_STYLE)== JAVA_STYLE) &&
           (((options & GNUC_STYLE) == GNUC_STYLE) ||
               ((options & ANSIC_STYLE) == ANSIC_STYLE)))
        {
            System.err.println("Option: can't select Java style and C style in the meantime.");
            System.exit(-1);
        }

        if((input_file_name.length() == 0) ||
           (output_file_name.length() == 0))
            usage(args);
    }

    /**
      * The main driver.
      * @param args the option list:
      * <TABLE>
      * <TR><TD>-C=<EM>float number</EM></TD><TD>set <EM>C</EM> to the <EM>float number</EM>, default value is 5</TD></TR>
      * <TR valign="top"><TD>-hash_name=<EM>name</EM></TD><TD>set hash function name to <EM>name</EM>, default value is `hash'</TD></TR>
      * <TR valign="top"><TD>-lookup_name=<EM>name</EM></TD><TD>set lookup function name to <EM>name</EM>, default value is `in_word_set'</TD></TR>
      * <TR valign="top"><TD>-wordlist_name=<EM>name</EM></TD><TD>set word list name to <EM>name</EM>, default value is `wordlist'</TD></TR>
      * <TR valign="top"><TD>-returnboolean</TD><TD>lookup function returns boolean instead of hash values</TD></TR>
      * <TR valign="top"><TD>-java</TD><TD>output Java program instead of C program</TD></TR>
      * <TR valign="top"><TD>-ansic</TD><TD>output ANSI C program, this is the default</TD></TR>
      * <TR valign="top"><TD>-gnuc</TD><TD>output GNU C program</TD></TR>
      * <TR valign="top"><TD>-printstructure</TD><TD>for composite hash values, print structre declaration in C output, or class declaration in Java output</TD></TR>
      * <TR valign="top"><TD>-global</TD><TD>output wordlist so that it could be global accessed</TD></TR>
      * </TABLE>
      */
    public static void main(String args[])
    {
        String current_key = new String();

        options(args);

        // open files
        // input
        try
        {
            input = new RandomAccessFile(input_file_name, "r");
        }
        catch(IOException e)
        {
            System.err.println("Unable to open input file `"+input_file_name+"'.");
            System.exit(-1);
        }
        // output
        try
        {
            output = new PrintWriter(new FileOutputStream(output_file_name, false), true);
        }
        catch(IOException e)
        {
            System.err.println("Unable to open output file `"+output_file_name+"'.");
            System.exit(-1);
        }

        //System.out.println(C);
        //System.out.print("debug["+debug.length+"]=");
        //for(int i=0; i<debug.length; i++)
        //    System.out.print(debug[i]+" ");
        //System.out.println();

        // the 1st processing pass,
        // Tasks: 1. know how many items are there in the file (=> M);
        //        2. know the longest length of an item (=> longest);

        read_declaration_part();
        //System.out.println("n_member: "+n_member);
        while(true)
        {
            String current_line = new String();

            try
            {
                // have met End Of File
                // According to source code of java.io.RandomAccessFile
                // a `null' return value means EOF
                if((current_line = input.readLine()) == null)
                    break;
            }
            catch (IOException e)
            {
                System.err.println("Error in reading file.");
                System.exit(-1);
            }

            if((current_line.charAt(0) == '%') &&
               (current_line.charAt(1) == '%'))
            {
                has_verba_func = true;
                break;
            }

            // # leads comments
            if(current_line.charAt(0) != '#')
            {
                // get the current parsing key
                current_key = parse_line(current_line);
                // add the counter
                M++;

                // determine `longest' and `shortest'
                // how long are the longest & shortest key
                if(current_key.length() > longest)
                    longest = current_key.length();
                if(current_key.length() < shortest)
                    shortest = current_key.length();

                // determine `min_char_val' and `max_char_val'
                // the smallest & largest ASCII value in all keys
                for(int i=0; i<current_key.length(); i++)
                {
                    // use a local variable to decrease amount of method calls
                    // thus increase the speed
                    char c = current_key.charAt(i);
                    if(c < min_char_val)
                        min_char_val = c;
                    if(c > max_char_val)
                        max_char_val = c;
                }
            }
        }

        // there is the 3rd section
        if(has_verba_func)
        {
            while(true)
            {
                String current_line = new String();

                try
                {
                    // have met End Of File
                    // According to source code of java.io.RandomAccessFile
                    // a `null' return value means EOF
                    if((current_line = input.readLine()) == null)
                        break;
                }
                catch (IOException e)
                {
                    System.err.println("Error in reading file.");
                    System.exit(-1);
                }

                verba_func = verba_func.concat(current_line+"\n");
            }
            //System.out.print(verba_func);
        }

        // calculate pertinent constants
        N = (long)Math.ceil(C*M);
        //System.out.println("N="+N+"\tM="+M+"\tlongest="+longest);
        //System.out.println("min_char_val="+min_char_val+"\tmax_char_val="+max_char_val);
        //for(int i=0; i<keywords.size(); i++)
        //{
        //    for(int j=0; j<n_member; j++)
        //        System.out.print((String)((Vector)keywords.elementAt(i)).elementAt(j)+",");
        //    System.out.println(" ==> " + i);
        //}

        // the 2nd processing pass: the principal pass
        // Tasks: 1. generate the table T1 and T2;
        //        2. generate the graph
        //        3. assure that the graph is acyclic

        // the reason why I have to re-read the input is that
        // Java is memory-eating.  Saving memory may be much better
        // than reading input only once.

        // setup the random generator for tables
        GGTable.setRandomGenerator(new GGRandom(new Date().getTime(), N));
        GGTable.setNHeads(longest);
        GGTable.setMinCharVal(min_char_val);
        GGTable.setMaxCharVal(max_char_val);

        while(true)
        {
            boolean graph_style = ACYCLIC;

            T1 = new GGTable();
            T2 = new GGTable();
            graph = new Vector();

            for(int i=0; i<keywords.size(); i++)
            {
                current_key = (String)((Vector)keywords.elementAt(i)).firstElement();
                // insert current key to table T1,T2 & graph
                graph_style = table_assignment(current_key, i);
                // no need to insert followed keys, just
                // have a break and repeat the whole process
                if(graph_style == CYCLIC)
                    break;
            }

            // After all, if the graph is still acyclic, we are done
            if(graph_style == ACYCLIC)
                break;
        }
        
        // debugging
        //T1.print_table();
        //T2.print_table();
        //print_graph();

        for(int i=0; i<graph.size(); i++)
            assign_g_value((GGNode)graph.elementAt(i), 0);

        //print_graph();

        if((options & JAVA_STYLE) == JAVA_STYLE)
            send_java_output(output);
        else
            send_c_output(output);

        output.close();
    }
}
