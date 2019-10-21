// Jackalope Audio Engine
// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/jackalope.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>
#include <jackalope/pcm.h>
#include <jackalope/pcm/ladspa.h>
#include <jackalope/pcm/portaudio.h>
#include <jackalope/pcm/sndfile.h>

namespace jackalope {

static shared_t<input_t> pcm_real_input_constructor(const string_t& name_in, shared_t<node_t> parent_in)
{
    return jackalope::make_shared<pcm_real_input_t>(name_in, parent_in);
}

static shared_t<output_t> pcm_real_output_constructor(const string_t& name_in, shared_t<node_t> parent_in)
{
    return jackalope::make_shared<pcm_real_output_t>(name_in, parent_in);
}

static shared_t<input_t> pcm_quad_input_constructor(const string_t& name_in, shared_t<node_t> parent_in)
{
    return jackalope::make_shared<pcm_quad_input_t>(name_in, parent_in);
}

static shared_t<output_t> pcm_quad_output_constructor(const string_t& name_in, shared_t<node_t> parent_in)
{
    return jackalope::make_shared<pcm_quad_output_t>(name_in, parent_in);
}

void pcm_init()
{
    add_input_constructor(JACKALOPE_PCM_CHANNEL_CLASS_REAL, pcm_real_input_constructor);
    add_output_constructor(JACKALOPE_PCM_CHANNEL_CLASS_REAL, pcm_real_output_constructor);

    add_input_constructor(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, pcm_quad_input_constructor);
    add_output_constructor(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, pcm_quad_output_constructor);

    pcm::ladspa_init();
    pcm::portaudio_init();
    pcm::sndfile_init();
}

pcm_real_input_t::pcm_real_input_t(const string_t& name_in, shared_t<node_t> parent_in)
: pcm_input_t(JACKALOPE_PCM_CHANNEL_CLASS_REAL, name_in, parent_in)
{ }

real_t * pcm_real_input_t::get_buffer_pointer()
{
    auto num_links = links.size();

    if (num_links == 0) {
        auto pcm_node = dynamic_pointer_cast<pcm_node_t>(parent);
        return pcm_node->get_domain().get_zero_buffer_pointer();
    } else if (num_links == 1) {
        auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(links.front()->get_from());
        return pcm_output->get_buffer_pointer();
    }

    throw_runtime_error("Can not mix together multiple outputs to an input");
}

pcm_quad_input_t::pcm_quad_input_t(const string_t& name_in, shared_t<node_t> parent_in)
: pcm_input_t(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, name_in, parent_in)
{ }

complex_t * pcm_quad_input_t::get_buffer_pointer()
{
    auto num_links = links.size();

    if (num_links == 0) {
        throw_runtime_error("Can not create a buffer pointer with zero links to input yet");
    } else if (num_links == 1) {
        auto pcm_output = dynamic_pointer_cast<pcm_quad_output_t>(links.front()->get_from());
        return pcm_output->get_buffer_pointer();
    }

    throw_runtime_error("Can not mix together multiple outputs to an input");
}

pcm_real_output_t::pcm_real_output_t(const string_t& name_in, shared_t<node_t> parent_in)
: pcm_output_t(JACKALOPE_PCM_CHANNEL_CLASS_REAL, name_in, parent_in)
{ }

pcm_quad_output_t::pcm_quad_output_t(const string_t& name_in, shared_t<node_t> parent_in)
: pcm_output_t(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, name_in, parent_in)
{ }

pcm_node_t::pcm_node_t(const string_t& name_in, node_init_list_t init_list_in)
: node_t(name_in, init_list_in)
{
    add_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
    add_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
}

pcm_domain_t& pcm_node_t::get_domain()
{
    return *domain;
}

void pcm_node_t::set_domain(shared_t<pcm_domain_t> domain_in)
{
    if (domain != nullptr) {
        throw_runtime_error("PCM node already has a PCM domain");
    }

    domain = domain_in;

    get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE).set(domain->get_sample_rate());
    get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE).set(domain->get_buffer_size());
}

void pcm_node_t::activate()
{
    auto& buffer_size = get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);
    auto& sample_rate = get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE);

    if (! buffer_size.is_defined()) {
        throw_runtime_error("property must be defined to activate a PCM node: ", JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);
    } else if (buffer_size.get_size() == 0) {
        throw_runtime_error("can not activate a PCM node with buffer size of 0");
    }

    if (! sample_rate.is_defined()) {
        throw_runtime_error("property must be defined to activate a PCM node: ", JACKALOPE_PCM_PROPERTY_SAMPLE_RATE);
    } else if (buffer_size.get_size() == 0) {
        throw_runtime_error("can not activate an PCM node with a sample rate of 0");
    }

    node_t::activate();
}

void pcm_node_t::input_ready(shared_t<input_t>)
{
    for (auto input : inputs) {
        auto input_class = extract_channel_class(input->get_class_name());

        if (input_class != JACKALOPE_PCM_CHANNEL_CLASS) {
            continue;
        } else if (input->links.size() > 0 && ! input->is_ready()) {
            return;
        }
    }

    pcm_ready();
}

void pcm_node_t::pcm_ready()
{
    // log_info("All pcm inputs are ready: ", name);
}

pcm_domain_t::pcm_domain_t(const string_t& name_in, node_init_list_t init_list_in)
: node_t(name_in, init_list_in)
{
    add_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
    add_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, property_t::type_t::size);

    add_slot("system:terminate", [](signal_t *) { jackalope_panic("can't cleanly terminate yet"); });
}

pcm_domain_t::~pcm_domain_t()
{ }

size_t pcm_domain_t::get_sample_rate()
{
    return get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE).get_size();
}

size_t pcm_domain_t::get_buffer_size()
{
    return get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE).get_size();
}

real_t * pcm_domain_t::get_zero_buffer_pointer()
{
    return zero_buffer.get_pointer();
}

void pcm_domain_t::activate()
{
    auto buffer_size = get_buffer_size();

    zero_buffer.set_num_samples(buffer_size);

    for(auto i : outputs) {
        auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(i);
        pcm_output->set_num_samples(buffer_size);
    }

    node_t::activate();
}

void pcm_domain_t::start()
{
    for(auto i : pcm_nodes) {
        i.lock()->start();
    }

    node_t::start();
}

void pcm_domain_t::reset()
{
    for(auto i : outputs) {
        i->reset();
    }

    for(auto i : pcm_nodes) {
        // log_info("Resetting node: ", i->get_name());
        i.lock()->reset();
    }
}

void pcm_node_t::notify()
{
    for(auto i : outputs) {
        i->notify();
    }
}

void pcm_domain_t::input_ready(shared_t<input_t>)
{
    for(auto i : inputs) {
        if (! i->is_ready()) {
            return;
        }
    }

    pcm_ready();
}

void pcm_domain_t::pcm_ready()
{
    // log_info("All PCM inputs are ready for PCM domain");
}

// FIXME how to properly const source_buffers_in ?
void pcm_domain_t::process(real_t ** source_buffers_in, real_t ** sink_buffers_in)
{
    for(auto i : pcm_nodes) {
        auto node = i.lock();
        if (node->inputs.size() == 0) {
            node->pcm_ready();
        }
    }

    for(size_t i = 0; i < outputs.size(); i++) {
        auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(outputs[i]);
        pcm_copy(source_buffers_in[i], pcm_output->get_buffer_pointer(), get_buffer_size());
        pcm_output->set_dirty();
    }

    notify();

    for(size_t i = 0; i < inputs.size(); i++) {
        auto pcm_input = dynamic_pointer_cast<pcm_real_input_t>(inputs[i]);
        pcm_copy(sink_buffers_in[i], pcm_input->get_buffer_pointer(), get_buffer_size());
    }

    reset();
}

void pcm_domain_t::process(const real_t * source_buffer_in, real_t * sink_buffer_in)
{
    if (source_buffer_in == nullptr && outputs.size() != 0) {
        throw_runtime_error("PCM domain has outputs but the source buffer was null");
    }

    if (sink_buffer_in == nullptr && inputs.size() != 0) {
        throw_runtime_error("PCM domain has inputs but the sink buffer was null");
    }

    assert(outputs.size() == 0);

    for(auto i : pcm_nodes) {
        auto node = i.lock();

        if (node->inputs.size() == 0) {
            node->pcm_ready();
        }
    }

    notify();

    for(size_t i = 0; i < inputs.size(); i++) {
        auto pcm_input = dynamic_pointer_cast<pcm_real_input_t>(inputs[i]);
        auto buffer_ptr = pcm_input->get_buffer_pointer();
        pcm_interleave(buffer_ptr, sink_buffer_in, i, inputs.size(), get_buffer_size());
    }

    reset();
}

void pcm_domain_t::notify()
{
    for(auto i : outputs) {
        auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(i);
        pcm_output->notify();
    }
}

pcm_driver_t::pcm_driver_t(const string_t& name_in, node_init_list_t init_list_in)
: node_t(name_in, init_list_in)
{ }

void pcm_driver_t::set_domain(shared_t<pcm_domain_t> domain_in)
{
    domain = domain_in;

    get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE).set(domain->get_sample_rate());
    get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE).set(domain->get_buffer_size());
}

void pcm_driver_t::input_ready(shared_t<input_t>)
{ }

void pcm_driver_t::notify()
{ }

} // namespace jackalope
