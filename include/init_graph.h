#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include "../include/hwloc-test.h"

using VType = std::string;
using Index = unsigned int;
using Mem   = unsigned long int;
using SIMD  = unsigned int;
using RAND  = std::string;

using EType = std::string;




const double NOPATH = 10000;

//not accessible via std::get<type>
struct Vertex {
  VType      type; 
  Index      index;
  Mem        mem_cap;
  SIMD       simd;
};

struct Edge {
  EType label;
  double weight=0;
};


using graph_t = boost::adjacency_list<
                     boost::vecS, //  out-edge container selector                  
                     boost::vecS, //  Vector container selector
                     boost::bidirectionalS, //sets bidirectional edge access
                     Vertex,                //vertex property
                     Edge>;                  //edge property (see bundled properties)
// the following settings seem to contradict some internal details of the adjacency list when using out_edges (hypothesis)
                     //boost::no_property,    //graph property 
                     //boost::multisetS>;     //edge container selector... again? 

using VD = graph_t::vertex_descriptor;
using ED = graph_t::edge_descriptor;

graph_t init_graph(const hwloc_topology_t & t);

std::string obj_type_toString(hwloc_obj_t & obj);

template<typename G>
using Distance = std::function<double(VD,VD,const G&)>;


//the querying interfaces will need a unified version. possibly with strongly typed parameters
//std::vector<VD> get_vds(const graph_t& g, const VType& t, Index i); deprecated

std::vector<ED> get_ed(const graph_t& g, VD va, VD vb, const EType&);

const EType& get_edge_label(const graph_t& g, const ED& ed);

//returns the vertex descriptor to the group vertex
VD make_group(const std::string& name, const std::vector<VD>& cont, graph_t& g);


//Dijkstras shortest distances algorithm
//returns the shortest distances. When no direct edge is available, accumulating distances
template<typename G> //next step: make the output container a template 
std::vector<double>
dijkstra_spaths(const G& g, VD va, Distance<G> func){
  using ED = typename boost::graph_traits<G>::edge_descriptor;
  using VD = typename boost::graph_traits<G>::vertex_descriptor;

  std::vector<VD> p(num_vertices(g));
  std::vector<double> d(num_vertices(g));

  //###helper function to get the input right
  std::function<double(ED)> f = [&](const ED& ed)
    {
      auto va = boost::source(ed, g);
      auto vb = boost::target(ed, g); 
      return func(va, vb, g); // by capturing the graph here, you don't need to point to g later
    };

  boost::dijkstra_shortest_paths(
      g,  //graph
      va, //source
      boost::weight_map(boost::function_property_map<decltype(f),ED,double>(f))
      //comment: do NOT(!!) use the make_function_propertymap() function. it fails to deduce correctly!
      .predecessor_map(boost::make_iterator_property_map(
                            p.begin(), get(boost::vertex_index, g)))
      .distance_map(boost::make_iterator_property_map(
                            d.begin(),get(boost::vertex_index, g)))
  );
  return d;
}

//for debugging..? prints predecessors...
std::vector<VD> shortest_path(const graph_t& g, VD va, VD vb, std::function<double(VD,VD,const graph_t&)> func);



///enumerating combinatorics
std::vector<std::vector<int>>
comb(const int k, const std::vector<int>& vec);

std::vector<std::pair<VD,double>>
find_closest_to(const graph_t& g,
                std::function<double(VD,VD,const graph_t&)> dist, //distance function (TODO check if this or the dijkstra find!)
                VType type, VD start);





template<typename T>
class bfs_counter : public boost::default_bfs_visitor{
public:
    //default ctors
    template <typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g)
    {
      ++num_;
      std::cout << "heureka i found a new vertex. There are now " << num_ << std::endl;
      std::cout << "its name is " << u << std::endl;
    }
    int num_ = 0;
};

template<typename T>
void count_obj(const graph_t& g){
  bfs_counter<T> bfsc;
  boost::breadth_first_search(g, 8, boost::visitor(bfsc));
  std::cout << "finished after " <<  bfsc.num_ << std::endl; // does not work: result is 0
}

template<typename T>
class bfs_accumulator : public boost::default_bfs_visitor{
public:
    //default ctors
    template <typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g)
    {
      t+=g[u].index;
      std::cout << "heureka i found a new vertex. Its index is " << g[u].index << std::endl;
    }
    T t = 0;
};

//perhaps allow std library algorithms for users to iterate over graphs...? nice to have
template< typename T>
void accumulate (const graph_t& g){
  bfs_accumulator<Index> bfsa;
  boost::breadth_first_search(g, 8, boost::visitor(bfsa));
  std::cout << "finished after " <<  bfsa.t << std::endl; // does not work: result is 0
}

//TODO list:
//partitioning balanced/evenly or with respect to distances in cores





//IMPLEMENTATION


//########QUERYING 
//check a tuple for its properties (type based)
// ending case for recursion
template <typename T>
constexpr bool check_props(T && t) {
  return true;
}
//Recursive case
template <typename T, typename P, typename... Args>
constexpr bool check_props(T&& t, P&& p, Args... args) {
  if (std::get<typename std::remove_reference<P>::type>(t)!=p) return false; //somehow trying to squeeze out the value in order to compare it
  return check_props(t, args...);
}

//query the vd from properties
template<typename... Args>
constexpr auto get_vds(const graph_t& g, Args&& ... args){
  std::vector<VD> res;
  auto range = boost::vertices(g);
  std::for_each(range.first, range.second, [&](const auto & vd)
     {
      if(check_props(std::make_tuple(g[vd].type,g[vd].index),args...))
        res.push_back(vd);
     });
  return res;
} 


//more general graph querying properties
//same as above, only not only for vertices but also edges in the form of query(T*, args) meaning a property and one ore more constraints (like larger than x, smaller than y...  and then when the next T* is found the process is repeated for the next predicates until the query ends also get will be utilised instead of makeing tuples


//next step: query these properties for each vertex/edge on a pattern search

//filtered graph to query stuff







// maybe use namespace boost instead....
//setter
template<typename T, typename G, typename EV>
inline void put(T type, G& g, EV ev, T value){
  boost::put(type, g, ev, value);
}

//getter
template<typename T, typename G, typename EV>
inline decltype(auto) get(T type, G& g, EV ev){
  return boost::get(type, g, ev);
}

//const type
template<typename T, typename G, typename EV>
inline decltype(auto) get(T type, const G& g, EV ev){
  return boost::get(type, g, ev);
}

//add vertex/edge, yes: adding an object with the property at once is discouraged, but still possible via boost::add_xxxx(...)
template<typename G>
inline decltype(auto) add_vertex(G& g) {
  return boost::add_vertex(g);
}

template<typename G>
inline decltype(auto) add_edge(VD va, VD vb, G& g) {
  return boost::add_edge(va, vb, g).first; //in the current graph setup parallel edges are allowed, so .second is never false
}

template<typename G>
inline void remove_edge(ED ed, G& g){
  boost::remove_edge(ed, g); //this invalidates all ede iterators
}

template<typename G, typename V>
inline void remove_vertex(V vd, G& g){
  boost::clear_vertex(vd,g);    //removes all edges connected to the vertex
  boost::remove_vertex(vd,g);   //removes the vertex and invalidates all vertex descriptors larger than vd (and of coursee again all edge descriptors
}


#endif
