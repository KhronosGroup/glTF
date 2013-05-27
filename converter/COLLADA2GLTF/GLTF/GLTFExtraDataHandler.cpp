/*
*/


//#include "generated14/COLLADASaxFWLColladaParserAutoGen14Attributes.h"

//#include "GeneratedSaxParserUtils.h"

#include <vector> //FIXME: this should be included by OpenCOLLADA.
#include "GLTFExtraDataHandler.h"
#include "GLTF.h"

namespace GLTF
{

	static const char* DOUBLE_SIDED = "double_sided";
/*
	static const char* MAX_EXTRA_ATTRIBUTE_CAST_SHADOWS = "cast_shadows";
	static const char* MAX_EXTRA_ATTRIBUTE_COLOR_MAP_AMOUNT = "color_map_amount";
	static const char* MAX_EXTRA_ATTRIBUTE_COLOR_MAP_ON = "color_map_on";
	static const char* MAX_EXTRA_ATTRIBUTE_INTENSITY_ON = "intensity_on";
	static const char* MAX_EXTRA_ATTRIBUTE_MODE = "mode";
	static const char* MAX_EXTRA_ATTRIBUTE_MULTIPLIER = "multiplier";
	static const char* MAX_EXTRA_ATTRIBUTE_RAY_BIAS = "ray_bias";
	static const char* MAX_EXTRA_ATTRIBUTE_RAYS_PER_SAMPLE = "rays_per_sample";


	static const char* MAX_EXTRA_ATTRIBUTE_BUMP = "bump";
	static const char* MAX_EXTRA_ATTRIBUTE_TEXTURE = "texture";
*/

	//------------------------------
	ExtraDataHandler::ExtraDataHandler()
	{
        _allExtras = shared_ptr<JSONObject> (new JSONObject());
	}

	//------------------------------
	ExtraDataHandler::~ExtraDataHandler()
	{

	}

	//------------------------------
	bool ExtraDataHandler::elementBegin( const COLLADASaxFWL::ParserChar* elementName, const GeneratedSaxParser::xmlChar** attributes )
	{
        mExtraTagType = EXTRA_TAG_TYPE_UNKNOWN;

        if (strcmp(elementName, DOUBLE_SIDED) == 0) {
            //Typically, may happen in EFFECT (MAX) or GEOMETRY (MAYA)
            mExtraTagType = EXTRA_TAG_TYPE_DOUBLE_SIDED;
            return true;
        }
        
        /*
		switch ( mExtraTagType )
		{
		case EXTRA_TAG_TYPE_UNKNOWN:
			{
				if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_SKYLIGHT) == 0 )
				{
					mExtraTagType = EXTRA_TAG_TYPE_SKYLIGHT;
					mExtraParameters.skyLightParameters.castShadows = false;
					mExtraParameters.skyLightParameters.colorMapAmount = 100;
					mExtraParameters.skyLightParameters.colorMapOn = true;
					mExtraParameters.skyLightParameters.intensityOn = true;
					mExtraParameters.skyLightParameters.mode = 1.0f;
					mExtraParameters.skyLightParameters.multiplier = 1.0f;
					mExtraParameters.skyLightParameters.rayBias = 0.004999995f;
					mExtraParameters.skyLightParameters.raysPerSample = 20;
				}

				else if( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_BUMP) == 0 )
				{
					determineBumpType(attributes);
					if( mExtraParameters.bumpParameters.bumpType != BUMP_TYPE_INVALID )
					{
						mExtraTagType = EXTRA_TAG_TYPE_BUMP;
						mExtraParameters.bumpParameters.textureAttributes = 0;
					}
				}
			}
			break;
		case EXTRA_TAG_TYPE_BUMP:
			if( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_TEXTURE) == 0 )
			{
				determineBumpTextureSamplerAndTexCoord(attributes);
			}
		}*/
		return false;
	}

    
    shared_ptr <JSONObject> ExtraDataHandler::getExtras(COLLADAFW::UniqueId uniqueId)
    {
        std::string id = uniqueId.toAscii();
        
        if (!this->_allExtras->contains(id)) {
            this->_allExtras->setValue(id, shared_ptr<JSONObject> (new JSONObject()));
        }
        
        return static_pointer_cast<JSONObject>(this->_allExtras->getValue(id));
    }
    
    
	//------------------------------
	bool ExtraDataHandler::elementEnd( const COLLADASaxFWL::ParserChar* elementName )
	{
        bool failed = false;
        if (mExtraTagType == EXTRA_TAG_TYPE_DOUBLE_SIDED) {
            shared_ptr <JSONObject> extras = getExtras(mCurrentElementUniqueId);
            bool val = GeneratedSaxParser::Utils::toBool(mTextBuffer.c_str(), failed);
            if ( !failed ) {
                extras->setBool("double_sided", val);
            }
        }
        
		/*
		switch ( mExtraTagType )
		{
		case EXTRA_TAG_TYPE_SKYLIGHT:
			{
				if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_SKYLIGHT) == 0 )
				{
					mExtraTagType = EXTRA_TAG_TYPE_UNKNOWN;
					addUniqueIdSkyLightParametersPair(mCurrentElementUniqueId, mExtraParameters.skyLightParameters);
				} 
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_CAST_SHADOWS) == 0 )
				{
					bool val = GeneratedSaxParser::Utils::toBool(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.castShadows = val;
					}
				}
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_COLOR_MAP_AMOUNT) == 0 )
				{
					float val = GeneratedSaxParser::Utils::toFloat(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.colorMapAmount = val;
					}
				}
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_COLOR_MAP_ON) == 0 )
				{
					bool val = GeneratedSaxParser::Utils::toBool(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.colorMapOn = val;
					}
				}
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_INTENSITY_ON) == 0 )
				{
					bool val = GeneratedSaxParser::Utils::toBool(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.castShadows = val;
					}
				}
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_MODE) == 0 )
				{
					float val = GeneratedSaxParser::Utils::toFloat(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.mode = val;
					}
				}
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_MULTIPLIER) == 0 )
				{
					float val = GeneratedSaxParser::Utils::toFloat(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.multiplier = val;
					}
				}
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_RAY_BIAS) == 0 )
				{
					float val = GeneratedSaxParser::Utils::toFloat(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.rayBias = val;
					}
				}
				else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_RAYS_PER_SAMPLE) == 0 )
				{
					int val = GeneratedSaxParser::Utils::toSint32(mTextBuffer.c_str(), failed);
					if ( !failed )
					{
						mExtraParameters.skyLightParameters.raysPerSample = val;
					}
				}
			}
			break;
		case EXTRA_TAG_TYPE_BUMP:
			{
				if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_BUMP) == 0 )
				{
					mExtraTagType = EXTRA_TAG_TYPE_UNKNOWN;
					addUniqueIdEffectBumpMapParametersPair(mCurrentElementUniqueId, mExtraParameters.bumpParameters);
				} 
				//else if ( strcmp(elementName, MAX_EXTRA_ATTRIBUTE_...) == 0 )
				//{
				//}
			}
		}
         */

		mTextBuffer.clear();
		return true;
	}

	//------------------------------
	bool ExtraDataHandler::textData( const COLLADASaxFWL::ParserChar* text, size_t textLength )
	{
		mTextBuffer.append(text, textLength);
		return true;
	}

	//------------------------------
	bool ExtraDataHandler::parseElement( const COLLADASaxFWL::ParserChar* profileName, const COLLADASaxFWL::StringHash& elementHash
		, const COLLADAFW::UniqueId& uniqueId, COLLADAFW::Object* object )
	{
		mCurrentElementUniqueId = uniqueId;
		mCurrentObject = 0;
		if( object != 0 && object->getUniqueId() == mCurrentElementUniqueId )
			mCurrentObject = object;
        /*

		switch ( elementHash )
		{
		case COLLADASaxFWL14::HASH_ELEMENT_LIGHT:
		case COLLADASaxFWL14::HASH_ELEMENT_TECHNIQUE:
			if ( strcmp(profileName, "OpenCOLLADA3dsMax") == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		default:
			return false;
		}*/
        return true;
	}

	//------------------------------
#if 0
	void ExtraDataHandler::determineBumpType( const GeneratedSaxParser::xmlChar** attributes )
	{
		mExtraParameters.bumpParameters.bumpType = BUMP_TYPE_INVALID;

		size_t index = 0;

		const GeneratedSaxParser::xmlChar* attributeKey = attributes[index++];
		const GeneratedSaxParser::xmlChar* attributeValue = 0;
		while( attributeKey != 0 )
		{
			attributeValue = attributes[index++];
			if( strcmp(attributeKey, "bumptype") == 0 && attributeValue != 0 )
			{
				if( strcmp(attributeValue, "HEIGHTFIELD") == 0 )
					mExtraParameters.bumpParameters.bumpType = BUMP_TYPE_HEIGHTFIELD;
				break;
			}

			attributeKey = attributes[index++];
		}
	}

	//------------------------------
	void ExtraDataHandler::determineBumpTextureSamplerAndTexCoord( const GeneratedSaxParser::xmlChar** attributes )
	{
		mExtraParameters.bumpParameters.textureAttributes = 0;

		if( mCurrentObject )
		{
			if( COLLADAFW::COLLADA_TYPE::EFFECT == mCurrentObject->getClassId() )
			{
				COLLADAFW::Effect* effect = (COLLADAFW::Effect*)mCurrentObject;
				mExtraParameters.bumpParameters.textureAttributes = effect->createExtraTextureAttributes();
			}
		}

		if( mExtraParameters.bumpParameters.textureAttributes == 0 )
			return;

		size_t index = 0;

		const GeneratedSaxParser::xmlChar* attributeKey = attributes[index++];
		const GeneratedSaxParser::xmlChar* attributeValue = 0;
		while( attributeKey != 0 )
		{
			attributeValue = attributes[index++];
			if( strcmp(attributeKey, "texture") == 0 && attributeValue != 0 )
			{
				if( mExtraParameters.bumpParameters.textureAttributes )
					mExtraParameters.bumpParameters.textureAttributes->textureSampler = attributeValue;
			}
			else if( strcmp(attributeKey, "texcoord") == 0 && attributeValue != 0 )
			{
				if( mExtraParameters.bumpParameters.textureAttributes )
					mExtraParameters.bumpParameters.textureAttributes->texCoord = attributeValue;
			}

			attributeKey = attributes[index++];
		}
	}
#endif
}
