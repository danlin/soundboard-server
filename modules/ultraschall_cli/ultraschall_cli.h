/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 ultraschall_cli
  vendor:             ultraschall
  version:            5.0.0
  name:               Ultraschall Commandline Utilities
  description:        Classes to provide Commandline Utilities
  website:            https://ultraschall.fm
  license:            MIT

  dependencies:       juce_core, juce_events

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#define ULTRASCHALL_CLI_H_INCLUDED

#include "cli/AsynchronousCommandlineReader.h"
#include "cli/LoggingUtilities.h"
