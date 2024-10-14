#pragma once
#include <string>
#include <functional>

struct Context{
    std::string OutputFile="a.out";
    std::vector<std::string> LibraryPaths;
    std::vector<std::string> ArchiveFiles;
    std::vector<std::string> ObjectFiles;
};

namespace ArgParser{

enum ExpectArgStatus{
    MatchSuccess,
    MatchContinue,
    MatchFail,
};

class ExpectArg{
public:
    virtual ExpectArgStatus operator()(char*)=0;
    virtual ~ExpectArg()=default;
};

std::vector<std::string> getOptionDashform(std::string);

class ExpectedInputFileArgs : public ExpectArg{
public:
    ExpectArgStatus operator()(char*) override;
};

class ExpectedWithOptionName: public ExpectArg{
    std::string Name;
protected:
    std::function<ExpectArgStatus(char*)> onFollow;
public:
    ExpectedWithOptionName(std::string _Name,std::function<ExpectArgStatus(char*)> _onFollow):Name(_Name),onFollow(_onFollow){};
    std::string getName() const {return Name;}
};

class ExpectedFlag : public ExpectedWithOptionName{
public:
    ExpectedFlag(std::string _Name,std::function<ExpectArgStatus(char*)> _onFollow):ExpectedWithOptionName(_Name,_onFollow){};
    ExpectArgStatus operator()(char*) override;
};

// Like -o a.out --output a.out
class ExpectedWithFollow : public ExpectedWithOptionName{
    ExpectArgStatus stage=MatchFail;
public:
    ExpectedWithFollow(std::string _Name,std::function<ExpectArgStatus(char*)> _onFollow):ExpectedWithOptionName(_Name,_onFollow){};
    ExpectArgStatus operator()(char*) override;
};

class ExpectedWithPrefix : public ExpectedWithOptionName{
public:
    ExpectedWithPrefix(std::string _Name,std::function<ExpectArgStatus(char*)> _onFollow):ExpectedWithOptionName((_Name.size()==1?_Name:_Name+"="),_onFollow){};
    ExpectArgStatus operator()(char*) override;
};

void parse(int argc,char** argv);

void dumpContext();
};