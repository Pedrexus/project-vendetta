#pragma once

#include <dx12pch.h>
#include <Helpers/Settings/Settings.h>


static constexpr auto MAX_BACK_BUFFER_COUNT = 3;


class IRendered
{
    // Indicates object data has changed 
    // and we need to update the constant buffer
    u32                         _NumFramesDirty = Settings::GetInt("graphics-frame-resources");

public:
    DirectX::GraphicsResource	resources[MAX_BACK_BUFFER_COUNT];

    // IRendered(&)

    bool IsDirty() const
    {
        return _NumFramesDirty > 0;
    }
    void Clean()
    {
        _NumFramesDirty--;
    }
    void Reset()
    {
        _NumFramesDirty = Settings::GetInt("graphics-frame-resources");
    }
};


struct IElement
{
    std::string                             name;
};

struct Texture : public IElement
{
    u32                                     id;
    std::wstring                            filename;
    Microsoft::WRL::ComPtr<ID3D12Resource>  resource;
};

struct Material
{
    struct alignas(16) Constants
    {
        Vector4                             diffuseAlbedo;

        Vector3                             FresnelR0;
        f32                                 roughness;

        Matrix                              transform;
    };

    struct Element : public Constants, public IElement, public IRendered {};

    static Constants Wood;
    static Constants Ivory;
};

struct Light
{
    static constexpr u32 MAXIMUM_AMOUNT = 16; // TODO: test with a very high limit,like 1024

    enum Kind : u32
    {
        Directional,
        Point,
        Spotlight,
    };

    struct alignas(16) Constants
    {
        Vector3 position;
        f32 falloffStart;

        Vector3 direction;
        f32 falloffEnd;

        Vector3 intensity;
        f32 spotPower;

        Kind kind;
    };

    struct Element : public Constants, public IElement, public IRendered {};

    static const Constants DirectionalDefault;
    static const Constants PointDefault;
    static const Constants SpotlightDefault;
};

namespace Object
{
    struct alignas(16) Constants
    {
        Matrix world;
        Matrix textureTransform;
    };

    struct Element : public Constants, public IElement, public IRendered
    {
        std::shared_ptr<GeometricPrimitive> model; // todo: std::variant
        std::shared_ptr<Material::Element> material;
        std::shared_ptr<Texture> texture;
    };
}