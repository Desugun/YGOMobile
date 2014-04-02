#include <jni.h>
#include "irrlicht.h"
#include "../Classes/gframe/BufferIO.h"
#include "NativeCrashHandler.h"
#include "../Classes/gframe/os.h"
#include <unistd.h>
#include <pthread.h>

using namespace irr;
using namespace gui;
extern "C" {
/*
 * Class:     cn_garymb_ygomobile_core_IrrlichtBridge
 * Method:    nativeInsertText
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_cn_garymb_ygomobile_core_IrrlichtBridge_nativeInsertText(JNIEnv* env, jclass clazz,
		jint handle, jstring textString) {
	if (handle) {
			IrrlichtDevice* device = (IrrlichtDevice*)handle;
			IGUIEnvironment* irrenv = device->getGUIEnvironment();
			IGUIElement* element = irrenv->getFocus();
			if (element && element->getType() == EGUIET_EDIT_BOX) {
				IGUIEditBox* editbox = (IGUIEditBox*)element;
				const char* text = env->GetStringUTFChars(textString, NULL);
				wchar_t content[256];
				BufferIO::DecodeUTF8(text, content);
				editbox->setText(content);
				irrenv->removeFocus(editbox);
				irrenv->setFocus(editbox->getParent());
				SEvent changeEvent;
				changeEvent.EventType = EET_GUI_EVENT;
				changeEvent.GUIEvent.Caller = editbox;
				changeEvent.GUIEvent.Element = 0;
				changeEvent.GUIEvent.EventType = EGET_EDITBOX_CHANGED;
				editbox->getParent()->OnEvent(changeEvent);
				SEvent enterEvent;
				enterEvent.EventType = EET_GUI_EVENT;
				enterEvent.GUIEvent.Caller = editbox;
				enterEvent.GUIEvent.Element = 0;
				enterEvent.GUIEvent.EventType = EGET_EDITBOX_ENTER;
				editbox->getParent()->OnEvent(enterEvent);
				env->DeleteLocalRef(textString);
			}
		}
}

/*
 * Class:     cn_garymb_ygomobile_core_IrrlichtBridge
 * Method:    nativeSetComboBoxSelection
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_cn_garymb_ygomobile_core_IrrlichtBridge_nativeSetComboBoxSelection(JNIEnv* env, jclass clazz,
		jint handle, jint idx) {
	if (handle) {
		IrrlichtDevice* device = (IrrlichtDevice*) handle;
		IGUIEnvironment* irrenv = device->getGUIEnvironment();
		IGUIElement* element = irrenv->getFocus();
		if (element && element->getParent()->getType() ==  EGUIET_COMBO_BOX) {
			IGUIComboBox* combo = (IGUIComboBox*)(element->getParent());
			core::list<IGUIElement*> children = combo->getChildren();
			core::list<IGUIElement*>::Iterator current = children.begin();
			do {
				if ((*current)->getType() == EGUIET_LIST_BOX) {
					break;
				}
				current ++;
			}while(current != children.end());
			if (current == children.end()) {
				return;
			}
			IGUIListBox* list = (IGUIListBox*)*current;
			list->setSelected(idx);
			SEvent changeEvent;
			changeEvent.EventType = EET_GUI_EVENT;
			changeEvent.GUIEvent.Caller = list;
			changeEvent.GUIEvent.Element = 0;
			changeEvent.GUIEvent.EventType = EGET_LISTBOX_CHANGED;
			combo->OnEvent(changeEvent);
		}
	}
}

/*
 * Class:     cn_garymb_ygomobile_core_IrrlichtBridge
 * Method:    nativeSetCheckBoxesSelection
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_cn_garymb_ygomobile_core_IrrlichtBridge_nativeSetCheckBoxesSelection(JNIEnv* env, jclass clazz,
		jint handle, jint idx) {
	if (handle) {
		IrrlichtDevice* device = (IrrlichtDevice*) handle;
		IGUIEnvironment* irrenv = device->getGUIEnvironment();
		IGUIElement* element = irrenv->getFocus();
		if (element) {
			IGUIWindow* window = (IGUIWindow*)(element);
			core::list<IGUIElement*> children = window->getChildren();
			core::list<IGUIElement*>::Iterator current = children.begin();
			int i = 0;
			do {
				if ((*current)->getType() == EGUIET_CHECK_BOX
						&& i++ == idx) {
					break;
				}
				current++;
			}while(current != children.end());
			if (current == children.end()) {
				return;
			}
			IGUICheckBox* checkbox = (IGUICheckBox*)*current;
			checkbox->setChecked(true);
			SEvent e;
			e.EventType = EET_GUI_EVENT;
			e.GUIEvent.Caller = checkbox;
			e.GUIEvent.Element = 0;
			e.GUIEvent.EventType = EGET_CHECKBOX_CHANGED;
			window->OnEvent(e);
			irrenv->setFocus(window);
		}
	}
}

static void* cancel_chain_thread(void* param) {
	IrrlichtDevice* device = (IrrlichtDevice*)param;
	irr::os::Printer::log("before send cancel chain");
	SEvent downevent;
	downevent.EventType = EET_MOUSE_INPUT_EVENT;
	//just cause a right up event to refresh texture
	downevent.MouseInput.Event = EMIE_RMOUSE_PRESSED_DOWN;
	device->postEventFromUser(downevent);
	usleep(20 * 1000);
	SEvent upevent;
	upevent.EventType = EET_MOUSE_INPUT_EVENT;
	//just cause a right up event to refresh texture
	upevent.MouseInput.Event = EMIE_RMOUSE_LEFT_UP;
	device->postEventFromUser(upevent);
}

/*
 * Class:     cn_garymb_ygomobile_core_IrrlichtBridge
 * Method:    nativeRefreshTexture
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_cn_garymb_ygomobile_core_IrrlichtBridge_nativeRefreshTexture(JNIEnv* env, jclass clazz,
		jint handle) {
	if (handle) {
		IrrlichtDevice* device = (IrrlichtDevice*)handle;

		if (device->isWindowFocused()) {
			irr::os::Printer::log("before send refresh event");
			SEvent event;
			event.EventType = EET_KEY_INPUT_EVENT;
			//just cause a right up event to refresh texture
			event.KeyInput.PressedDown = false;
			event.KeyInput.Shift = false;
			event.KeyInput.Control = false;
			event.KeyInput.Key = KEY_KEY_R;
			device->postEventFromUser(event);
		}
	}
}

/*
 * Class:     cn_garymb_ygomobile_core_IrrlichtBridge
 * Method:    nativeIgnoreChain
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_cn_garymb_ygomobile_core_IrrlichtBridge_nativeIgnoreChain(JNIEnv* env, jclass clazz,
		jint handle, jboolean begin) {
	if (handle) {
		IrrlichtDevice* device = (IrrlichtDevice*)handle;
		if (device->isWindowFocused()) {
			irr::os::Printer::log("before send ignore chain");
			SEvent event;
			event.EventType = EET_KEY_INPUT_EVENT;
			//just cause a right up event to refresh texture
			event.KeyInput.PressedDown = begin;
			event.KeyInput.Shift = false;
			event.KeyInput.Control = false;
			event.KeyInput.Key = KEY_KEY_S;
			device->postEventFromUser(event);
		}
	}
}

/*
 * Class:     cn_garymb_ygomobile_core_IrrlichtBridge
 * Method:    nativeReactChain
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_cn_garymb_ygomobile_core_IrrlichtBridge_nativeReactChain(JNIEnv* env, jclass clazz,
		jint handle, jboolean begin) {
	if (handle) {
		IrrlichtDevice* device = (IrrlichtDevice*)handle;
		if (device->isWindowFocused()) {
			irr::os::Printer::log("before send react chain");
			SEvent event;
			event.EventType = EET_KEY_INPUT_EVENT;
			//just cause a right up event to refresh texture
			event.KeyInput.PressedDown = begin;
			event.KeyInput.Shift = false;
			event.KeyInput.Control = false;
			event.KeyInput.Key = KEY_KEY_A;
			device->postEventFromUser(event);
		}
	}
}

/*
 * Class:     cn_garymb_ygomobile_core_IrrlichtBridge
 * Method:    nativeCancelChain
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_cn_garymb_ygomobile_core_IrrlichtBridge_nativeCancelChain(JNIEnv* env, jclass clazz,
		jint handle) {
	if (handle) {
		IrrlichtDevice* device = (IrrlichtDevice*) handle;
		if (device->isWindowFocused()) {
			pthread_t cencelThread;
			pthread_attr_t cancelAttr;
			pthread_attr_init(&cancelAttr);
			pthread_create(&cencelThread, &cancelAttr, cancel_chain_thread,
					(void*) device);
			pthread_attr_destroy(&cancelAttr);
			pthread_detach(cencelThread);
		}
	}
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	void *venv;
	if (vm->GetEnv((void**) &venv, JNI_VERSION_1_6) != JNI_OK) {
		return -1;
	}
	nativeCrashHandler_onLoad(vm);
	return JNI_VERSION_1_6;
}

}