#ifndef QUERY_GRAPH_H
#define QUERY_GRAPH_H

#include <iostream>
#include <vector>
#include "init_graph.h"

using Part_ID = int; //Partition ID (the number behind the vertex identifying a group/partition

using Partition = std::vector<std::pair<VD, Part_ID>>; //List of VDs and their ids, telling which group they are in  
// alternatively using Partition = std::unordered_map<VD,Part_ID>

using PartList = std::vector<Partition>; // if you were to check multiple partitionings...

using ValList = std::vector<int>; //actually bool... 0 if PartList[index] violates restrictions (primed for removal later on) 

struct default_tag{};
struct global_best_tag{}; //needs additional arguments
struct CPU_tag{int N_cpu};
struct cluster_tag{ int N_clu};

/*procedure 1 global optimal soulution (NP hard):
 * 1. make all partitions
 * 2. apply filters
 * 3. sort results 
 *
 *
 */


/*procedure 2: look for local solutions
 * breadth first search in graph (find N closest cores and call it a group, iterate)
 * dijstra search in graph (find N furthest cores and make groups/clusters around them)
 *
 *
 */

/* procedure 3 use distance vector from each CU to target CU as a distance metric. multiply it with itself (Markov chain) and find clusters that way... 
 *
 */


//PartList create_all_partitions(){ //creates all(!) possible and impossible partitions of CUs using each CU once
//  PartList npl {};
//  return npl;
//}


/* use OPTICS as default when no specifications are given
 * PROBLEM distances may generally not be symmetrical (ASSUMPTION: the are!)
 */

/* k means
 * assign k clusters-centers onto list of distances from CU 1 (8 cores 3 clusters -> every 2.06666 cores a new seed to grow a cluster)
 * optimise distances within cluster / shift them by reassigning to closest cluster center and reassigning a center core
 * iterate 5 times
 */
 


//at the moment assuming for the sake of simplicity of this function, that distance is symmetrical
//out_pl = vector of partitions to be written to
//
Partition part(
	//  std::function<double(VD,VD,const graph_t& g)> dist, // the distance with which to determin next/suitable neighbours
	//  const graph_t& g, //the graph
	//  PartList& out_pl  //vector that is going to be written to
	 ){
//Dummy TODO
// place here sophisticated partitioning algorithm...

  
  Partition out;  
  out.push_back(std::make_pair(VD(10),0));
  out.push_back(std::make_pair(VD(11),0));
  out.push_back(std::make_pair(VD(12),1));
  out.push_back(std::make_pair(VD(13),1));
  return out;
}


/*
template<typename P, typename T>
struct restriction{

  const P& _p //property

  const std::tuple<std::function<bool(T)>>& _c //constraints

};

each and every partition needs to fulfill all the restrictions to be accepted as a valid partitioning
*/

//use a view...???
//template<typename C>
//C sort_by_type(const C& cont){
//  it = cont.begin();
//  int i;
//  typename C::value_type temp_max; 
//  while (it!=cont.end()){ 
//    C
//  
//  }
//  return 
//}   

//this could be going to be one step of 
std::vector<VD> get_vev(){//

}



template<typename cVP,typename cEP> //containers for vertex and edge properties of the kind, that on each step one or multiple properties can be queried. these will be asked  
std::vector<VD> get_VD_by_path(){

} 
//iterators/dfs iterators incoming...

#endif
