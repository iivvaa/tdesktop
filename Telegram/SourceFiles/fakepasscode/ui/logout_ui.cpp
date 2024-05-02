#include "logout_ui.h"
#include "settings/settings_common.h"
#include "ui/widgets/buttons.h"
#include "ui/vertical_list.h"
#include "lang/lang_keys.h"
#include "main/main_domain.h"
#include "storage/storage_domain.h"
#include "data/data_user.h"
#include "core/application.h"
#include "main/main_session.h"
#include "main/main_domain.h"
#include "main/main_account.h"
#include "styles/style_settings.h"
#include "fakepasscode/log/fake_log.h"
#include "styles/style_menu_icons.h"
#include "ui/toast/toast.h"
#include "window/window_session_controller.h"
#include "window/window_controller.h"
#include "ui/layers/generic_box.h"
#include "styles/style_layers.h" // st::boxLabel

void LogoutUI::Create(not_null<Ui::VerticalLayout *> content,
                      Window::SessionController* session) {
    FakePasscode::HideAccountKind::HideAccountEnum value 
        = (_action != nullptr) ? _action->GetData(_accountIndex).Kind : FakePasscode::HideAccountKind::None;

    const auto tgl_logout = Ui::CreateChild<rpl::event_stream<bool>>(content.get());
    auto btn_logout = Settings::AddButtonWithIcon(
            content,
            tr::lng_logout(),
            st::settingsButton,
            {&st::menuIconLeave}
    )->toggleOn(tgl_logout->events_starting_with_copy(value == FakePasscode::HideAccountKind::Logout));
    const auto tgl_hide = Ui::CreateChild<rpl::event_stream<bool>>(content.get());
    auto btn_hide = Settings::AddButtonWithIcon(
            content,
            tr::lng_hide(),
            st::settingsButton,
            {&st::menuIconClear}
    )->toggleOn(tgl_hide->events_starting_with_copy(value == FakePasscode::HideAccountKind::HideAccount));
    
    auto clickHandler = [this, btn_logout, tgl_logout, btn_hide, tgl_hide, session] {
        bool is_logout = btn_logout->toggled();
        bool is_hide = btn_hide->toggled();

        FakePasscode::HideAccountKind::HideAccountEnum old_value
            = (_action != nullptr) ? _action->GetData(_accountIndex).Kind : FakePasscode::HideAccountKind::None;

        if (is_hide && old_value == FakePasscode::HideAccountKind::Logout) {
            // somebody clicked hide, when logout was active
            is_logout = false;
        }

        FakePasscode::HideAccountKind value;
        if (is_logout) {
            value = { FakePasscode::HideAccountKind::Logout };
        } else if (is_hide) {
            value = { FakePasscode::HideAccountKind::HideAccount };
        } else {
            value = { FakePasscode::HideAccountKind::None };
        }

        if (!_action) {
            _action = dynamic_cast<FakePasscode::LogoutAction*>(
                _domain->local().AddAction(_index, FakePasscode::ActionType::Logout));
        }

        if (value.Kind == FakePasscode::HideAccountKind::HideAccount) {
            if (Core::App().domain().local().ContainsAction(_index, ::FakePasscode::ActionType::DeleteActions)) {
                Ui::Toast::Show(tr::lng_delete_actions_hidden_conflict_err(tr::now));
                value = { old_value };
            }
        }

        Expects(_action != nullptr);
        if (_action) {
            FAKE_LOG(qsl("LogoutUI: Try Set %1 to %2").arg(_accountIndex).arg(value.Kind));

            QString error = _action->SetIfValid(_accountIndex, value);
            if (!error.isEmpty()) {
                session->window().show(Box([=](not_null<Ui::GenericBox*> box) {
                    box->setTitle(tr::lng_cant_change_value_title());
                    box->addRow(object_ptr<Ui::FlatLabel>(
                        box,
                        error,
                        st::boxLabel));
                    box->setCloseByOutsideClick(true);
                    box->addButton(tr::lng_box_ok(), [=] {
                        box->closeBox();
                    });
                }));
                value = _action->GetData(_accountIndex);
            }
        }


        // not for logout 
        tgl_logout->fire(value.Kind == FakePasscode::HideAccountKind::Logout);
        tgl_hide->fire(value.Kind == FakePasscode::HideAccountKind::HideAccount);

        _domain->local().writeAccounts();
    };

    btn_logout->addClickHandler(clickHandler);
    btn_hide->addClickHandler(clickHandler);
}
