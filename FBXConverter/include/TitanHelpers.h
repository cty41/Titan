#ifndef __STRING_CONVERTER__HH
#define __STRING_CONVERTER__HH

#include <string>
#include <sstream>
#include "TiVector3.h"
#include "TiVector2.h"
#include "TiColor.h"

typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > stringstream;

 std::string toString(size_t val, 
	unsigned short width = 0, char fill = ' ', 
	std::ios::fmtflags flags = std::ios::fmtflags(0) );

 std::string toString(float val, unsigned short precision = 6, 
	unsigned short width = 0, char fill = ' ', 
	std::ios::fmtflags flags = std::ios::fmtflags(0) );

 int parseInt(const std::string& val);

 unsigned int parseUint(const std::string& val);

 float parseFloat(const std::string&  val);

 bool parseBool(const std::string& val);

 bool stringStartsWith(const std::string& str, const std::string& pattern);

 bool stringEndsWith(const std::string& str, const std::string& pattern);

 struct AcceptArgs
 {
	 bool convertAll;
	 bool convertAscii;
	 std::string src_file, dst_file;

	 AcceptArgs()
		 :convertAll(false), convertAscii(false){}
 };

 bool parseArgs(AcceptArgs& args, int argc, char** argv);


 typedef unsigned int RGBA;
 /*
 *	
 */
struct TiVertexData
{
	Titan::Vector3 Position;
	RGBA			Color;
	std::vector<Titan::Vector2> UVs;
	Titan::Vector3 Normal;
	
	TiVertexData()
		:Color(0)
	{
		Position = Titan::Vector3::ZERO;
		Normal = Titan::Vector3::ZERO;
	}

	inline bool operator == ( const TiVertexData& rhs ) const
	{
		if(Position == rhs.Position && Color == rhs.Color && Normal == rhs.Normal && UVs.size() == rhs.UVs.size())
		{
			for(size_t i = 0; i != UVs.size(); ++i)
			{
				if(UVs[i] != rhs.UVs[i])
					return false;
			}
			return true;
		}
		return false;
	}

	inline bool operator != ( const TiVertexData& rhs ) const
	{
		if(Position == rhs.Position && Color == rhs.Color && Normal == rhs.Normal && UVs.size() == rhs.UVs.size())
		{
			for(size_t i = 0; i != UVs.size(); ++i)
			{
				if(UVs[i] != rhs.UVs[i])
					return true;
			}
			return false;
		}
		return true;

	}
};

struct StaticMeshTriangle
{
	TiVertexData	Vertices[3];
};


enum VertexDeclSemantic
{
	kVDS_POS = 0,
	kVDS_TEXCOORD = 1,
	kVDS_NORMAL = 2,
	kVDS_BINORMAL = 3,
	kVDS_TANGENT = 4,
	kVDS_COLOR = 5,
	kVDS_UNDEFINED = 0xffffffff,
};

struct TiVertexDecl
{
	VertexDeclSemantic	Semantic;
	size_t				SemanticIndex;
	size_t				ElementSizeInBytes;

	TiVertexDecl(VertexDeclSemantic Semantic, size_t idx, size_t sizeInBytes)
		: Semantic(Semantic), SemanticIndex(idx), ElementSizeInBytes(sizeInBytes){}
};

 class TiSubMesh;

 //whole fbx exportable data stores here
 class TiScene
 {
 public:
	 TiScene(){};

	 ~TiScene()
	 {
		 ClearScene();
	 }

	 void ClearScene();

 public:
	 std::vector<TiSubMesh*>	mSubMeshes;
 };

 class TiSubMesh
 {
 public:
	 TiSubMesh(TiScene* creator, const std::string& name)
	 :mCreator(creator), mName(name){}

	 ~TiSubMesh(){};
 //protected:
	 TiScene*	mCreator;
	 std::string	mName;
	 std::vector<TiVertexData>	mVertexData;
	 std::vector<TiVertexDecl> mVertexDecls;
	 std::vector<int>	mIndexes;
 };

 int FindVertexElement(const TiSubMesh& mesh, VertexDeclSemantic kVDS, size_t idx);

 enum MeshChunkID
 {
	 MCID_Mesh = 0,
	 MCID_SubMesh = 1,
	 MCID_VertexFormat = 2,
 };


#endif