
#include "../include/init_graph.h"
#include "../include/output_graph.h"

//this is the interface, which simply uses the distance function.
//it is recommended, to make the distance function not point to a certain graph, but to leave that as a parameter
//double distance( 
//              VD vd1,
//              VD vd2,
//              const graph_t& g,  // g is not THE graph, it is any graph included -> shadowing
//              std::function<double(VD,VD,const graph_t&)> func){
//  return func(vd1, vd2, g);
//}


////may need some sort of wildcard that always returns true when compared to predicates
//std::vector<VD> get_vds(const graph_t& g, const std::string& t, Index i){
//  std::vector<VD> res;  //<- necessary to always have something to return even if nothing matches
//  auto range = boost::vertices(g);
//    std::for_each(range.first, range.second, [&](const auto & vd)
//      {
//        if(g[vd].type==t && g[vd].index==i)
//          res.push_back(vd);
//      });
//  return res;
//} 

std::vector<VD> get_vds_by_type(const graph_t& g, const std::string& t){
  std::vector<VD> res;
  auto range = boost::vertices(g);
    std::for_each(range.first, range.second, [&](const auto& vd)
      {
        if(g[vd].type==t)
          res.push_back(vd);
      }
    );
  return res;
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
          res.push_back(ed);
      }
  );
  return res;
}

//can only be const... or can it..?
const EType& get_edge_label(const graph_t& g, const ED& ed){
  return g[ed].label;
}

VD make_group(const std::string& name, const std::vector<VD>& cont, graph_t& g){
  //insert group vertex into graph
  auto v = boost::add_vertex(g);
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
    boost::add_vertex(
        {obj_type_toString(obj), //type
         obj->logical_index},    //index
         g); 
    std::cout << "Added vertex: (type: " << obj_type_toString(obj) << ", index: " << obj->logical_index << ")" << std::endl;
  }


  //breadth first core tree traversal -> positive depths
  max_depth = hwloc_topology_get_depth(t);

  for (depth = 0; depth < max_depth; depth++) {
    std::cout << "adding Objects at level to graph: " <<  depth << std::endl;
    for (Index i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); ++i) {
      auto obj = hwloc_get_obj_by_depth(t, depth, i);
      auto v = boost::add_vertex(
         {obj_type_toString(obj), //type
          obj->logical_index},    //index
          g);  
    std::cout << "Added vertex: (type: " << obj_type_toString(obj) << ", index: " << obj->logical_index << ")" << std::endl;
    
    //add relationships
    if (obj->type!=HWLOC_OBJ_MACHINE){
      //from a childs point of view - the parent will have been added already and can be queried. Otherwise it would be necessary to find blank vertices and attach attributes to make them distinguishable  (alternative approach would be a depth first traversal, however parenthood can be over more than one level of depth) 
      //question remains how memory plays into this...
      //find all the parents:
      auto pa_obj = obj->parent;
      auto pa_id = get_vds(g, obj_type_toString(pa_obj), pa_obj->logical_index);
      for(auto & p : pa_id){ // "for" not necessary in a tree (hwloc) there is only one parent, but maybe aliases...)
        // child
        if (boost::add_edge(
        v, //out
        p, //in
        {"child", 10.0}, // edge property
        g).second)
          {std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;}
        // parent
        if (boost::add_edge(
        p, //out
        v, //in
        {"parent", 0.0}, // edge property
        g).second)
          {std::cout << "Added Edge: (from " << p << " to " << v << ", label: parent" << std::endl;}
        //checked if 2 same edges will exist and make trouble -> multiple identical edges can coexist...
        //what is the multiset selector in edge list good for them..? TODO
        }
      }
    }
  }

  //memory relationships - Adding here to not query vertices that have not been established yet
  nobj = hwloc_get_nbobjs_by_type(t, HWLOC_OBJ_NUMANODE);
  for (int i = 0; i<nobj; ++i){
    auto obj = hwloc_get_obj_by_type(t, HWLOC_OBJ_NUMANODE, i);
    auto v = get_vds(g, obj_type_toString(obj), obj->logical_index).at(0); 
    //query parent vertex - again only go from child nodes to parents
    auto pobj = obj->parent;
    auto p = get_vds(g, obj_type_toString(pobj), pobj->logical_index).at(0); 

    // child
    if (boost::add_edge( // the "if" may be unnecessary, because the container allows for parallel edges 
    v, //out - the numa node vertices are being constructed first, so their vd is the index i 
    p, //in
    {"child", 10.0}, // edge property
    g).second)
      {std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;}
    // parent
    if (boost::add_edge(
    p, //out
    v, //in
    {"parent", 0.0}, // edge property
    g).second)
      {std::cout << "Added Edge: (from " << p << " to " << v << ", label: parent" << std::endl;}
    //checked if 2 same edges will exist and make trouble -> multiple identical edges can coexist...
    //what is the multiset selector in edge list good for then..? 
  }

  //make default group with all memory and all cores
  auto grp = get_vds_by_type(g,"HWLOC_OBJ_CORE");
  auto mem = get_vds_by_type(g,"HWLOC_OBJ_NUMANODE");

 // auto grp = get_vds(g,"HWLOC_OBJ_CORE");
 // auto mem = get_vds(g,"HWLOC_OBJ_NUMANODE");

//  for (const auto& i : mem)
//    grp.push_back(i);
//  make_group("Group0", grp, g);
//
  return g;
}


//Dijkstra TODO return value
//prints predeccessors from target to source (potentially for debugging..?)
std::vector<VD> shortest_path(const graph_t& g, VD va, VD vb, std::function<double(VD,VD,const graph_t&)> func){
  std::vector<VD> directions(num_vertices(g));

  //###helper function to get the input right
  std::function<double(ED)> f = [&](const ED& ed)
    {
      auto va = boost::source(ed, g);
      auto vb = boost::target(ed, g); 
      return func(va, vb, g); // by capturing the graph here, you don't need to point to g later
    };

  boost::dijkstra_shortest_paths(
      g,  //graph
      va, //source
      boost::weight_map(boost::function_property_map<decltype(f),ED,double>(f))
      .predecessor_map(boost::make_iterator_property_map(
                      directions.begin(), get(boost::vertex_index, g))));

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

//possible language for paths: ("PU","child", "L1Cache") 

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


//naive implementation to provide requests like "show me clostest PUs" (close as defined by the function...)
//"naive" as it does assume symmetry
//so warning you might get trapped if you assumed the wrong amount, or if you take advantage of PUs, that are there but may be detrimental to performance due to distance, 
std::vector<std::pair<VD,double>>
find_closest_to(const graph_t& g,
                std::function<double(VD,VD,const graph_t&)> dist, //distance function (TODO check if this or the dijkstra find!)
                VType type, VD start){
  //get all VDs of specified type
  auto vds = get_vds(g, type);
  std::vector<std::pair<VD,double>> res(vds.size());
 // struct less_by_dist{
 //   bool operator(const auto& a, const auto& b) const { return a.second < b.second };
 // }
  
  std::transform(vds.begin(), vds.end(), res.begin(),[&](const auto& vd)
    {return  std::make_pair(vd, dijkstra_spaths(g,start,dist)[vd]);});           // <---- here!

  std::sort(res.begin(),res.end(),[&](const auto& a, const auto& b) { return a.second < b.second ; } );
  return res;
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
