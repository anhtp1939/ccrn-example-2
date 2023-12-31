/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __PLAYER_MENU_SERVICE_MAC_H_
#define __PLAYER_MENU_SERVICE_MAC_H_

#include <unordered_map>

#include "PlayerMacros.h"
#include "PlayerMenuServiceProtocol.h"
#include "cocos/base/RefVector.h"
#import <AppKit/AppKit.h>

//
// Menu item Helper
//

PLAYER_NS_BEGIN
class PlayerMenuItemMac;
PLAYER_NS_END

@interface NNMenuItem : NSMenuItem <NSMenuDelegate> {
    int scriptHandler;
    player::PlayerMenuItemMac *macMenuItem;
}
@property (nonatomic) int scriptHandler;
@property (nonatomic) const player::PlayerMenuItemMac *macMenuItem;

+ (id)createMenuItem:(const player::PlayerMenuItemMac *)macMenuItem;
@end

//
// PlayerMenuItemMac
//

PLAYER_NS_BEGIN

class PlayerMenuItemMac : public PlayerMenuItem {
public:
    static PlayerMenuItemMac *create(const std::string &menuId, const std::string &title);
    virtual ~PlayerMenuItemMac();

    virtual void setTitle(const std::string &title);
    virtual void setEnabled(bool enabled);
    virtual void setChecked(bool checked);
    virtual void setShortcut(const std::string &shortcut);

protected:
    PlayerMenuItemMac();

    PlayerMenuItemMac *_parent;
    NNMenuItem *_menuItem;
    NSMenu *_menu;
    cc::RefVector<PlayerMenuItemMac *> _children;

    friend class PlayerMenuServiceMac;
};

class PlayerMenuServiceMac : public PlayerMenuServiceProtocol {
public:
    PlayerMenuServiceMac();
    virtual ~PlayerMenuServiceMac();

    virtual PlayerMenuItem *addItem(const std::string &menuId, const std::string &title, const std::string &parentId, int order = MAX_ORDER);
    virtual PlayerMenuItem *addItem(const std::string &menuId, const std::string &title);
    virtual PlayerMenuItem *getItem(const std::string &menuId);
    virtual bool removeItem(const std::string &menuId);
    virtual void setMenuBarEnabled(bool enabled);

private:
    bool removeItemInternal(const std::string &menuId, bool isUpdateChildrenOrder);
    void updateChildrenOrder(PlayerMenuItemMac *parent);

private:
    PlayerMenuItemMac _root;
    std::unordered_map<std::string, PlayerMenuItemMac *> _items;
};

PLAYER_NS_END

#endif // __PLAYER_MENU_SERVICE_MAC_H_
