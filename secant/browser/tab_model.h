#ifndef SECANT_BROWSER_TAB_MODEL_H_
#define SECANT_BROWSER_TAB_MODEL_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "components/tabs/public/tab_interface.h"
#include "content/public/browser/web_contents.h"
#include "secant/browser/tab_state.h"

class Profile;
class TabStripModel;

namespace secant {

// Adapter over Chromium's TabStripModel. Tab lifecycle and activation are
// delegated to Chromium; Secant-specific metadata (workspace, folder, kind)
// stays in SecantTabState.
class SecantTabModel : public TabStripModelObserver {
 public:
  using WebContentsFactory =
      base::RepeatingCallback<std::unique_ptr<content::WebContents>()>;

  SecantTabModel(TabStripModel* tab_strip_model, Profile* profile);
  SecantTabModel(TabStripModel* tab_strip_model,
                 WebContentsFactory web_contents_factory);
  ~SecantTabModel() override;

  SecantTabModel(const SecantTabModel&) = delete;
  SecantTabModel& operator=(const SecantTabModel&) = delete;

  TabId CreateTab(WorkspaceId workspace_id, std::string url, std::string title);
  bool CloseTab(TabId id);
  bool MoveTab(TabId id, std::size_t index);
  bool ActivateTab(TabId id);
  bool SetKind(TabId id, TabKind kind);
  bool SetFolder(TabId id, std::optional<FolderId> folder_id);

  const std::vector<Tab>& tabs() const { return state_.tabs(); }
  const Tab* GetTab(TabId id) const { return state_.GetTab(id); }
  std::vector<const Tab*> TabsForWorkspace(WorkspaceId workspace_id) const {
    return state_.TabsForWorkspace(workspace_id);
  }
  std::optional<TabId> active_tab_id() const {
    return state_.active_tab_id();
  }

 private:
  // TabStripModelObserver:
  void OnTabStripModelChanged(
      TabStripModel* tab_strip_model,
      const TabStripModelChange& change,
      const TabStripSelectionChange& selection) override;
  void OnTabChangedAt(tabs::TabInterface* tab,
                      TabChangeType change_type) override;
  void OnTabPinnedStateChanged(tabs::TabInterface* tab, int index) override;
  void OnTabStripModelDestroyed(TabStripModel* tab_strip_model) override;

  TabId FindId(tabs::TabInterface* tab) const;
  tabs::TabInterface* FindTab(TabId id) const;
  int IndexOf(TabId id) const;
  void RecordTab(TabId id, tabs::TabInterface* tab);
  void ForgetTab(TabId id);
  void SyncOrder();
  void SyncActive();
  void UpdateTab(tabs::TabInterface* tab);

  raw_ptr<TabStripModel> tab_strip_model_;
  WebContentsFactory web_contents_factory_;
  SecantTabState state_;
  std::map<int32_t, TabId> id_by_handle_;
  std::map<TabId, int32_t> handle_by_id_;
};

}  // namespace secant

#endif  // SECANT_BROWSER_TAB_MODEL_H_
