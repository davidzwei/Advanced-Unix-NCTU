#include "Process.hpp"

using namespace std;

// Helper function to check if a struct dirent from /proc is a PID folder.
int is_pid_folder(const struct dirent *entry)
{
    const char *p;
    for (p = entry->d_name; *p; p++)
    {
        if (!isdigit(*p))
            return 0;
    }
    return 1;
}

// argument
void get_args(int argc, char **argv, vector<int> &arg, vector<string> &reg)
{
    int cmd_opt = 0;
    char optstr[] = "c:t:f:";
    while (1)
    {
        cmd_opt = getopt(argc, argv, optstr);
        if (cmd_opt == -1)
            break;
        switch (cmd_opt)
        {
        case 'c':
            arg[0] = 1;
            reg[0] = optarg;
            break;
        case 't':
            arg[1] = 1;
            reg[1] = optarg;
            break;
        case 'f':
            arg[2] = 1;
            reg[2] = optarg;
            break;
        case '?':
            printf("error args\n");
            break;
        default:
            break;
        }
    }
}

void regex_command(vector<struct file_info> &all_file_info, string reg)
{
    regex re(reg);
    for (auto iter = all_file_info.begin(); iter != all_file_info.end();)
    {
        std::smatch m;
        if (!regex_search(iter->COMMAND, m, re))
            iter = all_file_info.erase(iter);
        else
            iter++;
    }
}

void regex_type(vector<struct file_info> &all_file_info, string reg)
{
    for (auto iter = all_file_info.begin(); iter != all_file_info.end();)
    {
        if (iter->TYPE != reg)
            iter = all_file_info.erase(iter);
        else
            iter++;
    }
}

void regex_filename(vector<struct file_info> &all_file_info, string reg)
{
    regex re(reg);
    for (auto iter = all_file_info.begin(); iter != all_file_info.end();)
    {
        std::smatch m;
        string tmp = iter->NAME;
        if (!regex_search(tmp, m, re))
            iter = all_file_info.erase(iter);
        else
            iter++;
    }
}

// main
int main(int argc, char **argv)
{
    // arg
    vector<int> arg;
    vector<string> reg;
    for (int i = 0; i < 3; i++)
    {
        arg.push_back(0);
        reg.push_back("");
    }
    get_args(argc, argv, arg, reg);

    // get pid
    DIR *procdir;
    struct dirent *entry;
    vector<int> PID_list;
    // Open /proc directory.
    procdir = opendir("/proc");
    if (!procdir)
    {
        perror("opendir failed");
        return 1;
    }
    // Iterate through all files and folders of /proc.
    while ((entry = readdir(procdir)))
    {
        if (!is_pid_folder(entry))
        {
            continue;
        }
        PID_list.push_back(atoi(entry->d_name));
    }
    // generate process and parse file
    vector<struct file_info> all_file_info;
    for (auto id : PID_list)
    {
        Process process(id);
        // error in command, user
        if (process.find_cammand() == "-1" || process.get_uid() == -1)
        {
            continue;
        }
        process.parse(all_file_info);
    }
    // choose the argument file using regex
    // c
    if (arg[0] == 1)
    {
        regex_command(all_file_info, reg[0]);
    }
    // t
    if (arg[1] == 1)
    {
        vector<string> types{"REG", "CHR", "DIR", "FIFO", "SOCK", "unknown"};
        auto it = find(types.begin(), types.end(), reg[1]);
        if (it == types.end())
        {
            cout << "Invalid TYPE option." << endl;
            exit(0);
        }
        regex_type(all_file_info, reg[1]);
    }
    // f
    if (arg[2] == 1)
    {
        regex_filename(all_file_info, reg[2]);
    }

    string exe_inode; // tmp indoe for exe, avoid duplicate
    // print all element
    printf("COMMAND\tPID\tUSER\tFD\tTYPE\tNODE\tNAME\n");
    for (auto &file_object : all_file_info)
    {
        // avoid duplicate txt, mem
        if (file_object.FD == "txt")
            exe_inode = file_object.NODE;
        else if (file_object.FD == "mem" && file_object.NODE == exe_inode)
            continue;

        cout << file_object.COMMAND << "\t";
        cout << file_object.PID << "\t";
        cout << file_object.USER << "\t";
        cout << file_object.FD << "\t";
        cout << file_object.TYPE << "\t";
        cout << file_object.NODE << "\t";
        cout << file_object.NAME << '\n';
    }

    return 0;
}