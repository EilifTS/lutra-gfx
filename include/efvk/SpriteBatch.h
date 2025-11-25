#pragma once
#include "math/Camera2D.h"
#include "math/Color.h"
#include "math/Rectangle.h"
#include "math/vec.h"

#include "GraphicsContext.h"
#include "FrameManager.h"

namespace efvk
{
	class Sprite
	{
	public:
		ef::vec2 position;
		ef::vec2 size;

		// ef::Rectanglef src_rect{ ef::vec2(0.0f, 0.0f), ef::vec2(1.0f, 1.0f) };
		// Texture2D* texture = nullptr;
		float depth;
		ef::Color color;
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
		}
		void End(FrameManager& frame_manager);
		void Draw(const Sprite& sprite)
		{
			assert(camera != nullptr);

			Sprite new_sprite = sprite;

			/* Do camera transform */
			new_sprite.position = camera->WorldToScreenPosition(new_sprite.position);
			new_sprite.size = camera->WorldToScreenSize(new_sprite.size);

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

		std::vector<Sprite> sprite_list{};
	};
}