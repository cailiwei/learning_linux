flags = [
  '-Wall',
  '-Wextra',
  '-Werror',
  '-Wno-long-long',
  '-Wno-variadic-macros',
  '-fexceptions',
  '-DNDEBUG',
  '-std=c++11',
  '-x',
  'c++',
  '-I',
  '/usr/include',
  '-isystem',
  '/usr/lib/gcc/x86_64-linux-gnu/5/include',
  '-isystem',
  '/usr/include/x86_64-linux-gnu',
  '-isystem'
  '/usr/include/c++/5',
  '-isystem',
  '/usr/include/c++/5/bits',
  '-isystem',
  os.path.join(home_dir, 'workspace/Code_Opensource/kernel'),
  '-isystem',
  os.path.join(home_dir, 'workspace/Code_Opensource/kernel/include')
]

