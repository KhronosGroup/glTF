/*
*/

#ifndef __GLTF_EXTRADATAHANDLER_H__
#define __GLTF_EXTRADATAHANDLER_H__

#include "COLLADASaxFWLIExtraDataCallbackHandler.h"

#if (defined(WIN32) || defined(_LIBCPP_VERSION))
#include <memory>
#include <unordered_map>
#else
#include <tr1/memory>
#include <tr1/unordered_map>
#endif

#ifdef _LIBCPP_VERSION
namespace std{
    namespace tr1 = std;
}
#endif

using namespace std::tr1;
using namespace std;

namespace GLTF
{
    class JSONObject;
    
	class ExtraDataHandler : public COLLADASaxFWL::IExtraDataCallbackHandler
	{
	private:
		enum ExtraTagType
		{
			EXTRA_TAG_TYPE_UNKNOWN = 0,
			EXTRA_TAG_TYPE_DOUBLE_SIDED,
            EXTRA_TAG_TYPE_LOCK_AMBIENT_DIFFUSE,
			EXTRA_TAG_TYPE_BUMP
		};

	private:
        std::string mTextBuffer;
		ExtraTagType mExtraTagType;
		COLLADAFW::UniqueId mCurrentElementUniqueId;
		COLLADAFW::Object* mCurrentObject;

	public:
		ExtraDataHandler();
		~ExtraDataHandler();
		bool elementBegin( const COLLADASaxFWL::ParserChar* elementName, const GeneratedSaxParser::xmlChar** attributes);

		bool elementEnd(const COLLADASaxFWL::ParserChar* elementName );
		bool textData(const COLLADASaxFWL::ParserChar* text, size_t textLength);

		bool parseElement( const COLLADASaxFWL::ParserChar* profileName, const COLLADASaxFWL::StringHash& elementHash
			, const COLLADAFW::UniqueId& uniqueId, COLLADAFW::Object* object );
        
        shared_ptr <JSONObject> getExtras(COLLADAFW::UniqueId uniqueId);

	private:
        shared_ptr <JSONObject> _allExtras;
	};
}


#endif //__GLTF_EXTRADATAHANDLER_H__

