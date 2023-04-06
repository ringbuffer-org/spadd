#pragma once

#if 0

BEGIN_JUCE_MODULE_DECLARATION

      ID:               shared_processing_code
      vendor:           TU Berlin
      version:          0.0.1
      name:             shared_processing_code
      description:      Shared processing functions
      license:          GPL/Commercial
      dependencies:     juce_audio_utils

     END_JUCE_MODULE_DECLARATION

#endif

#include <juce_audio_utils/juce_audio_utils.h>

#include "Source/SpatialPartial.hpp"
#include "Source/Window.hpp"
#include "Source/Wavetable.hpp"