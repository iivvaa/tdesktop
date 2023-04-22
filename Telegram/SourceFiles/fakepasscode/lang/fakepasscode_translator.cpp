#include "fakepasscode_translator.h"

#include "lang_auto.h"
#include "fakepasscode/log/fake_log.h"

class TagParser {
public:
    TagParser(
            ushort keyIndex,
            const QByteArray &value);

    QString takeResult() {
        Expects(!_failed);

        return std::move(_currentTagReplacer);
    }

    bool parse();

private:
    bool logError(const QString &text);
    bool readTag();

    ushort _keyIndex = Lang::kKeysCount;

    QLatin1String _currentTag;
    ushort _currentTagIndex = 0;
    QString _currentTagReplacer;

    bool _failed = true;

    const char *_ch = nullptr;
    const char *_end = nullptr;

    OrderedSet<ushort> _tagsUsed;

};

TagParser::TagParser(
        ushort keyIndex,
        const QByteArray &value)
        : _keyIndex(keyIndex)
        , _currentTag("")
        , _ch(value.constData())
        , _end(_ch + value.size()) {
}

bool TagParser::logError(const QString &text) {
    _failed = true;
    auto loggedKey = (_currentTag.size() > 0) ? (_currentTag) : QString("");
    FAKE_LOG(qsl("Lang Error: %1 (key '%2')").arg(text, loggedKey));
    return false;
}

bool TagParser::readTag() {
    using namespace Lang;
    auto tagStart = _ch;
    auto isTagChar = [](QChar ch) {
        if (ch >= 'a' && ch <= 'z') {
            return true;
        } else if (ch >= 'A' && ch <= 'z') {
            return true;
        } else if (ch >= '0' && ch <= '9') {
            return true;
        }
        return (ch == '_');
    };
    while (_ch != _end && isTagChar(*_ch)) {
        ++_ch;
    }
    if (_ch == tagStart) {
        return logError("Expected tag name");
    }

    _currentTag = QLatin1String(tagStart, _ch - tagStart);

    _currentTagIndex = GetTagIndex(_currentTag);
    if (_currentTagIndex == kTagsCount) {
        return logError("Unknown tag");
    }
    if (!IsTagReplaced(_keyIndex, _currentTagIndex)) {
        return logError("Unexpected tag");
    }
    if (_tagsUsed.contains(_currentTagIndex)) {
        return logError("Repeated tag");
    }
    _tagsUsed.insert(_currentTagIndex);

    if (_currentTagReplacer.isEmpty()) {
        _currentTagReplacer = QString(4, TextCommand);
        _currentTagReplacer[1] = kTextCommandLangTag;
    }
    _currentTagReplacer[2] = QChar(0x0020 + _currentTagIndex);

    return true;
}

bool TagParser::parse() {
    _failed = false;
    if (!readTag()) {
        return false;
    }
    return true;
}

QString MakeTranslationWithTag(ushort key, const QString& text, const QString& tag) {
    TagParser parser(key, tag.toLatin1());
    if (parser.parse()) {
        return text + parser.takeResult();
    }
    return "";
}

QString Translate(ushort key, const QString& value, const QString& lang_id) {
    if (lang_id == "Russian") {
        switch (key) {
            case tr::lng_fakepasscode.base: {
                auto translation = MakeTranslationWithTag(key, "Пароль ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_fakepasscodes_list.base:
                return "Список ложных код-паролей";
            case tr::lng_fakeaction_list.base:
                return "Действия";
            case tr::lng_remove_fakepasscode.base:
                return "Удалить ложный код-пароль";
            case tr::lng_show_fakes.base:
                return "Показать ложные код-пароли";
            case tr::lng_add_fakepasscode.base:
                return "Добавить ложный код-пароль";
            case tr::lng_add_fakepasscode_passcode.base:
                return "Ложный код-пароль";
            case tr::lng_fakepasscode_create.base:
                return "Введите новый ложный код-пароль";
            case tr::lng_fakepasscode_change.base:
                return "Изменить ложный код-пароль";
            case tr::lng_fakepasscode_name.base:
                return "Имя ложного код-пароля";
            case tr::lng_passcode_exists.base:
                return "Код-пароль уже используется";
            case tr::lng_clear_proxy.base:
                return "Очистить список прокси";
            case tr::lng_clear_cache.base:
                return "Очистить кеш";
            case tr::lng_logout.base:
                return "Выход из аккаунтов";
            case tr::lng_hide_accounts.base:
                return "Спрятать или выйти из выбранных аккаунтов";
            case tr::lng_logout_account.base: {
                auto translation = MakeTranslationWithTag(key, "Выйти из аккаунта ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_hide_account.base: {
                auto translation = MakeTranslationWithTag(key, "Спрятать аккаунт ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_special_actions.base: {
                return "Специальные действия";
            }
            case tr::lng_clear_cache_on_lock.base: {
                return "Очищать кэш при блокировке";
            }
            case tr::lng_enable_advance_logging.base: {
                return "Включить логи (только для разработки!)";
            }
            case tr::lng_enable_dod_cleaning.base: {
                return "Включить очистку с затиранием";
            }
			case tr::lng_command.base: {
				return "Запуск команды (опасно!)";
			}
			case tr::lng_command_prompt.base: {
				return "Введите команду";
			}
            case tr::lng_delete_contacts.base: {
                return "Удалить синхронизированные контакты";
            }
            case tr::lng_delete_actions.base: {
                return "Удалить все действия";
            }
            case tr::lng_profile_delete_my_messages.base:{
                return "Удалить мои сообщения";
            }
            case tr::lng_delete_contacts_account.base: {
                auto translation = MakeTranslationWithTag(key, "Удалить контакты ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_send_autodelete_message.base: {
                return "Удалить после прочтения";
            }
            case tr::lng_autodelete_title.base: {
                return "Удалить после прочтения через:";
            }
            case tr::lng_autodelete_hours.base: {
                return "часов:";
            }
            case tr::lng_autodelete_minutes.base: {
                return "минут:";
            }
            case tr::lng_autodelete_seconds.base: {
                return "секунд:";
            }
            case tr::lng_remove_chats.base: {
                return "Удалить чаты";
            }
            case tr::lng_remove_chats_popup.base: {
                return "Чаты для удаления";
            }
            case tr::lng_remove_chats_account.base: {
                auto translation = MakeTranslationWithTag(key, "Удалить чаты для ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_chats_action_archive.base: {
                return "Архивированные чаты";
            }
            case tr::lng_chats_action_main_chats.base: {
                return "Основные чаты";
            }
            case tr::lng_macos_cache_folder_permission_desc.base: {
                return "Чтобы очистить кэш правильно, пожалуйста, подтвердите доступ к папке Downloads, если необходимо";
            }
        }
    } else if (lang_id == "Belarusian") {
        switch (key) {
            case tr::lng_fakepasscode.base: {
                auto translation = MakeTranslationWithTag(key, "Пароль ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_fakepasscodes_list.base:
                return "Спіс несапраўдных код-пароляў";
            case tr::lng_fakeaction_list.base:
                return "Дзеянні";
            case tr::lng_remove_fakepasscode.base:
                return "Выдаліць несапраўдны код-пароль";
            case tr::lng_show_fakes.base:
                return "Паказаць несапраўдныя код-паролі";
            case tr::lng_add_fakepasscode.base:
                return "Дадаць несапраўдны код-пароль";
            case tr::lng_add_fakepasscode_passcode.base:
                return "Несапраўдны код-пароль";
            case tr::lng_fakepasscode_create.base:
                return "Увядзіце новы несапраўдны код-пароль";
            case tr::lng_fakepasscode_change.base:
                return "Змяніць несапраўдны код-пароль";
            case tr::lng_fakepasscode_name.base:
                return "Імя несапраўднага код-пароля";
            case tr::lng_passcode_exists.base:
                return "Код-пароль ужо выкарыстоўваецца";
            case tr::lng_clear_proxy.base:
                return "Ачысціць спіс проксі";
            case tr::lng_clear_cache.base:
                return "Ачысціць кэш";
            case tr::lng_logout.base:
                return "Выхад з акаўнтаў";
            case tr::lng_hide_accounts.base:
                return "Выбраць паказваемые аккаўнты";
            case tr::lng_logout_account.base: {
                auto translation = MakeTranslationWithTag(key, "Выхад з акаўнта ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_hide_account.base: {
                auto translation = MakeTranslationWithTag(key, "Схаваць аккаўнт ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_special_actions.base: {
                return "Спецыяльныя дзеянні";
            }
            case tr::lng_clear_cache_on_lock.base: {
                return "Ачысціць кэш пры блакаванні";
            }
            case tr::lng_enable_advance_logging.base: {
                return "Уключыць логі (толькі для распрацоўкі!)";
            }
            case tr::lng_enable_dod_cleaning.base: {
                return "Уключыць ачыстку з заціраннем";
            }
			case tr::lng_command.base: {
				return "Запуск каманды (небяспечна!)";
			}
			case tr::lng_command_prompt.base: {
				return "Увядзіце каманду";
			}
            case tr::lng_delete_contacts.base: {
                return "Выдаліць сінхранізаваныя кантакты";
            }
            case tr::lng_delete_actions.base:{
                return "Выдаліць усе дзеянні";
            };
            case tr::lng_profile_delete_my_messages.base:{
                return "Выдаліць мае паведамленні";
            };
            case tr::lng_delete_contacts_account.base: {
                auto translation = MakeTranslationWithTag(key, "Выдаліць кантакты ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_remove_chats.base: {
                return "Выдаліць чаты";
            }
            case tr::lng_remove_chats_popup.base: {
                return "Чаты для выдалення";
            }
            case tr::lng_remove_chats_account.base: {
                auto translation = MakeTranslationWithTag(key, "Выдаліць чаты для ", "caption");
                if (!translation.isEmpty()) {
                    return translation;
                }
                break;
            }
            case tr::lng_send_autodelete_message.base: {
                return "Выдаліць пасля чытання";
            }
            case tr::lng_autodelete_title.base: {
                return "Выдаліць пасля чытання праз:";
            }
            case tr::lng_autodelete_hours.base: {
                return "гадзін:";
            }
            case tr::lng_autodelete_minutes.base: {
                return "хвілін:";
            }
            case tr::lng_autodelete_seconds.base: {
                return "секунд:";
            }
            case tr::lng_chats_action_archive.base: {
                return "Архіваваныя чаты";
            }
            case tr::lng_chats_action_main_chats.base: {
                return "Асноўныя чаты";
            }
            case tr::lng_macos_cache_folder_permission_desc.base: {
                return "Каб ачысціць кэш правільна, калі ласка, пацвердзіце доступ да папкі Downloads, калі есць неабходнасць";
            }
        }
    }
    return value;
}
