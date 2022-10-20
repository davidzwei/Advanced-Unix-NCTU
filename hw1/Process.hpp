#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <map>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <set>
#include <fstream>
#include <fcntl.h>
#include <filesystem>
#include <regex>

using namespace std;

struct file_info
{
    string COMMAND = "";
    int PID = -1;
    string USER = "";
    string FD = "";
    string TYPE = "unknown";
    string NODE = "-1";
    string NAME = "";
};

string type(const filesystem::path &path)
{
    if (filesystem::is_directory(path))
        return "DIR";
    else if (filesystem::is_regular_file(path))
        return "REG";
    else if (filesystem::is_character_file(path))
        return "CHR";
    else if (filesystem::is_fifo(path))
        return "FIFO";
    else if (filesystem::is_socket(path))
        return "SOCK";
    else
        return "unknown";
}

string inode(const filesystem::path &path)
{
    int ret;
    struct stat file_stat;
    // stat:symlink inode, lstat:original inode
    ret = stat(path.c_str(), &file_stat);
    if (ret >= 0)
    {
        return to_string(file_stat.st_ino);
    }
    return "";
}

class Process
{
public:
    int pid;
    int exe_inode = 0;
    string exe_name;
    Process(){};
    Process(int pid)
    {
        this->pid = pid;
    }

    void parse(vector<struct file_info> &all_file_info)
    {
        string path = "/proc/" + to_string(pid);
        // cwd, root, exe
        parse_link(path + "/cwd", "cwd", all_file_info);
        parse_link(path + "/root", "rtd", all_file_info);
        parse_link(path + "/exe", "txt", all_file_info);
        // maps
        parse_mem(all_file_info);
        // fd
        parse_fd(all_file_info);
    }

    string find_cammand()
    {
        string path = "/proc/" + to_string(pid) + "/comm";
        ifstream file(path.c_str());
        string sLine;
        if (file.good())
        {
            getline(file, sLine); // get whole first line
            return sLine;
        }
        else
        {
            return "-1";
        }
    }

    string find_user(int uid)
    {
        if (uid == -1)
            return "-1";
        struct passwd *user;
        user = getpwuid(uid);
        return string(user->pw_name);
    }

    int get_uid()
    {
        string path = "/proc/" + to_string(pid);
        struct stat statBuf;
        if (stat(path.c_str(), &statBuf) < 0)
        {
            return -1;
        }
        return statBuf.st_uid;
    }

    // root, cwd, exe
    void parse_link(string path, string status, vector<struct file_info> &all_file_info)
    {
        struct stat buf;
        if (stat(path.c_str(), &buf) < 0)
        {
            if (errno != EACCES)
            {
                return;
            }
        }
        filesystem::path target = path;

        if (access(target.c_str(), R_OK) == 0)
        {
            string f_type = type(target);
            string f_node = inode(target);
            string file_name = filesystem::read_symlink(target);

            std::string s = " (deleted)";
            std::string::size_type i = file_name.find(s); // index
            // deleted
            if (file_name.find(s) != string::npos)
            {
                file_name.erase(i, s.length()); // remove "deleted"
                all_file_info.push_back(get_file_info(status, type(target), inode(target), file_name));
            }
            else
            {
                all_file_info.push_back(get_file_info(status, type(target), inode(target), file_name));
            }
        }
        // premission denied,  // cannnot access
        else
        {
            string f_name = target.string() + " (Permission denied)";
            all_file_info.push_back(get_file_info(status, "unknown", "", f_name));
        }
    }

    // mem, maps
    void parse_mem(vector<struct file_info> &all_file_info)
    {
        string path = "/proc/" + to_string(pid) + "/maps";
        struct stat buf;
        if (stat(path.c_str(), &buf) < 0)
        {
            if (errno != EACCES)
            {
                return;
            }
        }
        filesystem::path target = "/proc/" + to_string(pid) + "/maps";
        // can access
        if (access(target.c_str(), R_OK) == 0)
        {
            ifstream mapsFile(target);
            string line;
            set<int> inode_set; // save unique i-node
            while (getline(mapsFile, line))
            {
                stringstream ss(line);
                string map_data[7]; // 4:inode, 5:filename, 6:(deleted)
                // ss >> region >> permission >> offset >> dev >> inode >> path_name >> error_message;
                for (int i = 0; i < 7; ++i)
                    ss >> map_data[i];

                int inode = stoi(map_data[4]);

                // get unique inode
                if (inode != 0 && inode_set.find(inode) == inode_set.end())
                {
                    inode_set.insert(inode);
                    filesystem::path m_path = map_data[5];
                    if (map_data[6].find("deleted") != string::npos)
                    {
                        all_file_info.push_back(get_file_info("DEL", "unknown", map_data[4], map_data[5]));
                    }
                    else
                    {
                        all_file_info.push_back(get_file_info("mem", type(m_path), map_data[4], map_data[5]));
                    }
                }
            }
        }
    }

    // fd
    void parse_fd(vector<struct file_info> &all_file_info)
    {
        string fd = "/proc/" + to_string(pid) + "/fd";
        string fdinfo = "/proc/" + to_string(pid) + "/fdinfo";

        filesystem::path target_fd = "/proc/" + to_string(pid) + "/fd";
        filesystem::path target_fdinfo = "/proc/" + to_string(pid) + "/fdinfo";

        if (access(target_fd.c_str(), R_OK) == 0 && access(target_fdinfo.c_str(), R_OK) == 0)
        {
            for (const auto entry : filesystem::directory_iterator(target_fd))
            {
                string name = entry.path().filename();
                string fdinfo_path = fdinfo + "/" + name;
                string fdinfo_buf;
                string rwu_mode; // r, w, u
                ifstream fdinfo(fdinfo_path);
                // read flag value to buffer
                for (int i = 0; i < 4; ++i)
                    fdinfo >> fdinfo_buf;

                if (fdinfo_buf.back() == '0')
                {
                    rwu_mode = "r";
                }
                else if (fdinfo_buf.back() == '1')
                {
                    rwu_mode = "w";
                }
                else if (fdinfo_buf.back() == '2')
                {
                    rwu_mode = "u";
                }
                // symbolic
                string file_name = filesystem::read_symlink(entry.path());

                std::string s = " (deleted)";
                std::string::size_type i = file_name.find(s); // index
                // deleted
                if (file_name.find(s) != string::npos)
                {
                    file_name.erase(i, s.length());
                    all_file_info.push_back(get_file_info(name + rwu_mode, type(entry.path()), inode(entry.path()), file_name));
                }
                else
                {
                    all_file_info.push_back(get_file_info(name + rwu_mode, type(entry.path()), inode(entry.path()), file_name));
                }
            }
        }
        // cannot access fd
        else
        {
            all_file_info.push_back(get_file_info("NOFD", "", "", fd + " (Permission denied)"));
        }
    }

    file_info get_file_info(string FD, string TYPE, string NODE, string NAME)
    {
        struct file_info file_info;
        file_info.COMMAND = find_cammand();
        file_info.PID = pid;
        file_info.USER = find_user(get_uid());
        file_info.FD = FD;
        file_info.TYPE = TYPE;
        file_info.NODE = NODE == "-1" ? "" : NODE;
        file_info.NAME = NAME;
        return file_info;
    }
};
