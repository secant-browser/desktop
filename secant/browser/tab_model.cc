#include "secant/browser/tab_model.h"

#include <algorithm>
#include <utility>

#include "base/functional/bind.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/tabs/tab_enums.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"

namespace secant {

SecantTabModel::SecantTabModel(TabStripModel* tab_strip_model, Profile* profile)
    : SecantTabModel(
          tab_strip_model,
          base::BindRepeating(
              [](Profile* profile) {
                return content::WebContents::Create(
                    content::WebContents::CreateParams(profile));
              },
              profile)) {}

SecantTabModel::SecantTabModel(TabStripModel* tab_strip_model,
                               WebContentsFactory web_contents_factory)
    : tab_strip_model_(tab_strip_model),
      web_contents_factory_(std::move(web_contents_factory)) {
  tab_strip_model_->AddObserver(this);
}

SecantTabModel::~SecantTabModel() {
  if (tab_strip_model_) {
    tab_strip_model_->RemoveObserver(this);
  }
}

TabId SecantTabModel::CreateTab(WorkspaceId workspace_id,
                                std::string url,
                                std::string title) {
  if (!tab_strip_model_ || web_contents_factory_.is_null()) {
    return 0;
  }

  std::unique_ptr<content::WebContents> contents = web_contents_factory_.Run();
  if (!contents) {
    return 0;
  }

  content::WebContents* raw_contents = contents.get();
  const bool activate = tab_strip_model_->count() == 0;
  tab_strip_model_->AppendWebContents(std::move(contents), activate);

  tabs::TabInterface* tab =
      tab_strip_model_->GetTabForWebContents(raw_contents);
  if (!tab) {
    return 0;
  }

  if (url.empty()) {
    url = tab->GetURL().spec();
  }
  if (title.empty()) {
    title = base::UTF16ToUTF8(tab->GetTitle());
  }

  const TabId id =
      state_.CreateTab(workspace_id, std::move(url), std::move(title));
  RecordTab(id, tab);
  UpdateTab(tab);
  SyncOrder();
  SyncActive();
  return id;
}

bool SecantTabModel::CloseTab(TabId id) {
  const int index = IndexOf(id);
  if (index == TabStripModel::kNoTab) {
    return false;
  }

  tab_strip_model_->CloseWebContentsAt(
      index, CLOSE_USER_GESTURE | CLOSE_CREATE_HISTORICAL_TAB);
  return true;
}

bool SecantTabModel::MoveTab(TabId id, std::size_t index) {
  const int from_index = IndexOf(id);
  if (from_index == TabStripModel::kNoTab || tab_strip_model_->count() == 0) {
    return false;
  }

  const int target_index = static_cast<int>(
      std::min(index, static_cast<std::size_t>(tab_strip_model_->count() - 1)));
  tab_strip_model_->MoveWebContentsAt(from_index, target_index,
                                      /*select_after_move=*/false);
  SyncOrder();
  SyncActive();
  return true;
}

bool SecantTabModel::ActivateTab(TabId id) {
  const int index = IndexOf(id);
  if (index == TabStripModel::kNoTab) {
    return false;
  }

  tab_strip_model_->ActivateTabAt(index);
  state_.ActivateTab(id);
  SyncActive();
  return true;
}

bool SecantTabModel::SetKind(TabId id, TabKind kind) {
  const int index = IndexOf(id);
  if (index == TabStripModel::kNoTab) {
    return false;
  }

  if (kind == TabKind::kPinned || kind == TabKind::kEssential) {
    if (tab_strip_model_->SetTabPinned(index, true) ==
        TabStripModel::kNoTab) {
      return false;
    }
  } else if (kind == TabKind::kNormal) {
    tab_strip_model_->SetTabPinned(index, false);
  }

  state_.SetKind(id, kind);
  SyncOrder();
  SyncActive();
  return true;
}

bool SecantTabModel::SetFolder(TabId id, std::optional<FolderId> folder_id) {
  return state_.SetFolder(id, folder_id);
}

void SecantTabModel::OnTabStripModelChanged(
    TabStripModel*,
    const TabStripModelChange& change,
    const TabStripSelectionChange&) {
  if (change.type() == TabStripModelChange::kRemoved) {
    for (const TabStripModelChange::RemovedTab& removed :
         change.GetRemove()->contents) {
      if (!removed.tab) {
        continue;
      }
      const TabId id = FindId(removed.tab);
      if (!id) {
        continue;
      }
      state_.CloseTab(id);
      ForgetTab(id);
    }
  }

  SyncOrder();
  SyncActive();
}

void SecantTabModel::OnTabChangedAt(tabs::TabInterface* tab, TabChangeType) {
  UpdateTab(tab);
}

void SecantTabModel::OnTabPinnedStateChanged(tabs::TabInterface* tab, int) {
  UpdateTab(tab);
  SyncOrder();
}

void SecantTabModel::OnTabStripModelDestroyed(TabStripModel*) {
  tab_strip_model_ = nullptr;
}

TabId SecantTabModel::FindId(tabs::TabInterface* tab) const {
  if (!tab) {
    return 0;
  }
  auto it = id_by_handle_.find(tab->GetHandle().raw_value());
  return it == id_by_handle_.end() ? 0 : it->second;
}

tabs::TabInterface* SecantTabModel::FindTab(TabId id) const {
  auto it = handle_by_id_.find(id);
  if (it == handle_by_id_.end()) {
    return nullptr;
  }
  return tabs::TabHandle(it->second).Get();
}

int SecantTabModel::IndexOf(TabId id) const {
  if (!tab_strip_model_) {
    return TabStripModel::kNoTab;
  }
  tabs::TabInterface* tab = FindTab(id);
  return tab ? tab_strip_model_->GetIndexOfTab(tab) : TabStripModel::kNoTab;
}

void SecantTabModel::RecordTab(TabId id, tabs::TabInterface* tab) {
  const int32_t handle = tab->GetHandle().raw_value();
  id_by_handle_[handle] = id;
  handle_by_id_[id] = handle;
}

void SecantTabModel::ForgetTab(TabId id) {
  auto it = handle_by_id_.find(id);
  if (it == handle_by_id_.end()) {
    return;
  }
  id_by_handle_.erase(it->second);
  handle_by_id_.erase(it);
}

void SecantTabModel::SyncOrder() {
  if (!tab_strip_model_) {
    return;
  }

  std::vector<TabId> order;
  order.reserve(tab_strip_model_->count());
  for (int i = 0; i < tab_strip_model_->count(); ++i) {
    tabs::TabInterface* tab = tab_strip_model_->GetTabAtIndex(i);
    const TabId id = FindId(tab);
    if (id) {
      order.push_back(id);
    }
  }
  state_.Reorder(order);
}

void SecantTabModel::SyncActive() {
  if (!tab_strip_model_) {
    state_.SetActiveTab(std::nullopt);
    return;
  }

  content::WebContents* contents = tab_strip_model_->GetActiveWebContents();
  tabs::TabInterface* tab =
      contents ? tab_strip_model_->GetTabForWebContents(contents) : nullptr;
  const TabId id = FindId(tab);
  state_.SetActiveTab(id ? std::optional<TabId>(id) : std::nullopt);
}

void SecantTabModel::UpdateTab(tabs::TabInterface* tab) {
  const TabId id = FindId(tab);
  if (!id) {
    return;
  }

  Tab* state = state_.MutableTab(id);
  if (!state) {
    return;
  }

  const std::string url = tab->GetURL().spec();
  const std::string title = base::UTF16ToUTF8(tab->GetTitle());
  state_.SetTabInfo(id, url, title);

  state = state_.MutableTab(id);
  if (state && (state->kind == TabKind::kNormal ||
                state->kind == TabKind::kPinned)) {
    state->kind = tab->IsPinned() ? TabKind::kPinned : TabKind::kNormal;
  }
}

}  // namespace secant
