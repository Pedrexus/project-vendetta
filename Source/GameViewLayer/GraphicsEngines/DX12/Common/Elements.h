#pragma once

#include <dx12pch.h>
#include <Helpers/Settings/Settings.h>


class IRendered
{
    // Indicates object data has changed 
    // and we need to update the constant buffer
    u32 _NumFramesDirty = Settings::GetInt("graphics-frame-resources");

public:
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
    std::wstring                            filename;
    Microsoft::WRL::ComPtr<ID3D12Resource>  resource;
};

namespace Material
{
    struct alignas(16) Constants
    {
        Vector4                             diffuseAlbedo;

        Vector3                             FresnelR0;
        f32                                 roughness;

        Matrix                              transform;
    };

    struct Element : public IElement, public IRendered, public Constants {
        static const Element Wood;
    };

    static const auto Wood = Element::Wood;
}

namespace Light
{
    static constexpr auto MAXIMUM_AMOUNT = 128; // TODO: test this limit

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

    struct Element : public IElement, public IRendered, public Constants {
        static const Element DirectionalDefault;
        static const Element PointDefault;
    };

    static const auto DirectionalDefault = Element::DirectionalDefault;
    static const Element PointDefault = Element::PointDefault;
    /*static const Element SpotlightDefault;*/
}

namespace Object
{
    struct alignas(16) Constants
    {
        Matrix world;
        Matrix textureTransform;
        
        u32 numLights;
        Light::Constants lights[Light::MAXIMUM_AMOUNT]; // could make a WorldConstants for this - all lights that affect the object
    };

    struct Element : public IElement, public IRendered 
    {
        Matrix world;
        Matrix textureTransform;

        std::shared_ptr<GeometricPrimitive> model; // todo: std::variant
        std::shared_ptr<Material::Element> material;
        std::shared_ptr<Texture> texture;

        /*Constants GetConstants(std::vector<Light::Constants>& worldLights)
        {
            Constants c;
            c.world = world;
            c.textureTransform = textureTransform;

            Quaternion rotation;
            Vector3 scale, translation;
            world.Decompose(scale, rotation, translation);

            for (u32 i = 0; i < Light::MAXIMUM_AMOUNT; i++)
                if (Vector3::Distance(worldLights[i].position, translation) < worldLights[i].falloffEnd)
                    c.lights[i] = worldLights[i];
        }*/
    };
}