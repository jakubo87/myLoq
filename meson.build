project('hwloc-test', 'cpp', default_options : ['cpp_std=c++14'],
	version: '0.1.0',
	meson_version: '>= 0.40.0',
)

src=['src/main.cc']



cxx=meson.get_compiler('cpp')
my_dep = cxx.find_library('hwloc', dirs : '/home/jakub/opt/hwloc-git')

exe=executable('hw-test', sources : src, dependencies : my_dep)
test('testing hwtest', exe)
