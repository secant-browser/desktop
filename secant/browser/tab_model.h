#ifndef SECANT_BROWSER_TAB_MODEL_H_
#define SECANT_BROWSER_TAB_MODEL_H_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace secant {

using TabId = std::uint64_t;
using WorkspaceId = std::uint64_t;
using FolderId = std::uint64_t;

enum class TabKind {
  kNormal,
  kPinned,
  kEssential,
  kSplit,
  kGlance,
};

struct Tab {
  TabId id = 0;
  WorkspaceId workspace_id = 0;
  std::optional<FolderId> folder_id;
  std::string url;
  std::string title;
  TabKind kind = TabKind::kNormal;
  bool active = false;
};

class SecantTabModel {
 public:
  SecantTabModel() = default;
  SecantTabModel(const SecantTabModel&) = delete;
  SecantTabModel& operator=(const SecantTabModel&) = delete;

  TabId CreateTab(WorkspaceId workspace_id, std::string url, std::string title);
  bool CloseTab(TabId id);
  bool MoveTab(TabId id, std::size_t index);
  bool ActivateTab(TabId id);
  bool SetKind(TabId id, TabKind kind);
  bool SetFolder(TabId id, std::optional<FolderId> folder_id);

  const std::vector<Tab>& tabs() const { return tabs_; }
  const Tab* GetTab(TabId id) const;
  std::vector<const Tab*> TabsForWorkspace(WorkspaceId workspace_id) const;
  std::optional<TabId> active_tab_id() const { return active_tab_id_; }

 private:
  std::vector<Tab>::iterator Find(TabId id);
  std::vector<Tab>::const_iterator Find(TabId id) const;

  TabId next_id_ = 1;
  std::vector<Tab> tabs_;
  std::optional<TabId> active_tab_id_;
};

}  // namespace secant

#endif  // SECANT_BROWSER_TAB_MODEL_H_
