#include "../common/headers.hpp"
#include "../common/utils.hpp"

const int MAXSIZE = 1 << 24;
char out[MAXSIZE], ext[MAXSIZE];
vector<string> outVector, extVector;

void getVector(char *buffer, vector<string> &v)
{
    v.clear();
    string current = "";
    int len = strlen(buffer);
    for (int i = 0; i < len; i++)
        if (buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n' || buffer[i] == '\r')
        {
            if (current != "")
                v.push_back(current);
            current = "";
        }
        else
            current += buffer[i];
    if (current != "")
        v.push_back(current);
}

void loadFile(string fileName, char *buffer, vector<string> &v)
{
    FILE *file = fopen(fileName.c_str(), "r");
    if (!file)
    {
        cerr << "Failed to open file: " << fileName << endl;
        exit(254);
    }
    fread(buffer, getFileSize(fileName), 1, file);
    getVector(buffer, v);
}

bool cmp(vector<string> &a, vector<string> &b)
{
    if (a.size() != b.size())
        return false;
    for (int i = 0; i < a.size(); i++)
        if (a[i] != b[i])
            return false;
    return true;
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        cerr << argv[0] << ": Command format error." << endl;
        exit(255); 
    }
    if (argc == 4)
    {
        cout << "No external files given, default accepted.";
        exit(0);
    }

    loadFile(argv[3], out, outVector);

    int peFlag = 0, acFlag = 0;
    for (int i = 4; i < argc; i++)
    {
        loadFile(argv[i], ext, extVector);
        if (!strcmp(ext, out))
        {
            acFlag = 1;
            break;
        }
        if (cmp(extVector, outVector))
            peFlag = 1;
    }
    if (acFlag)
    {
        cout << "Accepted";
        exit(0);
    }
    if (peFlag)
    {
        cout << "Presentation Error";
        exit(2);
    }
    cout << "Wrong Answer";
    exit(1);
}
