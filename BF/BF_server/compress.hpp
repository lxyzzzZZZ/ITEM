#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <thread>
#include <boost/filesystem.hpp>
#include <zlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <boost/algorithm/string.hpp>
#include <pthread.h>
#include <sys/file.h>
#define UNGZIPFILE_PATH "www/list/"
#define GZIPFILE_PATH "www/zip/"
#define RECORD_FILE "record.list"
#define HEAT_TIME 15
namespace bf = boost::filesystem;
class CompressSave
{
  private:
    std::string _file_dir;
    std::unordered_map<std::string, std::string> _file_list;
    pthread_rwlock_t _rwlock;
  private:
    //每次压缩存储线程启动时，从文件中读取列表信息
    bool GetListRecord()
    {
      // filename gzipfilename\n
      bf::path name(RECORD_FILE);
      if (!bf::exists(name))
      {
        std::cerr << "record file is no exists" << std::endl;
        return false;
      }

      std::ifstream file(RECORD_FILE, std::ios::binary);
      if (!file.is_open())
      {
        std::cerr << "open record file read error" << std::endl;
        return false;
      }
      int64_t fsize = bf::file_size(name);
      std::string body;
      body.resize(fsize);
      file.read(&body[0], fsize);
      if (!file.good())
      {
        std::cerr << "record file body read error" << std::endl;
        return false;
      }
      file.close();
      std::vector<std::string> list;
      boost::split(list, body, boost::is_any_of("\n"));
      for (auto i : list)
      {
        size_t pos = i.find(" ");
        if (pos == std::string::npos)
        {
          continue;
        }
        std::string key = i.substr(0, pos);
        std::string val = i.substr(pos + 1);
        _file_list[key] = val;
      }
      return true;
    }
    //当存储完成，都要将列表信息储存到文件中
    bool SetListRecord()
    {
      std::stringstream tmp;
      for (auto i : _file_list)
      {
        tmp << i.first << " " << i.second << std::endl;
      }
      std::ofstream file(RECORD_FILE, std::ios::binary|std::ios::trunc);
      if (!file.is_open())
      {
        std::cerr << "record file open error" << std::endl;
        return false;
      }
      file.write(tmp.str().c_str(), tmp.str().size());
      if (!file.good())
      {
        std::cerr << "record file write body error" << std::endl;
        return false;
      }
      file.close();
      return true;
    }
    
    //目录检测，对文件进行压缩存储
    bool DirectoryCheck()
    {
      if (!bf::exists(UNGZIPFILE_PATH))
      {
        bf::create_directory(UNGZIPFILE_PATH);
      }
      bf::directory_iterator item_begin(UNGZIPFILE_PATH);
      bf::directory_iterator item_end;
      for (; item_begin != item_end; ++item_begin)
      {
        if (bf::is_directory(item_begin->status()))
        {
          continue;
        }
        std::string name = item_begin->path().string();
        //std::string gzip = UNGZIPFILE_PATH + item_begin->path().filename().string() + ".gz";
        if (IsNeedCompress(name))
        {
          std::string gzip = GZIPFILE_PATH + item_begin->path().filename().string() + ".gz";
          CompressFile(name,gzip);
          AddFileRecord(name, gzip);
        }
      }
      return true;
    }

    //判断文件是否需要压缩存储
    bool IsNeedCompress(std::string &file)
    {
      struct stat st;
      if (stat(file.c_str(), &st) < 0)
      {
        std::cerr << "get file:[" << file << "] stat error" << std::endl;
        return false;
      }
      time_t cur_time = time(NULL);
      time_t acc_time = st.st_atime;

      if ((cur_time - acc_time) < HEAT_TIME)
      {
        return false;
      }
      return true;
    }
    
    //压缩文件
    bool CompressFile(std::string &file, std::string &gzip)
    {
      int fd = open(file.c_str(), O_RDONLY);
      if (fd < 0)
      {
        std::cerr << "com open file:[" << file << "] error" << std::endl;
        return false;
      }
      //std::string gzfile = file + ".gz";
      //gzFile gf = (gzfile, "wb");
      gzFile gf = gzopen(gzip.c_str(), "wb");
      if (gf == NULL)
      {
        std::cerr << "com open gzip:[" << gzip.c_str() << "] error" << std::endl;
        return false;
      }
      int ret;
      char buf[1024];
      flock(fd, LOCK_SH);
      while ((ret = read(fd, buf, 1024)) > 0)
      {
        gzwrite(gf, buf, ret);
      }
      flock(fd, LOCK_UN);
      close(fd);
      gzclose(gf);
      unlink(file.c_str());

      //_file_list[file] = gzip.c_str();
      return true;
    }
    
    //解压缩
    bool UnCompressFile(std::string &gzip, std::string &file)
    {
      int fd = open(file.c_str(), O_CREAT|O_WRONLY, 0664);
      if (fd < 0)
      {
        std::cerr << "open file" << file << " failed" << std::endl;
        return false;
      }

      gzFile gf = gzopen(gzip.c_str(), "rb");

      if (gf == NULL)
      {
        std::cerr << "open gzip" << gzip << " failed" << std::endl;
        close(fd);
        return false;
      }

      int ret;
      char buf[1024] = {0};
      flock(fd, LOCK_EX);
      while ((ret = gzread(gf, buf, 1024)) > 0)
      {
        int len = write(fd, buf, ret);
        if (len < 0)
        {
          std::cerr << "get gzip data failed" << std::endl;
          gzclose(gf);
          close(fd);
          flock(fd, LOCK_UN);
          return false;
        }
      }
      flock(fd, LOCK_UN);
      gzclose(gf);
      close(fd);
      unlink(gzip.c_str());
      return true;
    }

    //判断文件是否已经压缩
    //bool IsCompressed(std::string &file)
    //{
    //  
    //}
    bool GetNormalFile(std::string &name, std::string &body)
    {
      int64_t fsize = bf::file_size(name);
      body.resize(fsize);
      int fd = open(name.c_str(), O_CREAT|O_RDONLY);
      if (fd < 0)
      {
        std::cerr << "open file " << name << " failed" << std::endl;
        return false;
      }
      //std::ifstream file(name, std::ios::binary);
      //if (!file.is_open())
      //{
      //  std::cerr << "open file " << name << " failed" << std::endl;
      //  return false;
      //}
      flock(fd, LOCK_SH);
      int ret = read(fd, &body[0], fsize);
      flock(fd, LOCK_UN);
      if (ret != fsize)
      {
        std::cerr << "get file " << name << " body error" << std::endl;
        close(fd);
        return false;
      }
      close(fd);
      return true;
    }
    //目录检测，获取目录中的文件名
    //
  public:
    CompressSave()
    {
      pthread_rwlock_init(&_rwlock, NULL);
      if (!bf::exists(GZIPFILE_PATH))
      {
        bf::create_directory(GZIPFILE_PATH);
      }
    }
    ~CompressSave()
    {
      pthread_rwlock_destroy(&_rwlock);
    }
    //向外提供获取文件列表接口
    bool GetFileList(std::vector<std::string> &list)
    {
      pthread_rwlock_rdlock(&_rwlock);
      for (auto i : _file_list)
      {
        list.push_back(i.first);
      }
      pthread_rwlock_unlock(&_rwlock);
      return true;
    }
    

    //通过文件名称，获取文件对应的压缩包名称
    bool GetFileGzip(std::string &file, std::string &gzip)
    {
      pthread_rwlock_rdlock(&_rwlock);
      auto it = _file_list.find(file);
      if (it == _file_list.end())
      {
        pthread_rwlock_unlock(&_rwlock);
        return false;
      }
      gzip = it->second;
      pthread_rwlock_unlock(&_rwlock);
      return true;
    }
    //向外提供获取文件数据接口
    bool GetFileData(std::string &file, std::string &body)
    {
      //非压缩文件数据获取
      if (bf::exists(file))
      {
        GetNormalFile(file, body);
      }
      //压缩文件数据获取
      else 
      {
        std::string gzip;
        GetFileGzip(file, gzip);
        UnCompressFile(gzip, file);
        GetNormalFile(file, body);
      }
      return true;
    }
    
    bool AddFileRecord(const std::string file, const std::string &gzip)
    {
      pthread_rwlock_wrlock(&_rwlock); 
      _file_list[file] = gzip;
      pthread_rwlock_unlock(&_rwlock);
      return true;
    }
    bool SetFileData(const std::string &file,const std::string &body, const int64_t offset)
    {
      int fd = open(file.c_str(), O_CREAT|O_WRONLY, 0664);
      if (fd < 0)
      {
        std::cerr << "open file " << file << " error" << std::endl;
        return false;
      }
      flock(fd, LOCK_SH);
      lseek(fd, offset, SEEK_SET);
      int ret = write(fd, &body[0], body.size());
      if (ret < 0)
      {
        std::cerr << "save file " << file << " data error" << std::endl;
        flock(fd, LOCK_UN);
        return false;
      }
      flock(fd, LOCK_UN);
      close(fd);
      AddFileRecord(file, "");
      return true;
    }
    //热度低的文件进行压缩存储
    bool LowHeatFileSave()
    {
      GetListRecord();
      while(1)
      {
        DirectoryCheck();
        SetListRecord();
        sleep(3);

      }
      return true;
    }
};
