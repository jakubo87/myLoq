#ifndef VISITORS_H
#define VISITORS_H

#include "../include/hwloc-test.h"
#include "../include/init_graph.h"

//##############  VISITORS  ######################
class bfs_counter : public boost::default_bfs_visitor{
public:
    //default ctors
    bfs_counter(unsigned int& num ): num_(num){}

    template <typename Vertex, typename Graph >
    void initialize_vertex(Vertex u, const Graph& g) //what about filtered vertice..?
    {
      num_++;
    }
    unsigned int& num_;
};

template<typename T>
class bfs_accumulator : public boost::default_bfs_visitor{
public:
    //default ctors
    bfs_accumulator(T& sum, T Vertex::* mptr) : sum_(sum), mptr_(mptr) {} 

    template <typename Vertex, typename G >
    void discover_vertex(Vertex u, const G& g)
    {
      sum_+=boost::get(mptr_,g, u);   //g[u].index;
    }
    T& sum_;
    T Vertex::* mptr_;
};


#endif
