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

#include <jackalope/audio.h>
#include <jackalope/audio/ladspa.h>
#include <jackalope/audio/portaudio.h>
#include <jackalope/audio/sndfile.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/pcm.h>

namespace jackalope {

void audio_init()
{
    audio::ladspa_init();
    audio::portaudio_init();
    audio::sndfile_init();
}

audio_node_t::audio_node_t(const string_t& name_in, node_init_list_t init_list_in)
: node_t(name_in, init_list_in)
{
    add_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
    add_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
}

audio_domain_t& audio_node_t::get_domain()
{
    return *domain;
}

void audio_node_t::set_domain(audio_domain_t * domain_in)
{
    if (domain != nullptr) {
        throw_runtime_error("Audio node already has an audio domain");
    }

    domain = domain_in;

    get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).set(domain->get_sample_rate());
    get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).set(domain->get_buffer_size());
}

void audio_node_t::activate()
{
    auto& buffer_size = get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE);
    auto& sample_rate = get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE);

    if (! buffer_size.is_defined()) {
        throw_runtime_error("property must be defined to activate an audio node: ", JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE);
    } else if (buffer_size.get_size() == 0) {
        throw_runtime_error("can not activate an audio node with buffer size of 0");
    }

    if (! sample_rate.is_defined()) {
        throw_runtime_error("property must be defined to activate an audio node: ", JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE);
    } else if (buffer_size.get_size() == 0) {
        throw_runtime_error("can not activate an audio node with a sample rate of 0");
    }

    node_t::activate();
}

void audio_node_t::input_ready(input_t&)
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

void audio_node_t::pcm_ready()
{
    // log_info("All pcm inputs are ready: ", name);
}

audio_domain_t::audio_domain_t(const string_t& name_in, node_init_list_t init_list_in)
: node_t(name_in, init_list_in)
{
    add_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
    add_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
}

audio_domain_t::~audio_domain_t()
{
    for(auto i : audio_nodes) {
        delete i;
    }
}

audio_driver_t * audio_domain_t::_make_audio_driver(const string_t& class_name_in, const string_t& name_in, node_init_list_t init_list_in)
{
    string_t fully_qualified_name;

    if (name_in.find("::") != string_t::npos) {
        fully_qualified_name = class_name_in;
    } else {
        fully_qualified_name = to_string(JACKALOPE_AUDIO_DRIVER_CLASS_PREFIX, class_name_in);
    }

    auto new_node = jackalope::make_node<audio_driver_t>(name_in, fully_qualified_name, init_list_in);

    new_node->get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).set(get_buffer_size());
    new_node->get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).set(get_sample_rate());

    new_node->set_domain(this);

    return new_node;
}

audio_node_t * audio_domain_t::_make_audio_node(const string_t& class_name_in, const string_t& name_in, node_init_list_t init_list_in)
{
    string_t fully_qualified_name;

    if (name_in.find("::") != string_t::npos) {
        fully_qualified_name = class_name_in;
    } else {
        fully_qualified_name = to_string(JACKALOPE_AUDIO_NODE_CLASS_PREFIX, class_name_in);
    }

    auto new_node = jackalope::make_node<audio_node_t>(name_in, fully_qualified_name, init_list_in);

    new_node->set_domain(this);
    audio_nodes.push_back(new_node);

    return new_node;
}

size_t audio_domain_t::get_sample_rate()
{
    return get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).get_size();
}

size_t audio_domain_t::get_buffer_size()
{
    return get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).get_size();
}

real_t * audio_domain_t::get_zero_buffer_pointer()
{
    return zero_buffer.get_pointer();
}

void audio_domain_t::activate()
{
    auto buffer_size = get_buffer_size();

    zero_buffer.set_num_samples(buffer_size);

    for(auto i : outputs) {
        auto pcm_output = dynamic_cast<pcm_real_output_t *>(i);
        pcm_output->set_num_samples(buffer_size);
    }

    node_t::activate();
}

void audio_domain_t::reset()
{
    for(auto i : outputs) {
        i->reset();
    }

    for(auto i : audio_nodes) {
        // log_info("Resetting node: ", i->get_name());
        i->reset();
    }
}

void audio_node_t::notify()
{
    for(auto i : outputs) {
        i->notify();
    }
}

void audio_domain_t::input_ready(input_t&)
{
    for(auto i : inputs) {
        if (! i->is_ready()) {
            return;
        }
    }

    pcm_ready();
}

void audio_domain_t::pcm_ready()
{
    // log_info("All PCM inputs are ready for audio domain");
}

// FIXME how to properly const source_buffers_in ?
void audio_domain_t::process(real_t ** source_buffers_in, real_t ** sink_buffers_in)
{
    for(auto i : audio_nodes) {
        if (i->inputs.size() == 0) {
            i->pcm_ready();
        }
    }

    for(size_t i = 0; i < outputs.size(); i++) {
        auto pcm_output = dynamic_cast<pcm_real_output_t *>(outputs[i]);
        pcm_copy(source_buffers_in[i], pcm_output->get_buffer_pointer(), get_buffer_size());
        pcm_output->set_dirty();
    }

    notify();

    for(size_t i = 0; i < inputs.size(); i++) {
        auto pcm_input = dynamic_cast<pcm_real_input_t *>(inputs[i]);
        pcm_copy(sink_buffers_in[i], pcm_input->get_buffer_pointer(), get_buffer_size());
    }

    reset();
}

void audio_domain_t::process(const real_t * source_buffer_in, real_t * sink_buffer_in)
{
    if (source_buffer_in == nullptr && outputs.size() != 0) {
        throw_runtime_error("audio domain has outputs but the source buffer was null");
    }

    if (sink_buffer_in == nullptr && inputs.size() != 0) {
        throw_runtime_error("audio domain has inputs but the sink buffer was null");
    }

    assert(outputs.size() == 0);

    for(auto i : audio_nodes) {
        if (i->inputs.size() == 0) {
            i->pcm_ready();
        }
    }

    notify();

    for(size_t i = 0; i < inputs.size(); i++) {
        auto pcm_input = dynamic_cast<pcm_real_input_t *>(inputs[i]);
        auto buffer_ptr = pcm_input->get_buffer_pointer();
        pcm_interleave(buffer_ptr, sink_buffer_in, i, inputs.size(), get_buffer_size());
    }

    reset();
}

void audio_domain_t::notify()
{
    for(auto i : outputs) {
        auto pcm_output = dynamic_cast<pcm_real_output_t *>(i);
        pcm_output->notify();
    }
}

audio_driver_t::audio_driver_t(const string_t&, const string_t& name_in, node_init_list_t init_list_in)
: node_t(name_in, init_list_in)
{ }

void audio_driver_t::set_domain(audio_domain_t * domain_in)
{
    if (domain != nullptr) {
        throw_runtime_error("Audio driver already has an audio domain");
    }

    domain = domain_in;

    get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).set(domain->get_sample_rate());
    get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).set(domain->get_buffer_size());
}

} // namespace jackalope
