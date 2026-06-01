#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#include "Platform.h"
#include "ConfigManager/LocaleConfigManager.h"
#include "cocos2d.h"
#include "Application.h"

#include <mutex>
#include <condition_variable>

namespace kr2ios {
	std::condition_variable MessageBoxCond;
	std::mutex MessageBoxLock;
	int MsgBoxRet = -2;
	std::string InputBoxRetText;
}
using namespace kr2ios;

static int _TVPShowSimpleMessageBoxOnMainThread(const char *pszText, const char *pszTitle, unsigned int nButton, const char **btnText) {
	MsgBoxRet = -2;

	NSString *title = [NSString stringWithUTF8String:pszTitle ? pszTitle : ""];
	NSString *message = [NSString stringWithUTF8String:pszText ? pszText : ""];

	UIAlertController *alert = [UIAlertController alertControllerWithTitle:title
																	message:message
														 preferredStyle:UIAlertControllerStyleAlert];

	for (unsigned int i = 0; i < nButton; ++i) {
		NSString *btnTitle = [NSString stringWithUTF8String:btnText[i]];
		[alert addAction:[UIAlertAction actionWithTitle:btnTitle
													style:UIAlertActionStyleDefault
												  handler:^(UIAlertAction *action) {
			MsgBoxRet = i;
			MessageBoxCond.notify_all();
		}]];
	}

	UIViewController *rootViewController = [[[[UIApplication sharedApplication] delegate] window] rootViewController];
	if (rootViewController.presentedViewController) {
		[rootViewController.presentedViewController presentViewController:alert animated:YES completion:nil];
	} else {
		[rootViewController presentViewController:alert animated:YES completion:nil];
	}

	std::unique_lock<std::mutex> lk(MessageBoxLock);
	while (MsgBoxRet == -2) {
		MessageBoxCond.wait_for(lk, std::chrono::milliseconds(200));
	}

	return MsgBoxRet;
}

extern "C" int TVPShowSimpleMessageBox(const char *pszText, const char *pszTitle, unsigned int nButton, const char **btnText) {
	if (!cocos2d::Director::getInstance()) {
		return -1;
	}

	if (TVPMainThreadID == std::this_thread::get_id()) {
		return _TVPShowSimpleMessageBoxOnMainThread(pszText, pszTitle, nButton, btnText);
	}

	std::mutex mtx;
	std::condition_variable cond;
	std::unique_lock<std::mutex> lk(mtx);
	int result = -1;

	cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, pszText, pszTitle, nButton, btnText]() {
		result = _TVPShowSimpleMessageBoxOnMainThread(pszText, pszTitle, nButton, btnText);
		cond.notify_all();
	});

	cond.wait(lk);
	return result;
}

int TVPShowSimpleMessageBox(const ttstr & text, const ttstr & caption, const std::vector<ttstr> &vecButtons) {
	tTJSNarrowStringHolder pszText(text.c_str());
	tTJSNarrowStringHolder pszTitle(caption.c_str());
	std::vector<const char *> btnText; btnText.reserve(vecButtons.size());
	std::vector<std::string> btnTextHold; btnTextHold.reserve(vecButtons.size());
	for (const ttstr &btn : vecButtons) {
		btnTextHold.emplace_back(btn.AsStdString());
		btnText.emplace_back(btnTextHold.back().c_str());
	}
	return TVPShowSimpleMessageBox(pszText, pszTitle, btnText.size(), &btnText[0]);
}