import Options
from os import unlink, symlink, popen
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.env.append_value('LINKFLAGS', '-ltempered -lhidapi-hidraw -ltempered-util'.split())

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "tempered"
  obj.source = "tempered.cpp"
  obj.cxxflags = ["-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE"]

def shutdown():
  if Options.commands['clean']:
    if exists('tempered.node'): unlink('tempered.node')
  else:
    if exists('build/Release/tempered.node') and not exists('tempered.node'):
      symlink('build/Release/tempered.node', 'tempered.node')

