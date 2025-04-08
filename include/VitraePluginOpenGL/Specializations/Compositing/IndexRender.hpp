#pragma once

#include "Vitrae/Pipelines/Compositing/IndexRender.hpp"

namespace Vitrae
{
class Model;

class OpenGLComposeIndexRender : public ComposeIndexRender
{
  public:
    OpenGLComposeIndexRender(const SetupParams &params);

    std::size_t memory_cost() const override;

    const ParamList &getInputSpecs(const ParamAliases &) const override;
    const ParamList &getOutputSpecs() const override;
    const ParamList &getFilterSpecs(const ParamAliases &) const override;
    const ParamList &getConsumingSpecs(const ParamAliases &) const override;

    void extractUsedTypes(std::set<const TypeInfo *> &typeSet,
                          const ParamAliases &aliases) const override;
    void extractSubTasks(std::set<const Task *> &taskSet,
                         const ParamAliases &aliases) const override;

    void run(RenderComposeContext ctx) const override;
    void prepareRequiredLocalAssets(RenderComposeContext ctx) const override;

    StringView getFriendlyName() const override;

  protected:
    SetupParams m_params;
    ParamList m_inputSpecs, m_filterSpecs, m_consumingSpecs, m_outputSpecs;
    String m_friendlyName;

    struct SpecsPerAliases
    {
        ParamList inputSpecs, filterSpecs, consumingSpecs;
    };

    mutable StableMap<std::size_t, std::unique_ptr<SpecsPerAliases>> m_specsPerKey;

    std::size_t getSpecsKey(const ParamAliases &aliases) const;
};

} // namespace Vitrae