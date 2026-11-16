#include "secant/browser/tab_model.h"

#include <algorithm>
#include <utility>

namespace secant {

TabId SecantTabModel::CreateTab(WorkspaceId workspace_id,
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

bool SecantTabModel::CloseTab(TabId id) {
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

bool SecantTabModel::MoveTab(TabId id, std::size_t index) {
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

bool SecantTabModel::ActivateTab(TabId id) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return false;
  }

  for (Tab& tab : tabs_) {
    tab.active = tab.id == id;
  }
  active_tab_id_ = id;
  return true;
}

bool SecantTabModel::SetKind(TabId id, TabKind kind) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return false;
  }
  it->kind = kind;
  return true;
}

bool SecantTabModel::SetFolder(TabId id, std::optional<FolderId> folder_id) {
  auto it = Find(id);
  if (it == tabs_.end()) {
    return false;
  }
  it->folder_id = folder_id;
  return true;
}

const Tab* SecantTabModel::GetTab(TabId id) const {
  auto it = Find(id);
  return it == tabs_.end() ? nullptr : &*it;
}

std::vector<const Tab*> SecantTabModel::TabsForWorkspace(
    WorkspaceId workspace_id) const {
  std::vector<const Tab*> result;
  for (const Tab& tab : tabs_) {
    if (tab.workspace_id == workspace_id) {
      result.push_back(&tab);
    }
  }
  return result;
}

std::vector<Tab>::iterator SecantTabModel::Find(TabId id) {
  return std::find_if(tabs_.begin(), tabs_.end(),
                      [id](const Tab& tab) { return tab.id == id; });
}

std::vector<Tab>::const_iterator SecantTabModel::Find(TabId id) const {
  return std::find_if(tabs_.begin(), tabs_.end(),
                      [id](const Tab& tab) { return tab.id == id; });
}

}  // namespace secant
