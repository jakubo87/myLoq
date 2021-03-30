#include "../include/init_graph.h"
#include "../include/output_graph.h"

#include "../include/hwloc-test.h"

//*
//indentation of higher depth opjects
//*/
//std::ostream & operator<<(std::ostream & os,std::pair<std::string,int> p){
//  std::string s;
//  for (int i=0; i<=p.second; ++i){
//    s+=" ";
//  }
//  s+=s+p.first;
//
//  return operator<<(os,s);
//}
//
////general for all types with a std::to_string implementation
//template <class M>
//std::ostream & operator<<(std::ostream & os,std::pair<M,int> p){
//  std::string s;
//  for (int i=0; i<=p.second; ++i){
//    s+=" ";
//  }
//  s+=s+std::to_string(p.first);
//
//  return operator<<(os,s);
//}
//
//
////print topology
//std::ostream & operator<<(std::ostream & os, hwloc_topology_t & t){
//  //std::string s;
//
//  int max_depth=0;
//  int depth=0;
//  std::string s;
//
// //print breadth first
//  os << "breadth first approach\n";
//
//  max_depth = hwloc_topology_get_depth(t);
//
//
//  for (depth = 0; depth < max_depth; depth++) {
//      os << "Objects at level " <<  depth << std::endl;
//      for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
//        auto hw_obj = hwloc_get_obj_by_depth(t, depth, i);
//        s=obj_type_toString(hw_obj) + " " + std::to_string(i) + "\n";
//        s+= "OS_index: " + std::to_string(hw_obj->os_index) + "??? \n";
//        s+= "logical index: " + std::to_string(hw_obj->logical_index);
//        s+='\n';
//        os << std::make_pair(s,depth);
//      }
//    }
//
//  os << "\n";
//
//  return operator<<(os,"");
//}





/*Vertices:
 * VType type aka std::string
 * Index index aka unsigned long int
 * 
 *Edges:
 * EType label aka std::string
 * double weight=0
 *
 *
 *TODO init additional properties by user
 *
 *
 */
int main ()
{
  //#######################   INITIALISATION   ################################################

  
  auto g = init_graph(); //init lokal machine
//TODO expand graph!
 
  //TODO extensible properties or adding properties the boost way... 
  //struct Edge {
  //  EType label;
  //  double weight=0; //TODO remove? 
  //};
  //######################      UPDATING SETUP           #####################################

  //add new relationship: replicates
  //find last level cache TODO (for all CUs)
  auto l2 = get_vds(g,std::make_pair(&Vertex::type,"HWLOC_OBJ_L2CACHE"));
  auto mem = get_vds(g,std::make_pair(&Vertex::type,"HWLOC_OBJ_NUMANODE"));
  for (auto vl : l2){
    for (auto vm : mem){
     //this commented line was the former hoped sulotion. however it turned out, that the edges would not appear in the graph. Only like done underneath. This may be some quirk or some larger underlying problem like can be the invalidation of indizes/descriptors and iterators when modifying the graph
      auto e = add_edge(vl,vm, g).first; //<-- this alone doesn't work!? TODO
      put(&Edge::label,g,e, "replicates");
    }
  }

  boost::print_graph(g);

  //###################################################################
  //TESTS:

//  struct index_odd{
//    bool operator(Index i)(
//
//
// };
  //###############################   FILTER GRAPH   ################################################

  //VPred<Index,graph_t> twoorless{&g,&Vertex::index,2};

  auto fgv = filtered_graph(g, &Vertex::index, Index(2));
  std::cout << "show only vertices with index < 2" << std::endl; 
  boost::print_graph(fgv);
  make_dotfile(fgv, "filtered_edge_graph.dot");

//  auto  fge = filtered_graph(g, &Edge::label); 
//  //shallow copy..= also according to the documentation it will not change the original graph... whatever that means if tried...
//  // display all remaining vertices
//  auto fil_r = boost::edges(fge);
//  std::for_each(fil_r.first, fil_r.second, [&](auto e){ std::cout << boost::get(&Edge::label, g, e) << " ";});
//  std::cout << std::endl;
//  boost::print_graph(g); 
//  boost::print_graph(fge);
//  
//
//  //##################################    MATHS     #################################################
//  //combinatorics (to be integrated into finding best solution)
//  auto vec = comb(4,std::vector<int> {2,4,6,8,10});
//  std::cout << "combining  [2,4,6,8,10] into sets of 4:" << std::endl;
//  for(auto co : vec){
//    for(auto el : co)
//      std::cout << el << " ";
//    std::cout << std::endl;
//  }
//  std::cout << vec.size() << " possible combinations." << std::endl;
//
//
//  //################################     BASICS     ##################################################
//  //find vd 
//  auto vds = get_vds(
//      g,                //the graph
//      VType("HWLOC_OBJ_CORE"), //the type
//      Index(0));               //the index
//  std::cout << "vd of core 0: " << vds[0] << std::endl;
//
//  //testing for something not included in the
//  
//
//
//  //###############################      GET/SET     #################################################
//  const auto e1 = get_ed(g,2,1,"child").front();
//  std::cout << "setting the weight of child edge between vertices 2 and 1. Original value: " << get(&Edge::weight, g, e1) << std::endl;
//  put(&Edge::weight, g, e1, 1000);
//  std::cout << "new value is: " << get(&Edge::weight, g, e1) << std::endl;
//
//
//
//  //generic vd query TODO does this also work when queries are generated at runtime...???
//  vds = get_vds(g, VType("HWLOC_OBJ_CORE"));
//  std::cout << "testing generic querying for vds... CORES have vd: " << std::endl;
//  for (auto& v : vds)
//    std::cout << v << " ";
//  std::cout << std::endl;
//  std::cout << "VDs with rubbish type: " << std::endl;
//  vds = get_vds(g, VType("xxx"));
//  for (auto& v : vds)
//    std::cout << v << " ";
//  std::cout << std::endl;
//  //  vds = get_vds(g, 1); <- does not compile you have to write the explicit type
//  std::cout << "searching for VD with type: CORE and index: 1 (testing matching in reverse order): " << std::endl;
//  vds = get_vds(g, Index(1), VType("HWLOC_OBJ_CORE"));
//  for (auto& v : vds)
//    std::cout << v << " ";
//  std::cout << std::endl;
//
//
//  
//  //find edge properties
//  std::cout << "ed label from vd 1 to 2: " << get_edge_label(g, get_ed(g,1,2,"parent").at(0)) << std::endl;
//  //find edges by their property
//  std::cout << "List all edges with the parent property:" << std::endl;
//  auto eds = get_eds(g, "parent");
//  for (const auto& ed : eds){
//    std::cout << boost::source(ed,g) << " to " << boost::target(ed,g) << std::endl;
//  }
//
//
//  //GROUPS
//  //make group - at first arbitrary
//  std::vector<VD> vs = {10,12};
//  auto i = make_group("Group1", vs, g);
//  std::cout << " Group1 has vd: " << i << std::endl; 
//
//
//  //#################################VISITORS TODO###########################################
//  count_obj(g,1);
//  accumulate(g,&Vertex::index, 1);
//
//  //###### PATHS/patterns #########
//  //return the group members of group 
//  std::cout << "The members of Group1 are the following:" << std::endl;
//  //TODO make path queries like "is connected to group", or "is 'child' of cache" 
//
//
//  //find subgraphs
//  find_pattern(g); //TODO
//
//  ///NOTE!!!! in order to calculate properties simply make a subgraph and use std::accumlate... for instance...
//
//
//
//  //DISTANCES
//  //calc custom distance
//  //define a distance function:
//  std::function<double(VD,VD,const graph_t&)> dist1 =  [&](auto va, auto vb, const graph_t& g)
//    {
//      //the result, if the graph has no direct edge in any allowed category defined by this function
//      double res = NOPATH;  //default 
//      auto range = boost::out_edges(va, g); //TODO boost::out_edges works, if you "make clean" one in a while...!
//      //check all edges for label "child"
//       std::for_each (range.first, range.second,[&](const auto & ei){
//        if (g[ei].label=="child" && vb==target(ei,g))
//          res = 10.0;       //case of rising in the hierarchy
//      });
//      //check all edges for label "parent"
//      std::for_each (range.first, range.second,[&](const auto & ei){
//        if (g[ei].label=="parent" && va==source(ei,g) && vb==target(ei,g))
//          res = 0.0;        //case when descending in hierarchy
//      });
//
//      return res;
//    };
//
//
//  std::cout << "distance (5,7): " << dist1(5,7,g) << std::endl;
//  std::cout << "distance (8,9): " << dist1(8,9,g) << std::endl;
//  std::cout << "path from 9 to 8:" << std::endl; 
//  auto r1 = shortest_path(g, 8, 9, dist1); 
//  for (auto vd : r1)
//    std::cout << vd << " ";
//  std::cout << std::endl;
//
//  //return clostest vertices of specified type sorted by distance
//  auto cl_pus = find_closest_to(g, dist1, "HWLOC_OBJ_PU", 11);
//  std::cout << "closest PUs relative to vd(11) with respect to user defined function dist1:" << std::endl;
//  for (auto a : cl_pus){
//    std::cout << a.first << " " ;
//  }
//  std::cout << std::endl;
//  std::cout << "with distances (respectively):" << std::endl;
//  for (auto a : cl_pus){
//    std::cout << a.second << " " ;
//  }
//  std::cout << std::endl;
//
//  //dijstra
//  auto r2 = dijkstra_spaths(g, 5, dist1);
//  for(VD vd = 0; vd<r2.size(); ++vd)
//    std::cout << "from vd 5 to "<< vd << ", shortest distance: " << r2[vd] << std::endl; 
//
//
//  //dijstrap on a filtered graph
//  //auto r3 = dijkstra_spaths(fgv, 5, dist1);
//  //for(VD vd = 0; vd<r3.size(); ++vd)
//  //  std::cout << "from vd 5 to "<< vd << ", shortest distance: " << r3[vd] << std::endl; 
//
//
//  //TODO find partitioning
//
  //ancestry iterator
  std::cout << "is 3 an ancestor of 11?: " << is_ancestor(11,3,g) << std::endl;
  std::cout << "is 11 an ancestor of 3?: " << is_ancestor(3,11,g) << std::endl;
//
//
//
  make_dotfile_nolabel(g,"totalnl.dot");
  make_dotfile(g,"total.dot");
//
//  //isolate a subgraph and reduce to hwloc relationships
//  auto ctree2 = make_can_tree(g,14);
//  make_dotfile_nolabel(ctree2,"hwloc.dot");


  //copy tests with copy graph
  graph_t cfg; //deep copy of filtered graph (is this even possible...?, what about the vertex indices...?
  boost::copy_graph(g, cfg);
  make_dotfile_nolabel(cfg, "copied_nolabel.dot");

  return 0;
}
//TODO make tests generic
