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

#pragma once

#include <jackalope/types.h>

template <typename T>
void pcm_copy(const T * source_in, T * dest_in, const size_t num_samples_in)
{
    for(size_t i = 0; i < num_samples_in; i++) {
        dest_in[i] = source_in[i];
    }
}

template <typename T>
void pcm_zero(T * pcm_in, const size_t num_samples_in)
{
    for(size_t i = 0; i < num_samples_in; i++) {
        pcm_in[i] = 0;
    }
}

template <typename T>
void pcm_extract_interleaved_channel(const T * source_in, T * dest_in, const size_t extract_channel_in, const size_t num_channels_in, const size_t num_samples_in)
{
    for(size_t i = 0; i < num_samples_in; i++) {
        auto sample_num = num_channels_in * i + extract_channel_in;
        dest_in[i] = source_in[sample_num];
    }
}

template <typename T>
void pcm_interleave(const T * source_in, T * dest_in, const size_t interleave_num_in, const size_t num_channels_in, const size_t num_samples_in)
{
    for(size_t i = 0; i < num_samples_in; i++) {
        auto sample_num = num_channels_in * i + interleave_num_in;
        dest_in[sample_num] = source_in[i];
    }
}
