#include <goblin.h>


static goblinController* CT;


void myLogEventHandler(msgType msg,TModule mod,THandle OH,char *logText)
{
    CT -> PlainLogEventHandler(msg,mod,OH,logText);
}


int main(int ParamCount,const char *ParamStr[])
{
    CT = new goblinController();
    CT -> traceLevel = 1;

    const char* fileName = ParamStr[ParamCount-1];
    unsigned l = strlen(fileName);
    char* fileIn = new char[l+5];
    strcat(strcpy(fileIn,fileName),".gob");
    char* fileOut = new char[l+5];
    strcat(strcpy(fileOut,fileName),".rst");
    char* logFile = new char[l+5];
    strcat(strcpy(logFile,fileName),".log");

    cout << endl << "File access:" << endl; 
    int pc = CT->FindParam(ParamCount,ParamStr,"-silent");
    if (pc==0)
    {
        CT->logStream       = new ofstream(logFile);
        CT->logEventHandler = &myLogEventHandler;
        cout << " Writing transscript to " << logFile << "..." << endl; 
    }

    char *type = "unknown";
    try
    {
        goblinImport F(fileIn,*CT);
        type = F.Scan();
    }
    catch (...) {}

    abstractMixedGraph *G = NULL;

    if (strcmp(type,"graph")==0)
    {
        G = new graph(fileIn,*CT);
    }
    else if (strcmp(type,"digraph")==0)
    {
        G = new diGraph(fileIn,*CT);
    }
    else
    {
        cout << "...not a valid graph object: " << fileIn << endl << endl;
        exit(1);
    }

    cout << " ...Graph read from " << fileIn << endl; 

    CT -> traceStep = G->N();

    cout << " Writing components to " << fileOut << "..." << endl;

    CT -> Configure(ParamCount,ParamStr);
    if (CT->traceLevel>1) CT->traceLevel = 1;

    cout << endl << "Computing connected components..."; 
    char ret = G->StronglyConnected();
    cout << endl;

    G -> ReleasePredecessors();

    if (ret)
        cout << "...Graph is connected." << endl;
    else
    {
        cout << "...Graph is disconnected." << endl;

        int sh = CT->FindParam(ParamCount,ParamStr,"-sh");
        if (sh)
        {
            goblinExport F(fileOut);
            G -> WriteRegister(F,TokRegNodeColour);
        }
        else G -> Write(fileOut);
    }

    delete G;

    cout << endl << "Allocated " << goblinMaxSize << " bytes." << endl << endl;

    if (CT->logStream != &clog)
    {
        delete CT->logStream;
        CT->logStream = &clog;
    }

    delete CT;
    delete[] fileIn;
    delete[] fileOut;
    delete[] logFile;

    if (ret) return 0; else return 1;
}
