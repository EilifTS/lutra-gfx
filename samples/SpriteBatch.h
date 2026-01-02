#pragma once
#include <efvk/math/Color.h>
#include <efvk/math/Rectangle.h>
#include <efvk/math/vec.h>

#include <efvk/GraphicsContext.h>
#include <efvk/GraphicsPipeline.h>
#include <efvk/FrameManager.h>
#include <efvk/Texture.h>
#include <efvk/Buffer.h>
#include <efvk/DepthStencilBuffer.h>

#include <array>
#include <unordered_map>

namespace efvk
{
	struct Sprite
	{
		float x{};
		float y{};
		float w{};
		float h{};

		float src_x{};
		float src_y{};
		float src_w{ 1.0f };
		float src_h{ 1.0f };

		float depth{};
		u32 color{};
		Texture* texture = nullptr;
	};

	class SpriteBatch
	{
	public:
		SpriteBatch(GraphicsContext& ctx);

		SpriteBatch(const SpriteBatch&) = delete;
		SpriteBatch(SpriteBatch&&) = default;
		SpriteBatch& operator=(const SpriteBatch&) = delete;
		SpriteBatch& operator=(SpriteBatch&&) = default;

		void Begin()
		{
			sprite_list.clear();
			texture_index_lookup.clear();
			texture_count = 0;

			for (u32 i = 0; i < max_texture_count; i++)
			{
				texture_view_array[i] = placeholder_texture.DefaultView();
			}
		}
		void End(FrameManager& frame_manager);
		void Draw(const Sprite& sprite)
		{
			SpriteInternal new_sprite{
				.x = sprite.x,
				.y = sprite.y,
				.w = sprite.w,
				.h = sprite.h,
				.src_x = sprite.src_x,
				.src_y = sprite.src_y,
				.src_w = sprite.src_w,
				.src_h = sprite.src_h,
				.depth = sprite.depth,
				.color = sprite.color,
			};

			/* Do texture lookup */
			if (sprite.texture != nullptr)
			{
				if (texture_index_lookup.count(sprite.texture) == 0)
				{
					texture_index_lookup[sprite.texture] = texture_count;
					texture_view_array[texture_count] = sprite.texture->DefaultView();
					texture_count++;
					assert(texture_count <= max_texture_count); /* If we go over this limit then I have to extend this */
				}
				new_sprite.texture_id = texture_index_lookup[sprite.texture];
			}
			sprite_list.push_back(new_sprite);
		}

	private:
		GraphicsContext* ctx{};
		GraphicsPipeline pipeline{};
		DepthStencilBuffer ds_buffer{};

		struct SpriteInternal
		{
			float x{};
			float y{};
			float w{};
			float h{};

			float src_x{};
			float src_y{};
			float src_w{};
			float src_h{};

			float depth{};
			u32 color{};
			u32 texture_id{ 0xFFFFFFFF };
			u32 padding{};
		};

		std::vector<SpriteInternal> sprite_list{};
		Buffer sprite_buffer{};

		static constexpr u32 max_texture_count = 16;
		std::unordered_map<Texture*, u32> texture_index_lookup{};
		std::array<TextureView, max_texture_count> texture_view_array{};
		u32 texture_count{};
		Texture placeholder_texture{};
	};
}