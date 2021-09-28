#ifndef ALGO_H
#define ALGO_H

#include "../include/hwloc-test.h"
#include "../include/init_graph.h"
#include <boost/graph/kruskal_min_spanning_tree.hpp>


//Dijkstras shortest distances algorithm
//returns the shortest distances. When no direct edge is available, accumulating distances
template<typename G, typename V> //next step: make the output container a template 
std::vector<double>
dijkstra_spaths(const G& g, V va, Distance<G,V> func){
  using E = typename boost::graph_traits<G>::edge_descriptor; // VD are properties while vd_t are internal vertex descriptor types
  //using vd_t = typename boost::graph_traits<G>::vertex_descriptor;

  std::vector<V> p(num_vertices(g));
  std::vector<double> d(num_vertices(g));

  //###helper function to get the input/key right
  std::function<double(E)> f = [&](const E& ed)
    {
      V va_ = boost::source(ed, g);
      V vb_ = boost::target(ed, g); 
      return func(va_, vb_, g); // by capturing the graph here, you don't need to point to g later
    };
  
    boost::dijkstra_shortest_paths(
      g,  //graph
      va, //source
      boost::weight_map(boost::function_property_map<decltype(f),E,double>(f))
      //comment: do NOT(!!) use the make_function_propertymap() function. it fails to deduce correctly!
      .predecessor_map(boost::make_iterator_property_map(
                            p.begin(), get(boost::vertex_index, g)))
      .distance_map(boost::make_iterator_property_map(
                            d.begin(), get(boost::vertex_index, g)))
  );
  return d;
}

//Dijkstra TODO return value
//prints predeccessors from target to source (potentially for debugging..?)
template<typename G, typename V>
std::vector<V> shortest_path(const G& g, V va, V vb, Distance<G,V> func){
  std::vector<V> d(num_vertices(g));

  using E = typename boost::graph_traits<G>::edge_descriptor;
  //using vd_t = typename boost::graph_traits<G>::vertex_descriptor;
  //###helper function to get the input right
  std::function<double(E)> f = [&](const E& ed){
      V va_ = boost::source(ed, g);
      V vb_ = boost::target(ed, g); 
      return func(va_, vb_, g); // by capturing the graph here, you don't need to point to g later
    };

  boost::dijkstra_shortest_paths(
      g,  //graph
      va, //source
      boost::weight_map(boost::function_property_map<decltype(f),E,double>(f))
      .predecessor_map(boost::make_iterator_property_map(
                      d.begin(), get(boost::vertex_index, g))));

  std::vector<V> res;
  V p = vb; //target 
  while (p != va) //finish
  {
    res.push_back(p);
    p = d[p];
  }
  res.push_back(p);
  std::reverse(res.begin(), res.end()); //reverses predecessor map into path
  return res;
}


//simple algorithm to make k partitions of CUs by removing k-1 longest edges from a Kruskal MST
//in itself MSTs but not balanced  to any workload or number of CUs
template<typename G, typename V>
void k_partitions(G& g, int k,  Distance<G,V> dist){
//using E = typename boost::graph_traits<G>::edge_descriptor;
  using ndE = typename boost::graph_traits<ndgraph_t>::edge_descriptor;
  using ndV = typename boost::graph_traits<ndgraph_t>::vertex_descriptor;
//using V as given by the template param 

  auto fil = filtered_graph(g, &Vertex::type, VType("HWLOC_OBJ_PU"));

  boost::print_graph(fil);

  //keeping it slightly general should a replacement for this algo come
  ndgraph_t partg;
  boost::copy_graph(fil, partg);
  std::vector<ndE> mst_edges;
  
  auto range = boost::vertices(partg);
  //clear all edges
  std::for_each(range.first, range.second, [&](auto v){boost::clear_vertex(v, partg);});

  //make a note on where to find each corresponding vertex
  //this is important to be able to handle filtered graphs, as their vertex descriptors will generally be a subset from the original graph, and since vds are contiguous...
  std::map<ndV,V> orig_v; 
  std::for_each(range.first, range.second, [&](auto v){
    auto gvid = boost::get(&Vertex::vid, partg, v); //get global id
    auto origvd = get_vds(g,std::make_pair(&Vertex::vid, gvid)).front(); //get local vertex descriptor
    orig_v[v] = origvd; //map lokal to original vd  
  });

  //assign weights
  std::for_each(range.first,range.second,[&](auto va){
    std::for_each(range.first, range.second, [&](auto vb){
      if (va<vb){
        auto e = boost::add_edge(va, vb, partg).first;
        boost::put(&Edge::weight, partg, e, dist(orig_v[va], orig_v[vb],g));
      }
    });
  });

  kruskal_minimum_spanning_tree(partg, std::back_inserter(mst_edges), boost::weight_map(boost::get(&Edge::weight, partg)));

  for(int i=0; i < mst_edges.size(); ++i)
    std::cout << partg[mst_edges[i]].weight << std::endl;



  // back in the original graph, add (temporary?) edges that connect a partition
  const int j = mst_edges.size()-k+1;
  for (int i = 0; i<j; ++i){
    auto e = add_edge( //if temporary use boost:: to avoid getting an id
        orig_v[boost::source(mst_edges[i], partg)],
        orig_v[boost::target(mst_edges[i], partg)],
        g)
      .first; 
    boost::put(&Edge::label, g, e, "partition");
  }


}

#endif
