#include "secant/browser/tab_state.h"

#include <iostream>
#include <string>

namespace {

bool Check(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
  }
  return condition;
}

}  // namespace

int main() {
  secant::SecantTabState model;
  bool ok = true;

  const secant::TabId first =
      model.CreateTab(1, "https://first.example", "First");
  const secant::TabId second =
      model.CreateTab(1, "https://second.example", "Second");

  ok &= Check(model.tabs().size() == 2, "two tabs exist");
  ok &= Check(model.active_tab_id() == first, "first tab is active");
  ok &= Check(model.GetTab(first) != nullptr, "first tab is findable");
  ok &= Check(model.GetTab(first)->title == "First", "title is stored");

  ok &= Check(model.ActivateTab(second), "second tab activates");
  ok &= Check(model.active_tab_id() == second, "second tab is active");
  ok &= Check(model.GetTab(first)->active == false, "first tab is inactive");

  ok &= Check(model.MoveTab(first, 1), "first tab moves to the end");
  ok &= Check(model.tabs()[0].id == second, "second tab is first");
  ok &= Check(model.tabs()[1].id == first, "first tab is second");
  ok &= Check(model.active_tab_id() == second, "move preserves active tab");

  ok &= Check(model.SetKind(first, secant::TabKind::kPinned),
              "first tab is pinned");
  ok &= Check(model.GetTab(first)->kind == secant::TabKind::kPinned,
              "pinned kind is stored");

  ok &= Check(model.SetFolder(first, 7), "first tab enters a folder");
  ok &= Check(model.GetTab(first)->folder_id == 7, "folder id is stored");
  ok &= Check(model.SetFolder(first, std::nullopt), "first tab leaves folder");
  ok &= Check(!model.GetTab(first)->folder_id.has_value(),
              "folder id is cleared");

  ok &= Check(model.TabsForWorkspace(1).size() == 2,
              "workspace one has two tabs");
  ok &= Check(model.TabsForWorkspace(2).empty(),
              "workspace two is empty");

  const secant::TabId third =
      model.CreateTab(2, "https://third.example", "");
  ok &= Check(model.active_tab_id() == second,
              "creating a tab does not steal focus");
  ok &= Check(model.GetTab(third)->title == "https://third.example",
              "empty title falls back to url");
  ok &= Check(model.TabsForWorkspace(2).size() == 1,
              "workspace two has one tab");

  ok &= Check(model.CloseTab(second), "active tab closes");
  ok &= Check(model.tabs().size() == 2, "two tabs remain");
  ok &= Check(model.active_tab_id() == first,
              "next tab becomes active after close");

  ok &= Check(model.CloseTab(first), "first tab closes");
  ok &= Check(model.active_tab_id() == third,
              "third tab becomes active after close");

  ok &= Check(model.CloseTab(third), "last tab closes");
  ok &= Check(model.tabs().empty(), "no tabs remain");
  ok &= Check(!model.active_tab_id().has_value(), "no active tab remains");

  ok &= Check(!model.CloseTab(999), "unknown tab cannot close");
  ok &= Check(!model.MoveTab(999, 0), "unknown tab cannot move");
  ok &= Check(!model.ActivateTab(999), "unknown tab cannot activate");
  ok &= Check(!model.SetKind(999, secant::TabKind::kGlance),
              "unknown tab cannot change kind");
  ok &= Check(!model.SetFolder(999, 1), "unknown tab cannot change folder");

  if (!ok) {
    return 1;
  }
  std::cout << "tab_state_unittest: PASS\n";
  return 0;
}
