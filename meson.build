project('hwloc-test', 'cpp', default_options : ['cpp_std=c++14'],
	version: '0.1.0',
	meson_version: '>= 0.51.0',
)

src=['src/main.cc']



cxx=meson.get_compiler('cpp')
my_dep = cxx.find_library('hwloc')

#hwloc_inc=include_directories( 'hwloc-git')
#hwloc_lib=static_library('hwloc')
#hwloc_dep=declare_dependency (include_directories : hwloc_inc,
#			#version : '>2.0.0',
#			link_with: hwloc_lib)



exe=executable('hw-test',
		sources : src,
#		dependencies : hwloc_dep,
		dependencies : my_dep,
#		include_directories: hwloc_inc
		)
test('testing hwtest', exe)
