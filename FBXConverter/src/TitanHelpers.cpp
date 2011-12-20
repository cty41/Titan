#include "TitanHelpers.h"

std::string toString(size_t val, unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
//------------------------------------------------------------------------------//
std::string toString(float val, unsigned short precision, unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.precision(precision);
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
//------------------------------------------------------------------------------//
int parseInt(const std::string& val)
{
	// Use istringstream for direct correspondence with toString
	stringstream str(val);
	int ret = 0;
	str >> ret;

	return ret;
}
//------------------------------------------------------------------------------//
unsigned int parseUint(const std::string& val)
{
	stringstream str(val);
	unsigned int ret = 0;
	str >> ret;

	return ret;
}
//------------------------------------------------------------------------------//
float parseFloat(const std::string& val)
{
	// Use istringstream for direct correspondence with toString
	stringstream str(val);
	float ret = 0;
	str >> ret;

	return ret;
}

bool parseBool(const std::string& val)
{
	// Use istringstream for direct correspondence with toString
	stringstream str(val);
	bool ret = false;
	str >> ret;

	return ret;
}

bool stringStartsWith(const std::string& str, const std::string& pattern)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string startOfThis = str.substr(0, patternLen);
	return (startOfThis == pattern);
}

bool stringEndsWith(const std::string& str, const std::string& pattern)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string startOfThis = str.substr(str.size() - patternLen, patternLen);
	return (startOfThis == pattern);
}


bool parseArgs(AcceptArgs& args, int argc, char** argv)
{

	bool hasOutName = false, hasInName = false;

	for(int i = 1;i < argc;++i)
	{
		if(strcmp(argv[i], "-a") == 0)
		{
			args.convertAll = true;
			break;
		}
		if(stringStartsWith(argv[i], "in:"))
		{
			hasInName = true;
			args.src_file = argv[i];
			args.src_file = args.src_file.substr(3,std::string::npos);
		}
		else if(stringStartsWith(argv[i], "out:"))
		{
			hasOutName = true;
			args.dst_file = argv[i];
			args.dst_file = args.dst_file.substr(4,std::string::npos);
		}
	}
	if(args.convertAll || hasInName)
	{
		if(!hasOutName)
			args.dst_file = args.src_file;
	}
	else
		return false;

	return true;

}

void TiScene::ClearScene()
{
	for (int i = 0;i < mSubMeshes.size();++i)
	{
		delete mSubMeshes.at(i);
	}
	mSubMeshes.clear();
}

int FindVertexElement(const TiSubMesh& mesh, VertexDeclSemantic kVDS, size_t idx)
{
	for(int i = 0; i < mesh.mVertexDecls.size();++i)
	{
		if(mesh.mVertexDecls[i].Semantic == kVDS &&
			mesh.mVertexDecls[i].SemanticIndex == idx)
			return i;
	}
	return -1;
}