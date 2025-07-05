#include "SampleBase.hpp"

namespace Diligent
{

static const char* VSSource = R"glsl(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

void main(in uint VertId : SV_VertexID, out PSInput PSIn) 
{
    float4 Pos[3];
    Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
    Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
    Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

    float3 Col[3];
    Col[0] = float3(1.0, 0.0, 0.0); // red
    Col[1] = float3(0.0, 1.0, 0.0); // green
    Col[2] = float3(0.0, 0.0, 1.0); // blue

    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Col[VertId];
}
)glsl";

static const char* PSSource = R"glsl(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

struct PSOutput
{ 
    float4 Color : SV_TARGET; 
};

void main(in  PSInput  PSIn,
            out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)glsl";


class M_Triangle final : public SampleBase
{
public:
    virtual void Initialize(const SampleInitInfo& InitInfo) override final;

    virtual void Render() override final;
    virtual void Update(double CurrTime, double ElapsedTime, bool DoUpdateUI) override final;

    virtual const Char* GetSampleName() const override final { return "Ming|Triangle"; }

    virtual DesiredApplicationSettings GetDesiredApplicationSettings(bool IsInitialization) override
    {
        DesiredApplicationSettings setting{};
        setting.WindowWidth  = 1280;
        setting.WindowHeight = 720;
        setting.Flags        = DesiredApplicationSettings::SETTING_FLAG_WINDOW_WIDTH | DesiredApplicationSettings::SETTING_FLAG_WINDOW_HEIGHT;

        return setting;
    }

private:
    RefCntAutoPtr<IPipelineState> m_pso;
};


void M_Triangle::Initialize(const SampleInitInfo& InitInfo)
{
    SampleBase::Initialize(InitInfo);

    GraphicsPipelineStateCreateInfo pso_info;
    pso_info.PSODesc.Name                                  = "Simple triangle PSO";
    pso_info.PSODesc.PipelineType                          = PIPELINE_TYPE_GRAPHICS;
    pso_info.GraphicsPipeline.NumRenderTargets             = 1;
    pso_info.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
    pso_info.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
    pso_info.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pso_info.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
    pso_info.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

    ShaderCreateInfo shader_info;
    shader_info.SourceLanguage                  = SHADER_SOURCE_LANGUAGE_HLSL; // Shader语言
    shader_info.Desc.UseCombinedTextureSamplers = true;

    RefCntAutoPtr<IShader> pVS;
    {
        shader_info.Desc.ShaderType = SHADER_TYPE_VERTEX;
        shader_info.Source          = VSSource;
        shader_info.EntryPoint      = "main";
        m_pDevice->CreateShader(shader_info, &pVS);
    }

    RefCntAutoPtr<IShader> pPS;
    {
        shader_info.Desc.ShaderType = SHADER_TYPE_PIXEL;
        shader_info.Source          = PSSource;
        shader_info.EntryPoint      = "main";
        m_pDevice->CreateShader(shader_info, &pPS);
    }

    pso_info.pVS = pVS;
    pso_info.pPS = pPS;
    m_pDevice->CreateGraphicsPipelineState(pso_info, &m_pso);
}

void M_Triangle::Render()
{
    const float   ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
    ITextureView* pRTV         = m_pSwapChain->GetCurrentBackBufferRTV();
    ITextureView* pDSV         = m_pSwapChain->GetDepthBufferDSV();

    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->SetPipelineState(m_pso);

    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3;
    m_pImmediateContext->Draw(drawAttrs);
}

void M_Triangle::Update(double CurrTime, double ElapsedTime, bool DoUpdateUI)
{
    SampleBase::Update(CurrTime, ElapsedTime, DoUpdateUI);
}

SampleBase* CreateSample()
{
    return new M_Triangle();
}

} // namespace Diligent
