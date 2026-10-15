#pragma once

#include "Vitrae/Assets/BinaryImage.hpp"

#include "glad/glad.h"

namespace Vitrae
{

class OpenGLBinaryImage : public BinaryImage
{
  public:
    OpenGLBinaryImage(const SetupParams &params);
    ~OpenGLBinaryImage();

    void synchronize() override;
    bool isSynchronized() const override;
    std::size_t memory_cost() const override;

    inline GLuint getGlBufferHandle() const { return m_glBufferHandle; }

  private:
    void requestDataPtr() const override;
    void requestResizeData(std::size_t size) const override;

    GLenum getGlUsage() const;
    GLenum getGlTarget() const;

    GLuint m_glBufferHandle;
    ImageUsageHints m_usage;
};

} // namespace Vitrae
