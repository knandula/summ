/*    Provides a graph node structure for intermediate graph
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

import java.lang.Long;
import java.util.Enumeration;
import java.util.Hashtable;

/**
  * GGNode is an object class representing nodes in GGPerf's
  * intermediate graph.
  * @author Jiejun KONG
  */
public // merely for utilizing `javadoc'
class GGNode
{
    /** Label of the node. */
    protected long vertex;
    /** <EM>G</EM> value of the node. */
    protected long g_value;
    /** Adjancency list of the node.
      * All adjacent nodes with correspondent edges are stored in this list.
      * For any undirected graph, if A<->B, then A appears in B's
      * adjacency list while B is in A's adjacency list.
      */
    protected Hashtable adjacency;
    /** Flag indicating whether the node has been visited during the
      * current cyclicity checking process.
      * Since there are indefinite number of passes to check cyclicity of
      * a graph. I can't define a flag for each pass, therefore this flag
      * needs to be cleared before every checking pass to decide reachability.
      */
    protected boolean visited;

    // Constructor
    /**
      * Creates a new node labelled as the argument in the intermediate graph.
      * @param v The label.
      */
    protected GGNode(long v)
    {
        vertex = v;
        g_value = -1;
        adjacency = new Hashtable();
        visited = false;
    }

    // Accessors & Mutators
    // for `vertex'
    /**
      * Return the label of the node.
      * @return The label as a long value.
      */
    protected long getVertex()
    {
        return vertex;
    }
    /**
      * Set the label of this node to be the argument.
      * @param v The new label
      */
    protected void setVertex(long v)
    {
        vertex = v;
    }

    // for `g_value'
    /**
      * Return the <EM>G</EM> value of the node.
      * @return The <EM>G</EM> value as a long value.
      */
    protected long getGValue()
    {
        return g_value;
    }
    /**
      * Set the <EM>G</EM> value of this node to be the argument.
      * @param g The new <EM>G</EM> value.
      */
    protected void setGValue(long g)
    {
        g_value = g;
    }

    // for `adjacency'
    /**
      * Return all adjacent nodes of this node.
      * @return All adjacent nodes of this node as a java.util.Enumeration.
      */
    protected Enumeration getAdjacencyNodes()
    {
        return adjacency.keys();
    }
    /**
      * Return the edge between this node and the argument which maybe an adjacent node.
      * @param node The adjacent node.
      * @return the edge as a long value.
      */
    protected long getAdjacencyEdge(GGNode node)
    {
        return ((Long)adjacency.get(node)).longValue();
    }
    /**
      * Add an adjacent node with its edge to the adjacency list of this node.
      * @param node The adjacent node.
      * @param edge The edge between this node and the adjacent node.
      */
    protected void addAdjacency(GGNode node, Long edge)
    {
        adjacency.put(node, edge);
    }

    // for `visited'
    /**
      * Return the current status of the node.
      * @return the status as a boolean value. True means having been visited.
      */
    protected boolean getVisited()
    {
        return visited;
    }
    /**
      * During a cyclicity checking process, set the status to be the argument.
      * @param b the status. True mean having been visited.
      */
    protected void setVisited(boolean b)
    {
        visited = b;
    }

    /**
      * A debugging routine.
      * It is not an API function.
      * It prints the node, and all of its adjacent nodes along with edges.
      */
    protected void print_node()
    {
        System.out.print("Node "+getVertex()+" (G="+getGValue()+"):");

        Enumeration e = getAdjacencyNodes();
        while(e.hasMoreElements())
        {
            GGNode adj = (GGNode)e.nextElement();
            System.out.print("--"+getAdjacencyEdge(adj)+"-->"+adj.getVertex());
        }
        System.out.println();
    }
}
