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
 * Contributor(s): Blender Foundation
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 */

/** \file blender/blenloader/intern/versioning_defaults.c
 *  \ingroup blenloader
 */

#include "MEM_guardedalloc.h"

#include "BLI_utildefines.h"
#include "BLI_listbase.h"
#include "BLI_math.h"
#include "BLI_string.h"

#include "DNA_camera_types.h"
#include "DNA_mesh_types.h"
#include "DNA_gpencil_types.h"
#include "DNA_mesh_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"
#include "DNA_screen_types.h"
#include "DNA_space_types.h"
#include "DNA_userdef_types.h"
#include "DNA_windowmanager_types.h"
#include "DNA_workspace_types.h"

#include "BKE_appdir.h"
#include "BKE_brush.h"
#include "BKE_colortools.h"
#include "BKE_idprop.h"
#include "BKE_keyconfig.h"
#include "BKE_layer.h"
#include "BKE_library.h"
#include "BKE_main.h"
#include "BKE_node.h"
#include "BKE_paint.h"
#include "BKE_screen.h"
#include "BKE_workspace.h"

#include "BLO_readfile.h"

/**
 * Override values in in-memory startup.blend, avoids resaving for small changes.
 */
void BLO_update_defaults_userpref_blend(void)
{
	/* default so DPI is detected automatically */
	U.dpi = 0;
	U.ui_scale = 1.0f;

#ifdef WITH_PYTHON_SECURITY
	/* use alternative setting for security nuts
	 * otherwise we'd need to patch the binary blob - startup.blend.c */
	U.flag |= USER_SCRIPT_AUTOEXEC_DISABLE;
#else
	U.flag &= ~USER_SCRIPT_AUTOEXEC_DISABLE;
#endif

	/* Clear addon preferences. */
	for (bAddon *addon = U.addons.first; addon; addon = addon->next) {
		if (addon->prop) {
			IDP_FreeProperty(addon->prop);
			MEM_freeN(addon->prop);
			addon->prop = NULL;
		}
	}

	/* Transform tweak with single click and drag. */
	U.flag |= USER_RELEASECONFIRM;

	/* Ignore the theme saved in the blend file,
	 * instead use the theme from 'userdef_default_theme.c' */
	{
		bTheme *theme = U.themes.first;
		memcpy(theme, &U_theme_default, sizeof(bTheme));
	}

	/* Leave temp directory empty, will then get appropriate value per OS. */
	U.tempdir[0] = '\0';

	/* Only enable tooltips translation by default, without actually enabling translation itself, for now. */
	U.transopts = USER_TR_TOOLTIPS;
	U.memcachelimit = 4096;

	/* Auto perspective. */
	U.uiflag |= USER_AUTOPERSP;

	/* Default to left click select. */
	BKE_keyconfig_pref_set_select_mouse(&U, 0, true);
}

/**
 * Update defaults in startup.blend, without having to save and embed the file.
 * This function can be emptied each time the startup.blend is updated. */
void BLO_update_defaults_startup_blend(Main *bmain, const char *app_template)
{
	/* For all startup.blend files. */
	for (bScreen *screen = bmain->screen.first; screen; screen = screen->id.next) {
		for (ScrArea *sa = screen->areabase.first; sa; sa = sa->next) {
			for (ARegion *ar = sa->regionbase.first; ar; ar = ar->next) {
				/* Remove all stored panels, we want to use defaults (order, open/closed) as defined by UI code here! */
				BKE_area_region_panels_free(&ar->panels);

				/* some toolbars have been saved as initialized,
				 * we don't want them to have odd zoom-level or scrolling set, see: T47047 */
				if (ELEM(ar->regiontype, RGN_TYPE_UI, RGN_TYPE_TOOLS, RGN_TYPE_TOOL_PROPS)) {
					ar->v2d.flag &= ~V2D_IS_INITIALISED;
				}
			}

			for (SpaceLink *sl = sa->spacedata.first; sl; sl = sl->next) {
				switch (sl->spacetype) {
					case SPACE_VIEW3D:
					{
						View3D *v3d = (View3D *)sl;
						v3d->overlay.texture_paint_mode_opacity = 1.0f;
						v3d->overlay.weight_paint_mode_opacity = 1.0f;
						v3d->overlay.vertex_paint_mode_opacity = 1.0f;
						/* grease pencil settings */
						v3d->vertex_opacity = 1.0f;
						v3d->gp_flag |= V3D_GP_SHOW_EDIT_LINES;
						break;
					}
					case SPACE_FILE:
					{
						SpaceFile *sfile = (SpaceFile *)sl;
						if (sfile->params) {
							const char *dir_default = BKE_appdir_folder_default();
							if (dir_default) {
								STRNCPY(sfile->params->dir, dir_default);
								sfile->params->file[0] = '\0';
							}
						}
						break;
					}
				}
			}
		}
	}

	if (app_template == NULL) {
		/* Name all screens by their workspaces (avoids 'Default.###' names). */
		{
			/* Default only has one window. */
			wmWindow *win = ((wmWindowManager *)bmain->wm.first)->windows.first;
			for (WorkSpace *workspace = bmain->workspaces.first; workspace; workspace = workspace->id.next) {
				WorkSpaceLayout *layout = BKE_workspace_hook_layout_for_workspace_get(win->workspace_hook, workspace);
				bScreen *screen = layout->screen;
				BLI_strncpy(screen->id.name + 2, workspace->id.name + 2, sizeof(screen->id.name) - 2);
			}
		}

		{
			/* 'UV Editing' should use UV mode. */
			bScreen *screen = BLI_findstring(&bmain->screen, "UV Editing", offsetof(ID, name) + 2);
			for (ScrArea *sa = screen->areabase.first; sa; sa = sa->next) {
				for (SpaceLink *sl = sa->spacedata.first; sl; sl = sl->next) {
					if (sl->spacetype == SPACE_IMAGE) {
						SpaceImage *sima = (SpaceImage *)sl;
						if (sima->mode == SI_MODE_VIEW) {
							sima->mode = SI_MODE_UV;
						}
					}
				}
			}
		}
	}

	/* For 2D animation template. */
	if (app_template && STREQ(app_template, "2D_Animation")) {
		for (WorkSpace *workspace = bmain->workspaces.first; workspace; workspace = workspace->id.next) {
			const char *name = workspace->id.name + 2;

			if (STREQ(name, "Drawing")) {
				workspace->object_mode = OB_MODE_PAINT_GPENCIL;
			}
		}
		/* set object in drawing mode */
		for (Object *object = bmain->object.first; object; object = object->id.next) {
			if (object->type == OB_GPENCIL) {
				bGPdata *gpd = (bGPdata *)object->data;
				object->mode = OB_MODE_PAINT_GPENCIL;
				gpd->flag |= GP_DATA_STROKE_PAINTMODE;
				break;
			}
		}

		/* Be sure curfalloff and primitive are initializated */
		for (Scene *scene = bmain->scene.first; scene; scene = scene->id.next) {
			ToolSettings *ts = scene->toolsettings;
			if (ts->gp_sculpt.cur_falloff == NULL) {
				ts->gp_sculpt.cur_falloff = curvemapping_add(1, 0.0f, 0.0f, 1.0f, 1.0f);
				CurveMapping *gp_falloff_curve = ts->gp_sculpt.cur_falloff;
				curvemapping_initialize(gp_falloff_curve);
				curvemap_reset(
				        gp_falloff_curve->cm,
				        &gp_falloff_curve->clipr,
				        CURVE_PRESET_GAUSS,
				        CURVEMAP_SLOPE_POSITIVE);
			}
			if (ts->gp_sculpt.cur_primitive == NULL) {
				ts->gp_sculpt.cur_primitive = curvemapping_add(1, 0.0f, 0.0f, 1.0f, 1.0f);
				CurveMapping *gp_primitive_curve = ts->gp_sculpt.cur_primitive;
				curvemapping_initialize(gp_primitive_curve);
				curvemap_reset(
				        gp_primitive_curve->cm,
				        &gp_primitive_curve->clipr,
				        CURVE_PRESET_BELL,
				        CURVEMAP_SLOPE_POSITIVE);
			}
		}
	}

	/* For all builtin templates shipped with Blender. */
	bool builtin_template = (
	        !app_template ||
	        STREQ(app_template, "2D_Animation") ||
	        STREQ(app_template, "Sculpting") ||
	        STREQ(app_template, "VFX") ||
	        STREQ(app_template, "Video_Editing"));

	if (builtin_template) {
		/* Clear all tools to use default options instead, ignore the tool saved in the file. */
		for (WorkSpace *workspace = bmain->workspaces.first; workspace; workspace = workspace->id.next) {
			while (!BLI_listbase_is_empty(&workspace->tools)) {
				BKE_workspace_tool_remove(workspace, workspace->tools.first);
			}
		}

		for (bScreen *screen = bmain->screen.first; screen; screen = screen->id.next) {
			/* Hide channels in timelines. */
			for (ScrArea *sa = screen->areabase.first; sa; sa = sa->next) {
				SpaceAction *saction = (sa->spacetype == SPACE_ACTION) ? sa->spacedata.first : NULL;

				if (saction && saction->mode == SACTCONT_TIMELINE) {
					for (ARegion *ar = sa->regionbase.first; ar; ar = ar->next) {
						if (ar->regiontype == RGN_TYPE_CHANNELS) {
							ar->flag |= RGN_FLAG_HIDDEN;
						}
					}
				}
			}
		}

		for (Scene *scene = bmain->scene.first; scene; scene = scene->id.next) {
			BLI_strncpy(scene->r.engine, RE_engine_id_BLENDER_EEVEE, sizeof(scene->r.engine));

			scene->r.cfra = 1.0f;
			scene->r.displaymode = R_OUTPUT_WINDOW;

			/* AV Sync break physics sim caching, disable until that is fixed. */
			if (!(app_template && STREQ(app_template, "Video_Editing"))) {
				scene->audio.flag &= ~AUDIO_SYNC;
				scene->flag &= ~SCE_FRAME_DROP;
			}

			/* Don't enable compositing nodes. */
			if (scene->nodetree) {
				ntreeFreeNestedTree(scene->nodetree);
				MEM_freeN(scene->nodetree);
				scene->nodetree = NULL;
				scene->use_nodes = false;
			}

			/* Rename render layers. */
			BKE_view_layer_rename(bmain, scene, scene->view_layers.first, "View Layer");
		}

		/* Rename lamp objects. */
		for (Object *ob = bmain->object.first; ob; ob = ob->id.next) {
			if (STREQ(ob->id.name, "OBLamp")) {
				STRNCPY(ob->id.name, "OBLight");
			}
		}
		for (Lamp *lamp = bmain->lamp.first; lamp; lamp = lamp->id.next) {
			if (STREQ(lamp->id.name, "LALamp")) {
				STRNCPY(lamp->id.name, "LALight");
			}
		}

		for (Mesh *mesh = bmain->mesh.first; mesh; mesh = mesh->id.next) {
			/* Match default for new meshes. */
			mesh->smoothresh = DEG2RADF(30);
		}

		/* Grease Pencil New Eraser Brush */
		Brush *br;
		/* Rename old Hard Eraser */
		br = (Brush *)BKE_libblock_find_name(bmain, ID_BR, "Eraser Hard");
		if (br) {
			strcpy(br->id.name, "BREraser Point");
		}
		for (Scene *scene = bmain->scene.first; scene; scene = scene->id.next) {
			ToolSettings *ts = scene->toolsettings;
			/* create new hard brush (only create one, but need ToolSettings) */
			br = (Brush *)BKE_libblock_find_name(bmain, ID_BR, "Eraser Hard");
			if (!br) {
				Paint *paint = &ts->gp_paint->paint;
				Brush *old_brush = paint->brush;

				br = BKE_brush_add_gpencil(bmain, ts, "Eraser Hard");
				br->size = 30.0f;
				br->gpencil_settings->draw_strength = 1.0f;
				br->gpencil_settings->flag = (GP_BRUSH_ENABLE_CURSOR | GP_BRUSH_DEFAULT_ERASER);
				br->gpencil_settings->icon_id = GP_BRUSH_ICON_ERASE_HARD;
				br->gpencil_tool = GPAINT_TOOL_ERASE;
				br->gpencil_settings->eraser_mode = GP_BRUSH_ERASER_SOFT;
				br->gpencil_settings->era_strength_f = 100.0f;
				br->gpencil_settings->era_thickness_f = 50.0f;

				/* back to default brush */
				BKE_paint_brush_set(paint, old_brush);
			}
		}
	}

	for (bScreen *sc = bmain->screen.first; sc; sc = sc->id.next) {
		for (ScrArea *sa = sc->areabase.first; sa; sa = sa->next) {
			for (SpaceLink *sl = sa->spacedata.first; sl; sl = sl->next) {
				if (sl->spacetype == SPACE_VIEW3D) {
					View3D *v3d = (View3D *)sl;
					v3d->shading.flag |= V3D_SHADING_SPECULAR_HIGHLIGHT;
				}
			}
		}
	}

	for (Scene *scene = bmain->scene.first; scene; scene = scene->id.next) {
		copy_v3_v3(scene->display.light_direction, (float[3]){M_SQRT1_3, M_SQRT1_3, M_SQRT1_3});
		copy_v2_fl2(scene->safe_areas.title, 0.1f, 0.05f);
		copy_v2_fl2(scene->safe_areas.action, 0.035f, 0.035f);
	}

	/*********************Game engine transition*********************/
	// WARNING: ALWAYS KEEP THIS IN BLO_update_defaults_startup_blend
	for (Scene *sce = bmain->scene.first; sce; sce = sce->id.next) {
		/* game data */
		sce->gm.stereoflag = STEREO_NOSTEREO;
		sce->gm.stereomode = STEREO_ANAGLYPH;
		sce->gm.eyeseparation = 0.10;

		sce->gm.xplay = 640;
		sce->gm.yplay = 480;
		sce->gm.freqplay = 60;
		sce->gm.depth = 32;

		sce->gm.gravity = 9.8f;
		sce->gm.physicsEngine = WOPHY_BULLET;
		//sce->gm.mode = WO_ACTIVITY_CULLING | WO_DBVT_CULLING;
		sce->gm.occlusionRes = 128;
		sce->gm.ticrate = 60;
		sce->gm.maxlogicstep = 5;
		sce->gm.physubstep = 1;
		sce->gm.maxphystep = 5;
		sce->gm.timeScale = 1.0f;
		sce->gm.lineardeactthreshold = 0.8f;
		sce->gm.angulardeactthreshold = 1.0f;
		sce->gm.deactivationtime = 0.0f;

		sce->gm.obstacleSimulation = OBSTSIMULATION_NONE;
		sce->gm.levelHeight = 2.f;

		sce->gm.recastData.cellsize = 0.3f;
		sce->gm.recastData.cellheight = 0.2f;
		sce->gm.recastData.agentmaxslope = M_PI_4;
		sce->gm.recastData.agentmaxclimb = 0.9f;
		sce->gm.recastData.agentheight = 2.0f;
		sce->gm.recastData.agentradius = 0.6f;
		sce->gm.recastData.edgemaxlen = 12.0f;
		sce->gm.recastData.edgemaxerror = 1.3f;
		sce->gm.recastData.regionminsize = 8.f;
		sce->gm.recastData.regionmergesize = 20.f;
		sce->gm.recastData.vertsperpoly = 6;
		sce->gm.recastData.detailsampledist = 6.0f;
		sce->gm.recastData.detailsamplemaxerror = 1.0f;

		sce->gm.lodflag = SCE_LOD_USE_HYST;
		sce->gm.scehysteresis = 10;

		sce->gm.exitkey = 218; // Blender key code for ESC

		//sce->gm.pythonkeys[0] = LEFTCTRLKEY;
		//sce->gm.pythonkeys[1] = LEFTSHIFTKEY;
		//sce->gm.pythonkeys[2] = LEFTALTKEY;
		//sce->gm.pythonkeys[3] = TKEY;
	}

	for (Object *ob = bmain->object.first; ob; ob = ob->id.next) {
		/* Game engine defaults*/
		ob->mass = ob->inertia = 1.0f;
		ob->formfactor = 0.4f;
		ob->damping = 0.04f;
		ob->rdamping = 0.1f;
		ob->anisotropicFriction[0] = 1.0f;
		ob->anisotropicFriction[1] = 1.0f;
		ob->anisotropicFriction[2] = 1.0f;
		ob->gameflag = OB_PROP | OB_COLLISION;
		ob->gameflag2 = 0;
		ob->margin = 0.04f;
		ob->friction = 0.5;
		ob->init_state = 1;
		ob->state = 1;
		ob->obstacleRad = 1.0f;
		ob->step_height = 0.15f;
		ob->jump_speed = 10.0f;
		ob->fall_speed = 55.0f;
		ob->max_jumps = 1;
		ob->max_slope = M_PI_2;
		ob->col_group = 0x01;
		ob->col_mask = 0xffff;
		ob->lodfactor = 1.0f;
		ob->preview = NULL;
		ob->duplicator_visibility_flag = OB_DUPLI_FLAG_VIEWPORT | OB_DUPLI_FLAG_RENDER;
	}

	for (Camera *ca = bmain->camera.first; ca; ca = ca->id.next) {
		ca->lodfactor = 1.0f;
		ca->gameflag |= GAME_CAM_OBJECT_ACTIVITY_CULLING;
		ca->gameviewport.rightratio = 1.0f;
		ca->gameviewport.topratio = 1.0f;
	}
	/***********************End of Game engine transition**********************/
}
