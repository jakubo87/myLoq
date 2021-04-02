#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include "../include/hwloc-test.h"

using VType = std::string;
using Index = unsigned int;
using Mem   = unsigned long int;
using SIMD  = unsigned int;
using RAND  = std::string;
using VID   = unsigned long int; //eventually tags... for identifying vertices across  graphs
using Depth = int;

using EType = std::string;
using EID    = unsigned long int;


static Depth max_depth = 0; //
const double NOPATH = 10000;

//not accessible via std::get<type>
struct Vertex {
  VType      type; 
  Index      index;
  Mem        mem_cap;
  SIMD       simd;
  VID        vid; //global vertex descriptor to make graphs copyable and still reliably refer to the same vertex... overhead due to query, but meh..
  Depth      depth;
};

struct Edge {
  EType      label;
  double     weight;
  EID        eid;
};

using graph_t = boost::adjacency_list<
                     boost::vecS, //  out-edge container selector                  
                     boost::vecS, //  Vector container selector
                     boost::bidirectionalS, //sets bidirectional edge access
                     Vertex,                //vertex property
                     Edge>;                  //edge property (see bundled properties)
// the following settings seem to contradict some internal details of the adjacency list when using out_edges (hypothesis)
                     //boost::no_property,    //graph property 
                     //boost::multisetS>;     //edge container selector... probably the right one.. but might need major changes concerning VD

using ndgraph_t = boost::adjacency_list<boost::vecS,  boost::vecS, boost::undirectedS, Vertex, Edge>;


//graph internally used descriptors for the main graph, probably not derived types, can become invalidated
using ED = typename boost::graph_traits<graph_t>::edge_descriptor;
using VD = typename boost::graph_traits<graph_t>::vertex_descriptor;

//graph_t init_graph(const hwloc_topology_t & t);
graph_t init_graph();

std::string obj_type_toString(hwloc_obj_t & obj);

template<typename G, typename V>
using Distance = std::function<double(V,V,const G&)>;





//get gid
template<typename G, typename V>
inline
decltype(auto) get_vid(const G& g, V vd){ //const graph should prevent writes...
  return boost::get(&Vertex::vid, g, vd);
}

template<typename G, typename E>
inline
decltype(auto) get_eid(const G& g, E ed){ //const graph should prevent writes...
  return boost::get(&Edge::eid, g, ed);
}

////get key
//template<typename G, typename EVID>
//inline
//decltype(auto) get_key(const G& g, EVID evid){ //const graph should prevent writes...
//	std::enable_if(std::is_same<EV, boost::graph_traits<G>::vertex_descriptor>) return boost::get(boost::vertex_index, g, evid);
//	std::enable_if(std::is_same<EV, boost::graph_traits<G>::edge_descriptor>) return //TODO boost::get(boost::edge, g, evid);
//}



static EID maxEID=0;
static VID maxVID=0;

//TODO closest objects without dijkstra
//obtaining descriptor from gid possibly though scanning all possible keys as not access to values possible

//actually gets next id, not just max, but next_id would also not be satisfactory...
template<typename G>
EID getmax_eid(G& g){
  return maxEID++;
}
template<typename G>
VID getmax_vid(G& g){
  return maxVID++;
}

template<typename G, typename V>
double distance(V vd1, V vd2, const G& g, Distance<G,V> func){
  return func(g,vd1,vd2);
}



//returns the vertex descriptor to the group vertex
template<typename G, typename V>
V make_group(const std::string& name, const std::vector<V>& cont, G& g){
  //insert group vertex into graph
  auto v = add_vertex(g);
  put(&Vertex::type, g, v, name);
  for (auto & vd : cont){ //for all vertex descriptors
    //add a connection to the group members
    auto e = boost::add_edge(v,vd,g).first;
    put(&Edge::label, g, e, "member");
  }
  std::cout << "Group: " << name << " has been created" << std::endl;
  return v;
}

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
  res.push_back(p); //reverses predecessor map into path
  return res;
}


///enumerating combinatorics
std::vector<std::vector<int>>
comb(const int k, const std::vector<int>& vec);


template<typename G, typename V>
std::vector<std::pair<V,double>>
find_closest_to(const G& g,
                Distance<G,V> dist, //distance function check if this or the dijkstra find!)
                VType type, V start){
  //get all VDs of specified type
  auto vds = get_vds(g,std::make_pair(&Vertex::type,type));
  std::vector<std::pair<V,double>> res(vds.size());
 // struct less_by_dist{
 //   bool operator(const auto& a, const auto& b) const { return a.second < b.second };
 // }
  
  std::transform(vds.begin(), vds.end(), res.begin(),[&](const auto& vd)
    {return  std::make_pair(vd, dijkstra_spaths(g,start,dist)[vd]);});           // <---- here!

  std::sort(res.begin(),res.end(),[&](const auto& a, const auto& b) { return a.second < b.second ; } );
  return res;
}




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

template<typename G, typename V>
int count_obj(const G& g, V vd){
  unsigned int count=0;
  bfs_counter bfsc{count};
  boost::breadth_first_search(g,getkey(g,vd), boost::visitor(bfsc));
  std::cout << "counted " <<  count << std::endl;
  return count;
}

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


//perhaps allow std library algorithms for users to iterate over graphs...? nice to have
template< typename G, typename T,typename V>
T accumulate (const G& g,T Vertex::* mptr , V st_vd){
  T value=0;
  bfs_accumulator<T> bfsa(value, mptr);
  boost::breadth_first_search(g, getkey(g,st_vd), boost::visitor(bfsa));
  std::cout << "accumulated: " << value << std::endl; 
  return value;
}

//TODO list:
//partitioning balanced/evenly or with respect to distances in cores
//Idea
//1. make mst in separate temp graph
//2. remove last k-1 edges
//3. rattle k-means style until balance is satisfied or break after x iterations




//########QUERYING 
//check a tuple for its properties (type based)
// ending case for recursion
template <typename G, typename V>
constexpr bool check_props(G && g, V&& vd) {
  return true;
}
//Recursive case
template <typename G, typename V, typename P, typename... Args>
constexpr bool check_props(G&& g, V&& vd, P&& p, Args... args) {
  if (boost::get(p.first, g,vd ) != p.second) return false; //somehow trying to squeeze out the value in order to compare it
  return check_props(g, vd, args...);
}

//query the vd from properties
template<typename G, typename... Args>
constexpr auto get_vds(const G& g, Args&& ... args){
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  std::vector<V> res;
  auto range = boost::vertices(g);
  std::for_each(range.first, range.second, [&](const auto & vd)
     {
      if(check_props(g,vd,args...))
        res.push_back(vd);
     });
  return res;
} 



//query the ed for the edge from va to vb with label (std::string)
template <typename G, typename VD>
//std::vector<typename boost::graph_traits<G>::edge_descriptor> 
auto
get_ed(const G& g, VD va, VD vb, const EType& label){
  using ED = typename boost::graph_traits<G>::edge_descriptor;
  std::vector<ED> res;
  auto range = boost::out_edges(va,g);
  std::for_each(range.first, range.second,[&](const auto& ed)
      {
        if( boost::target(ed,g)==vb &&
            boost::get(&Edge::label, g, ed)==label)
          res.push_back(ed);
      }
  );
  return res;
}

//more general graph querying properties
//same as above, only not only for vertices but also edges in the form of query(T*, args) meaning a property and one ore more constraints (like larger than x, smaller than y...  and then when the next T* is found the process is repeated for the next predicates until the query ends also get will be utilised instead of makeing tuples


//next step: query these properties for each vertex/edge on a pattern search

//filtered graph to query stuff

template<typename T, typename G>
struct VPred {
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  //ctor
 // VPred(const G& g, const T Vertex::* mptr, const T& value): g_(&g), mptr_(mptr), value_(value) {}
  bool operator()(const E) const {return true;}
  bool operator()(const V v) const {
    return boost::get(mptr_,*g_, v)== value_;
  }
  G* g_;
  T Vertex::* mptr_;
  T value_;
};

template<typename T, typename G>
struct EPred {
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  //ctor
  EPred(const G& g, const T Edge::* mptr, const T& value): g_(&g), mptr_(mptr), value_(value) {}
  bool operator()(const V) const {return true;}
  bool operator()(const E e) const {
    return boost::get(mptr_,*g_, e)= value_;
  }
  G* g_;
  T Edge::* mptr_;
  T value_;
};




//some getter and setter for use with global indexed VD and ED for cross graph-comminucation (for internal use, use the boost variant with their internal types 
//setter
template<typename T, typename G, typename E>
inline void put(T Edge::* mptr, G& g, E ed, T value){
  boost::put(mptr, g, ed, value);
}

template<typename T, typename G, typename V>
inline void put(T Vertex::* mptr, G& g, V vd, T value){
  boost::put(mptr, g, vd, value);
}

//getter
template<typename T, typename G, typename V>
inline
decltype(auto) get(T Vertex::* mptr, G& g, V vd){
  return boost::get(mptr, g, vd);
}
template<typename T, typename G, typename E>
inline
decltype(auto) get(T Edge::* mptr, G& g, E ed){
  return boost::get(mptr, g, ed);
}
//const types
template<typename T, typename G, typename V>
inline
decltype(auto) get(T Vertex::* mptr, const G& g, V vd){
  return boost::get(mptr, g, vd);
}
template<typename T, typename G, typename E>
inline
decltype(auto) get(T Edge::* mptr, const G& g, E ed){
  return boost::get(mptr, g, ed);
}

//add vertex/edge, yes: adding an object with the property at once is discouraged, but still possible via boost::add_xxxx(...)
template<typename G, typename V>
inline
V add_vertex(G& g) {
  V v = boost::add_vertex(g);
  boost::put(&Vertex::vid, g, v, getmax_vid(g));
  return v; 
}

template<typename G, typename V, typename E>
inline
E add_edge(V va, V vb, G& g){
  E e = boost::add_edge(va, vb, g).first; //in the current graph setup parallel edges are allowed, so .second is never false
  boost::put(&Edge::eid, g, e, getmax_eid(g));
  return e;
}

template<typename G, typename E>
inline void remove_edge(E ed, G& g){
  boost::remove_edge(ed, g); //this invalidates all edge iterators... or it would if the werent indexed!
}

template<typename G, typename V>
inline void remove_vertex(V vd, G& g){
  boost::clear_vertex(vd,g);    //removes all edges connected to the vertex
  boost::remove_vertex(vd,g);   //removes the vertex and invalidates all vertex descriptors larger than vd (and of coursee again all edge descriptors
}


#endif
