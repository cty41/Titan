#ifndef  _TITAN_SCRIPT_TRANSLATOR__HH
#define  _TITAN_SCRIPT_TRANSLATOR__HH

#include "TiPrerequisites.h"
#include "TiScriptCompilerMgr.h"

namespace Titan
{
	typedef ScriptCompilerMgr::ScriptNodePtr ScriptNodePtr;
	enum ScriptTranslatorType
	{
		STT_MATERIAL = 0,
		STT_TECHNIQUE = 1,
		STT_PASS = 2,
		STT_TEXTURE_UNIT = 3,
		STT_SHADER = 4,
		STT_UNKNOWN = 200
	};

	class _DllExport ScriptTranslator : public GeneralAlloc
	{
	public:	 
		ScriptTranslator(ScriptTranslatorMgr* mgr);

		virtual ~ScriptTranslator(){}

		//we still need to implement this for every ScriptTranslator, and this method need to be called
		//almost every child ScriptTranslator
		virtual void translate(ScriptNodePtr& pScriptNode, const String& group);

		virtual const String&  getName() const = 0;

		ScriptTranslatorType	getType() const { return mType; }
	protected:
		ScriptTranslatorType	mType;
		ScriptTranslatorMgr*	mMgr;

	};

	class _DllExport MaterialTranslator: public ScriptTranslator
	{
	public:
		MaterialTranslator(ScriptTranslatorMgr* mgr);

		~MaterialTranslator(){}

		void translate(ScriptNodePtr& pScriptNode, const String& group);

		const String&  getName()  const { return mName; }

	protected:
		const String mName;
	};

	class _DllExport PassTranslator: public ScriptTranslator
	{
	public:
		PassTranslator(ScriptTranslatorMgr* mgr);

		~PassTranslator(){}

		void translate(ScriptNodePtr& pScriptNode, const String& group);

		const String&  getName()  const { return mName; }

	protected:
		void translateRenderState(ScriptNodePtr& pScriptNode, Pass* parent);

		void translateShader(ScriptNodePtr& pScriptNode, const String& group);

	protected:
		const String mName;
	};

	class _DllExport TextureUnitTranslator: public ScriptTranslator
	{
	public:
		TextureUnitTranslator(ScriptTranslatorMgr* mgr);

		~TextureUnitTranslator(){}

		void translate(ScriptNodePtr& pScriptNode, const String& group);

		const String&  getName()  const { return mName; }


	protected:
		const String mName;
	};


	//need to change, to do 
	class _DllExport ShaderTranslator: public ScriptTranslator
	{
	public:
		ShaderTranslator(ScriptTranslatorMgr* mgr);

		~ShaderTranslator(){}

		void translate(ScriptNodePtr& pScriptNode, const String& group);

		const String&  getName()  const { return mName; }

	protected:
		void translateShaderParam(ScriptNodePtr& pScriptNode, ShaderParamsPtr& params);

	protected:
		const String mName;
	};


}
#endif