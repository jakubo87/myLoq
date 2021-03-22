#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include "../include/hwloc-test.h"

using VType = std::string;
using Index = unsigned int;
using Mem   = unsigned long int;
using SIMD  = unsigned int;
using RAND  = std::string;
using VD    = unsigned long int; //eventually tags... for identifying vertices across  graphs

using EType = std::string;
using ED    = unsigned long int;



const double NOPATH = 10000;

//not accessible via std::get<type>
struct Vertex {
  VType      type; 
  Index      index;
  Mem        mem_cap;
  SIMD       simd;
  VD         vd; //global vertex descriptor to make graphs copyable and still reliably refer to the same vertex... overhead due to query, but meh..
};

struct Edge {
  EType      label;
  double     weight=0;
  ED         ed;
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

//graph internally used descriptors, can become invalidated
template<typename G>
using ed_t = typename boost::graph_traits<G>::edge_descriptor;
template<typename G>
using vd_t = typename boost::graph_traits<G>::vertex_descriptor;

graph_t init_graph(const hwloc_topology_t & t);

std::string obj_type_toString(hwloc_obj_t & obj);

template<typename G>
using Distance = std::function<double(vd_t,vd_t,const G&)>;

//conversion operator
struct getkey{ //get vertex/edge descriptor, the key of the respective graph
  template<typename G, typename EV>
  typename boost::graph_traits<G>::key_type operator()(const G& g, EV ev){return boost::get(&Vertex::vd, g, ev);}
};

template<typename G>
double distance(VD vd1, VD vd2, const graph_t& g, Distance<G> func){
  return func(getkey(g,vd1),getkey(g,vd2), g);
}

//query the ed for the edge from va to vb with label (std::string)
std::vector<ED> get_ed(const graph_t& g, VD va, VD vb, const EType& label){
  std::vector<ED> res;
  auto range = boost::edges(g);
  std::for_each(range.first, range.second,[&](const auto& ed)
      {
        if( boost::source(ed,g)==va && 
            boost::target(ed,g)==vb &&
            g[ed].label==label)
          res.push_back(boost::get(&Edge::ed, g , ed));
      }
  );
  return res;
}


//returns the vertex descriptor to the group vertex
template<typename G>
VD make_group(const std::string& name, const std::vector<VD>& cont, G& g){
  //insert group vertex into graph
  auto v = add_vertex(g);
  put(&Vertex::type, g, v, name);
  for (auto & vd : cont){ //for all vertex descriptors
    //add a connection to the group members
    auto e = boost::add_edge(v,vd,g).first;
    put(&Edge::label, g, e, "member");
  }
  std::cout << "Group: " << name << " has been created" << std::endl;
  //.. containing vertices ...
  return v;
}

//Dijkstras shortest distances algorithm
//returns the shortest distances. When no direct edge is available, accumulating distances
template<typename G> //next step: make the output container a template 
std::vector<double>
dijkstra_spaths(const G& g, VD va, Distance<G> func){
  using ed_t = typename boost::graph_traits<G>::edge_descriptor; // VD are properties while vd_t are internal vertex descriptor types
  using vd_t = typename boost::graph_traits<G>::vertex_descriptor;

  std::vector<VD> p(num_vertices(g));
  std::vector<double> d(num_vertices(g));

  //###helper function to get the input right
  std::function<double(ed_t)> f = [&](const ed_t& ed)
    {
      auto va = boost::get(&Vertex::vd, g, boost::source(ed, g));
      auto vb = boost::target(ed, g); 
      return func(va, vb, g); // by capturing the graph here, you don't need to point to g later
    };
  
  auto st_v = getkey(g,va);

  boost::dijkstra_shortest_paths(
      g,  //graph
      st_v, //source
      boost::weight_map(boost::function_property_map<decltype(f),ed_t,double>(f))
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
template<typename G>
std::vector<VD> shortest_path(const G& g, VD va, VD vb, Distance func){
  std::vector<VD> directions(num_vertices(g));

  using ed_t = typename boost::graph_traits<G>::edge_descriptor;
  using vd_t = typename boost::graph_traits<G>::vertex_descriptor;
  //###helper function to get the input right
  std::function<double(ED)> f = [&](const ed_t& ed){
      vd_t va = boost::source(ed, g);
      vd_t vb = boost::target(ed, g); 
      return func(va, vb, g); // by capturing the graph here, you don't need to point to g later
    };

  boost::dijkstra_shortest_paths(
      g,  //graph
      va, //source
      boost::weight_map(boost::function_property_map<decltype(f),ed_t,double>(f))
      .predecessor_map(boost::make_iterator_property_map(
                      directions.begin(), get(&Vertex::vd, g))));

  std::vector<VD> res;
  VD p = vb; //target 
  while (p != va) //finish
  {
    res.push_back(p);
    p = directions[p];
  }
  res.push_back(p);
  return res;
}


///enumerating combinatorics
std::vector<std::vector<int>>
comb(const int k, const std::vector<int>& vec);

std::vector<std::pair<VD,double>>
find_closest_to(const graph_t& g,
                std::function<double(VD,VD,const graph_t&)> dist, //distance function (TODO check if this or the dijkstra find!)
                VType type, VD start);





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

template<typename G>
int count_obj(const G& g, VD vd){
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



//IMPLEMENTATION


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
  using vd_t = typename boost::graph_traits<G>::vertex_descriptor;
  std::vector<vd_t> res;
  auto range = boost::vertices(g);
  std::for_each(range.first, range.second, [&](const auto & vd)
     {
      if(check_props(g,vd,args...))
        res.push_back(vd);
     });
  return res;
} 




//more general graph querying properties
//same as above, only not only for vertices but also edges in the form of query(T*, args) meaning a property and one ore more constraints (like larger than x, smaller than y...  and then when the next T* is found the process is repeated for the next predicates until the query ends also get will be utilised instead of makeing tuples


//next step: query these properties for each vertex/edge on a pattern search

//filtered graph to query stuff

template<typename T, typename G>
struct VPred {
  using Edge = typename boost::graph_traits<G>::edge_descriptor;
  using Vertex = typename boost::graph_traits<G>::vertex_descriptor;
  //ctor
  VPred(const G* g, const T Vertex::* mptr, const T& value): g_(g), mptr_(mptr), value_(value) {}
  bool operator()(const Edge) const {return true;}
  bool operator()(const Vertex v) const {
    return boost::get(mptr_,*g_, v)< value_;
  }
  const G* g_;
  const T Vertex::* mptr_;
  const T value_;
};





//some getter and setter for use with global indexed VD and ED for cross graph-comminucation (for internal use, use the boost variant with their internal types 
//setter
template<typename T, typename G>
inline void put(T Edge::* mptr, G& g, ED ed, T value){
  boost::put(mptr, g, getkey(g,ed), value);
}

template<typename T, typename G>
inline void put(T Vertex::* mptr, G& g, VD vd, T value){
  boost::put(mptr, g, getkey(g,vd), value);
}

//getter
template<typename T, typename G>
inline
decltype(auto) get(T Vertex::* mptr, G& g, VD vd){
  return boost::get(mptr, g,getkey(g, vd));
}
template<typename T, typename G>
inline
decltype(auto) get(T Edge::* mptr, G& g, ED ed){
  return boost::get(mptr, g, getkey(g,ed));
}
//const types
template<typename T, typename G>
inline
decltype(auto) get(T Vertex::* mptr, const G& g, VD vd){
  return boost::get(mptr, g,getkey(g, vd));
}
template<typename T, typename G>
inline
decltype(auto) get(T Edge::* mptr, const G& g, ED ed){
  return boost::get(mptr, g, getkey(g,ed));
}

//add vertex/edge, yes: adding an object with the property at once is discouraged, but still possible via boost::add_xxxx(...)
template<typename G>
inline
VD add_vertex(G& g) {
  auto v = boost::add_vertex(g);
  auto vd =  std::max(boost::get(&Vertex::vd,g))+1;
  boost::put(&Vertex::vd, g, v, vd);
  return vd; 
}

template<typename G>
inline
ED add_edge(VD va, VD vb, G& g){
  auto e = boost::add_edge(getkey(g,va),getkey(g,vb), g).first; //in the current graph setup parallel edges are allowed, so .second is never false
  auto ed = std::max(boost::get(&Edge::ed, g))+1;
  boost::put(&Edge::ed, g, e, ed);
  return ed;
}

template<typename G>
inline void remove_edge(ED ed, G& g){
  boost::remove_edge(getkey(g, ed), g); //this invalidates all edge iterators... or it would if the werent indexed!
}

template<typename G>
inline void remove_vertex(VD vd, G& g){
  auto v = getkey(g, vd);
  boost::clear_vertex(v,g);    //removes all edges connected to the vertex
  boost::remove_vertex(v,g);   //removes the vertex and invalidates all vertex descriptors larger than vd (and of coursee again all edge descriptors
}


#endif
