aux_source_directory(. SRC_LIST) 
add_library(mprpc SHARED ${SRC_LIST})
#用了aux_source_directory() cmakelists一直没改变  只要cmakelists文件不做改动就不会重新生成makefile文件的
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)