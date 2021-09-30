# myloq
Translates hwloc2-API topology into Boost Graph Library graph.

to compile:
make

Assumedly hwloc (v2.5.0 tested, 2.x.x may suffice) and Boost (v1.77.0 tested) are installed. For compilation gcc v11.1.0 has been used.


To transform the dotfile into a graphic (.svg) one can use the script (make executably if necessary):
./makesvg.sh or ./makepdf.sh

the idea is to...
1) access hwloc and make a boost graph library graph (init)
2) add information, modify it, if necessary. 
   Keep in mind, that adding and removal of objects lead to iterator invalidation. Use deep copies if necessary.
3) define distances, query data, group vertices, print results
4) 
5) map your HPC programs' needs concerning your hardware to exploit locality and plan data / task layout or accessibility patterns (not included)


GROUPS
groups are logical vertices, not representing any hardware directly that have out edges to all vertices belonging to the group. They can be applied to different levels of the hierarchy.
Changing a group will not modify the hardware topology, like rearranging and shifting parts, but simply shift the groups edges.
It is possible to have vertices belonging to multiple groups, but should be able to query.
it is possible that some vertices will be used as partitions for memory binding/calculation for instance. Edges would then contain the amount of memory needed for a certain task.
(They are wildcards really... use them however you like... for instance for task requirement calculations


The idea is an attempt to rewrite dyloc (https://github.com/fuchsto/dyloc), with a different focus and no dependencies with DASH (https://github.com/fuchsto/dash). Unlike in dyloc also no compatibility to transform back to hwloc structs is planned.
The ultimate goal is to have a somehwhat intuitive and unified API to traverse and query and model hardware topology without the premise of there being trees. This may be correct for compute cores but not always (grids) and certainly not for some memory architectures, that are simply not hierarchical or dependent.
