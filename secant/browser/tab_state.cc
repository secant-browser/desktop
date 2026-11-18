#include "secant/browser/tab_state.h"

#include <algorithm>
#include <utility>

namespace secant {

SecantTabState::SecantTabState() = default;
SecantTabState::~SecantTabState() = default;

TabId SecantTabState::CreateTab(WorkspaceId workspace_id,
                                std::string url,
                                std::string title) {
  Tab tab;
  tab.id = next_id_++;
  tab.workspace_id = workspace_id;
  tab.url = std::move(url);
  tab.title = title.empty() ? tab.url : std::move(title);
  tabs_.push_back(std::move(tab));

  const TabId id = tabs_.back().id;
  if (!active_tab_id_) {
    ActivateTab(id);
  }
  return id;
}

bool SecantTabState::CloseTab(TabId id) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return false;
  }

  const bool was_active = active_tab_id_ && *active_tab_id_ == id;
  const std::size_t index = static_cast<std::size_t>(it - tabs_.begin());
  tabs_.erase(it);

  if (!was_active) {
    return true;
  }

  active_tab_id_.reset();
  if (!tabs_.empty()) {
    const std::size_t next_index = std::min(index, tabs_.size() - 1);
    ActivateTab(tabs_[next_index].id);
  }
  return true;
}

bool SecantTabState::MoveTab(TabId id, std::size_t index) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return false;
  }

  const std::size_t old_index = static_cast<std::size_t>(it - tabs_.begin());
  const std::size_t new_index = std::min(index, tabs_.size() - 1);
  if (old_index == new_index) {
    return true;
  }

  Tab tab = std::move(*it);
  tabs_.erase(it);
  tabs_.insert(tabs_.begin() + new_index, std::move(tab));
  return true;
}

bool SecantTabState::ActivateTab(TabId id) {
  if (Find(id) == tabs_.end()) {
    return false;
  }
  SetActiveTab(id);
  return true;
}

bool SecantTabState::SetKind(TabId id, TabKind kind) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return false;
  }
  it->kind = kind;
  return true;
}

bool SecantTabState::SetFolder(TabId id, std::optional<FolderId> folder_id) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return false;
  }
  it->folder_id = folder_id;
  return true;
}

void SecantTabState::SetActiveTab(std::optional<TabId> id) {
  active_tab_id_ = id;
  for (Tab& tab : tabs_) {
    tab.active = id.has_value() && tab.id == *id;
  }
}

void SecantTabState::SetTabInfo(TabId id,
                                std::string url,
                                std::string title) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return;
  }
  if (!url.empty()) {
    it->url = std::move(url);
  }
  if (!title.empty()) {
    it->title = std::move(title);
  } else if (it->title.empty()) {
    it->title = it->url;
  }
}

void SecantTabState::Reorder(const std::vector<TabId>& order) {
  std::vector<Tab> reordered;
  reordered.reserve(order.size());
  for (TabId id : order) {
    auto it = Find(id);
    if (it != tabs_.end()) {
      reordered.push_back(std::move(*it));
    }
  }
  tabs_ = std::move(reordered);
}

const Tab* SecantTabState::GetTab(TabId id) const {
  auto it = Find(id);
  return it == tabs_.end() ? nullptr : &*it;
}

Tab* SecantTabState::MutableTab(TabId id) {
  auto it = Find(id);
  return it == tabs_.end() ? nullptr : &*it;
}

std::vector<const Tab*> SecantTabState::TabsForWorkspace(
    WorkspaceId workspace_id) const {
  std::vector<const Tab*> result;
  for (const Tab& tab : tabs_) {
    if (tab.workspace_id == workspace_id) {
      result.push_back(&tab);
    }
  }
  return result;
}

std::vector<Tab>::iterator SecantTabState::Find(TabId id) {
  return std::find_if(tabs_.begin(), tabs_.end(),
                      [id](const Tab& tab) { return tab.id == id; });
}

std::vector<Tab>::const_iterator SecantTabState::Find(TabId id) const {
  return std::find_if(tabs_.begin(), tabs_.end(),
                      [id](const Tab& tab) { return tab.id == id; });
}

}  // namespace secant
