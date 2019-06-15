#include <iostream>
#include <hwloc.h> //lokale Version?

/*
std::ostream & operator<<(hwloc_topology_t t){
  std::string s;
  std::cout << s << "\n";
}

template <class T>
auto std::ostream operator<<(T t){

  return;
}
*/


int main ()
{
  int max_depth=0;
  int max_sib;
  int depth=0;
  char string[128];


  hwloc_topology_t t;
  hwloc_topology_init(&t);  // initialization
  hwloc_topology_load(t);   // actual detection


 // nbcores = hwloc_get_nbobjs_by_type(t, HWLOC_OBJ_CORE);

  //hwloc_const_cpuset_t cpuset = hwloc_topology_get_complete_cpuset(t);

  std::cout << "copied from example: hwloc style.\n";

  max_depth = hwloc_topology_get_depth(t);

  for (depth = 0; depth < max_depth; depth++) {
      printf("*** Objects at level %d\n", depth);
      for (int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
        hwloc_obj_type_snprintf(string, sizeof(string), hwloc_get_obj_by_depth(t, depth, i), 0);
        printf("Index %u: %s\n", i, string);
      }
    }

/*
    printf("*** Printing overall tree\n");
    print_children(t, hwloc_get_root_obj(t), 0); <-- doesnt work print children undeclared
*/

  std::cout << "own approach.\n";

  for (depth = 0; depth < max_depth; depth++) {
      printf("*** Objects at level %d\n", depth);
      for (int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
        hwloc_obj_type_snprintf(string, sizeof(string), hwloc_get_obj_by_depth(t, depth, i), 0);
        printf("Index %u: %s\n", i, string);
      }
    }


//  hwloc_obj_t current;
 // std::cout << "depth: " << hwloc_topology_get_complete_cpuset(t) << "\n";
 // obj->




/*

  hwloc_obj_t root = hwloc_get_root_obj(t);

  hwloc_obj_t nodes = hwloc_get_next_child(t, root, nullptr);
 // while(nullptr != )
*/
  //cleaning up:
  hwloc_topology_destroy(t);
  return 0;
}

