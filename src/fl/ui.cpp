#include "fl/ui.h"
#include "fl/stdint.h"
#include "fl/compiler_control.h"

FL_DISABLE_WARNING_PUSH
FL_DISABLE_WARNING(float-equal)

namespace fl {

void UISlider::setValue(float value) {
    float oldValue = Super::value();
    if (value != oldValue) {
        Super::setValue(value);
        // Update the last frame value to keep state consistent
        mLastFrameValue = value;
        mLastFramevalueValid = true;
        // Invoke callbacks to notify listeners (including JavaScript components)
        mCallbacks.invoke(*this);
    }
}

void UISlider::Listener::onBeginFrame() {
    UISlider &owner = *mOwner;
    if (!owner.mLastFramevalueValid) {
        owner.mLastFrameValue = owner.value();
        owner.mLastFramevalueValid = true;
        return;
    }
    float value = owner.value();
    if (value != owner.mLastFrameValue) {
        owner.mCallbacks.invoke(*mOwner);
        owner.mLastFrameValue = value;
    }
}

void UIButton::Listener::onBeginFrame() {
    bool clicked_this_frame = mOwner->clicked();
    
    // Check the real button if one is attached
    if (mOwner->mRealButton) {
        if (mOwner->mRealButton->isPressed()) {
            clicked_this_frame = true;
            //mOwner->click(); // Update the UI button state
        }
    }
    
    const bool clicked_changed = (clicked_this_frame != mClickedLastFrame);
    mClickedLastFrame = clicked_this_frame;
    if (clicked_changed) {
        // FASTLED_WARN("Button: " << mOwner->name() << " clicked: " <<
        // mOwner->clicked());
        mOwner->mCallbacks.invoke(*mOwner);
    }
    // mOwner->mCallbacks.invoke(*mOwner);
}

void UICheckbox::Listener::onBeginFrame() {
    UICheckbox &owner = *mOwner;
    if (!owner.mLastFrameValueValid) {
        owner.mLastFrameValue = owner.value();
        owner.mLastFrameValueValid = true;
        return;
    }
    bool value = owner.value();
    if (value != owner.mLastFrameValue) {
        owner.mCallbacks.invoke(owner);
        owner.mLastFrameValue = value;
    }
}

void UINumberField::Listener::onBeginFrame() {
    UINumberField &owner = *mOwner;
    if (!owner.mLastFrameValueValid) {
        owner.mLastFrameValue = owner.value();
        owner.mLastFrameValueValid = true;
        return;
    }
    double value = owner.value();
    if (value != owner.mLastFrameValue) {
        owner.mCallbacks.invoke(owner);
        owner.mLastFrameValue = value;
    }
}

void UIDropdown::Listener::onBeginFrame() {
    UIDropdown &owner = *mOwner;
    if (!owner.mLastFrameValueValid) {
        owner.mLastFrameValue = owner.value_int();
        owner.mLastFrameValueValid = true;
        return;
    }
    int value = owner.value_int();
    if (value != owner.mLastFrameValue) {
        owner.mCallbacks.invoke(owner);
        owner.mLastFrameValue = value;
    }
}

} // end namespace fl

FL_DISABLE_WARNING_POP
