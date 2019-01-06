/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contributor(s): Tristan Porteries.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/Rasterizer/GPUFrameBuffer.cpp
 *  \ingroup bgerast
 */

//#include "GPUFrameBuffer.h"
//
//GPUFrameBuffer *GPUFrameBuffer::lastOffScreen = nullptr;
//
////GPUFrameBuffer::GPUFrameBuffer(unsigned int width, unsigned int height, int samples, GPUHDRType hdrType, GPUOffScreenMode mode, char errOut[256],
////                             RAS_Rasterizer::OffScreenType type)
////	:m_offScreen(GPU_offscreen_create(width, height, samples, hdrType, mode, errOut)),
////	m_type(type)
////{
////}
//
//GPUFrameBuffer::~GPUFrameBuffer()
//{
//	if (GetValid()) {
//		GPU_offscreen_free(m_offScreen);
//	}
//}
//
//bool GPUFrameBuffer::GetValid() const
//{
//	return (m_offScreen != nullptr);
//}
//
//void GPUFrameBuffer::Bind()
//{
//	/*GPU_offscreen_bind_simple(m_offScreen);
//	lastOffScreen = this;*/
//}
//
//GPUFrameBuffer *GPUFrameBuffer::Blit(GPUFrameBuffer *dstOffScreen, bool color, bool depth)
//{
//	//GPU_offscreen_blit(m_offScreen, dstOffScreen->m_offScreen, color, depth);
//
//	return nullptr; // dstOffScreen;
//}
//
//void GPUFrameBuffer::BindColorTexture(unsigned short slot)
//{
//	//GPU_texture_bind(GPU_offscreen_texture(m_offScreen), slot);
//}
//
//void GPUFrameBuffer::BindDepthTexture(unsigned short slot)
//{
//	//GPU_texture_bind(GPU_offscreen_depth_texture(m_offScreen), slot);
//}
//
//void GPUFrameBuffer::UnbindColorTexture()
//{
//	//GPU_texture_unbind(GPU_offscreen_texture(m_offScreen));
//}
//
//void GPUFrameBuffer::UnbindDepthTexture()
//{
//	//GPU_texture_unbind(GPU_offscreen_depth_texture(m_offScreen));
//}
//
//void GPUFrameBuffer::MipmapTexture()
//{
//	/*GPUTexture *tex = GPU_offscreen_texture(m_offScreen);
//	GPU_texture_filter_mode(tex, false, true, true);
//	GPU_texture_generate_mipmap(tex);*/
//}
//
//void GPUFrameBuffer::UnmipmapTexture()
//{
//	//GPU_texture_filter_mode(GPU_offscreen_texture(m_offScreen), false, true, false);
//}
//
//int GPUFrameBuffer::GetColorBindCode() const
//{
//	return -1;  GPU_offscreen_color_texture(m_offScreen);
//}
//
//int GPUFrameBuffer::GetSamples() const
//{
//	return -1; // GPU_offscreen_samples(m_offScreen);
//}
//
//unsigned int GPUFrameBuffer::GetWidth() const
//{
//	return GPU_offscreen_width(m_offScreen);
//}
//
//unsigned int GPUFrameBuffer::GetHeight() const
//{
//	return GPU_offscreen_height(m_offScreen);
//}
//
//RAS_Rasterizer::OffScreenType GPUFrameBuffer::GetType() const
//{
//	return m_type;
//}
//
//GPUTexture *GPUFrameBuffer::GetDepthTexture()
//{
//	return nullptr; // GPU_offscreen_depth_texture(m_offScreen);
//}
//
//GPUFrameBuffer *GPUFrameBuffer::GetLastOffScreen()
//{
//	return lastOffScreen;
//}
//
//void GPUFrameBuffer::RestoreScreen()
//{
//	GPU_framebuffer_restore();
//	lastOffScreen = nullptr;
//}
