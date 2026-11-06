#include <zmq.hpp>
#include <fstream>
#include <iostream>
#include <vector>
int main(int argc, char** argv) {
    if(argc!=3) return 2;
    std::ifstream f(argv[2],std::ios::binary);
    if(!f) return 3;
    std::vector<char> data((std::istreambuf_iterator<char>(f)),{});
    zmq::context_t ctx(1);
    zmq::socket_t s(ctx,zmq::socket_type::push);
    s.set(zmq::sockopt::linger,1000);
    s.set(zmq::sockopt::sndtimeo,3000);
    s.set(zmq::sockopt::immediate,1);
    s.connect(argv[1]);
    auto n=s.send(zmq::buffer(data),zmq::send_flags::none);
    if(!n) {std::cerr<<"send timeout\n";return 4;}
    std::cout<<"sent "<<*n<<" bytes from "<<argv[2]<<"\n";
}
