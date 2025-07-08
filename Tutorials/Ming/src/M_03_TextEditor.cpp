#include "SampleBase.hpp"
#include "imgui.h"
#include "ImguiTextEditor/TextEditor.h"

#include <fstream>

namespace Diligent
{

class M_TextEditor final : public SampleBase
{
public:
    void Initialize(const SampleInitInfo& InitInfo) override
    {
        SampleBase::Initialize(InitInfo);

        fileToEdit = "02_texture_cube.psh";
        m_editor   = std::make_shared<TextEditor>();
        editor_demo_init(*m_editor);
    }

    void Render() override
    {
        ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();

        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    void Update(double CurrTime, double ElapsedTime, bool DoUpdateUI) override
    {
        SampleBase::Update(CurrTime, ElapsedTime, DoUpdateUI);
    }

private:
    std::shared_ptr<TextEditor> m_editor;
    std::string                 fileToEdit;
    float                       ClearColor[4]   = {0.350f, 0.350f, 0.350f, 1.0f};
    bool                        bShowTextEditor = false;

    void UpdateUI() override
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        // if (ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        if (ImGui::Begin("Settings"))
        {
            ImGui::ColorEdit4("Color", ClearColor);
            ImGui::Checkbox("Show TextEditor", &bShowTextEditor);
            ImGui::Text("hello");

            if (ImGui::Button("Test"))
            {
                LOG_INFO_MESSAGE("Test");
            }
        }
        ImGui::End();

        if (bShowTextEditor)
            editor_demo_render(*m_editor, bShowTextEditor);
    }

    void editor_demo_render(TextEditor& editor, bool& open) const
    {
        auto cpos = editor.GetCursorPosition();
        ImGui::Begin("Text Editor Demo", &open, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
        ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save"))
                {
                    auto textToSave = editor.GetText();
                    /// save text....
                }
                if (ImGui::MenuItem("Quit", "Alt-F4")) return;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                bool ro = editor.IsReadOnly();
                if (ImGui::MenuItem("Read-only mode", nullptr, &ro)) editor.SetReadOnly(ro);
                ImGui::Separator();

                if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo())) editor.Undo();
                if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo())) editor.Redo();

                ImGui::Separator();

                if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection())) editor.Copy();
                if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection())) editor.Cut();
                if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection())) editor.Delete();
                if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr)) editor.Paste();

                ImGui::Separator();

                if (ImGui::MenuItem("Select all", nullptr, nullptr))
                    editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Dark palette")) editor.SetPalette(TextEditor::GetDarkPalette());
                if (ImGui::MenuItem("Light palette")) editor.SetPalette(TextEditor::GetLightPalette());
                if (ImGui::MenuItem("Retro blue palette")) editor.SetPalette(TextEditor::GetRetroBluePalette());
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                    editor.IsOverwrite() ? "Ovr" : "Ins", editor.CanUndo() ? "*" : " ", editor.GetLanguageDefinition().mName.c_str(),
                    fileToEdit.c_str());

        editor.Render("TextEditor");
        ImGui::End();
    }

    void editor_demo_init(TextEditor& editor) const
    {
        auto lang = TextEditor::LanguageDefinition::CPlusPlus();

        // set your own known preprocessor symbols...
        static const char* ppnames[] = {
            "NULL", "PM_REMOVE", "ZeroMemory", "DXGI_SWAP_EFFECT_DISCARD", "D3D_FEATURE_LEVEL", "D3D_DRIVER_TYPE_HARDWARE",
            "WINAPI", "D3D11_SDK_VERSION", "assert"};
        // ... and their corresponding values
        static const char* ppvalues[] = {"#define NULL ((void*)0)",
                                         "#define PM_REMOVE (0x0001)",
                                         "Microsoft's own memory zapper function\n(which is a macro actually)\nvoid ZeroMemory(\n\t[in] PVOID  "
                                         "Destination,\n\t[in] SIZE_T Length\n); ",
                                         "enum DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD = 0",
                                         "enum D3D_FEATURE_LEVEL",
                                         "enum D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE  = ( D3D_DRIVER_TYPE_UNKNOWN + 1 )",
                                         "#define WINAPI __stdcall",
                                         "#define D3D11_SDK_VERSION (7)",
                                         " #define assert(expression) (void)(                                                  \n"
                                         "    (!!(expression)) ||                                                              \n"
                                         "    (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \n"
                                         " )"};

        for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
        {
            TextEditor::Identifier id;
            id.mDeclaration = ppvalues[i];
            lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
        }

        // set your own identifiers
        static const char* identifiers[] = {"HWND",
                                            "HRESULT",
                                            "LPRESULT",
                                            "D3D11_RENDER_TARGET_VIEW_DESC",
                                            "DXGI_SWAP_CHAIN_DESC",
                                            "MSG",
                                            "LRESULT",
                                            "WPARAM",
                                            "LPARAM",
                                            "UINT",
                                            "LPVOID",
                                            "ID3D11Device",
                                            "ID3D11DeviceContext",
                                            "ID3D11Buffer",
                                            "ID3D11Buffer",
                                            "ID3D10Blob",
                                            "ID3D11VertexShader",
                                            "ID3D11InputLayout",
                                            "ID3D11Buffer",
                                            "ID3D10Blob",
                                            "ID3D11PixelShader",
                                            "ID3D11SamplerState",
                                            "ID3D11ShaderResourceView",
                                            "ID3D11RasterizerState",
                                            "ID3D11BlendState",
                                            "ID3D11DepthStencilState",
                                            "IDXGISwapChain",
                                            "ID3D11RenderTargetView",
                                            "ID3D11Texture2D",
                                            "TextEditor"};
        static const char* idecls[]      = {"typedef HWND_* HWND",
                                            "typedef long HRESULT",
                                            "typedef long* LPRESULT",
                                            "struct D3D11_RENDER_TARGET_VIEW_DESC",
                                            "struct DXGI_SWAP_CHAIN_DESC",
                                            "typedef tagMSG MSG\n * Message structure",
                                            "typedef LONG_PTR LRESULT",
                                            "WPARAM",
                                            "LPARAM",
                                            "UINT",
                                            "LPVOID",
                                            "ID3D11Device",
                                            "ID3D11DeviceContext",
                                            "ID3D11Buffer",
                                            "ID3D11Buffer",
                                            "ID3D10Blob",
                                            "ID3D11VertexShader",
                                            "ID3D11InputLayout",
                                            "ID3D11Buffer",
                                            "ID3D10Blob",
                                            "ID3D11PixelShader",
                                            "ID3D11SamplerState",
                                            "ID3D11ShaderResourceView",
                                            "ID3D11RasterizerState",
                                            "ID3D11BlendState",
                                            "ID3D11DepthStencilState",
                                            "IDXGISwapChain",
                                            "ID3D11RenderTargetView",
                                            "ID3D11Texture2D",
                                            "class TextEditor"};
        for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
        {
            TextEditor::Identifier id;
            id.mDeclaration = std::string(idecls[i]);
            lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
        }
        editor.SetLanguageDefinition(lang);
        // editor.SetPalette(TextEditor::GetLightPalette());

        // error markers
        TextEditor::ErrorMarkers markers;
        markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
        markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
        editor.SetErrorMarkers(markers);

        // "breakpoint" markers
        // TextEditor::Breakpoints bpts;
        // bpts.insert(24);
        // bpts.insert(47);
        // editor.SetBreakpoints(bpts);

        // static const char* fileToEdit = "ImGuiColorTextEdit/TextEditor.cpp";
        //	static const char* fileToEdit = "test.cpp";

        {
            std::ifstream t(fileToEdit);
            if (t.good())
            {
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                editor.SetText(str);
            }
        }
    }

    const Char* GetSampleName() const override { return "Ming|TextEditor"; }

    DesiredApplicationSettings GetDesiredApplicationSettings(bool IsInitialization) override
    {
        DesiredApplicationSettings setting{};
        setting.WindowWidth  = 1280;
        setting.WindowHeight = 720;
        setting.Flags        = DesiredApplicationSettings::SETTING_FLAG_WINDOW_WIDTH | DesiredApplicationSettings::SETTING_FLAG_WINDOW_HEIGHT;

        return setting;
    }
};

SampleBase* CreateSample()
{
    return new M_TextEditor();
}

} // namespace Diligent
