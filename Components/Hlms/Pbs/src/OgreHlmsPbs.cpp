﻿/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreStableHeaders.h"

#include "OgreHlmsPbs.h"
#include "OgreHlmsPbsDatablock.h"

#include "OgreViewport.h"
#include "OgreRenderTarget.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreHighLevelGpuProgram.h"

#include "OgreSceneManager.h"
#include "Compositor/OgreCompositorShadowNode.h"
#include "Vao/OgreVaoManager.h"
#include "Vao/OgreConstBufferPacked.h"
#include "Vao/OgreTexBufferPacked.h"

#include "CommandBuffer/OgreCommandBuffer.h"
#include "CommandBuffer/OgreCbTexture.h"
#include "CommandBuffer/OgreCbShaderBuffer.h"

#include "Animation/OgreSkeletonInstance.h"

namespace Ogre
{
    const IdString PbsProperty::HwGammaRead       = IdString( "hw_gamma_read" );
    const IdString PbsProperty::HwGammaWrite      = IdString( "hw_gamma_write" );
    const IdString PbsProperty::SignedIntTex      = IdString( "signed_int_textures" );
    const IdString PbsProperty::MaterialsPerBuffer= IdString( "materials_per_buffer" );

    const IdString PbsProperty::NumTextures       = IdString( "num_textures" );
    const IdString PbsProperty::DiffuseMap        = IdString( "diffuse_map" );
    const IdString PbsProperty::NormalMapTex      = IdString( "normal_map_tex" );
    const IdString PbsProperty::SpecularMap       = IdString( "specular_map" );
    const IdString PbsProperty::RoughnessMap      = IdString( "roughness_map" );
    const IdString PbsProperty::EnvProbeMap       = IdString( "envprobe_map" );
    const IdString PbsProperty::DetailWeightMap   = IdString( "detail_weight_map" );
    const IdString PbsProperty::DetailMap0        = IdString( "detail_map0" );
    const IdString PbsProperty::DetailMap1        = IdString( "detail_map1" );
    const IdString PbsProperty::DetailMap2        = IdString( "detail_map2" );
    const IdString PbsProperty::DetailMap3        = IdString( "detail_map3" );
    const IdString PbsProperty::DetailMapNm0     = IdString( "detail_map_nm0" );
    const IdString PbsProperty::DetailMapNm1     = IdString( "detail_map_nm1" );
    const IdString PbsProperty::DetailMapNm2     = IdString( "detail_map_nm2" );
    const IdString PbsProperty::DetailMapNm3     = IdString( "detail_map_nm3" );

    const IdString PbsProperty::NormalMap         = IdString( "normal_map" );

    const IdString PbsProperty::FresnelScalar     = IdString( "fresnel_scalar" );

    const IdString PbsProperty::NormalWeight          = IdString( "normal_weight" );
    const IdString PbsProperty::NormalWeightTex       = IdString( "normal_weight_tex" );
    const IdString PbsProperty::NormalWeightDetail0   = IdString( "normal_weight_detail0" );
    const IdString PbsProperty::NormalWeightDetail1   = IdString( "normal_weight_detail1" );
    const IdString PbsProperty::NormalWeightDetail2   = IdString( "normal_weight_detail2" );
    const IdString PbsProperty::NormalWeightDetail3   = IdString( "normal_weight_detail3" );

    const IdString PbsProperty::DetailWeights     = IdString( "detail_weights" );
    const IdString PbsProperty::DetailOffsetsD0   = IdString( "detail_offsetsD0" );
    const IdString PbsProperty::DetailOffsetsD1   = IdString( "detail_offsetsD1" );
    const IdString PbsProperty::DetailOffsetsD2   = IdString( "detail_offsetsD2" );
    const IdString PbsProperty::DetailOffsetsD3   = IdString( "detail_offsetsD3" );
    const IdString PbsProperty::DetailOffsetsN0   = IdString( "detail_offsetsN0" );
    const IdString PbsProperty::DetailOffsetsN1   = IdString( "detail_offsetsN1" );
    const IdString PbsProperty::DetailOffsetsN2   = IdString( "detail_offsetsN2" );
    const IdString PbsProperty::DetailOffsetsN3   = IdString( "detail_offsetsN3" );

    const IdString PbsProperty::UvDiffuse         = IdString( "uv_diffuse" );
    const IdString PbsProperty::UvNormal          = IdString( "uv_normal" );
    const IdString PbsProperty::UvSpecular        = IdString( "uv_specular" );
    const IdString PbsProperty::UvRoughness       = IdString( "uv_roughness" );
    const IdString PbsProperty::UvDetailWeight    = IdString( "uv_detail_weight" );
    const IdString PbsProperty::UvDetail0         = IdString( "uv_detail0" );
    const IdString PbsProperty::UvDetail1         = IdString( "uv_detail1" );
    const IdString PbsProperty::UvDetail2         = IdString( "uv_detail2" );
    const IdString PbsProperty::UvDetail3         = IdString( "uv_detail3" );
    const IdString PbsProperty::UvDetailNm0       = IdString( "uv_detail_nm0" );
    const IdString PbsProperty::UvDetailNm1       = IdString( "uv_detail_nm1" );
    const IdString PbsProperty::UvDetailNm2       = IdString( "uv_detail_nm2" );
    const IdString PbsProperty::UvDetailNm3       = IdString( "uv_detail_nm3" );

    const IdString PbsProperty::BlendModeIndex0   = IdString( "blend_mode_idx0" );
    const IdString PbsProperty::BlendModeIndex1   = IdString( "blend_mode_idx1" );
    const IdString PbsProperty::BlendModeIndex2   = IdString( "blend_mode_idx2" );
    const IdString PbsProperty::BlendModeIndex3   = IdString( "blend_mode_idx3" );

    const IdString PbsProperty::DetailMapsDiffuse = IdString( "detail_maps_diffuse" );
    const IdString PbsProperty::DetailMapsNormal  = IdString( "detail_maps_normal" );
    const IdString PbsProperty::FirstValidDetailMapNm= IdString( "first_valid_detail_map_nm" );

    const IdString *PbsProperty::UvSourcePtrs[NUM_PBSM_SOURCES] =
    {
        &PbsProperty::UvDiffuse,
        &PbsProperty::UvNormal,
        &PbsProperty::UvSpecular,
        &PbsProperty::UvRoughness,
        &PbsProperty::UvDetailWeight,
        &PbsProperty::UvDetail0,
        &PbsProperty::UvDetail1,
        &PbsProperty::UvDetail2,
        &PbsProperty::UvDetail3,
        &PbsProperty::UvDetailNm0,
        &PbsProperty::UvDetailNm1,
        &PbsProperty::UvDetailNm2,
        &PbsProperty::UvDetailNm3
    };

    const IdString *PbsProperty::DetailNormalWeights[4] =
    {
        &PbsProperty::NormalWeightDetail0,
        &PbsProperty::NormalWeightDetail1,
        &PbsProperty::NormalWeightDetail2,
        &PbsProperty::NormalWeightDetail3
    };

    const IdString *PbsProperty::DetailOffsetsDPtrs[4] =
    {
        &PbsProperty::DetailOffsetsD0,
        &PbsProperty::DetailOffsetsD1,
        &PbsProperty::DetailOffsetsD2,
        &PbsProperty::DetailOffsetsD3
    };

    const IdString *PbsProperty::DetailOffsetsNPtrs[4] =
    {
        &PbsProperty::DetailOffsetsN0,
        &PbsProperty::DetailOffsetsN1,
        &PbsProperty::DetailOffsetsN2,
        &PbsProperty::DetailOffsetsN3
    };

    const IdString *PbsProperty::BlendModes[4] =
    {
        &PbsProperty::BlendModeIndex0,
        &PbsProperty::BlendModeIndex1,
        &PbsProperty::BlendModeIndex2,
        &PbsProperty::BlendModeIndex3
    };

    const IdString *PbsProperty::DetailMaps[4] =
    {
        &PbsProperty::DetailMap0,
        &PbsProperty::DetailMap1,
        &PbsProperty::DetailMap2,
        &PbsProperty::DetailMap3,
    };

    const IdString *PbsProperty::DetailMapsNm[4] =
    {
        &PbsProperty::DetailMapNm0,
        &PbsProperty::DetailMapNm1,
        &PbsProperty::DetailMapNm2,
        &PbsProperty::DetailMapNm3,
    };

    extern const String c_pbsBlendModes[];

    HlmsPbs::HlmsPbs( Archive *dataFolder ) :
        Hlms( HLMS_PBS, "pbs", dataFolder ),
        ConstBufferPool( HlmsPbsDatablock::MaterialSizeInGpuAligned,
                         ConstBufferPool::ExtraBufferParams() ),
        mCurrentPassBuffer( 0 ),
        mCurrentConstBuffer( 0 ),
        mCurrentTexBuffer( 0 ),
        mLastBoundPool( 0 ),
        mStartMappedConstBuffer( 0 ),
        mCurrentMappedConstBuffer( 0 ),
        mCurrentConstBufferSize( 0 ),
        mRealStartMappedTexBuffer( 0 ),
        mStartMappedTexBuffer( 0 ),
        mCurrentMappedTexBuffer( 0 ),
        mCurrentTexBufferSize( 0 ),
        mTexLastOffset( 0 ),
        mLastTexBufferCmdOffset( (size_t)~0 ),
        mLastTextureHash( 0 ),
        mTextureBufferDefaultSize( 4 * 1024 * 1024 )
    {
    }
    //-----------------------------------------------------------------------------------
    HlmsPbs::~HlmsPbs()
    {
        destroyAllBuffers();
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::_changeRenderSystem( RenderSystem *newRs )
    {
        if( mVaoManager )
            destroyAllBuffers();

        ConstBufferPool::_changeRenderSystem( newRs );
        Hlms::_changeRenderSystem( newRs );

        if( newRs )
        {
            HlmsDatablockMap::const_iterator itor = mDatablocks.begin();
            HlmsDatablockMap::const_iterator end  = mDatablocks.end();

            while( itor != end )
            {
                assert( dynamic_cast<HlmsPbsDatablock*>( itor->second.datablock ) );
                HlmsPbsDatablock *datablock = static_cast<HlmsPbsDatablock*>( itor->second.datablock );

                requestSlot( datablock->mTextureHash, datablock, false );
                ++itor;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    const HlmsCache* HlmsPbs::createShaderCacheEntry( uint32 renderableHash,
                                                            const HlmsCache &passCache,
                                                            uint32 finalHash,
                                                            const QueuedRenderable &queuedRenderable )
    {
        const HlmsCache *retVal = Hlms::createShaderCacheEntry( renderableHash, passCache, finalHash,
                                                                queuedRenderable );

        //Set samplers.
        GpuProgramParametersSharedPtr vsParams = retVal->vertexShader->getDefaultParameters();
        GpuProgramParametersSharedPtr psParams = retVal->pixelShader->getDefaultParameters();

        int texUnit = 1; //Vertex shader consumes 1 slot with its tbuffer.
        if( !mPreparedPass.shadowMaps.empty() )
        {
            vector<int>::type shadowMaps;
            shadowMaps.reserve( mPreparedPass.shadowMaps.size() );
            for( texUnit=0; texUnit<(int)mPreparedPass.shadowMaps.size(); ++texUnit )
                shadowMaps.push_back( texUnit );

            psParams->setNamedConstant( "texShadowMap", &shadowMaps[0], shadowMaps.size(), 1 );
        }

        assert( dynamic_cast<const HlmsPbsDatablock*>( queuedRenderable.renderable->getDatablock() ) );
        const HlmsPbsDatablock *datablock = static_cast<const HlmsPbsDatablock*>(
                                                    queuedRenderable.renderable->getDatablock() );

        int numTextures = getProperty( PbsProperty::NumTextures );
        for( int i=0; i<numTextures; ++i )
        {
            psParams->setNamedConstant( "textureMaps[" + StringConverter::toString( i ) + "]",
                                        texUnit++ );
        }

        if( getProperty( PbsProperty::EnvProbeMap ) )
        {
            assert( !datablock->getTexture( PBSM_REFLECTION ).isNull() );
            psParams->setNamedConstant( "texEnvProbeMap", texUnit++ );
        }

        vsParams->setNamedConstant( "worldMatBuf", 0 );

        mRenderSystem->_setProgramsFromHlms( retVal );

        mRenderSystem->bindGpuProgramParameters( GPT_VERTEX_PROGRAM, vsParams, GPV_ALL );
        mRenderSystem->bindGpuProgramParameters( GPT_FRAGMENT_PROGRAM, psParams, GPV_ALL );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::setDetailMapProperties( HlmsPbsDatablock *datablock, PiecesMap *inOutPieces )
    {
        uint32 minNormalMap = 4;
        bool hasDiffuseMaps = false;
        bool hasNormalMaps = false;
        bool anyDetailWeight = false;
        for( size_t i=0; i<4; ++i )
        {
            uint8 blendMode = datablock->mBlendModes[i];

            setTextureProperty( *PbsProperty::DetailMaps[i], datablock,
                                static_cast<PbsTextureTypes>( PBSM_DETAIL0 + i ) );
            setTextureProperty( *PbsProperty::DetailMapsNm[i], datablock,
                                static_cast<PbsTextureTypes>( PBSM_DETAIL0_NM + i ) );

            if( !datablock->getTexture( PBSM_DETAIL0 + i ).isNull() )
            {
                inOutPieces[PixelShader][*PbsProperty::BlendModes[i]] =
                                                "@insertpiece( " + c_pbsBlendModes[blendMode] + ")";
                hasDiffuseMaps = true;
            }

            if( !datablock->getTexture( PBSM_DETAIL0_NM + i ).isNull() )
            {
                minNormalMap = std::min<uint32>( minNormalMap, i );
                hasNormalMaps = true;
            }

            if( datablock->mDetailsOffsetScale[i] != Vector4( 0, 0, 1, 1 ) )
                setProperty( *PbsProperty::DetailOffsetsDPtrs[i], 1 );

            if( datablock->mDetailsOffsetScale[i+4] != Vector4( 0, 0, 1, 1 ) )
                setProperty( *PbsProperty::DetailOffsetsNPtrs[i], 1 );

            if( datablock->mDetailWeight[i] != 1.0f &&
                (!datablock->getTexture( PBSM_DETAIL0 + i ).isNull() ||
                 !datablock->getTexture( PBSM_DETAIL0_NM + i ).isNull()) )
            {
                anyDetailWeight = true;
            }
        }

        if( hasDiffuseMaps )
            setProperty( PbsProperty::DetailMapsDiffuse, 4 );

        if( hasNormalMaps )
            setProperty( PbsProperty::DetailMapsNormal, 4 );

        setProperty( PbsProperty::FirstValidDetailMapNm, minNormalMap );

        if( anyDetailWeight )
            setProperty( PbsProperty::DetailWeights, 1 );
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::setTextureProperty( IdString propertyName, HlmsPbsDatablock *datablock,
                                      PbsTextureTypes texType )
    {
        uint8 idx = datablock->getBakedTextureIdx( texType );
        if( idx != NUM_PBSM_TEXTURE_TYPES )
        {
            //In the template the we subtract the "+1" for the index.
            //We need to increment it now otherwise @property( diffuse_map )
            //can translate to @property( 0 ) which is not what we want.
            setProperty( propertyName, idx + 1 );
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::calculateHashForPreCreate( Renderable *renderable, PiecesMap *inOutPieces )
    {
        assert( dynamic_cast<HlmsPbsDatablock*>( renderable->getDatablock() ) );
        HlmsPbsDatablock *datablock = static_cast<HlmsPbsDatablock*>(
                                                        renderable->getDatablock() );
        setProperty( PbsProperty::FresnelScalar, datablock->hasSeparateFresnel() );

        for( size_t i=0; i<PBSM_REFLECTION; ++i )
        {
            uint8 uvSource = datablock->mUvSource[i];
            setProperty( *PbsProperty::UvSourcePtrs[i], uvSource );

            if( !datablock->getTexture( i ).isNull() &&
                getProperty( *HlmsBaseProp::UvCountPtrs[uvSource] ) < 2 )
            {
                OGRE_EXCEPT( Exception::ERR_INVALID_STATE,
                             "Renderable needs at least 2 coordinates in UV set #" +
                             StringConverter::toString( uvSource ) +
                             ". Either change the mesh, or change the UV source settings",
                             "HlmsPbs::calculateHashForPreCreate" );
            }
        }

        int numNormalWeights = 0;
        if( datablock->getNormalMapWeight() != 1.0f && !datablock->getTexture( PBSM_NORMAL ).isNull() )
        {
            setProperty( PbsProperty::NormalWeightTex, 1 );
            ++numNormalWeights;
        }

        {
            size_t validDetailMaps = 0;
            for( size_t i=0; i<4; ++i )
            {
                if( !datablock->getTexture( PBSM_DETAIL0_NM + i ).isNull() )
                {
                    if( datablock->getDetailNormalWeight( i ) != 1.0f )
                    {
                        setProperty( *PbsProperty::DetailNormalWeights[validDetailMaps], 1 );
                        ++numNormalWeights;
                    }

                    ++validDetailMaps;
                }
            }
        }

        setProperty( PbsProperty::NormalWeight, numNormalWeights );

        setDetailMapProperties( datablock, inOutPieces );

        setProperty( PbsProperty::NumTextures, datablock->mBakedTextures.size() );

        setTextureProperty( PbsProperty::DiffuseMap,    datablock,  PBSM_DIFFUSE );
        setTextureProperty( PbsProperty::NormalMapTex,  datablock,  PBSM_NORMAL );
        setTextureProperty( PbsProperty::SpecularMap,   datablock,  PBSM_SPECULAR );
        setTextureProperty( PbsProperty::RoughnessMap,  datablock,  PBSM_ROUGHNESS );
        setTextureProperty( PbsProperty::EnvProbeMap,   datablock,  PBSM_REFLECTION );
        setTextureProperty( PbsProperty::DetailWeightMap,datablock, PBSM_DETAIL_WEIGHT );

        bool usesNormalMap = !datablock->getTexture( PBSM_NORMAL ).isNull();
        for( size_t i=PBSM_DETAIL0_NM; i<=PBSM_DETAIL3_NM; ++i )
            usesNormalMap |= !datablock->getTexture( i ).isNull();
        setProperty( PbsProperty::NormalMap, usesNormalMap );

        /*setProperty( HlmsBaseProp::, !datablock->getTexture( PBSM_DETAIL0 ).isNull() );
        setProperty( HlmsBaseProp::DiffuseMap, !datablock->getTexture( PBSM_DETAIL1 ).isNull() );*/
        bool normalMapCanBeSupported = (getProperty( HlmsBaseProp::Normal ) &&
                                        getProperty( HlmsBaseProp::Tangent )) ||
                                        getProperty( HlmsBaseProp::QTangent );

        if( !normalMapCanBeSupported && usesNormalMap )
        {
            OGRE_EXCEPT( Exception::ERR_INVALID_STATE,
                         "Renderable can't use normal maps but datablock wants normal maps. "
                         "Generate Tangents for this mesh to fix the problem or use a "
                         "datablock without normal maps.", "HlmsPbs::calculateHashForPreCreate" );
        }

        String slotsPerPoolStr = StringConverter::toString( mSlotsPerPool );
        inOutPieces[VertexShader][PbsProperty::MaterialsPerBuffer] = slotsPerPoolStr;
        inOutPieces[PixelShader][PbsProperty::MaterialsPerBuffer] = slotsPerPoolStr;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::calculateHashForPreCaster( Renderable *renderable, PiecesMap *inOutPieces )
    {
        //HlmsPbsDatablock *datablock = static_cast<HlmsPbsDatablock*>(
        //                                              renderable->getDatablock() );

        HlmsPropertyVec::iterator itor = mSetProperties.begin();
        HlmsPropertyVec::iterator end  = mSetProperties.end();

        while( itor != end )
        {
            if( itor->keyName != PbsProperty::HwGammaRead &&
                itor->keyName != PbsProperty::UvDiffuse &&
                itor->keyName != HlmsBaseProp::Skeleton &&
                itor->keyName != HlmsBaseProp::BonesPerVertex &&
                itor->keyName != HlmsBaseProp::DualParaboloidMapping &&
                itor->keyName != HlmsBaseProp::AlphaTest )
            {
                itor->value = 0;
            }

            ++itor;
        }

        String slotsPerPoolStr = StringConverter::toString( mSlotsPerPool );
        inOutPieces[VertexShader][PbsProperty::MaterialsPerBuffer] = slotsPerPoolStr;
        inOutPieces[PixelShader][PbsProperty::MaterialsPerBuffer] = slotsPerPoolStr;
    }
    //-----------------------------------------------------------------------------------
    HlmsCache HlmsPbs::preparePassHash( const CompositorShadowNode *shadowNode, bool casterPass,
                                        bool dualParaboloid, SceneManager *sceneManager )
    {
        HlmsCache retVal = Hlms::preparePassHash( shadowNode, casterPass, dualParaboloid, sceneManager );

        RenderTarget *renderTarget = sceneManager->getCurrentViewport()->getTarget();

        const RenderSystemCapabilities *capabilities = mRenderSystem->getCapabilities();
        setProperty( PbsProperty::HwGammaRead, capabilities->hasCapability( RSC_HW_GAMMA ) );
        setProperty( PbsProperty::HwGammaWrite, capabilities->hasCapability( RSC_HW_GAMMA ) &&
                                                        renderTarget->isHardwareGammaEnabled() );
        setProperty( PbsProperty::SignedIntTex, capabilities->hasCapability(
                                                            RSC_TEXTURE_SIGNED_INT ) );

        retVal.setProperties = mSetProperties;

        Camera *camera = sceneManager->getCameraInProgress();
        Matrix4 viewMatrix = camera->getViewMatrix(true);

        Matrix4 projectionMatrix = camera->getProjectionMatrixWithRSDepth();

        if( renderTarget->requiresTextureFlipping() )
        {
            projectionMatrix[1][0] = -projectionMatrix[1][0];
            projectionMatrix[1][1] = -projectionMatrix[1][1];
            projectionMatrix[1][2] = -projectionMatrix[1][2];
            projectionMatrix[1][3] = -projectionMatrix[1][3];
        }

        int32 numLights             = getProperty( HlmsBaseProp::LightsSpot );
        int32 numDirectionalLights  = getProperty( HlmsBaseProp::LightsDirectional );
        int32 numShadowMaps         = getProperty( HlmsBaseProp::NumShadowMaps );
        int32 numPssmSplits         = getProperty( HlmsBaseProp::PssmSplits );

        //mat4 viewProj;
        size_t mapSize = 16 * 4;

        if( !casterPass )
        {
            //mat4 view + mat4 shadowRcv[numShadowMaps].texWorldViewProj +
            //              vec2 shadowRcv[numShadowMaps].shadowDepthRange +
            //mat3 invViewMatCubemap (upgraded to three vec4)
            mapSize += ( 16 + (16 + 2 + 2) * numShadowMaps + 4 * 3 ) * 4;
            mapSize += numPssmSplits * 4;
            mapSize = alignToNextMultiple( mapSize, 16 );

            if( shadowNode )
            {
                //Seven variables * 4 (padded vec3) * 4 (bytes) * numLights
                mapSize += ( 7 * 4 * 4 ) * numLights;
            }
            else
            {
                //Three variables * 4 (padded vec3) * 4 (bytes) * numDirectionalLights
                mapSize += ( 3 * 4 * 4 ) * numDirectionalLights;
            }
        }
        else
        {
            mapSize += (2 + 2) * 4;
        }

        //Arbitrary 16kb (minimum supported by GL), should be enough.
        const size_t maxBufferSize = 16 * 1024;

        assert( mapSize <= maxBufferSize );

        if( mCurrentPassBuffer >= mPassBuffers.size() )
        {
            mPassBuffers.push_back( mVaoManager->createConstBuffer( maxBufferSize, BT_DYNAMIC_PERSISTENT,
                                                                    0, false ) );
        }

        ConstBufferPacked *passBuffer = mPassBuffers[mCurrentPassBuffer++];
        float *passBufferPtr = reinterpret_cast<float*>( passBuffer->map( 0, mapSize ) );

#ifndef NDEBUG
        const float *startupPtr = passBufferPtr;
#endif

        //---------------------------------------------------------------------------
        //                          ---- VERTEX SHADER ----
        //---------------------------------------------------------------------------

        //mat4 viewProj;
        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        Matrix4 tmp = viewProjMatrix.transpose();
        for( size_t i=0; i<16; ++i )
            *passBufferPtr++ = (float)tmp[0][i];

        mPreparedPass.viewProjMatrix    = viewProjMatrix;
        mPreparedPass.viewMatrix        = viewMatrix;

        if( !casterPass )
        {
            //mat4 view;
            tmp = viewMatrix.transpose();
            for( size_t i=0; i<16; ++i )
                *passBufferPtr++ = (float)tmp[0][i];

            for( int32 i=0; i<numShadowMaps; ++i )
            {
                //mat4 shadowRcv[numShadowMaps].texWorldViewProj
                Matrix4 viewProjTex = shadowNode->getViewProjectionMatrix( i );
                for( size_t j=0; j<16; ++j )
                    *passBufferPtr++ = (float)viewProjTex[0][j];

                //vec2 shadowRcv[numShadowMaps].shadowDepthRange
                Real fNear, fFar;
                shadowNode->getMinMaxDepthRange( i, fNear, fFar );
                const Real depthRange = fFar - fNear;
                *passBufferPtr++ = fNear;
                *passBufferPtr++ = 1.0f / depthRange;

                passBufferPtr += 2; //Padding
            }

            //---------------------------------------------------------------------------
            //                          ---- PIXEL SHADER ----
            //---------------------------------------------------------------------------

            Matrix3 viewMatrix3, invViewMatrix3;
            viewMatrix.extract3x3Matrix( viewMatrix3 );
            invViewMatrix3 = viewMatrix3.Inverse();

            //mat3 invViewMatCubemap
            for( size_t i=0; i<9; ++i )
            {
#ifdef OGRE_GLES2_WORKAROUND_2
                Matrix3 xRot( 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, -1.0f,
                              0.0f, 1.0f, 0.0f );
                xRot = xRot * invViewMatrix3;
                *passBufferPtr++ = (float)xRot[0][i];
#else
                *passBufferPtr++ = (float)invViewMatrix3[0][i];
#endif

                //Alignment: each row/column is one vec4, despite being 3x3
                if( !( (i+1) % 3 ) )
                    ++passBufferPtr;
            }

            //float pssmSplitPoints
            for( int32 i=0; i<numPssmSplits; ++i )
                *passBufferPtr++ = (*shadowNode->getPssmSplits(0))[i];

            passBufferPtr += alignToNextMultiple( numPssmSplits, 4 ) - numPssmSplits;

            if( shadowNode )
            {
                const LightClosestArray &lights = shadowNode->getShadowCastingLights();

                for( int32 i=0; i<numLights; ++i )
                {
                    Vector4 lightPos4 = lights[i].light->getAs4DVector();
                    Vector3 lightPos = viewMatrix3 * Vector3( lightPos4.x, lightPos4.y, lightPos4.z );

                    //vec3 lights[numLights].position
                    *passBufferPtr++ = lightPos.x;
                    *passBufferPtr++ = lightPos.y;
                    *passBufferPtr++ = lightPos.z;
                    ++passBufferPtr;

                    //vec3 lights[numLights].diffuse
                    ColourValue colour = lights[i].light->getDiffuseColour() *
                                         lights[i].light->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;

                    //vec3 lights[numLights].specular
                    colour = lights[i].light->getSpecularColour() * lights[i].light->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;

                    //vec3 lights[numLights].attenuation;
                    Real attenRange     = lights[i].light->getAttenuationRange();
                    Real attenLinear    = lights[i].light->getAttenuationLinear();
                    Real attenQuadratic = lights[i].light->getAttenuationQuadric();
                    *passBufferPtr++ = attenRange;
                    *passBufferPtr++ = attenLinear;
                    *passBufferPtr++ = attenQuadratic;
                    ++passBufferPtr;

                    //vec3 lights[numLights].spotDirection;
                    Vector3 spotDir = viewMatrix3 * lights[i].light->getDerivedDirection();
                    *passBufferPtr++ = spotDir.x;
                    *passBufferPtr++ = spotDir.y;
                    *passBufferPtr++ = spotDir.z;
                    ++passBufferPtr;

                    //vec3 lights[numLights].spotParams;
                    Radian innerAngle = lights[i].light->getSpotlightInnerAngle();
                    Radian outerAngle = lights[i].light->getSpotlightOuterAngle();
                    *passBufferPtr++ = 1.0f / ( cosf( innerAngle.valueRadians() * 0.5f ) -
                                             cosf( outerAngle.valueRadians() * 0.5f ) );
                    *passBufferPtr++ = cosf( outerAngle.valueRadians() * 0.5f );
                    *passBufferPtr++ = lights[i].light->getSpotlightFalloff();
                    ++passBufferPtr;

                    if( (size_t)i < shadowNode->getLocalTextures().size() )
                    {
                        //vec2 lights[numLights].invShadowMapSize
                        //TODO: textures[0] is out of bounds when using shadow atlas. Also see how what
                        //changes need to be done so that UV calculations land on the right place
                        uint32 texWidth  = shadowNode->getLocalTextures()[i].textures[0]->getWidth();
                        uint32 texHeight = shadowNode->getLocalTextures()[i].textures[0]->getHeight();
                        *passBufferPtr++ = 1.0f / texWidth;
                        *passBufferPtr++ = 1.0f / texHeight;
                        passBufferPtr += 2;
                    }
                    else
                    {
                        //If we have 3 directional lights and two shadow mapped lights, this is possible.
                        passBufferPtr += 4;
                    }
                }
            }
            else
            {
                //No shadow maps, only pass directional lights
                const LightListInfo &globalLightList = sceneManager->getGlobalLightList();

                for( int32 i=0; i<numDirectionalLights; ++i )
                {
                    Vector4 lightPos4 = globalLightList.lights[i]->getAs4DVector();
                    Vector3 lightPos = viewMatrix3 * Vector3( lightPos4.x, lightPos4.y, lightPos4.z );

                    //vec3 lights[numLights].position
                    *passBufferPtr++ = lightPos.x;
                    *passBufferPtr++ = lightPos.y;
                    *passBufferPtr++ = lightPos.z;
                    ++passBufferPtr;

                    //vec3 lights[numLights].diffuse
                    ColourValue colour = globalLightList.lights[i]->getDiffuseColour() *
                                         globalLightList.lights[i]->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;

                    //vec3 lights[numLights].specular
                    colour = globalLightList.lights[i]->getSpecularColour() * globalLightList.lights[i]->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;
                }
            }
        }
        else
        {
            //vec2 depthRange;
            Real fNear, fFar;
            shadowNode->getMinMaxDepthRange( camera, fNear, fFar );
            const Real depthRange = fFar - fNear;
            *passBufferPtr++ = fNear;
            *passBufferPtr++ = 1.0f / depthRange;
            passBufferPtr += 2;
        }

        assert( (size_t)(passBufferPtr - startupPtr) * 4 == mapSize );

        passBuffer->unmap( UO_KEEP_PERSISTENT );

        //mTexBuffers must hold at least one buffer to prevent out of bound exceptions.
        if( mTexBuffers.empty() )
        {
            size_t bufferSize = std::min<size_t>( mTextureBufferDefaultSize,
                                                  mVaoManager->getTexBufferMaxSize() );
            TexBufferPacked *newBuffer = mVaoManager->createTexBuffer( PF_FLOAT32_RGBA, bufferSize,
                                                                       BT_DYNAMIC_PERSISTENT, 0, false );
            mTexBuffers.push_back( newBuffer );
        }

        mLastTextureHash = 0;

        mLastBoundPool = 0;

        uploadDirtyDatablocks();

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    uint32 HlmsPbs::fillBuffersFor( const HlmsCache *cache, const QueuedRenderable &queuedRenderable,
                                    bool casterPass, uint32 lastCacheHash,
                                    uint32 lastTextureHash )
    {
        OGRE_EXCEPT( Exception::ERR_NOT_IMPLEMENTED,
                     "Trying to use slow-path on a desktop implementation. "
                     "Change the RenderQueue settings.",
                     "HlmsPbs::fillBuffersFor" );
    }
    //-----------------------------------------------------------------------------------
    uint32 HlmsPbs::fillBuffersForV1( const HlmsCache *cache,
                                      const QueuedRenderable &queuedRenderable,
                                      bool casterPass, uint32 lastCacheHash,
                                      CommandBuffer *commandBuffer )
    {
        return fillBuffersFor( cache, queuedRenderable, casterPass,
                               lastCacheHash, commandBuffer, true );
    }
    //-----------------------------------------------------------------------------------
    uint32 HlmsPbs::fillBuffersForV2( const HlmsCache *cache,
                                      const QueuedRenderable &queuedRenderable,
                                      bool casterPass, uint32 lastCacheHash,
                                      CommandBuffer *commandBuffer )
    {
        return fillBuffersFor( cache, queuedRenderable, casterPass,
                               lastCacheHash, commandBuffer, false );
    }
    //-----------------------------------------------------------------------------------
    uint32 HlmsPbs::fillBuffersFor( const HlmsCache *cache, const QueuedRenderable &queuedRenderable,
                                    bool casterPass, uint32 lastCacheHash,
                                    CommandBuffer *commandBuffer, bool isV1 )
    {
        assert( dynamic_cast<const HlmsPbsDatablock*>( queuedRenderable.renderable->getDatablock() ) );
        const HlmsPbsDatablock *datablock = static_cast<const HlmsPbsDatablock*>(
                                                queuedRenderable.renderable->getDatablock() );

        if( OGRE_EXTRACT_HLMS_TYPE_FROM_CACHE_HASH( lastCacheHash ) != HLMS_PBS )
        {
            //We changed HlmsType, rebind the shared textures.
            FastArray<TexturePtr>::const_iterator itor = mPreparedPass.shadowMaps.begin();
            FastArray<TexturePtr>::const_iterator end  = mPreparedPass.shadowMaps.end();

            //layout(binding = 0) uniform PassBuffer {} pass
            ConstBufferPacked *passBuffer = mPassBuffers.back();
            *commandBuffer->addCommand<CbShaderBuffer>() = CbShaderBuffer( 0, passBuffer, 0,
                                                                           passBuffer->
                                                                           getTotalSizeBytes() );

            if( !casterPass )
            {
                size_t texUnit = 1;
                while( itor != end )
                {
                    *commandBuffer->addCommand<CbTexture>() = CbTexture( texUnit, true, itor->get() );
                    ++texUnit;
                    ++itor;
                }
            }
            else
            {
                *commandBuffer->addCommand<CbTextureDisableFrom>() = CbTextureDisableFrom( 1 );
            }

            mLastTextureHash = 0;
            mLastBoundPool = 0;
            rebindTexBuffer( commandBuffer );
        }

        if( mLastBoundPool != datablock->getAssignedPool() )
        {
            //layout(binding = 1) uniform MaterialBuf {} materialArray
            const ConstBufferPool::BufferPool *newPool = datablock->getAssignedPool();
            *commandBuffer->addCommand<CbShaderBuffer>() = CbShaderBuffer( 1, newPool->materialBuffer, 0,
                                                                           newPool->materialBuffer->
                                                                           getTotalSizeBytes() );
            mLastBoundPool = newPool;
        }

        uint32 * RESTRICT_ALIAS currentMappedConstBuffer    = mCurrentMappedConstBuffer;
        float * RESTRICT_ALIAS currentMappedTexBuffer       = mCurrentMappedTexBuffer;

        bool hasSkeletonAnimation = queuedRenderable.renderable->hasSkeletonAnimation();

        const Matrix4 &worldMat = queuedRenderable.movableObject->_getParentNodeFullTransform();

        bool exceedsConstBuffer = (size_t)((currentMappedConstBuffer - mStartMappedConstBuffer) + 4) >
                                                                                mCurrentConstBufferSize;

        //---------------------------------------------------------------------------
        //                          ---- VERTEX SHADER ----
        //---------------------------------------------------------------------------
#if !OGRE_DOUBLE_PRECISION
        if( !hasSkeletonAnimation )
        {
            const size_t minimumTexBufferSize = 16 * (1 + !casterPass);
            bool exceedsTexBuffer = (currentMappedTexBuffer - mStartMappedTexBuffer) +
                                         minimumTexBufferSize >= mCurrentTexBufferSize;

            if( exceedsConstBuffer || exceedsTexBuffer )
            {
                currentMappedConstBuffer = mapNextConstBuffer( commandBuffer );

                if( exceedsTexBuffer )
                    mapNextTexBuffer( commandBuffer, minimumTexBufferSize * sizeof(float) );
                else
                    rebindTexBuffer( commandBuffer, true, minimumTexBufferSize * sizeof(float) );

                currentMappedTexBuffer = mCurrentMappedTexBuffer;
            }

            //uint worldMaterialIdx[]
            *currentMappedConstBuffer = datablock->getAssignedSlot() & 0x1FF;
            currentMappedConstBuffer += 4;

            //mat4 worldViewProj
            Matrix4 tmp = mPreparedPass.viewProjMatrix * worldMat;
    #ifdef OGRE_GLES2_WORKAROUND_1
            tmp = tmp.transpose();
    #endif
            memcpy( currentMappedTexBuffer, &tmp, sizeof(Matrix4) );
            currentMappedTexBuffer += 16;

            //mat4 worldView
            tmp = mPreparedPass.viewMatrix.concatenateAffine( worldMat );
    #ifdef OGRE_GLES2_WORKAROUND_1
            tmp = tmp.transpose();
    #endif
            memcpy( currentMappedTexBuffer, &tmp, sizeof(Matrix4) * !casterPass );
            currentMappedTexBuffer += 16 * !casterPass;
        }
        else
        {
            if( isV1 )
            {
                uint16 numWorldTransforms = queuedRenderable.renderable->getNumWorldTransforms();
                assert( numWorldTransforms <= 256 );

                const size_t minimumTexBufferSize = 12 * numWorldTransforms;
                bool exceedsTexBuffer = (currentMappedTexBuffer - mStartMappedTexBuffer) +
                        minimumTexBufferSize >= mCurrentTexBufferSize;

                if( exceedsConstBuffer || exceedsTexBuffer )
                {
                    currentMappedConstBuffer = mapNextConstBuffer( commandBuffer );

                    if( exceedsTexBuffer )
                        mapNextTexBuffer( commandBuffer, minimumTexBufferSize * sizeof(float) );
                    else
                        rebindTexBuffer( commandBuffer, true, minimumTexBufferSize * sizeof(float) );

                    currentMappedTexBuffer = mCurrentMappedTexBuffer;
                }

                //uint worldMaterialIdx[]
                size_t distToWorldMatStart = mCurrentMappedTexBuffer - mStartMappedTexBuffer;
                distToWorldMatStart >>= 2;
                *currentMappedConstBuffer = (distToWorldMatStart << 9 ) |
                        (datablock->getAssignedSlot() & 0x1FF);
                currentMappedConstBuffer += 4;

                //vec4 worldMat[][3]
                //TODO: Don't rely on a virtual function + make a direct 4x3 copy
                Matrix4 tmp[256];
                queuedRenderable.renderable->getWorldTransforms( tmp );
                for( size_t i=0; i<numWorldTransforms; ++i )
                {
                    memcpy( currentMappedTexBuffer, &tmp[i], 12 * sizeof(float) );
                    currentMappedTexBuffer += 12;
                }
            }
            else
            {
                SkeletonInstance *skeleton = queuedRenderable.movableObject->getSkeletonInstance();

#if OGRE_DEBUG_MODE
                assert( dynamic_cast<const RenderableAnimated*>( queuedRenderable.renderable ) );
#endif

                const RenderableAnimated *renderableAnimated = static_cast<const RenderableAnimated*>(
                                                                        queuedRenderable.renderable );

                const RenderableAnimated::IndexMap *indexMap = renderableAnimated->getBlendIndexToBoneIndexMap();

                const size_t minimumTexBufferSize = 12 * indexMap->size();
                bool exceedsTexBuffer = (currentMappedTexBuffer - mStartMappedTexBuffer) +
                                            minimumTexBufferSize >= mCurrentTexBufferSize;

                if( exceedsConstBuffer || exceedsTexBuffer )
                {
                    currentMappedConstBuffer = mapNextConstBuffer( commandBuffer );

                    if( exceedsTexBuffer )
                        mapNextTexBuffer( commandBuffer, minimumTexBufferSize * sizeof(float) );
                    else
                        rebindTexBuffer( commandBuffer, true, minimumTexBufferSize * sizeof(float) );

                    currentMappedTexBuffer = mCurrentMappedTexBuffer;
                }

                //uint worldMaterialIdx[]
                size_t distToWorldMatStart = mCurrentMappedTexBuffer - mStartMappedTexBuffer;
                distToWorldMatStart >>= 2;
                *currentMappedConstBuffer = (distToWorldMatStart << 9 ) |
                        (datablock->getAssignedSlot() & 0x1FF);
                currentMappedConstBuffer += 4;

                RenderableAnimated::IndexMap::const_iterator itBone = indexMap->begin();
                RenderableAnimated::IndexMap::const_iterator enBone = indexMap->end();

                while( itBone != enBone )
                {
                    const SimpleMatrixAf4x3 &mat4x3 = skeleton->_getBoneFullTransform( *itBone );
                    Matrix4 tmp2;
                    mat4x3.store( &tmp2 );
                    memcpy( currentMappedTexBuffer, tmp2[0], 12 * sizeof(float) );
                    //mat4x3.streamTo4x3( currentMappedTexBuffer );
                    currentMappedTexBuffer += 12;

                    ++itBone;
                }
            }
        }
#else
    #error Not Coded Yet! (cannot use memcpy on Matrix4)
#endif

        //---------------------------------------------------------------------------
        //                          ---- PIXEL SHADER ----
        //---------------------------------------------------------------------------

        if( !casterPass )
        {
            if( datablock->mTextureHash != mLastTextureHash )
            {
                //Rebind textures
                size_t texUnit = mPreparedPass.shadowMaps.size() + 1;

                PbsBakedTextureArray::const_iterator itor = datablock->mBakedTextures.begin();
                PbsBakedTextureArray::const_iterator end  = datablock->mBakedTextures.end();

                while( itor != end )
                {
                    *commandBuffer->addCommand<CbTexture>() =
                            CbTexture( texUnit++, true, itor->texture.get(), itor->samplerBlock );
                    ++itor;
                }

                *commandBuffer->addCommand<CbTextureDisableFrom>() = CbTextureDisableFrom( texUnit );

                mLastTextureHash = datablock->mTextureHash;
            }
        }

        mCurrentMappedConstBuffer   = currentMappedConstBuffer;
        mCurrentMappedTexBuffer     = currentMappedTexBuffer;

        return ((mCurrentMappedConstBuffer - mStartMappedConstBuffer) >> 2) - 1;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::unmapConstBuffer(void)
    {
        if( mStartMappedConstBuffer )
        {
            //Unmap the current buffer
            ConstBufferPacked *constBuffer = mConstBuffers[mCurrentConstBuffer];
            constBuffer->unmap( UO_KEEP_PERSISTENT, 0,
                                (mCurrentMappedConstBuffer - mStartMappedConstBuffer) * sizeof(uint32) );

            ++mCurrentConstBuffer;

            mStartMappedConstBuffer     = 0;
            mCurrentMappedConstBuffer   = 0;
            mCurrentConstBufferSize     = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    DECL_MALLOC uint32* HlmsPbs::mapNextConstBuffer( CommandBuffer *commandBuffer )
    {
        unmapConstBuffer();

        if( mCurrentConstBuffer >= mConstBuffers.size() )
        {
            size_t bufferSize = std::min<size_t>( 65535, mVaoManager->getConstBufferMaxSize() );
            ConstBufferPacked *newBuffer = mVaoManager->createConstBuffer( bufferSize,
                                                                           BT_DYNAMIC_PERSISTENT,
                                                                           0, false );
            mConstBuffers.push_back( newBuffer );
        }

        ConstBufferPacked *constBuffer = mConstBuffers[mCurrentConstBuffer];

        mStartMappedConstBuffer     = reinterpret_cast<uint32*>(
                                            constBuffer->map( 0, constBuffer->getNumElements() ) );
        mCurrentMappedConstBuffer   = mStartMappedConstBuffer;
        mCurrentConstBufferSize     = constBuffer->getNumElements() >> 2;

        *commandBuffer->addCommand<CbShaderBuffer>() = CbShaderBuffer( 2, constBuffer, 0, 0 );

        return mStartMappedConstBuffer;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::unmapTexBuffer( CommandBuffer *commandBuffer )
    {
        //Save our progress
        mTexLastOffset = (mCurrentMappedTexBuffer - mStartMappedTexBuffer) * sizeof(float);

        if( mStartMappedTexBuffer )
        {
            //Unmap the current buffer
            TexBufferPacked *texBuffer = mTexBuffers[mCurrentTexBuffer];
            texBuffer->unmap( UO_KEEP_PERSISTENT, 0, mTexLastOffset );

            CbShaderBuffer *shaderBufferCmd = reinterpret_cast<CbShaderBuffer*>(
                        commandBuffer->getCommandFromOffset( mLastTexBufferCmdOffset ) );
            if( shaderBufferCmd )
            {
                assert( shaderBufferCmd->bufferPacked == texBuffer );
                shaderBufferCmd->bindSizeBytes = mTexLastOffset;
                mLastTexBufferCmdOffset = (size_t)~0;
            }
        }

        mRealStartMappedTexBuffer = 0;
        mStartMappedTexBuffer   = 0;
        mCurrentMappedTexBuffer = 0;
        mCurrentTexBufferSize   = 0;

        //Ensure the proper alignment
        mTexLastOffset = alignToNextMultiple( mTexLastOffset, mVaoManager->getTexBufferAlignment() );
    }
    //-----------------------------------------------------------------------------------
    DECL_MALLOC float* HlmsPbs::mapNextTexBuffer( CommandBuffer *commandBuffer, size_t minimumSizeBytes )
    {
        unmapTexBuffer( commandBuffer );

        TexBufferPacked *texBuffer = mTexBuffers[mCurrentTexBuffer];

        mTexLastOffset = alignToNextMultiple( mTexLastOffset, mVaoManager->getTexBufferAlignment() );

        //We'll go out of bounds. This buffer is full. Get a new one and remap from 0.
        if( mTexLastOffset + minimumSizeBytes >= texBuffer->getTotalSizeBytes() )
        {
            mTexLastOffset = 0;
            ++mCurrentTexBuffer;

            if( mCurrentTexBuffer >= mTexBuffers.size() )
            {
                size_t bufferSize = std::min<size_t>( mTextureBufferDefaultSize,
                                                      mVaoManager->getTexBufferMaxSize() );
                TexBufferPacked *newBuffer = mVaoManager->createTexBuffer( PF_FLOAT32_RGBA, bufferSize,
                                                                           BT_DYNAMIC_PERSISTENT,
                                                                           0, false );
                mTexBuffers.push_back( newBuffer );
            }

            texBuffer = mTexBuffers[mCurrentTexBuffer];
        }

        mRealStartMappedTexBuffer   = reinterpret_cast<float*>(
                                            texBuffer->map( mTexLastOffset,
                                                            texBuffer->getNumElements() - mTexLastOffset,
                                                            false ) );
        mStartMappedTexBuffer   = mRealStartMappedTexBuffer;
        mCurrentMappedTexBuffer = mRealStartMappedTexBuffer;
        mCurrentTexBufferSize   = (texBuffer->getNumElements() - mTexLastOffset) >> 2;

        CbShaderBuffer *shaderBufferCmd = commandBuffer->addCommand<CbShaderBuffer>();
        *shaderBufferCmd = CbShaderBuffer( 0, texBuffer, 0, 0 );

        mLastTexBufferCmdOffset = commandBuffer->getCommandOffset( shaderBufferCmd );

        return mStartMappedTexBuffer;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::rebindTexBuffer( CommandBuffer *commandBuffer, bool resetOffset,
                                   size_t minimumSizeBytes )
    {
        assert( minimumSizeBytes > 0 );

        //Set the binding size of the old binding command (if exists)
        CbShaderBuffer *shaderBufferCmd = reinterpret_cast<CbShaderBuffer*>(
                    commandBuffer->getCommandFromOffset( mLastTexBufferCmdOffset ) );
        if( shaderBufferCmd )
        {
            assert( shaderBufferCmd->bufferPacked == mTexBuffers[mCurrentTexBuffer] );
            shaderBufferCmd->bindSizeBytes = (mCurrentMappedTexBuffer - mStartMappedTexBuffer) *
                                                sizeof(float);
        }

        const size_t bufferSizeBytes = mCurrentTexBufferSize * sizeof(float);
        size_t currentOffset = (mCurrentMappedTexBuffer - mStartMappedTexBuffer) * sizeof(float);
        currentOffset = alignToNextMultiple( currentOffset, mVaoManager->getTexBufferAlignment() );
        currentOffset = std::min( bufferSizeBytes, currentOffset );
        const size_t remainingSize = bufferSizeBytes - currentOffset;

        if( resetOffset && remainingSize < minimumSizeBytes )
        {
            mapNextTexBuffer( commandBuffer, minimumSizeBytes );
        }
        else
        {
            size_t bindOffset = (mStartMappedTexBuffer - mRealStartMappedTexBuffer) * sizeof(float);
            if( resetOffset )
            {
                mStartMappedTexBuffer = reinterpret_cast<float*>(
                            reinterpret_cast<unsigned char*>(mStartMappedTexBuffer) + currentOffset );
                mCurrentMappedTexBuffer = mStartMappedTexBuffer;
                mCurrentTexBufferSize -= currentOffset / sizeof(float);

                bindOffset = (mCurrentMappedTexBuffer - mRealStartMappedTexBuffer) * sizeof(float);
            }

            //Add a new binding command.
            shaderBufferCmd = commandBuffer->addCommand<CbShaderBuffer>();
            *shaderBufferCmd = CbShaderBuffer( 0, mTexBuffers[mCurrentTexBuffer], bindOffset, 0 );
            mLastTexBufferCmdOffset = commandBuffer->getCommandOffset( shaderBufferCmd );
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::destroyAllBuffers(void)
    {
        mCurrentPassBuffer  = 0;
        mCurrentConstBuffer = 0;
        mCurrentTexBuffer   = 0;
        mTexLastOffset      = 0;

        {
            TexBufferPackedVec::const_iterator itor = mTexBuffers.begin();
            TexBufferPackedVec::const_iterator end  = mTexBuffers.end();

            while( itor != end )
            {
                if( (*itor)->getMappingState() != MS_UNMAPPED )
                    (*itor)->unmap( UO_UNMAP_ALL );
                mVaoManager->destroyTexBuffer( *itor );
                ++itor;
            }

            mTexBuffers.clear();
        }

        {
            ConstBufferPackedVec::const_iterator itor = mConstBuffers.begin();
            ConstBufferPackedVec::const_iterator end  = mConstBuffers.end();

            while( itor != end )
            {
                if( (*itor)->getMappingState() != MS_UNMAPPED )
                    (*itor)->unmap( UO_UNMAP_ALL );
                mVaoManager->destroyConstBuffer( *itor );
                ++itor;
            }

            mConstBuffers.clear();
        }

        {
            ConstBufferPackedVec::const_iterator itor = mPassBuffers.begin();
            ConstBufferPackedVec::const_iterator end  = mPassBuffers.end();

            while( itor != end )
            {
                if( (*itor)->getMappingState() != MS_UNMAPPED )
                    (*itor)->unmap( UO_UNMAP_ALL );
                mVaoManager->destroyConstBuffer( *itor );
                ++itor;
            }

            mPassBuffers.clear();
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::preCommandBufferExecution( CommandBuffer *commandBuffer )
    {
        unmapConstBuffer();
        unmapTexBuffer( commandBuffer );

        TexBufferPackedVec::const_iterator itor = mTexBuffers.begin();
        TexBufferPackedVec::const_iterator end  = mTexBuffers.end();

        while( itor != end )
        {
            (*itor)->advanceFrame();
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::postCommandBufferExecution( CommandBuffer *commandBuffer )
    {
        TexBufferPackedVec::const_iterator itor = mTexBuffers.begin();
        TexBufferPackedVec::const_iterator end  = mTexBuffers.end();

        while( itor != end )
        {
            (*itor)->regressFrame();
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::frameEnded(void)
    {
        mCurrentPassBuffer  = 0;
        mCurrentConstBuffer = 0;
        mCurrentTexBuffer   = 0;
        mTexLastOffset      = 0;

        TexBufferPackedVec::const_iterator itor = mTexBuffers.begin();
        TexBufferPackedVec::const_iterator end  = mTexBuffers.end();

        while( itor != end )
        {
            (*itor)->advanceFrame();
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::setTextureBufferDefaultSize( size_t defaultSize )
    {
        mTextureBufferDefaultSize = defaultSize;
    }
    //-----------------------------------------------------------------------------------
    HlmsDatablock* HlmsPbs::createDatablockImpl( IdString datablockName,
                                                       const HlmsMacroblock *macroblock,
                                                       const HlmsBlendblock *blendblock,
                                                       const HlmsParamVec &paramVec )
    {
        return OGRE_NEW HlmsPbsDatablock( datablockName, this, macroblock, blendblock, paramVec );
    }
}