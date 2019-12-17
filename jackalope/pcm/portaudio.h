// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#pragma once

extern "C" {
#include <portaudio.h>
}

#include <jackalope/driver.h>
#include <jackalope/pcm.h>

#define JACKALOPE_PORTAUDIO_DRIVER_CLASS "pcm::portaudio"

namespace jackalope {

namespace pcm {

using portaudio_stream_t = PaStream;
using portaudio_stream_cb_time_info_t = PaStreamCallbackTimeInfo;
using portaudio_stream_cb_flags = PaStreamCallbackFlags;

void portaudio_init();

struct portaudio_driver_t : public driver_t {
    const string_t class_name = JACKALOPE_PORTAUDIO_DRIVER_CLASS;
    portaudio_stream_t * stream = nullptr;

    portaudio_driver_t(const init_list_t& init_list_in);
    virtual ~portaudio_driver_t();
    virtual void init() override;
    virtual void activate() override;
    virtual void start() override;
    virtual int process(const void *input_buffer_in, void *output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *time_info_in, portaudio_stream_cb_flags status_flags_in);
};

} // namespace pcm

} // namespace jackalope
