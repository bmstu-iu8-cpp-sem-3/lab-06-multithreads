// Copyright 2020 Your Name <your_email>

#include <boost/log/trivial.hpp>
#include <chrono>
#include <hasher.hpp>

struct Data {
  std::string str;
  std::string hash;
  std::size_t time;
  Data(std::size_t time_, const std::string& str_,
       const std::string& hash_);
};

json BrutFSha::data = {};

Data::Data(std::size_t time_, const std::string& str_,
           const std::string& hash_)
    : str(str_), hash(hash_), time(time_) {}

void to_json(json& j, const Data& d) {
  j = json{{"time", d.time},
           {"hash", d.hash},
           {"data", d.str}};
}

bool is_correct_ssh(const std::string& s){
  for( std::string::const_iterator it = s.end() - 1; it!=s.end() - 5 ; --it){
    if (*it != '0')
      return false;
  }
  return true;
}

void BrutFSha::work_threads(){
  std::vector<std::thread> vec_thr;
  for (unsigned int i = 0; i < n_threads; i++){
    vec_thr.push_back(std::thread(&BrutFSha::research_sha,this));
  }
  for (unsigned int i = 0; i < n_threads; i++){
    vec_thr[i].join();
  }
}

std::size_t BrutFSha::time() {
  auto curr = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::seconds>(
      curr.time_since_epoch()).count();
}

[[noreturn]] void BrutFSha::research_sha(){
  thread_local std::mt19937 g(std::random_device{}());
  while(true){
    std::string rand_str = std::to_string(g());
    const std::string hash
        = picosha2::hash256_hex_string(rand_str.begin(), rand_str.end());
    if(is_correct_ssh(hash)){
      BOOST_LOG_TRIVIAL(info) << "Thread id: "
                              << std::this_thread::get_id() << std::endl
                              << "Found suitable data '" << rand_str
                              << "' hash '" << hash << "'\n";
      std::lock_guard<std::mutex> lock(Mutex);
      BrutFSha::data.push_back(Data(time(), rand_str, hash));
    } else{
      BOOST_LOG_TRIVIAL(trace) << "Thread id: "
                               << std::this_thread::get_id() << std::endl
                               << "Found unsuitable data '" << rand_str
                               << "' hash '" << hash << std::endl;
    }
  }
}



