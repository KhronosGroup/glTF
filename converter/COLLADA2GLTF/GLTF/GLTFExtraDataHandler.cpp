/*
*/


//#include "generated14/COLLADASaxFWLColladaParserAutoGen14Attributes.h"

//#include "GeneratedSaxParserUtils.h"

#include <vector> //FIXME: this should be included by OpenCOLLADA.
#include "../GLTFOpenCOLLADA.h"
#include "GLTFExtraDataHandler.h"
#include "GLTF.h"

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

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
	ExtraDataHandler::ExtraDataHandler() : mExtraTagType(EXTRA_TAG_TYPE_UNKNOWN)
	{
        _allExtras = shared_ptr<JSONObject> (new JSONObject());
	}

	//------------------------------
	ExtraDataHandler::~ExtraDataHandler()
	{
	}
    
    void ExtraDataHandler::determineBumpTextureSamplerAndTexCoord( const GeneratedSaxParser::xmlChar** attributes )
	{
        COLLADAFW::TextureAttributes *textureAttributes = nullptr;

        shared_ptr <JSONObject> bump = nullptr;
		if(mCurrentObject) {
			if( COLLADAFW::COLLADA_TYPE::EFFECT == mCurrentObject->getClassId()) {
                shared_ptr <JSONObject> extras = this->getExtras(mCurrentElementUniqueId);
                assert(extras);
                shared_ptr <JSONObject> textures = extras->createObjectIfNeeded("textures");
                bump = textures->createObjectIfNeeded("bump");
                
                COLLADAFW::Effect* effect = (COLLADAFW::Effect*)mCurrentObject;
                textureAttributes = effect->createExtraTextureAttributes();
			}
		}
        
        if ((bump == nullptr) || (textureAttributes == nullptr))
            return;
        
		size_t index = 0;
        
		const GeneratedSaxParser::xmlChar* attributeKey = attributes[index++];
		const GeneratedSaxParser::xmlChar* attributeValue = 0;
		while( attributeKey != 0 ) {
			attributeValue = attributes[index++];
			if( attributeValue != 0 ) {
                bump->setString(attributeKey, attributeValue);
			}
            
			if (strcmp(attributeKey, "texture") == 0) {
                textureAttributes->textureSampler = attributeValue;
            } else if (strcmp(attributeKey, "texcoord")) {
                textureAttributes->texCoord = attributeValue;
			}
			attributeKey = attributes[index++];
		}
	}

	//------------------------------
	bool ExtraDataHandler::elementBegin( const COLLADASaxFWL::ParserChar* elementName, const GeneratedSaxParser::xmlChar** attributes )
	{
        if (mExtraTagType == EXTRA_TAG_TYPE_BUMP) {
            determineBumpTextureSamplerAndTexCoord(attributes);
        }
        mExtraTagType = EXTRA_TAG_TYPE_UNKNOWN;
        
        if (strcmp(elementName, DOUBLE_SIDED) == 0) {
            //Typically, may happen in EFFECT (MAX) or GEOMETRY (MAYA)
            mExtraTagType = EXTRA_TAG_TYPE_DOUBLE_SIDED;
            return true;
        }
        
        if (strcmp(elementName, "ambient_diffuse_lock") == 0) {
            mExtraTagType = EXTRA_TAG_TYPE_LOCK_AMBIENT_DIFFUSE;
            return true;
        }
        
        
        if (strcmp(elementName, "bump") == 0) {
            mExtraTagType = EXTRA_TAG_TYPE_BUMP;
        }

        if (strcmp(elementName, "atlas_min_s") == 0) {
            mExtraTagType = EXTRA_TAG_TYPE_ATLAS_MIN_S;
        }
        else if (strcmp(elementName, "atlas_max_s") == 0) {
            mExtraTagType = EXTRA_TAG_TYPE_ATLAS_MAX_S;
        }
        else if (strcmp(elementName, "atlas_min_t") == 0) {
            mExtraTagType = EXTRA_TAG_TYPE_ATLAS_MIN_T;
        }
        else if (strcmp(elementName, "atlas_max_t") == 0) {
            mExtraTagType = EXTRA_TAG_TYPE_ATLAS_MAX_T;
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
            const  COLLADASaxFWL::ParserChar* buffer = mTextBuffer.c_str();
            shared_ptr <JSONObject> extras = getExtras(mCurrentElementUniqueId);
            bool val = GeneratedSaxParser::Utils::toBool(&buffer, failed);
            if ( !failed ) {
                extras->setBool(kDoubleSided, val);
            }
        }
        else if (mExtraTagType == EXTRA_TAG_TYPE_LOCK_AMBIENT_DIFFUSE) {
            const  COLLADASaxFWL::ParserChar* buffer = mTextBuffer.c_str();
            shared_ptr <JSONObject> extras = getExtras(mCurrentElementUniqueId);
            bool val = GeneratedSaxParser::Utils::toBool(&buffer, failed);
            if ( !failed ) {
                extras->setBool("ambient_diffuse_lock", val);
            }
        }
        else if (mExtraTagType == EXTRA_TAG_TYPE_ATLAS_MIN_S) {
            const  COLLADASaxFWL::ParserChar* buffer = mTextBuffer.c_str();
            shared_ptr <JSONObject> extras = getExtras(mCurrentElementUniqueId);
            double val = GeneratedSaxParser::Utils::toDouble(&buffer, failed);
            if (!failed) {
                shared_ptr<JSONArray> atlasMin;
                if (!extras->contains("atlas_min"))
                {
                    atlasMin = make_shared<JSONArray>();
                    atlasMin->appendValue(make_shared<JSONNumber>(0.0));
                    atlasMin->appendValue(make_shared<JSONNumber>(0.0));
                    extras->setValue("atlas_min", atlasMin);
                }
                else
                {
                    atlasMin = extras->getArray("atlas_min");
                }
                
                atlasMin->values()[0] = make_shared<JSONNumber>(val);
            }
        }
        else if (mExtraTagType == EXTRA_TAG_TYPE_ATLAS_MAX_S) {
            const  COLLADASaxFWL::ParserChar* buffer = mTextBuffer.c_str();
            shared_ptr <JSONObject> extras = getExtras(mCurrentElementUniqueId);
            double val = GeneratedSaxParser::Utils::toDouble(&buffer, failed);
            if (!failed) {
                shared_ptr<JSONArray> atlasMax;
                if (!extras->contains("atlas_max"))
                {
                    atlasMax = make_shared<JSONArray>();
                    atlasMax->appendValue(make_shared<JSONNumber>(1.0));
                    atlasMax->appendValue(make_shared<JSONNumber>(1.0));
                    extras->setValue("atlas_max", atlasMax);
                }
                else
                {
                    atlasMax = extras->getArray("atlas_max");
                }

                atlasMax->values()[0] = make_shared<JSONNumber>(val);
            }
        }
        else if (mExtraTagType == EXTRA_TAG_TYPE_ATLAS_MIN_T) {
            const  COLLADASaxFWL::ParserChar* buffer = mTextBuffer.c_str();
            shared_ptr <JSONObject> extras = getExtras(mCurrentElementUniqueId);
            double val = GeneratedSaxParser::Utils::toDouble(&buffer, failed);
            if (!failed) {
                shared_ptr<JSONArray> atlasMin;
                if (!extras->contains("atlas_min"))
                {
                    atlasMin = make_shared<JSONArray>();
                    atlasMin->appendValue(make_shared<JSONNumber>(0.0));
                    atlasMin->appendValue(make_shared<JSONNumber>(0.0));
                    extras->setValue("atlas_min", atlasMin);
                }
                else
                {
                    atlasMin = extras->getArray("atlas_min");
                }

                atlasMin->values()[1] = make_shared<JSONNumber>(val);
            }
        }
        else if (mExtraTagType == EXTRA_TAG_TYPE_ATLAS_MAX_T) {
            const  COLLADASaxFWL::ParserChar* buffer = mTextBuffer.c_str();
            shared_ptr <JSONObject> extras = getExtras(mCurrentElementUniqueId);
            double val = GeneratedSaxParser::Utils::toDouble(&buffer, failed);
            if (!failed) {
                shared_ptr<JSONArray> atlasMax;
                if (!extras->contains("atlas_max"))
                {
                    atlasMax = make_shared<JSONArray>();
                    atlasMax->appendValue(make_shared<JSONNumber>(1.0));
                    atlasMax->appendValue(make_shared<JSONNumber>(1.0));
                    extras->setValue("atlas_max", atlasMax);
                }
                else
                {
                    atlasMax = extras->getArray("atlas_max");
                }

                atlasMax->values()[1] = make_shared<JSONNumber>(val);
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

#endif
}
