/*
   Copyright (C) 2003 - 2014 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file
 * Definitions for a container for wml_menu_item.
 */

#include "../global.hpp"
#include "wmi_container.hpp"
#include "handlers.hpp"
#include "menu_item.hpp"

#include "../config.hpp"
#include "../gamestatus.hpp"
#include "../log.hpp"
#include "../map_location.hpp"
#include "../resources.hpp"

#include <boost/foreach.hpp>

static lg::log_domain log_engine("engine");
#define WRN_NG LOG_STREAM(warn, log_engine)
#define LOG_NG LOG_STREAM(info, log_engine)

static const size_t MAX_WML_COMMANDS = 7;


// This file is in the game_events namespace.
namespace game_events
{

wmi_container::wmi_container()
	: wml_menu_items_()
{}

/**
 * Destructor.
 * Default implementation, but defined here because this function needs to be
 * able to see wml_menu_item's destructor.
 */
wmi_container::~wmi_container()
{
}


/** Erases the item with id @a key. */
wmi_container::size_type wmi_container::erase(const std::string & id)
{
	// Locate the item to erase.
	const map_t::iterator iter = wml_menu_items_.find(id);

	if ( iter == wml_menu_items_.end() ) {
		WRN_NG << "Trying to remove non-existent menu item '" << id << "'; ignoring.\n";
		// No such item.
		return 0;
	}

	// Clean up our bookkeeping.
	iter->second->finish_handler();

	// Remove the item from the map.
	wml_menu_items_.erase(iter);

	return 1; // Erased one item.
}

/**
 * Fires the menu item with the given @a id.
 * @returns true if a matching item was found (even if it could not be fired).
 * NOTE: The return value could be altered if it is decided that
 * play_controller::execute_command() needs something different.
 */
bool wmi_container::fire_item(const std::string & id, const map_location & hex) const
{
	// Does this item exist?
	const_iterator iter = find(id);
	if ( iter == end() )
		return false;
	const wml_menu_item & wmi = **iter;

	// Prepare for can show().
	resources::gamedata->get_variable("x1") = hex.x + 1;
	resources::gamedata->get_variable("y1") = hex.y + 1;
	scoped_xy_unit highlighted_unit("unit", hex.x, hex.y, *resources::units);

	// Can this item be shown?
	if ( wmi.can_show(hex) )
		wmi.fire_event(hex);

	return true;
}

/**
 * Returns the menu items that can be shown for the given location.
 * The number of items returned is limited by MAX_WML_COMMANDS.
 * @param[out] items        Pointers to applicable menu items will be pushed onto @a items.
 * @param[out] descriptions Menu item text will be pushed onto @descriptions (in the same order as @a items).
 */
void wmi_container::get_items(const map_location& hex,
                              std::vector<boost::shared_ptr<const wml_menu_item> > & items,
                              std::vector<std::string> & descriptions) const
{
	size_t item_count = 0;

	if ( empty() )
		// Nothing to do (skip setting game variables).
		return;

	// Prepare for can show().
	resources::gamedata->get_variable("x1") = hex.x + 1;
	resources::gamedata->get_variable("y1") = hex.y + 1;
	scoped_xy_unit highlighted_unit("unit", hex.x, hex.y, *resources::units);

	// Check each menu item.
	BOOST_FOREACH( const item_ptr & item, *this )
	{
		// Can this item be shown?
		if ( item->use_wml_menu() && item->can_show(hex) )
		{
			// Include this item.
			items.push_back(item);
			descriptions.push_back(item->menu_text());

			// Limit how many items can be returned.
			if ( ++item_count >= MAX_WML_COMMANDS )
				return;
		}
	}
}

/**
 * Initializes the implicit event handlers for inlined [command]s.
 */
void wmi_container::init_handlers() const
{
	// Applying default hotkeys here currently does not work because
	// the hotkeys are reset by play_controler::init_managers() ->
	// display_manager::display_manager, which is called after this.
	// The result is that default wml hotkeys will be ignored if wml
	// hotkeys are set to default in the preferences menu. (They are
	// still reapplied if set_menu_item is called again, for example
	// by starting a new campaign.) Since it isn't that important
	// I'll just leave it for now.

	unsigned wmi_count = 0;

	// Loop through each menu item.
	BOOST_FOREACH( const item_ptr & wmi, *this ) {
		// If this menu item has a [command], add a handler for it.
		wmi->init_handler();
		// Count the menu items (for the diagnostic message).
		++wmi_count;
	}

	// Diagnostic:
	if ( wmi_count > 0 ) {
		LOG_NG << wmi_count << " WML menu items found, loaded." << std::endl;
	}
}

void wmi_container::to_config(config& cfg) const
{
	// Loop through our items.
	BOOST_FOREACH( const item_ptr & item, *this )
		// Add this item as a child of cfg.
		item->to_config(cfg.add_child("menu_item"));
}

/**
 * Updates or creates (as appropriate) the menu item with the given @a id.
 */
void wmi_container::set_item(const std::string& id, const vconfig& menu_item)
{
	// Try to insert a dummy value. This combines looking for an existing
	// entry with insertion.
	map_t::iterator add_it = wml_menu_items_.insert(map_t::value_type(id, item_ptr())).first;

	if ( add_it->second )
		// Create a new menu item based on the old. This leaves the old item
		// alone in case someone else is holding on to (and processing) it.
		add_it->second.reset(new wml_menu_item(id, menu_item, *add_it->second));
	else
		// This is a new menu item.
		add_it->second.reset(new wml_menu_item(id, menu_item));
}

/**
 * Sets the current menu items to the "menu_item" children of @a cfg.
 */
void wmi_container::set_menu_items(const config& cfg)
{
	wml_menu_items_.clear();
	BOOST_FOREACH(const config &item, cfg.child_range("menu_item"))
	{
		if(!item.has_attribute("id")){ continue; }

		std::string id = item["id"];
		item_ptr & mref = wml_menu_items_[id];
		if ( !mref ) {
			mref.reset(new wml_menu_item(id, item));
		} else {
			WRN_NG << "duplicate menu item (" << id << ") while loading from config\n";
		}
	}
}

} // end namespace game_events

