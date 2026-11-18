#include "secant/browser/tab_model.h"

#include <memory>
#include <optional>
#include <string>

#include "base/functional/bind.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/tabs/test_tab_strip_model_delegate.h"
#include "chrome/test/base/testing_profile.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_renderer_host.h"
#include "content/public/test/web_contents_tester.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace secant {
namespace {

class SecantTabModelTest : public testing::Test {
 public:
  void SetUp() override {
    profile_ = std::make_unique<TestingProfile>();
    tab_strip_model_ =
        std::make_unique<TabStripModel>(&delegate_, profile_.get());
    model_ = std::make_unique<SecantTabModel>(
        tab_strip_model_.get(),
        base::BindRepeating(
            [](Profile* profile) {
              return content::WebContentsTester::CreateTestWebContents(profile,
                                                                       nullptr);
            },
            profile_.get()));
  }

  void TearDown() override {
    model_.reset();
    tab_strip_model_.reset();
    profile_.reset();
  }

  TabId CreateTab(WorkspaceId workspace_id,
                  const std::string& url,
                  const std::string& title) {
    return model_->CreateTab(workspace_id, url, title);
  }

 protected:
  content::BrowserTaskEnvironment task_environment_;
  content::RenderViewHostTestEnabler render_view_host_test_enabler_;
  std::unique_ptr<TestingProfile> profile_;
  TestTabStripModelDelegate delegate_;
  std::unique_ptr<TabStripModel> tab_strip_model_;
  std::unique_ptr<SecantTabModel> model_;
};

TEST_F(SecantTabModelTest, CreateAndActivate) {
  const TabId first = CreateTab(1, "https://first.example", "First");
  const TabId second = CreateTab(1, "https://second.example", "Second");

  ASSERT_NE(first, 0u);
  ASSERT_NE(second, 0u);
  ASSERT_EQ(model_->tabs().size(), 2u);
  ASSERT_TRUE(model_->active_tab_id().has_value());
  EXPECT_EQ(*model_->active_tab_id(), first);

  const Tab* first_tab = model_->GetTab(first);
  ASSERT_NE(first_tab, nullptr);
  EXPECT_EQ(first_tab->title, "First");
  EXPECT_TRUE(first_tab->active);
  EXPECT_EQ(tab_strip_model_->count(), 2);

  EXPECT_TRUE(model_->ActivateTab(second));
  ASSERT_TRUE(model_->active_tab_id().has_value());
  EXPECT_EQ(*model_->active_tab_id(), second);
  EXPECT_FALSE(model_->GetTab(first)->active);
}

TEST_F(SecantTabModelTest, MovePreservesActiveTab) {
  const TabId first = CreateTab(1, "https://first.example", "First");
  const TabId second = CreateTab(1, "https://second.example", "Second");
  ASSERT_TRUE(model_->ActivateTab(second));

  EXPECT_TRUE(model_->MoveTab(first, 1));
  ASSERT_EQ(model_->tabs().size(), 2u);
  EXPECT_EQ(model_->tabs()[0].id, second);
  EXPECT_EQ(model_->tabs()[1].id, first);
  ASSERT_TRUE(model_->active_tab_id().has_value());
  EXPECT_EQ(*model_->active_tab_id(), second);

  EXPECT_TRUE(model_->MoveTab(first, 99));
  EXPECT_EQ(model_->tabs()[0].id, second);
  EXPECT_EQ(model_->tabs()[1].id, first);
}

TEST_F(SecantTabModelTest, Metadata) {
  const TabId first = CreateTab(1, "https://first.example", "First");

  EXPECT_TRUE(model_->SetKind(first, TabKind::kPinned));
  ASSERT_NE(model_->GetTab(first), nullptr);
  EXPECT_EQ(model_->GetTab(first)->kind, TabKind::kPinned);
  EXPECT_TRUE(tab_strip_model_->IsTabPinned(0));

  EXPECT_TRUE(model_->SetFolder(first, 7));
  ASSERT_TRUE(model_->GetTab(first)->folder_id.has_value());
  EXPECT_EQ(*model_->GetTab(first)->folder_id, 7u);
  EXPECT_TRUE(model_->SetFolder(first, std::nullopt));
  EXPECT_FALSE(model_->GetTab(first)->folder_id.has_value());

  EXPECT_EQ(model_->TabsForWorkspace(1).size(), 1u);
  EXPECT_TRUE(model_->TabsForWorkspace(2).empty());
}

TEST_F(SecantTabModelTest, CloseSelectsNextTab) {
  const TabId first = CreateTab(1, "https://first.example", "First");
  const TabId second = CreateTab(1, "https://second.example", "Second");
  ASSERT_TRUE(model_->ActivateTab(second));

  EXPECT_TRUE(model_->CloseTab(second));
  ASSERT_EQ(model_->tabs().size(), 1u);
  ASSERT_TRUE(model_->active_tab_id().has_value());
  EXPECT_EQ(*model_->active_tab_id(), first);

  EXPECT_TRUE(model_->CloseTab(first));
  EXPECT_TRUE(model_->tabs().empty());
  EXPECT_FALSE(model_->active_tab_id().has_value());
}

TEST_F(SecantTabModelTest, EmptyTitleFallsBackToUrl) {
  const TabId first = CreateTab(1, "https://first.example", "First");
  const TabId third = CreateTab(2, "https://third.example", "");

  ASSERT_NE(third, 0u);
  ASSERT_NE(model_->GetTab(third), nullptr);
  EXPECT_EQ(model_->GetTab(third)->title, "https://third.example");
  ASSERT_TRUE(model_->active_tab_id().has_value());
  EXPECT_EQ(*model_->active_tab_id(), first);
  EXPECT_EQ(model_->TabsForWorkspace(2).size(), 1u);
}

TEST_F(SecantTabModelTest, UnknownIdsFail) {
  EXPECT_FALSE(model_->CloseTab(999));
  EXPECT_FALSE(model_->MoveTab(999, 0));
  EXPECT_FALSE(model_->ActivateTab(999));
  EXPECT_FALSE(model_->SetKind(999, TabKind::kGlance));
  EXPECT_FALSE(model_->SetFolder(999, 1));
}

}  // namespace
}  // namespace secant
