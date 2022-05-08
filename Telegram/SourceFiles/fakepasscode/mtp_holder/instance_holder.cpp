#include "instance_holder.h"
#include "mtp_holder.h"

#include <core/application.h>
#include <mtproto/facade.h>
#include <mtproto/mtp_instance.h>

namespace FakePasscode {

InstanceHolder::InstanceHolder(FakePasscode::FakeMtpHolder *parent, std::unique_ptr<MTP::Instance> &&instance)
    : _parent(parent)
    , _instance(std::move(instance))
    , _checkTimer([this]{check();})
    , _requestTimer([this]{logout();})
    , _logoutTimer([this]{die();})
{
    auto inst = _instance.get();
    Expects(parent != nullptr);
    Expects(inst != nullptr);

    inst->clearGlobalHandlers();
    inst->clearCallbacks();
    inst->lifetime().destroy();

    if (completed()) {
        //first scenario. No critical request registered. But we let instance to exist for 1 sec to complete possible requests
        _requestTimer.callOnce(1000);
    } else {
        //second scenario. Wait for all requests, but with 5 sec deadline
        _requestTimer.callOnce(5000);
        _checkTimer.callEach(100);
    }
}

bool InstanceHolder::completed() const {
    auto critRequests = _parent->getCriticalRequests(_instance.get());
    for (mtpRequestId request : critRequests) {
        if (_instance->state(request) != MTP::RequestSent) {
            return false;
        }
    }
    return true;
}

void InstanceHolder::check() {
    if (completed()) {
        logout();
    }
}

void InstanceHolder::logout() {
    _requestTimer.cancel();
    _checkTimer.cancel();
    _instance->logout([this]{die();});
    _logoutTimer.callOnce(1000);
}

void InstanceHolder::die() {
    _logoutTimer.cancel();
    _instance->clearCallbacks();
    Core::App().postponeCall(crl::guard(this, [this]{
        _parent->destroy(this);
    }));
}

}
