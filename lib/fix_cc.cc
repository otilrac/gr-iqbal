/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-iqbalance
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/iqbalance/fix_cc.h>

namespace gr {

iqbalance::fix_cc::sptr
iqbalance::fix_cc::make(float mag, float phase)
{
	return gnuradio::get_initial_sptr(new iqbalance::fix_cc(mag, phase));
}


iqbalance::fix_cc::fix_cc(float mag, float phase)
  : gr::sync_block ("fix_cc",
		   gr::io_signature::make(1, 1, sizeof (gr_complex)),
		   gr::io_signature::make(1, 1, sizeof (gr_complex))),
    d_mag(mag),
    d_phase(phase)
{
	message_port_register_in(pmt::mp("iqbal_corr"));
	set_msg_handler(pmt::mp("iqbal_corr"),
			boost::bind(&iqbalance::fix_cc::apply_new_corrections, this, boost::placeholders::_1));
}


iqbalance::fix_cc::~fix_cc()
{
	/* Nothing to do */
}


void
iqbalance::fix_cc::apply_new_corrections(pmt::pmt_t msg)
{
	if (!pmt::is_f32vector(msg))
		return;

	this->set_mag(pmt::f32vector_ref(msg, 0));
	this->set_phase(pmt::f32vector_ref(msg, 1));
}


int
iqbalance::fix_cc::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	int i;

	if (this->d_mag == 0.0f && this->d_phase == 0.0f) {
		memcpy(out, in, noutput_items * sizeof(gr_complex));
		return noutput_items;
	}

	const float magp1 = 1.0f + this->d_mag;
	const float phase = this->d_phase;

	for (i=0; i<noutput_items; i++) {
		gr_complex v = in[i];
		out[i] = gr_complex(
			v.real() * magp1,
			v.imag() + phase * v.real()
		);
	}

	return noutput_items;
}

} /* namespace gr */

