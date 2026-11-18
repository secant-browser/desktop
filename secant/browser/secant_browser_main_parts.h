#ifndef SECANT_BROWSER_SECANT_BROWSER_MAIN_PARTS_H_
#define SECANT_BROWSER_SECANT_BROWSER_MAIN_PARTS_H_

#include "chrome/browser/chrome_browser_main_extra_parts.h"

namespace secant {

// Secant's browser-process bootstrap point. It is registered from
// ChromeBrowserMainParts::Create so the product layer participates in the
// browser process lifecycle.
class SecantBrowserMainParts : public ChromeBrowserMainExtraParts {
 public:
  SecantBrowserMainParts();
  ~SecantBrowserMainParts() override;

  // ChromeBrowserMainExtraParts:
  void PreMainMessageLoopRun() override;
};

}  // namespace secant

#endif  // SECANT_BROWSER_SECANT_BROWSER_MAIN_PARTS_H_
