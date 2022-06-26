#aux_source_directory(. SRC_LIST)
set(SRC_LIST 
    mprpcapplication.cc 
    mprpcconfig.cc 
    rpcheader.pb.cc 
    rpcprovider.cc 
    mprpcchannel.cc
    mprpccontroller.cc
    # logger.cc
    # zookeeperutil.cc
    )
add_library(mprpc ${SRC_LIST})
#用了aux_source_directory() cmakelists一直没改变  只要cmakelists文件不做改动就不会重新生成makefile文件的
target_link_libraries(mprpc muduo_net muduo_base pthread )