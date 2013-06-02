/*
*/

#ifndef __GLTFANIMATIONCONVERTER_H__
#define __GLTFANIMATIONCONVERTER_H__

namespace GLTF
{
    shared_ptr <GLTFAnimation> convertOpenCOLLADAAnimationToGLTFAnimation(const COLLADAFW::Animation* animation);
    bool writeAnimation(shared_ptr <GLTFAnimation> cvtAnimation,
                        const COLLADAFW::AnimationList::AnimationClass animationClass,
                        AnimatedTargetsSharedPtr animatedTargets,
                        std::ofstream &animationsOutputStream,
                        GLTF::GLTFConverterContext &converterContext);
}


#endif

