// =============================================================================
// Missile Madness
//
// (c) 2003 DigiPen Institute of Technology
// =============================================================================

#pragma once

enum struct ErrorCode {
  Unknown,
  DisplayResolution,
  DisplayDepth,
  FailedToJoin,
};

void errorDisplayAndExit(ErrorCode code);
