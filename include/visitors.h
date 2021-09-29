#ifndef VISITORS_H
#define VISITORS_H

#include "../include/hwloc-test.h"
#include "../include/init_graph.h"
#include <boost/graph/breadth_first_search.hpp>

//##############  VISITORS  ######################
class bfs_counter : public boost::default_bfs_visitor{
public:
    //default ctors
    bfs_counter(unsigned int& num ): num_(num){}

    template <typename Vertex, typename Graph >
    void initialize_vertex(Vertex u, const Graph& g)
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
      sum_+=boost::get(mptr_,g, u);
    }
    T& sum_;
    T Vertex::* mptr_;
};

template<typename V>
class bfs_reacher : public boost::default_bfs_visitor{
public:
    //default ctors
    bfs_reacher(bool& res, V target) : res_(res), target_(target){}

    template <typename Vertex, typename G>
    void discover_vertex(Vertex u, const G& g)
    {
      if (u==target_ && end_ == 0) res_ = true;
    }
    template <typename Vertex, typename G>
    void non_tree_edge(Vertex u, const G& g)
    {
      end_ = 1; //disallow "finding" the target later 
    }
    bool end_ = 0;
    bool& res_;
    V target_; 
};


#endif
