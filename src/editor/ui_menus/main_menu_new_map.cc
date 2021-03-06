/*
 * Copyright (C) 2002-2020 by the Widelands Development Team
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "editor/ui_menus/main_menu_new_map.h"

#include "base/i18n.h"
#include "base/macros.h"
#include "editor/editorinteractive.h"
#include "graphic/font_handler.h"
#include "graphic/image.h"
#include "graphic/texture.h"
#include "logic/editor_game_base.h"
#include "logic/map.h"
#include "logic/map_objects/world/terrain_description.h"
#include "logic/map_objects/world/world.h"
#include "ui_basic/progresswindow.h"
#include "ui_basic/textarea.h"
#include "wlapplication_options.h"

inline EditorInteractive& MainMenuNewMap::eia() {
	return dynamic_cast<EditorInteractive&>(*get_parent());
}

MainMenuNewMap::MainMenuNewMap(EditorInteractive& parent, Registry& registry)
   : UI::UniqueWindow(&parent, "new_map_menu", &registry, 360, 150, _("New Map")),
     margin_(4),
     box_width_(get_inner_w() - 2 * margin_),
     box_(this, margin_, margin_, UI::Box::Vertical, 0, 0, margin_),
     map_size_box_(box_,
                   "new_map_menu",
                   4,
                   parent.egbase().map().get_width(),
                   parent.egbase().map().get_height()),
     list_(&box_, 0, 0, box_width_, 330, UI::PanelStyle::kWui),
     // Buttons
     button_box_(&box_, 0, 0, UI::Box::Horizontal, 0, 0, margin_),
     ok_button_(&button_box_,
                "create_map",
                0,
                0,
                box_width_ / 2 - margin_,
                0,
                UI::ButtonStyle::kWuiPrimary,
                _("Create Map")),
     cancel_button_(&button_box_,
                    "generate_map",
                    0,
                    0,
                    box_width_ / 2 - margin_,
                    0,
                    UI::ButtonStyle::kWuiSecondary,
                    _("Cancel")) {

	box_.set_size(100, 20);  // Prevent assert failures
	box_.add(&map_size_box_, UI::Box::Resizing::kExpandBoth);
	box_.add_space(margin_);
	UI::Textarea* terrain_label = new UI::Textarea(&box_, _("Terrain:"));
	box_.add(terrain_label);
	box_.add(&list_);
	box_.add_space(2 * margin_);

	cancel_button_.sigclicked.connect(boost::bind(&MainMenuNewMap::clicked_cancel, this));
	ok_button_.sigclicked.connect(boost::bind(&MainMenuNewMap::clicked_create_map, this));
	if (UI::g_fh->fontset()->is_rtl()) {
		button_box_.add(&ok_button_);
		button_box_.add(&cancel_button_);
	} else {
		button_box_.add(&cancel_button_);
		button_box_.add(&ok_button_);
	}
	box_.add(&button_box_);

	set_center_panel(&box_);
	fill_list();
	center_to_parent();
}

void MainMenuNewMap::clicked_create_map() {
	EditorInteractive& parent = eia();
	Widelands::EditorGameBase& egbase = parent.egbase();
	Widelands::Map* map = egbase.mutable_map();
	egbase.create_loader_ui({"editor"}, true, "images/loadscreens/editor.jpg");
	egbase.step_loader_ui(_("Creating empty map…"));

	parent.cleanup_for_load();

	map->create_empty_map(egbase, map_size_box_.selected_width(), map_size_box_.selected_height(),
	                      list_.get_selected(), _("No Name"),
	                      get_config_string("realname", pgettext("author_name", "Unknown")));

	egbase.create_tempfile_and_save_mapdata(FileSystem::ZIP);
	egbase.load_graphics();

	map->recalc_whole_map(egbase);
	parent.map_changed(EditorInteractive::MapWas::kReplaced);
	egbase.remove_loader_ui();
	die();
}

void MainMenuNewMap::clicked_cancel() {
	die();
}

/*
 * fill the terrain list
 */
void MainMenuNewMap::fill_list() {
	list_.clear();
	const Widelands::DescriptionMaintainer<Widelands::TerrainDescription>& terrains =
	   eia().egbase().world().terrains();

	for (Widelands::DescriptionIndex index = 0; index < terrains.size(); ++index) {
		const Widelands::TerrainDescription& terrain = terrains.get(index);
		upcast(Image const, image, &terrain.get_texture(0));
		list_.add(terrain.descname(), index, image);
	}
	list_.select(0);
}
