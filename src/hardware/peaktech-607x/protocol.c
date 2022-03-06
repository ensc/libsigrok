// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2022 Enrico Scholz <enrico.scholz@ensc.de>
 */

#include <config.h>

#include "protocol.h"

#include <libsigrok/libsigrok.h>
#include <libsigrok-internal.h>

static uint16_t get_crc(void const *data, size_t cnt)
{
	return sr_crc16(0xffff, data, cnt);
}

SR_PRIV void peaktech_607x_proto_crc_set(void *data, size_t cnt)
{
	struct pt_proto_hdr  const	*hdr = data;
	struct pt_proto_tail		*tail;
	size_t				payload_sz;

	g_assert(cnt >= sizeof *hdr + sizeof *tail);

	payload_sz = cnt - sizeof *tail;
	tail       = data + payload_sz;

	tail->crc = cpu_to_le16(get_crc(data, payload_sz));
}

SR_PRIV bool peaktech_607x_proto_crc_check(void const *data, size_t cnt)
{
	struct pt_proto_hdr  const	*hdr = data;
	struct pt_proto_tail const	*tail;
	size_t				payload_sz;

	g_assert(cnt >= sizeof *hdr + sizeof *tail);

	payload_sz = cnt - sizeof *tail;
	tail       = data + payload_sz;

	return (hdr->magic == 0xf7 &&
		tail->magic == 0xfd &&
		le16_to_cpu(tail->crc) == get_crc(data, payload_sz));
}
