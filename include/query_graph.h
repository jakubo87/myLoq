#ifndef QUERY_GRAPH_H
#define QUERY_GRAPH_H

#include "../include/hwloc-test.h"
#include "../include/init_graph.h"
#include "../include/visitors.h"
#include "../include/predicates.h"
#include <tuple>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/copy.hpp>

//Maybe some day...
//struct default_tag{};
//struct global_best_tag{}; //needs additional arguments
//struct CPU_tag{int N_cpu};
//struct cluster_tag{ int N_clu};


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


//########### AGGREGATE FUNCTIONS  #######################
template<typename G, typename V>
int count_obj(const G& g, V vd){
  unsigned int count=0;
  bfs_counter bfsc{count};
  boost::breadth_first_search(g,vd, boost::visitor(bfsc));
  std::cout << "counted " <<  count << std::endl;
  return count;
}

//perhaps allow std library algorithms for users to iterate over graphs...? nice to have
template< typename G, typename T,typename V>
T accumulate (const G& g, T Vertex::* mptr, V st_vd){
  T value=0;
  bfs_accumulator<T> bfsa(value, mptr);
  boost::breadth_first_search(g, st_vd, boost::visitor(bfsa));
  std::cout << "accumulated: " << value << std::endl; 
  return value;
}

template< typename G, typename V>
bool is_reachable (const G& g, V vd1, V vd2){
  bool res=0;
  bfs_reacher<V> reach(res, vd2);
  boost::breadth_first_search(g, vd1, boost::visitor(reach));
  std::cout << vd2 << " is reachable from " << vd1 <<": " << res << std::endl; 
  return res;
}


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
get_eds(const G& g, VD va, VD vb, const EType& label){
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


//######### COPY GRAPH ##########
template<typename Gi, typename Go>
void copy_graph(const Gi& gi, const Go& go){
  boost::copy_graph(gi, go);
}

//######### adjacent vertices #########
template<typename G, typename VD>
decltype(auto) adjacent_vertices(VD vd, const G& g){
  return boost::adjacent_vertices(vd, g);
}

//######### adjacent vertices #########
template<typename G>
decltype(auto) vertices(const G& g){
  return boost::vertices(g);
}




//goto parent iterator -> only in the parent/child cathegory
template<typename G>
class anc_iterator{
  using self_t = anc_iterator;
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  using E = typename boost::graph_traits<G>::edge_descriptor;
  
 public:
  anc_iterator()=delete;
  anc_iterator(const G& g, const V& vd): g(g), vd(vd){}  //ctor 
  V operator*() const {return vd;} 

  bool operator==(const self_t& rhs) const {return (&g==&rhs.g && vd==rhs.vd);} //comparing identity, not equality of the graph

  //rise operator
  self_t& operator++(){
    auto range = boost::out_edges(vd,g);
    auto it = std::find_if(range.first, range.second, [&](const auto& ed){
      return boost::get(&Edge::label, g, ed)=="child";}); // there should be only one
    if (it!=range.second) //if is someone's child, if not, does nothing
      vd = boost::target(*it,g);
    return *this;
  }
 private:  
  const G& g;
  V vd; //<-pos
};


//is va a descendent of vb?
template<typename G, typename V>
bool is_ancestor(const V& va, const V& vb, const G& g){

  bool res=false;
  anc_iterator<G> it(g,va);
  V vcur;
  do{
    vcur=*it;      //update
    if (vcur==vb){ //check
      res=true;    //success
      break;       //exit
    }
  }
  while(*(++it)!=vcur);  //exit if nothing happens (root node is reached)

  return res;
}

//lowest common ancestor
template<typename G, typename V>
V lca(const G& g, V va, V vb){
  anc_iterator<G> va_it(g,va);
  anc_iterator<G> vb_it(g,vb);
 
  while(*va_it !=  *vb_it){
    if(boost::get(&Vertex::depth, g, *va_it) >= boost::get(&Vertex::depth, g, *vb_it))
      ++va_it;
    if(boost::get(&Vertex::depth, g, *va_it) < boost::get(&Vertex::depth, g, *vb_it))
      ++vb_it;
  }
  return *va_it;
}

//###### Implementation
template <typename Map> //will SFINAE make it right? if we use a type, that is only existent in Edge or Vertex
struct constrained_map {
  using res_t = typename boost::property_traits<Map>::value_type;
  constrained_map(){ }
  constrained_map(Map prop) : m_prop(prop) { }
  template<typename EV>
  bool operator()(const EV& ev) const {
    return 0 < boost::get(m_prop, ev);
  }
  Map m_prop;
  //std::function<bool(res_t)> fun_= [&](auto res){return res!=0;};
};


//############################  FILTERING  ###############################################
// Vertex and edge filters
template<typename T, typename G> 
decltype(auto)
filtered_graph(G& g, T Edge::* p, T value){ 
  EPred<T,G> vfil{&g,p,value};
  using fil_t = decltype(vfil);
 // EPred<P, G>  efil(g);

  return boost::filtered_graph<G,fil_t,fil_t> (g, vfil, vfil);
}

template<typename T, typename G> 
decltype(auto)
filtered_graph(G& g, T Vertex::* p, T value){ 
  VPred<T,G> vfil{&g,p,value};
  using fil_t = decltype(vfil);

  return boost::filtered_graph<G,fil_t,fil_t> (g, vfil, vfil);
}

//Adjacency filter
template<typename V, typename G> 
decltype(auto)
vicinity(G& g, V vd){
  VPred_adj<G> vfil{&g, vd};
  using fil_t = decltype(vfil);

  return boost::filtered_graph<G,fil_t,fil_t> (g, vfil, vfil);
}

//Adjacency filter
template<typename G> 
decltype(auto)
isolate_cat(G& g, const EType& label){
//  static_assert(std::is_same<typename boost::graph_traits<G>::vertex_descriptor,V>));
  VPred_cat<G> vfil{&g, label};
  using fil_t = decltype(vfil);
 // EPred<P, G>  efil(g);

  return boost::filtered_graph<G,fil_t,fil_t> (g, vfil, vfil);
}




#endif
