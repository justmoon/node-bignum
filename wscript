import os

srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
    opt.tool_options('compiler_cxx')

def configure(conf):
    conf.check_tool('compiler_cxx')
    conf.env.append_value('LINKFLAGS', ['-lgmp']);
    conf.check_tool('node_addon')

def build(bld):
    obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
    obj.target = 'bigint'
    obj.source = 'bigint.cc'
    obj.cxxflags = ["-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE"]
    obj.includes = """
      %s/include
      /opt/local/include
      /usr/local/include
      /usr/include
    """ % os.getenv('PREFIX')
