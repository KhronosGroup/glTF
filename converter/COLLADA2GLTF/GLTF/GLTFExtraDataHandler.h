/*
*/

#ifndef __GLTF_EXTRADATAHANDLER_H__
#define __GLTF_EXTRADATAHANDLER_H__

#include "COLLADASaxFWLIExtraDataCallbackHandler.h"

#if (defined(WIN32) || defined(_LIBCPP_VERSION) || __cplusplus > 199711L)
#include <memory>
#include <unordered_map>
#else
#include <tr1/memory>
#include <tr1/unordered_map>
#endif

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
			EXTRA_TAG_TYPE_BUMP,

            EXTRA_TAG_TYPE_ATLAS_MIN_S,
            EXTRA_TAG_TYPE_ATLAS_MAX_S,
            EXTRA_TAG_TYPE_ATLAS_MIN_T,
            EXTRA_TAG_TYPE_ATLAS_MAX_T,
            EXTRA_TAG_TYPE_ATLAS_WRAP_S,
            EXTRA_TAG_TYPE_ATLAS_WRAP_T,
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
        
        std::shared_ptr <JSONObject> getExtras(COLLADAFW::UniqueId uniqueId);
        std::shared_ptr <JSONObject> allExtras() { return this->_allExtras; }
        
        void determineBumpTextureSamplerAndTexCoord( const GeneratedSaxParser::xmlChar** attributes );

	private:
        std::shared_ptr <JSONObject> _allExtras;
	};
}


#endif //__GLTF_EXTRADATAHANDLER_H__

