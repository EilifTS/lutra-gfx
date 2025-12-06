#pragma once
#include "math/Camera2D.h"
#include "math/Color.h"
#include "math/Rectangle.h"
#include "math/vec.h"

#include "GraphicsContext.h"
#include "FrameManager.h"
#include "Texture.h"

#include <array>

namespace efvk
{
	struct Sprite
	{
		ef::vec2 position;
		ef::vec2 size;

		ef::Rectanglef src_rect{ ef::vec2(0.0f, 0.0f), ef::vec2(1.0f, 1.0f) };
		Texture* texture = nullptr;
		float depth;
		ef::Color color{ 255, 255, 255, 255 };
	};

	class SpriteBatch
	{
	public:
		SpriteBatch(GraphicsContext& ctx);
		~SpriteBatch();

		SpriteBatch(const SpriteBatch&) = delete;
		SpriteBatch(SpriteBatch&&);
		SpriteBatch& operator=(const SpriteBatch&) = delete;
		SpriteBatch& operator=(SpriteBatch&&);

		void SetCamera(const ef::Camera2D& new_camera) { camera = &new_camera; }

		void Begin(const ef::Camera2D& camera)
		{
			this->camera = &camera;
			sprite_list.clear();
			texture_index_lookup.clear();
			texture_count = 0;

			for (u32 i = 0; i < max_texture_count; i++)
			{
				texture_array[i] = &placeholder_texture;
			}
		}
		void End(FrameManager& frame_manager);
		void Draw(const Sprite& sprite)
		{
			assert(camera != nullptr);

			SpriteInternal new_sprite{
				.src_rect = sprite.src_rect,
				.depth = sprite.depth,
				.color = sprite.color,
			};

			/* Do camera transform */
			new_sprite.position = camera->WorldToScreenPosition(sprite.position);
			new_sprite.size = camera->WorldToScreenSize(sprite.size);

			/* Do texture lookup */
			if (sprite.texture != nullptr)
			{
				if (texture_index_lookup.count(sprite.texture) == 0)
				{
					texture_index_lookup[sprite.texture] = texture_count;
					texture_array[texture_count] = sprite.texture;
					texture_count++;
					assert(texture_count <= max_texture_count); /* If we go over this limit then I have to extend this */
				}
				new_sprite.texture_id = texture_index_lookup[sprite.texture];
			}

			/* Cull anything outside the window rect */
			const ef::Rectanglef window_rect = ef::Rectanglef(ef::vec2(), camera->GetCameraSize());
			const ef::Rectanglef sprite_rect = ef::Rectanglef(new_sprite.position, new_sprite.size);

			if (window_rect.Intersects(sprite_rect))
			{
				sprite_list.push_back(new_sprite);
			}
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> pimpl{};

		const ef::Camera2D* camera{};

		struct SpriteInternal
		{
			ef::vec2 position{};
			ef::vec2 size{};

			ef::Rectanglef src_rect{ ef::vec2(0.0f, 0.0f), ef::vec2(1.0f, 1.0f) };
			u32 texture_id{0xFFFFFFFF};
			float depth{};
			ef::Color color{};
			u32 padding{};
		};

		static constexpr u32 max_texture_count = 16;
		std::vector<SpriteInternal> sprite_list{};
		std::unordered_map<Texture*, u32> texture_index_lookup{};
		std::array<Texture*, max_texture_count> texture_array{};
		u32 texture_count{};
		Texture placeholder_texture{};
	};
}