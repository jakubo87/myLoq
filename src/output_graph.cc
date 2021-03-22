
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





//PathQueries
//"find a tuple/structure
//will work under the assumption, that one is contained by the other for now, MxN relations can be resolved by unifying the containig side into a new entity... later work TODO  
//void find_pattern(const graph_t& g){
//  graph_t temp =g;
//  //find 'at least' 2 PUs with common L1 cache -> read: child/parent relation (unless otherwise customised or specified) (possible future work TODO)
//  //'at least is imoprtant to avoid enumerating combinatorics for now
//  //ALTERNATIVE approach dfs on a filtered graph (only parent) already counting PUs and checking predicate (number of PUs)
////for now all queries will have to be about containment in lack of other relationships
//  //list all PUs
//  auto sources = get_vds(temp, VType("HWLOC_OBJ_PU"));
//  auto gv = make_group("temp", sources, temp);
//
//  //make new graph including paths from PUs to cache
//  graph_t t = make_can_tree(temp ,gv , "HWLOC_OBJ_L1CACHE");
//  make_dotfile_nolabel(t,"pattern_find_test.dot"); 
//  //check for validity (count PUs in each sub graph from each start)
//  
//}
  

