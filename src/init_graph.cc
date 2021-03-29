
#include "../include/init_graph.h"
#include "../include/hwloc-test.h"







//name may be a bit misleading TODO: translation of hwloc_obj->type to string
std::string obj_type_toString(hwloc_obj_t & obj){
  switch (obj->type) {
    case HWLOC_OBJ_MACHINE    :  return "HWLOC_OBJ_MACHINE";
    case HWLOC_OBJ_PACKAGE    :  return "HWLOC_OBJ_PACKAGE";
    case HWLOC_OBJ_CORE       :  return "HWLOC_OBJ_CORE";
    case HWLOC_OBJ_PU         :  return "HWLOC_OBJ_PU";
    case HWLOC_OBJ_L1CACHE    :  return "HWLOC_OBJ_L1CACHE";
    case HWLOC_OBJ_L2CACHE    :  return "HWLOC_OBJ_L2CACHE";
    case HWLOC_OBJ_L3CACHE    :  return "HWLOC_OBJ_L3CACHE";
    case HWLOC_OBJ_L4CACHE    :  return "HWLOC_OBJ_L4CACHE";
    case HWLOC_OBJ_L5CACHE    :  return "HWLOC_OBJ_L5CACHE";
    case HWLOC_OBJ_L1ICACHE   :  return "HWLOC_OBJ_L1ICACHE";
    case HWLOC_OBJ_L2ICACHE   :  return "HWLOC_OBJ_L2ICACHE";
    case HWLOC_OBJ_L3ICACHE   :  return "HWLOC_OBJ_L3ICACHE";
    case HWLOC_OBJ_GROUP      :  return "HWLOC_OBJ_GROUP";
    case HWLOC_OBJ_NUMANODE   :  return "HWLOC_OBJ_NUMANODE";
    case HWLOC_OBJ_BRIDGE     :  return "HWLOC_OBJ_BRIDGE";
    case HWLOC_OBJ_PCI_DEVICE :  return "HWLOC_OBJ_PCI_DEVICE";
    case HWLOC_OBJ_OS_DEVICE  :  return "HWLOC_OBJ_OS_DEVICE";
    case HWLOC_OBJ_MISC       :  return "HWLOC_OBJ_MISC";
    case HWLOC_OBJ_MEMCACHE   :  return "HWLOC_OBJ_MEMCACHE";
    case HWLOC_OBJ_DIE        :  return "HWLOC_OBJ_DIE";
    case HWLOC_OBJ_TYPE_MAX   :  return "HWLOC_OBJ_TYPE_MAX"; //not in docu! TODO
    default                   :  return "HWLOC_CUSTOM_OBJ";
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


graph_t init_graph(const hwloc_topology_t & t){

  graph_t g;
  int max_depth=0;
  int depth=0;

 //get all the hw objs

  depth = hwloc_get_type_depth(t, HWLOC_OBJ_PACKAGE);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN) {
  printf("*** The number of packages is unknown\n");
  } else {
  printf("*** %u package(s)\n",hwloc_get_nbobjs_by_depth(t, depth));
  }

  //query all the special types and insert into graph TODO
  //first all the memory for testing
  auto nobj = hwloc_get_nbobjs_by_type(t, HWLOC_OBJ_NUMANODE);
  for (int i = 0; i<nobj; ++i){  
    auto obj = hwloc_get_obj_by_type(t, HWLOC_OBJ_NUMANODE, i);
    auto v = boost::add_vertex(g);
    boost::put(&Vertex::vid, g, v, getmax_vid(g)); //get new highest global index
    boost::put(&Vertex::type, g, v, obj_type_toString(obj));
    boost::put(&Vertex::index, g, v, obj->logical_index);
    std::cout << "Added vertex: (type: " << obj_type_toString(obj) << ", index: " << obj->logical_index << ")" << std::endl;
  }


  //breadth first core tree traversal -> positive depths
  max_depth = hwloc_topology_get_depth(t);

  for (depth = 0; depth < max_depth; depth++) {
    std::cout << "adding Objects at level to graph: " <<  depth << std::endl;
    for (Index i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); ++i) {
      auto obj = hwloc_get_obj_by_depth(t, depth, i);
      auto v = boost::add_vertex(g);
      boost::put(&Vertex::vid, g, v, getmax_vid(g)); //get new highest global index
      boost::put(&Vertex::type, g, v, obj_type_toString(obj));
      boost::put(&Vertex::index, g, v, obj->logical_index);
      std::cout << "Added vertex: (type: " << obj_type_toString(obj) << ", index: " << obj->logical_index << ")" << std::endl;
    //add relationships
    if (obj->type!=HWLOC_OBJ_MACHINE){
      //from a childs point of view - the parent will have been added already and can be queried. Otherwise it would be necessary to find blank vertices and attach attributes to make them distinguishable
      //question remains how memory plays into this...
      //find all the parents:
      auto pa_obj = obj->parent;
      auto pa_id = get_vds(
          g, 
          std::make_pair(&Vertex::type,obj_type_toString(pa_obj)),
          std::make_pair(&Vertex::index,pa_obj->logical_index)
        );
      for(auto & p : pa_id){ // "for" not necessary in a tree (hwloc) there is only one parent, but maybe aliases...)
        // child
        auto ep = boost::add_edge(v, p, g).first;
        boost::put(&Edge::eid, g, ep, getmax_eid(g));
        boost::put(&Edge::label, g, ep, "child");
        std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;
        // parent
        auto ec = boost::add_edge(p, v, g).first;
        boost::put(&Edge::eid, g, ec, getmax_eid(g));
        boost::put(&Edge::label, g, ec, "child");
        std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;
        }
      }
    }
  }

  //memory relationships - Adding here to not query vertices that have not been established yet
  nobj = hwloc_get_nbobjs_by_type(t, HWLOC_OBJ_NUMANODE);
  for (int i = 0; i<nobj; ++i){
    auto obj = hwloc_get_obj_by_type(t, HWLOC_OBJ_NUMANODE, i);
    auto v = get_vds(g,std::make_pair(&Vertex::type,obj_type_toString(obj)),std::make_pair(&Vertex::index, obj->logical_index)).front(); 
    //query parent vertex - again only go from child nodes to parents
    auto pobj = obj->parent;
    auto p = get_vds(g,std::make_pair(&Vertex::type,obj_type_toString(pobj)),std::make_pair(&Vertex::index,pobj->logical_index)).front(); 

    // child
    auto ep = boost::add_edge(v, p, g).first;
    boost::put(&Edge::eid, g, ep, getmax_eid(g));
    boost::put(&Edge::label, g, ep, "child");
    std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;
    // parent
    auto ec = boost::add_edge(p, v, g).first;
    boost::put(&Edge::eid, g, ec, getmax_eid(g));
    boost::put(&Edge::label, g, ec, "child");
    std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;
    //what is the multiset selector in edge list good for then..? 
  }
  return g;
}







//TODO establish a somewhat reasonable default distance function, for the case, that the user doesn't provide one, subject to change: depending on what data from hwloc will be transported over here
//... and make overloads or each function, where a canonical distance can be of use
//

//enumerating combinatorics
std::vector<std::vector<int>>
comb(const int k, const std::vector<int>& vec){
  std::vector<std::vector<int>> res{std::vector<int> (k)};
  res.pop_back(); //clean up but keep the type
  std::vector<decltype(vec.begin())> viter(k);
  for(int i=0;i<k; ++i)
     viter[i]=vec.begin()+i;
  
  std::vector<int> temp(k);

  while(viter[0]+k != vec.end()){
    for(int i=0; i<k; ++i)
      temp[i]=*(viter[i]);
    res.push_back(temp);
    //move iterators / check for reset / if so go deeper and reset all iterators above
    for(int i=k-1; i>=0;--i){
      if (viter[i]-i+k != vec.end()){ //<- if trouble go deeper!
        ++viter[i]; //increase 
        ++i;
        for (;i<k;++i){  //resetting all above
          viter[i]=viter[i-1]+1;
        }
        break;
      }
    } 
  }     
  for(int i=0; i<k; ++i)
    temp[i]=*(viter[i]);
  res.push_back(temp);
  return res;
}
