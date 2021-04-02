#ifndef OUTPUT_GRAPH_H
#define OUTPUT_GRAPH_H

#include "../include/init_graph.h"
#include "../include/hwloc-test.h"

//####### Printing the graph
template<typename G>
class label_writer{
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;

public:
  label_writer() = delete; //must have reference to graph
  void operator()(std::ostream& out, const V& v) const {
    out << "[label=\"" << boost::get(&Vertex::type, g, v) << " #" << boost::get(&Vertex::index, g, v) << "\"]";
  }
  void operator()(std::ostream& out, const E& e) const {
    auto label = boost::get(&Edge::label, g, e);
    if (label == "member")    {out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=red]"; return;}
    if (label == "replicates"){out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=blue]"; return;}
    if (label == "partition") {out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=green]"; return;}
    out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=black]";
  }
  const G& g;
};

//void make_dotfile(const graph_t & g, const std::string& dotf = "out.dot");
//void make_dotfile_nolabel(const graph_t & g, const std::string& dotf = "out.dot");
template<typename G>
void make_dotfile(const G& g, const std::string& dotf = "out.dot"){
  std::ofstream dot(dotf);
  label_writer<G> lw{g};   //constructor declaring what graph to use, to use the label_writer as a functor
  boost::write_graphviz(dot, g, lw, lw);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}

template<typename G>
void make_dotfile_nolabel(const G& g, const std::string& dotf = "out.dot"){
  std::ofstream dot(dotf);
  boost::write_graphviz(dot, g);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}


//void find_pattern(const graph_t& g);



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
    if(boost::get(&Vertex::depth, g, *va_it) >= boost::get(&Vertex::depth, g, vb))
      ++va_it;
    if(boost::get(&Vertex::depth, g, *va_it) < boost::get(&Vertex::depth, g, vb))
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

////NOTE: maybe just use the graph without & -> by copy, to preserve it and not accidentally write in it...
//template<typename P, typename G, typename EV> //NOTE can we just use this for querying literally anything...?
//decltype(auto)
//filtered_graph(G& g, P EV::* p){ // std::function<bool(P)>& fun){ //TODO make it arbitrary in length or leave it to the user.. IDEA make an filtered graph of a filtered graph recursively to facilitate all the needs... otherwise one would have to distinguish which is about vertices and which is about edges
//    //std::function fun
//  using map_t = decltype(boost::get(p,g));
//  constrained_map<map_t> filter(boost::get(p,g));
//  return boost::filtered_graph<G, constrained_map<map_t>>(g, filter);
//}
//
//template<typename P, typename G, typename EV> //NOTE can we just use this for querying literally anything...?
//decltype(auto)
//filtered_graph(const G& g, P EV::* p){ // std::function<bool(P)>& fun){ //TODO make it arbitrary in length or leave it to the user.. IDEA make an filtered graph of a filtered graph recursively to facilitate all the needs... otherwise one would have to distinguish which is about vertices and which is about edges
//    //std::function fun
//  using map_t = decltype(boost::get(p,g));
//  constrained_map<map_t> filter(boost::get(p,g));
//  return boost::filtered_graph<G, constrained_map<map_t>>(g, filter);
//}


template<typename T, typename G, typename EV> //NOTE can we just use this for querying literally anything...?
decltype(auto)
filtered_graph(G& g, T EV::* p, T value){ // std::function<bool(P)>& fun){ //TODO make it arbitrary in length or leave it to the user.. IDEA make an filtered graph of a filtered graph recursively to facilitate all the needs... otherwise one would have to distinguish which is about vertices and which is about edges
  //make a function, that chooses between edges and vertices...
  VPred<T,G> vfil{&g,p,value};
  using fil_t = decltype(vfil);
 // EPred<P, G>  efil(g);

  return boost::filtered_graph<G,fil_t,fil_t> (g, vfil, vfil);
}

//simple algorithm to make k partitions of CUs by removing k-1 longest edges from a Kruskal MST
//in itself MSTs but not balanced  to any workload or number of CUs
template<typename G, typename V>
void k_partitions(G& g, int k,  Distance<G,V> dist){
//using E = typename boost::graph_traits<G>::edge_descriptor;
  using ndE = typename boost::graph_traits<ndgraph_t>::edge_descriptor;
  using ndV = typename boost::graph_traits<ndgraph_t>::vertex_descriptor;
//using V as given by the template param 

  //TODO filter only the CUs
  //potentially use dijkstras algorythm for distances...
 //CHANGE PASSWORD ON GITHUB OR YOURE SCREWED!!!!!!! 

  //auto svert = get_vds(g, std::make_pair(&Vertex::type, "HWLOC_OBJ_PU"));
  //const auto num_vert = svert.size();

  //std::cout << "Partitioning " << num_vert << "PUs" << std::endl;

  auto fil = filtered_graph(g, &Vertex::type, VType("HWLOC_OBJ_PU"));

  boost::print_graph(fil);

  //keeping it slightly general should a replacement for this algo come
  ndgraph_t partg;
  std::cout << "still alive1" << std::endl;
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

  std::cout << "still alive" << std::endl;
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
