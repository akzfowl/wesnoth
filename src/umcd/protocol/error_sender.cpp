/*
	Copyright (C) 2013 by Pierre Talbot <ptalbot@mopong.net>
	Part of the Battle for Wesnoth Project http://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

#include "umcd/protocol/error_sender.hpp"
#include "umcd/protocol/header_data.hpp"
#include "umcd/special_packet.hpp"
#include "config.hpp"

namespace umcd{

void async_send_error(const boost::shared_ptr<boost::asio::ip::tcp::socket> &socket, const boost::system::error_condition& error)
{
	boost::shared_ptr<header_const_buffer::sender_type> sender = make_header_sender(socket, make_error_packet(error.message()));
	sender->async_send();
}
} // namespace umcd
