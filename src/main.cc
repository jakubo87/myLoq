#include <iostream>
#include "hwloc.h" //lokale Version?
#include <vector>
#include <iostream>
#include <string>
#include <utility>

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

std::ostream & operator<<(std::ostream & os, hwloc_topology_t & t){
  //std::string s;

  int max_depth=0;
  int max_sib;
  int depth=0;
  char string[128];
  std::string s;




  std::cout << "copied from example: hwloc style.\n";

  max_depth = hwloc_topology_get_depth(t);

  for (depth = 0; depth < max_depth; depth++) {
      printf("*** Objects at level %d\n", depth);
      for (int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
        hwloc_obj_type_snprintf(string, sizeof(string), hwloc_get_obj_by_depth(t, depth, i), 0);
        //printf("Index %u: %s\n", i, string);
        s=string+'\n';
        os << std::make_pair(s,depth);
      }
    }

 // switch case

  return operator<<(os,"");
}



int main ()
{

  hwloc_topology_t t;
  hwloc_topology_init(&t);  // initialization
  hwloc_topology_load(t);   // actual detection
  std::cout << t <<"\n";
     //cleaning up:
  hwloc_topology_destroy(t);
  return 0;
}

