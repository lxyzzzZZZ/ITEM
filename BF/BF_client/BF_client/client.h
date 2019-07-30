#pragma once
//检测目录中文件信息
//判断是否要备份
//备份信息
//记录文件备份信息 
//etag = mtime - fsize stringstream 
//ss << filename<<" "
//ss << std::hex<<mtime<<"-"<<std::hex<<fsize
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <thread>
#include "httplib.h"
#define CLIENT_BACKUP_DIR "backup"
#define CLIENT_BACKUP_INFO_FILE "back.list"
#define SERVER_IP "192.168.221.135"
#define SERVER_PORT 9999
#define BACKUP_URI "/list/"
//分块大小 10Mb
#define RANGE_MAX_SIZE (10 << 20)
namespace bf = boost::filesystem;

class ThrBackUp
{
private:
	std::string _file;
	int64_t _range_start;
	int64_t _range_len;
public:
	bool _res;
public:
	ThrBackUp(const std::string &file, int64_t start, int64_t len)
		:_res(true)
		,_file(file)
		,_range_start(start)
		,_range_len(len)
	{}
	void Start()
	{
		std::stringstream s;
		s << "backup file" << _file << "range:[" << _range_start << "-" << _range_len << "]" << std::endl;
		std::cout << s.str();
		std::ifstream path(_file, std::ios::binary);
		if (!path.is_open())
		{
			std::cerr << "range backup file" << _file << "failed" << std::endl;
			_res = false;
			return;
		}
		//跳转到range的起始位置
		path.seekg(_range_start, std::ios::beg);
		std::string body;
		body.resize(_range_len);
		//读取文件中range分块的文件数据
		path.read(&body[0], _range_len);
		if (!path.good())
		{
			std::cerr << "read file" << _file << "range data failed" << std::endl;
			_res = false;
			return;
		}
		path.close();

		//上传range数据
		bf::path name(_file);
		//组织上传的uri路径 method url version
		//PUT  /list/filename HTTP/1.1
		std::string url = BACKUP_URI + name.filename().string();
		//实例化一个httplib的客户端对象
		httplib::Client cli(SERVER_IP, SERVER_PORT);
		//定义http请求头信息
		httplib::Headers hdr;
		hdr.insert(std::make_pair("Content-Length", std::to_string(_range_len)));
		std::stringstream tmp;
		tmp << "bytes=" << _range_start << "-" << (_range_start + _range_len - 1);
		hdr.insert(std::make_pair("Range", tmp.str().c_str()));
		//通过实例化的Client向服务端发送PUT请求
		auto rsp = cli.Put(url.c_str(), hdr, body, "text/plain");
		if (rsp && rsp->status == 200)
		{
			_res = true;
		}
		else
		{
			std::cerr << "Start error" << std::endl;
			_res = false;
		}
		return;
	}
};
class Client
{
private:
	std::unordered_map<std::string, std::string> _backup_list;
public:
	Client()
	{
		bf::path file(CLIENT_BACKUP_DIR);
		if (!bf::exists(file))
		{
			bf::create_directory(file);
		}
	}
private:
	bool GetListInfo()
	{
		bf::path path(CLIENT_BACKUP_INFO_FILE);
		if (!bf::exists(path))
		{
			std::cerr << "List File " << path.string() << "is not exist" << std::endl;
			return false;
		}
		int64_t fsize = bf::file_size(path);
		if (0 == fsize)
		{
			std::cerr << "backup no backup info" << std::endl;
			return false;
		}
		std::string body;
		body.resize(fsize);
		std::ifstream file(CLIENT_BACKUP_INFO_FILE, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "List File Open Defeat" << std::endl;
			return false;
		}
		file.read(&body[0], fsize);
		if (!file.good())
		{
			std::cerr << "Read List File Body Defeat" << std::endl;
			return false;
		}
		file.close();
		std::vector<std::string> list;
		//以\n为界分割
		boost::split(list, body, boost::is_any_of("\n"));

		for (auto i : list)
		{
			size_t pos = i.find(" ");
			if (pos == std::string::npos)
			{
				continue;
			}
			std::string name = i.substr(0, pos);
			std::string val = i.substr(pos + 1);
			_backup_list[name] = val;
		}
		return true;
	}
	bool SetListInfo()
	{
		std::string body;
		for (auto i : _backup_list)
		{
			body += i.first + " " + i.second + "\n";
		}
		std::ofstream file(CLIENT_BACKUP_INFO_FILE, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Open List File Defeat" << std::endl;
			return false;
		}

		file.write(&body[0], body.size());
		if (!file.good())
		{
			std::cerr << "Set Backup Info Defeat" << std::endl;
			return false;
		}
		file.close();

		return true;
	}
	bool BackupDirListen(const std::string &path)
	{
		bf::path file(path);
		bf::directory_iterator item_begin(file);
		bf::directory_iterator item_end;

		for (; item_begin != item_end; ++item_begin)
		{
			if (bf::is_directory(item_begin->status()))
			{
				BackupDirListen(item_begin->path().string());
				continue;
			}
			if (FileBackup_IsNeed(item_begin->path().string()) == false)
			{
				continue;
			}
			std::cerr << "file:[" << item_begin->path().string() << "] need backup" << std::endl;
			if (PutFileData(item_begin->path().string()) == false)
			{
				continue;
			}
			std::cout << "==================" << std::endl;
			AddBackInfo(item_begin->path().string());
		}
		return true;
	}
	bool AddBackInfo(const std::string &file)
	{
		// etag = "mtime  filesize"
		std::string etag;
		if (GetFileEtag(file, etag) == false)
		{
			std::cerr << "Get File " << file << " Etag Error" << std::endl;
			return false;
		}
		_backup_list[file] = etag;
		std::cout << "add back info" << std::endl;
		return true;
	}
	bool GetFileEtag(const std::string &file, std::string& etag)
	{
		bf::path path(file);
		if (!bf::exists(path))
		{
			std::cerr << "GetFileEtag" << file << "not exists" << std::endl;
			return false;
		}
		int64_t fsize = bf::file_size(path);
		int64_t mtime = bf::last_write_time(path);
		std::stringstream tmp;
		tmp << std::hex << fsize << "-" << std::hex << mtime;
		etag = tmp.str();
		return true;
	}
	bool FileBackup_IsNeed(const std::string &file)
	{
		std::string etag;
		if (GetFileEtag(file, etag) == false)
		{
			std::cerr << "Get File " << file << " Etag Error" << std::endl;
			return false;
		}
		auto it = _backup_list.find(file);
		//判断是否要备份		
		if (it != _backup_list.end() && it->second == etag)
			return false;
		return true;
	}
	bool PutFileData(const std::string &file)
	{
		//分块传输大小对文件内容进行方块
		//通过获取分块传输是否成功判断整个文件是否成功上传
		//选择多线程处理

		//获取文件大小
		int64_t fsize = bf::file_size(file);
		if (fsize <= 0)
		{
			std::cerr << "File " << file << " unnecessary backup" << std::endl;
			return false;
		}
		//计算需要分分多少块，得到每块大小以及起始位置
				//循环创建线程，线程上传文件数据
		int count = (int)(fsize / RANGE_MAX_SIZE);
		std::vector<ThrBackUp> thr_res;
		std::vector<std::thread> thr_list;
		for (int i = 0; i <= count; i++)
		{
			int64_t range_start = i * RANGE_MAX_SIZE;
			int64_t range_end = ((i + 1) * RANGE_MAX_SIZE) - 1;
			if (i == count)
			{
				range_end = fsize - 1;
			}
			int64_t range_len = range_end - range_start + 1;
			ThrBackUp backuo_info(file, range_start, range_len);
			//std::thread thr(thr_start, file, range_start, range_len, &thr_res[i]);
			//thr_list.push_back(thr);
			std::cerr << "file:[" << file << "]range:[" << range_start << "-" << range_end << "-" << range_len << std::endl;
			thr_res.push_back(backuo_info);
		}
		for (int i = 0; i <= count; i++)
		{
			thr_list.push_back(std::thread(thr_start, &thr_res[i]));
		}

		//等待所有线程退出，判断文件上传结果-》上传成功，则添加文件的备份信息记录
		bool ret = true;
		for (int i = 0; i <= count; i++)
		{
			thr_list[i].join();
			if (thr_res[i]._res == true)
			{
				continue;
			}
			ret = false;
		}
		if (ret == false)
			return false;
		//AddBackInfo(file);
		std::cerr << "file:[" << file << "] backup success" << std::endl;
		return true;
	}
	static void thr_start(ThrBackUp *backup_info)
	{
		backup_info->Start();
		std::cout << "into thr_start" << std::endl;
		return;
	}

public:
	bool Start()
	{
		GetListInfo();
		while (1)
		{
			BackupDirListen(CLIENT_BACKUP_DIR);
			//std::cout << "Listen over" << std::endl;
			SetListInfo();
			//std::cout << "SetListInfo Over" << std::endl;
			Sleep(3000);
		}
		return true;
	}

};