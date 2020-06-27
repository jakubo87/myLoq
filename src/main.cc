#include <iostream>
#include <hwloc.h>
#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include "../include/init_graph.h"
#include "../include/output_graph.h"

/*
indentation of higher depth opjects
*/
std::ostream & operator<<(std::ostream & os,std::pair<std::string,int> p){
  std::string s;
  for (int i=0; i<=p.second; ++i){
    s+=" ";
  }
  s+=s+p.first;

  return operator<<(os,s);
}
//general for all types with a std::to_string implementation
template <class M>
std::ostream & operator<<(std::ostream & os,std::pair<M,int> p){
  std::string s;
  for (int i=0; i<=p.second; ++i){
    s+=" ";
  }
  s+=s+std::to_string(p.first);

  return operator<<(os,s);
}

std::string obj_type_toString(hwloc_obj_t & obj){
  switch (obj->type) {
    case HWLOC_OBJ_MACHINE :  return std::string("HWLOC_OBJ_MACHINE");
    case HWLOC_OBJ_PACKAGE :  return "HWLOC_OBJ_PACKAGE";
    case HWLOC_OBJ_CORE :     return "HWLOC_OBJ_CORE";
    case HWLOC_OBJ_PU :       return "HWLOC_OBJ_PU";
    case HWLOC_OBJ_L1CACHE :  return "HWLOC_OBJ_L1CACHE";
    case HWLOC_OBJ_L2CACHE :  return "HWLOC_OBJ_L2CACHE";
    case HWLOC_OBJ_L3CACHE :  return "HWLOC_OBJ_L3CACHE";
    case HWLOC_OBJ_L4CACHE :  return "HWLOC_OBJ_L4CACHE";
    case HWLOC_OBJ_L5CACHE :  return "HWLOC_OBJ_L5CACHE";
    case HWLOC_OBJ_L1ICACHE : return "HWLOC_OBJ_L1ICACHE";
    case HWLOC_OBJ_L2ICACHE : return "HWLOC_OBJ_L2ICACHE";
    case HWLOC_OBJ_L3ICACHE : return "HWLOC_OBJ_L3ICACHE";
    case HWLOC_OBJ_GROUP : return "HWLOC_OBJ_GROUP";
    case HWLOC_OBJ_NUMANODE : return "HWLOC_OBJ_NUMANODE";
    case HWLOC_OBJ_BRIDGE : return "HWLOC_OBJ_BRIDGE";
    case HWLOC_OBJ_PCI_DEVICE : return "HWLOC_OBJ_PCI_DEVICE";
    case HWLOC_OBJ_OS_DEVICE : return "HWLOC_OBJ_OS_DEVICE";
    case HWLOC_OBJ_MISC : return "HWLOC_OBJ_MISC";
    case HWLOC_OBJ_MEMCACHE : return "HWLOC_OBJ_MEMCACHE";
    case HWLOC_OBJ_DIE : return "HWLOC_OBJ_DIE";
    case HWLOC_OBJ_TYPE_MAX : return "HWLOC_OBJ_TYPE_MAX"; //not in docu! TODO
    default : return "HWLOC_CUSTOM_OBJ";
  };
}
//
//constexpr
//std::string obj_cachetype_toString(hwloc_obj_cache_t & obj){
//  switch (obj->type) {
//    case HWLOC_OBJ_CACHE_UNIFIED :  return std::string("HWLOC_OBJ_CACHE_UNIFIED");
//    case HWLOC_OBJ_CACHE_DATA :  return std::string("HHWLOC_OBJ_CACHE_DATA");
//    case HWLOC_OBJ_CACHE_INSTRUCTION : return std::string("HHWLOC_OBJ_CACHE_DATA");
//    default : return "HWLOC_CUSTOM_CACHE_OBJ"; //??? TODO
//  };
//}
//
//constexpr
//std::string obj_bridgetype_toString(hwloc_obj_cache_t & obj){
//  switch (obj->type) {
//    case HWLOC_OBJ_CACHE_UNIFIED :  return std::string("HWLOC_OBJ_CACHE_UNIFIED");
//    case HWLOC_OBJ_CACHE_DATA :  return std::string("HHWLOC_OBJ_CACHE_DATA");
//    case HWLOC_OBJ_CACHE_INSTRUCTION : return std::string("HHWLOC_OBJ_CACHE_DATA");
//    default : return "HWLOC_CUSTOM_CACHE_OBJ"; //??? TODO
//  };
//}

///// somehow not quite working
//void traverse_tree_DF(std::ostream & os, hwloc_obj_t obj, int depth){
//    do{
//      os << std::make_pair(obj_type_toString(obj), depth) << "\n";
//      os << "still alive\n";
//      if (obj->first_child){
//        os << "still alive2\n";
//        traverse_tree_DF(os,obj->memory_first_child,++depth);
//      }
//      os << "still alive3\n";
//      obj=obj->next_sibling;
//    }while (obj);
//    os << "still alive4\n";
//}
//
//void traverse_tree_DF(std::ostream & os, hwloc_obj_t obj){
//  traverse_tree_DF(os,obj, 0);
//}
//


//print topology
std::ostream & operator<<(std::ostream & os, hwloc_topology_t & t){
  //std::string s;

  int max_depth=0;
  int depth=0;
  std::string s;

 //print breadth first
  os << "breadth first approach\n";

  max_depth = hwloc_topology_get_depth(t);


  for (depth = 0; depth < max_depth; depth++) {
      os << "Objects at level " <<  depth << std::endl;
      for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
        auto hw_obj = hwloc_get_obj_by_depth(t, depth, i);
        s=obj_type_toString(hw_obj) + " " + std::to_string(i) + "\n";
        s+= "OS_index: " + std::to_string(hw_obj->os_index) + "??? \n";
        s+= "logical index: " + std::to_string(hw_obj->logical_index);
        s+='\n';
        os << std::make_pair(s,depth);
      }
    }

  os << "\n";

 ////print depth first
 // os << "depth first approach\n";
 // //
 // depth=0;
 // hwloc_obj_t obj=hwloc_get_obj_by_depth(t, 0, depth); //ROOT
 // traverse_tree_DF(os, obj);


  return operator<<(os,"");
}





int main ()
{
  //hwloc_init
  hwloc_topology_t t;
  hwloc_topology_init(&t);  // initialization
  //hwloc_topology_set_io_types_filter(t,HWLOC_TYPE_FILTER_KEEP_IMPORTANT);
  hwloc_topology_load(t);   // actual detection
  //std::cout << t <<"\n"; //print nodes
  
  auto g = init_graph(t);
//TODO expand graph!
  //cleaning up:
  hwloc_topology_destroy(t);
  //since data was copied hwloc is not needed anymore
  //
  make_dotfile(g);
  std::cout << "memory still missing" << std::endl;
  return 0;
}

