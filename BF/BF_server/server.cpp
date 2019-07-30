//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <fcntl.h>
#include <boost/filesystem.hpp>
#include "compress.hpp"
#define SERVER_ROOT_DIR "www"
#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 9999
#define SERVER_BACKUP_DIR SERVER_ROOT_DIR"/list/"
namespace bf = boost::filesystem;

CompressSave csave;
class CloudServer
{
  private:
   httplib::Server srv;
  public:
    CloudServer()
    {
      bf::path base_path(SERVER_ROOT_DIR);
      if (!bf::exists(base_path))
      {
        bf::create_directory(base_path);
      }
      bf::path list_path(SERVER_BACKUP_DIR);
      if (!bf::exists(list_path))
      {
        bf::create_directory(list_path);
      }
    }
    bool Start()
    {
     srv.set_base_dir(SERVER_ROOT_DIR);
     srv.Get("/(list(/){0,1}){0,1}",GetFileList);
     srv.Get("/list/(.*)",GetFileData);
     srv.Put("/list/(.*)",PutFileData);
     srv.listen(SERVER_IP, SERVER_PORT);
     return true;
    }
  private:
    static void PutFileData(const httplib::Request &req, httplib::Response &rsp)
    {
      //std::cout << "backup file" << req.path << std::endl;
      if (!req.has_header("Range"))
      {
        rsp.status = 400;
        return;
      }
      std::string range = req.get_header_value("Range");
      int64_t range_start;
       if (RangeParse(range, range_start) == false)
       {
         rsp.status = 400;
         return;
       }
      std::cout << "backup file:[" << req.path << "] range:[" << range << "]" << std::endl;
      std::string real = SERVER_ROOT_DIR + req.path;//bengin
      //std::ofstream file(real, std::ios::binary|std::ios::trunc); //ofstream默认截断
      csave.SetFileData(real, req.body, range_start);
      //int fd = open(real.c_str(), O_CREAT|O_WRONLY, 0664);
      //if (fd < 0)
      //{
      //  std::cerr<< "open file" << real << "error" << std::endl;
      //  rsp.status = 500;
      //  return;
      //}
      //lseek(fd, range_start, SEEK_SET);
      //size_t ret = write(fd, &req.body[0], req.body.size());
      //if (ret != req.body.size())
      //{
      //  std::cerr << "file write body error" << std::endl;
      //  rsp.status = 500;
      //  return;
      //}
      //close(fd);//end
      return;
    }
   static bool RangeParse(std::string &range, int64_t &start)
    {
      size_t pos1 = range.find("=");
      size_t pos2 = range.find("=");
      if (pos1 == std::string::npos || pos2 == std::string::npos)
      {
        std::cerr << "range:[" << range << "] format error" << std::endl;
        return false;
      }
      std::stringstream rs;
      rs << range.substr(pos1+1, pos2-pos1-1);
      rs >> start;
      return true;
    }
    static void GetFileList(const httplib::Request &req, httplib::Response &rsp)
    {
      std::vector<std::string> list;
      csave.GetFileList(list);

      //bf::path list(SERVER_BACKUP_DIR);
      //bf::directory_iterator item_begin(list);
      //bf::directory_iterator item_end;
      std::string body;
      body = "<html><body><ol><hr />";
      for (auto i : list)
      {
        //if (bf::is_directory(item_begin->status()))
        //{
        //  continue;
        //}
        bf::path path(i);
        std::string file = path.filename().string();
        std::string uri = "/list/" + file;
        body += "<h4><li>";
        body += "<a href='";
        body += uri;
        body += "'>";
        body += file;
        body += "</a>";
        body += "</li></h4>";
      }
      body += "<hr /></ol></body></html>";
      rsp.set_content(&body[0], "text/html");
      return;
    }
    static void GetFileData(const httplib::Request &req, httplib::Response &rsp)
    {
      std::string real = SERVER_ROOT_DIR + req.path;
      std::string body;
      csave.GetFileData(real, body);
      //std::string file = SERVER_ROOT_DIR + req.path;
      //if (!bf::exists(file))
      //{
      //  std::cerr << "file " << file << "is not exists" << std::endl;
      //  rsp.status = 404;
      //  return;
      //}
      //std::ifstream ifile(file, std::ios::binary);
      //if (!ifile.is_open())
      //{
      //  std::cerr << "open file " << file << "error" << std::endl;
      //  rsp.status = 500;
      //  return;
      //}
      //std::string body;
      //int64_t fsize = bf::file_size(file);
      //body.resize(fsize);
      //ifile.read(&body[0], fsize);
      //if (!ifile.good())
      //{
      //  std::cerr << "read file " << file << "body error" << std::endl;
      //  rsp.status = 500;
      //  return;
      //}
      rsp.set_content(body, "application/octet-stream");
    }
   // static void BackupFile(const httplib::Request &req, httplib::Response &rsp)
   // {
       
   //var }
};

void thr_start()
{
  csave.LowHeatFileSave();
}
int main()
{
  std::thread thr(thr_start);
  thr.detach();
  CloudServer srv;
  srv.Start();
  return 0;
}
