# hwloc-test
aim: translate hwloc2-API topology into Boost Graph Library graph.

to compile:
make

to transform the dotfile into a graphic (.svg) one can use the script (make executably if necessary):
./makedot

the idea is to...
1) access hwloc and make a boost graph library graph (init)
2) modify it, if necessary (and then never touch it again -  one exception: updating capacity) 
3) define distances, query data, group vertices, print results
4) make a hwloc (or other) compatible view on the topology, if necessary
5) eventually try to match your HPC programs needs concerning your hardware to exploit locality and plan data / task layout or accessibility patterns

GROUPS
groups are logical vertices, not representing any hardware directly, that have in and out edges to all vertices belonging to the group. They can be applied to different levels of the hierarchy.
Changing a group will not modify the hardware topology, like rearranging and shifting parts, but simply change its edges.
It is possible to have vertices belonging to multiple groups, but should be able to query.
it is possible that some vertices will be used as partitions for memory binding/calculation for instance. Edges would then contain the amount of memory needed for a certain task.
The user might also model some vetices as buses in order to estimate bus utilisation and make according modifications.

The whole thing is practically an attempt to rewrite dyloc, in order to have a slightly different model underneath. In future iterations the two projects will inevitably converge somewhat - or just merge
hwloc is insofar limited as it can ONLY describe trees. So topologies that facilitate exchange data between branches(child nodes) cannot be adequately modelled.
