#include "secant/browser/secant_browser_main_parts.h"

#include "base/logging.h"

namespace secant {

SecantBrowserMainParts::SecantBrowserMainParts() = default;
SecantBrowserMainParts::~SecantBrowserMainParts() = default;

void SecantBrowserMainParts::PreMainMessageLoopRun() {
  VLOG(1) << "SecantBrowserMainParts::PreMainMessageLoopRun";
}

}  // namespace secant
