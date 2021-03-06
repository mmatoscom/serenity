#include <LibGUI/GMenuBar.h>
#include <LibGUI/GEventLoop.h>

GMenuBar::GMenuBar()
{
}

GMenuBar::~GMenuBar()
{
    unrealize_menubar();
}

void GMenuBar::add_menu(OwnPtr<GMenu>&& menu)
{
    m_menus.append(move(menu));
}

int GMenuBar::realize_menubar()
{
    WSAPI_ClientMessage request;
    request.type = WSAPI_ClientMessage::Type::CreateMenubar;
    WSAPI_ServerMessage response = GEventLoop::current().sync_request(request, WSAPI_ServerMessage::Type::DidCreateMenubar);
    return response.menu.menubar_id;
}

void GMenuBar::unrealize_menubar()
{
    if (!m_menubar_id)
        return;
    WSAPI_ClientMessage request;
    request.type = WSAPI_ClientMessage::Type::DestroyMenubar;
    request.menu.menubar_id = m_menubar_id;
    GEventLoop::current().sync_request(request, WSAPI_ServerMessage::Type::DidDestroyMenubar);
    m_menubar_id = 0;
}

void GMenuBar::notify_added_to_application(Badge<GApplication>)
{
    ASSERT(!m_menubar_id);
    m_menubar_id = realize_menubar();
    ASSERT(m_menubar_id > 0);
    for (auto& menu : m_menus) {
        ASSERT(menu);
        int menu_id = menu->realize_menu();
        ASSERT(menu_id > 0);
        WSAPI_ClientMessage request;
        request.type = WSAPI_ClientMessage::Type::AddMenuToMenubar;
        request.menu.menubar_id = m_menubar_id;
        request.menu.menu_id = menu_id;
        GEventLoop::current().sync_request(request, WSAPI_ServerMessage::Type::DidAddMenuToMenubar);
    }
    WSAPI_ClientMessage request;
    request.type = WSAPI_ClientMessage::Type::SetApplicationMenubar;
    request.menu.menubar_id = m_menubar_id;
    GEventLoop::current().sync_request(request, WSAPI_ServerMessage::Type::DidSetApplicationMenubar);
}

void GMenuBar::notify_removed_from_application(Badge<GApplication>)
{
    unrealize_menubar();
}
