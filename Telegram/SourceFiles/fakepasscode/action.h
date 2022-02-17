#ifndef TELEGRAM_ACTION_H
#define TELEGRAM_ACTION_H

#include <QByteArray>
#include <memory>

namespace FakePasscode {
    enum class ActionType {
        ClearProxy = 0,
        ClearCache = 1,
        Logout = 2,
        Command = 3
    };

    const static std::vector<ActionType> kAvailableActions = {
        ActionType::ClearProxy,
        ActionType::ClearCache,
        ActionType::Logout,
        ActionType::Command
    };

    class Action {
    public:
        virtual ~Action() = default;

        virtual void Execute() = 0;

        virtual QByteArray Serialize() const = 0;

        virtual ActionType GetType() const = 0;
    };

    std::shared_ptr<Action> DeSerialize(QByteArray serialized);
    std::shared_ptr<Action> CreateAction(ActionType type, const QByteArray& inner_data = QByteArray());
}

#endif //TELEGRAM_ACTION_H
