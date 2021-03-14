
#include "../include/output_graph.h"
#include "../include/hwloc-test.h"


//is va a descendent of vb?
bool is_ancestor(const VD& va, const VD& vb, const graph_t& g){

  bool res=false;
  anc_iterator it(g,va);
  VD vcur;
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

//TODO subgraph-writer
//make canonical tree from  all the elements in group to root by adding all the parent/child edges & vertices on the way
graph_t make_can_tree(const graph_t& s, const VD& gv, const VType& type){
//TODO insert vertex and edge properties
  graph_t t; //target graph
  auto range = boost::adjacent_vertices(gv,s); //lists targets of outgoing edges -> vd of the group's members
  //starting vertex is range.first
  std::for_each(range.first, range.second,
      [&](const auto& vd)
      {
        anc_iterator a_it(s,vd);
        while (get(&Vertex::type, s, *a_it) != type){  
          VD curr =*a_it;
          ++a_it; 
          if (get_ed(t,curr, *a_it, "child").empty()){ 
            auto c = boost::add_edge(curr, *a_it, t).first; // keep it default constructible... TODO deep copy edge/vertex
            auto p = boost::add_edge(*a_it, curr, t).first;
            put(&Edge::label,t, c, "child");                //...and replenish stuff later... 
            put(&Edge::label,t, p, "parent");
          }
          else break;
        }
      }
      );
  std::cout << "created hwloc-tree of subgraph." << std::endl;
  return t;
}

graph_t make_can_tree(const graph_t& s , const VD& gv){
  return make_can_tree(s ,gv, "HWLOC_OBJ_MACHINE");
}



//PathQueries
//"find a tuple/structure
//will work under the assumption, that one is contained by the other for now, MxN relations can be resolved by unifying the containig side into a new entity... later work TODO  
void find_pattern(const graph_t& g){
  graph_t temp =g;
  //find 'at least' 2 PUs with common L1 cache -> read: child/parent relation (unless otherwise customised or specified) (possible future work TODO)
  //'at least is imoprtant to avoid enumerating combinatorics for now
  //ALTERNATIVE approach dfs on a filtered graph (only parent) already counting PUs and checking predicate (number of PUs)
//for now all queries will have to be about containment in lack of other relationships
  //list all PUs
  auto sources = get_vds(temp, VType("HWLOC_OBJ_PU"));
  auto gv = make_group("temp", sources, temp);

  //make new graph including paths from PUs to cache
  graph_t t = make_can_tree(temp ,gv , "HWLOC_OBJ_L1CACHE");
  make_dotfile_nolabel(t,"pattern_find_test.dot"); 
  //check for validity (count PUs in each sub graph from each start)
  

  //->bfs_search (while collecting properties -> counting)
  //replicate for ambiguity: you have 4 cores but only wanted 2? well guess what, you will have all the possibilities (2 over4)i see above... possible solution, copy graphs and delete PUs until the correct amount is contained, recursively?
  //std::vector<graph_t> res; 
  //find 2 PUs with common L1 cache -> read: child/parent relation (unless otherswise customised or specified)
  //SPOILER none available on test machine
  // a more mathematical approach
  // there exist 2 PU a,b  with a!=b, such that their common ancestor c (Parent(a)^n1 & P(b)^n2) is L2 cache or lower (n3's Parent(c).type == "L2_CACHE" 

}



//graph_t make_subgraph(const graph_t& s, const VD& gv){
//  //1. copy the whole graph
//  //2. look for all vertices that have no children if they are descendents of members of the group 
//  //if the group is the whole graph then the group will point to the root.
//  //if the group is a subgraph, then an out edge will point towards a vertex lower down in the hierarchy
//  //the subgraph will still include everything up to the root (so it gets included for all groups)
//  //if a subgraph is to exclude members, the branches that are included need to be in the group.
//  //TODO excluding group (inv_group) <- checks if an ancestor is in the excluded group
//  graph_t g(s); 
//    
//  return g;
//}

